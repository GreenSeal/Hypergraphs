//
// Created by denis on 24.04.2022.
//

#ifndef HYPERGRAPHS_CELL_T_HPP
#define HYPERGRAPHS_CELL_T_HPP

#include<limits>

struct net_t;

struct cell_t {
    explicit cell_t(uint32_t id_, uint32_t weight_ = 1) : id(id_), weight(weight_), nets(), gain(0) {}

    std::vector<std::vector<net_t>::pointer> nets;
    size_t id;
    int gain;
    size_t weight;
};

#endif //HYPERGRAPHS_CELL_T_HPP
