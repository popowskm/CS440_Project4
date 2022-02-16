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
    void from_file(fstream &file){
        free_space = PAGE_SIZE - 1;
        numrecords = 0;
        overflow = 0;

        string input, r;
        getline(file, input, '\n');
        stringstream s(input);

        while (true) {
            if(getline(s, r, '$')) {
                // Checks for '*' in string and sets
                if (r.find('*', 0) != string::npos) {
                    overflow = r[0] - '0';
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

    void add_record(Record r) {
        free_space -= r.size() + 4;
        numrecords += 1;
        records.push_back(r);
    }

    void print() {
        for (Record r: records) {
            r.print();
            cout << endl;
        }
    }

    // Pads to page size with asterisks. Delimits with $. Assumes data fits in block.
    void write_block(fstream &file) {
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
    int numBlocks; // n
    int i;
    int numRecords; // Records in index
    int nextFreePage; // Next page to write to
    string fName;

    // Insert new record into index
    void insertRecord(Record record) {
        fstream index_file;
        index_file.open(fName, fstream::in | fstream::out);
        Block block1, block2;

        // No records written to index yet
        if (numRecords == 0) {
            // Initialize index with first blocks (start with 2)
            block1.write_block(index_file);
            block2.write_block(index_file);

            nextFreePage = 2;
            numBlocks = 2;
            i = 1;
        }

        // Add record to the index in the correct block, creating overflow block if necessary
        int hash = hash_id(record.id, i);
        int location = hash * PAGE_SIZE;
        
        index_file.seekg(location);
        block1.from_file(index_file);
        if (block1.space() > record.size() + 1) {
            block1.add_record(record);
        }
        index_file.seekg(location);
        block1.write_block(index_file);
        // Take neccessary steps if capacity is reached

        numRecords += 1;
    }

    // Starts on the right with LSB
    int hash_id(int id, int j) {
        return id % int(pow(2,j));
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
        


    }

    // Given an ID, find the relevant record and print it
    // Record findRecordById(int id) {
        
    // }
};
