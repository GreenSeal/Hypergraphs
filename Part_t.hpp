//
// Created by denis on 09.05.2022.
//

#ifndef HYPERGRAPHS_PART_T_HPP
#define HYPERGRAPHS_PART_T_HPP

#include <list>
#include <cassert>
#include <unordered_set>
#include <iostream>

class part_t {
public:

    using cell_idx_t = size_t;

    part_t(size_t nets_size, size_t cells_size, std::unordered_set<size_t> cells_set = {}) : cells_(cells_set), bucket_(2*nets_size+1),
    cell_ptrs_(cells_size), p_max(nets_size) {
        for(auto cell_id : cells_set) {
            InsertInBucket(cell_id);
        }
    }

    static void FillGain(part_t& A, part_t& B, const std::vector<net_t>& nets, const std::vector<cell_t>& cells, bool is_mod) {
        //There has a place to be conversion from bucket idx to gain, or more precisely, gain = idx - p_max

        for(const auto& net : nets) {

            if(net.cells.size() == 1) continue;

            size_t num_v_in_A = 0;
            size_t num_v_in_B = 0;
            std::vector<cell_t>::pointer last_v_in_A = nullptr;
            std::vector<cell_t>::pointer last_v_in_B = nullptr;

            for(auto cell : net.cells) {
                if(A.ContainsInSet(cell->id)) {
                    ++num_v_in_A;
                    last_v_in_A = cell;
                } else if (B.ContainsInSet(cell->id)) {
                    ++num_v_in_B;
                    last_v_in_B = cell;
                } else {
                    std::cerr << "ERROR: bucket_t: input vertex doesn't belong A or B\n";
                    throw std::exception();
                }
            }

            if (num_v_in_A == 1 && num_v_in_B == 1) {
                for(auto cell : net.cells) {
                    if(A.ContainsInSet(cell->id))
                        UpdateGain(A, cell, 1, is_mod);
                    else
                        UpdateGain(B, cell, 1, is_mod);
                }
            } else if(num_v_in_B == 0 || num_v_in_A == 0) {
                for(auto cell : net.cells) {
                    if(A.ContainsInSet(cell->id))
                        UpdateGain(A, cell, -1, is_mod);
                    else
                        UpdateGain(B, cell, -1, is_mod);
                }
            } else if(num_v_in_B == 1) {
                UpdateGain(B, last_v_in_B, 1, is_mod);
            } else if(num_v_in_A == 1) {
                UpdateGain(A, last_v_in_A, 1, is_mod);
            }
        }
    }

    static void UpdateGain(part_t& part, std::vector<cell_t>::pointer cell, int sign_of_diff, bool is_mod) { //add check if cell is in bucket
        assert(cell != nullptr && "Nullptr cell in function UpdateGain");
        cell_idx_t idx = cell->id;
        int gain = cell->gain;
        if(is_mod) {
            part.bucket_.at(gain + part.p_max).erase(part.cell_ptrs_.at(idx));
            part.bucket_.at(gain + part.p_max + sign_of_diff).push_back(idx);
            part.cell_ptrs_.at(idx) = std::prev(part.bucket_.at(gain + part.p_max + sign_of_diff).end());
        } else {
            part.bucket_.at(gain + part.p_max).erase(part.cell_ptrs_.at(idx));
            part.bucket_.at(gain + part.p_max + sign_of_diff).push_front(idx);
            part.cell_ptrs_.at(idx) = part.bucket_.at(gain + part.p_max + sign_of_diff).begin();
        }
        cell -> gain += sign_of_diff;
    }

    size_t SetSize() const {
        return cells_.size();
    }

    bool IsBucketEmpty() const {
        bool is_bucket_empty = true;
        for(const auto& elt: bucket_) {
            if(!elt.empty()) {
                is_bucket_empty = false;
                break;
            }
        }
        return is_bucket_empty;
    }

    void InsertInSet(cell_idx_t id) {
        cells_.insert(id);
    }

    void EraseFromSet(cell_idx_t id) {
        cells_.erase(id);
    }

    bool ContainsInSet(cell_idx_t id) const {
        return cells_.contains(id);
    }



    void InsertInBucket(cell_idx_t id) {
        bucket_.at(p_max).push_back(id);
        cell_ptrs_.at(id) = std::prev(bucket_.at(p_max).end());
    }

    cell_idx_t GetBestVertex() {
        for(size_t best_gain = bucket_.size()-1; best_gain > 0; --best_gain) {
            if (!bucket_.at(best_gain).empty()) {
                cell_idx_t cell_idx = bucket_.at(best_gain).back();
                bucket_.at(best_gain).pop_back();
                cell_ptrs_.at(cell_idx) = bucket_.at(best_gain).end();
                return cell_idx;
            }
        }


        // for best_gain = 0
        if (!bucket_.at(0).empty()) {
            cell_idx_t cell_idx = bucket_.at(0).back();
            bucket_.at(0).pop_back();
            cell_ptrs_.at(cell_idx) = bucket_.at(0).end();
            return cell_idx;
        }

        throw std::runtime_error("ERROR: end of bucket, but trying to get best vertex\n");
    }

    std::unordered_set<size_t> GetSet() const {
        return cells_;
    }

    std::unordered_set<size_t> cells_;
    std::vector<std::list<cell_idx_t>> bucket_;
    std::vector<std::list<cell_idx_t>::iterator> cell_ptrs_;

    size_t p_max;
};
#endif //HYPERGRAPHS_PART_T_HPP
