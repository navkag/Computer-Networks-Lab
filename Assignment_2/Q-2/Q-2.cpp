#include <iostream>
#include <string>
#include <unordered_map>
#include <list>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>
#include <bits/stdc++.h>
using namespace std;

// Define the maximum number of pages the cache can store
const int MAX_CACHE_SIZE = 3;
int err=0;

// Define a struct to represent a web page
struct WebPage {
    string url;
    string content;
};

// Define the Cache class
class WebCache {
private:
    list<WebPage> cacheList;                                // Doubly-linked list to maintain LRU order
    unordered_map<string, list<WebPage>::iterator> cacheMap; // Map for quick access to cache entries: stores the list iterator for the url.
    int cacheSize;

public:
    WebCache() : cacheSize(0) {}

    // Function to retrieve a web page from the cache
    string getWebPage(const string& url) {
        if (cacheMap.find(url) != cacheMap.end()) {
            // Page found in cache, move it to the front (most recently used)
            cacheList.splice(cacheList.begin(), cacheList, cacheMap[url]);    // Move contents at cacheMap[url] from cacheList to cacheList.begin().
            return cacheMap[url]->content;
        } else {
            // Page not found in cache, fetch it using HTTP GET
            string content = fetchWebPage(url);



            if (!err && content.length() != 0) {
                // Check if cache is full
                if (cacheSize >= MAX_CACHE_SIZE) {
                    // Evict the least recently used page
                    removeLRUPage();
                }

                // Add the new page to the cache
                WebPage newPage = {url, content};
                cacheList.push_front(newPage);
                cacheMap[url] = cacheList.begin();
                cacheSize++;
            }
            err=0;
            return content;
        }
    }

    // Function to fetch a web page using HTTP GET
    string fetchWebPage(const string& url) {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            cerr << "Failed to create socket" << endl;
            err=1;
            return ""; // Error handling: Return an empty string or handle the error appropriately.
        }

        // Parse the URL to extract the host and path
        size_t host_start = url.find("://");
        if (host_start == string::npos) {
            cerr << "Invalid URL format: " << url << endl;
            close(sockfd);
            err=1;
            return ""; // Error handling: Return an empty string or handle the error appropriately.
        }

        host_start += 3; // Move past "://"
        size_t host_end = url.find('/', host_start);
        string host = url.substr(host_start, host_end - host_start);
        string path = (host_end != string::npos) ? url.substr(host_end) : "/";

        // Get host information: If host string is empty return error.
        struct hostent* server = gethostbyname(host.c_str());       // 
        if (!server) {
            cerr << "Failed to resolve host: " << host << endl;
            close(sockfd);
            err=1;
            return ""; // Error handling: Return an empty string or handle the error appropriately.
        }

        // Define the server address structure
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(80);
        memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

        // Connect to the server
        if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
            cerr << "Failed to connect to server" << endl;
            close(sockfd);
            err=1;
            return ""; // Error handling: Return an empty string or handle the error appropriately.
        }

        // Send an HTTP GET request
        string http_request = "GET " + path + " HTTP/1.1\r\n";
        http_request += "Host: " + host + "\r\n";
        http_request += "Connection: close\r\n\r\n";   // Non persistent

        if (send(sockfd, http_request.c_str(), http_request.length(), 0) == -1) {
            cerr << "Failed to send HTTP request" << endl;
            close(sockfd);
            err=1;
            return ""; // Error handling: Return an empty string or handle the error appropriately.
        }

        // Receive and parse the HTTP response
        string http_response;
        char buffer[1024];
        int bytes_received;

        while ((bytes_received = recv(sockfd, buffer, sizeof(buffer), 0)) > 0) {
            http_response.append(buffer, bytes_received);
        }

        if (bytes_received == -1) {
            cerr << "Failed to receive HTTP response" << endl;
            close(sockfd);
            err=1;
            return ""; // Error handling: Return an empty string or handle the error appropriately.
        }

        // Close the socket
        close(sockfd);

        // Find the start of the content: The first time \r\n\r\n occurs is at the beginning of the content.
        size_t content_start = http_response.find("\r\n\r\n");
        if (content_start != string::npos) {
            content_start += 4; // Move past the empty line
            size_t status_code_start = http_response.find("HTTP/1.1 ") + 9;
            size_t status_code_end = http_response.find(" ", status_code_start);
            string status_code = http_response.substr(status_code_start, status_code_end - status_code_start);
            if (status_code == "200") {
                return http_response.substr(content_start);
            } else {
                err = 1;
                cerr << "HTTP status code is not 200 OK: " << status_code << endl;
                return "";
            }
        }
        else {
            cerr << "Failed to parse HTTP response" << endl;
            err=1;
            return ""; // Error handling: Return an empty string or handle the error appropriately.
        }
    }

    // Function to remove the least recently used page from the cache
    void removeLRUPage() {
        if (!cacheList.empty()) {
            string lruUrl = cacheList.back().url;
            cacheMap.erase(lruUrl);
            cacheList.pop_back();
            cacheSize--;
        }
    }

    // Function to display the contents of the cache
    void displayCache() {
        int i = 1;
        for (const WebPage& page : cacheList) {
            cout << i << "." << page.url << endl;
            i++;
        }
    }
};

int main() {
    WebCache cache;

    cout << "Cache size: " << MAX_CACHE_SIZE << endl;

    while (true){
        string url;
        cout << "Enter URL (or 'exit' to quit): ";
        cin >> url;

        if (url == "exit"){
            break;
        }
        
        clock_t checkpoint1 = clock();

        cout << "\033[32mPage content: \033[0m\n" << cache.getWebPage(url) << endl;

        clock_t checkpoint2 = clock();
        cout << "\033[31mTime taken for page to load: \033[0m" << (checkpoint2 - checkpoint1)  << " clock cycles." << endl;

        cout << endl;
        cout << "\033[33mCache contents: \033[0m" << endl;
        cache.displayCache();
        cout << endl;
    }

    cout << "Successfully exited!\n";
    return 0;
}
