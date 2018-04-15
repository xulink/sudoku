#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<algorithm>
#include<time.h>
using namespace std;
char c;
int sodu[9][9], anssodu[9][9];
int row[50000][10];//用来存储所有的头行
int row1[10] = { 0,1,2,3,4,5,6,7,8,9 };
int shift[9] = { 0, 3, 6, 1, 4, 7, 2, 5, 8 };//位移序列
int maxlong = 0, sum = 0, n,flag;
void recursion(int x)//递归生成所有八位全排列（第一位）固定。
{
	if (x == 9)
	{
		for (int i = 1; i <= 9; i++)
			row[maxlong][i] = row1[i];
		maxlong++;
		return ;
	}
	else
	{
		int k;
		for (int i = x; i <=9; i++)
		{
			k = row1[x];
			row1[x] = row1[i];
			row1[i] = k;
			recursion(x + 1);
			k = row1[i];
			row1[i] = row1[x];
			row1[x] = k;
		}//生成全排列的过程主要是：把第一位固定，后面的位数再求全排列，由此递归。
		return ;
	}
}
void create()
{
	recursion(2);
	FILE* fp = fopen("sudoku.txt", "w");
	for (int m = 0; m < 6; m++)
	{
		if (m)
			std::next_permutation(shift + 3, shift + 6);
		for (int h = 0; h < 6; h++)
		{//两个for循环确定一种位移方式
			if (h)
				std::next_permutation(shift + 6, shift + 9);
			for (int i = 0; i < maxlong; i++)
			{//选定头行
				for (int s = 0; s < 9; s++)
				{
					for (int j = 0; j <= 8; j++)
					{
						if (j)
							fputc(' ', fp);
						c = '0' + row[i][(j + shift[s]) % 9 + 1];//由s和j来确定当前位置的位移序列
						fputc(c, fp);
					}
					if (s < 8)
						fputc('\n', fp);
				}
				sum++;
				if (sum == n)
				{//到达要求的数量即停止
					fclose(fp);
					return;
				}
				fputc('\n', fp);
				fputc('\n', fp);
			}
		}
	}
}
void solve2(int i,int j)//i和j表示当前位置
{
	if (i == 9 && j == 0)
	{
		flag = 1;
		return;
	}
	if (sodu[i][j] != 0)
	{//当前位置已有数字时直接进入下一空格
		anssodu[i][j] = sodu[i][j];
		if (j < 8)
			solve2(i, j + 1);
		else
			solve2(i + 1, 0);//对列坐标特判以确定是否需要换行
		if (flag)
			return;
	}
	else
	{
		for (int k = 1,x,y; k <= 9; k++)
		{
			for (x = 0; x <= 8; x++)
				if (sodu[x][j] == k)
					break;//对第j列进行去重判断
			for (y = 0; y <= 8; y++)
				if (sodu[i][y] == k)
					break;//对第i行进行去重判断
			if (y == 9 && x == 9)
			{
				for (x = (i / 3) * 3; x <= (i / 3) * 3 + 2; x++)
				{
					for (y = (j / 3) * 3; y <= (j / 3) * 3 + 2; y++)
					{
						if (sodu[x][y] == k)
							y = x = 100;
					}
				}//对sudo[i][j]所在的3*3区域进行去重判断
		   if (x == (i / 3) * 3 + 3 && y == (j / 3) * 3 + 3)
				  {//
					  sodu[i][j] = k;
					  anssodu[i][j] = sodu[i][j];
					  if (j < 8)
						  solve2(i, j + 1);
					  else
						  solve2(i + 1, 0);//将k填入当前位置并继续调用回溯函数
					  if (flag)
						  return;//如果已经找到一组解则不再继续递归下去
					  sodu[i][j] = 0;
				  }
			}
		}
	}
}
void solve(FILE*read)
{
	FILE*write = fopen("sudoku.txt", "w");
	int i=0,j=0,first=0;
	char ch;
	while (!feof(read))
	{//读取目标文件直到文件结尾
		ch = fgetc(read);
        if (ch == ' ')
			continue;
		else if (ch >= '0'&&ch <= '9')
		{
			sodu[i][j] = ch - '0';
			j++;
			if (i == 8 && j == 9)
			{
				flag = 0;
				solve2(0,0);//每完整读入一个数独题目就进行求解
				if (first)
				{
					fputc('\n', write);
					fputc('\n', write);
				}
				for (int x = 0; x <= 8; x++)
					for (int y = 0; y <= 8; y++)
					{//将解答写入sudoku.txt文件
						fputc(anssodu[x][y]+'0', write);
						if (y < 8)
							fputc(' ', write);
						else if (x < 8)
							fputc('\n', write);
					}
				first = 1;
				i = j = 0;//重置ij的值以进行下一次读入
			}
		}
		else if (ch == '\n')
		{
			if (j == 9)
			{
				i++;
				j = 0;
			}
		}
	}
	fclose(write);
}
int main(int argc, char *argv[])
{//对argv[1]进行判断以确定是对应于那种要求
	if (strcmp(argv[1], "-c") == 0)
	{
		n = atoi(argv[2]);
		if (n < 1)//处理不合理输入的情况
			printf("ERROR:-c N to create N different sudoku final-status.\n");
		else
		{
			double start_time = clock();
			create();
			printf("%d sudoku final games created. Execution time : %.03lf s\n", 
				n, (clock() - start_time) / CLOCKS_PER_SEC);
		}
	}
	else if (strcmp(argv[1], "-s") == 0)
	{
		FILE*read = fopen(argv[2], "r");
		if (read == NULL)//处理文件路径不合法的情况
			printf("cannot open the file\n");
		else
		{
			double start_time = clock();
			solve(read);
			printf("those sudoku puzzles solved. Execution time : %.03lf s\n",
				(clock() - start_time) / CLOCKS_PER_SEC);

		}
		fclose(read);
	}
	return 0;
}