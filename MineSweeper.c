#define _CRT_SECURE_NO_WARNINGS
#define EMPTY_BOARD 0
#define EMPTY_MASK ' '
#define MINE 9
#define COVERED '#'
#define FLAG 'F'
#define NUM_TO_ASCII 48

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>


int** make_board(int horizontal, int vertical);
char** make_mask(int horizontal, int vertical);
int** free_board(int** board, int horizontal, int vertical);
char** free_mask(char** mask, int horizontal, int vertical);
void initialize(int** board, char** mask, int horizontal, int vertical, int number_of_mine);
void place_mine(int** board, int horizontal, int vertical, int number_of_mine);
void generate_num(int** board, int horizontal, int vertical);
void print_mask(char** mask, int horizontal, int vertical);
void run(int** board, char** mask, int horizontal, int vertical, int number_of_mine);
void gotoxy(int x, int y);
void open_square(int** board, char** mask, int x, int y, int horizontal, int vertical);
void open(int x, int y, int** board, char** mask, int horizontal, int vertical);
int open_wave(int** board, char** mask, int x, int y, int horizontal, int vertical);
void game_over(int** board, char** mask, int horizontal, int vertical);
void game_win_check(int** board, char** mask, int horizontal, int vertical, int number_of_mine);

int flag_left_cnt;
int unopened_cnt;
int rank_easy = 999, rank_medium = 999, rank_hard = 999;
time_t start, end;
FILE* fp;


int main()
{
	fp = fopen("savefile.txt", "r");
	if (fp == NULL)
	{
		fp = fopen("savefile.txt", "w");
		fprintf(fp, "999 999 999");
		fclose(fp);
	}
	fscanf(fp, "%d %d %d", &rank_easy, &rank_medium, &rank_hard);
	fclose(fp);
	system("cls");
	system("title 지뢰찾기");
	system("color 70");
	int horizontal, vertical, number_of_mine;
	int user_horizontal, user_vertical, user_number_of_mine;
	int** board;
	char** mask;
	int user_select;

select:
	system("cls");
	printf("\n\t※ 지 뢰 찾 기 ※\n");
	printf("\tMade by kraftenty\n\n");
	printf("\t메뉴를 선택하세요\n\n");
	printf("\t① 초급 난이도\n\n\t② 중급 난이도\n\n\t③ 고급 난이도\n\n\t④ 사용자 설정\n\n\t⑤ 랭킹\n\n\t⑥ 종료\n\t>>_\b");
	scanf(" %d", &user_select);
	rewind(stdin);
	switch (user_select)
	{
	case 1:
		//초급 : 9x9 , 지뢰 10개
		board = make_board(11, 11);
		mask = make_mask(11, 11);
		horizontal = 11;
		vertical = 11;
		number_of_mine = 10;
		break;
	case 2:
		//중급 : 16x16 , 지뢰 40개
		board = make_board(18, 18);
		mask = make_mask(18, 18);
		horizontal = 18;
		vertical = 18;
		number_of_mine = 40;
		break;
	case 3:
		//고급 : 30x16 , 지뢰 99개
		board = make_board(32, 18);
		mask = make_mask(32, 18);
		horizontal = 32;
		vertical = 18;
		number_of_mine = 99;
		break;
	case 4:
		//사설모드
		system("cls");
		printf("\n\t 사용자 설정 모드");
		printf("\n\n 높이 >> ");
		scanf("%d", &user_vertical);
		printf("\n 너비 >> ");
		scanf("%d", &user_horizontal);
		printf("\n 지뢰의 개수 >> ");
		scanf("%d", &user_number_of_mine);
		if ((user_horizontal * user_vertical) <= user_number_of_mine)
		{
			printf("\n 지뢰가 너무 많습니다.");
			Sleep(1000);
			system("cls");
			goto select;
		}
		board = make_board(user_horizontal + 2, user_vertical + 2);
		mask = make_mask(user_horizontal + 2, user_vertical + 2);
		horizontal = user_horizontal + 2;
		vertical = user_vertical + 2;
		number_of_mine = user_number_of_mine;
		break;
	case 5:
		system("cls");
		printf("\n - 랭 킹 -\n\n");
		printf(" 초급 : %d초\n\n", rank_easy);
		printf(" 중급 : %d초\n\n", rank_medium);
		printf(" 고급 : %d초\n", rank_hard);
		printf("\n\n 아무 키를 눌러 메인으로 돌아가기");
		_getch();
		system("cls");
		goto select;
	case 6:
		exit(0);
	default:
		printf("\n\n\t잘못 입력하셨습니다. 다시 선택하세요!");
		Sleep(1000);
		system("cls");
		goto select;
	}
	initialize(board, mask, horizontal, vertical, number_of_mine);
	run(board, mask, horizontal, vertical, number_of_mine);
	return 0;
}
int** make_board(int horizontal, int vertical)
{
	int** board;
	board = (int**)malloc(sizeof(int*) * vertical);
	for (int i = 0; i < vertical; i++)
		board[i] = (int*)malloc(sizeof(int) * horizontal);
	for (int i = 0; i < vertical; i++)
		for (int j = 0; j < horizontal; j++)
			board[i][j] = EMPTY_BOARD;
	return board;
}
char** make_mask(int horizontal, int vertical)
{
	char** mask;
	mask = (char**)malloc(sizeof(char*) * vertical);
	for (int i = 0; i < vertical; i++)
		mask[i] = (char*)malloc(sizeof(char) * horizontal);
	for (int i = 0; i < vertical; i++)
		for (int j = 0; j < horizontal; j++)
			mask[i][j] = COVERED;
	return mask;
}
int** free_board(int** board, int horizontal, int vertical)
{
	for (int i = 0; i < vertical; i++)
		free(board[i]);
	free(board);
	return board;
}

