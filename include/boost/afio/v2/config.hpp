/* config.hpp
Configures Boost.AFIO
(C) 2015 Niall Douglas http://www.nedprod.com/
File Created: Dec 2015


Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

//! \def BOOST_AFIO_HEADERS_ONLY Whether AFIO is a headers only library. Defaults to 1 unless BOOST_ALL_DYN_LINK is defined.
#if !defined(BOOST_AFIO_HEADERS_ONLY) && !defined(BOOST_ALL_DYN_LINK)
#define BOOST_AFIO_HEADERS_ONLY 1
#endif

//! \def BOOST_AFIO_LOGGING_LEVEL How much detail to log. 0=disabled, 1=fatal, 2=error, 3=warn, 4=info, 5=debug, 6=all.
//! Defaults to error if NDEBUG defined, else info level.
#if !defined(BOOST_AFIO_LOGGING_LEVEL)
#ifdef NDEBUG
#define BOOST_AFIO_LOGGING_LEVEL 2  // error
#else
#define BOOST_AFIO_LOGGING_LEVEL 4  // info
#endif
#endif

//! \def BOOST_AFIO_LOG_BACKTRACE_LEVELS Bit mask of which log levels should be stack backtraced
//! which will slow those logs thirty fold or so. Defaults to (1<<1)|(1<<2)|(1<<3) i.e. stack backtrace
//! on fatal, error and warn logs.
#if !defined(BOOST_AFIO_LOG_BACKTRACE_LEVELS)
#define BOOST_AFIO_LOG_BACKTRACE_LEVELS ((1 << 1) | (1 << 2) | (1 << 3))
#endif

//! \def BOOST_AFIO_LOGGING_MEMORY How much memory to use for the log.
//! Defaults to 4Kb if NDEBUG defined, else 1Mb.
#if !defined(BOOST_AFIO_LOGGING_MEMORY)
#ifdef NDEBUG
#define BOOST_AFIO_LOGGING_MEMORY 4096
#else
#define BOOST_AFIO_LOGGING_MEMORY (1024 * 1024)
#endif
#endif


#if defined(_WIN32) && !defined(WIN32)
#define WIN32 _WIN32
#endif

#if defined(WIN32)
#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0600
#elif _WIN32_WINNT < 0x0600
#error _WIN32_WINNT must at least be set to Windows Vista for Boost AFIO to work
#endif
#if defined(NTDDI_VERSION) && NTDDI_VERSION < 0x06000000
#error NTDDI_VERSION must at least be set to Windows Vista for Boost AFIO to work
#endif
#endif

// Pull in detection of __MINGW64_VERSION_MAJOR
#ifdef __MINGW32__
#include <_mingw.h>
#endif

// If I'm on winclang, I can't stop the deprecation warnings from MSVCRT unless I do this
#if defined(_MSC_VER) && defined(__clang__)
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

#include "../boost-lite/include/import.h"

#ifndef __cpp_exceptions
#error Boost.AFIO needs C++ exceptions to be turned on
#endif
#ifndef __cpp_alias_templates
#error Boost.AFIO needs template alias support in the compiler
#endif
#ifndef __cpp_variadic_templates
#error Boost.AFIO needs variadic template support in the compiler
#endif
#ifndef __cpp_noexcept
#error Boost.AFIO needs noexcept support in the compiler
#endif
#ifndef __cpp_constexpr
#error Boost.AFIO needs constexpr (C++ 11) support in the compiler
#endif
#ifndef __cpp_thread_local
#error Boost.AFIO needs thread_local support in the compiler
#endif
#ifndef __cpp_init_captures
#error Boost.AFIO needs lambda init captures support in the compiler (C++ 14)
#endif
#ifndef __cpp_attributes
#error Boost.AFIO needs attributes support in the compiler
#endif
#if(defined(__GNUC__) && !defined(__clang__))
#define BOOST_AFIO_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#if BOOST_AFIO_GCC_VERSION < 40900
#error Boost.AFIO needs GCC 4.9 or later as the <regex> shipped in libstdc++ < 4.9 does not work
#endif
#endif

#if defined(BOOST_AFIO_LATEST_VERSION) && BOOST_AFIO_LATEST_VERSION < 2
#error You need to include the latest version of Boost.AFIO before any earlier versions within the same translation unit
#endif
#ifndef BOOST_AFIO_LATEST_VERSION
#define BOOST_AFIO_LATEST_VERSION 2
#endif

#undef BOOST_AFIO_V2_STL11_IMPL
#undef BOOST_AFIO_V2_FILESYSTEM_IMPL
#undef BOOST_AFIO_V2
#undef BOOST_AFIO_V2_NAMESPACE
#undef BOOST_AFIO_V2_NAMESPACE_BEGIN
#undef BOOST_AFIO_V2_NAMESPACE_END

// Default to the C++ 11 STL for atomic, chrono, mutex and thread except on Mingw32
#if(defined(BOOST_AFIO_USE_BOOST_THREAD) && BOOST_AFIO_USE_BOOST_THREAD) || (defined(__MINGW32__) && !defined(__MINGW64__) && !defined(__MINGW64_VERSION_MAJOR))
#if defined(BOOST_OUTCOME_USE_BOOST_THREAD) && BOOST_OUTCOME_USE_BOOST_THREAD != 1
#error You must configure Boost.Outcome and Boost.AFIO to both use Boost.Thread together or both not at all.
#endif
#define BOOST_OUTCOME_USE_BOOST_THREAD 1
#define BOOST_AFIO_V2_STL11_IMPL boost
#ifndef BOOST_THREAD_VERSION
#define BOOST_THREAD_VERSION 3
#endif
#if BOOST_THREAD_VERSION < 3
#error Boost.AFIO requires that Boost.Thread be configured to v3 or later
#endif
#else
#if defined(BOOST_OUTCOME_USE_BOOST_THREAD) && BOOST_OUTCOME_USE_BOOST_THREAD != 0
#error You must configure Boost.Outcome and Boost.AFIO to both use Boost.Thread together or both not at all.
#endif
#define BOOST_OUTCOME_USE_BOOST_THREAD 0
#define BOOST_AFIO_V2_STL11_IMPL std
#ifndef BOOST_AFIO_USE_BOOST_THREAD
#define BOOST_AFIO_USE_BOOST_THREAD 0
#endif
#endif
// Default to the C++ 11 STL if on MSVC (Dinkumware ships a copy), else Boost
#ifndef BOOST_AFIO_USE_BOOST_FILESYSTEM
#if _MSC_VER >= 1900  // >= VS 14
#define BOOST_AFIO_USE_BOOST_FILESYSTEM 0
#endif
#endif
#ifndef BOOST_AFIO_USE_BOOST_FILESYSTEM
#define BOOST_AFIO_USE_BOOST_FILESYSTEM 1
#endif
#if BOOST_AFIO_USE_BOOST_FILESYSTEM
#define BOOST_AFIO_V2_FILESYSTEM_IMPL boost
#define BOOST_AFIO_USE_LEGACY_FILESYSTEM_SEMANTICS 1
#else
#define BOOST_AFIO_V2_FILESYSTEM_IMPL std
#endif
#if BOOST_AFIO_LATEST_VERSION == 2
#define BOOST_AFIO_V2 (boost), (afio), (BOOSTLITE_NAMESPACE_VERSION(v2, BOOST_AFIO_V2_STL11_IMPL, BOOST_AFIO_V2_FILESYSTEM_IMPL), inline)
#else
#define BOOST_AFIO_V2 (boost), (afio), (BOOSTLITE_NAMESPACE_VERSION(v2, BOOST_AFIO_V2_STL11_IMPL, BOOST_AFIO_V2_FILESYSTEM_IMPL))
#endif
#if DOXYGEN_SHOULD_SKIP_THIS
//! The Boost namespace
namespace boost
{
  //! The AFIO namespace
  namespace afio
  {
    //! Inline namespace for this version of AFIO
    inline namespace v2
    {
      //! Collection of file system based algorithms
      namespace algorithm
      {
      }
    }
  }
}
#define BOOST_AFIO_V2_NAMESPACE boost::afio::v2
#define BOOST_AFIO_V2_NAMESPACE_BEGIN                                                                                                                                                                                                                                                                                          \
  namespace boost                                                                                                                                                                                                                                                                                                              \
  {                                                                                                                                                                                                                                                                                                                            \
    namespace afio                                                                                                                                                                                                                                                                                                             \
    {                                                                                                                                                                                                                                                                                                                          \
      inline namespace v2                                                                                                                                                                                                                                                                                                      \
      {
#define BOOST_AFIO_V2_NAMESPACE_END                                                                                                                                                                                                                                                                                            \
  }                                                                                                                                                                                                                                                                                                                            \
  }                                                                                                                                                                                                                                                                                                                            \
  }
#else
#define BOOST_AFIO_V2_NAMESPACE BOOSTLITE_NAMESPACE(BOOST_AFIO_V2)
#define BOOST_AFIO_V2_NAMESPACE_BEGIN BOOSTLITE_NAMESPACE_BEGIN(BOOST_AFIO_V2)
#define BOOST_AFIO_V2_NAMESPACE_END BOOSTLITE_NAMESPACE_END(BOOST_AFIO_V2)
#endif

// From automated matrix generator
#undef BOOST_AFIO_NEED_DEFINE
#undef BOOST_AFIO_NEED_DEFINE_DESCRIPTION
#if !BOOST_AFIO_USE_BOOST_THREAD && !BOOST_AFIO_USE_BOOST_FILESYSTEM
#ifndef BOOST_AFIO_NEED_DEFINE_00
#define BOOST_AFIO_NEED_DEFINE_DESCRIPTION "BOOST_AFIO_USE_BOOST_THREAD=0 BOOST_AFIO_USE_BOOST_FILESYSTEM=0"
#define BOOST_AFIO_NEED_DEFINE_00
#define BOOST_AFIO_NEED_DEFINE 1
#endif
#elif BOOST_AFIO_USE_BOOST_THREAD && !BOOST_AFIO_USE_BOOST_FILESYSTEM
#ifndef BOOST_AFIO_NEED_DEFINE_10
#define BOOST_AFIO_NEED_DEFINE_DESCRIPTION "BOOST_AFIO_USE_BOOST_THREAD=1 BOOST_AFIO_USE_BOOST_FILESYSTEM=0"
#define BOOST_AFIO_NEED_DEFINE_10
#define BOOST_AFIO_NEED_DEFINE 1
#endif
#elif !BOOST_AFIO_USE_BOOST_THREAD && BOOST_AFIO_USE_BOOST_FILESYSTEM
#ifndef BOOST_AFIO_NEED_DEFINE_01
#define BOOST_AFIO_NEED_DEFINE_DESCRIPTION "BOOST_AFIO_USE_BOOST_THREAD=0 BOOST_AFIO_USE_BOOST_FILESYSTEM=1"
#define BOOST_AFIO_NEED_DEFINE_01
#define BOOST_AFIO_NEED_DEFINE 1
#endif
#elif BOOST_AFIO_USE_BOOST_THREAD && BOOST_AFIO_USE_BOOST_FILESYSTEM
#ifndef BOOST_AFIO_NEED_DEFINE_11
#define BOOST_AFIO_NEED_DEFINE_DESCRIPTION "BOOST_AFIO_USE_BOOST_THREAD=1 BOOST_AFIO_USE_BOOST_FILESYSTEM=1"
#define BOOST_AFIO_NEED_DEFINE_11
#define BOOST_AFIO_NEED_DEFINE 1
#endif
#endif

#ifdef BOOST_AFIO_NEED_DEFINE
#undef BOOST_AFIO_AFIO_H

#define BOOST_STL11_ATOMIC_MAP_NO_ATOMIC_CHAR32_T  // missing VS14
#define BOOST_STL11_ATOMIC_MAP_NO_ATOMIC_CHAR16_T  // missing VS14
// Match Dinkumware's TR2 implementation
#define BOOST_STL1z_FILESYSTEM_MAP_NO_SYMLINK_OPTION
#define BOOST_STL1z_FILESYSTEM_MAP_NO_COPY_OPTION
#define BOOST_STL1z_FILESYSTEM_MAP_NO_CHANGE_EXTENSION
#define BOOST_STL1z_FILESYSTEM_MAP_NO_WRECURSIVE_DIRECTORY_ITERATOR
#define BOOST_STL1z_FILESYSTEM_MAP_NO_EXTENSION
#define BOOST_STL1z_FILESYSTEM_MAP_NO_TYPE_PRESENT
#define BOOST_STL1z_FILESYSTEM_MAP_NO_PORTABLE_FILE_NAME
#define BOOST_STL1z_FILESYSTEM_MAP_NO_PORTABLE_DIRECTORY_NAME
#define BOOST_STL1z_FILESYSTEM_MAP_NO_PORTABLE_POSIX_NAME
#define BOOST_STL1z_FILESYSTEM_MAP_NO_LEXICOGRAPHICAL_COMPARE
#define BOOST_STL1z_FILESYSTEM_MAP_NO_WINDOWS_NAME
#define BOOST_STL1z_FILESYSTEM_MAP_NO_PORTABLE_NAME
#define BOOST_STL1z_FILESYSTEM_MAP_NO_BASENAME
#define BOOST_STL1z_FILESYSTEM_MAP_NO_COMPLETE
#define BOOST_STL1z_FILESYSTEM_MAP_NO_IS_REGULAR
#define BOOST_STL1z_FILESYSTEM_MAP_NO_INITIAL_PATH
#define BOOST_STL1z_FILESYSTEM_MAP_NO_PERMISSIONS_PRESENT
#define BOOST_STL1z_FILESYSTEM_MAP_NO_CODECVT_ERROR_CATEGORY
#define BOOST_STL1z_FILESYSTEM_MAP_NO_WPATH
#define BOOST_STL1z_FILESYSTEM_MAP_NO_SYMBOLIC_LINK_EXISTS
#define BOOST_STL1z_FILESYSTEM_MAP_NO_COPY_DIRECTORY
#define BOOST_STL1z_FILESYSTEM_MAP_NO_NATIVE
#define BOOST_STL1z_FILESYSTEM_MAP_NO_UNIQUE_PATH

#include "../boost-lite/include/bind/stl11/std/atomic"
BOOST_AFIO_V2_NAMESPACE_BEGIN
namespace stl11
{
  using namespace boost_lite::bind::std::atomic;
}
BOOST_AFIO_V2_NAMESPACE_END
#if BOOST_OUTCOME_USE_BOOST_THREAD
#include "../boost-lite/include/bind/stl11/boost/chrono"
#include "../boost-lite/include/bind/stl11/boost/mutex"
#include "../boost-lite/include/bind/stl11/boost/ratio"
#include "../boost-lite/include/bind/stl11/boost/thread"
BOOST_AFIO_V2_NAMESPACE_BEGIN
namespace stl11
{
  namespace chrono = boost_lite::bind::boost::chrono;
  using namespace boost_lite::bind::boost::mutex;
  using namespace boost_lite::bind::boost::ratio;
  using namespace boost_lite::bind::boost::thread;
}
#else
#include "../boost-lite/include/bind/stl11/std/chrono"
#include "../boost-lite/include/bind/stl11/std/mutex"
#include "../boost-lite/include/bind/stl11/std/ratio"
#include "../boost-lite/include/bind/stl11/std/thread"
BOOST_AFIO_V2_NAMESPACE_BEGIN
namespace stl11
{
  namespace chrono = boost_lite::bind::std::chrono;
  using namespace boost_lite::bind::std::mutex;
  using namespace boost_lite::bind::std::ratio;
  using namespace boost_lite::bind::std::thread;
}
BOOST_AFIO_V2_NAMESPACE_END
#endif
#if BOOST_AFIO_USE_BOOST_FILESYSTEM
#include "../boost-lite/include/bind/stl1z/boost/filesystem"
BOOST_AFIO_V2_NAMESPACE_BEGIN
namespace stl1z
{
  namespace filesystem = boost_lite::bind::boost::filesystem;
}
BOOST_AFIO_V2_NAMESPACE_END
#else
#include "../boost-lite/include/bind/stl1z/std/filesystem"
BOOST_AFIO_V2_NAMESPACE_BEGIN
namespace stl1z
{
  namespace filesystem = boost_lite::bind::std::filesystem;
}
BOOST_AFIO_V2_NAMESPACE_END
#endif


// Bring in the Boost macro emulations
#include "../boost-lite/include/boost/config.hpp"

// Configure BOOST_AFIO_DECL
#if(defined(BOOST_AFIO_DYN_LINK) || defined(BOOST_ALL_DYN_LINK)) && !defined(BOOST_AFIO_STATIC_LINK)

#if defined(BOOST_AFIO_SOURCE)
#define BOOST_AFIO_DECL BOOST_SYMBOL_EXPORT
#define BOOST_AFIO_BUILD_DLL
#else
#define BOOST_AFIO_DECL BOOST_SYMBOL_IMPORT
#endif
#else
#define BOOST_AFIO_DECL
#endif  // building a shared library


#if !BOOST_AFIO_HAVE_CXX17_SPAN_IMPLEMENTATION
#include "../gsl-lite/include/gsl.h"
BOOST_AFIO_V2_NAMESPACE_BEGIN
template <class T> using span = gsl::span<T>;
using gsl::as_span;
BOOST_AFIO_V2_NAMESPACE_END
#endif


#if BOOST_AFIO_LOGGING_LEVEL
#include "../boost-lite/include/ringbuffer_log.hpp"

/*! \todo TODO FIXME Replace in-memory log with memory map file backed log.
*/
BOOST_AFIO_V2_NAMESPACE_BEGIN
//! The log used by AFIO
inline BOOST_AFIO_DECL ringbuffer_log::simple_ringbuffer_log<BOOST_AFIO_LOGGING_MEMORY> &log() noexcept
{
  static ringbuffer_log::simple_ringbuffer_log<BOOST_AFIO_LOGGING_MEMORY> _log(static_cast<ringbuffer_log::level>(BOOST_AFIO_LOGGING_LEVEL));
  return _log;
}
inline void record_error_into_afio_log(ringbuffer_log::level _level, const char *_message, unsigned _code1, unsigned _code2, const char *_function, unsigned lineno)
{
  // Here is a VERY useful place to breakpoint!
  log().emplace_back(_level, _message, _code1, _code2, _function, lineno);
}
BOOST_AFIO_V2_NAMESPACE_END
#endif

