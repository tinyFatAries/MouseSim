#include "../BaseApplication.h"
#include <iostream>
#include <fstream>
using namespace std;
using namespace Ogre;

class Maze
{
public:
	Maze(SceneManager* sceneManager);
	~Maze();

	void createPlane();
	/*create Maze from file .txt .png .jpg .bmp*/
	bool createFromFile(const string& filename, const string& suffix = "");
	void saveMaze(const string& filename, const string& suffix = "");
	void printMaze(bool saveToFile=false);

	/*build or destroyWall a wall in coordinate x,y*/
	void buildWall(int x, int y, bool rotate = false);
	void destroyWall(int x, int y);
	void buildPillar(int x, int y);

private:
	enum Limit{xNumber=17, yNumber=16};

	bool MazeX[xNumber][yNumber];
	bool MazeY[xNumber][yNumber];

	MovablePlane* mPlane;
	Entity* mPlaneEnt;
	SceneNode* mPlaneNode;
	SceneManager* mSceneManager;
};
