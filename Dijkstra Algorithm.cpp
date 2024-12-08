#include <iostream>
#include <queue>
#include <iomanip>

using namespace std;

// CLASS DEFINITION FOR ORDER
class Order
{
public:
    string name;
    int location;
    int deliveryTimeLimit;
};

// CLASS DEFINITION FOR RESTAURANT
class Restaurant
{
public:
    string name;
    int location;
    Order* orders;
    int numOrders;

    // Constructor
    Restaurant()
    {
        orders = nullptr;
        numOrders = 0;
    }

    // Destructor
    ~Restaurant()
    {
        delete[] orders;
    }
};

// CLASS DEFINITION FOR NODE
class Node
{
public:
    int id;
    int distance;
};

// FUNCTION TO INITIALIZE NODE
Node initNode(int id, int distance)
{
    Node node;
    node.id = id;
    node.distance = distance;
    return node;
}

// CLASS DEFINITION FOR EDGE
class Edge
{
public:
    int to;
    int weight;
};

// FUNCTION TO INITIALIZE EDGE
Edge initEdge(int to, int weight)
{
    Edge edge;
    edge.to = to;
    edge.weight = weight;
    return edge;
}

// FUNCTION TO ADD EDGE TO GRAPH
void addEdge(vector<vector<Edge>>& graph, int from, int to, int weight)
{
    graph[from].push_back(initEdge(to, weight));
    graph[to].push_back(initEdge(from, weight));
}

// FUNCTION TO CREATE GRAPH
void createGraph(int N, const Order* orders, int numOrders, const int* restaurantLocations, int numRestaurants, vector<vector<Edge>>& graph)
{
    graph.resize(N * N);

    // ADDING EDGES FOR EACH CELL IN THE GRID
    for (int i = 0; i < N * N; ++i)
    {
        int row = i / N;
        int col = i % N;

        if (row > 0)
            addEdge(graph, i, i - N, 1);
        if (row < N - 1)
            addEdge(graph, i, i + N, 1);
        if (col > 0)
            addEdge(graph, i, i - 1, 1);
        if (col < N - 1)
            addEdge(graph, i, i + 1, 1);
    }

    // ADDING EDGES FROM RESTAURANTS TO CUSTOMER LOCATIONS
    for (int i = 0; i < numRestaurants; ++i)
    {
        int restaurant = restaurantLocations[i];
        for (int j = 0; j < numOrders; ++j)
        {
            int customerLocation = orders[j].location;
            int distance = abs(restaurant - customerLocation);
            addEdge(graph, restaurant - 1, customerLocation - 1, distance);
        }
    }
}

// FUNCTION TO PERFORM DIJKSTRA'S ALGORITHM
void dijkstra(const vector<vector<Edge>>& graph, vector<Node>& nodes, int start)
{
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

    nodes[start].distance = 0;
    pq.push({ 0, start });

    while (!pq.empty())
    {
        int u = pq.top().second;
        pq.pop();

        for (const Edge& edge : graph[u])
        {
            int v = edge.to;
            int weight = edge.weight;

            if (nodes[v].distance > nodes[u].distance + weight)
            {
                nodes[v].distance = nodes[u].distance + weight;
                pq.push({ nodes[v].distance, v });
            }
        }
    }
}

// FUNCTION TO DISPLAY THE GRID WITH RESTAURANTS AND ORDERS
void displayGraph(int N, const int* restaurantLocations, int numRestaurants, const int* orderLocations, int numOrders)
{
    char** grid = new char* [N];
    for (int i = 0; i < N; ++i)
    {
        grid[i] = new char[N];
        fill_n(grid[i], N, '-');
    }

    // MARK RESTAURANT LOCATIONS
    for (int i = 0; i < numRestaurants; ++i)
    {
        int location = restaurantLocations[i];
        int row = (location - 1) / N;
        int col = (location - 1) % N;
        grid[row][col] = 'R';
    }

    // MARK ORDER LOCATIONS
    for (int i = 0; i < numOrders; ++i)
    {
        int location = orderLocations[i];
        int row = (location - 1) / N;
        int col = (location - 1) % N;
        if (grid[row][col] == 'R')
        {
            // IF RESTAURANT ALREADY EXISTS AT THE LOCATION, PRIORITIZE ORDER
            grid[row][col] = 'O';
        }
        else
        {
            grid[row][col] = '0';
        }
    }

    // PRINT THE GRID
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            cout << setw(3) << grid[i][j] << " ";
        }
        cout << endl;
    }

    // CLEAN UP DYNAMICALLY ALLOCATED MEMORY
    for (int i = 0; i < N; ++i)
    {
        delete[] grid[i];
    }
    delete[] grid;
}

