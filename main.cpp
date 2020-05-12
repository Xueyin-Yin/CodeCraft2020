#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <cstring>
#include <thread>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


using namespace std;
using uint = unsigned int;

// If don't want to print the intermediate instructions, comments the following line out.
#define DEBUG true

#ifndef UINT_MAX
#define UINT_MAX 0xffffffff
#endif

#define NUM_THREADS 4

// For testing on local dataset.
#ifdef DEBUG
#define INPUT_PATH "/data/test_data.txt"
#define OUTPUT_PATH "/projects/student/result.txt"
#endif

// For official dataset
#ifndef DEBUG
#define INPUT_PATH "/data/test_data.txt"
#define OUTPUT_PATH "/projects/student/result.txt"
#endif

vector<vector<pair<uint, uint>>> graph;
vector<vector<pair<uint, uint>>> _graph;
unordered_map<uint, uint> indexTable;
int partition_size;
vector<uint> ids;
uint NodeAmount = 0;
vector<vector<vector<vector<uint>>>> ress(NUM_THREADS);
vector<vector<vector<uint>>> res(5);

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

    char *mm = NULL;
    int fd = open(INPUT_PATH, O_RDONLY);

    if (fd < 0) {
        cout << "Cannot open this file" << endl;
        return -1;
    }

    long size = lseek(fd, 0, SEEK_END);
    mm = (char *)mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);

    if (mm == MAP_FAILED) {
        cout << "Map failed" << endl;
        return -1;
    } 

    vector<string> temp; 
    unordered_set<uint> pts;
    vector<vector<uint>> data;
    uint src;
    uint dest;
    uint amount;

    long ptr = 0;

    while (ptr < size) {
        src = 0;
        dest = 0;
        amount = 0;

        while (mm[ptr] != ',') {
            src = src * 10 + (mm[ptr] - '0');
            ptr++;
        }
        ptr++;
        while (mm[ptr] != ',') {
            dest = dest * 10 + (mm[ptr] - '0');
            ptr++;
        }
        ptr++;
        while(mm[ptr] != '\n') {
            amount = amount * 10 + (mm[ptr] - '0');
            ptr++;
        }

        data.push_back({src, dest, amount});
        pts.insert(src);
        pts.insert(dest);

        ptr++;
    }

    ids.assign(pts.begin(), pts.end());
    sort(ids.begin(), ids.end());

    for(uint i=0 ; i<ids.size() ; i++)
    {
        indexTable[ids[i]] = i;
    }

    for(vector<uint>& p: data)
    {
        uint from = indexTable[p[0]];
        uint to = indexTable[p[1]];
        amount = p[2];

        while(graph.size() <= from) 
            graph.push_back({});

        graph[from].push_back({to, amount});

        while(_graph.size() <= to) 
            _graph.push_back({});
        
        _graph[to].push_back({from, amount});
    }

    // for (auto pt = graph.begin(); pt != graph.end(); pt++) {
    //     sort(pt->second.begin(), pt->second.end());
    // }

    for(int i=0 ; i<graph.size() ; i++)
    {
        sort(graph[i].begin(), graph[i].end());
    }

    // for (auto pt = _graph.begin(); pt != _graph.end(); pt++) {
    //     sort(pt->second.begin(), pt->second.end());
    // }    

    for(int i=0 ; i<_graph.size() ; i++)
    {
        sort(_graph[i].begin(), _graph[i].end());
    }

    NodeAmount = ids.size();
	int divisor = NodeAmount / NUM_THREADS;
	int remainer = NodeAmount % NUM_THREADS;
	partition_size = (remainer == 0) ? divisor : divisor + 1;

    munmap(mm, size);
    close(fd);
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
    FILE *fp = fopen(OUTPUT_PATH, "wb");
    char buffer[34];

    if (fp == NULL) {
        cout << "Cannot output to this file" << endl;
        return -1;
    }

    uint count = 0;
    for (auto iter : res) {
        count += iter.size();
    }
    sprintf(buffer, "%u", count);
    strcat(buffer, "\n");
    fwrite(&buffer, 1, strlen(buffer), fp);

    for (auto iter : res) {
        for (auto iter1 : iter) {
            for (int i = 0; i < iter1.size() - 1; i++) {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer, "%u", iter1[i]);
                strcat(buffer, ",");
                fwrite(&buffer, 1, strlen(buffer), fp);                
            }
            if (iter1.size() > 0) {
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer, "%u", iter1[iter1.size() - 1]);
                strcat(buffer, "\n");
                fwrite(&buffer, 1, strlen(buffer), fp);    
            }
        }
    }

    fclose(fp);

    return 0;
}

struct IPath 
{
    vector<uint> path;
    uint FirstAmount;
    uint LastAmount;
    int size;

    IPath(vector<uint> _path, uint _first_amount = 0, uint _last_amount = 0):
        FirstAmount(_first_amount),
        LastAmount(_last_amount),
        path(_path)
    {
        size = _path.size();
    }
};

bool inRange(uint X, uint Y)
{
    // 0.2 <= Y/X <= 3.0

    long long x = X, y = Y;

    return (x <= y)? (y <= 3 * x): (x <= 5 * y);
}

