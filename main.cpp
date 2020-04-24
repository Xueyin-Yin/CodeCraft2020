#include <iostream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <stack>
#include <unordered_map>
#include <set>

using namespace std;

#define INPUT_PATH "/data/test_data.txt"
#define OUTPUT_PATH "/projects/student/result.txt"

vector<vector<int>> graph;
vector<vector<int>> _graph;
vector<int> idxToId;
unordered_map<int, int> idToIdx;
vector<int> visit;
vector<int> _visit;

vector<int> path;
vector<vector<int>> res;

int dfs(); // DFS in graph
int _dfs(); // DFS in _graph

int buildGraph(); // Build graph based on the input file
int writeResult(vector<vector<int>> res); // Write result to the output file

int buildGraph() {
    char buffer[1024];
    FILE *fp = fopen(INPUT_PATH, "rb");

    if (fp == NULL) {
        printf("Fail to open file!\n");
        return -1;
    }

    while (!feof(fp)) {
        fgets(buffer, 1024, fp);

    }

}

void dfs(unsigned int current_node, unsigned int root_node)
{
    for(int i=0 ; i<graph[current_node].size() ; i++)
    {
        unsigned int next_node = graph[current_node][i];
        if(next_node < root_node)
        {
            continue;
        }
        else if(visit1[next_node] == -2 && visit[next_node] == 0)
        {
            path.push_back(next_node);
            int path_length = path.size();
            if(path_length > 2)
            {
                res[path_length - 3].push_back(path);
            }
            path.pop_back();
        }
        else if(visit[next_node] == 1 || (visit1[next_node] != root_node && visit1[next_node] != -2))
        {
            continue;
        }
        else if(path.size() == 6 || next_node == root_node)
        {
            continue;
        }

        visit[next_node] = 1;
        path.push_back(next_node);
        dfs(graph, next_node, root_node, visit, visit1, res, path);
        path.pop_back();
        visit[next_node] = 0;
    }
}

void dfs1(const unordered_map<unsigned int, vector<unsigned int>> &thisGraph,  unsigned int current_node, unsigned int root_node, int length)
{
    for(int i=0 ; i<thisGraph[current_node].size() ; i++)
    {
        unsigned int next_node = thisGraph[current_node][i];
        if(next_node < root_node || visit[next_node] == 1)
        {
            continue;
        }

        visit1[next_node] = root_node;
        if(length == 3)
            continue;

        visit[next_node] = 1;
        dfs1(thisGraph, next_node, root_node, visit, visit1, length + 1);
        visit[next_node] = 0;
    }
}

int main(int argc, char* argv[]) {

    buildGraph();

    for()
    {
        unsigned int current_node;
        dfs1(graph, current_node, current_node, 1);
        dfs1(_graph, current_node, current_node, 1);

        for(int j=0 ; j<_graph[current_node].size() ; j++)
        {
            visit1[_graph[current_node][j]] = -2;
        }

        path.push_back(current_node);
        dfs(current_node, current_node);
        path.pop_back();

        for(int j=0 ; j<_graph[current_node].size() ; j++)
        {
            _visit[_graph[current_node][j]] = current_node;
        }
    }

    return 0;
}