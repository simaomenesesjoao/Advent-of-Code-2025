#include <cassert>
#include <list>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <map>

using Matrix = std::vector<std::vector<bool>>;
using IntMatrix = std::vector<std::vector<int>>;

Matrix parse_input(const std::string& filename){

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'\n";
        return {};
    }

    Matrix matrix;
    std::string line;

    while(std::getline(file, line)) {
        if(line.empty()){
            continue;
        }
        std::vector<bool> bool_line;
        for(char c: line){
            if(c == '@'){
                bool_line.push_back(true);
            } else {
                bool_line.push_back(false);
            }
        }
        matrix.push_back(bool_line);        
    }
    return matrix;
}

IntMatrix get_int_matrix(const Matrix& matrix){

    int Nrows = matrix.size();
    int Ncols = matrix.at(0).size();

    IntMatrix int_matrix(Nrows, std::vector<int>(Ncols, 0));
    
    unsigned int count = 0;
    for(int i=0; i<Nrows; i++){
        int first_i = std::max(0, i-1);
        int last_i = std::min(i+1, Nrows-1);

        for(int j=0; j<Ncols; j++){

            if(matrix.at(i).at(j) == false){
                continue;
            }

            int first_j = std::max(0, j-1);
            int last_j = std::min(j+1, Ncols-1);

            unsigned int local_count = 0;
            for(int ii = first_i; ii <= last_i; ii++){
                for(int jj = first_j; jj <= last_j; jj++){
                    if(matrix.at(ii).at(jj) == true){
                        local_count++;
                    }
                }
            }

            int_matrix.at(i).at(j) = local_count;

        }
    }
    return int_matrix;
}

unsigned int first_part(const Matrix& matrix){


    const auto int_matrix = get_int_matrix(matrix);

    unsigned int count = 0;
    for(const auto& row: int_matrix){
        for(int col: row){
            std::cout << col;
            if(col < 5 && col != 0){
                count++;
            }
        }
        std::cout << "\n";
    }
    return count;
}


struct Cell {
    int row, col, count;
};

unsigned int second_part(const Matrix& matrix){

    int Nrows = matrix.size();
    int Ncols = matrix.at(0).size();
    IntMatrix num_neighbours = get_int_matrix(matrix);

    std::map<std::pair<int, int>, Cell> cells_to_be_removed, cells_surrounded;

    unsigned int count = 0;
    for(int i=0; i<Nrows; i++){
        for(int j=0; j<Ncols; j++){

            unsigned int num_neighs = num_neighbours.at(i).at(j);
            if(num_neighs == 0){
                continue;
            }

            Cell cell{i, j, num_neighs};
            if(num_neighs < 5){
                cells_to_be_removed.try_emplace({i,j}, cell);
            } else {
                cells_surrounded.try_emplace({i,j}, cell);
            }
            

        }
    }

    // std::cout << "cells_to_be_removed:\n";
    // for(const auto& [key, val]: cells_to_be_removed){
    //     std::cout << key.first << " " << key.second << " " << val.count << "\n";
    // }
    
    // std::cout << "cells_surrounded:\n";
    // for(const auto& [key, val]: cells_surrounded){
    //     std::cout << key.first << " " << key.second << " " << val.count << "\n";
    // }

    while(!cells_to_be_removed.empty()){

        auto it = cells_to_be_removed.begin();
        const auto& [key, _] = *it;
        const auto& [i, j] = key;
        
        // std::cout << "new iteration: (i,j): " << i << " " << j << " - " << cells_to_be_removed.size() << "\n";

        num_neighbours.at(i).at(j) = 0;

        int first_i = std::max(0, i-1);
        int last_i = std::min(i+1, Nrows-1);
        int first_j = std::max(0, j-1);
        int last_j = std::min(j+1, Ncols-1);

        
        for(int ii = first_i; ii <= last_i; ii++){
            for(int jj = first_j; jj <= last_j; jj++){

                auto& cell = num_neighbours.at(ii).at(jj);
                // std::cout << "(ii,jj): " << ii << " " << jj << " " << cell << "\n" << std::flush;
                if(cell != 0){
                    cell--;
                    if(cell == 4){
                        // It was 5 and now is 4, so it needs to change places
                        auto it_s = cells_surrounded.find({ii,jj});
                        // if(it_s != cells_surrounded.end()){
                        //     std::cout << "iterator found.\n";
                        // } else {

                        //     std::cout << "iterator not found.\n";
                        // }
                        cells_surrounded.erase(it_s);
                        cells_to_be_removed.try_emplace({ii,jj});
                    }
                }
            }
        }
        cells_to_be_removed.erase(it);
        count++;


        // for(const auto& row: num_neighbours){
        //     for(const auto& c: row){
        //         std::cout << c; 
        //     }
        //     std::cout << "\n";
        // }

    }



    return count;
}

int main(){

    auto matrix = parse_input("input.txt");

    // for(const auto& row: matrix){
    //     for(const auto& c: row){
    //         std::cout << c;
    //     }
    //     std::cout << "\n";
    // }

    unsigned int count = first_part(matrix);
    std::cout << "count: " << count << "\n";


    count = second_part(matrix);
    std::cout << "count: " << count << "\n";
    
}