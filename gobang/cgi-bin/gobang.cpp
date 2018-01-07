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
#define PVE_FILE "/var/www/cgi-bin/data_file/"


using namespace std;
int X,Y;
int N=15; //棋盘大小
enum MSG_TYPE{
	RESET = 0,
	RUN,
	REVORK,
	RESTART,
};
enum GOAL{
	LIVE_ONE  = 3,
	TO_TWO	  =	5,
	LIVE_TWO  = 45,
	TO_THREE  = 30,
	LIVE_THR  = 460,
	TO_FOUR   = 900,
	LIVE_FOUR = 3000,
	TO_FIVE   = 10000,
};
int m_type = 0;
int m_is_first = 0;
int m_color; //1为黑棋，2为白棋 
int m_x_pos;
int m_y_pos;
int m_level;
char m_timestamp[1024];
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

		void Way(POINT & ps);
		void SetScore(const int i, const int j, const int Who,
				vector<vector<int> > & State); //评分函数
		int p_Score(int num, int bp[]); //实际的评分函数
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
}

void Game::Run(POINT & ps)
{
	Way(ps);
}


//只考虑当前情况，不递归的（贪心法)
void Game::Way(POINT & ps)
{
	int i = 0, j = 0;
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
				SetScore(i, j, m_color, HumanState); //这个的第三个参数是指who
				SetScore(i, j, 3 - m_color, ComputerState);
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
	if(m_level == 2)
	{
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
	}
	else
	{
		if(maxP >= maxC && maxP >= LIVE_FOUR) //人活三子，必须堵住
		{
			ps.x = ps1.x;
			ps.y = ps1.y;
		}
		else
		{
			ps.x = ps2.x;
			ps.y = ps2.y;
		}
	}
	X=ps.x;
	Y=ps.y;
}

//评分函数
void Game::SetScore(const int i, const int j, const int Who,
		vector<vector<int> > & State)
{
	int Another; 

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
	for (k = 1; k <= i; k++) //横向左侧
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
	temp = p_Score(num, bp); //num为两侧己方棋子数，bp则为统计两侧的棋子状态
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
	for (k = 1; k < N - i && k < N - j; k++)
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
	for (k = 1; k < N - j && k <= i; k++)
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
	for (k = 1; k <= j && k < N - i; k++)
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
		max += TO_FIVE; //成五
	}
	else if (num == 3)
	{
		if (bp[1] == 1 && bp[0] == 1) //不成五
		{
			max += 0;
		}
		else if (bp[1] == 0 && bp[0] == 0) //活四
		{
			max += LIVE_FOUR;
		}
		else
		{
			max += TO_FOUR; //冲四
		}
	}
	else if (num == 2)
	{
		if (bp[0] == 0 && bp[1] == 0)
		{
			max += LIVE_THR; //活三
		}
		else if (bp[0] == 1 && bp[1] == 1)
		{
			max += 0; //死三
		}
		else
		{
			max += TO_THREE; //冲三
		}
	}
	else if (num == 1)
	{
		if (bp[0] == 0 && bp[1] == 0)
		{
			max += LIVE_TWO;  //活二
		}
		else if (bp[0] == 1 && bp[1] == 1)
		{
			max += 0;
		}
		else
		{
			max += TO_TWO;
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


static void sw_send_pos_msg_to_js(char* pos_info)
{
	if(pos_info)
	{
		printf("content-type:text/html\n\n");
		printf("%s", pos_info);
	}
	return;
}

static void save_board_to_file()
{
	struct stat stat_info;
	char pre_file[256] = {0};
	char pre_pre_file[256] = {0};
	char cmd[128] = {0};
	memset(&stat_info, 0, sizeof(stat_info));
	snprintf(pre_file, sizeof(pre_file), "%s.pre", m_timestamp);
	snprintf(pre_pre_file, sizeof(pre_pre_file), "%s.pre.pre", m_timestamp);
	if (lstat(m_timestamp, &stat_info) == 0 && (!S_ISDIR(stat_info.st_mode)))
	{
		if (lstat(pre_file, &stat_info) == 0 && (!S_ISDIR(stat_info.st_mode)))
		{
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "cp %s %s", pre_file, pre_pre_file);
			system(cmd);
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "cp %s %s", m_timestamp, pre_file);
			system(cmd);
		}
		else
		{
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "cp %s %s", m_timestamp, pre_file);
			system(cmd);
		}
	}
	FILE *fp = NULL;
	fp = fopen(m_timestamp,"w+");
	if(fp == NULL)
		return;
	char data[1024] = {0};
	int len = 0;
	for (int i = 0; i < N; i++) 
		for (int j = 0; j < N; j++) 
			len += snprintf(data+len, sizeof(data)-len, "%d", board[i][j]);
	fwrite(data, len, 1, fp);
	fclose(fp);
	fp = NULL;
