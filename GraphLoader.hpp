//
// Created by denis on 22.04.2022.
//

#ifndef HYPERGRAPHS_GRAPHLOADER_HPP
#define HYPERGRAPHS_GRAPHLOADER_HPP

#include <set>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "Cell_t.hpp"
#include "Net_t.hpp"
#include "Part_t.hpp"

class graph_loader_t {
public:

    graph_loader_t(const std::string& file) : file_(file), file_stream_(file), cells_(), nets_() {
        if(!file_stream_.is_open()) {
            std::cerr << "ERROR: can't open input file\n";
            good_bit = false;
            return;
        }

        if(!LoadFirstLine()) return;

        cells_.reserve(num_cells);
        nets_.reserve(num_nets);

        for(unsigned long long i = 0; i < num_cells; ++i) {
            cells_.emplace_back(i);
        }

        for(unsigned long long i = 0; i < num_nets; ++i) {

            if(file_stream_.eof()) {
                std::cerr << "ERROR: Too little lines for all nets\n";
                good_bit = false;
                return;
            }

            net_t cur_net(i);

            std::string i_str;
            std::getline(file_stream_, i_str);
            std::stringstream str_stream(i_str);
            size_t i_num;

            if(option == 1 || option == 11) {
                str_stream >> cur_net.weight;
            }
            nets_.push_back(cur_net);

            while(str_stream >> i_num) {
                cells_.at(i_num-1).nets.push_back(&(*std::prev(nets_.end())));
                nets_.at(nets_.size()-1).cells.push_back(&cells_.at(i_num-1));
            }
        }

        if(option == 10 || option == 11) {
            if(file_stream_.eof()) {
                std::cerr << "ERROR: Too little weights for all cells_\n";
                good_bit = false;
                return;
            }

            for (unsigned long long i = 0; i < num_cells; ++i) {
                file_stream_ >> cells_.at(i).weight;
            }
        }

        char last_byte;
        file_stream_ >> last_byte; // to reach the end of file

        if(!file_stream_.eof()) {
            std::cerr << "ERROR: Too many symbols in input file\n";
            good_bit = false;
            return;
        }

        good_bit = true;
    }

    void WriteToFile(const std::unordered_set<size_t>& A, const std::unordered_set<size_t>& B) {
        std::string out_file = file_ + ".part.2";
        std::ofstream out_file_stream(out_file);

        if(!out_file_stream.is_open()) {
            std::cerr << "ERROR: Can't open file for write\n";
            return;
        }

        out_file_stream << num_nets << " " << num_cells;
        if(option != 0) out_file_stream << " " << option;
        out_file_stream << "\n";

        for(unsigned long long i = 0; i < nets_.size(); ++i) {
            if(option == 1 || option == 11) {
                out_file_stream << nets_.at(i).weight << " ";
            }

            for(auto it  = nets_.at(i).cells.begin(); it != std::prev(nets_.at(i).cells.end()); ++it) {
                if(A.contains((*it)->id))
                    out_file_stream << 1 << " ";
                else
                    out_file_stream << 0 << " ";
            }
            if(A.contains((*std::prev(nets_.at(i).cells.end()))->id))
                out_file_stream << 1 << "\n";
            else
                out_file_stream << 0 << "\n";
        }

//        if(option == 10 || option == 11) {
//            for(int i = 0; i < cells_.size(); ++i) {
//                out_file_stream << cells_.at(i).weight << "\n";
//            }
//        }
    }

    bool LoadFirstLine() {
        std::string i_str;
        int i_num;
        std::vector<int> input_nums;

        std::getline(file_stream_, i_str);
        std::stringstream str_stream(i_str);

        while(str_stream >> i_num) {
            input_nums.push_back(i_num);
        }

        if(input_nums.size() < 2) {
            std::cerr << "ERROR: too little first input nums, expected 2 or 3\n";
            good_bit = false;
            return false;
        } else if(input_nums.size() < 4) {
            num_nets = input_nums.at(0);
            num_cells = input_nums.at(1);
            option = (input_nums.size() == 3)? input_nums.at(2) : 0;
        } else {
            std::cerr << "ERROR: too many first input nums, expected 2 or 3\n";
            good_bit = false;
            return false;
        }

        return true;
    }

#if 0
    bool CompareGraphFiles() {
        std::ifstream original_file(file_, std::ifstream::binary|std::ifstream::ate);
        std::ifstream copy_file(file_ + "_output", std::ifstream::binary|std::ifstream::ate);

        if(original_file.fail() || copy_file.fail()) {
            std::cerr << "ERROR: Couldn't open files to compare\n";
            return false;
        }

        if(original_file.tellg() != copy_file.tellg()) {
            std::cout << original_file.tellg() << " " << copy_file.tellg() << "\n";
            std::cout << "Comparison end here\n";
            return false;
        }

        original_file.seekg(0, std::ifstream::beg);
        copy_file.seekg(0, std::ifstream::beg);

        return std::equal(std::istreambuf_iterator<char>(original_file.rdbuf()),
                     std::istreambuf_iterator<char>(),
                          std::istreambuf_iterator<char>(original_file.rdbuf()));
    }
#endif

    std::vector<net_t> nets_;
    std::vector<cell_t> cells_;

    bool good_bit;
    size_t num_cells;
    size_t num_nets;
    size_t option;
    std::string file_;
    std::fstream file_stream_;

};

#endif //HYPERGRAPHS_GRAPHLOADER_HPP
