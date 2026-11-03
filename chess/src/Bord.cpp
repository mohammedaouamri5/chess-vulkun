

#include "Bord.hpp"
#include "CONST.hpp"

Vector Bord::MyKing(const Vector& Me) {
    if (abs((*this)(Me)) == KIND::KING)
        return Me;
    if ((*this)(Me) > 0)
        return W_king;
    return B_king;
}


void Bord::Kings()
{
   
    // *⬇
    for (int i = 0; i < 8; ++i)
        // * ➡
        for (int j = 0; j < 8; ++j)

            if ((*this)[{j, i}] == KIND::KING)

                W_king = {j, i};

            else if ((*this)[{j, i}] == - KIND::KING)

                B_king = {j, i};
}

Bord::Bord(const Bord &other)

{

    this->W_king = other.W_king;
    this->B_king = other.B_king;
    this->king0 = other.king0;
    this->king7 = other.king7;
    this->ruke00 = other.ruke00;
    this->ruke07 = other.ruke07;
    this->ruke70 = other.ruke70;
    this->ruke77 = other.ruke77;
     
    this->The_Bord = new int[64];
    for (int i = 0; i < 64; ++i)
        this->The_Bord[i] = other.The_Bord[i];
}

Bord::Bord()
{ 
    int bord_main[64] = {-5, -2, -3, -15, -2401, -3, -2, -5,
                         -1, -1, -1, -1, -1, -1, -1, -1,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0,
                         1, 1, 1, 1, 1, 1, 1, 1,
                         5, 2, 3, 15, 2401, 3, 2, 5};
    this->The_Bord = new int[64];

    for (int i = 0; i < 64; ++i)
        this->The_Bord[i] = bord_main[i];
    Kings();
    this->king0 = true;
    this->king7 = true;
    this->ruke00 = true;
    this->ruke07 = true;
    this->ruke70 = true;
    this->ruke77 = true;
     
}

const int& Bord::operator[](Vector p_vec) const {
    if (0 <= p_vec.x &&
        0 <= p_vec.y &&
        p_vec.x < 8 &&
        p_vec.y < 8)
    {
        return  (this->The_Bord[p_vec.x + 8 * p_vec.y]);
    }
    return  NaN;
}

Bord::~Bord()
{
    delete[] this->The_Bord;
}

void Bord::UpDate()
{
    using namespace  KIND;
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
}


// const int& Bord::operator[](Vector p_vec) const {
//     if (0 <= p_vec.x &&
//         0 <= p_vec.y &&
//         p_vec.x < 8 &&
//         p_vec.y < 8)
//     {
//         return  (The_Bord[p_vec.x + 8 * p_vec.y]);
//     }
//     return  NaN;
// }