//	chmod(m_timestamp, 0777);
	return;
}

static void sw_get_board_from_file()
{
	FILE *fp = NULL;
	char *p = NULL;
	char buf[1024] = {0};
	char pos[4] = {0};
	int size = 0;
	struct stat stat_info;
	if (lstat(m_timestamp, &stat_info) == 0 && (!S_ISDIR(stat_info.st_mode)))
	{
		fp = fopen(m_timestamp,"r+");
		if(fp == NULL)
		{
			sw_send_pos_msg_to_js((char *)"Error");
			return;
		}
		size = fread(buf, 1, sizeof(buf)-1, fp);
		buf[size] = '\0';
		p = buf;
		for (int i = 0; i < N; i++) 
			for (int j = 0; j < N; j++) 
			{
				strncpy(pos, p, 1);
				if(pos[0] != '\0')
					board[i][j] = atoi(pos); //棋盘初始化 
				else
					board[i][j] = 0;
				if(p && p[0] != '\0')
					p++;
			}
		fclose(fp);
		fp = NULL;
	
	}
	else
	{
		for (int i = 0; i < N; i++) 
			for (int j = 0; j < N; j++) 
				board[i][j] = 0;
	}
	return;
}

static void sw_get_xy_pos()
{
	Game gobang;
	gobang.Init();

	POINT ps; //棋盘上每个格子的状态,0为啥也没有，1为黑棋，2为白棋
	sw_get_board_from_file();

	if(m_color == 1 && m_is_first == 1) //如果己方执黑且是第一步，则占据棋盘中心位置 黑棋为1，白棋为2 
	{ 
		for (int i = 0; i < N; i++) 
			for (int j = 0; j < N; j++) 
				board[i][j] = 0; //棋盘初始化 
		board[N / 2][N / 2] = 1; //更新棋盘信息
		char msg[16] = {0};
		snprintf(msg, sizeof(msg),"%d#%d", N/2, N/2);
		sw_send_pos_msg_to_js(msg);
	//	sw_send_pos_msg_to_js((char *)"Reset OK");
	}
	else 
	{
		board[m_x_pos][m_y_pos] = 3 - m_color;
		//更新棋盘信息 
		do
		{ 
			gobang.Run(ps);
			if (board[X][Y] == 0) //如果该位置为空则占据该位置 
			{
				board[X][Y] = m_color; //更新棋盘信息 
				char msg[16] = {0};
				snprintf(msg, sizeof(msg),"%d#%d", X, Y);
				sw_send_pos_msg_to_js(msg);
				break; //结束循环 
			}
		}
		while (true); 
		//循环直至随机得到一个空位置 
	}
	save_board_to_file();
	gobang.~Game();
}

static void sw_restart_board_data()
{
	int len = 0;
	FILE *fp = NULL;
	char data[1024] = {0};
	int i,j;
	fp = fopen(m_timestamp,"w+");
	if(fp == NULL)
		return;
	for (i = 0; i < N; i++) 
		for (j = 0; j < N; j++) 
			len += snprintf(data+len, sizeof(data)-len, "%d", 0);
	fwrite(data, len, 1, fp);
	fclose(fp);
	fp = NULL;
	if(m_color == 1)
		sw_get_xy_pos();
	else
		sw_send_pos_msg_to_js((char *)"Reset OK");
	return;
}

