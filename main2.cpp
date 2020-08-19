#include <iostream>
#include <cstring>
#include <unistd.h>
#include <windows.h>
#include "draw.h"
#define PI 3.14159265358979323846
using namespace std;

class tetrahedron
{
public:
	double 
	x[4]={-1,1,0,0},
	y[4]={0,0,-1,1},
	z[4]={-1/sqrt(2),-1/sqrt(2),1/sqrt(2),1/sqrt(2)};
	
	tetrahedron(double tx,double ty,double tz,double a)
	{
		for(int i=0;i<4;i++)
		{
			x[i]=a*x[i]/sqrt(2)+tx;
			y[i]=a*y[i]/sqrt(2)+ty;
			z[i]=a*z[i]/sqrt(2)+tz;
		}
	}
};

class camera
{
public:
	double x,y,z;
	double matrix[16],inv[16];
	camera(double r,double alfa,double beta)
	{
		//alfa is camera's angle along the xy plane.
		//beta is camera's angle along z axis
		//r is the distance from the camera to the origin
		double a=sin(alfa),b=cos(alfa),c=sin(beta),d=cos(beta);
		x=r*b*d;
		y=r*a*d;
		z=r*c;
		
		//matrix
		matrix[3]=matrix[7]=matrix[11]=0;
		matrix[15]=1;
		//x
		matrix[0]=a;
		matrix[1]=-b;
		matrix[2]=0;
		//y
		matrix[4]=b*c;
		matrix[5]=a*c;
		matrix[6]=-d;
		//z
		matrix[8]=b*d;
		matrix[9]=a*d;
		matrix[10]=c;
		
		matrix[12]=x;
		matrix[13]=y;
		matrix[14]=z;
		
		//invert
		invert(inv,matrix);
	}
	int convert(int cooInt[2],double tx,double ty,double tz)
	{
		//converts from world to pixel coordinates
		//returns 0 if the point is invisible to the camera
		double vec[3]={tx,ty,tz};
		transformVector(vec,inv);
		if(vec[2]>0)	return 0;
		double xI=-vec[0]/vec[2];
		double yI=-vec[1]/vec[2];
		xI*=WIDTH/dW/2;
		yI*=WIDTH/dH/2;
		xI+=WIDTH/dW/2;
		yI+=HEIGHT/dH/2;
		int A=(int)xI,B=(int)yI;
		cooInt[0]=A;
		cooInt[1]=B;
		return 1;
	}
	//rendering
	
	
	
	void renderPoint(char platno[HEIGHT/dH][WIDTH/dW],double tx,double ty,double tz,char c)
	{
		int vec[2];
		convert(vec,tx,ty,tz);
		drawPoint(platno,vec[0],vec[1],c);
	}
	
	void renderLine(char platno[HEIGHT/dH][WIDTH/dW],double tx1,double ty1,double tz1,double tx2,double ty2,double tz2,char c)
	{
		int vec1[2],vec2[2];
		convert(vec1,tx1,ty1,tz1);
		convert(vec2,tx2,ty2,tz2);
		
		drawLine(platno,vec1[0],vec1[1],vec2[0],vec2[1],c);
	}
	
	void renderTriangle(char platno[HEIGHT/dH][WIDTH/dW],double Ax,double Ay,double Az,double Bx,double By,double Bz,double Cx,double Cy,double Cz)
	{
		double
		A[3]={Ax,Ay,Az},	//vertices
		B[3]={Bx,By,Bz},
		C[3]={Cx,Cy,Cz},
		L[3]={0,4,-4};	//light source
		double n[3];	//normal vector
		normal(n,A,B,C);
		//I'm not sure why, but the program doesn't work unless I flip the normal vector
		n[0]*=-1;
		n[1]*=-1;
		n[2]*=-1;
		
		int p0[2],p1[2],p2[2];
		if(!convert(p0,Ax,Ay,Az))	return;
		if(!convert(p1,Bx,By,Bz))	return;
		if(!convert(p2,Cx,Cy,Cz))	return;
		//shoot the ray through every pixel
		for(int xi=0;xi<WIDTH/dW;xi++){
		for(int yi=0;yi<HEIGHT/dH;yi++){
			double pi[2]={(double)xi+0.5,(double)yi+0.5};
			//if the ray doesn't hit the triangle then continue
			if(!inTriangle(pi,p0,p1,p2))	continue;
			//point is the vector in camera's coordinate system
			double point[3]=
			{
			-(double)(xi-WIDTH/dW/2)/(double)(WIDTH/dW/2),
			-(double)(yi-HEIGHT/dH/2)/(double)(WIDTH/dH/2),
			1
			};
			//we transform it so it is in global coordinate system
			transformVector(point,matrix);
			//intersection between the triangle and the ray
			double 
			intersection[3],
			coordinates[3]={x,y,z};//coordinates of the camera
			rayPlane(intersection,A,n,coordinates,point);
			double l[3];	//unit vector pointing from intersection to light source
			vector(l,L,intersection);
			normalize(l);
			double luminance=clamp(dot(n,l),0,1);
			int color=(int)(luminance*(double)(strlen(palette)-1));
			drawPoint(platno,xi,yi,palette[color]);
		}}
	}
	
