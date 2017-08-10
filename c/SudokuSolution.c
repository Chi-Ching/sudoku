/***************************************************************************
 *   Copyright (C) 2005 by Thomas Shek                                     *
 *   chiching@localhost                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define GROUP(row,column) (row/3*3+column/3)

int map[9][9];
int p[9][9];
int solvedrow[9][9];
int solvedcolumn[9][9];
int solvedgroup[9][9];
int pass;
int pass1,pass2;

int bitcount[512];

void printbit(int a)
{
	int i;
	for (i=0;i<9;i++) printf("%c",(a>>(8-i))%2==1?'1'+8-i:'.');
}

void printbitn(int a)
{
	int i;
	for (i=0;i<9;i++) printf("%c",(a>>(8-i))%2==1?'1'+8-i:'.');
	printf("\n");
}

void printp()
{
	int i,j,k;
	for (i=0;i<9;i++)
	{
		for (j=0;j<9;j++)
		{
			if (p[i][j]==0)
			{
				for (k=0;k<9;k++) printf("%d",map[i][j]+1);
//				printf("xxxxxxxxx");
			}
			else printbit(p[i][j]);
			printf(" ");
		}
		printf("\n");
	}
}

void printmap()
{
	int i,j;
	for (i=0;i<9;i++)
	{
		for (j=0;j<9;j++) printf("%c ",map[i][j]==-1?'.':map[i][j]+'1');
		printf("\n");
	}

}

int getProgress()
{
	int i,j;
	int c;
	c=0;
	for (i=0;i<9;i++) for (j=0;j<9;j++) c += p[i][j];
	return c;
}



void clearlocation(int a, int row, int column)
{
	int i;
	int x,y;
	int bitmask;

	bitmask=~(1<<a);
	for (i=0;i<9;i++)
	{
		p[row][i]&=bitmask;
		p[i][column]&=bitmask;
	}
	y=row/3*3;
	x=column/3*3;
	p[y][x]&=bitmask;
	p[y+1][x]&=bitmask;
	p[y+2][x]&=bitmask;
	p[y][x+1]&=bitmask;
	p[y+1][x+1]&=bitmask;
	p[y+2][x+1]&=bitmask;
	p[y][x+2]&=bitmask;
	p[y+1][x+2]&=bitmask;
	p[y+2][x+2]&=bitmask;
}

void registersolved(int a, int row, int column)
{
	map[row][column]=a;
	clearlocation(a,row,column);
	p[row][column]=0;
	solvedrow[row][a]=1;
	solvedcolumn[column][a]=1;
	solvedgroup[GROUP(row,column)][a]=1;

}
void initp()
{
	int i,j;
	for (i=0;i<9;i++) for (j=0;j<9;j++)
	{
		p[i][j]=511;
		solvedrow[i][j]=solvedcolumn[i][j]=solvedgroup[i][j]=0;
	}
	for (i=0;i<9;i++) for (j=0;j<9;j++)
	{
		if (map[i][j]!=-1)
		{
			clearlocation(map[i][j],i,j);
			p[i][j]=0;
			registersolved(map[i][j],i,j);
		}
	}

}

void checkp()
{
	int i,j,k;
	int t;

	for (i=0;i<9;i++) for (j=0;j<9;j++) if (p[i][j]!=0)
	{
		t=1;
		for (k=0;k<9;k++)
		{
			if (p[i][j]==t)
			{
				registersolved(k,i,j);
				break;
			}
			t<<=1;
		}
	}
}

void checkrow()
{
	int i,j,k;
	int found;

	for (i=0;i<9;i++)
	{
		for (k=0;k<9;k++)
		{
			if (solvedrow[i][k]) continue;
			found=0;
			for (j=0;j<9;j++) found+=(p[i][j]>>k)%2;
			if (found==1)
			{
				for (j=0;j<9;j++) if ((p[i][j]>>k)%2) break;
				registersolved(k,i,j);
			}
		}
	}
}

void checkcolumn()
{
	int i,j,k;
	int found;

	for (j=0;j<9;j++)
	{
		for (k=0;k<9;k++)
		{
			if (solvedcolumn[j][k]) continue;
			found=0;
			for (i=0;i<9;i++) found+=(p[i][j]>>k)%2;
			if (found==1)
			{
				for (i=0;i<9;i++) if ((p[i][j]>>k)%2) break;
				registersolved(k,i,j);
			}
		}
	}
}

void checkgroup()
{
	int i,j,k,g,r;
	int found;

	for (g=0;g<9;g++)
	{
		i=g/3*3;
		j=g%3*3;
		for (k=0;k<9;k++)
		{
			if (solvedgroup[g][k]) continue;
			found=0;
			for (r=0;r<9;r++) found+=(p[i+r/3][j+r%3]>>k)%2;
			if (found==1)
			{
				for (r=0;r<9;r++) if ((p[i+r/3][j+r%3]>>k)%2) break;
				registersolved(k,i+r/3,j+r%3);
			}
		}
	}
}

int LevelOneLogic()
{
	pass1++;
	checkp();
	checkrow();
	checkcolumn();
	checkgroup();

	return 0;
}

int LevelTwoLogicCoreRecur(int* list, int x, int pos, int size, int* t, int* simplify, int* mask){
	int i,c,k;
	if (pos ==  size){
		c = 0;
		for (i =0; i< size; i++){
			if (t[i]) c++;
		}
		if (c==0 || c==size){

			return 0;
		}
		if (c == bitcount[x]){
			k=0;
			for (i=0; i<size;i++){
				if ((!t[i])&& (list[i] & x)){
					// found!
					simplify[k++] = i;
				}
			}
			if (k){
				*mask = x;
				return 1;
			}
		}
	}
	else{

		t[pos] = 0;
		if (LevelTwoLogicCoreRecur(list, x, pos + 1, size, t, simplify, mask)) return 1;
		x |= list[pos];
		t[pos] = 1;
		if (LevelTwoLogicCoreRecur(list, x, pos + 1, size, t, simplify, mask)) return 1;
	}
	return 0;
}

int LevelTwoLogicCore(int* list, int size, int* simplify, int* mask){

	int t[9];
	return LevelTwoLogicCoreRecur(list,0,0,size,t,simplify, mask);
}

int checkLevelTwoLogicColumn(){
	int i;
	int list[9];
	int listp[9];
	int size;
	int simplify[9];
	int mask;
	int a;

	for (a =0 ; a<9 ; a++){
		mask = size = 0;
		for (i=0;i<9;i++){
			list[i] = listp[i] = 0;
			simplify[i] = -1;
		}
		for (i=0;i<9;i++){
			if (p[i][a]!=0){
				list[size] = p[i][a];
				listp[size] = i;
				size++;
			}
		}

		if (size == 0 || size == 9) continue;

		if (LevelTwoLogicCore(list, size, simplify, &mask)){
			for (i=0;i<size; i++){
				if (simplify[i] == -1) break;
				p[listp[simplify[i]]][a] &= ~mask;
			}
			return 1;
		}
	}

	return 0;

}

int checkLevelTwoLogicRow(){
	int i;
	int list[9];
	int listp[9];
	int size;
	int simplify[9];
	int mask;
	int a;

	for (a =0 ; a<9 ; a++){
		mask = size = 0;
		for (i=0;i<9;i++){
			list[i] = listp[i] = 0;
			simplify[i] = -1;
		}
		for (i=0;i<9;i++){
			if (p[a][i]!=0){
				list[size] = p[a][i];
				listp[size] = i;
				size++;
			}
		}

		if (size == 0 || size == 9) continue;

		if (LevelTwoLogicCore(list, size, simplify, &mask)){
			for (i=0;i<size; i++){
				if (simplify[i] == -1) break;
				p[a][listp[simplify[i]]] &= ~mask;
			}
			return 1;
		}
	}

	return 0;

}

int checkLevelTwoLogicGroup(){
	int i;
	int list[9];
	int listp[9];
	int size;
	int simplify[9];
	int mask;
	int a;

	for (a =0 ; a<9 ; a++){
		mask = size = 0;
		for (i=0;i<9;i++){
			list[i] = listp[i] = 0;
			simplify[i] = -1;
		}
		for (i=0;i<9;i++){
			if (p[a/3*3 + i/3][a%3*3 + i%3]!=0){
				list[size] = p[a/3*3 + i/3][a%3*3 + i%3];
				listp[size] = i;
				size++;
			}
		}

		if (size == 0 || size == 9) continue;

		if (LevelTwoLogicCore(list, size, simplify, &mask)){
			for (i=0;i<size; i++){
				if (simplify[i] == -1) break;
				p[a/3*3 + listp[simplify[i]]/3][a%3*3 + listp[simplify[i]]%3] &= ~mask;
			}
			return 1;
		}
	}

	return 0;

}

// Return 1 : have some progress
// Return 0 : no progress
int LevelTwoLogic()
{
	pass2++;
	if (checkLevelTwoLogicColumn()) return 1;
	if (checkLevelTwoLogicRow()) return 1;
	if (checkLevelTwoLogicGroup()) return 1;

	return 0;
}

int solve()
{
	int last;
	int t;

	initp();
	last=getProgress();
	pass=0;
	pass1 = pass2 = 0;
	while (1)
	{
		while (1)
		{
			pass++;
			LevelOneLogic();
			t=getProgress();
			if (t==last) break;
			if (t==0) return 1;
			last=t;
		}
		if (!LevelTwoLogic()) break;
	}
	return 0;
}

void globalinit(){
	int i,t;
	for (i=0; i< 512; i++){
		t = i;
		bitcount[i] = 0;
		while (t != 0){
			if (t%2 == 1) bitcount[i]++;
			t/=2;
		}
	}
}

int main(int argc, char* argv[])
{
	int i,j;
	int c,cc;
	char str[50];
	int r;

#ifndef ONLINE_JUDGE
	close (0); open("in.txt", O_RDONLY);
//	close (1); open("out.txt", O_WRONLY | O_CREAT, 0600);
#endif

	globalinit();

	scanf("%d\n",&cc);
	for(c=0;c<cc;c++)
	{
		for (i=0;i<9;i++)
		{
			gets(str);
			for (j=0;j<9;j++) map[i][j]=str[j]-'0'-1;
		}
		gets(str);
		r = solve();
		if (r){
			printf("SUCCESS : %d (%d, %d)\n",pass, pass1, pass2);
			printmap();
			printf("\n");
		}
		else{
			printf("Failed in some point : %d (%d, %d)\n",pass, pass1, pass2);
			printmap();
			printf("\n");
			printp();
			printf("\n");
		}
	}
	return 0;
}