#ifndef BOOST_AFIO_LOG_FATAL_TO_CERR
#include <stdio.h>
#define BOOST_AFIO_LOG_FATAL_TO_CERR(expr)                                                                                                                                                                                                                                                                                     \
  fprintf(stderr, "%s\n", (expr));                                                                                                                                                                                                                                                                                             \
  fflush(stderr)
#endif

#if BOOST_AFIO_LOGGING_LEVEL >= 1
#define BOOST_AFIO_LOG_FATAL(inst, message)                                                                                                                                                                                                                                                                                    \
  {                                                                                                                                                                                                                                                                                                                            \
    BOOST_AFIO_V2_NAMESPACE::log().emplace_back(ringbuffer_log::level::fatal, (message), (unsigned) (uintptr_t)(inst), ringbuffer_log::this_thread_id(), (BOOST_AFIO_LOG_BACKTRACE_LEVELS & (1 << 1)) ? nullptr : __func__, __LINE__);                                                                                         \
    BOOST_AFIO_LOG_FATAL_TO_CERR(message);                                                                                                                                                                                                                                                                                     \
  }
#else
#define BOOST_AFIO_LOG_FATAL(inst, message) BOOST_AFIO_LOG_FATAL_TO_CERR(message)
#endif
#if BOOST_AFIO_LOGGING_LEVEL >= 2
#define BOOST_AFIO_LOG_ERROR(inst, message) BOOST_AFIO_V2_NAMESPACE::log().emplace_back(ringbuffer_log::level::error, (message), (unsigned) (uintptr_t)(inst), ringbuffer_log::this_thread_id(), (BOOST_AFIO_LOG_BACKTRACE_LEVELS & (1 << 2)) ? nullptr : __func__, __LINE__)
// Intercept when Outcome creates an error_code_extended and log it to our log too
#ifndef BOOST_OUTCOME_ERROR_CODE_EXTENDED_CREATION_HOOK
#define BOOST_OUTCOME_ERROR_CODE_EXTENDED_CREATION_HOOK                                                                                                                                                                                                                                                                        \
  if(*this)                                                                                                                                                                                                                                                                                                                    \
  BOOST_AFIO_V2_NAMESPACE::record_error_into_afio_log(ringbuffer_log::level::error, this->message().c_str(), this->value(), (unsigned) this->_unique_id, (BOOST_AFIO_LOG_BACKTRACE_LEVELS & (1 << 2)) ? nullptr : __func__, __LINE__)
