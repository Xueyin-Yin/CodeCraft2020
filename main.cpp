#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <pthread.h>

using namespace std;

#define INPUT_PATH "/data/test_data.txt"
#define OUTPUT_PATH "/projects/student/result.txt"

#define SEPARATOR ","

#define NUM_THREADS 3

unordered_map<unsigned int, vector<unsigned int>> graph;
unordered_map<unsigned int, vector<unsigned int>> _graph;
unordered_map<unsigned int, int> visit;
unordered_map<unsigned int, int> _visit;

pthread_t ths[NUM_THREADS];

void *(*curFunc)(void *);

vector<unsigned int> ids;

vector<unsigned int> path;
vector<vector<vector<unsigned int>>> res(5);

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

void createThreads() {

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&ths[i], NULL, curFunc, (void *) i);
    }
}

int buildGraph() {
    std::string line;
    unordered_set<unsigned int> pts;
    ifstream fin(INPUT_PATH, ios::in | ios::binary);

    if (!fin.is_open()) {
        cout << "Cannot open this file" << endl;
        return -1;
    } 

    vector<std::string> temp; 
    unsigned int src;
    unsigned int dest;

    while (getline(fin, line)) {
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

    for (auto pt = _graph.begin(); pt != _graph.end(); pt++) {
        sort(pt->second.begin(), pt->second.end());
    }    

    fin.close();

    return 0;

}

int writeResult() {
    ofstream fout(OUTPUT_PATH, ios::out | ios::binary);

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

void dfs(unsigned int current_node, unsigned int root_node, int depth)
{
    if (graph.find(current_node) == graph.end()) {
        return;
    }    

    for(int i=0 ; i<graph[current_node].size() ; i++)
    {
        unsigned int next_node = graph[current_node][i];
        if(next_node <= root_node)
        {
            continue;
        }
        if(_visit[next_node] == -2 && visit[next_node] == 0)
        {
            path.push_back(next_node);
            int path_length = path.size();
            if(path_length > 2)
            {
                    vector<unsigned int> temp(path);
                    res[path_length - 3].push_back(temp);
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

        visit[next_node] = 1;
        path.push_back(next_node); 
        dfs(next_node, root_node, depth + 1);
        path.pop_back();
        visit[next_node] = 0;
    }
}

void dfs1(unsigned int current_node, unsigned int root_node, int length)
{
    if (_graph.find(current_node) == _graph.end()) {
        return;
    }
    
    for(auto next_node : _graph[current_node])
    {
        
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

void *subTask(void *pid) {

    pthread_exit((void *) NULL);
}

void threadRun(void *(*task)(void *)) {

    curFunc = task;

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(ths[i], NULL);
    }
}

int main(int argc, char* argv[]) {

    createThreads();

    buildGraph();

    for (unsigned int node : ids) {
        visit[node] = 0;
        _visit[node] = -1;
    }

    for(unsigned int current_node : ids)
    {
        dfs1(current_node, current_node, 1);
        
        for(int j=0 ; j<_graph[current_node].size() ; j++)
        {
            _visit[_graph[current_node][j]] = -2;
        }

        path.push_back(current_node);
        dfs(current_node, current_node, 1);
        path.pop_back();


        for(int j=0 ; j<_graph[current_node].size() ; j++)
        {
            _visit[_graph[current_node][j]] = current_node;
        }
    }

    writeResult();

    return 0;
}