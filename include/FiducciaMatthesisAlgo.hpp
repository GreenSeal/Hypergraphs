//
// Created by denis on 24.04.2022.
//

#ifndef HYPERGRAPHS_FIDUCCIAMATTHESISALGO_HPP
#define HYPERGRAPHS_FIDUCCIAMATTHESISALGO_HPP

#include "GraphLoader.hpp"
#include "Part_t.hpp"
#include "CutHolder_t.hpp"

#include<cassert>
#include <chrono>

class FiducciaMatthesisAlgo {
public:

    FiducciaMatthesisAlgo(const std::string& filename_, bool is_mod_);
    void WritePart2File(const std::string &filename);

    void DoPathStep();
    void FillGain();
    int ComputeGainsForNet(const cell_t& cell, size_t num_v_in_A, size_t num_v_in_B);
    size_t CutCost();

    bool IsNewVertexAvailable(const part_t &A, const part_t &B);
    size_t GetBestVertex(part_t& A, part_t& B);

    std::tuple<size_t, size_t, double, double> run();

    graph_loader_t graph_loader;

    bool is_mod;
    cutholder_t cut;
    cutholder_t best_cut;
    std::vector<uint8_t> is_fixed;
    double rel_diff;
};

#endif //HYPERGRAPHS_FIDUCCIAMATTHESISALGO_HPP
