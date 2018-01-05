/**
 * @file gobang.cpp
 * @brief 
 * @author YOUR NAME (), 
 * @version 1.0
 * @history
 * 		参见 :
 * 		2012-11-21 YOUR NAME created
 */

#include <vector>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#define PVE_FILE "pve_file"


using namespace std;
int X,Y;
int colour;
int N=15; //棋盘大小
typedef struct node
{
	int x;
	int y;
}POINT;
vector<vector<int> > board;
//五子棋游戏类，进行游戏相关的操作

class Game
{
	private:
		int Total; //落子总数，用于判断是否已经下满，下满则为和棋

	public: //供外界调用的接口
		Game(); //构造函数，默认大小为15
		~Game(void);
		void Init(void); //数据初始化

		void Clear(void); //清空游戏数据
		void Run(POINT & ps); //计算机走棋

	private://核心算法模块

		//算法2
		void Way(POINT & ps);
		void SetScore(const int i, const int j, const int Who,
				vector<vector<int> > & State); //评分函数
		int p_Score(int num, int bp[]); //实际的评分函数
		bool first; //是否是第一步
};

Game::Game()
{
	Init();
}

Game::~Game(void)
{
}

void Game::Init(void)
{
	vector<int> temp(N);
	int i;
	for (i = 0; i < N; i++)
	{
		board.push_back(temp);
	}
	first = true; //是否是第一步，算法二要用
}

void Game::Run(POINT & ps)
{
	Way(ps);
}


//算法2：只考虑当前情况，不递归的（贪心法)
//参考Zhong_Zw的程序，下载自CSDN
void Game::Way(POINT & ps)
{
	int i,j;
	vector<vector<int> > HumanState, ComputerState;
	vector<int> temp(N, 0);
	int maxP = 0, maxC = 0; //计算机和人状态值的最大值
	POINT ps1, ps2;
	//如果是开局先走，下中央
	if (Total == 0)
	{
		ps.x = N/2;
		ps.y = N/2;
	}
	//初始化计算机和人的状态矩阵

	for (i = 0; i < N; i++)
	{
		HumanState.push_back(temp);
		ComputerState.push_back(temp);
	}


	for ( i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			if (board[i][j] == 0)
			{
				SetScore(i, j, colour, HumanState);
				SetScore(i, j, 3-colour, ComputerState);
			}
		}
	}


	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			if (HumanState[i][j] > maxP)
			{
				maxP = HumanState[i][j];
				ps1.x = i;
				ps1.y = j;
			}
			if (ComputerState[i][j] > maxC)
			{
				maxC = ComputerState[i][j];
				ps2.x = i;
				ps2.y = j;
			}
		}
	}
	if (maxP >= maxC)
	{
		ps.x = ps1.x;
		ps.y = ps1.y;
	}
	else
	{
		ps.x = ps2.x;
		ps.y = ps2.y;
	}
	X=ps.x;
	Y=ps.y;
}

