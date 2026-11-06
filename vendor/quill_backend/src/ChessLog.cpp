/*
 * The single translation unit of libquill_backend.so.
 *
 * It compiles all of quill's machinery (backend thread, sinks, queue, encoding)
 * into one shared object, and exposes the tiny API from ChessLog.hpp. The
 * chess sources therefore never compile quill's backend themselves.
 */

// Satisfies Log.hpp's contract; the backend itself never logs.
#define CHESSLOG_MODULE BORD

#include "ChessLog.hpp"

#include "Log.hpp" // channel registry (CHESSLOG_CHANNELS)

#include "quill/Backend.h"
#include "quill/Frontend.h"
#include "quill/sinks/ConsoleSink.h"

#include <atomic>
#include <memory>
#include <mutex>
#include <string_view>

namespace ChessLog
{
namespace
{
std::once_flag started_once;
std::atomic<bool> started{false};

ChessLogLogger* loggers[CHESSLOG_MOD_COUNT] = {};

char const* CONSOLE_SINK_NAME = "chess_console";

void create_console_logger_and_sink()
{
  quill::ConsoleSinkConfig console_config;
  // [tag] file:line LEVEL are baked into the message text by the frontend
  // builder; the backend only adds the timestamp.
  console_config.set_override_pattern_formatter_options(
    quill::PatternFormatterOptions{"%(time) %(message)"});

  std::shared_ptr<quill::Sink> console_sink =
    quill::FrontendImpl<ChessLogFrontendOptions>::create_or_get_sink<quill::ConsoleSink>(
      CONSOLE_SINK_NAME, console_config);

#define CHESSLOG_DEFINE_LOGGER(name, tag, color)                                              \
  loggers[CHESSLOG_Mod_##name] =                                                              \
    quill::FrontendImpl<ChessLogFrontendOptions>::create_or_get_logger(tag, console_sink);
  CHESSLOG_CHANNELS(CHESSLOG_DEFINE_LOGGER)
#undef CHESSLOG_DEFINE_LOGGER
}

void ensure_started() noexcept
{
  std::call_once(
    started_once,
    []()
    {
      try
      {
        create_console_logger_and_sink();

        quill::BackendOptions backend_options;
        // keep ANSI colours (and the board dump new lines) intact in the output
        backend_options.check_printable_char = nullptr;
        quill::Backend::start(backend_options);

        started.store(true, std::memory_order_release);
      }
      catch (...)
      {
        // Backend failed to start; publish() will silently do nothing.
        started.store(false, std::memory_order_release);
      }
    });
}
} // namespace

ChessLogLogger* logger(uint8_t mod) noexcept
{
  ensure_started();

  if (mod < CHESSLOG_MOD_COUNT)
    return loggers[mod];
  return nullptr;
}

void publish(Level level, uint8_t mod, const char* text, size_t size) noexcept
{
  ChessLogLogger* logger_ = logger(mod);
  if (!logger_)
    return;

  std::string_view text_view{text, size};
  try
  {
    // The prefixed QUILL_LOG_* macros are used so they can not collide with
    // the LOG_* macros that chess/src/Log.hpp defines for the user.
    switch (level)
    {
    case Level::Info:
      QUILL_LOG_INFO(logger_, "{}", text_view);
      break;
    case Level::Warn:
      QUILL_LOG_WARNING(logger_, "{}", text_view);
      break;
    case Level::Error:
      QUILL_LOG_ERROR(logger_, "{}", text_view);
      break;
    }
  }
  catch (...)
  {
  }
}

void start() noexcept { ensure_started(); }
} // namespace ChessLog
