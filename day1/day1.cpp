#include <iostream>
#include <fstream> 
#include <cstdint>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>

//Useful: n-ary transform algorithm
template <class Op, class OutIt, class InIt, class ... Ins> OutIt nTransform(Op f, OutIt out, InIt first, InIt last, Ins ... ins) {
    while(first != last) {
        *out++ = f(*first++, *ins++...);
    }
    return out;
}

uint16_t jumps(std::vector<uint16_t> data) {
    std::vector<int16_t> diffs;
    std::adjacent_difference(data.begin(), data.end(), std::back_inserter(diffs));
    return std::count_if(diffs.begin()+1, diffs.end(), [](int8_t d){return d>0;});
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
    std::vector<uint16_t> depths;
    for(std::string value; std::getline(file, value); depths.push_back(std::stoi(value))) {}

    //Don't need the file anymore.
    file.close();

    //Print number of positive differences
    std::cout << "Jumps: " << jumps(depths) << std::endl;

    //Create vector for the filtered depths
    std::vector<uint16_t> fDepths;

    //Let it be the sum of three consecutive depths
    nTransform([](uint16_t a, uint16_t b, uint16_t c){return a+b+c;}, std::back_inserter(fDepths), depths.begin(), depths.end()-2, depths.begin()+1,depths.begin()+2);

    //And now we print the filtered jump count
    std::cout << "Filtered Jumps: " << jumps(fDepths) << std::endl;

    return 0;
}