#endif
#else
#define BOOST_AFIO_LOG_ERROR(inst, message)
#endif
#if BOOST_AFIO_LOGGING_LEVEL >= 3
#define BOOST_AFIO_LOG_WARN(inst, message) BOOST_AFIO_V2_NAMESPACE::log().emplace_back(ringbuffer_log::level::warn, (message), (unsigned) (uintptr_t)(inst), ringbuffer_log::this_thread_id(), (BOOST_AFIO_LOG_BACKTRACE_LEVELS & (1 << 3)) ? nullptr : __func__, __LINE__)
#else
#define BOOST_AFIO_LOG_WARN(inst, message)
#endif

// Need Outcome in play before I can define logging level 4
#include "../outcome/include/boost/outcome.hpp"
BOOST_AFIO_V2_NAMESPACE_BEGIN
// We are so heavily tied into Outcome we just import it wholesale into our namespace
using namespace BOOST_OUTCOME_V1_NAMESPACE;
// Force these to the same overloading precedence as if they were defined in the AFIO namespace
using BOOST_OUTCOME_V1_NAMESPACE::outcome;
using BOOST_OUTCOME_V1_NAMESPACE::make_errored_result;
using BOOST_OUTCOME_V1_NAMESPACE::make_errored_outcome;
BOOST_AFIO_V2_NAMESPACE_END


