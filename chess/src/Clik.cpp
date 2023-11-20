
#include "Clik.hpp"
#include "Move.hpp"
#include "Vector.hpp"
#include "iostream"
#include "list"
/*
 * THE SIZE      :  is a (x , y) that contin the withe and the hight of the window
 * THE EDGE      :  is a (x , y) that contin the distens biween the bord and thee egde of the window
 * THE POSITION  :  is a (x , y) that contin the pposition of the mouse acording to the window
 * THE COORDONET :  is a (col , row) that contain tha cordonet of the mose acording to the bord
 */
 
/*

   @param g           : is for makeing the movement
   \param click(1/2) :  are for storing the clicks
   \param tourn     : is for make sure that the any player make one move

*/
namespace GAME
{
    int g = 1;
    int tourn = 1;
    Vector click1 = {-1, -1};
    Vector click2 = {-1, -1};
} // namespace GAME
 
 
void Move_Handler(const Vector& CORD, RBord & WIN) {
    Bord* BORD = WIN.BORD;
     
    std::list<Vector> moves = {};
    using namespace GAME;

#ifndef WL_DEBUG

    if (!((g % 2) ^ (((*BORD)[CORD] * tourn) > 0))) {
#endif // WL_DEBUG
        if (g % 2) {
            click1 = CORD;
                if ((*BORD)(click1)) {
                    WIN.Set_Move(Get_Move(CORD, BORD)); 
                    g += 1;
                }
        }
        else {
            click2 = CORD;
            if (WIN.Valid_move(click2)) {
                g += 1;
                tourn = -tourn;

                move(click1, click2, BORD);
                // UpGrade(); ReFrecheTheBord();
                WIN.Set_Move({});
            }
            else {
                g -= 1;
                Move_Handler(CORD, WIN);
            }

        }
#ifndef WL_DEBUG
    }
    else {
        if (((*BORD)[click1] * (*BORD)[CORD] ) > 0) {
            g -= 1; 
            WIN.Set_Move({});
            Move_Handler(CORD, WIN);
        }
    }   
#endif // WL_DEBUG
}
            
            
 