char** free_mask(char** mask, int horizontal, int vertical)
{
	for (int i = 0; i < vertical; i++)
		free(mask[i]);
	free(mask);
	return mask;
}

void run(int** board, char** mask, int horizontal, int vertical, int number_of_mine)
{
	char livekey = 0;
	int x = 0, y = 0;
	int initial_time_cnt = 0;
	flag_left_cnt = number_of_mine;
	system("cls");
	puts("아무 키를");
	puts("눌러서");
	puts("게임 시작");
	while (1)
	{
		game_win_check(board, mask, horizontal, vertical, number_of_mine);
		gotoxy(x, y);
		if (_kbhit())
		{
			livekey = _getch();
			gotoxy(0, 0);
			print_mask(mask, horizontal, vertical);
			//revised : 좌표값 보정
			int revised_x = (x + 2) / 2;
			int revised_y = y + 1;
			switch (livekey)
			{
			case 'w':
			case 'W':
				if (y > 0)
					y--;
				break;
			case 'a':
			case 'A':
				if (x > 0)
					x -= 2;
				break;
			case 's':
			case 'S':
				if (y < vertical - 3)
					y++;
				break;
			case 'd':
			case 'D':
				if (x < horizontal * 2 - 6)
					x += 2;
				break;
			case 'f':
			case 'F':
				if (mask[revised_y][revised_x] == COVERED)
				{
					mask[revised_y][revised_x] = FLAG;
					flag_left_cnt--;
				}
				else if (mask[revised_y][revised_x] == FLAG)
				{
					mask[revised_y][revised_x] = COVERED;
					flag_left_cnt++;
				}
				else
					break;
				system("cls");
				print_mask(mask, horizontal, vertical);
				break;
			case 'o':
			case 'O':
				if (mask[revised_y][revised_x] != FLAG)
				{
					if (initial_time_cnt == 0)
					{
						initial_time_cnt++;
						start = time(NULL);
					}
					Beep(290, 300);
					open(revised_x, revised_y, board, mask, horizontal, vertical);
					system("cls");
					print_mask(mask, horizontal, vertical);
				}
				break;
			case 'p':
			case 'P':
				if ((mask[revised_y][revised_x] != FLAG && mask[revised_y][revised_x] != COVERED) && (board[revised_y][revised_x] > 0 && board[revised_y][revised_x] < 9))
				{
					Beep(290, 300);
					open_square(board, mask, revised_x, revised_y, horizontal, vertical);
					system("cls");
					print_mask(mask, horizontal, vertical);
				}
				break;
			}
		}
		Sleep(50);
	}
}