#if BOOST_AFIO_LOGGING_LEVEL >= 4
#define BOOST_AFIO_LOG_INFO(inst, message) BOOST_AFIO_V2_NAMESPACE::log().emplace_back(ringbuffer_log::level::info, (message), (unsigned) (uintptr_t)(inst), ringbuffer_log::this_thread_id(), (BOOST_AFIO_LOG_BACKTRACE_LEVELS & (1 << 4)) ? nullptr : __func__, __LINE__)

// Need to expand out our namespace into a string
#define BOOST_AFIO_LOG_STRINGIFY9(s) #s "::"
#define BOOST_AFIO_LOG_STRINGIFY8(s) BOOST_AFIO_LOG_STRINGIFY9(s)
#define BOOST_AFIO_LOG_STRINGIFY7(s) BOOST_AFIO_LOG_STRINGIFY8(s)
#define BOOST_AFIO_LOG_STRINGIFY6(s) BOOST_AFIO_LOG_STRINGIFY7(s)
#define BOOST_AFIO_LOG_STRINGIFY5(s) BOOST_AFIO_LOG_STRINGIFY6(s)
#define BOOST_AFIO_LOG_STRINGIFY4(s) BOOST_AFIO_LOG_STRINGIFY5(s)
#define BOOST_AFIO_LOG_STRINGIFY3(s) BOOST_AFIO_LOG_STRINGIFY4(s)
#define BOOST_AFIO_LOG_STRINGIFY2(s) BOOST_AFIO_LOG_STRINGIFY3(s)
#define BOOST_AFIO_LOG_STRINGIFY(s) BOOST_AFIO_LOG_STRINGIFY2(s)
BOOST_AFIO_V2_NAMESPACE_BEGIN
//! Returns the AFIO namespace as a string
inline span<char> afio_namespace_string()
{
  static char buffer[64];
  static size_t length;
  if(length)
    return span<char>(buffer, length);
  const char *src = BOOST_AFIO_LOG_STRINGIFY(BOOST_AFIO_V2_NAMESPACE);
  char *bufferp = buffer;
  for(; *src && bufferp - buffer < sizeof(buffer); src++)
  {
    if(*src != ' ')
      *bufferp++ = *src;
  }
  *bufferp = 0;
  length = bufferp - buffer;
  return span<char>(buffer, length);
}
//! Returns the Outcome namespace as a string
inline span<char> outcome_namespace_string()
{
  static char buffer[64];
  static size_t length;
  if(length)
    return span<char>(buffer, length);
  const char *src = BOOST_AFIO_LOG_STRINGIFY(BOOST_OUTCOME_V1_NAMESPACE);
  char *bufferp = buffer;
  for(; *src && bufferp - buffer < sizeof(buffer); src++)
  {
    if(*src != ' ')
      *bufferp++ = *src;
  }
  *bufferp = 0;
  length = bufferp - buffer;
  return span<char>(buffer, length);
}
//! Strips a __PRETTY_FUNCTION__ of all instances of boost::afio:: and boost::outcome::
inline void strip_pretty_function(char *out, size_t bytes, const char *in)
{
  const span<char> remove1 = afio_namespace_string();
  const span<char> remove2 = outcome_namespace_string();
  for(--bytes; bytes && *in; --bytes)
  {
    if(!memcmp(in, remove1.data(), remove1.size()))
      in += remove1.size();
    if(!memcmp(in, remove2.data(), remove2.size()))
      in += remove2.size();
    *out++ = *in++;
  }
  *out = 0;
}
BOOST_AFIO_V2_NAMESPACE_END
#ifdef _MSC_VER
#define BOOST_AFIO_LOG_FUNCTION_CALL(inst)                                                                                                                                                                                                                                                                                     \
  {                                                                                                                                                                                                                                                                                                                            \
    char buffer[256];                                                                                                                                                                                                                                                                                                          \
    BOOST_AFIO_V2_NAMESPACE::strip_pretty_function(buffer, sizeof(buffer), __FUNCSIG__);                                                                                                                                                                                                                                       \
    BOOST_AFIO_LOG_INFO(inst, buffer);                                                                                                                                                                                                                                                                                         \
  }
