// Copyright (c) 2014-2022, The Monero Project
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

#pragma once

#define forwardable_wrapper_operator_bool(OPER) \
bool operator OPER (const forwardable_wrapper & other) const \
{ \
    return val OPER other(); \
} \

#define forwardable_wrapper_operator_assignment(OPER) \
forwardable_wrapper & operator OPER (const forwardable_wrapper & other) \
{ \
    val OPER other(); \
    return *this; \
} \

#define forwardable_wrapper_operator_mathop(OPER) \
forwardable_wrapper operator OPER (const forwardable_wrapper & other) const \
{ \
    return val OPER other(); \
} \

namespace cryptonote
{    
    /**
    A wrapper of the TUnderlying, that can be forward-declared.
    */
    template <class TUnderlying>
    class forwardable_wrapper
    {
    public:
        using value_type = TUnderlying;
        
        forwardable_wrapper(){}
        template <class TForeign>
        forwardable_wrapper(const TForeign & typeForeign)
        {
            val = TUnderlying(typeForeign);
        }
        virtual ~forwardable_wrapper(){}
        
        const TUnderlying & operator ()() const
        {
            return val;
        }
        TUnderlying & operator ()()
        {
            return val;
        }
        
        forwardable_wrapper_operator_bool(==)
        forwardable_wrapper_operator_bool(!=)
        forwardable_wrapper_operator_bool(>)
        forwardable_wrapper_operator_bool(<)
        forwardable_wrapper_operator_bool(>=)
        forwardable_wrapper_operator_bool(<=)
        
        operator bool() const
        {
            return val ? true : false; 
        }

    protected:
        TUnderlying val;
    private:
        
    };
    
    //std::ostream & operator<<(std::ostream & ostr, const forwardable_wrapper & diff);
}
