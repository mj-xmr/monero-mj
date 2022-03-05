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

//#include "checkpoints/checkpoints.h"
#include "cryptonote_config.h"
#include "cryptonote_basic/fwd.h"
#include "cryptonote_core/i_core_events.h"
#include "cryptonote_basic/i_miner.h"
#include "cryptonote_protocol/enums.h"
#include "common/command_line.h"
#include "cryptonote_basic/i_miner.h"
#include "warnings.h"
#include "crypto/hash.h"
#include "crypto/crypto.h"
#include "span.h"
#include "rpc/fwd.h"
//#include "rpc/message_data_structs.h"
//#include "rpc/core_rpc_server_commands_defs.h"

#include <boost/function.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/variables_map.hpp>

#include <ctime>

PUSH_WARNINGS
DISABLE_VS_WARNINGS(4355)

enum { HAVE_BLOCK_MAIN_CHAIN, HAVE_BLOCK_ALT_CHAIN, HAVE_BLOCK_INVALID };

namespace cryptonote
{
    class checkpoints;
    class miner;
    
    struct tx_blob_entry;
    struct tx_backlog_entry;
    struct block_complete_entry;
    struct tx_info;
    struct spent_key_image_info;
    
    struct tx_verification_context;
    
    struct i_cryptonote_protocol;
    struct cryptonote_connection_context;
  class Blockchain;
   enum class relay_category : uint8_t;
   namespace rpc
   {
       struct tx_in_pool;
       typedef std::unordered_map<crypto::key_image, std::vector<crypto::hash> > key_images_with_tx_hashes;
   }
   
   struct test_options {
     const std::pair<uint8_t, uint64_t> *hard_forks;
     const size_t long_term_block_weight_window;
   };
   
   typedef std::function<const epee::span<const unsigned char>(cryptonote::network_type network)> GetCheckpointsCallback;

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

  /************************************************************************/
  /*                                                                      */
  /************************************************************************/

   /**
    * @brief handles core cryptonote functionality
    *
    * This class coordinates cryptonote functionality including, but not
    * limited to, communication among the Blockchain, the transaction pool,
    * any miners, and the network.
    */
   class core_abstract: public i_miner_handler, public i_core_events
   {
   public:
       static std::unique_ptr<core_abstract> create(i_cryptonote_protocol* pprotoco);
    /**
     * @copydoc Blockchain::handle_get_objects
     *
     * @note see Blockchain::handle_get_objects()
     * @param context connection context associated with the request
     */
     virtual bool handle_get_objects(NOTIFY_REQUEST_GET_OBJECTS_request_t& arg, NOTIFY_RESPONSE_GET_OBJECTS_request_t& rsp, cryptonote_connection_context& context) = 0;

     /**
      * @brief calls various idle routines
      *
      * @note see miner::on_idle and tx_memory_pool::on_idle
      *
      * @return true
      */
     virtual bool on_idle() = 0;

     /**
      * @brief handles an incoming transaction
      *
      * Parses an incoming transaction and, if nothing is obviously wrong,
      * passes it along to the transaction pool
      *
      * @param tx_blob the tx to handle
      * @param tvc metadata about the transaction's validity
      * @param tx_relay how the transaction was received
      * @param relayed whether or not the transaction was relayed to us
      *
      * @return true if the transaction was accepted, false otherwise
      */
     virtual bool handle_incoming_tx(const tx_blob_entry& tx_blob, tx_verification_context& tvc, relay_method tx_relay, bool relayed) = 0;

     /**
      * @brief handles a list of incoming transactions
      *
      * Parses incoming transactions and, if nothing is obviously wrong,
      * passes them along to the transaction pool
      *
      * @pre `tx_blobs.size() == tvc.size()`
      *
      * @param tx_blobs the txs to handle
      * @param tvc metadata about the transactions' validity
      * @param tx_relay how the transaction was received.
      * @param relayed whether or not the transactions were relayed to us
      *
      * @return true if the transactions were accepted, false otherwise
      */
     virtual bool handle_incoming_txs(epee::span<const tx_blob_entry> tx_blobs, epee::span<tx_verification_context> tvc, relay_method tx_relay, bool relayed) = 0;

