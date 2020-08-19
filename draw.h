#include <iostream>
#include <windows.h>
#include "functions.h"

//screen dimensions

#define WIDTH 800
#define HEIGHT 608

//WIDTH and height of each character in pixels
const int dW=4,dH=8;

//set cursor at start to avoid flickering
void gotoxy ( short x, short y )
{
COORD coord = {x, y};
SetConsoleCursorPosition ( GetStdHandle ( STD_OUTPUT_HANDLE ), coord );
}
char palette[]=".,-~:;=!*#OB8%&";
//declaration
void plotLineLow(char platno[HEIGHT/dH][WIDTH/dW],int x0,int y0,int x1,int y1,char c);
void plotLineHigh(char platno[HEIGHT/dH][WIDTH/dW],int x0,int y0,int x1,int y1,char c);
void drawPoint(char platno[HEIGHT/dH][WIDTH/dW],int A,int B,char c);
void drawLine(char platno[HEIGHT/dH][WIDTH/dW],int A,int B,int C,int D,char c);
void drawTriangleInterior(char platno[HEIGHT/dH][WIDTH/dW],int A[2],int B[2],int C[2],char c);
void drawTriangle(char platno[HEIGHT/dH][WIDTH/dW],int A[2],int B[2],int C[2],char c1,char c2,char c3);

//functions

void drawPoint(char platno[HEIGHT/dH][WIDTH/dW],int A,int B,char c)
{
	if(A<0||B<0||A>=WIDTH/dW||B>=HEIGHT/dH)	return;
	platno[B][A]=c;
}

void drawLine(char platno[HEIGHT/dH][WIDTH/dW],int A,int B,int C,int D,char c)
{
	//Bresenham's line algorithm
	
	//sort points so the first one is to the left
	if(A>C)
	{
		int t;
		t=A;
		A=C;
		C=t;
		t=B;
		B=D;
		D=t;
	}
	
	//in case points are outside the screen
	if(C<0||(B<0&&D<0)||A>=WIDTH/dW||(B>=HEIGHT/dH&&D>=HEIGHT/dH))	return;
	if(A<0)	B-=A*D/C;
	if(C>=WIDTH/dW)	D-=(C-WIDTH/dW+1)*B/A;
	if(B<0)	A-=B*C/D;
	if(D<0)	C-=D*A/B;
	if(B>HEIGHT/dH)	A-=(B-HEIGHT/dH+1)*C/B;
	if(D>HEIGHT/dH)	A-=(D-HEIGHT/dH+1)*A/B;
	
	if(B==D)
	{
		for(int i=A;i<=C;i++)
			platno[B][i]=c;
		return;
	}
	if(A==C)
	{
		int min=B,max=D;
		if(D<B)
		{
			min=D;
			max=B;
		}
		for(int i=min;i<=max;i++)
			platno[i][A]=c;
		return;
	}
	if(abs(D-B)<abs(C-A))
		plotLineLow(platno,A,B,C,D,c);
	else
	{
		if(B>D)	plotLineHigh(platno,C,D,A,B,c);
		else	plotLineHigh(platno,A,B,C,D,c);
	}
}

void plotLineLow(char platno[HEIGHT/dH][WIDTH/dW],int x0,int y0,int x1,int y1,char c)
{
    int dx = x1 - x0, dy = y1 - y0, yi = 1;
    if (dy < 0)
    {
        yi = -1;
        dy = -dy;
	}
    int D = 2*dy - dx;
    int y = y0;

    for (int x=x0;x<=x1;x++)
    {
        platno[y][x]=c;
        if( D > 0)
        {
            y += yi;
            D -= 2*dx;
		}
        D += 2*dy;
	}
}

void plotLineHigh(char platno[HEIGHT/dH][WIDTH/dW],int x0,int y0,int x1,int y1,char c)
{
    int dx = x1 - x0, dy = y1 - y0, xi = 1;
    if (dx < 0)
    {
        xi = -1;
        dx = -dx;
	}
    int D = 2*dx - dy;
    int x = x0;

    for (int y=y0;y<=y1;y++)
    {
        platno[y][x]=c;
        if( D > 0)
        {
            x += xi;
            D -= 2*dy;
		}
        D += 2*dx;
	}
}

void drawTriangleInterior(char platno[HEIGHT/dH][WIDTH/dW],int A[2],int B[2],int C[2],char c)
{
	for(int i=0;i<WIDTH/dW;i++){
	for(int j=0;j<WIDTH/dW;j++){
		double p[2]={(double)i+0.5,(double)j+0.5};
		if(inTriangle(p,A,B,C))	drawPoint(platno,i,j,c);
	}}
}

void drawTriangle(char platno[HEIGHT/dH][WIDTH/dW],int A[2],int B[2],int C[2],char c1,char c2,char c3)
{
	drawTriangleInterior(platno,A,B,C,c3);
	drawLine(platno,A[0],A[1],B[0],B[1],c2);
	drawLine(platno,A[0],A[1],C[0],C[1],c2);
	drawLine(platno,C[0],C[1],B[0],B[1],c2);
	drawPoint(platno,A[0],A[1],c1);
	drawPoint(platno,B[0],B[1],c1);
	drawPoint(platno,C[0],C[1],c1);
}
