#include "Game.hpp"
#include "Object.hpp"
#include "TextInput.hpp"
#include<cmath>
#define PLAYERS 5
#define BALL 0
#define TEAM_A 1
#define TEAM_B 2
#define PLAYER_RADIUS 30
#define PLAYER_MASS 1
#define BALL_RADIUS 14
#define BALL_MASS 1
#define FRAME_RATE 0.015
#define FIELD_SRC "Assets/field.jpg"
#define FONT_SRC "./Assets/FreeSans.ttf"
#define STAR_SRC "Assets/star.png"
#define HEADER_SRC "Assets/header.png"
#include<vector>
//Constructor
Game::Game(int _throw_radius,int _max_initial_speed,int _width,int _height, Window* _window) : Physics(_throw_radius,_max_initial_speed, _width, _height)
{
	goal_status = 0;
	goal_team_A = 0;
	goal_team_B = 0;
	selected_player = nullptr;
	finished = false;
	is_mouse_down = false;
	winner = 0;
	window = _window;
	turn = TEAM_A;
	SetPreloadPositions();
	teamA = new Team(CreatePlayers(TEAM_A));
	teamB = new Team(CreatePlayers(TEAM_B));
	ball = new Ball(preload_position[BALL][0].x,preload_position[BALL][0].y,BALL_RADIUS,BALL_MASS);
}

void Game::StartGame()
{
	rounds = GetInput("Rounds");
	goal_per_round = GetInput("Goal Per Round");
	Render();
}

void Game::SetPreloadPositions()
{
	//Poisition of Ball
	preload_position[BALL].push_back(Point(390,220));
	//Poisition of team A players
	preload_position[TEAM_A].push_back(Point(50,240));
	preload_position[TEAM_A].push_back(Point(160,120));
	preload_position[TEAM_A].push_back(Point(160,360));
	preload_position[TEAM_A].push_back(Point(300,190));
	preload_position[TEAM_A].push_back(Point(300,290));
	//Poisition of team B players
	preload_position[TEAM_B].push_back(Point(750,240));
	preload_position[TEAM_B].push_back(Point(640,120));
	preload_position[TEAM_B].push_back(Point(640,360));
	preload_position[TEAM_B].push_back(Point(500,190));
	preload_position[TEAM_B].push_back(Point(500,290));
}

std::vector<Player*> Game::CreatePlayers(int team_id)
{
	std::vector<Player*> players;
	for(int i = 0;i < PLAYERS;i++)
	{
		int pos_x = preload_position[team_id][i].x;
		int pos_y = preload_position[team_id][i].y;
		players.push_back(new Player(pos_x,pos_y,PLAYER_RADIUS,PLAYER_MASS,team_id));
	}
	return players;
}

void Game::Render()
{
	while(true)
	{
		window->clear();
		Update();
		if(finished) ShowFinishedScreen();
		else ShowGame();
		delay(FRAME_RATE*1000);
	}
}

int Game::GetInput(std::string filed_name)
{
	try
	{
		TextInputWindow textInputWindow(width,height,filed_name,window);
		textInputWindow.display();
		return stoi(textInputWindow.getText());
	}
	catch(string exception)
	{
		cerr << "EXCEPTION: " << exception << endl;
		exit(1);
	}
}

void Game::ShowGame()
{
	MoveObjects();
	GameStatus();
	//Draw Background
	DrawBackground();
	//Draw Header
	DrawHeader();
	//Draw Ball
	DrawBall();
	//Draw Player
	DrawPlayer(TEAM_A);
	DrawPlayer(TEAM_B);
	DrawStars();
	window->update_screen();
}