#else
#define BOOST_AFIO_LOG_FUNCTION_CALL(inst)                                                                                                                                                                                                                                                                                     \
  {                                                                                                                                                                                                                                                                                                                            \
    char buffer[256];                                                                                                                                                                                                                                                                                                          \
    BOOST_AFIO_V2_NAMESPACE::strip_pretty_function(buffer, sizeof(buffer), __PRETTY_FUNCTION__);                                                                                                                                                                                                                               \
    BOOST_AFIO_LOG_INFO(inst, buffer);                                                                                                                                                                                                                                                                                         \
  }
#endif
#else
#define BOOST_AFIO_LOG_INFO(inst, message)
#define BOOST_AFIO_LOG_FUNCTION_CALL(inst)
#endif
#if BOOST_AFIO_LOGGING_LEVEL >= 5
#define BOOST_AFIO_LOG_DEBUG(inst, message) BOOST_AFIO_V2_NAMESPACE::log().emplace_back(ringbuffer_log::level::debug, (message), (unsigned) (uintptr_t)(inst), ringbuffer_log::this_thread_id(), (BOOST_AFIO_LOG_BACKTRACE_LEVELS & (1 << 5)) ? nullptr : __func__, __LINE__)
#else
#define BOOST_AFIO_LOG_DEBUG(inst, message)
#endif
#if BOOST_AFIO_LOGGING_LEVEL >= 6
#define BOOST_AFIO_LOG_ALL(inst, message) BOOST_AFIO_V2_NAMESPACE::log().emplace_back(ringbuffer_log::level::all, (message), (unsigned) (uintptr_t)(inst), ringbuffer_log::this_thread_id(), (BOOST_AFIO_LOG_BACKTRACE_LEVELS & (1 << 6)) ? nullptr : __func__, __LINE__)
#else
#define BOOST_AFIO_LOG_ALL(inst, message)
#endif

