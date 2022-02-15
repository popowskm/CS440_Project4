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

    void print() {
        cout << "\tID: " << id << "\n";
        cout << "\tNAME: " << name << "\n";
        cout << "\tBIO: " << bio << "\n";
        cout << "\tMANAGER_ID: " << manager_id << "\n";
    }

    int size() {
        return 16 + name.length() + bio.length();
    }

    void write(fstream &ouput_file){
        ouput_file << id << "," << name << "," << bio << "," << manager_id << endl;
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
    int block_size;
    string fName;

    // Insert new record into index
    void insertRecord(Record record) {

        // No records written to index yet
        if (numRecords == 0) {
            // Initialize index with first blocks (start with 2)

        }

        // Add record to the index in the correct block, creating overflow block if necessary


        // Take neccessary steps if capacity is reached


    }

public:
    LinearHashIndex(string indexFileName) {
        numBlocks = 0;
        i = 0;
        numRecords = 0;
        block_size = 0;
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
                stringstream  s(line);

                getline(s, word, ',');
                components.push_back(word);
                getline(s, word, ',');
                components.push_back(word);
                getline(s, word, ',');
                components.push_back(word);
                getline(s, word, ',');
                components.push_back(word);
                Record r(components);
                insertRecord(r);
            }
            else
            {
                break;
            }
        }
        


    }

    // Given an ID, find the relevant record and print it
    Record findRecordById(int id) {
        
    }
};
