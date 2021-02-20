#ifndef BALL_H
#define BALL_H

#include "Object.hpp"
#include <string>

class Ball : public Object
{
public:
	Ball(int init_x,int init_y,int _radius,int _mass);
	int CollideWithEdges(int width,int height);

	std::string GetBallImage(){return ball_image;}
private:
	std::string ball_image;
};


#endif