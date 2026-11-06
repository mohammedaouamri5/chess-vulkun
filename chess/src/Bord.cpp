#include "Bord.hpp"
#include "CONST.hpp"
#include "Move.hpp"

Vector Bord::MyKing(const Vector &Me) {
  if (abs((*this)(Me)) == KIND::KING)
    return Me;
  if ((*this)(Me) > 0)
    return W_king;
  return B_king;
}

void Bord::Kings() {

  // *⬇
  for (int i = 0; i < 8; ++i)
    // * ➡
    for (int j = 0; j < 8; ++j)

      if ((*this)[{j, i}] == KIND::KING)

        W_king = {j, i};

      else if ((*this)[{j, i}] == -KIND::KING)

        B_king = {j, i};
}

Bord::Bord() {
  int8_t bord_main[64] = {-5, -2, -3, -15, -100, -3, -2, -5, -1, -1, -1, -1, -1,
                          -1, -1, -1, 0,   0,    0,  0,  0,  0,  0,  0,  0,  0,
                          0,  0,  0,  0,   0,    0,  0,  0,  0,  0,  0,  0,  0,
                          0,  0,  0,  0,   0,    0,  0,  0,  0,  1,  1,  1,  1,
                          1,  1,  1,  1,   5,    2,  3,  15, 100, 3, 2,  5};

  for (int i = 0; i < 64; ++i)
    this->The_Bord[i] = bord_main[i];
  Kings();
  this->king0 = true;
  this->king7 = true;
  this->ruke00 = true;
  this->ruke07 = true;
  this->ruke70 = true;
  this->ruke77 = true;

  this->coler = WHITE;
  this->OOP_CORD = Vector({-1, -1});
  this->MAK_MOVES();
}

const int8_t &Bord::operator[](Vector p_vec) const {
  if (0 <= p_vec.x && 0 <= p_vec.y && p_vec.x < 8 && p_vec.y < 8) {
    return (this->The_Bord[p_vec.x + 8 * p_vec.y]);
  }
  return NaN;
}

void Bord::UpDate(bool SWAP) {
  using namespace KIND;
  if (SWAP)
    switchTurn();
  if ((*this)[{4, 7}] != KING)
    this->king7 = false;

  if ((*this)[{4, 0}] != -KING)
    this->king0 = false;

  if ((*this)[{0, 0}] != -RUKE)
    this->ruke00 = false;
  if ((*this)[{7, 0}] != -RUKE)
    this->ruke07 = false;
  if ((*this)[{0, 7}] != RUKE)
    this->ruke70 = false;
  if ((*this)[{7, 7}] != RUKE)
    this->ruke77 = false;

  this->MAK_MOVES();
}

// * fill the MAP : every peice of the NEXT player ( this->coler ) and its
//   legal moves , made upfrant so the clicks only look up
void Bord::MAK_MOVES() {
  this->MOVES.clear();
  for (int i = 0; i < 8; ++i)    // * ⬇
    for (int j = 0; j < 8; ++j) { // * ➡
      Vector MO(j, i);
      if (IsMyTurn(MO))
        this->MOVES.add(MO, Get_Move(MO, this));
    }

  // * checkmate / stalemate : the side to move has NO legal move at all
  this->GAME_OVER = 0;
  bool ANY = false;
  for (int i = 0; i < this->MOVES.N; ++i)
    if (!this->MOVES.P[i].moves.empty()) {
      ANY = true;
      break;
    }
  if (!ANY) {
    bool IN_CHECK = ImOAttacked(
        (this->coler == WHITE) ? this->W_king : this->B_king, this);
    if (IN_CHECK)
      this->GAME_OVER = (this->coler == WHITE) ? -1 : 1;
    else
      this->GAME_OVER = 2;
  }
}

// * the 4 choise squares grow from the pone square towrd the middel rows,
//   so they are allways on the bord ( even on the a/h file ).
Vector Bord::PROM_Square(int K) const {
  int IN = ((*this)(this->PROM_CORD) > 0) ? 1 : -1;
  return Vector(this->PROM_CORD.x, this->PROM_CORD.y + K * IN);
}

int8_t Bord::PROM_Clicked(const Vector &CLICK) const {
  using namespace KIND;
  // * 0 = QUIN , 1 = RUKE , 2 = BESHOP , 3 = KNIGHT
  const int8_t CHOISE[4] = {QUIN, RUKE, BESHOP, KNIGHT};
  for (int K = 0; K < 4; ++K)
    if (CLICK == this->PROM_Square(K))
      return CHOISE[K];
  return 0;
}

void Bord::PROM_Pick(int8_t TYPE) {
  // * the pone on the last row becomes the picked peice ...
  int8_t SIDE = ((*this)(this->PROM_CORD) > 0) ? 1 : -1;
  (*this)(this->PROM_CORD) = SIDE * TYPE;

  // * ... and NOW the turn pases ( it was keept by the promotion move )
  this->PROM_CORD = Vector({-1, -1});
  this->UpDate();
}