	void renderTetra(char platno[HEIGHT/dH][WIDTH/dW],tetrahedron tetra,char c1,char c2)
	{
		//search Painter's Algorithm for info
		
		int index[4]={0,1,2,3};
		double koo[4][3]=
		{
			{tetra.x[0],tetra.y[0],tetra.z[0]},
			{tetra.x[1],tetra.y[1],tetra.z[1]},
			{tetra.x[2],tetra.y[2],tetra.z[2]},
			{tetra.x[3],tetra.y[3],tetra.z[3]}
		};
		transformVector(koo[0],inv);
		transformVector(koo[1],inv);
		transformVector(koo[2],inv);
		transformVector(koo[3],inv);
		double depth[4]=
		{
			(koo[0][2]+koo[1][2]+koo[2][2])/3,
			(koo[0][2]+koo[1][2]+koo[3][2])/3,
			(koo[0][2]+koo[2][2]+koo[3][2])/3,
			(koo[1][2]+koo[2][2]+koo[3][2])/3
		};
		
		//sort by depth
		for(int i=3;i>0;i--)
		{
			for(int j=0;j<i;j++)
			{
				if(depth[j]<depth[j+1])
				{
					int t=index[j];
					index[j]=index[j+1];
					index[j+1]=t;
					double temp=depth[j];
					depth[j]=depth[j+1];
					depth[j+1]=temp;
				}
			}
		}
		int point[4][2];
		for(int i=0;i<4;i++)
		{
			if(!convert(point[i],tetra.x[i],tetra.y[i],tetra.z[i]))	return;
		}
		
		int faces[4][3]=
		{
			{0,1,2},
			{0,1,3},
			{0,2,3},
			{1,2,3}
		};
		
		//commented part is without shading
		
		
		for(int i=0;i<4;i++)
			renderTriangle(platno,tetra.x[faces[index[i]][0]],tetra.y[faces[index[i]][0]],tetra.z[faces[index[i]][0]],tetra.x[faces[index[i]][1]],tetra.y[faces[index[i]][1]],tetra.z[faces[index[i]][1]],tetra.x[faces[index[i]][2]],tetra.y[faces[index[i]][2]],tetra.z[faces[index[i]][2]]);	
		
		/*
		for(int i=0;i<4;i++)
			drawTriangle(platno,point[faces[index[i]][0]],point[faces[index[i]][1]],point[faces[index[i]][2]],c1,c2,' ');
		*/
	}

};





int main()
{
    tetrahedron tetra(0,0,0,1);
    //camera angles
	double alfa=PI/3,beta=0;
	//drawing loop:
	while(1)
	{
		camera cam(2,alfa,beta);
		char platno[HEIGHT/dH][WIDTH/dW];
		
		for(int i=0;i<HEIGHT/dH;i++){
		for(int j=0;j<WIDTH/dW;j++){
			platno[i][j]=0;
		}}
		
		cam.renderTetra(platno,tetra,'@',',');
		
		//display:
		for(int i=0;i<HEIGHT/dH;i++){
		for(int j=0;j<WIDTH/dW;j++){
			printf("%c",platno[i][j]);
		}
		printf("\n");
		}
		
		//instead of system("cls") i used this because it looks smoother
		gotoxy(0,0);
		
		//update camera position
		alfa+=0.01*PI;
		beta+=0.02*PI;
	}
	return 0;
}
