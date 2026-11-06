#ifndef LIST_HPP
#define LIST_HPP

#include "Vector.hpp"

// * a move list on the stack ( no std::list , no heap )
struct MoveList {
  Vector M[32]; // queen = 27 max, king = 8 + 2 castling
  int N = 0;
  inline void add(const Vector &V) {
    if (N < 32)
      M[N++] = V;
  }
  inline int size() const { return N; }
  inline bool empty() const { return N == 0; }
  inline Vector *begin() { return M; }
  inline Vector *end() { return M + N; }
  inline const Vector *begin() const { return M; }
  inline const Vector *end() const { return M + N; }
};

// * ONE entry of the map : a piece and its legal moves
struct PIECE_MOVES {
  Vector where;   // where the peice stands
  MoveList moves; // its legal moves ( ready made )
};

// * the MAP : every peice of the NEXT player -> its moves
//   ( is made upfrant in Bord::UpDate , then we only look up )
struct MAP {
  PIECE_MOVES P[16]; // one side can never have more than 16 peices
  int N = 0;

  inline void clear(void) { N = 0; }

  inline void add(const Vector &WHERE, const MoveList &MS) {
    if (N < 16) {
      P[N].where = WHERE;
      P[N].moves = MS;
      ++N;
    }
  }

  // is CORD in the map ? ( -1 = no , otherwize the index )
  inline int find(const Vector &CORD) const {
    for (int i = 0; i < N; ++i)
      if (P[i].where == CORD)
        return i;
    return -1;
  }

  inline const MoveList &get_(const Vector &CORD) const {
    static const MoveList EMPTY; // no moves
    int I = find(CORD);
    return (I >= 0) ? P[I].moves : EMPTY;
  }
};

#endif // LIST_HPP
