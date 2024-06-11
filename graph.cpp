#include <filesystem>
#include <iostream>
#include <map>
#include <vector>
#include <queue>
#include <set>
#include <limits>
#include <algorithm>

// Create a namespace alias for std::filesystem
namespace fs = std::filesystem;

// Define a class to represent the file system as a graph
class FileSystemGraph {
    // Use a map to represent the adjacency list of the graph
    // Each key is a path, and the value is a vector of paths that are directly reachable from the key
    std::map<fs::path, std::vector<fs::path>> adjList;

public:
    // Add a vertex to the graph
    void addVertex(const fs::path& path) {
        // If the path is not already in the adjacency list, add it with an empty vector
        if (!adjList.count(path)) {
            adjList[path] = {};
        }
    }

    // Add an edge to the graph
    void addEdge(const fs::path& source, const fs::path& destination) {
        // Ensure that the source and destination vertices are in the graph
        addVertex(source);
        addVertex(destination);
        // Add an edge from the source to the destination
        adjList[source].push_back(destination);
    }

    // Build the graph by traversing the file system
    void buildGraph(const fs::path& root) {
        // If the root path does not exist or is not a directory, return immediately
        if (!fs::exists(root) || !fs::is_directory(root)) {
            return;
        }

        // Iterate over the entries in the root directory
        for (const auto& entry : fs::directory_iterator(root)) {
            try {
                // Add an edge from the root to the entry
                addEdge(root, entry.path());
                // If the entry is a directory, recursively build the graph for that directory
                if (fs::is_directory(entry.path())) {
                    buildGraph(entry.path());
                }
            } catch (fs::filesystem_error& e) {
                // If a filesystem error occurs (e.g., insufficient permissions), print the error and continue with the next entry
                std::cerr << e.what() << '\n';
                continue;
            }
        }
    }

    // Print the graph
    void printGraph() {
        // Iterate over the vertices in the graph
        for (const auto& pair : adjList) {
            // Print the path for the vertex
            std::cout << pair.first << ":\n";
            // Iterate over the neighbors of the vertex
            for (const auto& neighbor : pair.second) {
                // Print the path for the neighbor
                std::cout << "  " << neighbor << "\n";
            }
        }
    }

    // Find the shortest path between two directories
    std::vector<fs::path> shortestPath(const fs::path& source, const fs::path& destination) {
        std::map<fs::path, fs::path> previous;
        std::map<fs::path, int> distances;
        std::set<fs::path> unvisited;

        for (const auto& pair : adjList) {
            distances[pair.first] = std::numeric_limits<int>::max();
            previous[pair.first] = "";
            unvisited.insert(pair.first);
        }

        distances[source] = 0;

        while (!unvisited.empty()) {
            fs::path current = *unvisited.begin();
            for (const auto& path : unvisited) {
                if (distances[path] < distances[current]) {
                    current = path;
                }
            }

            if (current == destination) {
                break;
            }

            unvisited.erase(current);

            for (const auto& neighbor : adjList[current]) {
                int alt = distances[current] + 1;
                if (alt < distances[neighbor]) {
                    distances[neighbor] = alt;
                    previous[neighbor] = current;
                }
            }
        }

        std::vector<fs::path> path;
        for (fs::path p = destination; p != ""; p = previous[p]) {
            path.push_back(p);
        }
        std::reverse(path.begin(), path.end());

        return path;
    }

    // Find a minimum spanning tree of the file system
    std::map<fs::path, std::vector<fs::path>> minSpanTree() {
        std::map<fs::path, std::vector<fs::path>> tree;
        std::set<fs::path> visited;
        std::priority_queue<std::pair<int, fs::path>, std::vector<std::pair<int, fs::path>>, std::greater<>> pq;

        fs::path start = adjList.begin()->first;
        pq.push({0, start});

        while (!pq.empty()) {
            fs::path current = pq.top().second;
            pq.pop();

            if (visited.count(current)) {
                continue;
            }

            visited.insert(current);

            for (const auto& neighbor : adjList[current]) {
                if (!visited.count(neighbor)) {
                    tree[current].push_back(neighbor);
                    pq.push({1, neighbor});
                }
            }
        }

        return tree;
    }
};

//Doing 1 test at a time is recommended. Comment out the other tests to run a specific test. Changing file paths is also recommended.
int main() {
    // Create a FileSystemGraph object
    FileSystemGraph graph;

    // Test 1: Adding vertices
    graph.addVertex("C:/TestVertex1");
    graph.addVertex("C:/TestVertex2");
    std::cout << "Vertices added successfully.\n";

    // Test 2: Adding edges
    graph.addEdge("C:/TestVertex1", "C:/TestVertex2");
    graph.addEdge("C:/TestVertex2", "C:/TestVertex3");
    std::cout << "Edges added successfully.\n";

    // Build the graph starting from the root directory
    graph.buildGraph("C:/");  // Replace with the path to the root directory
    // Print the graph. This will go through the entire file system and will take some time depending on the size of the file system.
    // graph.printGraph();

    // Demonstrate the shortestPath method
    // Test 3: Find the shortest path between two directories
    fs::path source1 = "C:/Windows";
    fs::path destination1 = "C:/Windows/System32";
    std::vector<fs::path> path1 = graph.shortestPath(source1, destination1);
    std::cout << "Shortest path from " << source1 << " to " << destination1 << ":\n";
    for (const auto& p : path1) {
        std::cout << "  " << p << "\n";
    }
    std::cout << "Length of shortest path: " << path1.size() << "\n";

    // Test 4: Find the shortest path between two different directories
    fs::path source2 = "C:/Windows/System32";
    fs::path destination2 = "C:/Windows/System32/drivers";
    std::vector<fs::path> path2 = graph.shortestPath(source2, destination2);
    std::cout << "Shortest path from " << source2 << " to " << destination2 << ":\n";
    for (const auto& p : path2) {
        std::cout << "  " << p << "\n";
    }
    std::cout << "Length of shortest path: " << path2.size() << "\n";

    // Demonstrate the minSpanTree method
    // Test 5: Find a minimum spanning tree of the file system
    std::map<fs::path, std::vector<fs::path>> tree1 = graph.minSpanTree();
    std::cout << "Minimum spanning tree:\n";
    for (const auto& pair : tree1) {
        std::cout << pair.first << ":\n";
        for (const auto& neighbor : pair.second) {
            std::cout << "  " << neighbor << "\n";
        }
    }

    // Test 6: Build a new graph from a different directory and find its minimum spanning tree
    FileSystemGraph graph2;
    graph2.buildGraph("C:/Program Files");  // Replace with the path to the directory
    std::map<fs::path, std::vector<fs::path>> tree2 = graph2.minSpanTree();
    std::cout << "Minimum spanning tree of the second graph:\n";
    for (const auto& pair : tree2) {
        std::cout << pair.first << ":\n";
        for (const auto& neighbor : pair.second) {
            std::cout << "  " << neighbor << "\n";
        }
    }

    return 0;
}
