/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

For a C++ project simply rename the file to .cpp and run premake 

*/

// BomberSUS.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <raylib.h>
#include <vector>

using namespace std;

float version = 0.0;
int num_texturas = 0;

map<string, string> texturas;
map<string, Texture2D> level_texturas;

int map_h;
int map_w;

string** background;
int background_w;
int background_h;

string** foreground;
int foreground_w;
int foreground_h;

string** objects;
int objects_w;
int objects_h;

int framesCounter = 0;
bool GameOver = false;

//int movement_limit = 1;

bool audio_version;

int bombsOnField;
int bomb_limit = 1;
bool can_bomb = true;
int powerUp_count = 2;


struct Bombs
{
	int b_f_pos_i;
	int b_f_pos_j;
	int b_framesCounter;
	
	Vector3 b_position;

	Color b_color;
	
	string b_foreground_pos;
	string b_objects_pos;
	string b_up_foreground_pos;
	string b_down_foreground_pos;
	string b_right_foreground_pos;
	string b_left_foreground_pos;
	string b_up_objects_pos;
	string b_down_objects_pos;
	string b_right_objects_pos;
	string b_left_objects_pos;

	float b_ratius;
	float b_time;
	
	bool bombOnField;
	bool bomb_exploted;
};

vector<Bombs*>bomb;

bool is_passable(int x, int z)
{
	return objects[x][z] == "0" || foreground[x][z] == "B";
}

void print_fg(){
	for (int i = 0; i < foreground_h; i++)
	{
		for (int j = 0; j < foreground_w; j++)
		{
			cout << foreground[i][j];
		}
		cout << endl;
	}
}

void draw_bomb(int i, int j) {
	if (foreground[i][j] == "B")
	{
		for (int i = 0; i < bomb.size(); i++)
		{
			DrawSphere(bomb[i]->b_position, bomb[i]->b_ratius, bomb[i]->b_color);
		}
	}
}

Vector3 explosion_pos(Vector3 playerPosition, int i, int j, int b_x_inc, int b_z_inc) {
	Vector3 b_pos = {
		playerPosition.x + float(j) + b_x_inc,
		playerPosition.y + 1,
		playerPosition.z + float(i) + b_z_inc
	};
	return(b_pos);
}




