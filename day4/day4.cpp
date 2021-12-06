#include <iostream>
#include <fstream> 
#include <cstdint>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <numeric>

//Useful: split strings by delimeter
std::vector<std::string> delSplit(std::string s, std::string del) {
    size_t start = 0, len = del.length();
    std::vector<std::string> out;

    for (size_t end; (end = s.find(del,start)) != std::string::npos; start = end + len) {
        out.push_back(s.substr(start, end - start));
    }

    out.push_back(s.substr(start));
    return out;
}

//Win condition checker
bool wins(std::vector<bool> status, uint8_t m, uint8_t n) {
    bool ret = 0;
    for(uint8_t i=0;i<m;i++){
        uint16_t s = 0;
        for(uint8_t j=0;j<n;j++){
            s += status[n*i+j];
        }
        ret |= (s == n);
    }
    for(uint8_t j=0;j<n;j++){
        uint16_t s = 0;
        for(uint8_t i=0;i<m;i++){
            s += status[n*i+j];
        }
        ret |= (s == m);
    }
    return ret;
}

//Score calculator
uint32_t score(std::vector<uint16_t> board, std::vector<bool> status, uint16_t g){
    std::vector<uint16_t> nBoard;
    std::transform(board.begin(), board.end(), status.begin(), std::back_inserter(nBoard), [](uint16_t x, bool y){return x*(1-y);});
    return std::accumulate(nBoard.begin(), nBoard.end(), 0) * g;
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

    //Read data from file

    //Bingo dimensions
    uint16_t m = 1, n = 0;

    //We start by getting the list of numbers and the first board to see its properties
    std::vector<uint16_t> guesses;
    std::vector<std::vector<uint16_t>> boards;
    std::vector<std::vector<bool>> sBoards;
    {
        std::string line;
        std::getline(file,line);
        for(std::string x : delSplit(line,",")) {
            guesses.push_back(std::stoi(x));
        }
        std::getline(file,line); //Skip one line
        std::vector<uint16_t> board; //First board
        //Find number of columns
        std::getline(file,line);{
            std::istringstream sLine(line);
            while (!sLine.eof()) {
                uint16_t x;
                sLine >> x;
                board.push_back(x);
                n++;
            }
        }
        
        //Now the number of lines
        for(std::getline(file,line); !line.empty(); m++) {
            std::istringstream sLine(line);
            for (uint8_t j = 0; j < n; j++) {
                uint16_t x;
                sLine >> x;
                board.push_back(x);
            }
            std::getline(file,line);
        }
        boards.push_back(board);
        //Also push a boolean vector of zeroes to the board status
        sBoards.push_back(std::vector<bool>(m*n));
    }

    //Now we load the other boards
    while(!file.eof()) {
        std::vector<uint16_t> board;
        for (uint16_t i = 0; i < m * n; i++){
            uint16_t x;
            file >> x;
            board.push_back(x);
        }
        boards.push_back(board);
        sBoards.push_back(std::vector<bool>(m*n));
    }
    //Also generate a vector with indices to keep track of winners
    std::vector<uint16_t> index(boards.size());
    std::iota(index.begin(), index.end(), 0);
    std::vector<uint16_t> winners;
    std::vector<uint16_t> lastCall(boards.size());

    //Now we iterate over the guesses
    for(uint16_t g : guesses) {
        for (uint16_t& i : index) {
            std::transform(boards[i].begin(), boards[i].end(), sBoards[i].begin(), sBoards[i].begin(), [=](uint16_t x, bool b){return x == g || b;});
            //If a board has won, add it to the winners list and mark for deletion
            if (wins(sBoards[i], m, n)) {
                winners.push_back(i);
                lastCall[i] = g;
                i = -1;
            }
        }
        //Delete the indices we don't need anymore
        index.erase(std::remove_if(index.begin(), index.end(), [](uint16_t i){return i == (uint16_t)-1;}),index.end());
        if(index.size() == 0){ break; } //Stop if all boards have won already
    }

    //Now for the scores
    std::vector<uint32_t> scores;
    for (uint16_t w : winners) {
        scores.push_back(score(boards[w],sBoards[w],lastCall[w]));
    }

    for(uint32_t s : scores) {
        std::cout << s << " ";
    }
    std::cout << std::endl;

    return 0;
}