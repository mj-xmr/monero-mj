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
//#include "pch_ut.h"
#include "decoy_wrappers.h"
#include "wallet/wallet2.h"

#define GAMMA_SHAPE 19.28
#define GAMMA_SCALE (1/1.61)

extern "C" {
//#include "crypto/keccak.h"
}

/// Currently, a bad pick is defined as max unsigned
const uint64_t wallet2_wrapper::BAD_PICK = std::numeric_limits<uint64_t>::max();
/// Under this length of the RCT vector, an exception is thrown of too low a length of the rct vector.
const uint64_t wallet2_wrapper::MIN_RCT_LENGTH = CRYPTONOTE_DEFAULT_TX_SPENDABLE_AGE + 1;

wallet2_wrapper::wallet2_wrapper()
: pwallet(std::unique_ptr<tools::wallet2>())
{

}
wallet2_wrapper::~wallet2_wrapper()
{

}
namespace tools {
class gamma_picker_my : public gamma_picker
{
public:
    using gamma_picker::gamma_picker;
    void dump_vars(const std::string & filename, const std::string & id) const;
};
}

void wallet2_wrapper::gamma(uint64_t numberRCTs) const
{
    const std::vector<uint64_t> rct_offsets = init_offests(numberRCTs);
    tools::gamma_picker_my picker(rct_offsets);
    picker.dump_vars("", "");
}

uint64_t wallet2_wrapper::gamma_pick(uint64_t numberRCTs) const
{
    const std::vector<uint64_t> rct_offsets = init_offests(numberRCTs);
    tools::gamma_picker_my picker(rct_offsets);
    const uint64_t pick = picker.pick();
    return pick;
}

void wallet2_wrapper::gamma_pick_reinit(const std::vector<uint64_t> & rct_offsets)
{
    //if (ppicker == nullptr)
    {
        ppicker.reset(new tools::gamma_picker_my(rct_offsets));
        //ppicker->dump_vars("", "");
        int a = 0; int b =a ;
    }
}

uint64_t wallet2_wrapper::gamma_pick_inited() const
{
    //return 0;
    const uint64_t pick = ppicker->pick();
    return pick;
}

std::vector<uint64_t> wallet2_wrapper::init_offests(uint64_t numberRCTs) const
{
    std::vector<uint64_t> rct_offsets;
    for (uint64_t i = 0; i < numberRCTs; ++i)
    {
        rct_offsets.push_back(i + 1);
        //rct_offsets.push_back(1); // Should make no difference, since the values aren't really read (?)
    }
    return rct_offsets;
}

static uint64_t xxx = 3;
struct gamma_engine_x
    {
        //gamma_engine_x(uint64_t x)
        //:x (x)
        //{

        //}
        //uint64_t x = 0;
      typedef uint64_t result_type;
      static constexpr result_type min() { return 0; }
      static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }
      //result_type max() { return x; }
      result_type operator()() { return xxx++; }
    } ;

    struct gamma_engine
    {
      typedef uint64_t result_type;
      static constexpr result_type min() { return 0; }
      static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }
      result_type operator()() { return crypto::rand<result_type>(); }
    };

double wallet2_wrapper::gamma_distrib(double x) const
{
    std::gamma_distribution<double> gamma = std::gamma_distribution<double>(GAMMA_SHAPE, GAMMA_SCALE);
    //auto engine = gamma_engine_x(x);
    //auto engine = gamma_engine_x();
    auto engine = gamma_engine();
    return gamma(engine);
}

//bool wallet2_wrapper::tx_add_fake_output_wrap(std::vector<std::vector<tools::wallet2::get_outs_entry>> &outs, uint64_t global_index, const crypto::public_key& output_public_key, const rct::key& mask, uint64_t real_index, bool unlocked) const
//{

//}

// void wallet2::get_outs


namespace tools {
void gamma_picker_my::dump_vars(const std::string & filename, const std::string & id) const
{
    std::cout << "Dumping " << id << " to: " << filename << std::endl;
    std::cout << "rct_offsets.front() = " << rct_offsets.front() << std::endl;
    std::cout << "rct_offsets.back() = " << rct_offsets.back() << std::endl;
    
    std::cout << "rct_offsets.size() = " << rct_offsets.size() << std::endl;
    std::cout << "end - begin = " << end - begin  << std::endl;
    std::cout << "num_rct_outputs = " << num_rct_outputs  << std::endl;
    std::cout << "average_output_time = " << average_output_time  << std::endl << std::endl;
}

}

uint64_t wallet2_wrapper::calculate_fee(bool use_per_byte_fee, const cryptonote::transaction &tx, size_t blob_size, uint64_t base_fee, uint64_t fee_multiplier, uint64_t fee_quantization_mask) const
{
    return calculate_fee_wrap(use_per_byte_fee, tx, blob_size, base_fee, fee_multiplier, fee_quantization_mask);
}

uint64_t wallet2_wrapper::calculate_fee(uint64_t fee_per_kb, size_t bytes, uint64_t fee_multiplier) const
{
    return calculate_fee_wrap(fee_per_kb, bytes, fee_multiplier);
}

uint64_t wallet2_wrapper::calculate_fee_from_weight(uint64_t base_fee, uint64_t weight, uint64_t fee_multiplier, uint64_t fee_quantization_mask) const
{
    return calculate_fee_from_weight_wrap(base_fee, weight, fee_multiplier, fee_quantization_mask);
}
