#ifndef BORD_HPP
#define BORD_HPP

#pragma once


#include "istream"
#include "Vector.hpp"


  


class Bord
{
private:
    const int NaN = 91;
public:
    bool king0, king7, ruke00, ruke07, ruke70, ruke77;
    void Kings();
    int *The_Bord;
    Vector W_king, B_king;
    Vector OOP_CORD = Vector({-1,-1});//onpeson pown cord
    
    void move(const Vector& from, const Vector& to) {
        (*this)(to) = (*this)(from);
        (*this)(from) = 0;
    }
    
    Vector MyKing(const Vector& Me);
    Bord();
    
    Bord(const Bord &other);
    ~Bord();
    // * (➡ , ⬇)
    const int& operator[](Vector p_vec) const; 
    int& operator()(Vector p_vec) const {
        if (0 <= p_vec.x &&
            0 <= p_vec.y &&
            p_vec.x < 8 &&
            p_vec.y < 8)
       
            return  (The_Bord[p_vec.x + 8 * p_vec.y]);
    }
    
    void UpDate() ;
    void Print(Vector cord)
    {
        if ((*this)[(cord)] >= 0)
            std::cout << " ";
        std::cout << (*this)[(cord)] << "<==>";
        cord.Print();
        printf("\n");
    }



};


#endif // BORD_HPP