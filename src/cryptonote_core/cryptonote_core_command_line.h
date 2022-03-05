// Copyright (c) 2014-2020, The Monero Project
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
//
// Parts of this file are originally copyright (c) 2012-2013 The Cryptonote developers

#pragma once

#include "cryptonote_basic/difficulty.h"
#include "common/command_line.h"

namespace cryptonote
{
  extern const command_line::arg_descriptor<std::string, false, true, 2> arg_data_dir;
  extern const command_line::arg_descriptor<bool, false> arg_testnet_on;
  extern const command_line::arg_descriptor<bool, false> arg_stagenet_on;
  extern const command_line::arg_descriptor<bool, false> arg_regtest_on;
  extern const command_line::arg_descriptor<difficulty_type> arg_fixed_difficulty;
  extern const command_line::arg_descriptor<bool> arg_offline;
  extern const command_line::arg_descriptor<size_t> arg_block_download_max_size;
  extern const command_line::arg_descriptor<bool> arg_sync_pruned_blocks;
  extern const command_line::arg_descriptor<bool> arg_keep_fakechain;
  extern const command_line::arg_descriptor<bool> arg_disable_dns_checkpoints;
  extern const command_line::arg_descriptor<bool> arg_test_drop_download;
  extern const command_line::arg_descriptor<uint64_t> arg_test_drop_download_height;
  extern const command_line::arg_descriptor<int> arg_test_dbg_lock_sleep;
  extern const command_line::arg_descriptor<bool> arg_dns_checkpoints;
  extern const command_line::arg_descriptor<uint64_t> arg_fast_block_sync;
  extern const command_line::arg_descriptor<uint64_t> arg_prep_blocks_threads;
  extern const command_line::arg_descriptor<uint64_t> arg_show_time_stats;
  extern const command_line::arg_descriptor<size_t> arg_block_sync_size;
  extern const command_line::arg_descriptor<std::string> arg_check_updates;
  extern const command_line::arg_descriptor<bool> arg_fluffy_blocks;
  extern const command_line::arg_descriptor<bool> arg_no_fluffy_blocks;
  extern const command_line::arg_descriptor<size_t> arg_max_txpool_weight;
  extern const command_line::arg_descriptor<std::string> arg_block_notify;
  extern const command_line::arg_descriptor<bool> arg_prune_blockchain;
  extern const command_line::arg_descriptor<std::string> arg_reorg_notify;
  extern const command_line::arg_descriptor<std::string> arg_block_rate_notify;
  extern const command_line::arg_descriptor<bool> arg_keep_alt_blocks;
}