     /**
      * @brief handles a list of incoming transactions
      *
      * Parses incoming transactions and, if nothing is obviously wrong,
      * passes them along to the transaction pool
      *
      * @param tx_blobs the txs to handle
      * @param tvc metadata about the transactions' validity
      * @param tx_relay how the transaction was received.
      * @param relayed whether or not the transactions were relayed to us
      *
      * @return true if the transactions were accepted, false otherwise
      */
     virtual bool handle_incoming_txs(const std::vector<tx_blob_entry>& tx_blobs, std::vector<tx_verification_context>& tvc, relay_method tx_relay, bool relayed) = 0;
     /**
      * @brief handles an incoming block
      *
      * periodic update to checkpoints is triggered here
      * Attempts to add the block to the Blockchain and, on success,
      * optionally updates the miner's block template.
      *
      * @param block_blob the block to be added
      * @param block the block to be added, or NULL
      * @param bvc return-by-reference metadata context about the block's validity
      * @param update_miner_blocktemplate whether or not to update the miner's block template
      *
      * @return false if loading new checkpoints fails, or the block is not
      * added, otherwise true
      */
     virtual bool handle_incoming_block(const blobdata& block_blob, const block *b, block_verification_context& bvc, bool update_miner_blocktemplate = true) = 0;

     /**
      * @copydoc Blockchain::prepare_handle_incoming_blocks
      *
      * @note see Blockchain::prepare_handle_incoming_blocks
      */
     virtual bool prepare_handle_incoming_blocks(const std::vector<block_complete_entry> &blocks_entry, std::vector<block> &blocks) = 0;

     /**
      * @copydoc Blockchain::cleanup_handle_incoming_blocks
      *
      * @note see Blockchain::cleanup_handle_incoming_blocks
      */
     virtual bool cleanup_handle_incoming_blocks(bool force_sync = false) = 0;
     	     	
     /**
      * @brief check the size of a block against the current maximum
      *
      * @param block_blob the block to check
      *
      * @return whether or not the block is too big
      */
     virtual bool check_incoming_block_size(const blobdata& block_blob) const = 0;

     /**
      * @brief get the cryptonote protocol instance
      *
      * @return the instance
      */
     virtual i_cryptonote_protocol* get_protocol() = 0;

     /**
      * @copydoc Blockchain::get_miner_data
      *
      * @note see Blockchain::get_miner_data
      */
     virtual bool get_miner_data(uint8_t& major_version, uint64_t& height, crypto::hash& prev_id, crypto::hash& seed_hash, difficulty_type& difficulty, uint64_t& median_weight, uint64_t& already_generated_coins, std::vector<tx_block_template_backlog_entry>& tx_backlog) = 0;

     /**
      * @brief called when a transaction is relayed.
      * @note Should only be invoked from `levin_notify`.
      */
     virtual void on_transactions_relayed(epee::span<const cryptonote::blobdata> tx_blobs, relay_method tx_relay) = 0;


     /**
      * @brief gets the miner instance
      *
      * @return a reference to the miner instance
      */
     virtual miner& get_miner() = 0;

     /**
      * @brief gets the miner instance (const)
      *
      * @return a const reference to the miner instance
      */
     virtual const miner& get_miner() const = 0;

     /**
      * @brief adds command line options to the given options set
      *
      * As of now, there are no command line options specific to core,
      * so this function simply returns.
      *
      * @param desc return-by-reference the command line options set to add to
      */
     static void init_options(boost::program_options::options_description& desc);

     /**
      * @brief initializes the core as needed
      *
      * This function initializes the transaction pool, the Blockchain, and
      * a miner instance with parameters given on the command line (or defaults)
      *
      * @param vm command line parameters
      * @param test_options configuration options for testing
      * @param get_checkpoints if set, will be called to get checkpoints data, must return checkpoints data pointer and size or nullptr if there ain't any checkpoints for specific network type
      * @param allow_dns whether or not to allow DNS requests
      *
      * @return false if one of the init steps fails, otherwise true
      */
     virtual bool init(const boost::program_options::variables_map& vm, const test_options *test_options = NULL, const GetCheckpointsCallback& get_checkpoints = nullptr, bool allow_dns = true) = 0;

