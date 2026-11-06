#ifndef MOVE_HPP
#define MOVE_HPP

#pragma once

#include "Bord.hpp"
#include "CONST.hpp"
#include "Clik.hpp"
#include "List.hpp"
#include "RBord.hpp"
#include "Vector.hpp"

MoveList Get_Move(const Vector &CORD, Bord *BORD);
bool IsAttacked(const Vector &CORD, int BY, Bord *BORD);
bool ImOAttacked(const Vector &CORD, Bord *BORD);
bool CanImove(const Vector &FROM, const Vector &TO, Bord *BORD);
void ITERATION(Bord *BORD, MoveList &OUT, const Vector &CORD,
               const Vector &DIR);
void P_Pmove(const Vector &CORD, Bord *BORD, MoveList &OUT);
void casling(const Vector &KING, Bord *BORD, MoveList &OUT);

//|  THE SET OF FUNCTIONS FOR GETTING THE MOVES THAT CAN BE ATTACKED THE KING

namespace ATTACK {
void RUCK(const Vector &CORD, Bord *BORD, MoveList &OUT);
void BESHOP(const Vector &CORD, Bord *BORD, MoveList &OUT);
void KNIGHT(const Vector &CORD, Bord *BORD, MoveList &OUT);
void KING(const Vector &CORD, Bord *BORD, MoveList &OUT);
void QUIN(const Vector &CORD, Bord *BORD, MoveList &OUT);
void PAWN(const Vector &CORD, Bord *BORD, MoveList &OUT);
} // namespace ATTACK
// namespace ATTACK : RUCK, BESHOP, KNIGHT, PAWN, QUIN, KING
//   all become  void NAME(const Vector &CORD, Bord *BORD, MoveList &OUT);

void O_O(Vector, Bord *);
void O_O_O(Vector, Bord *);
void move(Vector, Vector, Bord *);

#endif
