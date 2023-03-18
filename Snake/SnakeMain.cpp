#include <iostream>
//#include <time.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

SDL_Window* win = NULL;
SDL_Renderer* ren = NULL;

int win_width = 1280;
int win_height = 720;

enum Direction
{
	DOWN, LEFT, RIGHT, UP, STOP
};

struct Snake
{
	SDL_Point body[500];

	int size = 1;

	int dir = STOP;
};

struct CheckGame
{
	bool checkMenu = true;
	bool checkOption = false;
	bool checkRecords = false;
	bool startGame = false;
	bool checkRefresh = false;
};

struct Counter
{
	int score = 0;
	int bonusCount = 0;
	int pts[5];
	char rec1[5];
	char rec2[5];
	char rec3[5];
	char rec4[5];
	char rec5[5];
};


void DeInit(int error)
{
	if (ren != NULL) SDL_DestroyRenderer(ren);
	if (win != NULL) SDL_DestroyWindow(win);
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
	exit(error);
}

void Init()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("SDL_INIT ERROR: %s\n", SDL_GetError());
		system("pause");
		DeInit(0);
	}

	int res;
	if ((res = IMG_Init(IMG_INIT_PNG || IMG_INIT_JPG)) == 0)
	{
		printf("IMG_INIT ERROR: %s\n", SDL_GetError());
		system("pause");
		DeInit(0);
	}

	if (!(res && IMG_INIT_PNG))
	{
		printf("IMG_INIT_PNG ERROR: %s\n", SDL_GetError());
		system("pause");
		DeInit(0);
	}
	if (!(res && IMG_INIT_JPG))
	{
		printf("IMG_INIT_JPG ERROR: %s\n", SDL_GetError());
		system("pause");
		DeInit(0);
	}

	if (TTF_Init())
	{
		printf("TTF_INIT ERROR: %s\n", SDL_GetError());
		system("pause");
		DeInit(0);
	}

	win = SDL_CreateWindow("SnakeGame", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_width, win_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE); // координаты левого угла окна, размерность окна

	if (win == NULL)
	{
		printf("WINDOW ERROR: %s\n", SDL_GetError());
		system("pause");
		DeInit(0);
	}

	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

	if (ren == NULL)
	{
		printf("RENDER ERROR: %s\n", SDL_GetError());
		system("pause");
		DeInit(0);
	}
}

void screenColor()
{
	SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
	SDL_RenderClear(ren);
}

void fileRecords(Counter& counter)
{
	FILE* f1;

	if (fopen_s(&f1, "Records.txt", "rt") != 0)
	{
		printf("Error work with file!\n");
		exit(1);
	}

	for (int i = 0; i < 5; i++)
	{
		fscanf_s(f1, "%i", &counter.pts[i]);
	}

	for (int i = 0; i < 5; i++)
	{
		if (counter.score > counter.pts[i])
		{
			counter.pts[i] = counter.score;
			break;
		}
	}

	fclose(f1);

	if (fopen_s(&f1, "Records.txt", "wt") != 0)
	{
		printf("Error work with file!\n");
		exit(1);
	}

	for (int i = 0; i < 5; i++)
	{
		fprintf_s(f1, "%i\n", counter.pts[i]);
	}

	fclose(f1);
}

void fileRead(Counter& counter)
{
	FILE* f;

	if (fopen_s(&f, "Records.txt", "rt") != 0)
	{
		printf("Error work with file!\n");
		exit(1);
	}

	fscanf_s(f, "%s", &counter.rec1, 5);
	fscanf_s(f, "%s", &counter.rec2, 5);
	fscanf_s(f, "%s", &counter.rec3, 5);
	fscanf_s(f, "%s", &counter.rec4, 5);
	fscanf_s(f, "%s", &counter.rec5, 5);

	fclose(f);
}

void drawBlock(SDL_Point block[4])
{
	SDL_Rect w;
	SDL_SetRenderDrawColor(ren, 255, 255, 0, 255);

	for (int i = 0; i < 4; i++)
	{
		w = { block[i].x, block[i].y, 20, 20 };
		SDL_RenderFillRect(ren, &w);
		SDL_RenderPresent(ren);
	}
}

