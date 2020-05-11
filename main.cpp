#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

using namespace std;

#define INPUT_PATH "/data/test_data.txt"
#define OUTPUT_PATH "/projects/student/result.txt"

#define SEPARATOR ","

using uint = unsigned int;

unordered_map<uint, vector<pair<uint, uint>>> graph;
unordered_map<uint, vector<pair<uint, uint>>> _graph;
unordered_map<uint, int> visit;
unordered_map<uint, int> _visit;

vector<uint> ids;

vector<uint> path;
vector<vector<vector<uint>>> res(5);

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

uint strtoui(string str)
{
	uint result(0);
	for (int i = str.size()-1;i >= 0;i--) {
		uint temp(0),k = str.size() - i - 1;

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
    unordered_set<uint> pts;
    ifstream fin(INPUT_PATH, ios::in | ios::binary);

    if (!fin.is_open()) {
        cout << "Cannot open this file" << endl;
        return -1;
    } 

    vector<std::string> temp; 
    uint src;
    uint dest;
    uint amount;

    while (getline(fin, line)) {
        temp.clear();

        splitString(line, temp, SEPARATOR);
        src = strtoui(temp[0]);
        dest = strtoui(temp[1]);
        amount = strtoui(temp[2]);

        graph[src].push_back({dest, amount});
  
        _graph[dest].push_back({src, amount});

        pts.insert(src);
        pts.insert(dest);

    }

    ids.assign(pts.begin(), pts.end());
    sort(ids.begin(), ids.end());

    for (auto pt = graph.begin(); pt != graph.end(); pt++) {
        sort(pt->second.begin(), pt->second.end());
    }

    for (auto pt = _graph.begin(); pt != _graph.end(); pt++) {
        sort(pt->second.begin(), pt->second.end());
    }    

    fin.close();

    return 0;

}

int writeResult() {
    ofstream fout(OUTPUT_PATH, ios::out);

    if (fout.fail()) {
        cout << "Cannot output to this file" << endl;
        return -1;
    }

    int count = 0;
    for (auto iter : res) {
        count += iter.size();
    }

    fout << count << endl;
    for (auto iter : res) {
        for (auto iter1 : iter) {
            for (int i = 0; i < iter1.size() - 1; i++) {
                fout << iter1[i] << ",";
            }
            if (iter1.size() > 0) fout << iter1[iter1.size() - 1] << endl;
        }
    }

    fout.close();

    return 0;
}

void dfs(uint current_node, uint root_node, int depth, uint first_amount, uint pre_amount)
{
    if (graph.find(current_node) == graph.end()) {
        return;
    }    

    float q;

    for(int i=0 ; i<graph[current_node].size() ; i++)
    {
        auto next_edge = graph[current_node][i];
        uint next_node = next_edge.first;
        uint amount = next_edge.second;

        if(next_node <= root_node)
        {
            continue;
        }

        q = amount * 1.0 / pre_amount;

        if ((current_node != root_node) && (q < 0.2 || q > 3.0)) {
        	continue;
        }

        if(_visit[next_node] == -2 && visit[next_node] == 0)
        {
            path.push_back(next_node);
            int path_length = path.size();
            if(path_length > 2)
            {
            	q = first_amount * 1.0 / amount;
            	if (q >= 0.2 && q <= 3.0) {
	                vector<uint> temp(path);
	                res[path_length - 3].push_back(temp);            		
            	}
            }
            path.pop_back();
        }
        if(visit[next_node] == 1)
        {
            continue;
        }
        if((depth > 3 && _visit[next_node] != root_node) && _visit[next_node] != -2)
        {
            continue;
        }
        if(path.size() == 6 || next_node == root_node)
        {
            continue;
        }

        if (current_node == root_node) {
        	first_amount = amount;
        }

        visit[next_node] = 1;
        path.push_back(next_node); 
        dfs(next_node, root_node, depth + 1, first_amount, amount);
        path.pop_back();
        visit[next_node] = 0;
    }
}

void dfs1(uint current_node, uint root_node, int length)
{
    if (_graph.find(current_node) == _graph.end()) {
        return;
    }
    
    for(auto next_edge : _graph[current_node])
    {
        uint next_node = next_edge.first;

        if(next_node < root_node || visit[next_node] == 1)
        {
            continue;
        }

        _visit[next_node] = root_node;
        if(length == 3)
            continue;

        visit[next_node] = 1;
        dfs1(next_node, root_node, length + 1);
        visit[next_node] = 0;
    }
}

int main(int argc, char* argv[]) {

    buildGraph();

    for (uint node : ids) {
        visit[node] = 0;
        _visit[node] = -1;
    }

    for(uint current_node : ids)
    {
        dfs1(current_node, current_node, 1);
        
        for(int j=0 ; j<_graph[current_node].size() ; j++)
        {
            _visit[_graph[current_node][j].first] = -2;
        }

        path.push_back(current_node);
        dfs(current_node, current_node, 1, 0, 0);
        path.pop_back();


        for(int j=0 ; j<_graph[current_node].size() ; j++)
        {
            _visit[_graph[current_node][j].first] = current_node;
        }
    }

    writeResult();

    return 0;
}