void print_mask(char** mask, int horizontal, int vertical)
{
	int i, j, cnt = 0;
	for (i = 1; i < (vertical - 1); i++)
	{
		for (j = 1; j < (horizontal - 1); j++)
		{
			printf("%c ", mask[i][j]);
			if (mask[i][j] == FLAG || mask[i][j] == COVERED)
				cnt++;
		}
		printf("\n");
	}
	unopened_cnt = cnt;
	printf("┌─────────────────────────────────┐");
	printf("\n│ WASD 커서이동 F 플래그          │");
	printf("\n│ O 오픈        P 주위오픈        │ ");
	printf("\n│                                 │ ");
	printf("\n│ 남은 플래그 : %2d                │ ", flag_left_cnt);
	printf("\n└─────────────────────────────────┘");
}
void initialize(int** board, char** mask, int horizontal, int vertical, int number_of_mine)
{
	place_mine(board, horizontal, vertical, number_of_mine);
	generate_num(board, horizontal, vertical);
}

void place_mine(int** board, int horizontal, int vertical, int number_of_mine)
{
	srand((unsigned)time(NULL));
	int cnt = 0, vertical_temp, horizonal_temp;
	do
	{
		vertical_temp = rand() % (vertical - 2) + 1;
		horizonal_temp = rand() % (horizontal - 2) + 1;
		if (board[vertical_temp][horizonal_temp] != MINE)
		{
			board[vertical_temp][horizonal_temp] = MINE;
			++cnt;
		}
	} while (cnt != number_of_mine);
}