int draw(void) {

	//Codigo raylib

	const int screenWidth = 1600;
	const int screenHeight = 900;

	InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera mode");

	if (audio_version)
	{
		InitAudioDevice();
	}

	Music music = LoadMusicStream("AmongUs_Remix.mp3");

	PlayMusicStream(music);

	map<string, string>::iterator it;

	for (it = texturas.begin(); it != texturas.end(); it++)
	{
		Image image = LoadImage(it->second.c_str());
		level_texturas[it->first] = LoadTextureFromImage(image);


		cout << it->first << " " << it->second << endl;
	}

	Camera3D camera = { 0 };
	camera.position = { 0.0f, 20.0f, 2.0f };
	camera.target = { 0.0f, 0.0f, 0.0f };
	camera.up = { 0.0f, 1.0f, 0.0f };
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	float cubePx = 0.0f;
	float cubePy = 0.0f;
	float cubePz = 0.0f;

	float cube_size = 1.0f;

	SetTargetFPS(60);
	while (!WindowShouldClose())
	{

		UpdateMusicStream(music);

		if (framesCounter == 3000)
		{
			framesCounter = 0;
		}
		else {
			framesCounter++;
		}


		BeginDrawing();

		ClearBackground(RAYWHITE);

		BeginMode3D(camera);

		float x_margin = background_w % 2 == 0 ? .5 : 0;
		float z_margin = background_h % 2 == 0 ? .5 : 0;

		float x_offset = background_w / 2;
		float z_offset = background_h / 2;

		Vector3 cubePosition = {
			-x_offset + x_margin,
			0.0f,
			-z_offset + z_margin
		};
		Vector3 playerPosition = {
			-x_offset + x_margin,
			0.0f,
			-z_offset + z_margin
		};
		Vector3 Bombposition = {
			-x_offset + x_margin,
			0.0f,
			-z_offset + z_margin
		};

		for (int i = 0; i < background_h; i++) {
			for (int j = 0; j < background_w; j++) {
				Vector3 pos = {
					cubePosition.x + float(j),
					cubePosition.y,
					cubePosition.z + float(i)
				};
				string b = background[i][j];
				DrawCubeTexture(level_texturas[b], pos, 1.0f, 1.0f, 1.0f, WHITE);
				DrawCubeWires(cubePosition, 1.0f, 1.0f, 1.0f, WHITE);
				//cout << cubePosition.x << endl;
			}
		}

		for (int i = 0; i < foreground_h; i++) {
			for (int j = 0; j < foreground_w; j++) {
				Vector3 pos = {
					cubePosition.x + float(j),
					cubePosition.y + 1,
					cubePosition.z + float(i)
				};
				string f = foreground[i][j];
				if (f != "0" && f != "1" && f != "2" && f != "3" && f != "4" && f != "B") {
					DrawCubeTexture(level_texturas[f], pos, 1.0f, 1.0f, 1.0f, WHITE);
					DrawCubeWires(cubePosition, 1.0f, 1.0f, 1.0f, WHITE);
					//cout << cubePosition.x << endl;
				}
			}
		}


		for (int i = 0; i < objects_h; i++) {
			for (int j = 0; j < objects_w; j++) {
				if (objects[i][j] == "1" || objects[i][j] == "2" || objects[i][j] == "3" || objects[i][j] == "4") {
					int x_inc = 0;
					int z_inc = 0;
					if (objects[i][j] == "1") {
						//derecha x+1
						if (IsKeyPressed(KEY_RIGHT)) {
							print_fg();
							if (j + 1 < objects_w)
								if (is_passable(i, j + 1)) {
									x_inc = 1;
								}
						}
						//izquierda x-1
						else if (IsKeyPressed(KEY_LEFT)) {
							print_fg();
							if (j - 1 >= 0)
								if (is_passable(i, j - 1)) {
									x_inc = -1;
								}
						}
						//arriba z+1
						else if (IsKeyPressed(KEY_UP)) {
							print_fg();
							if (i - 1 >= 0)
								if (is_passable(i - 1, j)) {
									z_inc = -1;
								}
						}
						//abajo z-1
						else if (IsKeyPressed(KEY_DOWN)) {
							print_fg();
							cout << i + 1;
							if (i + 1 < objects_h)
								if (is_passable(i + 1, j)) {
									z_inc = 1;
								}
						}
						/*

						if (x_inc != 0) {
							players[i].pos_x += x_inc;
						}

						else if (z_inc != 0) {
							players[i].pos_z += z_inc;
						}*/
					}
					Vector3 pos = {
						playerPosition.x + float(j) + x_inc,
						playerPosition.y + 1,
						playerPosition.z + float(i) + z_inc
					};

					if (bombsOnField == bomb_limit)
					{

					}

					//cambiar la casilla a casilla bomba y crear instancia de bomba
					if (objects[i][j] == "1") {
						if (IsKeyPressed(KEY_SPACE) && can_bomb) {
							foreground[i][j] = "B";
							Bombs* newBomb = new Bombs;
							newBomb->b_foreground_pos = foreground[i][j];
							newBomb->b_objects_pos = objects[i][j];
							newBomb->b_up_foreground_pos = foreground[i + 1][j];
							newBomb->b_down_foreground_pos = foreground[i - 1][j];
							newBomb->b_right_foreground_pos = foreground[i][j + 1];
							newBomb->b_left_foreground_pos = foreground[i][j - 1];
							newBomb->b_f_pos_i = i;
							newBomb->b_f_pos_j = j;
							newBomb->b_position = pos;
							newBomb->b_color = BLACK;
							newBomb->b_ratius = 0.2;
							newBomb->b_time = 3;
							newBomb->bombOnField = true;
							newBomb->b_framesCounter = 0;
							newBomb->bomb_exploted = false;
							bomb.push_back(newBomb);
							cout << "bomba" << endl;
							print_fg();
							bombsOnField++;
							can_bomb = false;
						}
					}

					//cambiar la posición del player
					if (x_inc != 0) {
						objects[i][j] = "0";
						objects[i][j + x_inc] = "1";
					}
					if (z_inc != 0) {
						objects[i][j] = "0";
						objects[i + z_inc][j] = "1";
					}

					//designar el color de los players
					Color c = WHITE;
					if (objects[i][j] == "2")
						c = BLUE;
					else if (objects[i][j] == "3")
						c = RED;
					else if (objects[i][j] == "4")
						c = YELLOW;
					DrawSphere(pos, 0.5, c);

				}

				//DrawSphere(pos, bomb[k]->b_ratius, RED);

				//dibujar bomba
				draw_bomb(i, j);

				//gestionar las bombas
				for (int k = 0; k < bomb.size(); k++)
				{
					//mira si la bomba esta en el mapa
					if (bomb[k]->bombOnField)
					{
						//aumentar los frames que la bomba esta en el mapa
						if (bomb[k]->b_framesCounter != 1500 && bomb[k]->b_framesCounter <= 1499)
						{
							bomb[k]->b_framesCounter++;
						}
						//reducir el tiempo restante para que la bomba explote
						if (bomb[k]->b_framesCounter == 1500 && bomb[k]->b_time > 0)
						{
							bomb[k]->b_time -= 1;
							bomb[k]->b_framesCounter = 0;
						}
						//gestionar la explosion de la bomba
						else if (bomb[k]->b_time == 0)
						{
							foreground[bomb[k]->b_f_pos_i][bomb[k]->b_f_pos_j] = "0";
							bomb[k]->bomb_exploted = true;
							bomb[k]->b_foreground_pos = "0";
							bomb[k]->bombOnField = false;

							//destruir los bloques alrededor de la bomba
							if (bomb[k]->bomb_exploted)
							{
								if (bomb[k]->b_up_foreground_pos == "T")
								{
									foreground[bomb[k]->b_f_pos_i + 1][bomb[k]->b_f_pos_j] = "0";
									objects[bomb[k]->b_f_pos_i + 1][bomb[k]->b_f_pos_j] = "0";
								}
								if (bomb[k]->b_down_foreground_pos == "T")
								{
									foreground[bomb[k]->b_f_pos_i - 1][bomb[k]->b_f_pos_j] = "0";
									objects[bomb[k]->b_f_pos_i - 1][bomb[k]->b_f_pos_j] = "0";
								}
								if (bomb[k]->b_right_foreground_pos == "T")
								{
									foreground[bomb[k]->b_f_pos_i][bomb[k]->b_f_pos_j + 1] = "0";
									objects[bomb[k]->b_f_pos_i][bomb[k]->b_f_pos_j + 1] = "0";
								}
								if (bomb[k]->b_left_foreground_pos == "T")
								{
									foreground[bomb[k]->b_f_pos_i][bomb[k]->b_f_pos_j - 1] = "0";
									objects[bomb[k]->b_f_pos_i][bomb[k]->b_f_pos_j - 1] = "0";
								}
							}
							delete bomb[k];
							bomb.erase(bomb.begin());
							can_bomb = true;
							bombsOnField -= 1;
						}
						/*b_framesCounter_limit = 3000;*/
					}
				}
			}
		}



		//DrawGrid(10, 1.0f);
		EndMode3D();
		DrawText("Welcome to bomberSUS!", 10, 40, 20, DARKGRAY);
		DrawFPS(10, 10);
		EndDrawing();

	}
	CloseAudioDevice();
	CloseWindow();

	return 0;


}


