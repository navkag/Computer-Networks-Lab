#include <bits/stdc++.h>
using namespace std;
#define INF 1e9

class Router
{
public:
    int router_id;
    vector<pair<Router *, int>> neighbours;
    vector<int> Routing_Table_distances;
    vector<int> Routing_Table;

    Router(int id, int n) // Initialize Routing Table
    {
        this->router_id = id;
        Routing_Table_distances.resize(n + 1, INF);
        Routing_Table.resize(n + 1, -1);
    }

    void add_neighbour(Router *neighbour, int weight)

    {
        neighbours.push_back(make_pair(neighbour, weight));
    }

    void Update_Routing_Table()

    {
        Routing_Table_distances[this->router_id] = 0;
        Routing_Table[this->router_id] = router_id;
        multiset<pair<int, Router *>> pri_queue;

        pri_queue.insert(make_pair(0, this));

        while (pri_queue.size() != 0)

        {
            pair<int, Router *> vertex = *(pri_queue.begin());
            pri_queue.erase(pri_queue.begin());
            int distance = vertex.first;
            Router *router = vertex.second;

            for (auto child : router->neighbours)

            {

                if (Routing_Table_distances[(child.first)->router_id] > distance + child.second)

                {
                    Routing_Table[(child.first)->router_id] = router->router_id;
                    Routing_Table_distances[(child.first)->router_id] = distance + child.second;
                    pri_queue.insert(make_pair(distance + child.second, child.first));
                }
            }
        }
    }

    void Print_Routing_Table()
    {
        int n = Routing_Table.size() - 1;

        for (int i = 1; i <= n; i++)
        {
            if (i == router_id)
                continue;
            int link;
            if (Routing_Table[i] == this->router_id)
            {
                link = i;
            }

            else
            {
                link = i;
                while (Routing_Table[link] != this->router_id)
                {
                    link = Routing_Table[link];
                }
            }

            cout << "Destination Router " << i << " "
                 << "takes outgoing link " << router_id << "," << link << endl;
        }
    }
};

int main()
{

    cout << "Enter the number of nodes and total edges in the system " << endl;

    int n, m;
    cin >> n >> m;

    vector<Router *> Routers(n + 1);

    for (int i = 1; i <= n; i++)
    {
        Routers[i] = new Router(i, n);
    }

    cout << "Enter the edges of the system " << endl;

    for (int i = 0; i < m; i++)
    {
        int node1, node2, weight;
        cin >> node1 >> node2 >> weight;

        Routers[node1]->add_neighbour(Routers[node2], weight);
        Routers[node2]->add_neighbour(Routers[node1], weight);
    }

    for (int i = 1; i <= n; i++)
    {
        Routers[i]->Update_Routing_Table();
    }

    while (1)
    {
        cout << "Do you want to Exit?(press y) " << endl;

        string s;
        cin >> s;

        if (s == "y")
        {
            break;
        }

        cout << "Enter node whose Routing Table you want " << endl;

        int node;
        cin >> node;

        Routers[node]->Print_Routing_Table();
    }

    return 0;
}