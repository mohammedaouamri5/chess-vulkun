#pragma once
#ifndef Vector_HPP
#define Vector_HPP

#include"iostream"
#include <string>
/*
 * THE SIZE      :  is a (x , y) that contin the withe and the hight of the window 
 * THE EDGE      :  is a (x , y) that contin the distens biween the bord and thee egde of the window
 * THE POSITION  :  is a (x , y) that contin the pposition of the mouse acording to the window
 * THE COORDONET :  is a (col , row) that contain tha cordonet of the mose acording to the bord
*/
struct Vector
{
	Vector()
		: x((int)0), y((int)0)
	{
	}

	Vector(int p_x, int p_y): x(p_x), y(p_y){}

	void Print()
	{
		std::cout << x << ", " << y << std::endl;
	}
	std::string str() {
		return std::string("{ " + std::to_string(x) + " , " + std::to_string(y) + " }");
	}
	int x, y;
	Vector operator*(const Vector& p_vec) const;
	Vector operator*(int p_lamda) const;

	Vector operator/(const Vector& p_vec) const;
	Vector operator/(int p_lamda) const;


	Vector operator+(const Vector& p_vec) const;
	Vector operator+(int p_lamda) const;

	Vector operator-(const Vector& p_vec) const;
	Vector operator-(int p_lambda) const;

	int operator<(const Vector& p_vec) const;
	int operator>(const Vector& p_vec) const;
 	bool operator==(const Vector& other) const;
	void operator+=(const Vector& other);
	void operator+=(const int& other);
	void operator*=(const Vector& other);
	void operator*=(const int& other);
	void UP();
	void RIGHT();
	void DOWN();
	void LEFT();
	
	
 

};




#endif // Vector_HPP