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
#include <easyx.h>			// ����ͼ�ο�ͷ�ļ�
#include <conio.h>
//����������������
#define BODYNUMS 100
//���ͻ�������С���������tasknum,tasknum���ΪBODYNUMS��������������
#define BUFFER_SIZE 100
#define G 9.8

using namespace std;


int main(int argc, char *argv[]) {
	
	/*��������*/
	//�������ݳ�ʼ��
	int vs[BODYNUMS];
	int ms[BODYNUMS];
	int pxs[BODYNUMS], pys[BODYNUMS];
	int rs[BODYNUMS];
	//����ĺ���---���ٶȽ��
	//float forcesA[BODYNUMS];//Ҫ���͵�����
	/*END��������END*/


	/*MPI��������*/
	//gsize:����������rank:��ǰ���̵Ľ��̱�ʶ��
	int  gsize, rank;
	//����buffer����
	float sbuf[BUFFER_SIZE];
	//����buffer����
	float *rbuf, rbuf_size;
	//����ĺ���---���ٶȽ��
	float forcesA[BODYNUMS];//Ҫ���͵�����
	//Ҫ�����Դ����
	/*END MPI�������� END*/

	/*MPI��ʼ��*/
	MPI_Status status;
	MPI_Request request;
	MPI_Init(&argc, &argv); /* ��ʼ��MPI����*/
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); /*��ȡ��ǰ�������н��̵ı�ʶ��*/
	MPI_Comm_size(MPI_COMM_WORLD, &gsize); /*��ȡ���в�������Ľ��̸���*/
	/*END MPI��ʼ�� END*/

	//���ý��ܻ�������С
	rbuf_size = BODYNUMS;
	rbuf = (float *)malloc(rbuf_size * sizeof(float));

	//����������Ҫ��ɵ�������(����㲥��
	int tasknum = BODYNUMS / gsize;

	//�����̳�ʼ������
	if (rank == 0) {
		//���������ʼ����ʼ����
		initgraph(500, 500, SHOWCONSOLE);
		srand((unsigned)time(NULL));
		for (int i = 0; i < BODYNUMS; i++)
		{
			//���ٶ������ΧΪ[1,200]
			vs[i] = (rand() % (200)) + 1;
			//���������ΧΪ[50,400]
			ms[i] = (rand() % (400 - 50 + 1)) + 50;
			//���꣨x,y)�ڣ�0��0������500��500����
			pxs[i] = (rand() % (501));
			pys[i] = (rand() % (501));
			//�뾶�����ΧΪ[1,10]
			rs[i] = (rand() % (10)) + 1;
			//�˶�����
			cout <<"origin:"<< i << ":(" << pxs[i] << "," << pys[i] << ")\tr=" << rs[i] << ",m=" << ms[i] << ",v=" << vs[i] << endl;
			solidcircle(pxs[i], pys[i], rs[i]);
			
		}
		getchar();
	}

	for (int cycle = 0; cycle < 5; cycle++) {
		//�������ݽ��й㲥
		MPI_Bcast(&vs, BODYNUMS, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&ms, BODYNUMS, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&pxs, BODYNUMS, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&pys, BODYNUMS, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&rs, BODYNUMS, MPI_INT, 0, MPI_COMM_WORLD);


		//ÿ�����̴����Լ�������
		int g;
		// ÿ������Ϊsbuf ��ֵ
		for (g = 0; g < gsize; ++g)
		{
			//�������g
			if (rank == g)
			{
				/*���н�����������Լ��ļ�������*/
				//�������list���ڽ���
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
				//�����Ĳ���
				QuadTree T;
				InitQuadTree(&T);
				CreateTree(&T, 0, 0, 500, 500, tempBodys);
				//printTree(T);
				/*END�������END*/


				//���������Լ���tasknum������ļ��㣬���ŵ����ͻ���������
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


		//�����̴���gather�õ�������
		if (rank == 0)
		{
			//update
			int delta = 1;//���ʱ��
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