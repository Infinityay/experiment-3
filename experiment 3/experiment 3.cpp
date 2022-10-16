#include <graphics.h>
#include <conio.h>
#include <string>
#include <math.h>
#include <iostream>
#include<vector>
using namespace std;
void FloodFill4(int x, int y, COLORREF oldcolor, COLORREF newcolor);
//声明画直线函数
void DDALine(int x0, int x1, int y0, int y1, COLORREF color);
// 多边形顶点结构
struct point
{
	int x, y;
	point(int x1, int y1) {
		x = x1;
		y = y1;
	}
};

// 新边表 和活动边表的结构
typedef struct XET {
	double x, dx, ymax;
	XET* next;
}AET, NET;

//声明多边形扫描线算法与种子填充法
// 传入顶点序列与多边形颜色color
void polygonSwipeLine(vector<point> vertices, COLORREF color);


int main() {


	//创建窗口
	initgraph(800, 600, EW_SHOWCONSOLE);
	//设置背景颜色为白色
	setbkcolor(WHITE);
	//用背景色清空屏幕
	cleardevice();

	ExMessage m;		// 定义消息变量
	// 起点终点
	int x = 0, y = 0;
	vector<point> vertices;
	int clickTime = 0, type = 0;
	COLORREF color = RED;
	cout << "------------您处于多边形扫描转换算法中----------" << endl;
	while (true)
	{

		// 获取一条鼠标或按键消息
		m = getmessage(EX_MOUSE | EX_KEY);

		switch (m.message)
		{


		case WM_LBUTTONDOWN:
			clickTime++;
			x = m.x;
			y = m.y;
			vertices.push_back(point(x, y));
			cout << "第" << clickTime << "个顶点：(" << x << "," << y << ")" << endl;
			break;
		case WM_RBUTTONDOWN:
		{
			int i;
			// 用DDA右键连接点
			for (i = 0; i < vertices.size() - 1; i++)
			{
				DDALine(vertices[i].x, vertices[i + 1].x, vertices[i].y, vertices[i + 1].y, color);
			}
			DDALine(vertices[i].x, vertices[0].x, vertices[i].y, vertices[0].y, color);

		}
		case WM_KEYDOWN:
			//按ctrl切换算法 
			if (m.vkcode == VK_CONTROL) {
				type++;
				type = type % 2;
				if (type == 0)
				{
					color = RED;
					clickTime = 0;
					vertices.clear();
					cout << "------------您处于多边形扫描转换算法中----------" << endl;
				}
				else if (type == 1)
				{
					color = GREEN;
					clickTime = 0;
					vertices.clear();
					cout << "------------您处于种子区域填充算法中----------" << endl;
				}
			}

			if (m.vkcode == VK_SPACE)
			{
				if (type == 0)
				{
					cout << "---------开始多边形扫描转换---------" << endl;
					polygonSwipeLine(vertices, color);// 按空格键开始画图
					clickTime = 0;
					vertices.clear();
					cout << "---------多边形扫描转换成功！---------" << endl;
				}
				else if (type == 1)
				{
					FloodFill4(vertices[0].x , vertices[0].y , WHITE, BLUE);
				}

			}
			if (m.vkcode == VK_ESCAPE)
				return 0;	// 按 ESC 键退出程序
		}
	}

	// 按任意键退出
	_getch();
	closegraph();
	return 0;
}



