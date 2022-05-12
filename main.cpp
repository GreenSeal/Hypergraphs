#include <iostream>
#include "FiducciaMatthesisAlgo.hpp"

int main(int argc, char** argv) {

    size_t start_cutcost, finish_cutcost;
    double time, reduced;
    bool is_mod;

    if(argc < 2) {
        throw std::runtime_error("ERROR: Too little input arguments\n");
    } else if(argc == 2) {
        is_mod = false;
        FiducciaMatthesisAlgo alg(argv[1]);
        std::tie(start_cutcost, finish_cutcost, reduced, time) = alg.run();
        alg.graph_loader.WriteToFile(alg.best_A_, alg.best_B_);
    } else if (argc == 3) {
        if(std::string(argv[2]) == "-m") {
            is_mod = true;
            FiducciaMatthesisAlgo alg(argv[1], true);
            std::tie(start_cutcost, finish_cutcost, reduced, time) = alg.run();
            alg.graph_loader.WriteToFile(alg.best_A_, alg.best_B_);
        } else {
            throw std::runtime_error("ERROR: Invalid second argument to function\n");
        }
    } else {
        throw std::runtime_error("ERROR: Too many input arguments\n");
    }

    std::string filename(argv[1]);
    std::string prefix = (is_mod)? ".mod" : "";
    auto save_file = std::ofstream{filename + prefix + ".result"};
    save_file << start_cutcost << std::endl;
    save_file << finish_cutcost << std::endl;
    save_file << reduced << std::endl;
    save_file << time << std::endl;
}
