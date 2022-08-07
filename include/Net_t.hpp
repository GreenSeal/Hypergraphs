//
// Created by denis on 24.04.2022.
//

#ifndef HYPERGRAPHS_NET_T_HPP
#define HYPERGRAPHS_NET_T_HPP

struct cell_t;

struct net_t {
    explicit net_t(size_t id_, size_t weight_ = 1) : id(id_), weight(weight_), cells() {}

    std::vector<std::vector<cell_t>::pointer> cells;
    size_t id;
    size_t weight;
};

#endif //HYPERGRAPHS_NET_T_HPP