#include <time.h>  // for struct timespec

BOOST_AFIO_V2_NAMESPACE_BEGIN

// The C++ 11 runtime is much better at exception state than Boost so no choice here
using std::make_exception_ptr;
using std::error_code;
using std::generic_category;
using std::system_category;
using std::system_error;

// Too darn useful
using std::to_string;
namespace detail
{
  template <class F> using function_ptr = boost::outcome::detail::function_ptr<F>;
  using boost::outcome::detail::make_function_ptr;
  using boost::outcome::detail::emplace_function_ptr;
}
namespace detail
{

  namespace Impl
  {
    template <typename T, bool iscomparable> struct is_nullptr
    {
      bool operator()(T c) const noexcept { return !c; }
    };
    template <typename T> struct is_nullptr<T, false>
    {
      bool operator()(T) const noexcept { return false; }
    };
  }
//! Compile-time safe detector of if \em v is nullptr (can cope with non-pointer convertibles)
#if defined(__GNUC__) && (BOOST_GCC < 41000 || defined(__MINGW32__))
  template <typename T> bool is_nullptr(T v) noexcept { return Impl::is_nullptr<T, std::is_constructible<bool, T>::value>()(std::forward<T>(v)); }
#else
  template <typename T> bool is_nullptr(T v) noexcept { return Impl::is_nullptr<T, std::is_trivially_constructible<bool, T>::value>()(std::forward<T>(v)); }
#endif


