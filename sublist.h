#include <string>
#include <queue>
#include <fstream>

// class for managing a single sublist and its corresponding buffer
class Sublist {
    public:
        //constructor: initialize members and read the first block
        Sublist(int position, int max_blocks, std::fstream& input, int block_size, int tuple_length);

        std::queue<std::string> tuples; // queue of tuples read from file that are currently in buffer for this particular sublist
        bool exhausted;                 // true iff sublist is exhausted

        // aggregate all tuples in this sublist that match with the field CID; reload from file as necessary if the tuples buffer for this sublist becomes empty
        double aggregate(int cid);

    private:
        std::fstream& input;  // reference to input stream
        int blocks_read;      // number of blocks already read from this sublist
        int max_blocks;       // maximum number of blocks that can be read from a sublist
        int position;         // next position in text file to read from
        int block_size;       // size of one tuple in bytes, according to lab assignment specifications
        int tuple_length;     // number of tuples that can fit in one block, according to lab assignment specifications

        // read next block and put it in buffer; if no more blocks left then turn on exhausted flag and return
        void read_block();
};