     /**
      * @copydoc Blockchain::reset_and_set_genesis_block
      *
      * @note see Blockchain::reset_and_set_genesis_block
      */
     virtual bool set_genesis_block(const block& b) = 0;

     /**
      * @brief performs safe shutdown steps for core and core components
      *
      * Uninitializes the miner instance, transaction pool, and Blockchain
      *
      * @return true
      */
     virtual bool deinit() = 0;

     /**
      * @brief sets to drop blocks downloaded (for testing)
      */
     virtual void test_drop_download() = 0;

     /**
      * @brief sets to drop blocks downloaded below a certain height
      *
      * @param height height below which to drop blocks
      */
     virtual void test_drop_download_height(uint64_t height) = 0;

     /**
      * @brief gets whether or not to drop blocks (for testing)
      *
      * @return whether or not to drop blocks
      */
     virtual bool get_test_drop_download() const = 0;

     /**
      * @brief gets whether or not to drop blocks
      *
      * If the current blockchain height <= our block drop threshold
      * and test drop blocks is set, return true
      *
      * @return see above
      */
     virtual bool get_test_drop_download_height() const = 0;

     /**
      * @copydoc Blockchain::get_current_blockchain_height
      *
      * @note see Blockchain::get_current_blockchain_height()
      */
     virtual uint64_t get_current_blockchain_height() const = 0;

     /**
      * @brief get the hash and height of the most recent block
      *
      * @param height return-by-reference height of the block
      * @param top_id return-by-reference hash of the block
      */
     virtual void get_blockchain_top(uint64_t& height, crypto::hash& top_id) const = 0;

     /**
      * @copydoc Blockchain::get_blocks(uint64_t, size_t, std::vector<std::pair<cryptonote::blobdata,block>>&, std::vector<transaction>&) const
      *
      * @note see Blockchain::get_blocks(uint64_t, size_t, std::vector<std::pair<cryptonote::blobdata,block>>&, std::vector<transaction>&) const
      */
     virtual bool get_blocks(uint64_t start_offset, size_t count, std::vector<std::pair<cryptonote::blobdata,block>>& blocks, std::vector<cryptonote::blobdata>& txs) const = 0;

     /**
      * @copydoc Blockchain::get_blocks(uint64_t, size_t, std::vector<std::pair<cryptonote::blobdata,block>>&) const
      *
      * @note see Blockchain::get_blocks(uint64_t, size_t, std::vector<std::pair<cryptonote::blobdata,block>>&) const
      */
     virtual bool get_blocks(uint64_t start_offset, size_t count, std::vector<std::pair<cryptonote::blobdata,block>>& blocks) const = 0;

     /**
      * @copydoc Blockchain::get_blocks(uint64_t, size_t, std::vector<std::pair<cryptonote::blobdata,block>>&) const
      *
      * @note see Blockchain::get_blocks(uint64_t, size_t, std::vector<std::pair<cryptonote::blobdata,block>>&) const
      */
     virtual bool get_blocks(uint64_t start_offset, size_t count, std::vector<block>& blocks) const = 0;

     /**
      * @copydoc Blockchain::get_blocks(const t_ids_container&, t_blocks_container&, t_missed_container&) const
      *
      * @note see Blockchain::get_blocks(const t_ids_container&, t_blocks_container&, t_missed_container&) const
      */
     //template<class t_ids_container, class t_blocks_container, class t_missed_container>
     //bool get_blocks(const t_ids_container& block_ids, t_blocks_container& blocks, t_missed_container& missed_bs) const;
     /**
      * @copydoc Blockchain::get_block_id_by_height
      *
      * @note see Blockchain::get_block_id_by_height
      */
     virtual crypto::hash get_block_id_by_height(uint64_t height) const = 0;

     /**
      * @copydoc Blockchain::get_transactions
      *
      * @note see Blockchain::get_transactions
      */
     virtual bool get_transactions(const std::vector<crypto::hash>& txs_ids, std::vector<cryptonote::blobdata>& txs, std::vector<crypto::hash>& missed_txs, bool pruned = false) const = 0;

