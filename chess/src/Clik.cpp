#define CHESSLOG_MODULE BORD
#include "Clik.hpp"
#include "Log.hpp"
#include "Move.hpp"
#include "Vector.hpp"
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
     
    using namespace GAME;

    // * game over ( checkmate / stalemate ) : no clicks responde
    if (BORD->GAME_OVER != 0)
        return;

    // * promotion pick : the pone is on the last row , the ONLY thing a
    //   board click can do is choose the piece ( nothing else responds )
    if (BORD->IsPending()) {
        int8_t TYPE = BORD->PROM_Clicked(CORD);
        if (TYPE) {
            BORD->PROM_Pick(TYPE);
            LOG_INFO("pone promoted")
                .with_variable("square", CORD)
                .with_variable("piece", (int)TYPE)
                .log();
        }
        return;
    }

#ifndef WL_DEBUG

    if (!((g % 2) ^ (((*BORD)[CORD] * tourn) > 0))) {
#endif // WL_DEBUG
        if (g % 2) {
            click1 = CORD;
                if ((*BORD)(click1)) {
                    // the moves are already in the MAP ( made in UpDate )
                    WIN.Set_Move(BORD->MOVES.get_(click1)); 
                    LOG_INFO("pick piece").with_variable("square", CORD).log();
                    g += 1;
                }
        }
        else {
            click2 = CORD;
            if (WIN.Valid_move(click2)) {
                g += 1;
                tourn = -tourn;

                move(click1, click2, BORD);
                LOG_INFO("piece moved")
                    .with_variable("from", click1)
                    .with_variable("to", click2)
                    .log();
                // UpGrade(); ReFrecheTheBord();
                WIN.Set_Move({});
            }
            else {
                LOG_WARN("rejected, not a legal square")
                    .with_variable("square", CORD)
                    .log();
                // clear the selection ( no recursion ) ...
                g = 1;
                WIN.Set_Move({});
                click1 = {-1, -1};
                // ... and re-select in the same pass if a own peice was clicked
                if (((*BORD)[CORD] * tourn) > 0) {
                    click1 = CORD;
                    WIN.Set_Move(BORD->MOVES.get_(click1));
                    LOG_INFO("pick piece").with_variable("square", CORD).log();
                    g = 2;
                }
            }

        }
#ifndef WL_DEBUG
    }
    else {
        // switching selection to another own peice ( no recursion )
        if (((*BORD)[click1] * (*BORD)[CORD]) > 0) {
            g = 1;
            WIN.Set_Move({});
            click1 = CORD;
            WIN.Set_Move(BORD->MOVES.get_(click1));
            LOG_INFO("pick piece").with_variable("square", CORD).log();
            g = 2;
        }
    }
#endif // WL_DEBUG
}
            
            
 