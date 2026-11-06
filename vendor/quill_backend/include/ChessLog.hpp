#pragma once
/*
 * Public entry points of the chess logging backend (libquill_backend.so).
 *
 * The user-facing layer is chess/src/Log.hpp (the builder + the LOG_* macros);
 * you normally never include this header directly. It exists so that the chess
 * sources only depend on this small API, while all of quill's machinery is
 * compiled inside the single translation unit of the shared library.
 */

/*
 * The LOG_INFO / LOG_WARN / LOG_ERR macros belong to the chess builder API
 * (chess/src/Log.hpp), so quill's own non-prefixed aliases must not exist.
 * The prefixed QUILL_LOG_* macros keep working.
 */
#define QUILL_DISABLE_NON_PREFIXED_MACROS

#include "quill/LogMacros.h"
#include "quill/Logger.h"
#include "quill/core/Common.h"
#include "quill/core/FrontendOptions.h"

#include <cstddef>
#include <cstdint>

/*
 * Frontend options used by the whole application:
 *  - BoundedDropping queue of 512 KiB: the queue NEVER reallocates, so logging
 *    is truly zero-alloc, and a slow backend can never stall the game/search
 *    thread: if it can not keep up, new messages are dropped instead.
 */
struct ChessLogFrontendOptions : quill::FrontendOptions
{
  static constexpr quill::QueueType queue_type = quill::QueueType::BoundedDropping;
  static constexpr size_t initial_queue_capacity = 512u * 1024u;
};

using ChessLogLogger = quill::LoggerImpl<ChessLogFrontendOptions>;

namespace ChessLog
{
enum class Level : uint8_t
{
  Info,
  Warn,
  Error
};

/*
 * Start the backend thread (lazily, on first use) and return the logger of a
 * channel. `mod` is the channel index from the registry in chess/src/Log.hpp
 * (CHESSLOG_Mod_*). Returns nullptr if the backend could not be started.
 */
QUILL_NODISCARD ChessLogLogger* logger(uint8_t mod) noexcept;

/*
 * Publish one already-formatted line (`text` may contain ANSI colours and new
 * lines). No allocation; cheap queue push on the caller's thread.
 */
void publish(Level level, uint8_t mod, const char* text, size_t size) noexcept;

/* Explicitly start the backend thread (publish() would do it lazily anyway). */
void start() noexcept;
} // namespace ChessLog