     /**
      * @copydoc Blockchain::get_transactions
      *
      * @note see Blockchain::get_transactions
      */
     virtual bool get_split_transactions_blobs(const std::vector<crypto::hash>& txs_ids, std::vector<std::tuple<crypto::hash, cryptonote::blobdata, crypto::hash, cryptonote::blobdata>>& txs, std::vector<crypto::hash>& missed_txs) const = 0;

     /**
      * @copydoc Blockchain::get_transactions
      *
      * @note see Blockchain::get_transactions
      */
     virtual bool get_transactions(const std::vector<crypto::hash>& txs_ids, std::vector<transaction>& txs, std::vector<crypto::hash>& missed_txs, bool pruned = false) const = 0;

     /**
      * @copydoc Blockchain::get_block_by_hash
      *
      * @note see Blockchain::get_block_by_hash
      */
     virtual bool get_block_by_hash(const crypto::hash &h, block &blk, bool *orphan = NULL) const = 0;

     /**
      * @copydoc Blockchain::get_alternative_blocks
      *
      * @note see Blockchain::get_alternative_blocks(std::vector<block>&) const
      */
     virtual bool get_alternative_blocks(std::vector<block>& blocks) const = 0;

     /**
      * @copydoc Blockchain::get_alternative_blocks_count
      *
      * @note see Blockchain::get_alternative_blocks_count() const
      */
     virtual size_t get_alternative_blocks_count() const = 0;

     /**
      * @brief set the pointer to the cryptonote protocol object to use
      *
      * @param pprotocol the pointer to set ours as
      */
     virtual void set_cryptonote_protocol(i_cryptonote_protocol* pprotocol) = 0;

     /**
      * @copydoc Blockchain::set_checkpoints
      *
      * @note see Blockchain::set_checkpoints()
      */
     virtual void set_checkpoints(checkpoints&& chk_pts) = 0;

     /**
      * @brief set the file path to read from when loading checkpoints
      *
      * @param path the path to set ours as
      */
     virtual void set_checkpoints_file_path(const std::string& path) = 0;

     /**
      * @brief set whether or not we enforce DNS checkpoints
      *
      * @param enforce_dns enforce DNS checkpoints or not
      */
     virtual void set_enforce_dns_checkpoints(bool enforce_dns) = 0;

     /**
      * @brief set a listener for txes being added to the txpool
      *
      * @param callable to notify, or empty function to disable.
      */
     virtual void set_txpool_listener(boost::function<void(std::vector<txpool_event>)> zmq_pub) = 0;

     /**
      * @brief set whether or not to enable or disable DNS checkpoints
      *
      * @param disble whether to disable DNS checkpoints
      */
     virtual void disable_dns_checkpoints(bool disable = true) = 0;

     /**
      * @copydoc tx_memory_pool::have_tx
      *
      * @note see tx_memory_pool::have_tx
      */
     virtual bool pool_has_tx(const crypto::hash &txid) const = 0;

     /**
      * @copydoc tx_memory_pool::get_transactions
      * @param include_sensitive_txes include private transactions
      *
      * @note see tx_memory_pool::get_transactions
      */
     virtual bool get_pool_transactions(std::vector<transaction>& txs, bool include_sensitive_txes = false) const = 0;

     /**
      * @copydoc tx_memory_pool::get_txpool_backlog
      * @param include_sensitive_txes include private transactions
      *
      * @note see tx_memory_pool::get_txpool_backlog
      */
     virtual bool get_txpool_backlog(std::vector<tx_backlog_entry>& backlog, bool include_sensitive_txes = false) const = 0;
     
     /**
      * @copydoc tx_memory_pool::get_transactions
      * @param include_sensitive_txes include private transactions
      *
      * @note see tx_memory_pool::get_transactions
      */
     virtual bool get_pool_transaction_hashes(std::vector<crypto::hash>& txs, bool include_sensitive_txes = false) const = 0;

     /**
      * @copydoc tx_memory_pool::get_transactions
      * @param include_sensitive_txes include private transactions
      *
      * @note see tx_memory_pool::get_transactions
      */
     virtual bool get_pool_transaction_stats(struct txpool_stats& stats, bool include_sensitive_txes = false) const = 0;