void drawSnake(Snake snake)
{
	SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);

	SDL_Rect b;

	for (int i = 0; i < snake.size; i++)
	{
		b = { snake.body[i].x, snake.body[i].y, 20, 20 };
		SDL_RenderFillRect(ren, &b);
	}
}

void moveSnake(Snake& snake)
{
	for (int i = snake.size - 1; i > 0; i--)
	{
		snake.body[i] = snake.body[i - 1];
	}

	switch (snake.dir)
	{
	case UP:
		snake.body[0].y -= 20;
		break;
	case DOWN:
		snake.body[0].y += 20;
		break;
	case RIGHT:
		snake.body[0].x += 20;
		break;
	case LEFT:
		snake.body[0].x -= 20;
		break;
	}

	drawSnake(snake);
}

void drawFood(SDL_Point& food)
{
	SDL_Rect f = { food.x, food.y, 20, 20 };
	SDL_SetRenderDrawColor(ren, 0, 255, 127, 255);
	SDL_RenderFillRect(ren, &f);
}

void drawBonus(SDL_Point& bonus)
{
	SDL_Rect b = { bonus.x, bonus.y, 20, 20 };
	SDL_SetRenderDrawColor(ren, 255, 0, 255, 255);
	SDL_RenderFillRect(ren, &b);
}

void snakeTurn(Snake& snake)
{
	if (snake.body[snake.size - 1].x - snake.body[snake.size - 2].x > 0 && snake.body[snake.size - 1].y == snake.body[snake.size - 2].y)
	{
		snake.dir = RIGHT;
	}
	if (snake.body[snake.size - 1].x - snake.body[snake.size - 2].x < 0 && snake.body[snake.size - 1].y == snake.body[snake.size - 2].y)
	{
		snake.dir = LEFT;
	}
	if (snake.body[snake.size - 1].y - snake.body[snake.size - 2].y > 0 && snake.body[snake.size - 1].x == snake.body[snake.size - 2].x)
	{
		snake.dir = DOWN;
	}
	if (snake.body[snake.size - 1].y - snake.body[snake.size - 2].y < 0 && snake.body[snake.size - 1].x == snake.body[snake.size - 2].x)
	{
		snake.dir = UP;
	}

	SDL_Point c = snake.body[0];
	snake.body[0] = snake.body[snake.size - 1];
	snake.body[snake.size - 1] = c;

	for (int i = 1; i < snake.size / 2; i++)
	{
		c = snake.body[i];
		snake.body[i] = snake.body[snake.size - 1 - i];
		snake.body[snake.size - 1 - i] = c;
	}
}

void snakeCollision(Snake& snake, bool& running, CheckGame& check, Counter& counter)
{
	for (int i = 1; i < snake.size; i++)
	{
		if (snake.body[i].x == snake.body[0].x && snake.body[i].y == snake.body[0].y)
		{
			check.checkMenu = true;
			check.startGame = false;
			fileRecords(counter);
		}
	}
}

/*void wallCollision1(Snake& snake, SDL_Point block[4], bool& running, CheckGame& check, Counter& counter)
{
	if (snake.body[0].x > win_width) { snake.body[0].x = 0; }
	if (snake.body[0].x < 0) { snake.body[0].x = win_width - 1; }
	if (snake.body[0].y > win_height) { snake.body[0].y = 0; }
	if (snake.body[0].y < 0) { snake.body[0].y = win_height - 1; }

	int rCol = 20;
	int xCol, yCol;

	for (int i = 0; i < 4; i++)
	{
		xCol = snake.body[0].x - block[i].x;
		yCol = snake.body[0].y - block[i].y;

		if (xCol * xCol + yCol * yCol < rCol * rCol)
		{
			check.checkMenu = true;
			check.startGame = false;
			fileRecords(counter);
		}
	}
}*/

