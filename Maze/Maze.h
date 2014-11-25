#include "../BaseApplication.h"
#include <iostream>
#include <fstream>
#include <list>
using namespace std;
using namespace Ogre;
using namespace OgreBites;

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

	/*mouse event*/
	void injectMousePressed(const OIS::MouseEvent &arg, enum OIS::MouseButtonID id,\
			SdkTrayManager* mTrayMgr, SceneManager* mSceneMgr, Camera* mCamera);

	/*frame listener*/
	void frameRenderingQueued(const FrameEvent& evt);

private:
	enum Limit{xNumber=17, yNumber=16};
	struct coor {
		int x, y;
		coor(int _x, int _y) : x(_x), y(_y) {}
	};

	bool MazeX[xNumber][yNumber];
	bool MazeY[xNumber][yNumber];

	MovablePlane* mPlane;
	Entity* mPlaneEnt;
	SceneNode* mPlaneNode;
	SceneManager* mSceneManager;

	bool mHideWall;
	std::vector<pair<string, float> > *mWallStatus;
};
