#pragma once
/*
 * =============================================================================
 *   chess logging — compile-time channels, zero allocations, builder API
 * =============================================================================
 *
 *  HOW TO USE (in a .cpp file, as the FIRST lines):
 *
 *      #define CHESSLOG_MODULE BORD      // the channel of this file
 *      #include "Log.hpp"
 *
 *      LOG_INFO("en-passant check")
 *          .with_variable("right", CORD + DIR::RIGHT)
 *          .with_variable("left",  CORD + DIR::LEFT)
 *          .log();                          // .log() is optional: the temporary
 *                                           // flushes in its destructor too
 *
 *      LOG_WARN("king unsafe").with_variable("to", TO).log();
 *      LOG_ERR("no king found").log();
 *
 *  BUILD FLAGS (configure-time, CMake only — e.g. `cmake --preset logs-bord`
 *  or `cmake -B build -DLOG_BORD=1`):
 *
 *      LOG_RENDERING / LOG_BORD / LOG_ENGINE
 *          A channel's LOG_* calls only exist when its flag is set. Without the
 *          flag the call compiles to `while (false) ...` — literally zero
 *          bytes: the arguments are never even evaluated.
 *      CHESS_LOG_COLORS
 *          1 by default. Set to 0 to strip the ANSI colours of the [tag].
 *
 *  HOW TO ADD A CHANNEL — this file is the ONLY place to touch:
 *
 *      1. add one line to the CHESSLOG_CHANNELS table below:
 *             F(NAME, "tag", color-code)
 *      2. write code with `#define CHESSLOG_MODULE NAME` in the file
 *      3. enable it at configure time with -DLOG_NAME=1 (no CMake changes:
 *         every -DLOG_* flag is picked up automatically)
 *
 *  Output format (backend timestamp + prefix baked in by the builder):
 *      12:34:56.123 [bord] Move.cpp:42 INFO en-passant check right={5,-1}
 */

#include "ChessLog.hpp"
#include "Vector.hpp"
#include "List.hpp"

#include <cstdint>
#include <cstdio>

// The module of a TU that never declared one (e.g. headers chain-including
// this file). The user-facing macros must still be defined with a valid module
// for the include graph to work; such TUs just do not call LOG_* themselves.
#ifndef CHESSLOG_MODULE
#define CHESSLOG_MODULE BORD
#endif

#ifndef CHESS_LOG_COLORS
#define CHESS_LOG_COLORS 1
#endif

// ---------------------------------------------------------------------------
// channel registry — add new channels HERE (one line each), nothing else
// ---------------------------------------------------------------------------
#define CHESSLOG_CHANNELS(F) \
  F(RENDERING, "rendering", 36) \
  F(BORD, "bord", 33) \
  F(ENGINE, "engine", 35)

// ---------------------------------------------------------------------------
// everything below this line is generated from the registry, do not edit
// ---------------------------------------------------------------------------

#define CHESSLOG_CAT2_(a, b) a##b
#define CHESSLOG_CAT2(a, b) CHESSLOG_CAT2_(a, b)

// channel indices (registry order)
#define CHESSLOG_MOD_ENUM_ENTRY(name, tag, color) CHESSLOG_Mod_##name,
enum : uint8_t
{
  CHESSLOG_CHANNELS(CHESSLOG_MOD_ENUM_ENTRY) CHESSLOG_MOD_COUNT
};
#undef CHESSLOG_MOD_ENUM_ENTRY

// tag + colour of each channel (indexed by CHESSLOG_Mod_*)
#define CHESSLOG_MOD_TABLE_ENTRY(name, tag, color) {tag, color},
struct CHESSLOG_ModInfo
{
  const char* tag;
  int color;
};
inline constexpr CHESSLOG_ModInfo CHESSLOG_MOD_TABLE[] = {
  CHESSLOG_CHANNELS(CHESSLOG_MOD_TABLE_ENTRY)
};
#undef CHESSLOG_MOD_TABLE_ENTRY

// whether the module used in this TU is compiled in (undefined flags -> 0)
#define CHESSLOG_ACTIVE CHESSLOG_CAT2(LOG_, CHESSLOG_MODULE)

class Bord; // defined in Bord.hpp; its formatter lives in LogFormatters.hpp

namespace Chess
{
class Builder; // defined just below
} // namespace Chess

/*
 * Forward declarations of the value formatters used by with_variable() — they
 * must be visible before the Builder template below, so that its body finds
 * them in two-phase lookup at instantiation time. The Bord overload is
 * *defined* in LogFormatters.hpp (included from Bord.hpp).
 */
