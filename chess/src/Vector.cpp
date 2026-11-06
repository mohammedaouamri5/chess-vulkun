

#include "Vector.hpp"

Vector Vector::operator*(const Vector &p_vec) const
{
    return Vector(this->x * p_vec.x, this->y * p_vec.y);
}

Vector Vector::operator*(int p_lamda) const
{
    return Vector(this->x * p_lamda, this->y * p_lamda);
}

Vector Vector::operator/(const Vector &p_vec) const
{
    return Vector(this->x / p_vec.x, this->y / p_vec.y);
}

Vector Vector::operator/(int p_lamda) const
{ 
    return Vector(this->x / p_lamda, this->y / p_lamda);
}

Vector Vector::operator+(const Vector &p_vec) const 
{
    return Vector(this->x + p_vec.x, this->y + p_vec.y);
}

Vector Vector::operator+(int p_lamda) const
{
    return Vector(this->x + p_lamda, this->y + p_lamda);
}

Vector Vector::operator-(const Vector &p_vec) const
{
    return Vector(this->x - p_vec.x, this->y - p_vec.y);
}
Vector Vector::operator-(int p_lambda) const
{
    return Vector(this->x - p_lambda, this->y - p_lambda);
}

int Vector::operator>(const Vector &p_vec) const
{
    return ((this->x > p_vec.x) && (this->y > p_vec.y));
}
int Vector::operator<(const Vector &p_vec) const
{
    return ((this->x < p_vec.x) && (this->y < p_vec.y));
}

bool Vector::operator==(const Vector &other) const
{
    // Compare the data members of both objects for equality
    return (this->x == other.x && this->y == other.y);
}
void Vector::operator+=(const Vector& other) 
{
    this->x += other.x;
    this->y += other.y;
}
void Vector::operator+=(const int& other) 
{
    this->x += other;
    this->y += other;
}
void Vector::operator*=(const Vector& other) 
{
    this->x *= other.x;
    this->y *= other.y;
}
void Vector::operator*=(const int& other) 
{
    this->x *= other;
    this->y *= other;
}

void Vector::UP()
{
    this->y--;
}
void Vector::RIGHT()
{
    this->x++;
}
void Vector::DOWN()
{
    this->y++;
}
void Vector::LEFT()
{
    this->x--;
}