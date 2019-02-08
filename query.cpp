#include <iostream>
#include <numeric>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <queue>
#include <list>
#include <unordered_set>
using namespace std;
const int tuple_length = 250; // size of one tuple in bytes, according to lab assignment specifications
const int block_size = 15;    // number of tuples that can fit in one block, according to lab assignment specifications

/* parameters: input is the fstream for the file; memory is the allowed memory in MB
 * returns: the number of sublists we have to use due to memory limitations for the given memory
 * algorithm (phase 1):
 *   calculate how many blocks can fit into main memory at once
 *   for each such sublist, read it, sort in place by CID field, replace sublist in file
 */
int phase1(fstream& input, int memory) {
    int sublist_size = memory * pow(2, 20) / (tuple_length * block_size);        // calculate how many blocks can fit into memory (in terms of number of blocks)
    int sublist_size_tuples = sublist_size * block_size;                         // calculate how many tuples can fit into memory (in terms of number of tuples)
    cout << "number of tuples that can fit at a time in memory: " << sublist_size_tuples << endl;

    bool end = input.eof(); // keep track of when we reach end of file

    // until EOF reached, keep reading in sublist-sized tuples, sort them in-place, and put them back in the same position in the file
    int count;
    for (count = 0; !end; ++count) {
        vector<string> chunk;
        string line;

        // keep reading until maximum number of tuples read, or EOF reached
        for (int i = 0; i < sublist_size_tuples && getline(input, line); ++i)
            chunk.push_back(line);

        // output iteration number, and total memory consumption of the vector (MB)
        cout << "iteration: " << (count+1) << ", total memory used (MB): " << chunk[0].size() * chunk.size() / pow(2,20)  << " MB" << endl;

        // sort in-place by CID field; c++'s sort() uses in-place quicksort
        // other option is using priority_queue so it stores it in a min-heap data structure
        sort(chunk.begin(), chunk.end(), [](const string& s1, const string& s2) -> bool {return stoi(s1.substr(18,9)) < stoi(s2.substr(18,9));});

        // if EOF encountered, reset eof bit in fstream so we can seekp() file again to write back to it
        end = input.eof();
        if (end)
            input.clear();

        // seek to start of the chunk and write back to file; in "tuple_length + 1", the +1 is for newlines that we have to account for
        input.seekp(count * (tuple_length + 1) * sublist_size_tuples);
        for (string s: chunk)
            input << s << endl;
    }
    return count;
}

// class for managing a single sublist
class Sublist {
    public:
        queue<string> tuples; // queue of tuples read from file that are currently in buffer for this particular sublist
        bool exhausted;       // true iff sublist is exhausted

        //constructor: initialize members and read one block
        Sublist(int position, int max_blocks, fstream& input) : input(input) {
            this->position = position;
            this->blocks_read = 0;
            this->exhausted = false;
            this->max_blocks = max_blocks;
            this->read_block();
        }
        // aggregate all tuples in this sublist that match with the field CID; reload from file as necessary if the tuples buffer for this sublist becomes empty
        double aggregate(int cid) {
            double sum = 0;
            while (!this->exhausted) {
                if (stoi(tuples.front().substr(18,9)) == cid) {
                    sum += stod(tuples.front().substr(241,9));
                    tuples.pop();
                }
                else
                    break;
                if (tuples.empty())
                    this->read_block();
            }
            return sum;
        }
    private:
        int blocks_read; // number of blocks already read from this sublist
        int position;    // next position in text file to read from
        int max_blocks;  // maximum number of blocks that can be read from a sublist
        fstream& input;  // reference to input stream

        // read next block and put it in buffer (return true); if no more blocks left then turn on exhausted flag and return false
        void read_block() {
            input.seekg(position);
            string line;
            for (int i = 0; i < block_size && getline(input, line); ++i) {
                tuples.push(line);
            }

            // update blocks_read and next position to read from; if max amount read then turn on exhausted flag
            ++blocks_read;
            position += (tuple_length + 1) * block_size;
            if (input.eof() || blocks_read == max_blocks) {
                input.clear();
                exhausted = true;
            }
        }
};

/* parameters: input is the fstream for the file; memory is the allowed memory in MB; count is the number of sublists in the file
 * algorithm (phase 2):
 *   load first block of each sublist into main memory buffer  
 *   iterate to find next highest CID key amongst the first tuple in each sublist buffer
 *   compute the aggregate for each CID by incrementing its "sum"
 *   if buffer becomes empty, replace it with the next block from the same sublist; if sublist becomes exhausted then remove its reference altogether
 *   output aggregate for each CID one by one, starting from the lowest value CID
 */
void phase2(fstream& input, int memory, int count) {
    // seek to start of file
    input.seekg(0);

    int sublist_size = memory * pow(2, 20) / (tuple_length * block_size);        // calculate how many blocks can fit into memory (in terms of number of blocks)
    int sublist_size_tuples = sublist_size * block_size;                         // calculate how many tuples can fit into memory (in terms of number of tuples)

    // initialize buffer; using std::list instead of std::vector since we want to dynamically remove exhausted buffers from list in constant time, while iterating
    list<Sublist*> buffer; 
    for (int i = 0; i < count; ++i) {
        Sublist* temp = new Sublist(i * (tuple_length + 1) * sublist_size_tuples, sublist_size, input);
        buffer.push_back(temp);
    }

    while (!buffer.empty()) {
        // retrieve CID with least value amongst the first tuple of each sublist
        int cid = numeric_limits<int>::max();
        for (auto& it : buffer) {
            cid = min(cid, stoi(it->tuples.front().substr(18,9)));
        }

        // aggregate the sum over all buffers
        double sum = 0;
        for (auto it = buffer.begin(); it != buffer.end(); ) {
            sum += (*it)->aggregate(cid);

            // if sublist is now exhausted, then remove it from buffer
            if ((*it)->exhausted)
                it = buffer.erase(it);
            else
                ++it;
        }
        // write aggregate of current CID's Amount-Paid to stdout
        cout << to_string(cid) << " " << fixed << setprecision(2) << sum << endl;
    }
}

int main(int argc, char* argv[]) {
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
    int count = phase1(input, memory);

    // do phase 2 and write to stdout
    phase2(input, memory, count);

    // close input file and return
    input.close();
    return 0;
}
