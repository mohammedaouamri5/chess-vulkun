
#pragma once
#ifndef CLICK_HPP
#define CLICK_HPP

#include "RBord.hpp"
#include "Vector.hpp"

// the difrence  betwin (x ; y) and (col , row) is that
// (x , y) is the coordonet of the mose in the window 
// (col , row ) i the coordonit of the mose in the bord 
void Move_Handler(const Vector& CORD , RBord & BORD ) ;
 
#endif // CLICK_HPP