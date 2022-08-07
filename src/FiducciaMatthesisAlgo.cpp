//
// Created by denist on 8/7/22.
//

#include "FiducciaMatthesisAlgo.hpp"


FiducciaMatthesisAlgo::FiducciaMatthesisAlgo(const std::string& filename_, bool is_mod_)
: graph_loader(filename_), best_cut(), rel_diff(0.3), is_fixed(graph_loader.cells_.size(), 0), is_mod(is_mod_) {
    best_cut.cut.resize(graph_loader.cells_.size());
    // 1 in cut.cut.at(i) means v with idx i lies in part B
    for(size_t i = graph_loader.cells_.size()/2; i < graph_loader.cells_.size(); ++i) {
        best_cut.cut.at(i) = 1;
    }

    best_cut.a_size = graph_loader.cells_.size()/2;
    best_cut.b_size = graph_loader.cells_.size() - best_cut.a_size;

    cut = best_cut;

    //then fill bucket_
}

void FiducciaMatthesisAlgo::WritePart2File(const std::string &filename) {
    std::ofstream part2_stream{filename + ".part.2"};
    for(auto elt: best_cut.cut) {
        part2_stream << elt << std::endl;
    }
}

void FiducciaMatthesisAlgo::DoPathStep() {

    cut = best_cut;
    FillGain();
    part_t A{graph_loader.cells_, graph_loader.nets_.size(), 0, cut.cut};
    part_t B{graph_loader.cells_, graph_loader.nets_.size(), 1, cut.cut};
    std::for_each(is_fixed.begin(), is_fixed.end(), [](uint8_t& elt){elt = 0;});

    int cur_diff_gain = 0;
    int best_diff_gain = 0;

    while(IsNewVertexAvailable(A, B)) {
        cell_t &v = graph_loader.cells_.at(GetBestVertex(A, B));//there we must immediately fix v
        is_fixed.at(v.id) = 1;

        cut.cut.at(v.id) = !cut.cut.at(v.id);
        if(cut.cut.at(v.id)) {
            cut.b_size += 1;
            cut.a_size -= 1;
        } else {
            cut.a_size += 1;
            cut.b_size -= 1;
        }


        for(auto& net:  v.nets) {

            if(net->cells.size() == 1) continue;

            size_t num_v_in_A = 0;
            size_t num_v_in_B = 0;

            for(const auto& v : net->cells) {
                if(!cut.cut.at(v->id)) ++num_v_in_A;
                else if(cut.cut.at(v->id)) ++num_v_in_B;
            }

            if(num_v_in_A > 2 && num_v_in_B > 2) continue;

            for(auto& cell :  net->cells) {
                if(!is_fixed.at(cell->id)) {
                    if(cut.cut.at(v.id) == 0) {
                        num_v_in_A -= 1;
                        num_v_in_B += 1;
                    } else {
                        num_v_in_A += 1;
                        num_v_in_B -= 1;
                    }
                    cut.cut.at(v.id) = !cut.cut.at(v.id);
                    int before_gain = ComputeGainsForNet(*cell, num_v_in_A, num_v_in_B);

                    if(cut.cut.at(v.id) == 0) {
                        num_v_in_A -= 1;
                        num_v_in_B += 1;
                    } else {
                        num_v_in_A += 1;
                        num_v_in_B -= 1;
                    }
                    cut.cut.at(v.id) = !cut.cut.at(v.id);
                    int after_gain = ComputeGainsForNet(*cell, num_v_in_A, num_v_in_B);

                    if(after_gain != before_gain) {
                        //std::cout << "BAM\n";

                        if(!cut.cut.at(cell->id)) {
                            A.UpdateGain(cell, after_gain - before_gain, is_mod);
                        } else {
                            B.UpdateGain(cell, after_gain - before_gain, is_mod);
                        }

                        cell->gain += after_gain - before_gain;
                    }
                }
            }
        }
        //I REMOVED is_fixed FLAG BE CAREFUL with checking of visiting vertexes
        cur_diff_gain += v.gain;

        if (cur_diff_gain > best_diff_gain) {
            best_cut = cut;
            best_diff_gain = cur_diff_gain;
        }
    }
}

