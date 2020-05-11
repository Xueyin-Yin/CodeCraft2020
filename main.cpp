#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <cstring>
#include <thread>

using namespace std;
using uint = unsigned int;

// If don't want to print the intermediate instructions, comments the following line out.
// #define DEBUG true

#define SEPARATOR ","
#define NUM_THREADS 4

// For testing on local dataset.
#ifdef DEBUG
#define INPUT_PATH "../HuaweiSet/1004812/test_data.txt"
#define OUTPUT_PATH "explore/result.txt"
#endif

// For official dataset
#ifndef DEBUG
#define INPUT_PATH "/data/test_data.txt"
#define OUTPUT_PATH "/projects/student/result.txt"
#endif

unordered_map<uint, vector<pair<uint, uint>>> graph;
unordered_map<uint, vector<pair<uint, uint>>> _graph;
int partition_size;
vector<uint> ids;
vector<vector<vector<vector<uint>>>> ress(NUM_THREADS);
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

		if ('0' <= str[i] && str[i] <= '9') 
        {
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
        ress[i].push_back(vector<vector<uint>> ());
        ress[i].push_back(vector<vector<uint>> ());
        ress[i].push_back(vector<vector<uint>> ());
        ress[i].push_back(vector<vector<uint>> ());
        ress[i].push_back(vector<vector<uint>> ());
    }
}

int buildGraph() {
    string line;
    unordered_set<uint> pts;
    ifstream fin(INPUT_PATH, ios::in | ios::binary);

#ifdef DEBUG
    cout << "Begin building graph." << endl; 
#endif

    if (!fin.is_open()) {
        cout << "Cannot open this file" << endl;
        return -1;
    } 

    vector<string> temp; 
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

    int num_ids = ids.size();
	int divisor = num_ids / NUM_THREADS;
	int remainer = num_ids % NUM_THREADS;
	partition_size = (remainer == 0) ? divisor : divisor + 1;

    fin.close();

#ifdef DEBUG
    cout << "Finished building graph." << endl;
#endif

    return 0;
}

vector<uint> comparePath(int lenIdx, int *indexs)
{
    vector<int*> ptrs;
    vector<vector<uint>*> ptrarray;

    for (int i = 0; i < NUM_THREADS; i++) {
        if (indexs[i] < ress[i][lenIdx].size()) {
            ptrs.push_back(&indexs[i]);
            ptrarray.push_back(&ress[i][lenIdx][indexs[i]]);
        }
    }

    int *ptr = ptrs[0];
    vector<uint>* return_array = ptrarray[0];
    int N = return_array->size();
    for(int idx=1 ; idx<ptrarray.size() ; idx++)
    {
        for(int sizei=0 ; sizei < N ; sizei++)
        {
            if(ptrarray[idx]->at(sizei) < return_array->at(sizei))
            {
                return_array = ptrarray[idx];
                ptr = ptrs[idx];
                break;
            }
            else if(ptrarray[idx]->at(sizei) > return_array->at(sizei))
            {
                break;
            }
        }
    }

    *ptr = *ptr + 1;
    return *return_array;
}

void mergeResults()
{
    int indexs[NUM_THREADS];

#ifdef DEBUG
    int _NUM_LINE = 0;
#endif

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

            vector<uint> smallestPath = comparePath(lenIdx, indexs);
            res[lenIdx].push_back(smallestPath);
#ifdef DEBUG
            _NUM_LINE++;
#endif
        }
    }

#ifdef DEBUG
    cout << "Finished merging results from " << NUM_THREADS << " threads." << endl;
    cout << "Found " << _NUM_LINE << " cycles." << endl;
#endif
}

int writeResult() {
    ofstream fout(OUTPUT_PATH, ios::out | ios::binary);

#ifdef DEBUG
    cout << "Begin writing results into file." << endl;
#endif

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

#ifdef DEBUG
    cout << "Writing successfully!" << endl;
#endif

    return 0;
}

