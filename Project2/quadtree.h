#pragma once
#include <list>
#include <vector>
#include<algorithm>
#include<time.h>
#include <iostream>
using namespace std;
typedef struct QuadNode
{
	int index;
	int x;
	int y;
	int quality;
	struct QuadNode *nwchild, *nechild, *swchild, *sechild;
}QuadNode, *QuadTree;
void InitQuadTree(QuadTree *T);
void DestroyQuadTree(QuadTree *T);
void printTree(QuadTree T);
float Tot_Mass(QuadTree T, QuadNode target);
int Tot_Center_X(QuadTree T, QuadNode target);
int Tot_Center_Y(QuadTree T, QuadNode target);
void CreateTree(QuadTree *T, int wmin, int hmin, int wmax, int hmax, list<QuadNode> nodes);