void dfs(int threadId, 
         uint current_node, 
         uint root_node, 
         uint first_amount,
         vector<uint>& path,
         vector<bool>& visit, 
         vector<vector<IPath>>& _visit, 
         int depth, 
         uint pre_amount)
{
    // if (graph.find(current_node) == graph.end() || depth > 4) 
    if(current_node >= graph.size() || depth > 4)
    {
        return;
    }    

    for(pair<uint, uint>& next_edge: graph[current_node])
    {
        uint next_node = next_edge.first;
        uint amount = next_edge.second;

        // If next_node is smaller than root_node, that means next_node has already been considered before, skip.
        // If next_node has already been visited in current path, skip.
        if(next_node < root_node || (next_node != root_node && visit[next_node]))
        {
            continue;
        }

        if (current_node != root_node && !inRange(pre_amount, amount)) {
            continue;
        }       

        if(current_node == root_node) first_amount = amount;

        if(next_node == root_node)
        {
            int path_length = path.size();
            if(path_length >= 3 && inRange(amount, first_amount))
            {
                vector<uint> tmp_path(path);
                ress[threadId][path_length - 3].push_back(tmp_path);
            }

            continue;
        }
        // else if(depth == 4 && _visit.find(next_node) != _visit.end())
        else if(depth == 4)
        {        
            for(IPath &ipath: _visit[next_node])
            { 
                int _len = ipath.size + path.size();
                if(3 <= _len && _len <= 7) 
                {
                    if(!inRange(ipath.FirstAmount, first_amount)) continue;
                    if(!inRange(amount, ipath.LastAmount)) continue;

                    vector<uint> tmp_path(path);
                    bool nodeVisited = false;
                    for(int j=ipath.size-1 ; j>=0 ; j--)
                    {
                        uint _node = ipath.path[j];

                        // if(visit.find(_node) != visit.end())
                        if(visit[_node])
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
        
        // visit.insert(next_node);
        visit[next_node] = true;
        path.push_back(next_node); 
        dfs(threadId, next_node, root_node, first_amount, path, visit, _visit, depth + 1, amount);
        path.pop_back();
        // visit.erase(next_node);
        visit[next_node] = false;
    }
}

void inverted_dfs(uint current_node, uint root_node, uint first_amount,int length,
            vector<uint>& path,
            vector<bool> &visit,
            vector<vector<IPath>> &_visit, 
            uint pre_amount)
{
    // if (_graph.find(current_node) == _graph.end()) 
    if(current_node >= _graph.size())
    {
        return;
    }
    
    for(pair<uint, uint>& next_edge : _graph[current_node])
    {
        uint next_node = next_edge.first;
        uint amount = next_edge.second;

        if(next_node < root_node || visit[next_node])
        {
            continue;
        }

        if (current_node != root_node && !inRange(amount, pre_amount)) {
            continue;
        }

        if(current_node == root_node) first_amount = amount;

        path.push_back(next_node);
        _visit[next_node].push_back(IPath(path, first_amount, amount));
        
        if(length == 3)
        {
            path.pop_back();
            continue;
        }

        // visit.insert(next_node);
        visit[next_node] = true;
        inverted_dfs(next_node, root_node, first_amount, length + 1, path, visit, _visit, amount);
        // visit.erase(next_node);
        visit[next_node] = false;
        path.pop_back();
    }
}

void subTask(int threadId)
{
    int start = threadId * partition_size;
    int end = (threadId + 1) * partition_size;

    for (uint i = start; i < ((end <= NodeAmount) ? end : NodeAmount); i++) {
        uint current_node = i;

        // unordered_set<uint> visit; 
        vector<bool> visit(NodeAmount);
        vector<vector<IPath>> _visit(NodeAmount);
        vector<uint> reverse_path;
        uint first_amount = 0;

        // visit.insert(current_node);
        visit[current_node] = true;
        inverted_dfs(current_node, current_node, first_amount, 1, reverse_path, visit, _visit, 0);        
        // visit.erase(current_node);
        visit[current_node] = false;

        // for(unordered_map<uint, vector<IPath>>::iterator it = _visit.begin() ; it != _visit.end() ; it++)
        for(int j=0 ; j<_visit.size() ; j++)
        {
            // if(it->second.size() > 1)
            if(_visit[j].size() > 1)
            {
                sort(_visit[j].begin(), _visit[j].end(), [](const IPath &_a, const IPath &_b){
                    if(_a.size != _b.size || _a.path.empty() || _b.path.empty()) 
                        return _a.size <= _b.size;
                    
                    int _len = _a.size;
                    for(int j=_len-1 ; j>=0 ; j--)
                    {
                        if(_a.path[j] != _b.path[j])
                            return _a.path[j] < _b.path[j];
                    }

                    return _a.path[0] < _b.path[0];
                });
            }
        }       

        vector<uint> path;

        path.push_back(current_node);
        dfs(threadId, current_node, current_node, first_amount, path, visit, _visit, 1, 0);
        path.pop_back();
    }
}

void parallelDFS() {

    vector<thread> ths;
    for(int i=0 ; i < NUM_THREADS ; i++)
    {
        ths.push_back(thread(subTask, i));
    }

    for(int i=0 ; i < NUM_THREADS ; i++)
    {
        ths[i].join();
    }

}


int main(int argc, char* argv[]) {

#ifdef DEBUG
    initRess();

    time_t start = {0};
    time_t end = {0};

    time(&start);
    buildGraph();
    time(&end);

    cout << "Build Graph: " << end - start << " s" << endl;

    time(&start);
    parallelDFS();
    time(&end);

    cout << "DFS: " << end - start << " s" << endl;

    time(&start);
    mergeResults();
    time(&end);

    cout << "Merge result: " << end - start << " s" << endl;

    time(&start);
    writeResult();
    time(&end);

    cout << "Write result: " << end - start << " s" << endl;

#endif

#ifndef DEBUG
    initRess();

    buildGraph();

    parallelDFS();

    mergeResults();

    writeResult();
#endif

    return 0;

}