#include <iostream>
#include "GraphLoader.hpp"
#include "FiducciaMatthesisAlgo.hpp"

int main() {
    FiducciaMatthesisAlgo alg("D:\\benchmark_set\\3Dspectralwave2.mtx.hgr");
    alg.graph_loader.WriteToFile();
    alg.run();
}
