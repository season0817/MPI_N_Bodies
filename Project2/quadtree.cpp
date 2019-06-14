#include "quadtree.h"
#include <stdio.h>
#include<malloc.h> 
#include <list>
#include <vector>
#include<algorithm>
#include<time.h>
#include <iostream>
using namespace std;

void InitQuadTree(QuadTree *T) {
	*T = NULL;
}
void DestroyQuadTree(QuadTree *T) {
	if (*T)
	{
		if ((*T)->nechild)
			DestroyQuadTree(&(*T)->nechild);
		if ((*T)->nwchild)
			DestroyQuadTree(&(*T)->nwchild);
		if ((*T)->sechild)
			DestroyQuadTree(&(*T)->sechild);
		if ((*T)->swchild)
			DestroyQuadTree(&(*T)->swchild);
	}
	free(*T);
	*T = NULL;
}
void printTree(QuadTree T)
{
	if (T)
	{
		cout <<T->index<< ":(" << T->x << ',' << T->y << ")" << endl;
		printTree(T->nwchild);
		printTree(T->nechild);
		printTree(T->swchild);
		printTree(T->sechild);
	}
}
float Tot_Mass(QuadTree T, QuadNode target)
{
	float nw = 0, ne = 0, sw = 0, se = 0;
	int count = 0;
	if (!T->nwchild && !T->nechild && !T->sechild && !T->swchild) {
		if (T->index != target.index) {
			return T->quality;
		}
		else {
			return 0.0;
		}
	}
	if (T->nwchild)
	{
		nw=Tot_Mass(T->nwchild, target);
	}
	if (T->nechild)
	{
		ne = Tot_Mass(T->nechild, target);
	}
	if (T->swchild)
	{
		sw = Tot_Mass(T->swchild, target);
	}
	if (T->sechild)
	{
		se = Tot_Mass(T->sechild, target);
	}
	if (nw != 0.0)
	{
		count += 1;
	}
	if (ne != 0.0)
	{
		count += 1;
	}
	if (sw != 0.0)
	{
		count += 1;
	}
	if (se != 0.0)
	{
		count += 1;
	}
	return float((nw + ne + sw + se) / count);
}
int Tot_Center_X(QuadTree T, QuadNode target)
{
	
	float nw = 0, ne = 0, sw = 0, se = 0;
	int count = 0;
	if (!T->nwchild && !T->nechild && !T->sechild && !T->swchild) {
		if (T->index != target.index) {
			//cout << "T->x:"<<T->x << endl;
			return T->x;
		}
		else {
			return 0.0;
		}
	}
	
	if (T->nwchild)
	{
		nw = Tot_Center_X(T->nwchild, target);
	}
	if (T->nechild)
	{
		ne = Tot_Center_X(T->nechild, target);
	}
	if (T->swchild)
	{
		sw = Tot_Center_X(T->swchild, target);
	}
	if (T->sechild)
	{
		se = Tot_Center_X(T->sechild, target);
	}
	if (nw != 0.0)
	{
		count += 1;
	}
	if (ne != 0.0)
	{
		count += 1;
	}
	if (sw != 0.0)
	{
		count += 1;
	}
	if (se != 0.0)
	{
		count += 1;
	}

	return int((nw + ne + sw + se) / count);
}
int Tot_Center_Y(QuadTree T, QuadNode target)
{
	float nw = 0, ne = 0, sw = 0, se = 0;
	int count = 0;
	if (!T->nwchild && !T->nechild && !T->sechild && !T->swchild) {
		if (T->index != target.index) {
			//cout << "T->quality:"<<T->quality << endl;
			return T->y;
		}
		else {
			return 0.0;
		}
	}
	if (T->nwchild)
	{
		nw = Tot_Center_Y(T->nwchild, target);
	}
	if (T->nechild)
	{
		ne = Tot_Center_Y(T->nechild, target);
	}
	if (T->swchild)
	{
		sw = Tot_Center_Y(T->swchild, target);
	}
	if (T->sechild)
	{
		se = Tot_Center_Y(T->sechild, target);
	}
	if (nw != 0.0)
	{
		count += 1;
	}
	if (ne != 0.0)
	{
		count += 1;
	}
	if (sw != 0.0)
	{
		count += 1;
	}
	if (se != 0.0)
	{
		count += 1;
	}

	return int((nw + ne + sw + se) / count);
}
void CreateTree(QuadTree *T, int wmin, int hmin, int wmax, int hmax, list<QuadNode> nodes)
{
	if (nodes.size() > 1)
	{
		list<QuadNode> nw, ne, sw, se;
		for (auto n : nodes)
		{
			if (n.x < ((wmin + wmax) / 2))
			{
				if (n.y < ((hmin + hmax) / 2))
					nw.push_back(n);
				else
					sw.push_back(n);
			}
			else {
				if (n.y < ((hmin + hmax) / 2))
					ne.push_back(n);
				else
					se.push_back(n);
			}
		}
		
		*T = (QuadTree)malloc(sizeof(QuadNode));
		(*T)->quality = -1;
		(*T)->index= -1;
		(*T)->x = -1;
		(*T)->y = -1;
		(*T)->nechild = NULL;
		(*T)->nwchild = NULL;
		(*T)->swchild = NULL;
		(*T)->sechild = NULL;
		
		CreateTree(&((*T)->nwchild), wmin, hmin, (wmin + wmax) / 2, (hmin + hmax) / 2, nw);
		CreateTree(&(*T)->nechild, (wmin + wmax) / 2, hmin, wmax, (hmin + hmax) / 2, ne);
		CreateTree(&(*T)->swchild, wmin, (hmin + hmax) / 2, (wmin + wmax) / 2, hmax, sw);
		CreateTree(&(*T)->sechild, (wmin + wmax) / 2, (hmin + hmax) / 2, wmax, hmax, se);
	}
	else if (nodes.size() == 1) {
		*T = (QuadTree)malloc(sizeof(QuadNode));
		for (auto n : nodes)
		{
			(*T)->quality = n.quality;
			(*T)->index = n.index;
			(*T)->x = n.x;
			(*T)->y = n.y;
			(*T)->nechild = NULL;
			(*T)->nwchild = NULL;
			(*T)->swchild = NULL;
			(*T)->sechild = NULL;
		}
	}
	else {
		*T = NULL;
	}
}