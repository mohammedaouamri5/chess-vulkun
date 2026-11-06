#define CHESSLOG_MODULE BORD
#include "Move.hpp"
#include "Log.hpp"
#include "Vector.hpp"

MoveList Get_Move(const Vector &CORD, Bord *BORD) {
  MoveList MOVE;
  if (!BORD->IsMyTurn(CORD))
    return MOVE;
  switch (abs((*BORD)[CORD])) {
  case KIND::KING:
    casling(CORD, BORD, MOVE);
    ATTACK::KING(CORD, BORD, MOVE);
    break;
  case KIND::QUIN:
    ATTACK::QUIN(CORD, BORD, MOVE);
    break;
  case KIND::KNIGHT:
    ATTACK::KNIGHT(CORD, BORD, MOVE);
    break;
  case KIND::RUKE:
    ATTACK::RUCK(CORD, BORD, MOVE);
    break;
  case KIND::BESHOP:
    ATTACK::BESHOP(CORD, BORD, MOVE);
    break;
  case KIND::PAWN:
    P_Pmove(CORD, BORD, MOVE);
    break;
  default:
    return MOVE;
  }

  // keep only the moves that leave my king safe (one pass, no remove / goto)
  int KEEP = 0;
  for (int i = 0; i < MOVE.N; ++i)
    if (CanImove(CORD, MOVE.M[i], BORD))
      MOVE.M[KEEP++] = MOVE.M[i];
  MOVE.N = KEEP;

  return MOVE;
}

// is CORD attacked by the side BY ? ( 1 = white , -1 = black )
// no piece needed on CORD, and no lists : just scan the rays
bool IsAttacked(const Vector &CORD, int BY, Bord *BORD) {

  // * ➡ ⬇ ⬅ ⬆
  for (const Vector &DIR : MOVES::RUCK) {
    Vector SCAN = CORD + DIR;
    while ((*BORD)[SCAN] == 0)
      SCAN += DIR;
    int piece = (*BORD)[SCAN];
    if (piece * BY > 0 && (abs(piece) == KIND::RUKE || abs(piece) == KIND::QUIN))
      return true;
  }

  for (const Vector &DIR : MOVES::BESHOP) {
    Vector SCAN = CORD + DIR;
    while ((*BORD)[SCAN] == 0)
      SCAN += DIR;
    int piece = (*BORD)[SCAN];
    if (piece * BY > 0 &&
        (abs(piece) == KIND::BESHOP || abs(piece) == KIND::QUIN))
      return true;
  }

  for (const std::array<Vector, 4> &MOVES : {MOVES::BESHOP, MOVES::RUCK})
    for (const Vector &DIR : MOVES) {
      int piece = (*BORD)[CORD + DIR];
      if (piece * BY > 0 && abs(piece) == KIND::KING)
        return true;
    }

  for (const Vector &SCAN : MOVES::KNIGHT)
    for (const int &i : {-1, 1}) {
      int piece = (*BORD)[CORD + (SCAN * i)];
      if (piece * BY > 0 && abs(piece) == KIND::KNIGHT)
        return true;
    }

  // a pawn of side BY attacks CORD from (±1 , BY) :
  // white pawn attacks up (y - 1) so it sits one row below (y + 1)
  for (const int &DX : {-1, 1})
    if ((*BORD)[CORD + Vector(DX, BY)] == BY * KIND::PAWN)
      return true;

  return false;
}

// is the piece on CORD attacked ( by the other side )
bool ImOAttacked(const Vector &CORD, Bord *BORD) {
  int ME = (*BORD)[CORD];
  if (ME == 0 || ME == NaN)
    return false;
  return IsAttacked(CORD, (ME > 0) ? -1 : 1, BORD);
}

bool CanImove(const Vector &FROM, const Vector &TO, Bord *BORD) {

  Vector MyKing = BORD->MyKing(FROM);
  if (MyKing == FROM)
    MyKing = TO;
  int to = (*BORD)(TO);

  // en passant : a pawn goes diagonal on an empty square, the pawn that
  // gets eaten is beside FROM ( not on TO ) so remove it for the test
  bool EP = abs((*BORD)(FROM)) == KIND::PAWN && TO.x != FROM.x && to == 0;
  Vector VICTIM(TO.x, FROM.y);
  int victim = 0;

  BORD->move(FROM, TO);
  if (EP) {
    victim = (*BORD)(VICTIM);
    (*BORD)(VICTIM) = 0;
  }
  bool kingsafty = ImOAttacked(MyKing, BORD);
  if (EP)
    (*BORD)(VICTIM) = victim;
  BORD->move(TO, FROM);
  (*BORD)(TO) = to;
  return !kingsafty;
}

