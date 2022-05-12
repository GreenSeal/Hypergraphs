//
// Created by denis on 24.04.2022.
//

#ifndef HYPERGRAPHS_FIDUCCIAMATTHESISALGO_HPP
#define HYPERGRAPHS_FIDUCCIAMATTHESISALGO_HPP

#include "GraphLoader.hpp"
#include "Part_t.hpp"

#include<cassert>

class FiducciaMatthesisAlgo {
public:

    FiducciaMatthesisAlgo(const std::string& filename_) : graph_loader(filename_), best_A_(), best_B_(), rel_diff(0.3), is_fixed(graph_loader.cells_.size(), 0) { //may be avoid copy here
        for(size_t i = 0; i < graph_loader.cells_.size()/2; ++i) {
            best_A_.insert(graph_loader.cells_.at(i).id);
        }

        for(size_t j = graph_loader.cells_.size()/2; j < graph_loader.cells_.size(); ++j) {
            best_B_.insert(graph_loader.cells_.at(j).id);
        }

        best_cut_cost_ = CutCost(best_A_, best_B_);//do initialize cut here

        //then fill bucket_
    }

    void DoPathStep(part_t& A, part_t& B) { //may be avoid copy here

        int cur_diff_gain = 0;
        int best_diff_gain = 0;

        while(IsNewVertexAvailable(A, B)) {
            cell_t &v = graph_loader.cells_.at(GetBestVertex(A, B));//there we must immediately fix v
            is_fixed.at(v.id) = 1;
            bool from_A_to_b = false;

            if (A.ContainsInSet(v.id)) {
                from_A_to_b = true;
                B.InsertInSet(v.id);
                A.EraseFromSet(v.id);
            } else {
                from_A_to_b = false;
                A.InsertInSet(v.id);
                B.EraseFromSet(v.id);
            }

            for(auto& net:  v.nets){

                if(net->cells.size() == 1) continue;

                size_t num_v_in_A = 0;
                size_t num_v_in_B = 0;

                for(const auto& v : net->cells) {
                    if(A.ContainsInSet(v->id)) ++num_v_in_A;
                    else if(B.ContainsInSet(v->id)) ++num_v_in_B;
                    else throw std::range_error("ERROR: v must lie in A or in B");
                }

                if(num_v_in_A > 2 && num_v_in_B > 2) continue;

                for(auto& cell :  net->cells) {
                    if(!is_fixed.at(cell->id)) {
                        if(from_A_to_b) {
                            A.InsertInSet(v.id);
                            B.EraseFromSet(v.id);
                        } else {
                            B.InsertInSet(v.id);
                            A.EraseFromSet(v.id);
                        }
                        int before_gain = ComputeGainsForNet(A, B, *cell, num_v_in_A, num_v_in_B);

                        if(from_A_to_b) {
                            B.InsertInSet(v.id);
                            A.EraseFromSet(v.id);
                        } else {
                            A.InsertInSet(v.id);
                            B.EraseFromSet(v.id);
                        }
                        int after_gain = ComputeGainsForNet(A, B, *cell, num_v_in_A, num_v_in_B);

                        if(after_gain != before_gain) {
                                cell->gain += after_gain - before_gain;

                                std::cout << "darova nahui\n";

                                if(A.ContainsInSet(cell->id)) {
                                    part_t::UpdateGain(A, cell, after_gain - before_gain);
                                } else if(B.ContainsInSet(cell->id)) {
                                    part_t::UpdateGain(B, cell, after_gain - before_gain);
                                } else {
                                    std::cerr << "ERROR: cell with updating gain doesn't contain nor A neither B\n";
                                }
                        }
                    }
                }
            }


            //I REMOVED is_fixed FLAG BE CAREFUL with checking of visiting vertexes
            //size_t cur_cut_cost = CutCost(A.GetSet(), B.GetSet());

            cur_diff_gain += v.gain;

            if (cur_diff_gain > best_diff_gain) {
                best_A_ = A.GetSet();
                best_B_ = B.GetSet();
                //best_cut_cost_ = cur_cut_cost;
            }

            size_t unfixed_remain = 0;
            for(auto& elt : is_fixed)
                if(elt == 0) ++unfixed_remain;

            if(unfixed_remain%2920 == 0)
                std::cout << "Processing DoPathStep... " << (292008-unfixed_remain)/292008 << "%\n";
        }

        best_cut_cost_ = CutCost(best_A_, best_B_);

    }

//    std::vector<int> ComputeGainsForNet(const part_t& A, const part_t& B, const net_t& net) {
//        std::vector<int> cell_gains{};
//        cell_gains.reserve(net.cells.size());
//
//        size_t num_v_in_A = 0;
//        size_t num_v_in_B = 0;
//
//        if(net.cells.size() == 1) return {0};
//
//        for(const auto& v : net.cells) {
//            if(A.ContainsInSet(v->id)) ++num_v_in_A;
//            else if(B.ContainsInSet(v->id)) ++num_v_in_B;
//            else throw std::range_error("ERROR: v must lie in A or in B");
//        }
//
//        if(num_v_in_A == 1 && num_v_in_B == 1) {
//            for(const auto& cell : net.cells) {
//                if(!is_fixed.at(cell->id))
//                    cell_gains.push_back(1);
//            }
//        } else if(num_v_in_A == 0 || num_v_in_B == 0) {
//            for(const auto& cell: net.cells) {
//                if(!is_fixed.at(cell->id))
//                    cell_gains.push_back(-1);
//            }
//        } else if (num_v_in_B == 1) {
//            for(const auto& cell: net.cells) {
//                if(!is_fixed.at(cell->id)) {
//                    if (B.ContainsInSet(cell->id))
//                        cell_gains.push_back(1);
//                    else
//                        cell_gains.push_back(0);
//                }
//            }
//        } else if(num_v_in_A == 1) {
//            for(const auto& cell: net.cells) {
//                if(!is_fixed.at(cell->id)) {
//                    if (A.ContainsInSet(cell->id))
//                        cell_gains.push_back(1);
//                    else
//                        cell_gains.push_back(0);
//                }
//            }
//        } else {
//            for(const auto& cell: net.cells) {
//                if(!is_fixed.at(cell->id))
//                    cell_gains.push_back(0);
//            }
//        }
//
//        return cell_gains;
//    }

