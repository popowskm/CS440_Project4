#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <bitset>
using namespace std;

class Record {
public:
    int id, manager_id;
    std::string bio, name;

    Record(vector<std::string> fields) {
        id = stoi(fields[0]);
        name = fields[1];
        bio = fields[2];
        manager_id = stoi(fields[3]);
    }

    Record(string raw) {
        string word;
        stringstream  s(raw);

        getline(s, word, ',');
        id = stoi(word);
        getline(s, word, ',');
        name = word;
        getline(s, word, ',');
        bio = word;
        getline(s, word, ',');
        manager_id = stoi(word);
    }

    void print() {
        cout << "\tID: " << id << "\n";
        cout << "\tNAME: " << name << "\n";
        cout << "\tBIO: " << bio << "\n";
        cout << "\tMANAGER_ID: " << manager_id << "\n";
    }

    int size() {
        return 16 + name.length() + bio.length();
    }

    // Does not write endl or delimiter of any kind
    void write(fstream &ouput_file){
        ouput_file << id << "," << name << "," << bio << "," << manager_id;
    }
};

class Block {

private:
    const int PAGE_SIZE = 4096;
    vector<Record> records;
    int free_space;
    int numrecords;
    int overflow;


public:
    // Generates empty block
    Block() {
        free_space = PAGE_SIZE - 1; // Bytes of data free in block
        numrecords = 0; // Number of records in block
        overflow = 0; // Location of overflow block in index
    }

    // Generates block from current position in file by reading and parsing a page of chars
    void from_file(int pos, fstream &file){
        free_space = PAGE_SIZE - 1;
        numrecords = 0;
        overflow = 0;
        records.clear();

        string input, r;
        file.seekg(pos * PAGE_SIZE);
        getline(file, input, '\n');
        stringstream s(input);

        while (true) {
            if(getline(s, r, '$')) {
                // Checks for '*' in string and sets
                if (r.find('*', 0) != string::npos) {
                    string over;
                    for (char c: r) {
                        if (c != '*') {
                            over += c;
                        }
                    }
                    overflow = stoi(over);
                    break;
                }
                Record temp(r);
                
                free_space -= r.length() + 1;
                numrecords += 1;

                records.push_back(temp);
            }
            else {
                break;
            }

        }
    }

    void set_overflow(int o) {
        overflow = o;
    }

    int get_overflow() {
        return overflow;
    }

    void add_record(Record r) {
        free_space -= r.size() + 4;
        numrecords += 1;
        records.push_back(r);
    }

    // 
    Record id_match(int id, fstream &file) {
        for (Record temp: records) {
            if (temp.id == id) {
                return temp;
            }
        }
        if(overflow != 0) {
            Block b;
            b.from_file(overflow, file);
            return b.id_match(id, file);
        }
        else {
            Record r("0,0,0,0");
            return r;
        }
    }

    void print() {
        for (Record r: records) {
            r.print();
            cout << endl;
        }
    }

    // Pads to page size with asterisks. Delimits with $. Assumes data fits in block.
    void write_block(int pos, fstream &file) {
        file.seekg(pos * PAGE_SIZE);
        for (Record item: records) {
            item.write(file);
            file << '$';
        }

        file << overflow;
        
        for (int j = 0; j < free_space - to_string(overflow).length(); j++) {
            file << '*';
        }
        file << '\n' << flush;
    }

    int space() {
        return free_space;
    }
};


class LinearHashIndex {

private:
    const int PAGE_SIZE = 4096; // Record size at max 708

    vector<int> pageDirectory;  // Where pageDirectory[h(id)] gives page index of block
                                // can scan to pages using index*PAGE_SIZE as offset (using seek function)
    int numBlocks; // Number of buckets, doesn't include overflow blocks
    int i;
    int numRecords; // Records in index
    int nextFreePage; // Next page to write to
    string fName;

    // Insert new record into index
    void insertRecord(Record record) {
        fstream index_file;
        index_file.open(fName, fstream::in | fstream::out);
        Block block1, block2, block3;

        // No records written to index yet
        if (numRecords == 0) {
            // Initialize index with first blocks (start with 2)
            block1.write_block(0, index_file);
            block1.write_block(1, index_file);

            nextFreePage = 2;
            numBlocks = 2;
            i = 1;
            pageDirectory.push_back(0);
            pageDirectory.push_back(1);
        }

        // Add record to the index in the correct block, creating overflow block if necessary
        int hash = hash_id(record.id);
        // bitflip
        if (hash > numBlocks) {
            hash = hash - pow(2,(i-1));
        }

        int location = pageDirectory[hash];
        while (true) {
            block1.from_file(location, index_file);
            if (block1.space() > record.size() + 1) {
                // Add block to record, write and break
                block1.add_record(record);
                block1.write_block(location, index_file);
                break;
            }
            else {
                // Set location to overflow position and loop
                if (block1.get_overflow() > 0) {
                    location = block1.get_overflow();
                }
                else {
                    // Create overflow block, set overflow of previous block, write and break
                    block1.set_overflow(generate_overflow(block2, index_file));
                    block1.write_block(location, index_file);
                    block2.add_record(record);
                    block2.write_block(block1.get_overflow(), index_file);
                    break;
                }
            }
        }

        // TODO
        // Check capacity
        // assume 70% capacity of a block is 4 records
        if (numRecords >= (4 * numBlocks)) {
            // increment n
            //      create new block
            //      reshuffle?
            numBlocks += 1;
            block3.write_block(nextFreePage, index_file);
            pageDirectory.push_back(nextFreePage);
            nextFreePage += 1;

            // increment i bc numBlocks reached capacity
            if (numBlocks == pow(2,1)) {
                i += 1;
            }    
        }
        // Take neccessary steps if capacity is reached

        // Case for when just n increases
            // Increases blocks by 1
            // Check block + potential overflow blocks for records with ids that were bitflipped
            // Migrate relevant records
            // Write new blocks out



        numRecords += 1;
    }

    // Creates overflow block at next free page and returns that position to pass into overflow
    int generate_overflow(Block b, fstream &file) {
        b.write_block(nextFreePage, file);
        nextFreePage += 1;
        return nextFreePage - 1;
    }

    // Starts on the right with LSB
    int hash_id(int id) {
        return id % int(pow(2,i));
    }

    

public:
    LinearHashIndex(string indexFileName) {
        numBlocks = 0;
        i = 0;
        numRecords = 0;
        fName = indexFileName;
    }

    // Read csv file and add records to the index
    void createFromFile(string csvFName) {
        fstream input_file;
        input_file.open(csvFName, ios::in);

        while (true)
        {
            string line, word;
            vector<string> components;
            
            if (getline(input_file, line, '\n'))
            {
                Record r(line);
                insertRecord(r);

            }
            else
            {
                break;
            }
        }
        
        input_file.close();

    }

    // Given an ID, find the relevant record and print it
    Record findRecordById(int id) {
        int page = pageDirectory[hash_id(id)];
        Block b;
        fstream index_file;
        index_file.open(fName, fstream::in | fstream::out);
        b.from_file(page, index_file);

        Record match("0,0,0,0");
        match = b.id_match(id, index_file);
        return match;
    }
};