int main()
{
	string temp;
	ifstream level("level03.sus");
	if (!level.is_open()) {
		cout << "ERROR: Archibo mal avierto\n";
		return 1;
	}
	//revisar la cabecera
	getline(level, temp, ';');
	if (temp != "SUS") {
		cout << "ERROR: cabecera erronea" << endl;
		return 2;
	}
	cout << temp << endl;
	getline(level, temp, ';');
	cout << temp << endl;
	version = stof(temp);
	//saltar de linea
	getline(level, temp);
	//revisar el titulo
	getline(level, temp, ';');
	if (temp != "TITLE") {
		cout << "ERROR: Titulo erroneo" << endl;
		return 3;
	}
	cout << temp << endl;
	//saltar de linea
	getline(level, temp);
	if (version == 0.5 || version == 1.0)
	{
		audio_version = true;
		//revisar audio
		getline(level, temp, ';');
		if (temp != "MUSIC") {
			cout << "ERROR: error de audio" << endl;
			return 4;
		}
		cout << temp << ":";
		getline(level, temp, ';');
		cout << temp << endl;
		getline(level, temp);
	}
	else
	{
		audio_version = false;
		getline(level, temp);
	}
	getline(level, temp, ';');
	//revisar texturas
	if (temp != "TEXTURES") {
		cout << "ERROR: error en el indice de texturas" << endl;
		return 5;
	}
	cout << temp << ":";
	getline(level, temp, ';');
	cout << temp << endl;
	num_texturas = stoi(temp);
	getline(level, temp);


	for (int i = 0; i < num_texturas; i++) {
		string key;
		getline(level, key, ';');
		string value;
		getline(level, value, ';');
		texturas[key] = value;
		cout << key << ":" << value << endl;
		getline(level, temp);
	}

	getline(level, temp, ';');

	if (temp != "BACKGROUND") {
		cout << "ERROR: mapa mal formado" << endl;
		return 6;
	}
	cout << temp << " ";
	getline(level, temp, ';');
	cout << "[" << temp << "]" << ",";
	background_w = stoi(temp);
	getline(level, temp, ';');
	cout << "[" << temp << "]" << endl;
	background_h = stoi(temp);
	getline(level, temp);


	background = new string * [background_h];
	for (int i = 0; i < background_h; i++) { background[i] = new string[background_w]; }

	for (int i = 0; i < background_h; i++)
	{
		for (int j = 0; j < background_w; j++)
		{
			getline(level, temp, ';');
			cout << temp;
			background[i][j] = temp;
		}
		cout << endl;
		getline(level, temp);
	}

	getline(level, temp, ';');

	if (temp != "FOREGROUND") {
		cout << "ERROR: foreground mal formado" << endl;
		return 7;
	}
	cout << temp << " ";
	getline(level, temp, ';');
	cout << "[" << temp << "]" << ",";
	foreground_w = stoi(temp);
	getline(level, temp, ';');
	cout << "[" << temp << "]" << endl;
	foreground_h = stoi(temp);
	getline(level, temp);


	foreground = new string * [foreground_h];
	for (int i = 0; i < foreground_h; i++) { foreground[i] = new string[foreground_w]; }

	for (int i = 0; i < foreground_h; i++) {
		for (int j = 0; j < foreground_w; j++) {
			getline(level, temp, ';');
			cout << temp;
			foreground[i][j] = temp;
		}
		cout << endl;
		getline(level, temp);
	}
	getline(level, temp, ';');
	if (temp != "OBJECTS") {
		cout << "ERROR: objetos mal formado" << endl;
		return 8;
	}
	cout << temp << " ";
	getline(level, temp, ';');
	cout << "[" << temp << "]" << ",";
	objects_w = stoi(temp);
	getline(level, temp, ';');
	cout << "[" << temp << "]" << endl;
	objects_h = stoi(temp);
	getline(level, temp);

	objects = new string * [objects_h];
	for (int i = 0; i < objects_h; i++) { objects[i] = new string[objects_w]; }

	for (int i = 0; i < objects_h; i++)
	{
		for (int j = 0; j < objects_w; j++)
		{
			getline(level, temp, ';');
			cout << temp;

			if (true)
			{

			}
			objects[i][j] = temp;
		}
		cout << endl;
		getline(level, temp);
	}
	draw();
}