//评分函数
void Game::SetScore(const int i, const int j, const int Who,
		vector<vector<int> > & State)
{
	int Another; //与要求的状态相对的另一方
	int bp[2], k, num, max = 0, temp = 0;
	if (Who == 1)
	{
		Another = 2;
	}
	else if (Who == 2)
	{
		Another = 1;
	}
	else
	{
		return;
	}


	//横向
	//向右
	bp[0] = 1; bp[1] = 1;
	num = 0;
	for (k = 1; k < N - i; k++)
	{
		if (board[i+k][j] == Who)
		{
			num++;
			continue;
		}
		if (board[i+k][j] == 0)
		{
			bp[1] = 0;
			break;
		}
		if (board[i+k][j] == Another)
		{
			bp[1] = 1;
			break;
		}
	}
	//向左
	for (k = 1; k <= i; k++)
	{
		if (board[i-k][j] == Who)
		{
			num++;
			continue;
		}
		if (board[i-k][j] == 0)
		{
			bp[0] = 0;
			break;
		}
		if (board[i-k][j] == Another)
		{
			bp[0] = 1;
			break;
		}
	}
	//调用评分函数
	temp = p_Score(num, bp);
	max += temp;

	//纵向
	bp[0] = 1; bp[1] = 1;
	num = 0;
	//向下
	for (k = 1; k < N - j; k++)
	{
		if (board[i][j+k] == Who)
		{
			num++;
			continue;
		}
		if (board[i][j+k] == 0)
		{
			bp[1] = 0;
			break;
		}
		if (board[i][j+k] == Another)
		{
			bp[1] = 1;
			break;
		}
	}
	//向上
	for (k = 1; k <= j; k++)
	{
		if (board[i][j-k] == Who)
		{
			num++;
			continue;
		}
		if (board[i][j-k] == 0)
		{
			bp[0] = 0;
			break;
		}
		if (board[i][j-k] == Another)
		{
			bp[0] = 1;
			break;
		}
	}
	temp = p_Score(num, bp);
	max += temp;

	//从左上到右下
	bp[0] = 1; bp[1] = 1;
	num = 0;

	//下
	for (k = 1; k < N-i && k < N-j; k++)
	{
		if (board[i+k][j+k] == Who)
		{
			num++;
			continue;
		}
		if (board[i+k][j+k] == 0)
		{
			bp[1] = 0;
			break;
		}
		if (board[i+k][j+k] == Another)
		{
			bp[1] = 1;
			break;
		}
	}
	//上
	for (k = 1; k <= j && k <= i; k++)
	{
		if (board[i-k][j-k] == Who)
		{
			num++;
			continue;
		}
		if (board[i-k][j-k] == 0)
		{
			bp[0] = 0;
			break;
		}
		if (board[i-k][j-k] == Another)
		{
			bp[0] = 1;
			break;
		}
	}
	temp = p_Score(num, bp);
	max += temp;

	//从右上到左下
	bp[0] = 1; bp[1] = 1;
	num = 0;
	//下
	for (k = 1; k < N-j && k <= i; k++)
	{
		if (board[i-k][j+k] == Who)
		{
			num++;
			continue;
		}
		if (board[i-k][j+k] == 0)
		{
			bp[1] = 0;
			break;
		}
		if (board[i-k][j+k] == Another)
		{
			bp[1] = 1;
			break;
		}
	}
	//上
	for (k = 1; k <= j && k < N-i; k++)
	{
		if (board[i+k][j-k] == Who)
		{
			num++;
			continue;
		}
		if (board[i+k][j-k] == 0)
		{
			bp[0] = 0;
			break;
		}
		if (board[i+k][j-k] == Another)
		{
			bp[0] = 1;
			break;
		}
	}
	temp = p_Score(num, bp);
	max += temp;

	if (max > State[i][j])
	{
		State[i][j] = max;
	}
}

//实际的评分函数
int Game::p_Score(int num, int bp[])
{
	int max = 0;
	if (num >= 4)
	{
		max += 10000; //成五
	}
	else if (num == 3)
	{
		if (bp[1] == 1 && bp[0] == 1) //不成五
		{
			max += 0;
		}
		else if (bp[1] == 0 && bp[0] == 0) //活四
		{
			max += 3000;
		}
		else
		{
			max += 900; //冲四
		}
	}
	else if (num == 2)
	{
		if (bp[0] == 0 && bp[1] == 0)
		{
			max += 460; //活三
		}
		else if (bp[0] == 1 && bp[1] == 1)
		{
			max += 0; //不成五
		}
		else
		{
			max += 30; //死三
		}
	}
	else if (num == 1)
	{
		if (bp[0] == 0 && bp[1] == 0)
		{
			max += 45;  //活二
		}
		else if (bp[0] == 1 && bp[1] == 1)
		{
			max += 0;
		}
		else
		{
			max += 5;
		}
	}
	else if (num == 0)
	{
		if (bp[0] == 0 && bp[1] == 0)
		{
			max += 3;
		}
		else if (bp[0] == 1 && bp[1] == 1)
		{
			max += 0;
		}
		else
		{
			max += 1;
		}
	}
	return max;
}

static char m_is_first[8] = {0}; 
static char m_color[8] = {0};
static char m_x_pos[4] = {0};
static char m_y_pos[4] = {0};
static bool m_init = false;

static void send_pos_msg_to_js(char* pos_info)
{
	if(pos_info)
	{
		printf("content-type:text/html\n\n");
		printf("%s", pos_info);
	}
	return;
}

