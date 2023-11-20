

#include "Move.hpp"
#include "Vector.hpp"

void Print(const std::list<Vector> &moves)
{
    for (Vector CORD : moves)
        CORD.Print();
}

std::list<Vector> Get_Move(const Vector &CORD, Bord *BORD)
{
    std::list<Vector> MOVE = {};
    switch (abs((*BORD)[CORD]))
    {
    case KIND::KING:
        MOVE = casling(CORD, BORD) ;
        break;
    case KIND::QUIN:
        MOVE = ATTACK::QUIN(CORD, BORD);
        break;
    case KIND::KNIGHT:
        MOVE = ATTACK::KNIGHT(CORD, BORD);
        break;
    case KIND::RUKE:
        MOVE = ATTACK::RUCK(CORD, BORD);
        break;
    case KIND::BESHOP:
        MOVE = ATTACK::BESHOP(CORD, BORD);
        break;
    case KIND::PAWN:
        MOVE = P_Pmove(CORD, BORD);
        break;
    default:
        return {};
    }
Repet:
    for (Vector SCAN : MOVE)
        if (!CanImove(CORD, SCAN, BORD))
        {
            SCAN.Print();
            MOVE.remove(SCAN);
            goto Repet;
        }
 
    return MOVE;
}

bool ImOAttacked(const Vector &CORD, Bord *BORD)
{
    for (const Vector &SCAN : ATTACK::RUCK(CORD, BORD))
        if ((*BORD)(SCAN) != 0 && (*BORD)(SCAN) % KIND::RUKE == 0)
            return true;
    for (const Vector &SCAN : ATTACK::BESHOP(CORD, BORD))
        if ((*BORD)(SCAN) != 0 && (*BORD)(SCAN) % KIND::BESHOP == 0)
            return true;
    for (const Vector &SCAN : ATTACK::KING(CORD, BORD))
        if ((*BORD)(SCAN) != 0 && (*BORD)(SCAN) % KIND::RUKE == 0)
            return true;
    for (const Vector &SCAN : ATTACK::KNIGHT(CORD, BORD))
        if ((*BORD)(SCAN) != 0 && (*BORD)(SCAN) % KIND::RUKE == 0)
            return true;
    for (const Vector &SCAN : ATTACK::PAWN(CORD, BORD) )
        if ( abs((*BORD)(SCAN)) == KIND::PAWN)
            return true;

    return false;
}

bool CanImove(const Vector &FROM, const Vector &TO, Bord *BORD)
{
    int to = (*BORD)(TO);   
    BORD->move(FROM, TO);
    bool kingsafty = ImOAttacked(BORD->MyKing(TO), BORD);
    BORD->move(TO, FROM);
    (*BORD)(TO) = to;
    return !kingsafty;
}
 

std::list<Vector> ATTACK::RUCK(const Vector &CORD, Bord *BORD)
{
    std::list<Vector> ATTACK = {};

    for (const Vector &DIR : MOVES::RUCK)
        ITERATION(BORD, ATTACK, CORD, DIR);

    return ATTACK;
}

std::list<Vector> ATTACK::BESHOP(const Vector &CORD, Bord *BORD)
{
    std::list<Vector> ATTACK = {};
    for (const Vector &DIR : MOVES::BESHOP)
        ITERATION(BORD, ATTACK, CORD, DIR);
    return ATTACK;
}

std::list<Vector> ATTACK::KNIGHT(const Vector &CORD, Bord *BORD)
{
    std::list<Vector> ATTACK;

    for (Vector SCAN : MOVES::KNIGHT)
        for (const int &i : {-1, 1})
            if ((*BORD)[CORD + (SCAN * i)] != NaN)
                if (((*BORD)[CORD] * (*BORD)[CORD + (SCAN * i)] <= 0))
                    ATTACK.push_front(CORD + (SCAN * i));

    return ATTACK;
}

std::list<Vector> ATTACK::PAWN(const Vector &CORD, Bord *BORD)
{

    std::list<Vector> ATTACK;
    Vector SCAN;

    SCAN = (Vector({1, - ((*BORD)[CORD]/abs((*BORD)[CORD])) }));

    if ((*BORD)[CORD] * (*BORD)[SCAN + CORD] < 0 &&
        (*BORD)[SCAN + CORD] != NaN)
        ATTACK.push_front(SCAN + CORD);

    // LEFT catchebul POWN
    SCAN *= {-1, 1};
    if ((*BORD)[CORD] * (*BORD)[SCAN + CORD] < 0 &&
        (*BORD)[SCAN + CORD] != NaN)
        ATTACK.push_front(SCAN + CORD);

    return ATTACK;
}

std::list<Vector> ATTACK::QUIN(const Vector &CORD, Bord *BORD)
{
    std::list<Vector> ATTACK = {};

    for (const std::array<Vector, 4> &MOVES : {MOVES::BESHOP, MOVES::RUCK})
        for (const Vector &DIR : MOVES)
            ITERATION(BORD, ATTACK, CORD, DIR);
    return ATTACK;
}

std::list<Vector> ATTACK::KING(const Vector &CORD, Bord *BORD)
{
    std::list<Vector> ATTACK;
    for (const std::array<Vector, 4> &MOVES : {MOVES::BESHOP, MOVES::RUCK})
        for (const Vector &DIR : MOVES)
            if ((*BORD)[CORD] * (*BORD)[CORD + DIR] <= 0 && ((*BORD)[CORD + DIR] != NaN))
                ATTACK.push_front(CORD + DIR);
    return ATTACK;
}
 

