#include <iostream>
#include <fstream> 
#include <cstdint>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <sstream>
#include <iterator>

//Useful: splitting strings
std::vector<std::string> split(std::string const &in) { 
    std::istringstream buffer(in);
    std::vector<std::string> out((std::istream_iterator<std::string>(buffer)), 
                                 std::istream_iterator<std::string>());
    return out;
}


int main (int argc, char** argv) {
    // First argument is filename
    if (argc != 2) {
        std::cout << "Please input filename" << std::endl;
        return 1;
    }

    // Try to open file
    std::ifstream file;
    file.open(argv[1]);

    if (!file.is_open()){
        std::cout << "Unable to open file " << argv[1] << std::endl;
        return 1;
    }

    //Read data from file into vector
    std::vector<std::string> commands;
    std::string value;

    //Just read each command and apply its action
    uint16_t hor = 0;
    uint16_t aim = 0;
    uint32_t ver = 0;

    for(std::string command; std::getline(file, command);) {
        //Split the string
        std::vector<std::string> comm = split(command);
        //Check what command it is and apply the corresponding transformation
        switch (comm[0][0]) {
            case 'd':
                aim += std::stoi(comm[1]);
                break;
            case 'u':
                aim -= std::stoi(comm[1]);
                break;
            case 'f':
                uint8_t delta = std::stoi(comm[1]);
                hor += delta;
                ver += aim*delta;
                break;
        }
    }

    //Don't need the file anymore.
    file.close();

    //Print out the result
    std::cout << "Horizontal: " << hor << std::endl;
    std::cout << "Vertical: " << aim << std::endl;
    std::cout << "Product: " << hor*aim << std::endl;

    std::cout << "True vertical: " << ver << std::endl;
    std::cout << "True product: " << hor*ver << std::endl;

    return 0;
}