#include "Player.hpp"
#include<cmath>
#define TEAM_A 1
#define TEAM_B 2
#define PLAYER1_SRC "Assets/Player1.png"
#define PLAYER2_SRC "Assets/Player2.png"
#define PLAYER_ACCELERATION 300
//Constructor
Player::Player(int init_x,int init_y,int _radius,int _mass,int _team_id) : Object(init_x,init_y,_radius,_mass)
{
	if(_team_id == TEAM_A) player_image = PLAYER1_SRC;
	if(_team_id == TEAM_B) player_image = PLAYER2_SRC;
	acceleration = PLAYER_ACCELERATION;
}

void Player::CollideWithEdges(int width,int height)
{
	int top_corner = y - radius;
	int bottom_corner = y + radius;
	int left_corner = x - radius;
	int right_corner = x + radius;
	if(top_corner <= 0) {Reflect("y"); y = radius;}
	else if(bottom_corner >= height) {Reflect("y"); y = height - radius;}
	else if(left_corner <= 0) {Reflect("x"); x = radius;}
	else if(right_corner >= width) {Reflect("x"); x = width - radius;}
}