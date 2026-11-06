#ifndef BORD_HPP
#define BORD_HPP

#include "CONST.hpp"
#include <cassert>
#include <cstdint>
#include <glm/fwd.hpp>
#pragma once

#include "Vector.hpp"
#include "List.hpp"

class Bord {
public:
  bool king0, king7, ruke00, ruke07, ruke70, ruke77;
  void Kings();
  int8_t The_Bord[64];
  Vector W_king, B_king;
  Vector OOP_CORD = Vector({-1, -1}); // onpeson pown cord
  Vector PROM_CORD = Vector({-1, -1}); // the pone waiting to be promoted
  COLOR coler = WHITE;

  inline bool IsMyTurn(const Vector &cord) const {
    return (*this)(cord) * this->coler > 0;
  }
  inline void switchTurn(void) {
    if (this->coler == WHITE)
      this->coler = BLACK;
    else
      this->coler = WHITE;
  }
  void move(const Vector &from, const Vector &to) {
    (*this)(to) = (*this)(from);
    (*this)(from) = 0;
  }

  Vector MyKing(const Vector &Me);
  Bord();

  // * the promotion : the pone stays on the last row , the turn does NOT pass,
  //   the next pick is the piece he wants ( quad/roque/fou/cavalier )
  inline bool IsPending(void) const { return (*this).PROM_CORD != Vector({-1, -1}); }

  // * the 4 choises on the bord : 0 = QUIN , 1 = RUKE , 2 = BESHOP , 3 = KNIGHT
  Vector PROM_Square(int K) const;
  // * the type the board click stands for ( 0 = not a choise square )
  int8_t PROM_Clicked(const Vector &CLICK) const;
  // * apply the picked piece and pass the turn
  void PROM_Pick(int8_t TYPE);

  // * the MAP : each peice of the NEXT player and its moves ( upfrant )
  MAP MOVES;
  int GAME_OVER = 0; // 0 = playing , 1 = white won , -1 = black won , 2 = stalemate
  void MAK_MOVES();
  void UpDate(bool SWAP = true);

  // * (➡ , ⬇)
  const int8_t &operator[](Vector p_vec) const;

  int8_t &operator()(Vector p_vec) {
    static int8_t outOfBounds = NaN;

    if (!(0 <= p_vec.x && 0 <= p_vec.y && p_vec.x < 8 && p_vec.y < 8)) {
      assert(("out of bounds Bord access", false));
      return outOfBounds; // unreach in release
    }

    return (The_Bord[p_vec.x + 8 * p_vec.y]);
  }

  const int8_t &operator()(Vector p_vec) const {
    if (0 <= p_vec.x && 0 <= p_vec.y && p_vec.x < 8 && p_vec.y < 8)

      return (The_Bord[p_vec.x + 8 * p_vec.y]);

    return NaN;
  }
};


#endif // BORD_HPP
