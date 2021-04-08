//
//  Bismillah ar-Rahmaan ar-Raheem
//
//  Easylogging++ v9.96.7
//  Cross-platform logging library for C++ applications
//
//  Copyright (c) 2012-2018 Zuhd Web Services
//  Copyright (c) 2012-2018 @abumusamq
//
//  This library is released under the MIT Licence.
//  https://github.com/zuhd-org/easyloggingpp/blob/master/LICENSE
//
//  https://zuhd.org
//  http://muflihun.com
//

#include "easylogging++_base.h"
#include "easylogging++.h"
#include "WriterBasic.h"

#include <sstream>
#include <string>

namespace el
{
namespace base
{

WriterBasic::WriterBasic(Level level, Color color, const char* file, base::type::LineNumber line,
                         const char* func, base::DispatchAction dispatchAction,
                         base::type::VerboseLevel verboseLevel)
    : impl(level, color, file, line, func, dispatchAction)
{
}
WriterBasic::~WriterBasic() {}

IWriterBasic& WriterBasic::construct(const char *loggerId)
{
    impl.construct(loggerId);
    return *this;
}

IWriterBasic& WriterBasic::operator<<(std::ostream& (*log)(std::ostream&))
{
    impl << log;
    return *this;
}

IWriterBasic& WriterBasic::operator<<(const std::string & val)
{
    impl << val;
    return *this;
}

IWriterBasic& WriterBasic::operator<<(const char * val)
{
    impl << val;
    return *this;
}

IWriterBasic& WriterBasic::operator<<(void * val)
{
    impl << val;
    return *this;
}

IWriterBasic& WriterBasic::operator<<(unsigned char val)
{
    impl << val;
    return *this;
}

IWriterBasic& WriterBasic::operator<<(signed char val)
{
    impl << val;
    return *this;
}

IWriterBasic& WriterBasic::operator<<(int val)
{
    impl << val;
    return *this;
}

IWriterBasic& WriterBasic::operator<<(long val)
{
    impl << val;
    return *this;
}

IWriterBasic& WriterBasic::operator<<(long long val)
{
    impl << val;
    return *this;
}

IWriterBasic& WriterBasic::operator<<(unsigned val)
{
    impl << val;
    return *this;
}

IWriterBasic& WriterBasic::operator<<(unsigned long val)
{
    impl << val;
    return *this;
}

IWriterBasic& WriterBasic::operator<<(unsigned long long val)
{
    impl << val;
    return *this;
}

IWriterBasic& WriterBasic::operator<<(float val)
{
    impl << val;
    return *this;
}

IWriterBasic& WriterBasic::operator<<(double val)
{
    impl << val;
    return *this;
}

IWriterBasic& WriterBasic::operator<<(long double val)
{
    impl << val;
    return *this;
}

}
}
