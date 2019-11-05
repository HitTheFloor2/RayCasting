// RayCasting.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <GL/glut.h>
#include <vector>
#include <iostream>
using namespace std;

#define SIDELEN 300		//视面1000*1000个像素
float Image[SIDELEN * SIDELEN * 4];
float EDGE = 2.0;		//包围盒边长
float Radius = 0.9;
float vlight = 1.0;		//默认一个体素的发光
float valpha = 0.9;		//默认一个体素的透光率
float STEP = 0.05;

class Position {
public:
	float x;
	float y;
	float z;
	float value;
	Position(float x1, float y1, float z1) {
		x = x1;
		y = y1;
		z = z1;
		value = 0.0;
	}
	Position() {
		x = 0.0;
		y = 0.0;
		z = 0.0;
		value = 0.0;
	}
	void show() {
		cout << x << " " << y << " " << z << endl;
	}
};

float getDistance(Position p1, Position p2) {
	return sqrt((p1.x - p2.x) * (p1.x - p2.x) +
		(p1.y - p2.y) * (p1.y - p2.y) +
		(p1.z - p2.z) * (p1.z - p2.z));
}
float getDistance(float pX, float pY, float pZ,
	float lineBackX, float lineBackY, float lineBackZ,
	float lineFrontX, float lineFrontY, float lineFrontZ)
{
	double sa, sb, sc, cosA, sinA, H;
	sa = sqrt((pX - lineBackX) * (pX - lineBackX) + (pY - lineBackY) * (pY - lineBackY)
		+ (pZ - lineBackZ) * (pZ - lineBackZ));
	sb = sqrt((lineBackX - lineFrontX) * (lineBackX - lineFrontX)
		+ (lineBackY - lineFrontY) * (lineBackY - lineFrontY)
		+ (lineBackZ - lineFrontZ) * (lineBackZ - lineFrontZ));
	sc = sqrt((pX - lineFrontX) * (pX - lineFrontX) + (pY - lineFrontY) * (pY - lineFrontY)
		+ (pZ - lineFrontZ) * (pZ - lineFrontZ));
	cosA = (sb * sb + sc * sc - sa * sa) / (2.0 * sb * sc);
	sinA = sqrt(1.0 - cosA * cosA);
	H = sc * sinA;
	return H;
}

float getDistance(Position p, Position start, Position end) {
	return getDistance(p.x, p.y, p.z, end.x, end.y, end.z, start.x, start.y, start.z);
}



Position canvaPos2Postion(int x, int y) {
	//将成像面上的（x,y）转换成三维空间的位置
	Position p = Position();
	return p;
}
vector<Position> rayQueunInsert(Position p, vector<Position> rayQueue) {
	//以Position的value大小升序
	if (rayQueue.size() == 0) {
		rayQueue.push_back(p);
		return rayQueue;
	}
	else {
		vector<Position>::iterator it = rayQueue.begin();
		for (; it != rayQueue.end(); ++it) {
			if (it->value > p.value) {
				rayQueue.insert(it, p);
				return rayQueue;
			}
		}
		rayQueue.push_back(p);
		return rayQueue;
	}
}
float raycasting(Position start, Position end) {
	//int a;
	//cout << getDistance(Position(0, 0, 0.5), Position(0, 0, 0), Position(0, 1, 0)) << endl;
	//cin >> a;
	//在体元集合中找到所有和start-end射线相交的体元，计算累计的光照强度
	//将找到的体元按照距离视点的远近从小到大加入队列
	//遍历队列，每一个元素在的光照强度会被队列之前的元素减弱光强
	//由于每一个元素（体元）的不透明度是一样的，可以简化为以当前元素偏移值
	//从底部开始找 i-x j-y k-z
	Position bboxStart = Position(-1.0, -1.0, -1.0);
	vector<Position> rayQueue;
	float res = 0.0;
	bool hasLightVoxel = false;
	//cout << "line-------------------------" << endl;
	//start.show(); end.show();
	for (int i = 0; bboxStart.x + i * STEP < 1.0; i++) {
		for (int j = 0; bboxStart.y + j * STEP < 1.0; j++) {
			for (int k = 0; bboxStart.z + k * STEP < 1.0; k++) {
				//当前体元中心点
				Position center = Position(((float)(i + 0.5)) * STEP + bboxStart.x,
					((float)(j + 0.5)) * STEP + bboxStart.y,
					((float)(k + 0.5)) * STEP + bboxStart.z);

				//center.show();
				//当中心点和射线的距离小于体元边长的0.5倍，则认为相交
				float dis = getDistance(center, start, end);
				//cout << "dis=" << dis << endl;
				//center.show();

				if (dis <= STEP) {
					//cout << "find a pos" << endl;
					//center.show();
					center.value = dis;
					rayQueue = rayQueunInsert(center, rayQueue);
					if (hasLightVoxel == false) {
						if (fabs(getDistance(center, Position(0.0, 0.0, 0.0)) - Radius) <= 2 * STEP) {
							cout << "find a light" << endl;
							//center.show();
							hasLightVoxel = true;
						}
					}
				}
			}
		}
	}
	if (!hasLightVoxel) {
		//无发光体元
		return 0.0;
	}
	//检查当前的rayQueue，计算这条射线产生的光照
	for (int i = 0; i < rayQueue.size(); i++) {
		//判断是否是球体上的体元
		Position ballcenter = Position(0.0, 0.0, 0.0);
		if (fabs(getDistance(rayQueue[i], ballcenter) - Radius) <= 2 * STEP) {
			res += pow(valpha, i + 1) * vlight;
		}
	}

	return res >= 1.0 ? 1.0 : res;

}



