// Copyright (c) 2018-2022, The Monero Project

// 
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification, are
// permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this list of
//    conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice, this list
//    of conditions and the following disclaimer in the documentation and/or other
//    materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
// THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <memory>
#include <vector>

namespace tools
{
    class wallet2;
    //class gamma_picker;
    class gamma_picker_my;
}

namespace cryptonote
{
    class transaction;
}

class wallet2_wrapper
{
public:
    wallet2_wrapper();
    virtual ~wallet2_wrapper();
    
    void gamma(uint64_t numberRCTs) const;
    uint64_t gamma_pick(uint64_t numberRCTs) const;
    void gamma_pick_reinit(const std::vector<uint64_t> & numberRCTs);
    uint64_t gamma_pick_inited() const;
    std::vector<uint64_t> init_offests(uint64_t numberRCTs) const;
    double gamma_distrib(double x) const;
    
    //bool tx_add_fake_output_wrap(std::vector<std::vector<tools::wallet2::get_outs_entry>> &outs, uint64_t global_index, const crypto::public_key& output_public_key, const rct::key& mask, uint64_t real_index, bool unlocked) const;
    
    static const uint64_t BAD_PICK;
    static const uint64_t MIN_RCT_LENGTH;
    
    
    uint64_t calculate_fee(bool use_per_byte_fee, const cryptonote::transaction &tx, size_t blob_size, uint64_t base_fee, uint64_t fee_multiplier, uint64_t fee_quantization_mask) const;
    uint64_t calculate_fee(uint64_t fee_per_kb, size_t bytes, uint64_t fee_multiplier) const;
    uint64_t calculate_fee_from_weight(uint64_t base_fee, uint64_t weight, uint64_t fee_multiplier, uint64_t fee_quantization_mask) const;

    
private:
    std::unique_ptr<tools::wallet2> pwallet;
    std::unique_ptr<tools::gamma_picker_my> ppicker;
};