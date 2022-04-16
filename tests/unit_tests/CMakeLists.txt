# Copyright (c) 2014-2022, The Monero Project
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this list of
#    conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice, this list
#    of conditions and the following disclaimer in the documentation and/or other
#    materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors may be
#    used to endorse or promote products derived from this software without specific
#    prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
# THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
# THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

set(unit_tests_sources
  decoy.cpp
  decoy_wrappers.cpp
  main.cpp
#  scaling_2021.cpp
)

set(unit_tests_headers
  decoy_wrappers.h
  unit_tests_utils.h)

monero_add_minimal_executable(unit_tests
  ${unit_tests_sources}
  ${unit_tests_headers})
target_link_libraries(unit_tests
  PRIVATE
 #   ringct
    cryptonote_protocol
    cryptonote_core
#    daemon_messages
#    daemon_rpc_server
#    blockchain_db
#    lmdb_lib
#    rpc
#    net
#    serialization
    wallet
#    p2p
    version
    ${Boost_CHRONO_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${GTEST_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${EXTRA_LIBRARIES}
    ${ZMQ_LIB})
set_property(TARGET unit_tests
  PROPERTY
    FOLDER "tests")

if (NOT MSVC)
  set_property(TARGET unit_tests
    APPEND_STRING
    PROPERTY
      COMPILE_FLAGS " -Wno-undef -Wno-sign-compare")
endif ()

#monero_use_pch_private(unit_tests pch_ut.h)

SET_PROPERTY(SOURCE main.cpp PROPERTY COMPILE_FLAGS -DDEFAULT_DATA_DIR="\\"${CMAKE_SOURCE_DIR}/tests/data\\"")

SET_PROPERTY(SOURCE memwipe.cpp PROPERTY COMPILE_FLAGS -Ofast)

add_test(
  NAME    unit_tests
  COMMAND unit_tests --data-dir "${TEST_DATA_DIR}")

monero_add_minimal_executable(test_notifier test_notifier.cpp)
target_link_libraries(test_notifier ${EXTRA_LIBRARIES})
set_property(TARGET test_notifier PROPERTY FOLDER "tests")