void Game::DrawStars()
{
	Team *team;
	if(turn == TEAM_A) team = teamA;
	if(turn == TEAM_B) team = teamB;
	for(int i = 0;i < PLAYERS;i++)
	{
		int x = team->GetPlayer(i)->GetX() - 30;
		int y = team->GetPlayer(i)->GetY() - 30;
		window->draw_img(STAR_SRC, Rectangle(x,y,60,60));
	}
}
void Game::DrawHeader()
{
	window->draw_img(HEADER_SRC, Rectangle(0,0,width,50));
	window->show_text("Team A   Goals: "+std::to_string(teamA->GetTotalGoals()),Point(10,10),WHITE,FONT_SRC, 20);
	window->show_text("Goals: "+std::to_string(teamB->GetTotalGoals()) + "   Team B",Point(width-200,10),WHITE,FONT_SRC, 20);
	std::string text = "Team ";
	RGB color(0,0,0);
	if(turn == TEAM_A)
	{
		text += "A";
		color = RGB(0,128,0);
	}
	else
	{
		text += "B";
		color = RGB(255,0,0);
	}
	window->show_text(text,Point(width/2-30,10),color,FONT_SRC, 20);
}

void Game::ShowFinishedScreen()
{
	window->draw_rect(Rectangle(0,0,width,height), BLACK);
	std::string message = "Team ";
	std::string goals   = "Goals: ";
	std::string rounds   = "Rounds: ";
	if(winner == TEAM_A)
	{
		message += "A won!";
		goals   += std::to_string(teamA->GetTotalGoals());
		rounds  += std::to_string(teamA->GetWonRounds());
	}
	else if(winner == TEAM_B)
	{
		message += "B won!";
		goals   += std::to_string(teamB->GetTotalGoals());
		rounds  += std::to_string(teamB->GetWonRounds());
	}
	window->show_text(message, Point(250, 150), WHITE, FONT_SRC, 50);
	window->show_text(goals, Point(320, 220), WHITE, FONT_SRC, 30);
	window->show_text(rounds, Point(320, 270), WHITE, FONT_SRC, 30);
	window->update_screen();
}

void Game::GameStatus()
{
	if(goal_status != 0 && !finished)
	{
		GoalStatus();
		goal_status = 0;
		RoundStatus();
		Reset();
	}

	if(IsRoundFinished(teamA))
	{
		finished = true;
		winner = TEAM_A;
	}
	else if(IsRoundFinished(teamB))
	{
		finished = true;
		winner = TEAM_B;
	}
}

bool Game::IsRoundFinished(Team *team)
{
	return team->GetWonRounds() >= ceil(rounds/2.0);
}

void Game::RoundStatus()
{
	if(goal_team_A >= goal_per_round || goal_team_B >= goal_per_round)
	{
		if(goal_team_A >= goal_per_round)
		{
			teamA->IncreaseWonRounds();
		}
		else
		{
			teamB->IncreaseWonRounds();
		}
		goal_team_A = 0,goal_team_B = 0;
	}
}

void Game::GoalStatus()
{
	if(goal_status == TEAM_A)
	{
		goal_team_B++;
		teamB->IncreaseTotalGoals();
	}
	if(goal_status == TEAM_B)
	{
		goal_team_A++;
		teamA->IncreaseTotalGoals();
	}
}

void Game::DrawBackground()
{
	window->draw_img(FIELD_SRC, Rectangle(0,0,width,height));
}

void Game::Reset()
{
	ball->SetX(preload_position[BALL][0].x);
	ball->SetY(preload_position[BALL][0].y);
	ball->SetVx(0);
	ball->SetVy(0);
	ResetPlayerPosition(TEAM_A);
	ResetPlayerPosition(TEAM_B);
}

void Game::ResetPlayerPosition(int team_id)
{
	Team *team;
	if(team_id == TEAM_A) team = teamA;
	else if(team_id == TEAM_B) team = teamB;
	for(int i = 0;i < PLAYERS;i++)
	{
		int pos_x = preload_position[team_id][i].x;
		int pos_y = preload_position[team_id][i].y;
		team->GetPlayer(i)->SetX(pos_x);
		team->GetPlayer(i)->SetY(pos_y);
		team->GetPlayer(i)->SetVx(0);
		team->GetPlayer(i)->SetVy(0);
	}
}

void Game::Update()
{
	is_mouse_down = false;
	while(window->has_pending_event())
	{
		Event e = window->poll_for_event();
		switch(e.get_type())
		{
			case Event::EventType::QUIT:
			{
				exit(0);
				break;
			}
			case Event::EventType::LCLICK:
			{
				LeftClick(e);
			}
			case Event::EventType::LRELEASE:
			{
				LeftRelease(e);
			}
		}
	}
}