// FUNCTION TO CALCULATE DELIVERY TIME FOR A RIDER
int calculateDeliveryTimeForRider(int riderLocation, const Order* orders, int numOrders, const vector<vector<Edge>>& graph)
{
    vector<Node> nodes(graph.size(), initNode(-1, numeric_limits<int>::max()));
    for (int j = 0; j < graph.size(); ++j)
        nodes[j].id = j + 1;

    dijkstra(graph, nodes, riderLocation - 1);

    int maxDeliveryTime = 0;
    for (int i = 0; i < numOrders; ++i)
    {
        int distance = nodes[orders[i].location - 1].distance;
        int deliveryTimeLimit = orders[i].deliveryTimeLimit;
        maxDeliveryTime = max(maxDeliveryTime, min(distance, deliveryTimeLimit));
    }
    return maxDeliveryTime;
}

// FUNCTION TO CALCULATE TOTAL TIME FOR ALL RIDERS
int calculateTotalTime(const int* restaurantLocations, int numRestaurants, const Order* orders, int numOrders, int N, int I)
{
    vector<vector<Edge>> graph;
    createGraph(N, orders, numOrders, restaurantLocations, numRestaurants, graph);

    if (numRestaurants == 1)
    {
        int maxDeliveryTime = 0;
        for (int i = 0; i < numOrders; ++i)
        {
            maxDeliveryTime = max(maxDeliveryTime, calculateDeliveryTimeForRider(restaurantLocations[0], &orders[i], 1, graph));
        }
        return maxDeliveryTime * 2; // NO NEED TO ADD 2 FOR THE RETURN TRIP
    }

    if (I >= numRestaurants)
    {
        int totalDeliveryTime = 0;
        for (int i = 0; i < numRestaurants; ++i)
        {
            totalDeliveryTime += calculateDeliveryTimeForRider(restaurantLocations[i], orders, numOrders, graph);
        }
        return totalDeliveryTime;
    }

    int** riderDeliveryTimes = new int* [numRestaurants];
    for (int i = 0; i < numRestaurants; ++i)
    {
        riderDeliveryTimes[i] = new int[numOrders];
        riderDeliveryTimes[i][0] = calculateDeliveryTimeForRider(restaurantLocations[i], orders, numOrders, graph);
    }

    for (int i = 0; i < numRestaurants; ++i)
    {
        sort(riderDeliveryTimes[i], riderDeliveryTimes[i] + numOrders, greater<int>());
    }

    int maxTotalDeliveryTime = 0;
    for (int i = 0; i < I; ++i)
    {
        int maxDeliveryTime = 0;
        for (int j = 0; j < numRestaurants; ++j)
        {
            maxDeliveryTime = max(maxDeliveryTime, riderDeliveryTimes[j][i]);
        }
        maxTotalDeliveryTime += maxDeliveryTime;
    }

    // CLEAN UP DYNAMICALLY ALLOCATED MEMORY
    for (int i = 0; i < numRestaurants; ++i)
    {
        delete[] riderDeliveryTimes[i];
    }
    delete[] riderDeliveryTimes;

    return maxTotalDeliveryTime;
}

// FUNCTION TO GET ORDER LOCATIONS FROM ORDERS ARRAY
int* getOrderLocations(const Order* orders, int numOrders)
{
    int* orderLocations = new int[numOrders];
    for (int i = 0; i < numOrders; ++i)
    {
        orderLocations[i] = orders[i].location;
    }
    return orderLocations;
}

// MAIN FUNCTION
int main()
{
    int T;
    cout << "Enter the number of test cases: ";
    cin >> T;

    for (int t = 1; t <= T; ++t)
    {
        int N, I, R;
        cout << "Test Case " << t << ":" << endl;
        cout << "Enter grid size (N x N), number of riders (I), and number of restaurants (R): ";
        cin >> N >> I >> R;

        int* restaurantLocations = new int[R];
        Order* orders = new Order[R * N];
        int numOrders = 0;
        cout << "Enter details of restaurants and orders:" << endl;

        for (int i = 0; i < R; ++i)
        {
            cout << "Restaurant " << i + 1 << ":" << endl;
            string name;
            int location, O;
            cin >> name >> location >> O;
            restaurantLocations[i] = location;
            for (int j = 0; j < O; ++j)
            {
                string orderName;
                int orderLocation, deliveryTimeLimit;
                cin >> orderName >> orderLocation >> deliveryTimeLimit;
                orders[numOrders++] = { orderName, orderLocation, deliveryTimeLimit };
            }
        }

        // CALCULATE TOTAL TIME
        int totalTime = calculateTotalTime(restaurantLocations, R, orders, numOrders, N, I);
        cout << "Total minimum time required: " << totalTime << " time units" << endl;

        // DISPLAY THE GRID
        cout << "Grid with Restaurants and Orders:" << endl;
        displayGraph(N, restaurantLocations, R, getOrderLocations(orders, numOrders), numOrders);
        cout << endl;

        // CLEAN UP DYNAMICALLY ALLOCATED MEMORY
        delete[] restaurantLocations;
        delete[] orders;
    }
    return 0;
}