template <class T>
void ChessLog_print(Chess::Builder&, const T&); // fallback, see below

void ChessLog_print(Chess::Builder&, const int&);
void ChessLog_print(Chess::Builder&, const unsigned&);
void ChessLog_print(Chess::Builder&, const long&);
void ChessLog_print(Chess::Builder&, const unsigned long&);
void ChessLog_print(Chess::Builder&, const char&);
void ChessLog_print(Chess::Builder&, const bool&);
void ChessLog_print(Chess::Builder&, const float&);
void ChessLog_print(Chess::Builder&, const double&);
void ChessLog_print(Chess::Builder&, const char* const&);
void ChessLog_print(Chess::Builder&, const Vector&);
void ChessLog_print(Chess::Builder&, const MoveList&);
void ChessLog_print(Chess::Builder&, const Bord&);

namespace Chess
{

/*
 * One line in the making; an object on the stack, no std::string, no heap:
 *
 *   LOG_INFO("msg").with_variable("name", value).log();
 *
 * forget to call .log() and the destructor publishes the line anyway.
 */
class Builder
{
public:
  Builder(ChessLog::Level level, uint8_t mod, const char* file, int line, const char* message)
    : _level(level), _mod(mod), _file(trim_path(file)), _line(line)
  {
    const CHESSLOG_ModInfo& info = CHESSLOG_MOD_TABLE[_mod];
    append('[');
#if CHESS_LOG_COLORS
    append("\x1b[");
    append_uint(static_cast<uint64_t>(info.color));
    append('m');
#endif
    append(info.tag);
    append(']');
#if CHESS_LOG_COLORS
    append("\x1b[0m");
#endif
    append(' ');
    append(_file);
    append(':');
    append_uint(static_cast<uint64_t>(_line));
    append(' ');
    append(LEVEL_NAME[static_cast<uint8_t>(level)]);
    append(' ');
    append(message);
  }

  Builder(const Builder&) = delete;
  Builder& operator=(const Builder&) = delete;

  template <class T>
  Builder& with_variable(const char* name, T&& value)
  {
    if (_vars)
      append(", ");
    else
      append(' ');
    _vars = true;
    append(name);
    append('=');
    ChessLog_print(*this, static_cast<T&&>(value));
    return *this;
  }

  /* raw text with no key= or separator */
  Builder& with_text(const char* text)
  {
    append(' ');
    append(text);
    return *this;
  }

  void log() noexcept
  {
    if (_logged)
      return;
    _logged = true;
    if (_overflowed)
      append("...");
    ChessLog::publish(_level, _mod, _buf, _len);
  }

  ~Builder() { log(); }

  // ---- the value writers used by the ChessLog_print() formatters ----

  void append(char c)
  {
    if (_len < CAP)
      _buf[_len++] = c;
    else
      _overflowed = true;
  }

  void append(const char* s)
  {
    if (!s)
      s = "(null)";
    while (*s)
      append(*s++);
  }

  void append_uint(uint64_t value)
  {
    char digits[20];
    int count = 0;
    do
    {
      digits[count++] = static_cast<char>('0' + (value % 10));
      value /= 10;
    } while (value != 0);
    while (count != 0)
      append(digits[--count]);
  }

  void append_int(long long value)
  {
    if (value < 0)
    {
      append('-');
      append_uint(static_cast<uint64_t>(0) - static_cast<uint64_t>(value));
    }
    else
      append_uint(static_cast<uint64_t>(value));
  }

  void append_float(double value)
  {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%g", value);
    append(buffer);
  }

private:
  static constexpr size_t CAP = 1024 - 4; // keep room for the "..." tail
  static constexpr const char* LEVEL_NAME[3] = {"INFO", "WARN", "ERR"};

  static const char* trim_path(const char* path)
  {
    const char* base = path;
    for (const char* it = path; *it; ++it)
      if (*it == '/' || *it == '\\')
        base = it + 1;
    return base;
  }

  ChessLog::Level _level;
  uint8_t _mod;
  const char* _file;
  int _line;
  char _buf[CAP + 4];
  size_t _len = 0;
  bool _vars = false;
  bool _overflowed = false;
  bool _logged = false;
};

} // namespace Chess

// ---------------------------------------------------------------------------
// value formatters — how each type is drawn by .with_variable()
// (primitive types + Vector + list<Vector> here; Bord in LogFormatters.hpp)
// ---------------------------------------------------------------------------