     /**
      * @copydoc tx_memory_pool::get_transaction
      *
      * @note see tx_memory_pool::get_transaction
      */
     virtual bool get_pool_transaction(const crypto::hash& id, cryptonote::blobdata& tx, relay_category tx_category) const = 0;

     /**
      * @copydoc tx_memory_pool::get_pool_transactions_and_spent_keys_info
      * @param include_sensitive_txes include private transactions
      *
      * @note see tx_memory_pool::get_pool_transactions_and_spent_keys_info
      */
     virtual bool get_pool_transactions_and_spent_keys_info(std::vector<tx_info>& tx_infos, std::vector<spent_key_image_info>& key_image_infos, bool include_sensitive_txes = false) const = 0;

     /**
      * @copydoc tx_memory_pool::get_pool_for_rpc
      *
      * @note see tx_memory_pool::get_pool_for_rpc
      */
     virtual bool get_pool_for_rpc(std::vector<cryptonote::rpc::tx_in_pool>& tx_infos, cryptonote::rpc::key_images_with_tx_hashes& key_image_infos) const = 0;

     /**
      * @copydoc tx_memory_pool::get_transactions_count
      * @param include_sensitive_txes include private transactions
      *
      * @note see tx_memory_pool::get_transactions_count
      */
     virtual size_t get_pool_transactions_count(bool include_sensitive_txes = false) const = 0;

     /**
      * @copydoc Blockchain::get_total_transactions
      *
      * @note see Blockchain::get_total_transactions
      */
     virtual size_t get_blockchain_total_transactions() const = 0;

     /**
      * @copydoc Blockchain::have_block
      *
      * @note see Blockchain::have_block
      */
     virtual bool have_block_unlocked(const crypto::hash& id, int *where = NULL) const = 0;
     virtual bool have_block(const crypto::hash& id, int *where = NULL) const = 0;

     /**
      * @copydoc Blockchain::get_short_chain_history
      *
      * @note see Blockchain::get_short_chain_history
      */
     virtual bool get_short_chain_history(std::list<crypto::hash>& ids) const = 0;

     /**
      * @copydoc Blockchain::find_blockchain_supplement(const std::list<crypto::hash>&, NOTIFY_RESPONSE_CHAIN_ENTRY::request&) const
      *
      * @note see Blockchain::find_blockchain_supplement(const std::list<crypto::hash>&, NOTIFY_RESPONSE_CHAIN_ENTRY::request&) const
      */
     virtual bool find_blockchain_supplement(const std::list<crypto::hash>& qblock_ids, bool clip_pruned, NOTIFY_RESPONSE_CHAIN_ENTRY_request_t& resp) const = 0;

     /**
      * @copydoc Blockchain::find_blockchain_supplement(const uint64_t, const std::list<crypto::hash>&, std::vector<std::pair<cryptonote::blobdata, std::vector<cryptonote::blobdata> > >&, uint64_t&, uint64_t&, size_t) const
      *
      * @note see Blockchain::find_blockchain_supplement(const uint64_t, const std::list<crypto::hash>&, std::vector<std::pair<cryptonote::blobdata, std::vector<transaction> > >&, uint64_t&, uint64_t&, size_t) const
      */
     virtual bool find_blockchain_supplement(const uint64_t req_start_block, const std::list<crypto::hash>& qblock_ids, std::vector<std::pair<std::pair<cryptonote::blobdata, crypto::hash>, std::vector<std::pair<crypto::hash, cryptonote::blobdata> > > >& blocks, uint64_t& total_height, uint64_t& start_height, bool pruned, bool get_miner_tx_hash, size_t max_block_count, size_t max_tx_count) const = 0;

     /**
      * @copydoc Blockchain::get_tx_outputs_gindexs
      *
      * @note see Blockchain::get_tx_outputs_gindexs
      */
     virtual bool get_tx_outputs_gindexs(const crypto::hash& tx_id, std::vector<uint64_t>& indexs) const = 0;
     virtual bool get_tx_outputs_gindexs(const crypto::hash& tx_id, size_t n_txes, std::vector<std::vector<uint64_t>>& indexs) const = 0;

     /**
      * @copydoc Blockchain::get_tail_id
      *
      * @note see Blockchain::get_tail_id
      */
     virtual crypto::hash get_tail_id() const = 0;