void Game::LeftClick(Event e)
{
	Point mouse_position = e.get_mouse_position();
	if(turn == TEAM_A) SelectPawn(TEAM_A,mouse_position);
	else SelectPawn(TEAM_B,mouse_position);
	is_mouse_down = true;
}

void Game::LeftRelease(Event e)
{
	if(selected_player != nullptr && !is_mouse_down)
	{
		StartMove(e,selected_player);
		selected_player = nullptr;
		if(turn == TEAM_A) turn = TEAM_B;
		else turn = TEAM_A;
	}

	is_mouse_down = false;
}
void Game::DrawBall()
{
	std::string img_src = ball->GetBallImage();
	int pos_x = ceil(ball->GetX());
	int pos_y = ceil(ball->GetY());
	int radius = ball->GetRadius();
	window->draw_img(img_src, Rectangle(pos_x,pos_y,radius*2,radius*2));
}

void Game::DrawPlayer(int team_id)
{
	Team* team;
	if(team_id == TEAM_A) team = teamA;
	if(team_id == TEAM_B) team = teamB;
	for(int i = 0;i < team->PlayerSize();i++)
	{
		std::string img_src = team->GetPlayer(i)->GetPlayerImage();
		int pos_x = ceil(team->GetPlayer(i)->GetX());
		int pos_y = ceil(team->GetPlayer(i)->GetY());
		int radius = team->GetPlayer(i)->GetRadius();
		window->draw_img(img_src, Rectangle(pos_x-radius,pos_y-radius,radius*2,radius*2));
	}
}

void Game::SelectPawn(int team_id,Point mouse_position)
{
	Team* team;
	if(team_id == TEAM_A) team = teamA;
	if(team_id == TEAM_B) team = teamB;

	for(int i = 0;i < team->PlayerSize();i++)
	{
		int radius = team->GetPlayer(i)->GetRadius();
		int pos_x = ceil(team->GetPlayer(i)->GetX());
		int pos_y = ceil(team->GetPlayer(i)->GetY());

		if(IsSelected(radius,pos_x,pos_y,mouse_position))
		{
			selected_player = team->GetPlayer(i);
		}
	}
}

bool Game::IsSelected(int radius,int pos_x,int pos_y,Point mouse_position)
{
	int mouse_x = mouse_position.x;
	int mouse_y = mouse_position.y;
	return pow(radius,2)-(pow(pos_x-mouse_x,2) + pow(pos_y-mouse_y,2)) >= 0;
}

void Game::StartMove(Event e,Player *player)
{
	Point mouse_position = e.get_mouse_position();
	Point pawn_position;
	pawn_position.x = player->GetX();
	pawn_position.y = player->GetY();
	Point d = SubtractVector(mouse_position,pawn_position);
	int d_size = VectorSize(d);
	Point velocity = CalculateVelocity(d,d_size);
	player->SetVx(velocity.x);
	player->SetVy(velocity.y);
}

void Game::MoveObjects()
{
	ball->Move(FRAME_RATE);
	goal_status = ball->CollideWithEdges(width,height);
	bool collided_players = false;
	for(int i = 0;i < PLAYERS;i++)
	{
		for(int j = 0;j < PLAYERS;j++)
		{
			collided_players = Collision(teamA->GetPlayer(i),teamA->GetPlayer(j))
			|| Collision(teamA->GetPlayer(i),teamB->GetPlayer(j))
			|| Collision(teamB->GetPlayer(i),teamB->GetPlayer(j));
		}
		Collision(teamA->GetPlayer(i),ball);
		Collision(teamB->GetPlayer(i),ball);
		teamA->GetPlayer(i)->CollideWithEdges(width,height);
		teamB->GetPlayer(i)->CollideWithEdges(width,height);

		if(!collided_players)
		{
			teamA->GetPlayer(i)->Move(FRAME_RATE);
			teamB->GetPlayer(i)->Move(FRAME_RATE);
		}
		collided_players = false;
	}

}