inline void ChessLog_print(Chess::Builder& out, const int& value) { out.append_int(value); }
inline void ChessLog_print(Chess::Builder& out, const int8_t& value) { out.append_int(value); }
inline void ChessLog_print(Chess::Builder& out, const uint8_t& value) { out.append_uint(value); }
inline void ChessLog_print(Chess::Builder& out, const unsigned& value) { out.append_uint(value); }
inline void ChessLog_print(Chess::Builder& out, const long& value) { out.append_int(value); }
inline void ChessLog_print(Chess::Builder& out, const unsigned long& value) { out.append_uint(value); }
inline void ChessLog_print(Chess::Builder& out, const char& value) { out.append(value); }
inline void ChessLog_print(Chess::Builder& out, const bool& value)
{
  value ? out.append("true") : out.append("false");
}
inline void ChessLog_print(Chess::Builder& out, const float& value) { out.append_float(value); }
inline void ChessLog_print(Chess::Builder& out, const double& value) { out.append_float(value); }
inline void ChessLog_print(Chess::Builder& out, const char* const& value) { out.append(value); }
inline void ChessLog_print(Chess::Builder& out, const Vector& value)
{
  out.append('{');
  out.append_int(value.x);
  out.append(", ");
  out.append_int(value.y);
  out.append('}');
}
inline void ChessLog_print(Chess::Builder& out, const MoveList& value)
{
  out.append('(');
  bool first = true;
  for (const Vector& move : value)
  {
    if (!first)
      out.append(", ");
    first = false;
    ChessLog_print(out, move);
  }
  out.append(')');
}

/* the fallback stays undefined: an unsupported type fails to link with a
 * clear "undefined reference to ChessLog_print(Chess::Builder&, T const&)"
 * error naming the exact type */
/*
template <class T>
void ChessLog_print(Chess::Builder&, const T&)
{
  static_assert(!sizeof(T),
                "chesslog: unsupported type for with_variable(); add a ChessLog_print() "
                "overload in Log.hpp / LogFormatters.hpp");
}
*/

// ---------------------------------------------------------------------------
// user-facing macros
// ---------------------------------------------------------------------------

#if CHESSLOG_ACTIVE
#define LOG_INFO(msg)                                                                          \
  ::Chess::Builder(ChessLog::Level::Info, CHESSLOG_CAT2(CHESSLOG_Mod_, CHESSLOG_MODULE),        \
                   __FILE__, __LINE__, msg)
#define LOG_WARN(msg)                                                                          \
  ::Chess::Builder(ChessLog::Level::Warn, CHESSLOG_CAT2(CHESSLOG_Mod_, CHESSLOG_MODULE),        \
                   __FILE__, __LINE__, msg)
#define LOG_ERR(msg)                                                                           \
  ::Chess::Builder(ChessLog::Level::Error, CHESSLOG_CAT2(CHESSLOG_Mod_, CHESSLOG_MODULE),       \
                   __FILE__, __LINE__, msg)

/* direct quill-macro passthrough for engine hot loops: it takes quill's own
 * format string + args and skips the builder text work */
#define LOG_INFO_RAW(...)                                                                      \
  QUILL_LOG_INFO(ChessLog::logger(CHESSLOG_CAT2(CHESSLOG_Mod_, CHESSLOG_MODULE)), __VA_ARGS__)
#define LOG_WARN_RAW(...)                                                                      \
  QUILL_LOG_WARNING(ChessLog::logger(CHESSLOG_CAT2(CHESSLOG_Mod_, CHESSLOG_MODULE)), __VA_ARGS__)
#define LOG_ERR_RAW(...)                                                                       \
  QUILL_LOG_ERROR(ChessLog::logger(CHESSLOG_CAT2(CHESSLOG_Mod_, CHESSLOG_MODULE)), __VA_ARGS__)
#else
#define LOG_INFO(msg)                                                                          \
  while (false) ::Chess::Builder(ChessLog::Level::Info, 0u, "", 0, msg)
#define LOG_WARN(msg)                                                                          \
  while (false) ::Chess::Builder(ChessLog::Level::Warn, 0u, "", 0, msg)
#define LOG_ERR(msg)                                                                           \
  while (false) ::Chess::Builder(ChessLog::Level::Error, 0u, "", 0, msg)
#define LOG_INFO_RAW(...) while (false) QUILL_LOG_INFO(ChessLog::logger(0u), __VA_ARGS__)
#define LOG_WARN_RAW(...) while (false) QUILL_LOG_WARNING(ChessLog::logger(0u), __VA_ARGS__)
#define LOG_ERR_RAW(...) while (false) QUILL_LOG_ERROR(ChessLog::logger(0u), __VA_ARGS__)
#endif
