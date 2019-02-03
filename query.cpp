#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <sys/time.h> 
#include <sys/resource.h> 
using namespace std;
const int tuple_length = 250; // size of one tuple in bytes
const int block_size = 15;    // number of tuples that can fit in one block
/* 
    SQL query to process:
    SELECT CID, SUM(Amount-Paid)
    FROM CLAIM
    GROUP BY CID;

    Number of tuples to consider:
    a) 100,000
    b) 1,000,000

    Main memory: // use setrlimit() or other methods to restrict memory usage
    c)  5 MB
    d) 10 MB

    Other info:
    Length of each tuple: 250 bytes
    Data block size:      15 tuples = 3750 bytes

    Algorithm:
    Phase 1 (e.g. 5MB memory):
    int chunk = 5*(2^20)/3750 = 1398 blocks can fit into main memory at once
    repeat 20970 + 1 = 20971 times
       read next chunk
       sort in place by CID field
       replace chunk in database with the sorted version

    Phase 2:
    Load the first block of each sublist into main memory buffer
    Use priority queue to keep track of the smallest key among first remaining elements of all the lists
    Repeatedly find the least value of the least value of the sort key present among the first available tuples of the buffer
        computle aggregate by incrementing "sum" 
        if a buffer becomes empty, replace it with the next block from the same sublist; if no more tuples in sublist then leave the block as it is
        when there are no more tuples with the current value (v) output the tuple consisting of grouping attributes and "sum"

*/
void phase1(fstream& input, int memory) {
    // calculate how many blocks can fit into memory
    int chunk_size = memory * pow(2, 20) / (tuple_length * block_size);
    //cout << chunk_size * block_size << endl;
    bool end = input.eof();

    for (int count = 0; !end; ++count) {
        vector<string> chunk;
        string line;

        // keep reading until maximum number of tuples read, or EOF reached
        for (int i = 0; i < chunk_size * block_size && getline(input, line); ++i) {
            //cout << count << ": " << input.tellg() << ": " << input.tellp() << endl;
            chunk.push_back(line);
        }
        // sort in-place by CID field; assuming that c++'s sort() uses in-place quicksort; otherwise we'll have to change this
        // or alternatively, just merge into sorted order as we read the records (binary search log(n)), but it might be n^2
        sort(chunk.begin(), chunk.end(), [](const string& s1, const string& s2) -> bool {return stoi(s1.substr(18,9)) < stoi(s2.substr(18,9));});

        //cout << count << ": " << input.tellg() << ": " << input.tellp() << endl;
        // reset eof bit so we can seekp() file again
        end = input.eof();
        if (end)
            input.clear();

        // seek to start of the chunk and write back to file; "tuple_length + 1", the +1 is for newlines that we have to account for
        input.seekp(count * (tuple_length + 1) * chunk_size * block_size);
        //cout << count << ": " << input.tellg() << ": " << input.tellp() << endl;
        for (string s: chunk)
            input << s << endl;
    }
    //for (string x: chunk)
        //cout << x << endl;
}
int main(int argc, char* argv[]) {
    // set memory limit
    /*
    struct rlimit memlimit;
    long bytes = 100*1024*1024;
    memlimit.rlim_cur = bytes;
    memlimit.rlim_max = bytes;
    setrlimit(RLIMIT_AS, &memlimit);
    */

    // handle input filename and memory through arguments
    if (argc != 3) {
        cerr << "ERROR: usage: query.cpp <input_file> <memory restriction in megabytes>" << endl;
        return 1;
    }
    fstream input;
    input.open(argv[1]);
    if (!input.is_open()) {
        cerr << "ERROR: cannot open input file" << endl;
        return 1;
    }
    int memory = atoi(argv[2]);

    // do phase 1 and write back to file
    phase1(input, memory);

    // do phase 2 and write to stdout

    // close input file and return
    input.close();
    return 0;
}