    int ComputeGainsForNet(const part_t& A, const part_t& B, const cell_t& cell, size_t num_v_in_A, size_t num_v_in_B) {
        //cell MUST contains in net
//        size_t num_v_in_A = 0;
//        size_t num_v_in_B = 0;

//        if(net.cells.size() == 1) return 0;

//        for(const auto& v : net.cells) {
//            if(A.ContainsInSet(v->id)) ++num_v_in_A;
//            else if(B.ContainsInSet(v->id)) ++num_v_in_B;
//            else throw std::range_error("ERROR: v must lie in A or in B");
//        }

        if(num_v_in_A == 1 && num_v_in_B == 1) {
            return 1;
        } else if(num_v_in_A == 0 || num_v_in_B == 0) {
            return -1;
        } else if (num_v_in_B == 1) {
            if (B.ContainsInSet(cell.id)) return 1;
            else return 0;
        } else if(num_v_in_A == 1) {
            if (A.ContainsInSet(cell.id)) return 1;
            else return 0;
        } else {
            return 0;
        }
    }

    size_t CutCost(const std::unordered_set<size_t>& A, const std::unordered_set<size_t>& B) {
        size_t cut_cost = 0;
        for(const auto& net : graph_loader.nets_) {
            bool found_v_in_A = false;
            bool found_v_in_B = false;

            for(const auto& cell : net.cells) {
                if(A.contains(cell->id)) found_v_in_A = true;
                if(B.contains(cell->id)) found_v_in_B = true;
            }

            if(found_v_in_A && found_v_in_B) {
                cut_cost += 1;
            }
        }

        return cut_cost;
    }

    bool IsNewVertexAvailable(const part_t& A, const part_t& B) {
        if(A.SetSize()/static_cast<float>(B.SetSize()) < 1-rel_diff) {
            return (!B.IsBucketEmpty())? true : false;
        } else if(B.SetSize()/static_cast<float>(A.SetSize()) < 1-rel_diff) {
            return (!A.IsBucketEmpty())? true: false;
        } else {
            return (!A.IsBucketEmpty() || !B.IsBucketEmpty())? true : false;
        }
    }

    size_t GetBestVertex(part_t& A, part_t& B) {
        if(A.SetSize()/static_cast<float>(B.SetSize()) < 1-rel_diff) {
            if(!B.IsBucketEmpty())
                return B.GetBestVertex();
            else
                throw std::runtime_error("ERROR: trying to get vertex from B, but B is empty");
        } else if(B.SetSize()/static_cast<float>(A.SetSize()) < 1-rel_diff) {
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
                size_t A_best_v = A.GetBestVertex();
                size_t B_best_v = B.GetBestVertex();
                return (graph_loader.cells_.at(A_best_v).gain > graph_loader.cells_.at(B_best_v).gain) ? A_best_v : B_best_v;
            }
        }
    }

    bool IsFixed(size_t id) const {
        return is_fixed.at(id) == 1;
    }

    void run() {
        const size_t pass_num = 1;
        for(size_t i = 0; i < pass_num; ++i) {
            part_t A{graph_loader.nets_.size(), graph_loader.cells_.size(), best_A_};
            part_t B{graph_loader.nets_.size(), graph_loader.cells_.size(), best_B_};
            part_t::FillGain(A, B, graph_loader.nets_, graph_loader.cells_);
            DoPathStep(A, B);
            std::cout << "Processing... " << static_cast<float>(i/pass_num)*100.0 << "% complete\n";
        }

        std::cout << "Balance: " << best_A_.size()/(float)best_B_.size() << "\n";
        std::cout << "CutSize: " << CutCost(best_A_, best_B_) << "\n";
    }

    graph_loader_t graph_loader;

    size_t best_cut_cost_;
    std::unordered_set<size_t> best_A_;
    std::unordered_set<size_t> best_B_;

    std::vector<uint8_t> is_fixed;

    double rel_diff;
};

#endif //HYPERGRAPHS_FIDUCCIAMATTHESISALGO_HPP