  template <typename callable> class UndoerImpl
  {
    bool _dismissed;
    callable undoer;
    UndoerImpl() = delete;
    UndoerImpl(const UndoerImpl &) = delete;
    UndoerImpl &operator=(const UndoerImpl &) = delete;
    explicit UndoerImpl(callable &&c)
        : _dismissed(false)
        , undoer(std::move(c))
    {
    }
    void int_trigger()
    {
      if(!_dismissed && !is_nullptr(undoer))
      {
        undoer();
        _dismissed = true;
      }
    }

  public:
    UndoerImpl(UndoerImpl &&o) noexcept : _dismissed(o._dismissed), undoer(std::move(o.undoer)) { o._dismissed = true; }
    UndoerImpl &operator=(UndoerImpl &&o) noexcept
    {
      int_trigger();
      _dismissed = o._dismissed;
      undoer = std::move(o.undoer);
      o._dismissed = true;
      return *this;
    }
    template <typename _callable> friend UndoerImpl<_callable> Undoer(_callable c);
    ~UndoerImpl() { int_trigger(); }
    //! Returns if the Undoer is dismissed
    bool dismissed() const { return _dismissed; }
    //! Dismisses the Undoer
    void dismiss(bool d = true) { _dismissed = d; }
    //! Undismisses the Undoer
    void undismiss(bool d = true) { _dismissed = !d; }
  };  // UndoerImpl


  /*! \brief Alexandrescu style rollbacks, a la C++ 11.

  Example of usage:
  \code
  auto resetpos=Undoer([&s]() { s.seekg(0, std::ios::beg); });
  ...
  resetpos.dismiss();
  \endcode
  */
  template <typename callable> inline UndoerImpl<callable> Undoer(callable c)
  {
    // static_assert(!std::is_function<callable>::value && !std::is_member_function_pointer<callable>::value && !std::is_member_object_pointer<callable>::value && !has_call_operator<callable>::value, "Undoer applied to a type not providing a call operator");
    auto foo = UndoerImpl<callable>(std::move(c));
    return foo;
  }  // Undoer

}  // namespace detail


// Temporary in lieu of full fat afio::path
/* \todo Full fat afio::path needs to be able to variant a win32 path
and a nt kernel path.
\todo A variant of an open handle as base and a relative path fragment
from there is also needed, though I have no idea how to manage lifetime
for such a thing.
\todo It would make a great deal of sense if afio::path were
a linked list of filesystem::path fragments as things like directory
hierarchy walks do a lot of leaf node splitting which for a 32k path
means a ton load of memory copying. Something like LLVM's list of
string fragments would be far faster - look for an existing implementation
before writing our own! One of those path fragments could variant onto
an open handle to solve the earlier issue.
*/
using fixme_path = stl1z::filesystem::path;

//! Constexpr typesafe bitwise flags support
template <class Enum> struct bitfield : public Enum
{
  //! The C style enum type which represents flags in this bitfield
  using enum_type = typename Enum::enum_type;
  //! The type which the C style enum implicitly converts to
  using underlying_type = std::underlying_type_t<enum_type>;

private:
  underlying_type _value;

public:
  //! Default construct to all bits zero
  constexpr bitfield() noexcept : _value(0) {}
  //! Implicit construction from the C style enum
  constexpr bitfield(enum_type v) noexcept : _value(v) {}
  //! Implicit construction from the underlying type of the C enum
  constexpr bitfield(underlying_type v) noexcept : _value(v) {}

  //! Permit explicit casting to the underlying type
  explicit constexpr operator underlying_type() const noexcept { return _value; }
  //! Test for non-zeroness
  constexpr operator bool() const noexcept { return !!_value; }
  //! Test for zeroness
  constexpr bool operator!() const noexcept { return !_value; }

  //! Performs a bitwise NOT
  constexpr bitfield operator~() const noexcept { return bitfield(~_value); }
  //! Performs a bitwise AND
  constexpr bitfield operator&(bitfield o) const noexcept { return bitfield(_value & o._value); }
  //! Performs a bitwise AND
  constexpr bitfield operator&(enum_type o) const noexcept { return bitfield(_value & o); }
  //! Performs a bitwise AND
  BOOST_CXX14_CONSTEXPR bitfield &operator&=(bitfield o) noexcept
  {
    _value &= o._value;
    return *this;
  }
  //! Performs a bitwise AND
  BOOST_CXX14_CONSTEXPR bitfield &operator&=(enum_type o) noexcept
  {
    _value &= o;
    return *this;
  }
  //! Performs a bitwise OR
  constexpr bitfield operator|(bitfield o) const noexcept { return bitfield(_value | o._value); }
  //! Performs a bitwise OR
  constexpr bitfield operator|(enum_type o) const noexcept { return bitfield(_value | o); }
  //! Performs a bitwise OR
  BOOST_CXX14_CONSTEXPR bitfield &operator|=(bitfield o) noexcept
  {
    _value |= o._value;
    return *this;
  }
  //! Performs a bitwise OR
  BOOST_CXX14_CONSTEXPR bitfield &operator|=(enum_type o) noexcept
  {
    _value |= o;
    return *this;
  }
  //! Performs a bitwise XOR
  constexpr bitfield operator^(bitfield o) const noexcept { return bitfield(_value ^ o._value); }
  //! Performs a bitwise XOR
  constexpr bitfield operator^(enum_type o) const noexcept { return bitfield(_value ^ o); }
  //! Performs a bitwise XOR
  BOOST_CXX14_CONSTEXPR bitfield &operator^=(bitfield o) noexcept
  {
    _value ^= o._value;
    return *this;
  }
  //! Performs a bitwise XOR
  BOOST_CXX14_CONSTEXPR bitfield &operator^=(enum_type o) noexcept
  {
    _value ^= o;
    return *this;
  }
};