// 实现多边形扫描线算法
void polygonSwipeLine(vector<point> vertices, COLORREF color) {

	//得到扫描线范围
	int line_Ymax = vertices[0].y, line_Ymin = vertices[0].y;
	for (int i = 1; i < vertices.size(); i++)
	{
		if (vertices[i].y > line_Ymax)
		{
			line_Ymax = vertices[i].y;
		}
		if (vertices[i].y < line_Ymin) {
			line_Ymin = vertices[i].y;
		}
	}


	// NET初始化 新编表最多800?/600?个桶（因为我分辨率设的800 * 600） 反正每一条扫描线一个桶
	NET* pNET[800];
	for (int i = line_Ymin; i <= line_Ymax; i++)
	{
		pNET[i] = new NET;
		pNET[i]->next = NULL;
	}
	// AET表初始化
	AET* pAET = new AET;
	pAET->next = NULL;

	//创建NET表
	int count = vertices.size();
	// 每一条扫描线
	for (int i = line_Ymin; i <= line_Ymax; i++)
	{
		//第i条扫描线下 所有点进行处理
		for (int j = 0; j < count; j++)
		{
			// 找到交点
			if (i == vertices[j].y)
			{
				//每一个点最多两条线段放入NET 
				//一个点跟前面的一个点形成一条线段，跟后面的点也形成线段     
				//判断当前点的高低，使ymax、DX、DY的计算有变化


				if (vertices[(j - 1 + count) % count].y > vertices[j].y)
				{
					//一侧
					NET* p = new NET;
					p->x = vertices[j].x;
					p->ymax = vertices[(j - 1 + count) % count].y;//与当前扫描线相交的活性边 的 最高点即为相邻顶点的y
					double DX = vertices[(j - 1 + count) % count].x - vertices[j].x;
					double DY = vertices[(j - 1 + count) % count].y - vertices[j].y;
					p->dx = DX / DY;  //dx为直线斜率的倒数
					// 把p插进新编表
					p->next = pNET[i]->next;
					pNET[i]->next = p;
				}

				if (vertices[(j + 1) % count].y > vertices[j].y)
				{
					//另一侧
					NET* p = new NET;
					p->x = vertices[j].x;
					p->ymax = vertices[(j + 1) % count].y;
					double DX = vertices[(j + 1) % count].x - vertices[j].x;
					double DY = vertices[(j + 1) % count].y - vertices[j].y;
					//dx为直线斜率的倒数
					p->dx = DX / DY;
					p->next = pNET[i]->next;
					pNET[i]->next = p;
				}
			}
		}
	}

	/*建立并更新活性边表AET*/
	for (int i = line_Ymin; i <= line_Ymax; i++) {
		/*更新活性边表AET，计算扫描线与边的新的交点x，此时y值没有达到临界值的话*/
		NET* p = pAET->next;
		while (p) {
			p->x = p->x + p->dx;
			p = p->next;
		}

		/*更新完以后，对活性边表AET按照x值从小到大排序*/
		AET* tq = pAET;
		p = pAET->next;
		tq->next = NULL;
		while (p) {
			while (tq->next && p->x >= tq->next->x)
				tq = tq->next;
			NET* s = p->next;
			p->next = tq->next;
			tq->next = p;
			p = s;
			tq = pAET;
		}

		/*从AET表中删除ymax==i的结点*/
		AET* q = pAET;
		p = q->next;
		while (p) {
			if (p->ymax == i) {
				q->next = p->next;
				delete p;
				p = q->next;
			}
			else {
				q = q->next;
				p = q->next;
			}
		}
		/*将NET中的新点加入AET，并用插入法按X值递增排序*/
		p = pNET[i]->next;
		q = pAET;
		while (p) {
			while (q->next && p->x >= q->next->x)
				q = q->next;
			NET* s = p->next;
			p->next = q->next;
			q->next = p;
			p = s;
			q = pAET;
		}

		/*配对填充颜色*/
		p = pAET->next;
		while (p && p->next) {
			for (int j = p->x; j <= p->next->x; j++) {
				putpixel(j, i, color);
			}
			p = p->next->next;
		}

	}
}




void DDALine(int x0, int x1, int y0, int y1, COLORREF color) {
	int step = 0;
	int dx = x1 - x0, dy = y1 - y0, dx_abs = abs(dx), dy_abs = abs(dy);
	float increx, increy, x, y;
	// 斜率判断
	step = dx_abs >= dy_abs ? dx_abs : dy_abs;
	increx = float(dx) / step;
	increy = float(dy) / step;
	x = float(x0);  y = float(y0);

	for (int i = 0; i <= step; i++) {
		putpixel(int(x + 0.5), int(y + 0.5), color);
		x += increx;
		y += increy;
	}

}

void FloodFill4(int x, int y, COLORREF oldcolor, COLORREF newcolor)
{
	if (getpixel(x, y) == oldcolor)   //属于区域内点oldcolor
	{
		putpixel(x, y, newcolor);
		FloodFill4(x, y + 1, oldcolor, newcolor);
		FloodFill4(x, y - 1, oldcolor, newcolor);
		FloodFill4(x - 1, y, oldcolor, newcolor);
		FloodFill4(x + 1, y, oldcolor, newcolor);
	}
}
