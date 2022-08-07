//
// Created by denis on 09.05.2022.
//

#ifndef HYPERGRAPHS_PART_T_HPP
#define HYPERGRAPHS_PART_T_HPP

#include <list>
#include <cassert>
#include <unordered_set>
#include <iostream>
#include <vector>
#include "Cell_t.hpp"

class part_t {
public:

    using cell_idx_t = size_t;

    part_t(const std::vector<cell_t> &cells, size_t nets_size, const bool is_B, const std::vector<size_t> &cut);

    void UpdateGain(std::vector<cell_t>::pointer cell, int diff, bool is_mod);
    bool IsBucketEmpty() const;
    cell_idx_t GetBestVertex();
    size_t FindBestVertex();

    std::vector<std::list<cell_idx_t>> bucket_;
    std::vector<std::list<cell_idx_t>::iterator> cell_ptrs_;
    long long int cur_max_gain_idx;

    const size_t p_max;
};
#endif //HYPERGRAPHS_PART_T_HPP
