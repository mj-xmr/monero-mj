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

#include <boost/range/adaptor/transformed.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "common/command_line.h"
#include "common/varint.h"
#include "cryptonote_core/tx_pool.h"
#include "cryptonote_core/cryptonote_core.h"
#include "cryptonote_core/blockchain.h"
#include "blockchain_db/blockchain_db.h"
#include "version.h"
#include "string_tools.h"

#undef MONERO_DEFAULT_LOG_CATEGORY
#define MONERO_DEFAULT_LOG_CATEGORY "bcutil"

namespace po = boost::program_options;
using namespace epee;
using namespace cryptonote;

size_t estimate_rct_tx_size(int n_inputs, int mixin, int n_outputs, size_t extra_size, bool bulletproof)
{
	size_t size = 0;
	
	// tx prefix
	
	// first few bytes
	size += 1 + 6;
	
	// vin
	size += n_inputs * (1+6+(mixin+1)*2+32);
	
	// vout
	size += n_outputs * (6+32);
	
	// extra
	size += extra_size;
	
	// rct signatures
	
	// type
	size += 1;
	
	// rangeSigs
	if (bulletproof)
	{
		size_t log_padded_outputs = 0;
		while ((1<<log_padded_outputs) < n_outputs)
			++log_padded_outputs;
		size += (2 * (6 + log_padded_outputs) + 4 + 5) * 32 + 3;
	}
	else
		size += (2*64*32+32+64*32) * n_outputs;
	
	// MGs
	size += n_inputs * (64 * (mixin+1) + 32);
	
	// mixRing - not serialized, can be reconstructed
	/* size += 2 * 32 * (mixin+1) * n_inputs; */
	
	// pseudoOuts
	size += 32 * n_inputs;
	// ecdhInfo
	size += 2 * 32 * n_outputs;
	// outPk - only commitment is saved
	size += 32 * n_outputs;
	// txnFee
	size += 4;
	
	LOG_PRINT_L2("estimated " << (bulletproof ? "bulletproof" : "borromean") << " rct tx size for " << n_inputs << " inputs with ring size " << (mixin+1) << " and " << n_outputs << " outputs: " << size << " (" << ((32 * n_inputs/*+1*/) + 2 * 32 * (mixin+1) * n_inputs + 32 * n_outputs) << " saved)");
	return size;
}
size_t estimate_tx_size(bool use_rct, int n_inputs, int mixin, int n_outputs, size_t extra_size, bool bulletproof)
{
	if (use_rct)
		return estimate_rct_tx_size(n_inputs, mixin, n_outputs, extra_size, bulletproof);
  return 0;
}
uint64_t estimate_tx_weight(bool use_rct, int n_inputs, int mixin, int n_outputs, size_t extra_size, bool bulletproof)
{
	size_t size = estimate_tx_size(use_rct, n_inputs, mixin, n_outputs, extra_size, bulletproof);
	if (use_rct && bulletproof && n_outputs > 2)
	{
		const uint64_t bp_base = 368;
		size_t log_padded_outputs = 2;
		while ((1<<log_padded_outputs) < n_outputs)
			++log_padded_outputs;
		uint64_t nlr = 2 * (6 + log_padded_outputs);
		const uint64_t bp_size = 32 * (9 + nlr);
		const uint64_t bp_clawback = (bp_base * (1<<log_padded_outputs) - bp_size) * 4 / 5;
		MDEBUG("clawback on size " << size << ": " << bp_clawback);
		size += bp_clawback;
	}
	return size;
}
uint64_t calculate_fee_from_weight(uint64_t base_fee, uint64_t weight, uint64_t fee_multiplier, uint64_t fee_quantization_mask)
{
	uint64_t fee = weight * base_fee * fee_multiplier;
	fee = (fee + fee_quantization_mask - 1) / fee_quantization_mask * fee_quantization_mask;
	return fee;
}
uint64_t calculate_fee(bool use_per_byte_fee, const cryptonote::transaction &tx, size_t blob_size, uint64_t base_fee, uint64_t fee_multiplier, uint64_t fee_quantization_mask)
{
	if (use_per_byte_fee) {
		return calculate_fee_from_weight(base_fee, cryptonote::get_transaction_weight(tx, blob_size), fee_multiplier, fee_quantization_mask);
	}
  return 0;
}
uint64_t estimate_fee(bool use_per_byte_fee, bool use_rct, int n_inputs, int mixin, int n_outputs, size_t extra_size, bool bulletproof, uint64_t base_fee, uint64_t fee_multiplier, uint64_t fee_quantization_mask)
{
	if (use_per_byte_fee)
	{
		const size_t estimated_tx_weight = estimate_tx_weight(use_rct, n_inputs, mixin, n_outputs, extra_size, bulletproof);
		return calculate_fee_from_weight(base_fee, estimated_tx_weight, fee_multiplier, fee_quantization_mask);
	}
  return 0;
}

