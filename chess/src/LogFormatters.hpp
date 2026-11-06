#pragma once
/*
 * Value formatters that need the full chess types:
 *
 *   - Vector / list<Vector> live in Log.hpp (they have no heavy dependency)
 *   - Bord lives here, because it needs the complete class definition
 *
 * This file is included automatically at the bottom of Bord.hpp, so every
 * translation unit that can see a Bord can also log one.
 */

#include "Log.hpp"

inline void ChessLog_print(Chess::Builder& out, const Bord& bord)
{
  out.append('\n');
  for (int y = 0; y < 8; ++y)
  {
    for (int x = 0; x < 8; ++x)
    {
      const int piece = bord(Vector(x, y));
      char c = '?';
      if (piece == 0)
        c = '.';
      else
      {
        switch (piece < 0 ? -piece : piece)
        {
        case KIND::KING:   c = piece < 0 ? 'k' : 'K'; break;
        case KIND::QUIN:   c = piece < 0 ? 'q' : 'Q'; break;
        case KIND::RUKE:   c = piece < 0 ? 'r' : 'R'; break;
        case KIND::BESHOP: c = piece < 0 ? 'b' : 'B'; break;
        case KIND::KNIGHT: c = piece < 0 ? 'n' : 'N'; break;
        case KIND::PAWN:   c = piece < 0 ? 'p' : 'P'; break;
        default:           c = '?';                   break;
        }
      }
      out.append(c);
    }
    out.append('\n');
  }
}
