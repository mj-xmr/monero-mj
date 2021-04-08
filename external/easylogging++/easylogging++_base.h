//
//  Bismillah ar-Rahmaan ar-Raheem
//
//  Easylogging++ v9.96.7
//  Single-header only, cross-platform logging library for C++ applications
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

#ifndef EASYLOGGINGPP_BASE_H
#define EASYLOGGINGPP_BASE_H

#include <ostream>
#include <memory>

#undef ELPP_FUNC
#  define ELPP_FUNC __PRETTY_FUNCTION__

namespace el
{

namespace base
{
namespace type
{
typedef unsigned int EnumType;
typedef unsigned long int LineNumber;
typedef unsigned short VerboseLevel;
}
/// @brief Action to be taken for dispatching
enum class DispatchAction : base::type::EnumType
{
    None = 1, NormalLog = 2, SysLog = 4, FileOnlyLog = 8,
};
}
/// @brief Represents enumeration for severity level used to determine level of logging
///
/// @detail With Easylogging++, developers may disable or enable any level regardless of
/// what the severity is. Or they can choose to log using hierarchical logging flag
enum class Level : base::type::EnumType
{
    /// @brief Generic level that represents all the levels. Useful when setting global configuration for all levels
    Global = 1,
    /// @brief Information that can be useful to back-trace certain events - mostly useful than debug logs.
    Trace = 2,
    /// @brief Informational events most useful for developers to debug application
    Debug = 4,
    /// @brief Severe error information that will presumably abort application
    Fatal = 8,
    /// @brief Information representing errors in application but application will keep running
    Error = 16,
    /// @brief Useful when application has potentially harmful situtaions
    Warning = 32,
    /// @brief Information that can be highly useful and vary with verbose logging level.
    Verbose = 64,
    /// @brief Mainly useful to represent current progress of application
    Info = 128,
    /// @brief Represents unknown level
    Unknown = 1010
};
enum class Color : base::type::EnumType
{
    Default,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
};

namespace base
{
class Writer;

class IWriterTpl
{
public:

};

/**
Abstraction of Writer for basic types, without using templates.

This approach spares a lot of code generation time, if only basic types are needed.
*/
class IWriterBasic : public std::ostream
{
public:
    static std::unique_ptr<IWriterBasic> create(Level level, Color color, const char* file, base::type::LineNumber line,
                                           const char* func, base::DispatchAction dispatchAction = base::DispatchAction::NormalLog,
                                           base::type::VerboseLevel verboseLevel = 0);

    IWriterBasic();
    virtual ~IWriterBasic();

    virtual IWriterBasic& construct(const char *loggerId) = 0;

    virtual IWriterBasic& operator<<(std::ostream& (*log)(std::ostream&)) = 0;
    virtual IWriterBasic& operator<<(const std::string & val) = 0;
    virtual IWriterBasic& operator<<(const char * val) = 0;
    virtual IWriterBasic& operator<<(void * val) = 0;
    virtual IWriterBasic& operator<<(unsigned char val) = 0;
    virtual IWriterBasic& operator<<(signed char val) = 0;
    virtual IWriterBasic& operator<<(int val) = 0;
    virtual IWriterBasic& operator<<(long val) = 0;
    virtual IWriterBasic& operator<<(long long val) = 0;
    virtual IWriterBasic& operator<<(unsigned val) = 0;
    virtual IWriterBasic& operator<<(unsigned long val) = 0;
    virtual IWriterBasic& operator<<(unsigned long long val) = 0;
    virtual IWriterBasic& operator<<(float val) = 0;
    virtual IWriterBasic& operator<<(double val) = 0;
    virtual IWriterBasic& operator<<(long double val) = 0;

private:
};

} // base

class ILoggers
{
public:
    static bool allowed(Level leve, const char* cat);
};
}

#endif
