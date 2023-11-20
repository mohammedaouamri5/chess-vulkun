#ifndef MOVE_HPP
#define MOVE_HPP

#pragma once

#include "Bord.hpp"
#include "Clik.hpp"
#include "RBord.hpp"
#include "Vector.hpp"
#include "iostream"
#include "list"
#include "CONST.hpp"
 



std::list<Vector> Get_Move(const Vector& pies, Bord* BORD);

//|  THE SET OF FUNCTIONS FOR GETTING THE MOVES THAT CAN BE ATTACKED THE KING 
 
namespace ATTACK  
{

    std::list<Vector>   QUIN(const Vector &CORD, Bord *BORD);
    std::list<Vector>   KING(const Vector &CORD, Bord *BORD);
    std::list<Vector> KNIGHT(const Vector &CORD, Bord *BORD);
    std::list<Vector>   RUCK(const Vector &CORD, Bord *BORD);
    std::list<Vector> BESHOP(const Vector &CORD, Bord *BORD);
    std::list<Vector>   PAWN(const Vector &CORD, Bord *BORD);

}

std::list<Vector> P_Pmove(const Vector &CORD, Bord *BORD);

bool ImOAttacked(const Vector& CORD, Bord* BORD);
bool CanImove(const Vector& FROM , const Vector& TO, Bord* BORD);
std::list<Vector> casling(const Vector& KING, Bord* BORD);
void ITERATION(Bord *BORD, std::list<Vector> &ATTACK, const Vector &CORD, const Vector &DIR);
void Print(const std::list<Vector> &moves);

void   O_O(Vector, Vector, Bord*);
void O_O_O(Vector, Vector, Bord*);
void  move(Vector, Vector, Bord*);

#endif