void ATTACK::RUCK(const Vector &CORD, Bord *BORD, MoveList &OUT) {
  for (const Vector &DIR : MOVES::RUCK)
    ITERATION(BORD, OUT, CORD, DIR);
}

void ATTACK::BESHOP(const Vector &CORD, Bord *BORD, MoveList &OUT) {
  for (const Vector &DIR : MOVES::BESHOP)
    ITERATION(BORD, OUT, CORD, DIR);
}

void ATTACK::KNIGHT(const Vector &CORD, Bord *BORD, MoveList &OUT) {

  for (Vector SCAN : MOVES::KNIGHT)
    for (const int &i : {-1, 1})
      if ((*BORD)[CORD + (SCAN * i)] != NaN)
        if (((*BORD)[CORD] * (*BORD)[CORD + (SCAN * i)] <= 0))
          OUT.add(CORD + (SCAN * i));
}

// only the diagonals where there is something to eat
void ATTACK::PAWN(const Vector &CORD, Bord *BORD, MoveList &OUT) {

  int ME = (*BORD)[CORD];
  int SIDE = (ME > 0) ? 1 : -1; // no more  ME / abs(ME)  ( divide by 0 )
  Vector SCAN;

  SCAN = Vector(1, -SIDE);

  if (ME * (*BORD)[SCAN + CORD] < 0 && (*BORD)[SCAN + CORD] != NaN)
    OUT.add(SCAN + CORD);

  // LEFT catchebul POWN
  SCAN *= {-1, 1};
  if (ME * (*BORD)[SCAN + CORD] < 0 && (*BORD)[SCAN + CORD] != NaN)
    OUT.add(SCAN + CORD);
}

void ATTACK::QUIN(const Vector &CORD, Bord *BORD, MoveList &OUT) {

  for (const std::array<Vector, 4> &MOVES : {MOVES::BESHOP, MOVES::RUCK})
    for (const Vector &DIR : MOVES)
      ITERATION(BORD, OUT, CORD, DIR);
}

void ATTACK::KING(const Vector &CORD, Bord *BORD, MoveList &OUT) {
  for (const std::array<Vector, 4> &MOVES : {MOVES::BESHOP, MOVES::RUCK})
    for (const Vector &DIR : MOVES)
      if ((*BORD)[CORD] * (*BORD)[CORD + DIR] <= 0 &&
          ((*BORD)[CORD + DIR] != NaN))
        OUT.add(CORD + DIR);
}

void ITERATION(Bord *BORD, MoveList &OUT, const Vector &CORD,
               const Vector &DIR) {
  Vector SCAN = CORD + DIR;
  while (((*BORD)[CORD] * (*BORD)[SCAN] <= 0) && ((*BORD)[SCAN] != NaN)) {
    OUT.add(SCAN);
    if ((*BORD)[SCAN])
      break;
    SCAN += DIR;
  }
}

void P_Pmove(const Vector &CORD, Bord *BORD, MoveList &OUT) {

  int SIDE = ((*BORD)(CORD) > 0) ? 1 : -1;

  if (!(*BORD)[(CORD + (DIR::UP * (*BORD)[CORD]))])
    OUT.add((CORD + (DIR::UP * (*BORD)[CORD])));

  // double push : ONLY from the home rank ( white row 6 , black row 1 )
  // ( the old check on the square 2 rows behind the pawn was wrong and
  //   let pawns jump 2 squares from any rank )
  int HOME_Y = (SIDE > 0) ? 6 : 1;
  if (CORD.y == HOME_Y) {
    if (!(*BORD)[CORD + (DIR::UP * 2 * SIDE)] &&
        !(*BORD)[CORD + (DIR::UP * SIDE)])
      OUT.add(CORD + (DIR::UP * 2 * SIDE));
  }
  ATTACK::PAWN(CORD, BORD, OUT);

  // on peson : the pawn that just double pushed is right beside me
  // and it must be an enemy pawn ( OOP_CORD is reset on every move )
  if (((CORD + DIR::RIGHT) == BORD->OOP_CORD) ||
      ((CORD + DIR::LEFT) == BORD->OOP_CORD))
    if ((*BORD)[CORD] * (*BORD)[BORD->OOP_CORD] < 0)

      OUT.add(BORD->OOP_CORD + Vector(0, -(*BORD)[CORD]));
}