void FiducciaMatthesisAlgo::FillGain() {
    //There has a place to be conversion from bucket idx to gain, or more precisely, gain = idx - p_max

    for(auto&& cell: graph_loader.cells_) {
        cell.gain = 0;
    }

    for(const auto& net : graph_loader.nets_) {

        if(net.cells.size() == 1) continue;

        size_t num_v_in_A = 0;
        size_t num_v_in_B = 0;
        std::vector<cell_t>::pointer last_v_in_A = nullptr;
        std::vector<cell_t>::pointer last_v_in_B = nullptr;

        for(auto cell : net.cells) {
            if(!cut.cut.at(cell->id)) {
                ++num_v_in_A;
                last_v_in_A = cell;
            } else if (cut.cut.at(cell->id)) {
                ++num_v_in_B;
                last_v_in_B = cell;
            } else {
                std::cerr << "ERROR: bucket_t: input vertex doesn't belong A or B\n";
                throw std::exception();
            }
        }

        if (num_v_in_A == 1 && num_v_in_B == 1) {
            for(auto cell : net.cells) {
                cell->gain += 1;
            }
        } else if(num_v_in_B == 0 || num_v_in_A == 0) {
            for(auto cell : net.cells) {
                cell->gain += -1;
            }
        } else if(num_v_in_B == 1) {
            last_v_in_B->gain += 1;
        } else if(num_v_in_A == 1) {
            last_v_in_A->gain += 1;
        }
    }
}

int FiducciaMatthesisAlgo::ComputeGainsForNet(const cell_t& cell, size_t num_v_in_A, size_t num_v_in_B) {
    //cell MUST contains in net

    if(num_v_in_A == 1 && num_v_in_B == 1) {
        return 1;
    } else if(num_v_in_A == 0 || num_v_in_B == 0) {
        return -1;
    } else if (num_v_in_B == 1) {
        if (cut.cut.at(cell.id)) return 1;
        else return 0;
    } else if(num_v_in_A == 1) {
        if (!cut.cut.at(cell.id)) return 1;
        else return 0;
    } else {
        return 0;
    }
}

size_t FiducciaMatthesisAlgo::CutCost() {
    size_t cut_cost = 0;
    for(const auto& net : graph_loader.nets_) {
        bool found_v_in_A = false;
        bool found_v_in_B = false;

        for(const auto& cell : net.cells) {
            if(!best_cut.cut.at(cell->id)) found_v_in_A = true;
            if(best_cut.cut.at(cell->id)) found_v_in_B = true;
            if(found_v_in_A && found_v_in_B) break;
        }

        if(found_v_in_A && found_v_in_B) {
            cut_cost += 1;
        }
    }

    return cut_cost;
}

bool FiducciaMatthesisAlgo::IsNewVertexAvailable(const part_t &A, const part_t &B) {
    if(cut.a_size/static_cast<float>(cut.b_size) < 1-rel_diff) {
        return !B.IsBucketEmpty();
    } else if(cut.b_size/static_cast<float>(cut.a_size) < 1-rel_diff) {
        return !A.IsBucketEmpty();
    } else {
        return (!A.IsBucketEmpty() || !B.IsBucketEmpty());
    }
}

size_t FiducciaMatthesisAlgo::GetBestVertex(part_t& A, part_t& B) {
    if(cut.a_size/static_cast<float>(cut.b_size) < 1-rel_diff) {
        if(!B.IsBucketEmpty())
            return B.GetBestVertex();
        else
            throw std::runtime_error("ERROR: trying to get vertex from B, but B is empty");
    } else if(cut.b_size/static_cast<float>(cut.a_size) < 1-rel_diff) {
        if(!A.IsBucketEmpty())
            return A.GetBestVertex();
        else
            throw std::runtime_error("ERROR: trying to get vertex from A, but A is empty");
    } else {
        if(A.IsBucketEmpty()) {
            return B.GetBestVertex();
        } else if(B.IsBucketEmpty()) {
            return A.GetBestVertex();
        } else {
            size_t A_best_v = A.FindBestVertex();
            size_t B_best_v = B.FindBestVertex();
            return (graph_loader.cells_.at(A_best_v).gain > graph_loader.cells_.at(B_best_v).gain) ? \
            A.GetBestVertex() : B.GetBestVertex();
        }
    }
}

std::tuple<size_t, size_t, double, double> FiducciaMatthesisAlgo::run() {

    size_t start_cutcost = CutCost();
    const size_t pass_num = 5;
    auto start = std::chrono::steady_clock::now();

    for(size_t i = 0; i < pass_num; ++i) {
        DoPathStep();
        std::cout << "Processing... " << (i+1)/static_cast<float>(pass_num)*100.0 << "% complete\n";
    }

    auto finish = std::chrono::steady_clock::now();
    double time = std::chrono::duration<double>(finish-start).count();
    size_t finish_cutcost = CutCost();
    double reduced = (start_cutcost - finish_cutcost)/(double)start_cutcost;

    return std::tuple<size_t, size_t, double, double>(start_cutcost, finish_cutcost, reduced, time);
}
