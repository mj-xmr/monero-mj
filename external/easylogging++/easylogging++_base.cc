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
bool ILoggers::allowed(Level leve, const char* cat)
{
    return Loggers::allowed(leve, cat);
}
namespace base
{

// Factory method
std::unique_ptr<IWriterBasic> IWriterBasic::create(Level level, Color color, const char* file, base::type::LineNumber line,
        const char* func, base::DispatchAction dispatchAction,
        base::type::VerboseLevel verboseLevel)
{
    return std::unique_ptr<IWriterBasic>(new el::base::WriterBasic(level, color, file, line, func, dispatchAction));
}

IWriterBasic::IWriterBasic() {}
IWriterBasic::~IWriterBasic() {}

}
}