void casling(const Vector &KING, Bord *BORD, MoveList &OUT) {

  if (ImOAttacked(KING, BORD))
    return;

  // the square where the king lands is checked later by Get_Move,
  // here only the square he passes through
  if ((*BORD)(KING) == KIND::KING && (BORD->king7)) {
    if ((BORD->ruke70) && ((*BORD)(KING + DIR::LEFT * 1) == 0) &&
        ((*BORD)(KING + DIR::LEFT * 2) == 0) &&
        ((*BORD)(KING + DIR::LEFT * 3) == 0) &&
        (CanImove(KING, KING + DIR::LEFT * 1, BORD)))
      OUT.add(KING + DIR::LEFT * 2);

    if ((BORD->ruke77) && ((*BORD)(KING + DIR::RIGHT * 1) == 0) &&
        ((*BORD)(KING + DIR::RIGHT * 2) == 0) &&
        (CanImove(KING, KING + DIR::RIGHT * 1, BORD)))
      OUT.add(KING + DIR::RIGHT * 2);
  } else if ((*BORD)(KING) == -KIND::KING && (BORD->king0)) {
    if ((BORD->ruke00) && ((*BORD)(KING + DIR::LEFT * 1) == 0) &&
        ((*BORD)(KING + DIR::LEFT * 2) == 0) &&
        ((*BORD)(KING + DIR::LEFT * 3) == 0) &&
        (CanImove(KING, KING + DIR::LEFT * 1, BORD)))
      OUT.add(KING + DIR::LEFT * 2); // was LEFT * 3 : king landed on the rook

    if ((BORD->ruke07) && ((*BORD)(KING + DIR::RIGHT * 1) == 0) &&
        ((*BORD)(KING + DIR::RIGHT * 2) == 0) &&
        (CanImove(KING, KING + DIR::RIGHT * 1, BORD)))
      OUT.add(KING + DIR::RIGHT * 2);
  }
}

void O_O(Vector From, Bord *BORD) {
  Vector Rock(From + DIR::RIGHT * 3);

  BORD->move(From, From + DIR::RIGHT * 2);
  BORD->move(Rock, Rock - DIR::RIGHT * 2);
}
void O_O_O(Vector From, Bord *BORD) {
  Vector Rock(From + DIR::LEFT * 4);

  BORD->move(From, From + DIR::LEFT * 2);
  BORD->move(Rock, Rock - DIR::LEFT * 3);
}

void move(Vector From, Vector To, Bord *BORD) {
  // O(false);
  //  TODO : ADD TO HISTORIC
  int SIDE = ((*BORD)(From) > 0) ? 1 : -1;
  bool LAST_ROW = false; // the pone arived at the last row ( promotion pick )

  // always reset, only a double push can set it again
  BORD->OOP_CORD = Vector({-1, -1});

  switch (abs((*BORD)(From))) {
  case KIND::KING:

  {
    if ((To.x - From.x) == 2)
      O_O(From, BORD);

    else if ((To.x - From.x) == -2)
      O_O_O(From, BORD);
    else
      BORD->move(From, To);

    // MyKing() reads these, they must follow the king
    if (SIDE > 0)
      BORD->W_king = To;
    else
      BORD->B_king = To;
  }

  break;
  case KIND::PAWN: {
    if (abs(To.y - From.y) == 2) {

      BORD->move(From, To);
      BORD->OOP_CORD = To;
    } else if (To.x != From.x && (*BORD)(To) == 0) {
      // on peson : the eaten pawn is beside From, not on To
      BORD->move(From, To);
      (*BORD)(Vector(To.x, From.y)) = 0;
    } else {
      BORD->move(From, To);
    }

    // * the pone arived at the last row : it stays a pone and the turn does
    //   NOT pass , the next pick is what piece he wants to become
    if (To.y == 0 || To.y == 7) {
      BORD->PROM_CORD = To;
      LAST_ROW = true;
    }
  } break;
  default:
    BORD->move(From, To);
    break;
  }

  BORD->UpDate(!LAST_ROW); // the promotion does not pass the turn
}
