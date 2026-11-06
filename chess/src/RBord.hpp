#pragma once

#ifndef RBORD_HPP
#define RBORD_HPP
#include "CONST.hpp"
#include "Walnut/Application.h"
#include "Vector.hpp"
#include "Walnut/Image.h"
#include "Bord.hpp"
#include "List.hpp"

#include <memory>

 
 
class RBord : public Walnut::Layer
{
public:
	Bord* BORD = new Bord( );
	MoveList MOVES;

public:
	// this function are for randring
	virtual void OnAttach()   override {
		this->bb = std::make_shared<Walnut::Image>("img/bb.png");
		this->bk = std::make_shared<Walnut::Image>("img/bk.png");
		this->bn = std::make_shared<Walnut::Image>("img/bn.png");
		this->bp = std::make_shared<Walnut::Image>("img/bp.png");
		this->bq = std::make_shared<Walnut::Image>("img/bq.png");
		this->br = std::make_shared<Walnut::Image>("img/br.png");
		this->wb = std::make_shared<Walnut::Image>("img/wb.png");
		this->wk = std::make_shared<Walnut::Image>("img/wk.png");
		this->wn = std::make_shared<Walnut::Image>("img/wn.png");
		this->wp = std::make_shared<Walnut::Image>("img/wp.png");
		this->wq = std::make_shared<Walnut::Image>("img/wq.png");
		this->wr = std::make_shared<Walnut::Image>("img/wr.png");
	}
	virtual void OnDetach()   override {
		 
		delete  this->BORD;

	}
	virtual void OnUIRender() override {
		this->Rendere();
	}




	int Valid_move(Vector COORDONET);
	void Set_Move(const MoveList& p_moves);

private:

	void R_image(const ImVec2& POSITHION, const ImVec2& boardSize, const Vector& Pies);
	void Rendere();  
	void GetImage(int8_t pies, ImVec2 dim);
	void DrawChessboard(const ImVec2& startPos, const ImVec2& boardSize, const ImVec4& coler1, const ImVec4& coler2);

private:

	std::shared_ptr<Walnut::Image> bb ;
	std::shared_ptr<Walnut::Image> bk ;
	std::shared_ptr<Walnut::Image> bn ;
	std::shared_ptr<Walnut::Image> bp ;
	std::shared_ptr<Walnut::Image> bq ;
	std::shared_ptr<Walnut::Image> br ;
	std::shared_ptr<Walnut::Image> wb ;
	std::shared_ptr<Walnut::Image> wk ;
	std::shared_ptr<Walnut::Image> wn ;
	std::shared_ptr<Walnut::Image> wp ;
	std::shared_ptr<Walnut::Image> wq ;
	std::shared_ptr<Walnut::Image> wr ;

};





class COSTEM : public Walnut::Layer
{
public:
 
public:
	
	virtual void OnAttach()   override
	{
		
		this->flipe = std::make_shared<Walnut::Image>("img/flip.png");
	}
	virtual void OnDetach()   override {
		 
	}
	virtual void OnUIRender() override {
		this->Rendere();
	}
	
private:
 
	void Rendere(); 

private:

	std::shared_ptr<Walnut::Image> flipe;
	 
};


#endif // !RBORD_HPP

   