//! Begins a typesafe bitfield
#define BOOST_AFIO_BITFIELD_BEGIN(type)                                                                                                                                                                                                                                                                                        \
  \
struct type##_base                                                                                                                                                                                                                                                                                                             \
  \
{                                                                                                                                                                                                                                                                                                                         \
  enum enum_type : unsigned

//! Ends a typesafe bitfield
#define BOOST_AFIO_BITFIELD_END(type)                                                                                                                                                                                                                                                                                          \
  \
;                                                                                                                                                                                                                                                                                                                         \
  }                                                                                                                                                                                                                                                                                                                            \
  ;                                                                                                                                                                                                                                                                                                                            \
  \
using type = bitfield<type##_base>;

// Native handle support
namespace win
{
  using handle = void *;
  using dword = unsigned long;
}

BOOST_AFIO_V2_NAMESPACE_END


///////////////////////////////////////////////////////////////////////////////
//  Auto library naming
#if !defined(BOOST_AFIO_SOURCE) && !defined(BOOST_ALL_NO_LIB) && !defined(BOOST_AFIO_NO_LIB) && !AFIO_STANDALONE && !BOOST_AFIO_HEADERS_ONLY

#define BOOST_LIB_NAME boost_afio

// tell the auto-link code to select a dll when required:
#if defined(BOOST_ALL_DYN_LINK) || defined(BOOST_AFIO_DYN_LINK)
#define BOOST_DYN_LINK
#endif

#include <boost/config/auto_link.hpp>

#endif  // auto-linking disabled

//#define BOOST_THREAD_VERSION 4
//#define BOOST_THREAD_PROVIDES_VARIADIC_THREAD
//#define BOOST_THREAD_DONT_PROVIDE_FUTURE
//#define BOOST_THREAD_PROVIDES_SIGNATURE_PACKAGED_TASK
#if BOOST_AFIO_HEADERS_ONLY == 1 && !defined(BOOST_AFIO_SOURCE)
#define BOOST_AFIO_HEADERS_ONLY_FUNC_SPEC inline
#define BOOST_AFIO_HEADERS_ONLY_MEMFUNC_SPEC inline
#define BOOST_AFIO_HEADERS_ONLY_VIRTUAL_SPEC inline virtual
#else
#define BOOST_AFIO_HEADERS_ONLY_FUNC_SPEC extern BOOST_AFIO_DECL
#define BOOST_AFIO_HEADERS_ONLY_MEMFUNC_SPEC
#define BOOST_AFIO_HEADERS_ONLY_VIRTUAL_SPEC virtual
#endif

#if defined(__has_feature)
#if __has_feature(thread_sanitizer)
#define BOOST_AFIO_DISABLE_THREAD_SANITIZE __attribute__((no_sanitize_thread))
#endif
#endif
#ifndef BOOST_AFIO_DISABLE_THREAD_SANITIZE
#define BOOST_AFIO_DISABLE_THREAD_SANITIZE
#endif

#ifndef BOOST_AFIO_THREAD_LOCAL
#ifdef __cpp_thread_local
#define BOOST_AFIO_THREAD_LOCAL thread_local
#elif defined(_MSC_VER)
#define BOOST_AFIO_THREAD_LOCAL __declspec(thread)
#elif defined(__GNUC__)
#define BOOST_AFIO_THREAD_LOCAL __thread
#else
#error Unknown compiler, cannot set BOOST_AFIO_THREAD_LOCAL
#endif
#endif

#endif  // BOOST_AFIO_NEED_DEFINE
