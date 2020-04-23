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

int main(int argc, char* argv[]) {

    buildGraph();



    return 0;
}