void Draw_RC() {

	/*
	*		^ y
	*		|
	*		---->
	*
	*
	*/
	//包围盒的起点

	//当前的球在(0,0,0)位置
	//正方形成像面，中心点在(0,0,2),大小为WIDGH*HEIGHT
	//视点位置为(0,0,5)
	//对Image上的每一个像素，视为从视点到画布对应位置的
	int a;
	float* temp = Image;

	Position eye = Position(0.0, 0.0, 5.0);
	Position panelStart = Position(-0.25, -0.25, 2.0);
	float panel_length = (-1.0) * 2.0 * panelStart.x;

	float step = panel_length / SIDELEN;
	float val = 0.0;
	Position p = Position();
	p.z = 4.0;

	//raycasting(eye, Position(0, 0, 0));
	//cin >> a;

	for (int j = 0; j < SIDELEN; j++)//逐个合成像素值
	{
		p.x = j * step + panelStart.x;
		for (int i = 0; i < SIDELEN; i++)
		{
			p.y = i * step + panelStart.y;
			//对视图上一个像素进行RC算法,射线由eye和p决定
			val = raycasting(eye, p);
			if (val > 0.0) {
				cout << "x = " << p.x << ", y = " << p.y << ", val = " << val << endl;
			}
			temp[0] = val;
			temp[1] = val;
			temp[2] = val;
			temp[3] = val;
			temp += 4;
		}
	}
}

//显示函数
void Mydisplay()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawPixels(SIDELEN, SIDELEN, GL_RGBA, GL_FLOAT, Image);//使用OpenGL的绘图函数
	glFlush();
}
void DrawRed() {
	float* LinePS = Image;
	int x = 500, y = 500;
	for (int j = 0; j < SIDELEN; j++)//逐个合成像素值
	{
		for (int i = 0; i < SIDELEN; i++)
		{
			//Composite(LinePS, i, j, CData, Dim, R, T);
			if ((i - x) * (i - x) + (j - y) * (j - y) > 90000) {
				LinePS[0] = 1.0;
				LinePS[1] = 1.0;
				LinePS[2] = 1.0;
				LinePS[3] = 1.0;
				LinePS += 4;
			}
			else {
				LinePS[0] = 0.4;
				LinePS[1] = 0.4;
				LinePS[2] = 0.4;
				LinePS[3] = 1.0;
				LinePS += 4;
			}

		}
	}
}

int main(int argc, char** argv)
{
	//DrawRed();
	Draw_RC();
	//使用OpenGL显示此二维图像
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowSize(SIDELEN, SIDELEN);
	glutInitWindowPosition(200, 200);
	glutCreateWindow("Ray-Casting");
	glClearColor(0, 0, 0, 1);//背景设为黑色
	glutDisplayFunc(Mydisplay);//显示图像
	glutMainLoop();
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