void wallCollision(Snake& snake, SDL_Point block[4], bool& running, CheckGame& check, Counter& counter)
{
	if (snake.body[0].x > win_width) { snake.body[0].x = 0; }
	if (snake.body[0].x < 0) { snake.body[0].x = win_width; }
	if (snake.body[0].y > win_height) { snake.body[0].y = 0; }
	if (snake.body[0].y < 0) { snake.body[0].y = win_height; }

	for (int i = 0; i < 4; i++)
	{
		if (snake.body[0].x == block[i].x && snake.body[0].y == block[i].y)
		{
			check.checkMenu = true;
			check.startGame = false;
			fileRecords(counter);
		}
	}
}

/*void foodCollision1(Snake& snake, SDL_Point& food, Counter& counter)
{
	int rCol = 20;
	int xCol, yCol;

	xCol = snake.body[0].x - food.x;
	yCol = snake.body[0].y - food.y;

	if (xCol* xCol + yCol * yCol < rCol * rCol)
	{
		snake.size++;
		counter.score++;
		counter.bonusCount++;

		for (int i = snake.size - 1; i > 0; i--)
		{
			snake.body[i] = snake.body[i - 1];
		}

		food = { rand() % (win_width - 40), rand() % (win_height - 40) };
		food.x -= food.x % 20;
		food.y -= food.y % 20;

		drawFood(food);
	}

	drawSnake(snake);
}*/

void foodCollision(Snake& snake, SDL_Point& food, Counter& counter)
{
	if (snake.body[0].x == food.x && snake.body[0].y == food.y)
	{
		snake.size++;
		counter.score++;
		counter.bonusCount++;

		for (int i = snake.size - 1; i > 0; i--)
		{
			snake.body[i] = snake.body[i - 1];
		}

		food = { rand() % (win_width - 40), rand() % (win_height - 40) };
		food.x -= food.x % 20;
		food.y -= food.y % 20;

		drawFood(food);
	}

	drawSnake(snake);
}

/*void bonusCollision1(Snake& snake, SDL_Point& bonus, Counter& counter)
{
	int rCol = 20;
	int xCol, yCol;

	xCol = snake.body[0].x - bonus.x;
	yCol = snake.body[0].y - bonus.y;

	if (xCol * xCol + yCol * yCol < rCol * rCol)
	{
		counter.score += 10;
		counter.bonusCount = 0;

		bonus = { rand() % (win_width - 40), rand() % (win_height - 40) };
		bonus.x -= bonus.x % 20;
		bonus.y -= bonus.y % 20;
	}
}*/

void bonusCollision(Snake& snake, SDL_Point& bonus, Counter& counter)
{
	int rCol = 20;
	int xCol, yCol;

	xCol = snake.body[0].x - bonus.x;
	yCol = snake.body[0].y - bonus.y;

	if (snake.body[0].x == bonus.x && snake.body[0].y == bonus.y)
	{
		counter.score += 10;
		counter.bonusCount = 0;

		bonus = { rand() % (win_width - 40), rand() % (win_height - 40) };
		bonus.x -= bonus.x % 20;
		bonus.y -= bonus.y % 20;
	}
}

/*void checkCollision1(SDL_Point& food, SDL_Point block[4], SDL_Point& bonus)
{
	int rCol = 20;
	int xCol, yCol;

	for (int i = 0; i < 4; i++)
	{
		xCol = block[i].x - food.x;
		yCol = block[i].y - food.y;

		if (xCol * xCol + yCol * yCol < rCol * rCol)
		{
			food = { rand() % win_width, rand() % win_height };
			food.x -= food.x % 20;
			food.y -= food.y % 20;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		xCol = block[i].x - bonus.x;
		yCol = block[i].y - bonus.y;

		if (xCol * xCol + yCol * yCol < rCol * rCol)
		{
			bonus = { rand() % win_width, rand() % win_height };
		}
	}
}*/