void generate_num(int** board, int horizontal, int vertical)
{
	int i, j, k;
	int di[8] = { -1,-1,-1,0,0,1,1,1 };
	int dj[8] = { -1,0,1,-1,1,-1,0,1 };
	for (i = 0; i < vertical; i++)
	{
		for (j = 0; j < horizontal; j++)
		{
			if (board[i][j] == MINE)
			{
				for (k = 0; k < 8; k++)
				{
					if (board[i + di[k]][j + dj[k]] != MINE)
						board[i + di[k]][j + dj[k]]++;
				}
			}
		}
	}
	for (i = 0; i < vertical; i++)
	{
		board[i][0] = -1;
		board[i][horizontal - 1] = -1;
	}
	for (j = 0; j < horizontal; j++)
	{
		board[0][j] = -1;
		board[vertical - 1][j] = -1;
	}
}
void gotoxy(int x, int y)
{
	COORD pos = { x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}
void open_square(int** board, char** mask, int x, int y, int horizontal, int vertical)
{
	int k, flag_count = 0, mine_count = 0;
	int dx[8] = { -1,-1,-1,0,0,1,1,1 };
	int dy[8] = { -1,0,1,-1,1,-1,0,1 };
	for (k = 0; k < 8; k++)
	{
		if (mask[y + dy[k]][x + dx[k]] == FLAG)
			++flag_count;
	}
	if (flag_count == board[y][x])
	{
		for (k = 0; k < 8; k++)
		{
			if ((x + dx[k] >= 0) && (x + dx[k] < horizontal))
			{
				if (board[y + dy[k]][x + dx[k]] == MINE && mask[y + dy[k]][x + dx[k]] != FLAG)
					game_over(board, mask, horizontal, vertical);
				if (board[y + dy[k]][x + dx[k]] == EMPTY_BOARD)
				{
					mask[y + dy[k]][x + dx[k]] = EMPTY_MASK;
					open_wave(board, mask, x, y, horizontal, vertical);
				}
				else if (board[y + dy[k]][x + dx[k]] != MINE)
				{
					mask[y + dy[k]][x + dx[k]] = board[y + dy[k]][x + dx[k]] + NUM_TO_ASCII;
				}
				else
					continue;
			}
		}
	}
}

void open(int x, int y, int** board, char** mask, int horizontal, int vertical)
{
	if (board[y][x] == MINE)
		game_over(board, mask, horizontal, vertical);
	if (mask[y][x] == COVERED && board[y][x] > EMPTY_BOARD)
	{
		mask[y][x] = board[y][x] + NUM_TO_ASCII;
	}
	if (board[y][x] == EMPTY_BOARD)
	{
		mask[y][x] = EMPTY_MASK;
		open_wave(board, mask, x, y, horizontal, vertical);
	}
}

int open_wave(int** board, char** mask, int x, int y, int horizontal, int vertical)
{
	int k;
	int dx[8] = { -1,-1,-1,0,0,1,1,1 };
	int dy[8] = { -1,0,1,-1,1,-1,0,1 };
	for (k = 0; k < 8; k++)
	{
		if ((x + dx[k] >= 0) && (x + dx[k] < horizontal))
		{
			if (board[y + dy[k]][x + dx[k]] == MINE || mask[y + dy[k]][x + dx[k]] == FLAG)
				continue;
			else if (board[y + dy[k]][x + dx[k]] == EMPTY_BOARD && mask[y + dy[k]][x + dx[k]] == COVERED)
			{
				mask[y + dy[k]][x + dx[k]] = EMPTY_MASK;
				open_wave(board, mask, x + dx[k], y + dy[k], horizontal, vertical);
			}
			else
			{
				if (board[y + dy[k]][x + dx[k]] == EMPTY_BOARD)
					mask[y + dy[k]][x + dx[k]] = EMPTY_MASK;
				else
					mask[y + dy[k]][x + dx[k]] = board[y + dy[k]][x + dx[k]] + NUM_TO_ASCII;
			}
		}
	}
	return 0;
}
void game_over(int** board, char** mask, int horizontal, int vertical)
{
	system("cls");
	system("color 4F");
	Beep(130, 400);
	Beep(100, 400);
	printf("\n\n\n\n\t 게임 오버 !");
	printf("\n 잠시 후 메인으로 이동합니다.");
	Sleep(1500);
	free_board(board, horizontal, vertical);
	free_mask(mask, horizontal, vertical);
	main();
}
void game_win_check(int** board, char** mask, int horizontal, int vertical, int number_of_mine)
{
	if (unopened_cnt == number_of_mine)
	{
		end = time(NULL);
		system("cls");
		system("color 2F");
		Beep(390, 100);
		Beep(430, 100);
		Beep(470, 100);
		Beep(500, 100);
		printf("\n\n\n\n\t 승 리 !");
		printf("\n 플레이 타임 : %d", (int)(end - start));
		printf("\n 잠시 후 메인으로 이동합니다.");
		if (number_of_mine == 10)
		{
			if ((int)(end - start) < rank_easy)
				rank_easy = (int)(end - start);
		}
		else if (number_of_mine == 40)
		{
			if ((int)(end - start) < rank_medium)
				rank_medium = (int)(end - start);
		}
		else if (number_of_mine == 99)
		{
			if ((int)(end - start) < rank_hard)
				rank_hard = (int)(end - start);
		}
		else
		{
			printf("\n사용자 설정 모드는 랭킹에 반영될 수 없습니다.\n");
		}
		fp = fopen("savefile.txt", "w");
		fprintf(fp, "%d %d %d", rank_easy, rank_medium, rank_hard);
		fclose(fp);

		Sleep(2500);
		free_board(board, horizontal, vertical);
		free_mask(mask, horizontal, vertical);
		main();
	}
}
