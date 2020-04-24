#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <stack>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

using namespace std;

#define TEST_PATH ".\\data\\test1.txt"
#define INPUT_PATH "/data/test_data.txt"
#define OUTPUT_PATH "/projects/student/result.txt"

#define SEPARATOR ","

unordered_map<unsigned int, vector<unsigned int>> graph;
unordered_map<unsigned int, vector<unsigned int>> _graph;
unordered_map<unsigned int, int> visit;
unordered_map<unsigned int, int> _visit;

vector<unsigned int> ids;

vector<int> path;
vector<vector<int>> res;

int dfs(); // DFS in graph
int _dfs(); // DFS in _graph

// For debug
void debugGraph();
void debugIds();

void splitString(const string& s, vector<string>& v, const string& c);
int buildGraph(); // Build graph based on the input file
int writeResult(vector<vector<int>> res); // Write result to the output file

// Debug the ids
void debugIds() {
    for (auto id : ids) {
        cout << id << " ";
    }
    cout << endl;
}

// Debug the graph
void debugGraph() {
    for (auto pt = graph.begin(); pt != graph.end(); pt++) {
        cout << pt->first << " ->  { ";

        for (auto i : pt->second) {
            cout << i << " ";
        } 

        cout << "}" << endl;
    }
}

void splitString(const string& s, vector<string>& v, const string& c) {
  string::size_type pos1, pos2;
  pos2 = s.find(c);
  pos1 = 0;

  while(string::npos != pos2) {
    v.push_back(s.substr(pos1, pos2-pos1));
 
    pos1 = pos2 + c.size();
    pos2 = s.find(c, pos1);
  }

  if(pos1 != s.length())
    v.push_back(s.substr(pos1));
}


unsigned int strtoui(string str)
{
	unsigned int result(0);
	for (int i = str.size()-1;i >= 0;i--) {
		unsigned int temp(0),k = str.size() - i - 1;

		if (isdigit(str[i])) {

			temp = str[i] - '0';
			while (k--)
				temp *= 10;
			result += temp;
		}
		else
			break;
	}
	return result;
}

int buildGraph() {
    std::string line;
    unordered_set<unsigned int> pts;
//    ifstream fin(INPUT_PATH, ios::in | ios::binary);
    ifstream fin(TEST_PATH, ios::in | ios::binary);

    if (!fin.is_open()) {
        cout << "Cannot open this file" << endl;
        return -1;
    } 

    vector<std::string> temp; 
    unsigned int src;
    unsigned int dest;

    while (1) {
        getline(fin, line);
        if (fin.eof()) {
            break;
        }
        temp.clear();

        splitString(line, temp, SEPARATOR);
        src = strtoui(temp[0]);
        dest = strtoui(temp[1]);

        graph[src].push_back(dest);
        _graph[dest].push_back(src);

        pts.insert(src);
        pts.insert(dest);

    }

    ids.assign(pts.begin(), pts.end());
    sort(ids.begin(), ids.end());

    for (auto pt = graph.begin(); pt != graph.end(); pt++) {
        sort(pt->second.begin(), pt->second.end());
    }

    fin.close();

    return 0;

}

int main(int argc, char* argv[]) {

    buildGraph();

 //   debugGraph();
 //   debugIds();

    return 0;
}