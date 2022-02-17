/*
Skeleton code for linear hash indexing
*/

#include <string>
#include <ios>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include "classes.h"
using namespace std;


int main(int argc, char* const argv[]) {

    // Create the index
    ofstream temp;
    temp.open("EmployeeIndex");
    temp << ' ';
    temp.close();
    
    LinearHashIndex emp_index("EmployeeIndex");
    emp_index.createFromFile("Employee.csv");

    // Loop to lookup IDs until user is ready to quit
    while (true)
    {
        string input;
        cout << "Please enter id for lookup or q to exit." << endl;
        cin >> input;

        if (input == "q")
        {
            break;
        }
        else
        {
            Record temp = emp_index.findRecordById(stoi(input));
            if (temp.name == "0"){
                cout << "No match found" << endl;
            }
            else {
                temp.print();
            }
        }
    }
    

    return 0;
}
