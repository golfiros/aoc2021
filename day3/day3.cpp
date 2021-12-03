#include <iostream>
#include <fstream> 
#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

//Useful: Function that gives bitset from string
std::vector<bool> strToBits(std::string str) {
    std::vector<bool> out;
    for(char c : str){
        out.push_back((bool)(c-'0'));
    }
    return out;
}

//Useful: Function that returns most common bit in a column
bool mostCommon(std::vector<std::vector<bool>> vec, uint8_t col) {
    uint16_t n = std::accumulate(vec.begin(), vec.end(), 0, [=](uint16_t x, std::vector<bool> y){return x + y[col];});
    return n >= vec.size() - n;
}

//Useful: msbf bitset to integer

uint16_t bitsToInt(std::vector<bool> vec) { 
    return std::accumulate(vec.begin(), vec.end(), 0, [](uint16_t x, uint8_t y){return (x << 1) + y;});
}

int main (int argc, char** argv) {
    // First argument is filename
    if (argc != 2) {
        std::cerr << "Unexpected number of arguments" << std::endl;
        return 1;
    }

    // Try to open file
    std::ifstream file;
    file.open(argv[1]);

    if (!file.is_open()){
        std::cout << "Unable to open file '" << argv[1] << "'" << std::endl;
        return 1;
    }

    //Read data from file into vector
    std::vector<std::vector<bool>> readings;
    for (std::string value; std::getline(file, value); readings.push_back(strToBits(value))) {}

    //Don't need the file anymore.
    file.close();

    //Store the length of the entries
    uint16_t len = readings[0].size();

    //Now we count and set the bits
    std::vector<bool> gammaVec;
    for(uint8_t i = 0; i < len; i++) {
        gammaVec.push_back(mostCommon(readings, i));
    }
    uint16_t gamma = bitsToInt(gammaVec);
    gammaVec.flip();
    uint16_t epsilon = bitsToInt(gammaVec);

    std::cout << "Gamma reading: " << gamma << std::endl;
    std::cout << "Epsilon reading: " << epsilon << std::endl;
    std::cout << "Product: " << gamma*epsilon << std::endl;

    //Now we create lists for each of our other two values, copies of the original
    std::vector<std::vector<bool>> oxyVec(readings), co2Vec(readings);

    //Process the vectors
    //Oxygen
    for(uint8_t i = 0; (i < len && oxyVec.size() > 1); i++) {
        bool comm = mostCommon(oxyVec, i);
        //Erase the values that are not the most common
        oxyVec.erase(std::remove_if(oxyVec.begin(), oxyVec.end(), [=](std::vector<bool> n){return n[i] != comm;}),oxyVec.end());
    }
    //CO2
    for(uint8_t i = 0; (i < len && co2Vec.size() > 1); i++) {
        bool comm = !mostCommon(co2Vec, i);
        //Erase the values that are not the most common
        co2Vec.erase(std::remove_if(co2Vec.begin(), co2Vec.end(), [=](std::vector<bool> n){return n[i] != comm;}),co2Vec.end());
    }

    uint16_t oxy = bitsToInt(oxyVec[0]), co2 = bitsToInt(co2Vec[0]);

    std::cout << "Oxygen reading: " << oxy << std::endl;
    std::cout << "CO2 reading: " << co2 << std::endl;
    std::cout << "Product: " << oxy*co2 << std::endl;

    return 0;
}