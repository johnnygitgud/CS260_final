#include <filesystem>
#include <iostream>
#include <map>
#include <vector>

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
};

int main() {
    // Create a FileSystemGraph object
    FileSystemGraph graph;
    // Build the graph starting from the root directory
    graph.buildGraph("C:/");  // Replace with the path to the root directory
    // Print the graph
    graph.printGraph();
    return 0;
}