void checkCollision(SDL_Point& food, SDL_Point block[4], SDL_Point& bonus)
{
	for (int i = 0; i < 4; i++)
	{
		if (block[i].x == food.x && block[i].y == food.y)
		{
			food = { rand() % win_width, rand() % win_height };
			food.x -= food.x % 20;
			food.y -= food.y % 20;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		if (block[i].x == bonus.x && block[i].y == bonus.y)
		{
			bonus = { rand() % win_width, rand() % win_height };
			bonus.x -= bonus.x % 20;
			bonus.y -= bonus.y % 20;
		}
	}
}

void stateKey(Snake& snake, const Uint8*& state)
{
	if (state[SDL_SCANCODE_UP] && !state[SDL_SCANCODE_DOWN]) snake.dir = UP;
	if (!state[SDL_SCANCODE_UP] && state[SDL_SCANCODE_DOWN]) snake.dir = DOWN;
	if (state[SDL_SCANCODE_RIGHT] && !state[SDL_SCANCODE_LEFT]) snake.dir = RIGHT;
	if (!state[SDL_SCANCODE_RIGHT] && state[SDL_SCANCODE_LEFT]) snake.dir = LEFT;
}

void drawMenu()
{
	SDL_Rect src_rect, dst_rect;

	SDL_Surface* win_surface = IMG_Load("Back1.bmp");
	SDL_Texture* win_texture = SDL_CreateTextureFromSurface(ren, win_surface);
	SDL_FreeSurface(win_surface);
	SDL_RenderCopy(ren, win_texture, NULL, NULL);

	SDL_Surface* txt_surf = IMG_Load("Play.bmp");
	SDL_Texture* txt_text = SDL_CreateTextureFromSurface(ren, txt_surf);
	src_rect = { 0, 0, txt_surf->w, txt_surf->h };
	SDL_FreeSurface(txt_surf);
	dst_rect = { win_width / 2 - 48, win_height / 2 - 96, src_rect.w, src_rect.h };
	SDL_RenderCopy(ren, txt_text, &src_rect, &dst_rect);

	txt_surf = IMG_Load("Option.bmp");
	txt_text = SDL_CreateTextureFromSurface(ren, txt_surf);
	src_rect = { 0, 0, txt_surf->w, txt_surf->h };
	SDL_FreeSurface(txt_surf);
	dst_rect = { win_width / 2 - 48, win_height / 2 - 20, src_rect.w, src_rect.h };
	SDL_RenderCopy(ren, txt_text, &src_rect, &dst_rect);

	txt_surf = IMG_Load("Rec.bmp");
	txt_text = SDL_CreateTextureFromSurface(ren, txt_surf);
	src_rect = { 0, 0, txt_surf->w, txt_surf->h };
	SDL_FreeSurface(txt_surf);
	dst_rect = { win_width / 2 - 48, win_height / 2 + 28, src_rect.w, src_rect.h };
	SDL_RenderCopy(ren, txt_text, &src_rect, &dst_rect);

	txt_surf = IMG_Load("Exit.bmp");
	txt_text = SDL_CreateTextureFromSurface(ren, txt_surf);
	src_rect = { 0, 0, txt_surf->w, txt_surf->h };
	SDL_FreeSurface(txt_surf);
	dst_rect = { win_width / 2 - 48, win_height / 2 + 75, src_rect.w, src_rect.h };
	SDL_RenderCopy(ren, txt_text, &src_rect, &dst_rect);

	SDL_RenderPresent(ren);

	SDL_DestroyTexture(win_texture);
	SDL_DestroyTexture(txt_text);
}

void refresh(Snake& snake, Counter& counter, SDL_Point& food, SDL_Point block[4], SDL_Point& bonus)
{
	snake.size = 1;
	snake.dir = STOP;

	snake.body[0] = { win_width / 2, win_height / 2 };
	snake.body[0].x -= snake.body[0].x % 20;
	snake.body[0].y -= snake.body[0].y % 20;
	food = { rand() % win_width, rand() % win_height };
	food.x -= food.x % 20;
	food.y -= food.y % 20;
	bonus = { rand() % win_width, rand() % win_height };
	bonus.x -= bonus.x % 20;
	bonus.y -= bonus.y % 20;

	for (int i = 0; i < 4; i++)
	{
		block[i] = { rand() % win_width, rand() % win_height };
		block[i].x -= block[i].x % 20;
		block[i].y -= block[i].y % 20;
	}

	counter.bonusCount = 0;
	counter.score = 0;
}

void Game(bool& running, Snake& snake, SDL_Point& food, SDL_Point block[4], SDL_Point& bonus, Counter& counter, CheckGame& check)
{
	moveSnake(snake);

	snakeCollision(snake, running, check, counter);

	foodCollision(snake, food, counter);

	wallCollision(snake, block, running, check, counter);

	if (counter.bonusCount >= 5)
	{
		drawBonus(bonus);
		bonusCollision(snake, bonus, counter);
	}

	checkCollision(food, block, bonus);
}

bool Menu(bool& running, CheckGame& check, Snake& snake)
{
	drawMenu();

	SDL_Event ev;

	while (true)
	{
		while (SDL_PollEvent(&ev))
		{
			switch (ev.type)
			{
			case SDL_QUIT:
				running = false;
				return false;
				break;

			case SDL_KEYDOWN:
				if (ev.key.keysym.sym == SDLK_ESCAPE)
				{
					running = false;
					return false;
				}

				if (ev.key.keysym.sym == SDLK_1)
				{
					check.checkMenu = false;
					check.startGame = true;
					check.checkRefresh = true;
					running = true;
					return false;
				}

				if (ev.key.keysym.sym == SDLK_2)
				{
					check.checkMenu = false;
					check.checkOption = true;
					return false;
				}

				if (ev.key.keysym.sym == SDLK_3)
				{
					check.checkMenu = false;
					check.checkRecords = true;
					return false;
				}

				if (ev.key.keysym.sym == SDLK_4)
				{
					running = false;
					return false;
				}
			}
		}
	}
}

bool Option(bool& running, CheckGame& check, Counter& counter)
{
	SDL_Surface* win_surface = IMG_Load("Back1.bmp");
	SDL_Texture* win_texture = SDL_CreateTextureFromSurface(ren, win_surface);
	SDL_FreeSurface(win_surface);
	SDL_RenderCopy(ren, win_texture, NULL, NULL);
	SDL_RenderPresent(ren);

	SDL_Event ev;

	while (SDL_PollEvent(&ev))
	{
		switch (ev.type)
		{
		case SDL_QUIT:
			running = false;
			return false;
			break;

		case SDL_KEYDOWN:
			if (ev.key.keysym.sym == SDLK_ESCAPE)
			{
				check.checkMenu = true;
				check.checkOption = false;
				return false;
			}
		}
	}

	SDL_DestroyTexture(win_texture);
}

bool Records(bool& running, CheckGame& check, Counter& counter)
{
	SDL_Rect src_rect, dst_rect;

	SDL_Surface* win_surface = IMG_Load("Back1.bmp");
	SDL_Texture* win_texture = SDL_CreateTextureFromSurface(ren, win_surface);
	SDL_FreeSurface(win_surface);
	SDL_RenderCopy(ren, win_texture, NULL, NULL);
	SDL_RenderPresent(ren);

	SDL_Surface* txt_surf = IMG_Load("Score.bmp");
	SDL_Texture* txt_text = SDL_CreateTextureFromSurface(ren, txt_surf);
	src_rect = { 0, 0, txt_surf->w, txt_surf->h };
	SDL_FreeSurface(txt_surf);
	dst_rect = { win_width / 2 - 48, win_height / 2 - 144, src_rect.w, src_rect.h };
	SDL_RenderCopy(ren, txt_text, &src_rect, &dst_rect);

	txt_surf = IMG_Load("Score.bmp");
	txt_text = SDL_CreateTextureFromSurface(ren, txt_surf);
	src_rect = { 0, 0, txt_surf->w, txt_surf->h };
	SDL_FreeSurface(txt_surf);
	dst_rect = { win_width / 2 - 48, win_height / 2 - 86, src_rect.w, src_rect.h };
	SDL_RenderCopy(ren, txt_text, &src_rect, &dst_rect);

	txt_surf = IMG_Load("Score.bmp");
	txt_text = SDL_CreateTextureFromSurface(ren, txt_surf);
	src_rect = { 0, 0, txt_surf->w, txt_surf->h };
	SDL_FreeSurface(txt_surf);
	dst_rect = { win_width / 2 - 48, win_height / 2 - 28, src_rect.w, src_rect.h };
	SDL_RenderCopy(ren, txt_text, &src_rect, &dst_rect);

	txt_surf = IMG_Load("Score.bmp");
	txt_text = SDL_CreateTextureFromSurface(ren, txt_surf);
	src_rect = { 0, 0, txt_surf->w, txt_surf->h };
	SDL_FreeSurface(txt_surf);
	dst_rect = { win_width / 2 - 48, win_height / 2 + 30, src_rect.w, src_rect.h };
	SDL_RenderCopy(ren, txt_text, &src_rect, &dst_rect);

	txt_surf = IMG_Load("Score.bmp");
	txt_text = SDL_CreateTextureFromSurface(ren, txt_surf);
	src_rect = { 0, 0, txt_surf->w, txt_surf->h };
	SDL_FreeSurface(txt_surf);
	dst_rect = { win_width / 2 - 48, win_height / 2 + 88, src_rect.w, src_rect.h };
	SDL_RenderCopy(ren, txt_text, &src_rect, &dst_rect);

	TTF_Font* font = TTF_OpenFont("111.ttf", 29);
	SDL_Color font_color = { 0, 0, 0, 255 };

	fileRead(counter);

	SDL_Surface* str_surf;
	SDL_Texture* str_texture;

	str_surf = TTF_RenderText_Blended(font, counter.rec1, font_color);
	str_texture = SDL_CreateTextureFromSurface(ren, str_surf);
	src_rect = { 0, 0, str_surf->w, str_surf->h };
	SDL_FreeSurface(str_surf);
	dst_rect = { win_width / 2 - 48 + 130, win_height / 2 - 144, src_rect.w, src_rect.h };
	SDL_RenderCopy(ren, str_texture, &src_rect, &dst_rect);

	str_surf = TTF_RenderText_Blended(font, counter.rec2, font_color);
	str_texture = SDL_CreateTextureFromSurface(ren, str_surf);
	src_rect = { 0, 0, str_surf->w, str_surf->h };
	SDL_FreeSurface(str_surf);
	dst_rect = { win_width / 2 - 48 + 130, win_height / 2 - 86, src_rect.w, src_rect.h };
	SDL_RenderCopy(ren, str_texture, &src_rect, &dst_rect);

	str_surf = TTF_RenderText_Blended(font, counter.rec3, font_color);
	str_texture = SDL_CreateTextureFromSurface(ren, str_surf);
	src_rect = { 0, 0, str_surf->w, str_surf->h };
	SDL_FreeSurface(str_surf);
	dst_rect = { win_width / 2 - 48 + 130, win_height / 2 - 28, src_rect.w, src_rect.h };
	SDL_RenderCopy(ren, str_texture, &src_rect, &dst_rect);

	str_surf = TTF_RenderText_Blended(font, counter.rec4, font_color);
	str_texture = SDL_CreateTextureFromSurface(ren, str_surf);
	src_rect = { 0, 0, str_surf->w, str_surf->h };
	SDL_FreeSurface(str_surf);
	dst_rect = { win_width / 2 - 48 + 130, win_height / 2 + 30, src_rect.w, src_rect.h };
	SDL_RenderCopy(ren, str_texture, &src_rect, &dst_rect);

	str_surf = TTF_RenderText_Blended(font, counter.rec5, font_color);
	str_texture = SDL_CreateTextureFromSurface(ren, str_surf);
	src_rect = { 0, 0, str_surf->w, str_surf->h };
	SDL_FreeSurface(str_surf);
	dst_rect = { win_width / 2 - 48 + 130, win_height / 2 + 88, src_rect.w, src_rect.h };
	SDL_RenderCopy(ren, str_texture, &src_rect, &dst_rect);

	SDL_RenderPresent(ren);

	SDL_Event ev;

	while (true)
	{
		while (SDL_PollEvent(&ev))
		{
			switch (ev.type)
			{
			case SDL_QUIT:
				running = false;
				return false;
				break;

			case SDL_KEYDOWN:
				if (ev.key.keysym.sym == SDLK_ESCAPE)
				{
					check.checkMenu = true;
					check.checkRecords = false;
					return false;
				}
				break;
			}
		}
	}

	TTF_CloseFont(font);
	SDL_DestroyTexture(win_texture);
	SDL_DestroyTexture(txt_text);
}


int main(int argc, char* argv[])
{

#pragma region INITIALISATION

	srand(time(0));

	Init();

	Counter counter;
	CheckGame check;
	Snake snake;

	snake.body[0] = { win_width / 2, win_height / 2 };
	snake.body[0].x -= snake.body[0].x % 20;
	snake.body[0].y -= snake.body[0].y % 20;

	SDL_Point food = { rand() % (win_width - 40), rand() % (win_height - 40) };
	food.x -= food.x % 20;
	food.y -= food.y % 20;
	SDL_Point bonus = { rand() % (win_width - 40), rand() % (win_height - 40) };
	bonus.x -= bonus.x % 20;
	bonus.y -= bonus.y % 20;

	SDL_Point block[4];

	for (int i = 0; i < 4; i++)
	{
		block[i] = { rand() % win_width, rand() % win_height };
		block[i].x -= block[i].x % 20;
		block[i].y -= block[i].y % 20;
	}

	SDL_Event e;

	const Uint8* state = SDL_GetKeyboardState(NULL);

	TTF_Font* font = TTF_OpenFont("111.ttf", 50);
	SDL_Color font_color = { 255, 255, 255, 255 };

	SDL_Rect src_rect, dst_rect;
	SDL_Surface* str_surf;
	SDL_Texture* str_texture = nullptr;
	char str[15] = "Score 0";

#pragma endregion

	// RUNNING
	bool running = true;
	while (running)
	{

		screenColor();

#pragma region EVENTS

		// EVENTS
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				running = false;
				break;

			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_SPACE)
				{
					snakeTurn(snake);

					drawSnake(snake);
					SDL_RenderPresent(ren);
				}

				if (e.key.keysym.sym == SDLK_ESCAPE)
				{
					running = false;
					return false;
					fileRecords(counter);
				}

			case SDL_WINDOWEVENT_RESIZED:
				if (e.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					win_width = e.window.data1;
					win_height = e.window.data2;
				}
				break;

				if (e.key.keysym.sym == SDLK_ESCAPE)
				{
					running = false;
					return false;
				}

				//if (e.key.keysym.sym == SDLK_DOWN) { snake.dir = DOWN; }
				//if (e.key.keysym.sym == SDLK_RIGHT) { snake.dir = RIGHT; }
				//if (e.key.keysym.sym == SDLK_LEFT) { snake.dir = LEFT; }
				break;
			}
		}

		// MOVE_EVENTS
		stateKey(snake, state);

