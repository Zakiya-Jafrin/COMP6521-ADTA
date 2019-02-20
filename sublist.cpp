#include "sublist.h"
using namespace std;

//constructor: initialize members and read the first block
Sublist::Sublist(int position, int max_blocks, fstream& input, int block_size, int tuple_length) : input(input) {
    this->position = position;
    this->blocks_read = 0;
    this->exhausted = false;
    this->max_blocks = max_blocks;
    this->block_size = block_size;
    this->tuple_length = tuple_length;
    this->read_block();
}

// aggregate all tuples in this sublist that match with the field CID; reload from file as necessary if the tuples buffer for this sublist becomes empty
double Sublist::aggregate(int cid) {
    double sum = 0;
    while (!(this->exhausted && this->tuples.empty())) {
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

// read next block and put it in buffer; if no more blocks left then turn on exhausted flag and return
void Sublist::read_block() {
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