void ITERATION(Bord *BORD, std::list<Vector> &ATTACK, const Vector &CORD, const Vector &DIR)
{
    Vector SCAN = CORD + DIR;
    while ( ( (*BORD)[CORD] * (*BORD)[SCAN] <= 0 ) && ((*BORD)[SCAN] != NaN))
    {
        ATTACK.push_front(SCAN);
        if ((*BORD)[SCAN])
            break;
        SCAN += DIR;
    }
}

std::list<Vector> P_Pmove(const Vector &CORD, Bord *BORD)
{
    std::list<Vector> MOVE;

    if (!(*BORD)[(CORD + (DIR::UP * (*BORD)[CORD]))])
        MOVE.push_front((CORD + (DIR::UP * (*BORD)[CORD])));
    if ((*BORD)[CORD + ((DIR::DOWN * 2) * (*BORD)[CORD])] == NaN)
        if (!(*BORD)[CORD + ((DIR::UP * 2) * (*BORD)[CORD])] &&
            !(*BORD)[CORD + (DIR::UP * (*BORD)[CORD])])
                MOVE.push_front(CORD + ((DIR::UP * 2) * (*BORD)[CORD]));
    for (Vector SCAN : ATTACK::PAWN(CORD, BORD))
            MOVE.push_front(SCAN);


    // TODO TODO : on peson 

    std::cout << "============================================================" << std::endl;
    std::cout << " (CORD + DIR::RIGHT) " << (CORD + DIR::RIGHT).str() << std::endl;
    std::cout << " (CORD + DIR::LEFT) " << (CORD + DIR::LEFT).str() << std::endl;
    std::cout << "  (BORD->OOP_CORD) " <<  BORD->OOP_CORD.str() << std::endl;

    if (((CORD + DIR::RIGHT) == BORD->OOP_CORD) ^ ((CORD + DIR::LEFT) == BORD->OOP_CORD))
        
        MOVE.push_front(BORD->OOP_CORD + Vector(0, - (*BORD)[CORD]));

    return MOVE;
}

std::list<Vector> casling(const Vector &KING, Bord *BORD)
{

    std::list<Vector> MOVE = {};

    if (ImOAttacked(KING, BORD))
        return MOVE;
 
    if ((*BORD)(KING) == KIND::KING && (BORD->king7))
    {
        if ((BORD->ruke70) &&
            (CanImove(KING, KING +DIR::LEFT * 1 , BORD)) &&
            (CanImove(KING, KING +DIR::LEFT * 2 , BORD)) &&
            ((*BORD)(KING + DIR::LEFT * 1)  == 0) &&
            ((*BORD)(KING + DIR::LEFT * 2)  == 0) &&
            ((*BORD)(KING + DIR::LEFT * 3)  == 0))
            MOVE.push_front(KING + DIR::LEFT * 2);

        if ((BORD->ruke77) &&
            (CanImove(KING, KING + DIR::RIGHT * 1 , BORD)) &&
            (CanImove(KING, KING + DIR::RIGHT * 2 , BORD)) &&
            ((*BORD)(KING + DIR::RIGHT * 1) == 0) &&
            ((*BORD)(KING + DIR::RIGHT * 2) == 0) )
            MOVE.push_front(KING + DIR::RIGHT * 2  );
    }
    else if ((*BORD)(KING) == -KIND::KING && (BORD->king0))
    {
        if ((BORD->ruke00) &&
            (CanImove(KING, KING + DIR::LEFT * 1, BORD)) &&
            (CanImove(KING, KING + DIR::LEFT * 2, BORD)) &&
            ((*BORD)(KING + DIR::LEFT * 1) == 0) &&
            ((*BORD)(KING + DIR::LEFT * 2) == 0) &&
            ((*BORD)(KING + DIR::LEFT * 3) == 0))
            MOVE.push_front(KING + DIR::LEFT * 3 );

        if ((BORD->ruke07) &&
            (CanImove(KING, KING + DIR::RIGHT * 1, BORD)) &&
            (CanImove(KING, KING + DIR::RIGHT * 2, BORD)) &&
            ((*BORD)(KING + DIR::RIGHT * 1) == 0) &&
            ((*BORD)(KING + DIR::RIGHT * 2) == 0))
            MOVE.push_front(KING + DIR::RIGHT * 2);
    }
    return MOVE;
}





void O_O(Vector From, Bord* BORD) {
    Vector  Rock(From + DIR::RIGHT * 3);

    BORD->move(From, From + DIR::RIGHT * 2);
    BORD->move(Rock, Rock - DIR::RIGHT * 2);
}
void O_O_O(Vector From, Bord* BORD) {
    Vector  Rock(From + DIR::LEFT * 4 );
    
    BORD->move(From, From + DIR::LEFT * 2);
    BORD->move(Rock, Rock - DIR::LEFT * 3);
}


void move(Vector From, Vector To, Bord* BORD) {
    //O(false);
    // TODO : ADD TO HISTORIC  
    switch (abs((*BORD)(From))) {
    case KIND::KING:

    {
        if ((To.x - From.x) == 2) 
            O_O(From, BORD);
        
        else if ((To.x - From.x) == -2) 
            O_O_O(From, BORD);
        else  
            BORD->move(From, To);
    }
        
    break;
        case 1:
    {
        // std::cout << 
        if (abs(To.y - From.y) == 2) {

            BORD->move(From, To);
            BORD->OOP_CORD = To;
        }
        else if (BORD->OOP_CORD == Vector(To.x, From.y) )
        {
            BORD->move(From, To);
            (*BORD)(Vector(To.x, From.y)) = 0;
            BORD->OOP_CORD = Vector({ -1,-1 });
        }
        else
        {
            BORD->OOP_CORD = Vector({ -1,-1 });
            BORD->move(From, To);
        }
    }
        break;
    default:
        BORD->move(From, To);
        break;
    }

    BORD->UpDate();

}