     /**
      * @copydoc Blockchain::get_block_cumulative_difficulty
      *
      * @note see Blockchain::get_block_cumulative_difficulty
      */
     virtual difficulty_type get_block_cumulative_difficulty(uint64_t height) const = 0;

     /**
      * @copydoc Blockchain::get_outs
      *
      * @note see Blockchain::get_outs
      */
     virtual bool get_outs(const COMMAND_RPC_GET_OUTPUTS_BIN_request_t& req, COMMAND_RPC_GET_OUTPUTS_BIN_response_t& res) const = 0;

     /**
      * @copydoc Blockchain::get_output_distribution
      *
      * @brief get per block distribution of outputs of a given amount
      */
     virtual bool get_output_distribution(uint64_t amount, uint64_t from_height, uint64_t to_height, uint64_t &start_height, std::vector<uint64_t> &distribution, uint64_t &base) const = 0;

     /**
      * @copydoc miner::pause
      *
      * @note see miner::pause
      */
     virtual void pause_mine() = 0;

     /**
      * @copydoc miner::resume
      *
      * @note see miner::resume
      */
     virtual void resume_mine() = 0;

     /**
      * @brief gets the Blockchain instance
      *
      * @return a reference to the Blockchain instance
      */
     virtual Blockchain& get_blockchain_storage() = 0;

     /**
      * @brief gets the Blockchain instance (const)
      *
      * @return a const reference to the Blockchain instance
      */
     virtual const Blockchain& get_blockchain_storage() const = 0;

     /**
      * @copydoc tx_memory_pool::print_pool
      *
      * @note see tx_memory_pool::print_pool
      */
     virtual std::string print_pool(bool short_format) const = 0;

     /**
      * @brief gets the core synchronization status
      *
      * @return core synchronization status
      */
     virtual bool is_synchronized() const = 0;

     /**
      * @copydoc miner::on_synchronized
      *
      * @note see miner::on_synchronized
      */
     virtual void on_synchronized() = 0;

     /**
      * @copydoc Blockchain::safesyncmode
      *
      * 2note see Blockchain::safesyncmode
      */
     virtual void safesyncmode(const bool onoff) = 0;

     /**
      * @brief sets the target blockchain height
      *
      * @param target_blockchain_height the height to set
      */
     virtual void set_target_blockchain_height(uint64_t target_blockchain_height) = 0;

     /**
      * @brief gets the target blockchain height
      *
      * @param target_blockchain_height the target height
      */
     virtual uint64_t get_target_blockchain_height() const = 0;

     /**
      * @brief returns the newest hardfork version known to the blockchain
      *
      * @return the version
      */
     virtual uint8_t get_ideal_hard_fork_version() const = 0;

     /**
      * @brief return the ideal hard fork version for a given block height
      *
      * @return what it says above
      */
     virtual uint8_t get_ideal_hard_fork_version(uint64_t height) const = 0;

     /**
      * @brief return the hard fork version for a given block height
      *
      * @return what it says above
      */
     virtual uint8_t get_hard_fork_version(uint64_t height) const = 0;

     /**
      * @brief return the earliest block a given version may activate
      *
      * @return what it says above
      */
     virtual uint64_t get_earliest_ideal_height_for_version(uint8_t version) const = 0;

     /**
      * @brief gets start_time
      *
      */
     virtual std::time_t get_start_time() const = 0;

     /**
      * @brief tells the Blockchain to update its checkpoints
      *
      * This function will check if enough time has passed since the last
      * time checkpoints were updated and tell the Blockchain to update
      * its checkpoints if it is time.  If updating checkpoints fails,
      * the daemon is told to shut down.
      *
      * @note see Blockchain::update_checkpoints()
      */
     virtual bool update_checkpoints(const bool skip_dns = false) = 0;

     /**
      * @brief tells the daemon to wind down operations and stop running
      *
      * Currently this function raises SIGTERM, allowing the installed signal
      * handlers to do the actual stopping.
      */
     virtual void graceful_exit() = 0;

     /**
      * @brief stops the daemon running
      *
      * @note see graceful_exit()
      */
     virtual void stop() = 0;