Game wahaha;
static void get_xy_pos()
{
	FILE *fp = NULL;
	char *p = NULL, *q = NULL;
	char buf[1024] = {0};
	char pos[4] = {0};
	fp = fopen(PVE_FILE,"r+");
	if(fp == NULL)
	{
		send_pos_msg_to_js((char *)"Error");
		return;
	}
	int size = fread(buf, 1, sizeof(buf)-1, fp);
	buf[size] = '\0';
	p = buf;
	wahaha.Init();
//	char pos_info[4046] = {0};
	for (int i = 0; i < N; i++) 
		for (int j = 0; j < N; j++) 
		{
			strncpy(pos, p, 1);
			if(pos && pos[0] != '\0')
				board[i][j] = atoi(pos); //棋盘初始化 
			else
				board[i][j] = 0;
			if(p && p[0] != '\0')
				p++;
		}
#if 0
			if(board[i][j] == 1)
				snprintf(pos_info+strlen(pos_info), sizeof(pos_info)-strlen(pos), "[x:%d,y:%d]", i, j);
			else
				snprintf(pos_info+strlen(pos_info), sizeof(pos_info)-strlen(pos), "[%d+%d]", i, j);
		}
	printf("content-type:text/html\n\n");
	printf("%s", pos_info);
#endif
	if(strcmp(m_color, "black") == 0)
		colour = 1;
	else
		colour = 2;

	POINT ps; //棋盘上每个格子的状态,0为啥也没有，1为黑棋，2为白棋

	if(colour == 1 && strcmp(m_is_first, "true") == 0) //如果己方执黑且是第一步，则占据棋盘中心位置 黑棋为1，白棋为2 
	{ 
		for (int i = 0; i < N; i++) 
			for (int j = 0; j < N; j++) 
				board[i][j] = 0; //棋盘初始化 
		board[N / 2][N / 2] = 1; //更新棋盘信息
		char msg[16] = {0};
		snprintf(msg, sizeof(msg),"%d#%d", N/2, N/2);
		send_pos_msg_to_js(msg);
	}
	else 
	{
		board[atoi(m_x_pos)][atoi(m_y_pos)] = colour;
		//更新棋盘信息 
		do
		{ 
			wahaha.Run(ps);
			if (board[X][Y] == 0) //如果该位置为空则占据该位置 
			{
				board[X][Y] = colour; //更新棋盘信息 
				char msg[16] = {0};
				snprintf(msg, sizeof(msg),"%d#%d", X, Y);
				send_pos_msg_to_js(msg);
				break; //结束循环 
			}
		}
		while (true); 
		//循环直至随机得到一个空位置 
	}
	fp = fopen(PVE_FILE,"w+");
	if(fp == NULL)
		return;
	char data[1024] = {0};
	int len = 0;
	for (int i = 0; i < N; i++) 
		for (int j = 0; j < N; j++) 
			len += snprintf(data+len, sizeof(data)-len, "%d", board[i][j]);
	fwrite(data, len, 1, fp);
#if 0
	printf("content-type:text/html\n\n");
	printf("%s", data);
#endif
	fclose(fp);
	fp = NULL;
	chmod(PVE_FILE, 0777);
	wahaha.~Game();
}

static void *parse_thread_func(void *arg)
{
	int len = 0;
	char buf[1024] = {0};
	char *recvbuf = NULL;
	recvbuf = getenv("CONTENT_LENGTH");
	if(recvbuf == NULL)
		return NULL;
	len = atoi(recvbuf);
	if((len > 0)&&(fgets(buf, len+1, stdin)!=NULL))
	{
		char *p = NULL, *q = NULL;
		p = q = buf;
		q = strstr(p, "#");
		if(q)
		{
			strncpy(m_is_first, p, q-p);
			q++;
		}
		p = strstr(q, "#");
		if(p)
		{
			strncpy(m_color, q, p-q);
			p++;
		}
		q = strstr(p, "#");
		if(q)
		{
			strncpy(m_x_pos, p, q-p);
			q++;
		}
		strcpy(m_y_pos, q);
		get_xy_pos();
	}
	return NULL;
}

int main() 
{ 
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_t m_cmd_thread_id;
	sched_param param;
	param.sched_priority = 0;
	int res = pthread_create(&m_cmd_thread_id, &attr, parse_thread_func, NULL);
	if(res == -1)
		return -1;
	pthread_join(m_cmd_thread_id, NULL);
	return 0; 
}
