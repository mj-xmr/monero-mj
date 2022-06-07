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
#include "cryptonote_config.h"
#include "gtest/gtest.h"

#include <iostream>
#include <iomanip>
#include <fstream>

/*
Each test is accompanied with the previous decision lines and last line, which is reached by this decision branch.
Currently referenced tag = v0.17.3.0
*/

namespace {
    void run_picker(int minMul, int maxMul, int numDraws = 100, const std::string & fileNameOut = "")
{
    // https://github.com/monero-project/monero/blob/v0.17.3.0/src/wallet/wallet2.cpp#L1079
    /*
    Statistically probe how often the picks are good at which multiplier of the MIN_RCT_LENGTH
    */
    const int NUM_DRAWS = numDraws;
    //const char * fileNameOut  = "/tmp/mrl_mul_2_ratio_good.csv";
    std::ofstream fout;
    if (fileNameOut.size())
    {
        fout = std::ofstream(fileNameOut);
        fout << "# multiplier_of_the_minimal_vector_length" << "," << "ratio_good_picks" << '\n';
    }
    //for (double mul = 1e5; mul >= 1; mul *= 0.85) /// TODO: This has to go to Python impl.
    for (double mul = maxMul; mul >= minMul; mul *= 0.85)
    {
        int num_hits = 0;
        for (int i = 0; i < NUM_DRAWS; ++i)
        {
            //continue;
            const uint64_t pick = wallet2_wrapper().gamma_pick(wallet2_wrapper::MIN_RCT_LENGTH * mul);
            if (pick != wallet2_wrapper::BAD_PICK)
            {
                ++num_hits;
            }
        }
        const double ratio_good_picks = num_hits / double(NUM_DRAWS);
        if (fout.is_open())
        {
            fout << mul << "," << ratio_good_picks << '\n';
        }
        std::cout << "mul = " << mul << ",\tRatio good 2 all = " << ratio_good_picks << std::endl;
    }
    std::cout << "Num draws = " << NUM_DRAWS << std::endl;
    if (fout.is_open())
    {
        std::cout << "Data stored in = " << fileNameOut << std::endl;
    }
}

}

TEST(decoy, gamma_lessEqual_than_spendable_age_Throws)
{
    // https://github.com/monero-project/monero/blob/v0.17.3.0/src/wallet/wallet2.cpp#L1020
    EXPECT_ANY_THROW(wallet2_wrapper().gamma(wallet2_wrapper::MIN_RCT_LENGTH - 1));
}

TEST(decoy, gamma_more_than_spendable_age_constructOK)
{
    // https://github.com/monero-project/monero/blob/v0.17.3.0/src/wallet/wallet2.cpp#L1029
    EXPECT_NO_THROW(wallet2_wrapper().gamma(wallet2_wrapper::MIN_RCT_LENGTH));
}

TEST(decoy, gamma_more_than_spendable_age_badPick)
{
    // https://github.com/monero-project/monero/blob/v0.17.3.0/src/wallet/wallet2.cpp#L1053
    // https://github.com/monero-project/monero/blob/v0.17.3.0/src/wallet/wallet2.cpp#L1067
    const uint64_t pick = wallet2_wrapper().gamma_pick(wallet2_wrapper::MIN_RCT_LENGTH);
    ASSERT_EQ(pick, wallet2_wrapper::BAD_PICK);
}

TEST(decoy, gamma_more_than_spendable_age_goodPickAtLeastOnce)
{
    // https://github.com/monero-project/monero/blob/v0.17.3.0/src/wallet/wallet2.cpp#L1079
    const int NUM_DRAWS = 50;
    for (double mul = 10; mul <= 1e4; mul *= 1.40)
    {
        for (int i = 0; i < NUM_DRAWS; ++i)
        {
            const uint64_t pick = wallet2_wrapper().gamma_pick(wallet2_wrapper::MIN_RCT_LENGTH * mul);
            if (pick != wallet2_wrapper::BAD_PICK)
            {
                return; // Success: Found at least one good pick
            }
        }
    }
    FAIL(); // If we reached here, it means we haven't found any good picks, but we should have.
}

// Note: This is technically NOT a unit test, because it's too intense.
TEST(decoy, gamma_more_than_spendable_age_goodPickStatistical)
{
    // https://github.com/monero-project/monero/blob/v0.17.3.0/src/wallet/wallet2.cpp#L1079
    /*
    Statistically probe how often the picks are good at which multiplier of the MIN_RCT_LENGTH
    */
    const int NUM_DRAWS = 100;
    const char * fileNameOut = "/tmp/mrl_mul_2_ratio_good.csv";
    run_picker(50, 1e5, NUM_DRAWS, fileNameOut);    // This matches the Python implementation
    //run_picker(1, 1e5, NUM_DRAWS, fileNameOut);   // What it should be
}

TEST(decoy, gamma_test)
{
    // https://github.com/monero-project/monero/blob/v0.17.3.0/src/wallet/wallet2.cpp#L1079
    /*
    A corner case for the Python version, where anything below 50 crashes the calculations
    */
    const int NUM_DRAWS = 100;
    run_picker(1, 50, NUM_DRAWS);
}

TEST(decoy, gamma_export_distrib)
{
    const int NUM_DRAWS = 5000;
    const char * fileNameOut  = "/tmp/mrl_gamma_distrib.csv";
    std::ofstream fout(fileNameOut);
    for (double i = 0; i <= NUM_DRAWS ; ++i)
    {
        //std::cout << "i = " << i << std::endl;
        const double val = wallet2_wrapper().gamma_distrib(i);
        fout << val << '\n';
        //std::cout << "i = " << i << ", gamma = " << val << std::endl;
    }
    std::cout << "Gamma Data stored in = " << fileNameOut << std::endl;
}

