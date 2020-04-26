#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <cstring>
#include <pthread.h>

using namespace std;

#define INPUT_PATH "/data/test_data.txt"
#define OUTPUT_PATH "/projects/student/result.txt"

#define SEPARATOR ","

#define NUM_THREADS 8

unordered_map<unsigned int, vector<unsigned int>> graph;
unordered_map<unsigned int, vector<unsigned int>> _graph;

int partition_size;

vector<unsigned int> ids;

vector<vector<vector<vector<unsigned int>>>> ress(NUM_THREADS);
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

void initRess() {

    for (int i = 0; i < NUM_THREADS; i++) {
        ress[i].push_back(vector<vector<unsigned int>> ());
        ress[i].push_back(vector<vector<unsigned int>> ());
        ress[i].push_back(vector<vector<unsigned int>> ());
        ress[i].push_back(vector<vector<unsigned int>> ());
        ress[i].push_back(vector<vector<unsigned int>> ());
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

    int num_ids = ids.size();
	int divisor = num_ids / NUM_THREADS;
	int remainer = num_ids % NUM_THREADS;
	partition_size = (remainer == 0) ? divisor : divisor + 1;

    fin.close();

    return 0;

}

vector<unsigned int> comparePath(int lenIdx, int *indexs)
{
    vector<int*> ptrs;
    vector<vector<unsigned int>*> ptrarray;

    for (int i = 0; i < NUM_THREADS; i++) {
        if (indexs[i] < ress[i][lenIdx].size()) {
            ptrs.push_back(&indexs[i]);
            ptrarray.push_back(&ress[i][lenIdx][indexs[i]]);
        }
    }

    int *ptr = ptrs[0];
    vector<unsigned int>* arr = ptrarray[0];
    int N = arr->size();
    for(int idx=1 ; idx<ptrarray.size() ; idx++)
    {
        for(int sizei=0 ; sizei < N ; sizei++)
        {
            if(ptrarray[idx]->at(sizei) < arr->at(sizei))
            {
                arr = ptrarray[idx];
                ptr = ptrs[idx];
                break;
            }
            else if(ptrarray[idx]->at(sizei) > arr->at(sizei))
            {
                break;
            }
        }
    }

    *ptr = *ptr + 1;

    return *arr;
}

void mergeResults()
{
    int indexs[NUM_THREADS];

    for(int len=3 ; len<8 ; len++)
    {
        int lenIdx = len - 3;
        
        memset(indexs, 0, NUM_THREADS * sizeof(int));

        while(1)
        {
            int count = 0;
            for (int i = 0; i < NUM_THREADS; i++) {
                if (indexs[i] >= ress[i][lenIdx].size()) {
                    count++;
                }
            }

            if (count == NUM_THREADS) break;

            vector<unsigned int> smallestPath = comparePath(lenIdx, indexs);
            res[lenIdx].push_back(smallestPath);
        }
    }
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

void dfs(int threadId, unsigned int current_node, unsigned int root_node, 
         vector<unsigned int>& path,
         unordered_set<unsigned int>& visit, unordered_map<unsigned int, int>& _visit, int depth)
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
        if((_visit.find(next_node) != _visit.end() && _visit[next_node] == -2) && (visit.find(next_node) == visit.end()))
        {
            path.push_back(next_node);
            int path_length = path.size();
            if(path_length > 2)
            {
                vector<unsigned int> temp(path);
                ress[threadId][path_length - 3].push_back(temp);
            }
            path.pop_back();
        }
        if((visit.find(next_node) != visit.end())) {
            continue;
        }
        if (depth > 3 && (_visit.find(next_node) == _visit.end() || (_visit[next_node] != root_node && _visit[next_node] != -2)))
        {
            continue;
        }
        if(path.size() == 6 || next_node == root_node)
        {
            continue;
        }

        visit.insert(next_node);
        path.push_back(next_node); 
        dfs(threadId, next_node, root_node, path, visit, _visit, depth + 1);
        path.pop_back();
        visit.erase(next_node);
    }
}

void dfs1(unsigned int current_node, unsigned int root_node, int length, 
            unordered_set<unsigned int> &visit,
            unordered_map<unsigned int, int> &_visit)
{
    if (_graph.find(current_node) == _graph.end()) {
        return;
    }
    
    for(auto next_node : _graph[current_node])
    {
        
        if(next_node < root_node || visit.find(next_node) != visit.end())
        {
            continue;
        }

        _visit[next_node] = root_node;
        if(length == 3)
            continue;

        visit.insert(next_node);
        dfs1(next_node, root_node, length + 1, visit, _visit);
        visit.erase(next_node);
    }
}

void *subTask(void *pid) {

    int start = *((int *)pid);
    int threadId = start;
    int end = (start + 1) * partition_size;

    for (int i = start * partition_size; i < ((end <= ids.size()) ? end : ids.size()); i++) {
        unsigned int current_node = ids[i];

        unordered_set<unsigned int> visit; 
        unordered_map<unsigned int, int> _visit;
        vector<unsigned int> path;

        dfs1(current_node, current_node, 1, visit, _visit);

        for(int j=0 ; j<_graph[current_node].size() ; j++)
        {
            _visit[_graph[current_node][j]] = -2;
        }

        path.push_back(current_node);
        dfs(threadId, current_node, current_node, path, visit, _visit, 1);
        path.pop_back();

        for(int j=0 ; j<_graph[current_node].size() ; j++)
        {
            _visit[_graph[current_node][j]] = current_node;
        }
    }

    free(pid);
    pthread_exit((void *) NULL);
}

void parallelDFS() {
    int *pid;
    pthread_t *ths = (pthread_t*)malloc(sizeof(pthread_t) * NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) {
        pid = (int *) malloc (sizeof(int));
        *pid = i;
        if (pthread_create(&ths[i], NULL, subTask, (void *) pid) != 0) {
            perror("Thread create");
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(ths[i], NULL) != 0) {
            perror("Thread join");
        }
    }
    if (ths != NULL) free(ths);
}


int main(int argc, char* argv[]) {

    initRess();

    buildGraph();

    parallelDFS();

    mergeResults();

    writeResult();

    return 0;
}