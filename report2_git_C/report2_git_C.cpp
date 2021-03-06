// report1.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <iostream>
#include <conio.h>
#include <random>

#define MAX_GENERATION 30000
#define MAX_GENOM_LIST 50
#define MAX_WEIGHT 60
#define VAR_NUM 2

#define INDIVIDUAL_MUTATION_RATE 0.3//個体突然変異率
#define ALPHA 0.5

#define COEFFICIENT 0.1//評価関数の係数

double varMax[] = { 4,4 };
double varMin[] = { -1.5,-3 };
double eliteX[VAR_NUM];
double x[MAX_GENOM_LIST][VAR_NUM];//座標
double x_prev[MAX_GENOM_LIST][VAR_NUM];//座標
double result[MAX_GENOM_LIST] = {};//評価関数の値
double functionValue[MAX_GENOM_LIST] = {};//与えられた関数の値
double resultSumValue = 0;//評価関数の合計

bool selection();
bool blxAlphaCrossover();
bool mutation();
bool calc(bool enableDisplay);
bool calcResult();
bool prevDataToData();
int random(int min, int max);
double random(int min, double max);
double random(double min, int  max);
double random(double min, double max);

int main()
{
	for (int i = 0; i < MAX_GENOM_LIST; i++)//変数の初期化
	{
		for (int j = 0; j < VAR_NUM; j++)
		{
			x[i][j] = random(varMin[j], varMax[j]);//遺伝子の初期設定
			printf_s("%lf", x[i][j]);
		}
		printf_s(" \t f(x,y)=%7.4lf\t Result=%9.5lf\n", functionValue[i], result[i]);
	}

	calcResult();

	for (int i = 0; i <= MAX_GENERATION; i++)//メインのループ
	{
		bool change = selection();//選択

		blxAlphaCrossover();

		//mutation();//突然変異

		if (i % (MAX_GENERATION / 10) == 0)
		{
			printf_s("i=%d\n", i);
			calc(true);
		}
		else
		{
			calc(false);
		}//評価関数の計算
	}
	calc(true);//終了時の変数表示
	while (!_kbhit())
	{

	}
	return 0;
}

bool selection()
{
	int max_num = 0;//最も評価の良い個体の番号
	double resultSumValue = 0;
	calc(false);

	for (int i = 0; i < MAX_GENOM_LIST; i++)
		//ルーレット選択用に評価関数の合計と一番評価の良い番号を取得
	{
		if (result[i] > result[max_num])
			max_num = i;
		resultSumValue += result[i];
	}

	for (int i = 0; i < VAR_NUM; i++)
	{
		eliteX[i] = x[max_num][i];
	}

	for (int i = 0; i < MAX_GENOM_LIST; i++)
	{
		for (int j = 0; j < VAR_NUM; j++)
		{
			x_prev[i][j] = x[i][j];
		}
	}

	for (int i = 0; i < MAX_GENOM_LIST; i++)
	{
		double selector = random(0.0, 1.0);//乱数を生成
		double needle = 0;//ルーレットの針を生成
		int j = 0;
		for (;; j++)
		{
			needle += (result[j] / resultSumValue);//ルーレットの針を乱数の値まで進める
			if (needle > selector)
				break;
			if (j == (MAX_GENOM_LIST - 1))
				break;
		}
		for (int k = 0; k < VAR_NUM; k++)
		{
			x[i][k] = x_prev[j][k];
		}
	}

	return false;
}

bool blxAlphaCrossover()
{
	prevDataToData();

	for (int i = 0; i < MAX_GENOM_LIST; i += 2)//2個ずつ交叉
	{
		for (int j = 0; j < VAR_NUM; j++)
		{
			double ave, length;

			ave = (x[i][j] + x[i + 1][j]) / 2;
			length = std::abs((x[i][j] - x[i + 1][j]));

			x[i][j] = random(ave - length * (1 + ALPHA * 2) / 2, ave + length * (1 + ALPHA * 2) / 2);
			x[i + 1][j] = random(ave - length * (1 + ALPHA * 2) / 2, ave + length * (1 + ALPHA * 2) / 2);
		}
	}
	return true;
}

bool mutation()
{
	for (int i = 1; i < MAX_GENOM_LIST; i++)
	{
		if (random(0.0, 1.0) <= INDIVIDUAL_MUTATION_RATE)//個体突然変異率の計算
		{
			for (int j = 0; j < VAR_NUM; j++)
			{
				x[i][j] = random(varMin[j], varMax[j]);//遺伝子突然変異率の計算　変化する場合はビット反転
			}
		}
	}
	return true;
}

bool calc(bool enableDisplay)
{
	int minNum = 0, maxNum = 0;

	calcResult();
	for (int i = 0; i < MAX_GENOM_LIST; i++)
	{
		//評価関数が最小の奴と最大のやつを検索
		if (result[i] < result[minNum])
		{
			minNum = i;
		}
		if (result[i] > result[maxNum])
		{
			maxNum = i;
		}
	}
	//評価関数が最もいいやつを保存
	for (int i = 0; i < VAR_NUM; i++)
	{
		x[minNum][i] = eliteX[i];
	}

	calcResult();
	if (enableDisplay)
	{
		for (int i = 0; i < MAX_GENOM_LIST; i++)
		{
			for (int j = 0; j < VAR_NUM; j++)
			{
				printf_s("%lf,", x[i][j]);//デバッグ用
			}
			printf_s(" \t f(x,y)=%7.4lf\t Result=%7.4lf\n", functionValue[i], result[i]);
		}
	}
	return true;
}

bool calcResult()
{
	int maxNum = 0;
	double seg;
	for (int i = 0; i < MAX_GENOM_LIST; i++)
	{
		functionValue[i] = std::sin(x[i][0] + x[i][1]) + std::pow((x[i][0] - x[i][1]), 2.0) - 1.5*x[i][0] + 2.5*x[i][1] + 1;//与えられた関数
		if (functionValue[maxNum] < functionValue[i])//座標の中で最も関数が大きいやつを検索
		{
			maxNum = i;
		}
	}
	seg = functionValue[maxNum];//評価関数の切片を与えられた関数が最も大きいやつにセット
	double resultSumValue = 0;

	for (int i = 0; i < MAX_GENOM_LIST; i++)
	{
		bool flag = true;
		double coefficient = 0.01;//評価関数用の定数
		for (int j = 0; j < VAR_NUM; j++)
		{
			if (x[i][j] > varMax[j] || x[i][j] < varMin[j])//座標が場外にいるやつの処理
				flag = false;
		}
		result[i] = std::pow((functionValue[i] - seg), 2.0);//与えられた関数の値から切片で設定した値を引いて2乗する→与えられた関数の値が小さいやつが強くなる

		if (!flag)//場外に出たやつの処理
		{
			result[i] *= coefficient;
		}
		resultSumValue += result[i];
	}
	return true;
}

bool prevDataToData()
{
	for (int i = 0; i < MAX_GENOM_LIST; i++)
	{
		for (int j = 0; j < VAR_NUM; j++)
		{
			x_prev[i][j] = x[i][j];
		}
	}
	return true;
}

int random(int min, int max)
{
	//乱数の設定(int)
	std::random_device rnd;
	std::mt19937 engine(rnd());
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(engine);
}

double random(int min, double max)
{
	return random((double)min, max);
}

double random(double min, int max)
{
	return random(min, (double)max);
}

double random(double min, double max)
{
	//乱数の設定(double)
	std::random_device rnd;
	std::mt19937 engine(rnd());
	std::uniform_real_distribution<double> distribution(min, max);
	return distribution(engine);
}