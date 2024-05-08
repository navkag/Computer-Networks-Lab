#include "bits/stdc++.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <random>
#include <cmath>
using namespace std;

int num_websites;
struct Website
{
    int website_id;
    string owner_id;
    int bandwidth;
    int processing_power;
    vector<pair<int, int> > request_queue;
};

struct HttpRequest
{
    int request_id;
    int website_id;
    int processing_time;
};

class LoadBalancer
{
public:
    LoadBalancer()
    {
        global_time = 0;
        // next_free_time = 0;
    }

    void add_website(int website_id, const string &owner_id, int bandwidth, int processing_power)
    {
        Website *website = new Website();
        website->website_id = website_id;
        website->owner_id = owner_id;
        website->bandwidth = bandwidth;
        website->processing_power = processing_power;
        websites[website_id] = website;
    }

    void enqueue_request(HttpRequest *http_request, int time)
    {
        Website *website = websites[http_request->website_id];
        requests[http_request->request_id] = http_request;
        website->request_queue.push_back(make_pair(http_request->request_id, time));
    }

    void dequeueRequest(int power_divisor)
    {
        vector<pair<double, int> > weights;
        for (auto &website : websites)
        {
            double w = website.second->bandwidth + website.second->processing_power;
            int id = website.first;
            weights.push_back({w, id});
        }
        unordered_map<int, double> band;
        for (auto &website : websites)
        {
            double w = website.second->bandwidth + website.second->processing_power;
            int id = website.first;
            band[id] = w / power_divisor;
            // cout << band[id] << endl;
        }
        vector<pair<int, int> > virtual_time;
        unordered_map<int, double> processing_times_temp;
        unordered_map<int, double> real_processing_times;
        unordered_map<int, double> arrival_times;
        for (auto &website : websites)
        {
            vector<pair<int, int> > reqs = website.second->request_queue;
            int id = website.first;
            double t = 0;
            for (int i = 0; i < reqs.size(); i++)
            {
                t = max(t, (double)reqs[i].second) + (double)requests[reqs[i].first]->processing_time / band[id];
                processing_times_temp[reqs[i].first] = t;
                real_processing_times[reqs[i].first] = t;
            }
            // cout << endl;
        }

        vector<pair<double, int> > processing_times;
        for (auto i : processing_times_temp)
        {
            int f = i.first;
            double s = i.second;
            processing_times.push_back({s, f});
        }
        sort(processing_times.begin(), processing_times.end());
        for (auto i : processing_times)
        {
            int id = i.second;
            cout << "Http_Request with request_ID #" << id << " is processed at time " << real_processing_times[id] << '\n';
        }
    }

private:
    int global_time;
    int next_free_time;
    map<int, Website *> websites;
    map<int, HttpRequest *> requests;
};

int main()
{
    LoadBalancer *lb = new LoadBalancer();
    int num_requests = 0;
    num_websites = 0;
    // int max_time = 0;
    cout << "Enter number of websites: ";
    cin >> num_websites;
    cout << "Enter total number of requests: ";
    cin >> num_requests;
    for (int i = 0; i < num_websites; i++)
    {
        cout << "Entry for website " << i + 1 << ":\n";
        int website_id = 0;
        string owner_id = "";
        int bandwidth = 0;
        int processing_power = 0;
        cout << "Enter website ID: ";
        cin >> website_id;
        cout << "Enter owner ID: ";
        cin >> owner_id;
        cout << "Enter bandwidth: ";
        cin >> bandwidth;
        cout << "Enter processing power: ";
        cin >> processing_power;
        cout << endl;
        lb->add_website(website_id, owner_id, bandwidth, processing_power);
    }
    cout << endl;
    for (int i = 0; i < num_requests; i++)
    {
        cout << "Entry for request " << i + 1 << ":\n";
        HttpRequest *http_req = new HttpRequest();
        int req_id;
        cout << "Enter request ID: ";
        cin >> req_id;
        int web_id;
        cout << "Enter website ID: ";
        cin >> web_id;
        int arrival_time;
        cout << "Enter arrival time: ";
        cin >> arrival_time;
        int process_time;
        cout << "Enter process time: ";
        cin >> process_time;
        http_req->website_id = web_id;
        http_req->request_id = req_id;
        http_req->processing_time = process_time;
        cout << endl;
        lb->enqueue_request(http_req, arrival_time);
        // max_time += process_time;
    }
    int power_divisor;
    // cout << "Enter the total power of the server : ";
    cout << "Enter power divisor: ";
    cin >> power_divisor;
    cout << "Processing the requests...\n";
    lb->dequeueRequest(power_divisor);

    return 0;
}