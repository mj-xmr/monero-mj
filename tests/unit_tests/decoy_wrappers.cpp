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
#include "pch_ut.h"
#include "decoy_wrappers.h"
#include "wallet/wallet2.h"

extern "C" {
//#include "crypto/keccak.h"
}

/// Currently, a bad pick is defined as max unsigned
const uint64_t wallet2_wrapper::BAD_PICK = std::numeric_limits<uint64_t>::max();
/// Under this length of the RCT vector, an exception is thrown of too low a length of the rct vector.
const uint64_t wallet2_wrapper::MIN_RCT_LENGTH = CRYPTONOTE_DEFAULT_TX_SPENDABLE_AGE + 1;

wallet2_wrapper::wallet2_wrapper()
: pwallet(std::make_unique<tools::wallet2>())
{
    
}
wallet2_wrapper::~wallet2_wrapper()
{
    
}

void wallet2_wrapper::gamma(int numberRCTs) const
{
    const std::vector<uint64_t> rct_offsets = init_offests(numberRCTs);
    tools::gamma_picker picker(rct_offsets);
}

uint64_t wallet2_wrapper::gamma_pick(int numberRCTs) const
{
    const std::vector<uint64_t> rct_offsets = init_offests(numberRCTs);
    tools::gamma_picker picker(rct_offsets);
    const uint64_t pick = picker.pick();
    return pick;
}

std::vector<uint64_t> wallet2_wrapper::init_offests(int numberRCTs) const
{
    std::vector<uint64_t> rct_offsets;
    for (int i = 0; i < numberRCTs; ++i)
    {
        rct_offsets.push_back(i + 1);
        //rct_offsets.push_back(1); // Should make no difference, since the values aren't really read (?)
    }
    return rct_offsets;
}

//bool wallet2_wrapper::tx_add_fake_output_wrap(std::vector<std::vector<tools::wallet2::get_outs_entry>> &outs, uint64_t global_index, const crypto::public_key& output_public_key, const rct::key& mask, uint64_t real_index, bool unlocked) const
//{
    
//}

// void wallet2::get_outs



    