     /**
      * @copydoc Blockchain::have_tx_keyimg_as_spent
      *
      * @note see Blockchain::have_tx_keyimg_as_spent
      */
     virtual bool is_key_image_spent(const crypto::key_image& key_im) const = 0;

     /**
      * @brief check if multiple key images are spent
      *
      * plural version of is_key_image_spent()
      *
      * @param key_im list of key images to check
      * @param spent return-by-reference result for each image checked
      *
      * @return true
      */
     virtual bool are_key_images_spent(const std::vector<crypto::key_image>& key_im, std::vector<bool> &spent) const = 0;

     /**
      * @brief check if multiple key images are spent in the transaction pool
      *
      * @param key_im list of key images to check
      * @param spent return-by-reference result for each image checked
      *
      * @return true
      */
     virtual bool are_key_images_spent_in_pool(const std::vector<crypto::key_image>& key_im, std::vector<bool> &spent) const = 0;

     /**
      * @brief get the number of blocks to sync in one go
      *
      * @return the number of blocks to sync in one go
      */
     virtual size_t get_block_sync_size(uint64_t height) const = 0;

     /**
      * @brief get the sum of coinbase tx amounts between blocks
      *
      * @return the number of blocks to sync in one go
      */
     virtual std::pair<boost::multiprecision::uint128_t, boost::multiprecision::uint128_t> get_coinbase_tx_sum(const uint64_t start_offset, const size_t count) = 0;
     
     /**
      * @brief get the network type we're on
      *
      * @return which network are we on?
      */     
     virtual network_type get_nettype() const = 0;

     /**
      * @brief check whether an update is known to be available or not
      *
      * This does not actually trigger a check, but returns the result
      * of the last check
      *
      * @return whether an update is known to be available or not
      */
     virtual bool is_update_available() const = 0;

     /**
      * @brief get whether fluffy blocks are enabled
      *
      * @return whether fluffy blocks are enabled
      */
     virtual bool fluffy_blocks_enabled() const = 0;

     /**
      * @brief check a set of hashes against the precompiled hash set
      *
      * @return number of usable blocks
      */
     virtual uint64_t prevalidate_block_hashes(uint64_t height, const std::vector<crypto::hash> &hashes, const std::vector<uint64_t> &weights) = 0;

     /**
      * @brief get free disk space on the blockchain partition
      *
      * @return free space in bytes
      */
     virtual uint64_t get_free_space() const = 0;

     /**
      * @brief get whether the core is running offline
      *
      * @return whether the core is running offline
      */
     virtual bool offline() const = 0;

     /**
      * @brief get the blockchain pruning seed
      *
      * @return the blockchain pruning seed
      */
     virtual uint32_t get_blockchain_pruning_seed() const = 0;

     /**
      * @brief prune the blockchain
      *
      * @param pruning_seed the seed to use to prune the chain (0 for default, highly recommended)
      *
      * @return true iff success
      */
     virtual bool prune_blockchain(uint32_t pruning_seed = 0) = 0;

     /**
      * @brief incrementally prunes blockchain
      *
      * @return true on success, false otherwise
      */
     virtual bool update_blockchain_pruning() = 0;

     /**
      * @brief checks the blockchain pruning if enabled
      *
      * @return true on success, false otherwise
      */
     virtual bool check_blockchain_pruning() = 0;

     /**
      * @brief checks whether a given block height is included in the precompiled block hash area
      *
      * @param height the height to check for
      */
     virtual bool is_within_compiled_block_hash_area(uint64_t height) const = 0;

     /**
      * @brief checks whether block weights are known for the given range
      */
     virtual bool has_block_weights(uint64_t height, uint64_t nblocks) const = 0;

     /**
      * @brief flushes the bad txs cache
      */
     virtual void flush_bad_txs_cache() = 0;

     /**
      * @brief flushes the invalid block cache
      */
     virtual void flush_invalid_blocks() = 0;

     /**
      * @brief returns the set of transactions in the txpool which are not in the argument
      *
      * @param hashes hashes of transactions to exclude from the result
      *
      * @return true iff success, false otherwise
      */
     virtual bool get_txpool_complement(const std::vector<crypto::hash> &hashes, std::vector<cryptonote::blobdata> &txes) = 0;

   private:
   };
}

POP_WARNINGS
