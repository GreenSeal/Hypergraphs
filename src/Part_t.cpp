//
// Created by denist on 8/7/22.
//

#include "Part_t.hpp"

part_t::part_t(const std::vector<cell_t> &cells, size_t nets_size, const bool is_B,
       const std::vector<size_t> &cut) : bucket_(2*nets_size+1), cell_ptrs_(cells.size()), p_max(nets_size) {

    cur_max_gain_idx = 0;

    for(size_t id = 0; id < cut.size(); ++id) {
        if((!is_B && !cut.at(id)) || (is_B && cut.at(id))) {
            if(cells.at(id).gain + p_max > cur_max_gain_idx) {
                cur_max_gain_idx = cells.at(id).gain + p_max;
            }

            bucket_.at(cells.at(id).gain + p_max).emplace_back(id);
            cell_ptrs_.at(id) = std::prev(bucket_.at(cells.at(id).gain+p_max).end());
        }
    }
}

void part_t::UpdateGain(std::vector<cell_t>::pointer cell, int diff, bool is_mod) { //add check if cell is in bucket
    assert(cell != nullptr && "Nullptr cell in function UpdateGain");
    if(std::abs(diff) > 2) {
        throw std::range_error("diff unexpectedly large");
    }

    size_t id = cell->id;
    int gain = cell->gain;

    if(is_mod) {
        bucket_.at(gain+p_max+diff).splice(bucket_.at(gain+p_max+diff).end(),
                                           bucket_.at(gain+p_max), cell_ptrs_.at(id));
    } else {
        bucket_.at(gain+p_max+diff).splice(bucket_.at(gain+p_max+diff).begin(),
                                           bucket_.at(gain+p_max+diff), cell_ptrs_.at(id));
    }
    if(gain + p_max + diff > cur_max_gain_idx) {
        cur_max_gain_idx = gain + p_max + diff;
    } else if(bucket_.at(gain+p_max).empty()) {
        if(diff == -1) {
            cur_max_gain_idx = gain + p_max + diff;
        } else if((diff == -2) && !bucket_.at(gain+p_max-1).empty()) {
            cur_max_gain_idx = gain + p_max - 1;
        } else if((diff == -2) && bucket_.at(gain+p_max-1).empty()) {
            cur_max_gain_idx = gain + p_max + diff;
        }

    }
}

size_t part_t::FindBestVertex() {
    return bucket_.at(cur_max_gain_idx).back();
}

bool part_t::IsBucketEmpty() const {
    return cur_max_gain_idx == -1;
}

part_t::cell_idx_t part_t::GetBestVertex() {
    cell_idx_t idx = bucket_.at(cur_max_gain_idx).back();
    bucket_.at(cur_max_gain_idx).pop_back();
    cell_ptrs_.at(idx) = bucket_.at(cur_max_gain_idx).end();
    if(bucket_.at(cur_max_gain_idx).begin() == bucket_.at(cur_max_gain_idx).end()) {
        while((bucket_.at(cur_max_gain_idx).begin() == bucket_.at(cur_max_gain_idx).end())) {
            cur_max_gain_idx -= 1;
            if(cur_max_gain_idx == -1) {
                break;
            }
        }
    }
    return idx;
}
