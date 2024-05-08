#include <bits/stdc++.h>
#include <random>
using namespace std;

#define INFINTY 100000

double generateExponentialTime(double lambda)
{
    random_device rd;
    mt19937 gen(rd());
    exponential_distribution<double> exponential(lambda);
    return exponential(gen);
}

int main()
{
    int num_of_servers, size_of_buffer;

    double arrival_rate, service_rate;
    cout << "Enter the arrival rate: ";
    cin >> arrival_rate;
    cout << "Enter the service rate: ";
    cin >> service_rate;

    double total_time;
    cout << "Enter the end arrival time: ";
    cin >> total_time;

    cout << endl;
    cout << "Passengers are going to arrive at time = :-" << endl;
    cout << "0" << ", ";
    vector<double> arrival_times;
    arrival_times.push_back(0);
    
    double current_time = 0;
    while (1)
    {
        double x = generateExponentialTime(arrival_rate);
        if (current_time + x <= total_time)
        {
            current_time += x;
            arrival_times.push_back(current_time);
            cout << current_time << ", ";
        }
        else
        {
            break;
        }
    }
    map<double,int>getPassenger;
    for(int i=0;i<arrival_times.size();i++)
    {
        getPassenger[arrival_times[i]]=i;
    }

    cout << endl;

    for(int i=1;i<=4;i++)
    {
        vector<double>waiting(arrival_times.size(),-1);
        
        if(i==1)
        {
            cout<<"Single server with infinite queue"<<endl;
            num_of_servers=1;
            size_of_buffer=INT_MAX;
        }
        if(i==2)
        {
            cout<<"Single server with finite queue"<<endl;
            num_of_servers=1;
            cout << "Enter the size of buffer: ";
            cin >> size_of_buffer;
        }
        if(i==3)
        {
            cout<<"Multiple server with infinite queue"<<endl;
            cout << "Enter the number of security scanners in the system: ";
            cin >> num_of_servers;
            size_of_buffer=INT_MAX;
        }
        if(i==4)
        {
            cout<<"Multiple server with finite queue"<<endl;
            cout << "Enter the number of security scanners in the system: ";
            cin >> num_of_servers;
            cout << "Enter the size of buffer: ";
            cin >> size_of_buffer;

        }

        vector<double>completion(num_of_servers,-1);
        vector<double>servicing(num_of_servers,0);
        vector<double>cpu_util(num_of_servers,-1);
        vector<vector<int> >queuelength(num_of_servers,vector<int>(arrival_times.size()));
        
        cout << "Starting the simulation..." << endl;
        sleep(1);
        cout << endl;

        vector<queue<double> > occupied(num_of_servers);
        vector<bool> server_busy(num_of_servers, false);
        vector<double> completion_time(num_of_servers, double(-1));

        int index = 0;

        while (index < arrival_times.size())
        {
            for (int i = 0; i < num_of_servers; i++)
            {
                while (completion_time[i] != -1 && completion_time[i] <= arrival_times[index])
                {
                    if (occupied[i].size() == 0)
                    {
                        cout << "Time " << completion_time[i] << ": Server " << i << " has completed servicing a passenger." << endl;
                        completion[i]=completion_time[i];
                        completion_time[i] = -1;
                        server_busy[i] = false;
                    }
                    else
                    {
                        cout << "Time " << completion_time[i] << ": Server " << i << " has completed servicing a passenger and now has started servicing a passenger who had";
                        cout << " arrived at time = " << occupied[i].front() << " and the passenger's servicing will be completed at time = ";
                        waiting[getPassenger[occupied[i].front()]]=completion_time[i]-occupied[i].front();
                        double y=generateExponentialTime(service_rate);
                        completion_time[i] += y;
                        servicing[i]+=y;
                        cout << completion_time[i] << "." << endl;
                        occupied[i].pop();
                    }
                }
            }

            vector<int> free_servers;
            for (int i = 0; i < num_of_servers; i++)
            {
                if (server_busy[i] == false)
                {
                    free_servers.push_back(i);
                }
            }

            

            if (free_servers.size() > 0)
            {
                int x = rand() % (free_servers.size());
                double y=generateExponentialTime(service_rate);
                completion_time[free_servers[x]] = arrival_times[index] + y;
                servicing[free_servers[x]]+=y;
                server_busy[free_servers[x]] = true;
                waiting[index]=0;
                cout << "Time " << arrival_times[index] << ": Server " << free_servers[x] << " has started servicing the passenger who just arrived. Servicing will be completed at time ";
                cout << completion_time[free_servers[x]] << "." << endl;
            }
            else
            {
                int ind = -1;
                int least = INT_MAX;
                for (int i = 0; i < num_of_servers; i++)
                {
                    if (occupied[i].size() < size_of_buffer)
                    {
                        if (occupied[i].size() < least)
                        {
                            least = occupied[i].size();
                            ind = i;
                        }
                    }
                }
                if (ind == -1)
                {
                    cout << "Time " << arrival_times[index] << ": All the buffers are filled and all the servers are busy. Thus the passenger who just arrived is being dropped." << endl;
                }
                else
                {
                    occupied[ind].push(arrival_times[index]);
                    cout << "Time " << arrival_times[index] << ": The passenger has joined the queue of the server " << ind << "." << endl;
                }
            }
            for(int i=0;i<num_of_servers;i++)
            queuelength[i][index]=occupied[i].size();
            index++;
        }

        cout << endl;
        cout << "Arrivals done!!!" << endl;
        cout << endl;

        for (int i = 0; i < num_of_servers; i++)
        {
            while (completion_time[i] != -1)
            {
                if (occupied[i].size() == 0)
                {
                    cout << "Time " << completion_time[i] << ": Server " << i << " has completed servicing a passenger." << endl;
                    completion[i]=completion_time[i];
                    completion_time[i] = -1;
                    server_busy[i] = false;
                }
                else
                {
                    double x = occupied[i].front();
                    occupied[i].pop();
                    cout << "Time " << completion_time[i] << ": Server " << i << " has completed servicing a passenger and now has started servicing a passenger who had arrived ";
                    waiting[getPassenger[x]]=completion_time[i]-x;
                    double y=generateExponentialTime(service_rate);
                    completion_time[i] += y;
                    servicing[i]+=y;
                    cout << "at time = " << x << " and the servicing of the passenger will be completed at " << completion_time[i] << endl;
                }
            }
        }

        double twait=0;
        int n=0;
        cout<<endl<<"Average waiting time"<<endl;
        for(auto i : waiting)
        {
            //cout<<i<<" ";
            if(i!=-1)
            {
                twait+=i;
                n++;
            }
        }
        double avgwait=twait/n;
        cout<<avgwait<<endl;
        
        for(int i=0;i<num_of_servers;i++)
        {
            if(completion[i]!=-1)
            {
                cpu_util[i]=servicing[i]/completion[i];
            }
        }
        cout<<endl<<"System utilisation"<<endl;
        for(int i=0;i<num_of_servers;i++)
        {
            cout<<cpu_util[i]<<" ";
        }
        cout<<endl;
        vector<double>avg_queue_len(num_of_servers,0);
        for(int i=0;i<num_of_servers;i++)
        {
            for(int j=0;j<arrival_times.size();j++)
            {
                avg_queue_len[i]+=queuelength[i][j];
            }
            avg_queue_len[i]/=arrival_times.size();
        }
        cout<<endl<<"Average queue length"<<endl;
        for(auto i:avg_queue_len)
        {
            cout<<i<<" ";
        }

        cout << endl;
        cout << "Simulation done!!!" << endl;
        cout << endl;
    }
}