static void sw_reset_board_data()
{
	int len = 0;
	FILE *fp = NULL;
	char data[1024] = {0};
	int i,j;
	fp = fopen(m_timestamp,"w+");
	if(fp == NULL)
		return;
	for (i = 0; i < N; i++) 
		for (j = 0; j < N; j++) 
			len += snprintf(data+len, sizeof(data)-len, "%d", 0);
	fwrite(data, len, 1, fp);
	fclose(fp);
	fp = NULL;
	sw_send_pos_msg_to_js((char *)"Reset OK");
	return;
}

static void sw_parse_msg_from_js(char *msg, int size)
{
	//strcpy(buf, "type=0#first=0#color=1#x=-1#y=0#level=0");
	if(msg == NULL || size <= 0)
		return;
	char *buf = msg;
	char *pi = NULL;
	pi = strstr(buf, "type=");
	if(pi)
		pi += strlen("type=");
	if(pi)
		m_type = atoi(pi);
	pi = strstr(buf, "first=");
	if(pi)
		pi += strlen("first=");
	if(pi)
		m_is_first = atoi(pi);
	pi = strstr(buf, "color=");
	if(pi)
		pi += strlen("color=");
	if(pi)
		m_color = atoi(pi);
	pi = strstr(buf, "x=");
	if(pi)
		pi += strlen("x=");
	if(pi)
		m_x_pos = atoi(pi);
	pi = strstr(buf, "y=");
	if(pi)
		pi += strlen("y=");
	if(pi)
		m_y_pos = atoi(pi);
	pi = strstr(buf, "level=");
	if(pi)
		pi += strlen("level=");
	if(pi)
		m_level = atoi(pi);
	pi = strstr(buf, "timestamp=");
	if(pi)
		pi += strlen("timestamp=");
	if(pi)
		snprintf(m_timestamp, sizeof(m_timestamp), "%s%s", PVE_FILE, pi);
//	printf("type=%d, is_first=%d, color=%d, x=%d, y=%d, level=%d, m_timestamp=%s.\n", m_type, m_is_first, m_color, m_x_pos, m_y_pos, m_level, m_timestamp);
	return;
}

static void sw_revork_one_step()
{
	struct stat stat_info;
	char pre_file[256] = {0};
	char pre_pre_file[256] = {0};
	char cmd[128] = {0};
	snprintf(pre_file, sizeof(pre_file), "%s.pre", m_timestamp);
	snprintf(pre_pre_file, sizeof(pre_pre_file), "%s.pre.pre", m_timestamp);
	if (lstat(pre_file, &stat_info) == 0 && (!S_ISDIR(stat_info.st_mode)))
	{
		if (lstat(pre_pre_file, &stat_info) == 0 && (!S_ISDIR(stat_info.st_mode)))
		{
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "cp %s %s", pre_file, m_timestamp);
			system(cmd);
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "cp %s %s", pre_pre_file, pre_file);
			system(cmd);
		}
		else
		{
			memset(cmd, 0, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "cp %s %s", pre_file, m_timestamp);
			system(cmd);
		}
		sw_send_pos_msg_to_js((char *)"Revork OK");
	}
	return;
}

int main() 
{ 
	int len = 0;
	char buf[1024] = {0};
	char *recvbuf = NULL;
	recvbuf = getenv("CONTENT_LENGTH");
	if(recvbuf == NULL)
		return -1;
	len = atoi(recvbuf);
	if((len > 0)&&(fgets(buf, len+1, stdin)!=NULL))
	{
		sw_parse_msg_from_js(buf, len+1);
//		strcpy(buf, "type=3#first=1#color=1#x=-1#y=-1#level=0#timestamp=1515255533393");
//		parse_msg_from_js(buf, sizeof(buf));
		switch (m_type){
			case RESET: 
				sw_reset_board_data();
				break;
			case RUN:
				sw_get_xy_pos();
				break;
			case REVORK: //悔棋
				sw_revork_one_step();
				break;
			case RESTART:
				sw_restart_board_data();
				break;
			default:
				break;
		}
	}
	return 0; 
}
