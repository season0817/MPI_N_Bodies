#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include<list>
#include <vector>
#include<algorithm>
#include<time.h>
#define random(x) (rand()%x)
#include "quadtree.h"
#include <easyx.h>			// 引用图形库头文件
#include <conio.h>
//星体个数即任务个数
#define BODYNUMS 100
//发送缓冲区大小：必须大于tasknum,tasknum最大为BODYNUMS（即任务总数）
#define BUFFER_SIZE 100
#define G 9.8

using namespace std;


int main(int argc, char *argv[]) {
	
	/*变量声明*/
	//星体数据初始化
	int vs[BODYNUMS];
	int ms[BODYNUMS];
	int pxs[BODYNUMS], pys[BODYNUMS];
	int rs[BODYNUMS];
	//计算的合力---加速度结果
	//float forcesA[BODYNUMS];//要发送的数据
	/*END变量声明END*/


	/*MPI变量声明*/
	//gsize:进程总数；rank:当前进程的进程标识号
	int  gsize, rank;
	//发送buffer数组
	float sbuf[BUFFER_SIZE];
	//接受buffer数组
	float *rbuf, rbuf_size;
	//计算的合力---加速度结果
	float forcesA[BODYNUMS];//要发送的数据
	//要处理的源数据
	/*END MPI变量声明 END*/

	/*MPI初始化*/
	MPI_Status status;
	MPI_Request request;
	MPI_Init(&argc, &argv); /* 初始化MPI环境*/
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); /*获取当前正在运行进程的标识号*/
	MPI_Comm_size(MPI_COMM_WORLD, &gsize); /*获取所有参与运算的进程个数*/
	/*END MPI初始化 END*/

	//设置接受缓冲区大小
	rbuf_size = BODYNUMS;
	rbuf = (float *)malloc(rbuf_size * sizeof(float));

	//单个进程需要完成的任务数(无需广播）
	int tasknum = BODYNUMS / gsize;

	//根进程初始化数据
	if (rank == 0) {
		//随机方法初始化初始数据
		initgraph(500, 500, SHOWCONSOLE);
		srand((unsigned)time(NULL));
		for (int i = 0; i < BODYNUMS; i++)
		{
			//初速度随机范围为[1,200]
			vs[i] = (rand() % (200)) + 1;
			//质量随机范围为[50,400]
			ms[i] = (rand() % (400 - 50 + 1)) + 50;
			//坐标（x,y)在（0，0）到（500，500）内
			pxs[i] = (rand() % (501));
			pys[i] = (rand() % (501));
			//半径随机范围为[1,10]
			rs[i] = (rand() % (10)) + 1;
			//运动方向
			cout <<"origin:"<< i << ":(" << pxs[i] << "," << pys[i] << ")\tr=" << rs[i] << ",m=" << ms[i] << ",v=" << vs[i] << endl;
			solidcircle(pxs[i], pys[i], rs[i]);
			
		}
		getchar();
	}

	for (int cycle = 0; cycle < 5; cycle++) {
		//所需数据进行广播
		MPI_Bcast(&vs, BODYNUMS, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&ms, BODYNUMS, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&pxs, BODYNUMS, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&pys, BODYNUMS, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&rs, BODYNUMS, MPI_INT, 0, MPI_COMM_WORLD);


		//每个进程处理自己的任务
		int g;
		// 每个进程为sbuf 赋值
		for (g = 0; g < gsize; ++g)
		{
			//进入进程g
			if (rank == g)
			{
				/*自行建树，并完成自己的计算任务*/
				//创建结点list用于建树
				list<QuadNode> tempBodys;
				for (int i = 0; i < BODYNUMS; ++i)
				{
					QuadNode temp = {
						i,//index
						pxs[i],//x
						pys[i],//y
						ms[i],
					};
					tempBodys.push_back(temp);
				}
				//建立四叉树
				QuadTree T;
				InitQuadTree(&T);
				CreateTree(&T, 0, 0, 500, 500, tempBodys);
				//printTree(T);
				/*END建树完毕END*/


				//处理属于自己的tasknum个任务的计算，并放到发送缓冲数组中
				int count = 0;
				int count_complete = 0;
				for (auto node : tempBodys)
				{
					if (count >= rank && count < (tasknum + rank))
					{
						float tempm = Tot_Mass(T, node);
						int tempx = Tot_Center_X(T, node);
						int tempy = Tot_Center_Y(T, node);

						int w_r = fabs(node.x - tempx);
						int h_r = fabs(node.y - tempy);
						int tempr2 = w_r*w_r + h_r*h_r;

						forcesA[count_complete] = G*tempm / tempr2;
						sbuf[count_complete] = G*tempm / tempr2;
						//cout << sbuf[count_complete] << endl;
						count_complete += 1;
						//cout << node.index << ":" << forces[i] <<"("<<tempx<<","<<tempy<<"),m="<<tempm<< endl;
					}
					count += 1;
				}
			}
		}


		MPI_Gather(sbuf, tasknum, MPI_INT, rbuf, tasknum, MPI_INT, 0, MPI_COMM_WORLD);


		//根进程处理gather得到的数据
		if (rank == 0)
		{
			//update
			int delta = 1;//间隔时间
			cleardevice();
			srand((unsigned)time(NULL));
			for (int i = 0; i < rbuf_size; i++)
			{
				double cos = double(rand() / double(RAND_MAX));
				double probx = double(rand() / double(RAND_MAX));
				double proby = double(rand() / double(RAND_MAX));
				int directionx = 1;
				if (probx >= 0.5) {
					directionx = -1;
				}
				int directiony = 1;
				if (proby >= 0.5) {
					directiony = -1;
				}
				vs[i] = vs[i] + rbuf[i] * delta;
				pxs[i] = pxs[i] + vs[i] * delta*cos*probx;
				pys[i] = pys[i] + vs[i] * delta*(1 - cos)*proby;
				//cout << rbuf[i] << endl;
				cout << "after:" << i << 
					":(" << pxs[i] << "," << pys[i] << ")\tr=" << rs[i] <<
					",m=" << ms[i] << ",v=" << vs[i] << endl;
				//setfillcolor(BLUE);
				solidcircle(pxs[i], pys[i], rs[i]);
			}
			getchar();
		}

	}
	MPI_Finalize();

	_getch();
	closegraph();
	
	return 0;
}