struct output_data
{
  uint64_t amount;
  uint64_t index;
  mutable bool coinbase;
  mutable uint64_t height;
  output_data(uint64_t a, uint64_t i, bool cb, uint64_t h): amount(a), index(i), coinbase(cb), height(h) {}
  bool operator==(const output_data &other) const { return other.amount == amount && other.index == index; }
  void info(bool c, uint64_t h) const { coinbase = c; height =h; }
};
namespace std
{
  template<> struct hash<output_data>
  {
    size_t operator()(const output_data &od) const
    {
      const uint64_t data[2] = {od.amount, od.index};
      crypto::hash h;
      crypto::cn_fast_hash(data, 2 * sizeof(uint64_t), h);
      return reinterpret_cast<const std::size_t &>(h);
    }
  };
}

struct reference
{
  uint64_t height;
  uint64_t ring_size;
  uint64_t position;
  reference(uint64_t h, uint64_t rs, uint64_t p): height(h), ring_size(rs), position(p) {}
};

int main(int argc, char* argv[])
{
  TRY_ENTRY();

  epee::string_tools::set_module_name_and_folder(argv[0]);

  uint32_t log_level = 0;

  tools::on_startup();

  boost::filesystem::path output_file_path;

  po::options_description desc_cmd_only("Command line options");
  po::options_description desc_cmd_sett("Command line options and settings options");
  const command_line::arg_descriptor<std::string> arg_log_level  = {"log-level",  "0-4 or categories", ""};
  const command_line::arg_descriptor<bool> arg_rct_only  = {"rct-only", "Only work on ringCT outputs", false};
  const command_line::arg_descriptor<std::string> arg_input = {"input", ""};

  command_line::add_arg(desc_cmd_sett, cryptonote::arg_testnet_on);
  command_line::add_arg(desc_cmd_sett, cryptonote::arg_stagenet_on);
  command_line::add_arg(desc_cmd_sett, arg_log_level);
  command_line::add_arg(desc_cmd_sett, arg_rct_only);
  command_line::add_arg(desc_cmd_sett, arg_input);
  command_line::add_arg(desc_cmd_only, command_line::arg_help);

  po::options_description desc_options("Allowed options");
  desc_options.add(desc_cmd_only).add(desc_cmd_sett);

  po::positional_options_description positional_options;
  positional_options.add(arg_input.name, -1);

  po::variables_map vm;
  bool r = command_line::handle_error_helper(desc_options, [&]()
  {
    auto parser = po::command_line_parser(argc, argv).options(desc_options).positional(positional_options);
    po::store(parser.run(), vm);
    po::notify(vm);
    return true;
  });
  if (! r)
    return 1;

  if (command_line::get_arg(vm, command_line::arg_help))
  {
    std::cout << "Monero '" << MONERO_RELEASE_NAME << "' (v" << MONERO_VERSION_FULL << ")" << ENDL << ENDL;
    std::cout << desc_options << std::endl;
    return 1;
  }

  mlog_configure(mlog_get_default_log_path("monero-blockchain-usage.log"), true);
  if (!command_line::is_arg_defaulted(vm, arg_log_level))
    mlog_set_log(command_line::get_arg(vm, arg_log_level).c_str());
  else
    mlog_set_log(std::string(std::to_string(log_level) + ",bcutil:INFO").c_str());

  LOG_PRINT_L0("Starting...");

  bool opt_testnet = command_line::get_arg(vm, cryptonote::arg_testnet_on);
  bool opt_stagenet = command_line::get_arg(vm, cryptonote::arg_stagenet_on);
  network_type net_type = opt_testnet ? TESTNET : opt_stagenet ? STAGENET : MAINNET;
  bool opt_rct_only = command_line::get_arg(vm, arg_rct_only);

  // If we wanted to use the memory pool, we would set up a fake_core.

  // Use Blockchain instead of lower-level BlockchainDB for two reasons:
  // 1. Blockchain has the init() method for easy setup
  // 2. exporter needs to use get_current_blockchain_height(), get_block_id_by_height(), get_block_by_hash()
  //
  // cannot match blockchain_storage setup above with just one line,
  // e.g.
  //   Blockchain* core_storage = new Blockchain(NULL);
  // because unlike blockchain_storage constructor, which takes a pointer to
  // tx_memory_pool, Blockchain's constructor takes tx_memory_pool object.
  LOG_PRINT_L0("Initializing source blockchain (BlockchainDB)");
  const std::string input = command_line::get_arg(vm, arg_input);
  std::unique_ptr<Blockchain> core_storage;
  tx_memory_pool m_mempool(*core_storage);
  core_storage.reset(new Blockchain(m_mempool));
  BlockchainDB* db = new_db();
  if (db == NULL)
  {
    LOG_ERROR("Failed to initialize a database");
    throw std::runtime_error("Failed to initialize a database");
  }
  LOG_PRINT_L0("database: LMDB");

  const std::string filename = input;
  LOG_PRINT_L0("Loading blockchain from folder " << filename << " ...");

  try
  {
    db->open(filename, DBF_RDONLY);
  }
  catch (const std::exception& e)
  {
    LOG_PRINT_L0("Error opening database: " << e.what());
    return 1;
  }
  r = core_storage->init(db, net_type);

  CHECK_AND_ASSERT_MES(r, 1, "Failed to initialize source blockchain storage");
  LOG_PRINT_L0("Source blockchain storage initialized OK");

  crypto::hash txid;
  epee::string_tools::hex_to_pod("d1ec19b652af6679082a1a2a27aeec4c9230ca33262759f1c120e8a1c8ac26f1", txid);
  cryptonote::transaction tx = core_storage->get_db().get_tx(txid);

  printf("The transaction's fee stored on chain is: %lu\n\n", tx.rct_signatures.txnFee);

  // the MyMonero server returned these values at the time I constructed the tx
  uint64_t base_fee = 4587;
  uint64_t fee_mask = 10000;

  bool use_per_byte_fee = true;
  bool use_rct = true;
  uint64_t default_mixin = 11 - 1;
  bool bulletproof = true;
  uint64_t fee_multiplier = 1;
  uint64_t extra_size = 0; // for some reason, extra.size() here is 0: https://github.com/mymonero/mymonero-core-cpp/blob/a53e57f2a376b05bb0f4d851713321c749e5d8d9/src/monero_transfer_utils.cpp#L277
  uint64_t estimated_fee = estimate_fee(use_per_byte_fee, use_rct, 2, default_mixin, 2, extra_size, bulletproof, base_fee, fee_multiplier, fee_mask);

  printf("The estimated fee that MyMonero would calculate for this transaction is: %lu\n\n", estimated_fee);

  if (estimated_fee == tx.rct_signatures.txnFee)
    printf("MyMonero must have used the estimated fee, therefore this is very likely the code that is being used by MyMonero in production (whatever code is being used has this bug in it regardless).\n");

  LOG_PRINT_L0("Done");
  return 0;

  CATCH_ENTRY("Export error", 1);
}
