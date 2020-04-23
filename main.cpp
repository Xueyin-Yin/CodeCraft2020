#include <iostream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <stack>
#include <map>
#include <set>

using namespace std;

#define INPUT_PATH "/data/test_data.txt"
#define OUTPUT_PATH "/projects/student/result.txt"

map<int, set<int>> graph;

int tarjan(); // Find all strongly-connected components
int buildGraph(); // Build graph based on the input file
int writeResult(); // Write result to the output file

int main(int argc, char* argv[]) {
    return 0;
}