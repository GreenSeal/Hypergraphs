//
// Created by denis on 22.04.2022.
//

#ifndef HYPERGRAPHS_GRAPHLOADER_HPP
#define HYPERGRAPHS_GRAPHLOADER_HPP

#include <unordered_set>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "Cell_t.hpp"
#include "Net_t.hpp"

class graph_loader_t {
public:

    graph_loader_t(const std::string& file);
    void WriteToFile(const std::unordered_set<size_t>& A, const std::unordered_set<size_t>& B);
    bool LoadFirstLine();

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