void dfs(int threadId, 
         uint current_node, 
         uint root_node, 
         vector<uint>& path,
         unordered_set<uint>& visit, 
         unordered_map<uint, vector<vector<uint>>>& _visit, 
         int depth, 
         uint pre_amount)
{
    if (graph.find(current_node) == graph.end() || depth > 4) {
        return;
    }    

    for(pair<uint, uint> next_edge: graph[current_node])
    {
        uint next_node = next_edge.first;
        uint amount = next_edge.second;
        // If next_node is smaller than root_node, that means next_node has already been considered before, skip.
        // If next_node has already been visited in current path, skip.

        if(next_node < root_node || (next_node != root_node && visit.find(next_node) != visit.end()))
        {
            continue;
        }

        float q = amount * 1.0 / pre_amount;
        if (current_node != root_node && (q < 0.2 || q > 3.0)) {
            continue;
        }       

        if(next_node == root_node)
        {
            int path_length = path.size();
            if(path_length >= 3)
            {
                vector<uint> tmp_path(path);
                ress[threadId][path_length - 3].push_back(tmp_path);
            }

            continue;
        }
        else if(depth == 4 && _visit.find(next_node) != _visit.end())
        {        
            for(int i=0 ; i<_visit[next_node].size() ; i++)
            { 
                int _len = _visit[next_node][i].size() + path.size();
                if(3 <= _len && _len <= 7) 
                {
                    vector<uint> tmp_path(path);
                    bool nodeVisited = false;
                    for(int j=_visit[next_node][i].size()-1, k=0 ; j>=0 ; j--, k++)
                    {
                        uint _node = _visit[next_node][i][j];
                        if(visit.find(_node) != visit.end())
                        {
                            nodeVisited = true;
                            break;
                        }

                        tmp_path.push_back(_node);
                    }

                    if(nodeVisited) continue;

                    int path_length = tmp_path.size();
                    ress[threadId][path_length - 3].push_back(tmp_path);
                }
            } 
        }

        if(depth >= 4) continue;
        
        visit.insert(next_node);
        path.push_back(next_node); 
        dfs(threadId, next_node, root_node, path, visit, _visit, depth + 1, amount);
        path.pop_back();
        visit.erase(next_node);
    }
}

void inverted_dfs(uint current_node, uint root_node, int length, 
            vector<uint>& path,
            unordered_set<uint> &visit,
            unordered_map<uint, vector<vector<uint>>> &_visit, 
            uint pre_amount)
{
    if (_graph.find(current_node) == _graph.end()) {
        return;
    }
    
    for(pair<uint, uint> next_edge : _graph[current_node])
    {
        uint next_node = next_edge.first;
        uint amount = next_edge.second;

        if(next_node < root_node || visit.find(next_node) != visit.end())
        {
            continue;
        }

        float q = pre_amount * 1.0 / amount;

        if (current_node != root_node && (q < 0.2 || q > 3.0)) {
            continue;
        }

        path.push_back(next_node);
        _visit[next_node].push_back(path);
        
        if(length == 3)
        {
            path.pop_back();
            continue;
        }

        visit.insert(next_node);
        inverted_dfs(next_node, root_node, length + 1, path, visit, _visit, amount);
        visit.erase(next_node);
        path.pop_back();
    }
}

void subTask(int threadId)
{
    int start = threadId * partition_size;
    int end = (threadId + 1) * partition_size;

    for (int i = start; i < ((end <= ids.size()) ? end : ids.size()); i++) {
        uint current_node = ids[i];

        unordered_set<uint> visit; 
        unordered_map<uint, vector<vector<uint>>> _visit;
        vector<uint> reverse_path;

        visit.insert(current_node);
        inverted_dfs(current_node, current_node, 1, reverse_path, visit, _visit, 0);        
        visit.erase(current_node);

        for(unordered_map<uint, vector<vector<uint>>>::iterator it = _visit.begin() ; it != _visit.end() ; it++)
        {
            if(it->second.size() > 1)
            {
                sort(it->second.begin(), it->second.end(), [](const vector<uint> &_a, const vector<uint> &_b){
                    if(_a.size() != _b.size() || _a.empty() || _b.empty()) 
                        return _a.size() <= _b.size();
                    
                    int _len = _a.size();
                    for(int j=_len-1 ; j>=0 ; j--)
                    {
                        if(_a[j] != _b[j])
                            return _a[j] < _b[j];
                    }

                    return _a[0] < _b[0];
                });
            }
        }       

        vector<uint> path;

        path.push_back(current_node);
        dfs(threadId, current_node, current_node, path, visit, _visit, 1, 0);
        path.pop_back();
    }
}

void parallelDFS() {
#ifdef DEBUG
    cout << "Begin parallel DFS." << endl;
#endif

    vector<thread> ths;
    for(int i=0 ; i < NUM_THREADS ; i++)
    {
        ths.push_back(thread(subTask, i));
    }

    for(int i=0 ; i < NUM_THREADS ; i++)
    {
        ths[i].join();
    }

#ifdef DEBUG
    cout << "Parallel job finished." << endl;
#endif
}


int main(int argc, char* argv[]) {

    initRess();

    buildGraph();

    parallelDFS();

    mergeResults();

    writeResult();

    return 0;
}