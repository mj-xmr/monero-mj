// Copyright (c) 2017-2021, The Monero Project
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

#ifndef EL_WRITER_BASIC_H
#define EL_WRITER_BASIC_H

#include "easylogging++_base.h"
#include "easylogging++.h"

namespace el
{
namespace base
{

/**
Writer for basic types.
Wraps Writer and delegates the processing there.
*/
class WriterBasic : public IWriterBasic
{
public:
    virtual ~WriterBasic();

    WriterBasic(Level level, Color color, const char* file, base::type::LineNumber line,
                                           const char* func, base::DispatchAction dispatchAction = base::DispatchAction::NormalLog,
                                           base::type::VerboseLevel verboseLevel = 0);

    IWriterBasic& construct(const char *loggerId) override;

    virtual IWriterBasic& operator<<(std::ostream& (*log)(std::ostream&)) override;
    virtual IWriterBasic& operator<<(const std::string & val) override;
    virtual IWriterBasic& operator<<(const char * val) override;
    virtual IWriterBasic& operator<<(void * val) override;
    virtual IWriterBasic& operator<<(unsigned char val) override;
    virtual IWriterBasic& operator<<(signed char val) override;
    virtual IWriterBasic& operator<<(int val) override;
    virtual IWriterBasic& operator<<(long val) override;
    virtual IWriterBasic& operator<<(long long val) override;
    virtual IWriterBasic& operator<<(unsigned val) override;
    virtual IWriterBasic& operator<<(unsigned long val) override;
    virtual IWriterBasic& operator<<(unsigned long long val) override;
    virtual IWriterBasic& operator<<(float val) override;
    virtual IWriterBasic& operator<<(double val) override;
    virtual IWriterBasic& operator<<(long double val) override;

private:
    Writer impl;
};

} // base

} // el

#endif