#pragma endregion

#pragma region LOGIC

		if (check.startGame)
		{

			Game(running, snake, food, block, bonus, counter, check);

			str_surf = TTF_RenderText_Blended(font, str, font_color);
			str_texture = SDL_CreateTextureFromSurface(ren, str_surf);
			src_rect = { 0, 0, str_surf->w, str_surf->h };
			SDL_FreeSurface(str_surf);
			dst_rect = { 10, win_height - 80, src_rect.w, src_rect.h };
			SDL_RenderCopy(ren, str_texture, &src_rect, &dst_rect);
			sprintf_s(str, "Score %i", counter.score);

#pragma endregion

#pragma region DRAWING

			drawSnake(snake);
			drawFood(food);
			drawBlock(block);

			SDL_Delay(75);
			SDL_RenderPresent(ren);

		}

		if (check.checkMenu)
		{
			Menu(running, check, snake);
		}
		if (check.checkOption)
		{
			Option(running, check, counter);
		}
		if (check.checkRecords)
		{
			Records(running, check, counter);
		}
		if (check.checkRefresh)
		{
			refresh(snake, counter, food, block, bonus);
			check.checkRefresh = false;
		}

#pragma endregion

	}

	printf("Your score = %i\n", counter.score);
	SDL_DestroyTexture(str_texture);

	DeInit(0);
	return 0;
}