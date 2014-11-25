#include "Maze.h"

Maze::Maze(SceneManager* sceneManager) : mSceneManager(sceneManager)
{
	createPlane();

	mWallStatus = new std::vector<pair<string, float> >;
}

Maze::~Maze()
{
	delete mPlane;
	mPlane = 0;
//	delete mWallStatus;
//	mWallStatus = 0;
}

void Maze::createPlane()
{
	MaterialPtr mat = MaterialManager::getSingleton().create("PlaneMat", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	//////TextureUnitState* tuisTexture =
	mat->getTechnique(0)->getPass(0)->createTextureUnitState("grass_1024.jpg");

	mPlane = new Ogre::MovablePlane("Plane");
	mPlane->d = 10;
	mPlane->normal = Vector3::UNIT_Y;

	MeshManager::getSingleton().createPlane("PlaneMesh", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			*mPlane, 320, 320, 1, 1, true, 1, 1, 1, Vector3::UNIT_Z);
	mPlaneEnt = mSceneManager->createEntity("PlaneEntity", "PlaneMesh");
	mPlaneEnt->setMaterialName("PlaneMat");

	mPlaneNode = mSceneManager->getRootSceneNode()->createChildSceneNode();
	mPlaneNode->attachObject(mPlaneEnt);

	/*initial Maze to full walls*/
	for (int i = 0; i < xNumber; ++i)
		for (int j = 0; j < yNumber; ++j)
		{
			MazeX[i][j] = true;
			MazeY[i][j] = true;
		}
	/*put all the pillars*/
	for (int i = 0; i < xNumber; ++i)
		for (int j = 0; j < xNumber; ++j)
		{
			buildPillar(i, j);
		}
}

/*create Maze from file .txt .png .jpg .bmp*/
bool Maze::createFromFile(const string& filename, const string& suffix)
{
	bool startParse = false;
	string fullname = filename + "." + suffix;
	if (suffix == "txt")
	{
		char word, buf[5];
		int x = 0, y = yNumber;
		buf[4] = '\0';
		ifstream infile;
		infile.open(fullname.data());

		if (infile.fail())
			return false;

		while (1)
		{
			infile.read(&word, sizeof(char));//search for a parse start : "+"
			if (word == '+' && startParse == false) startParse = true;
			if (startParse == false) continue;
		
			for(x = 0; x < yNumber; x++)
			{
				infile.read(buf, sizeof(char)*4);
				if(string(buf) == "---+") MazeX[y][x] = true;
				else MazeX[y][x] = false;
			}
			while(1)
			{
				infile.read(&word, sizeof(char));// read for '\n'
				if(word == '\n') break;
			}
			for(x = 0; x < yNumber; x++)//0,15-->16,15
			{
				infile.read(buf, sizeof(char)*4);
				if(string(buf) == "|   ") MazeY[x][y-1] = true;				
				else MazeY[x][y-1] = false;
			}
			MazeY[x][y-1] = true;//the last column must be true

			startParse = false;
			y--;
			if(y == 0) //the last row muset be true
			{
				for(x = 0; x < yNumber; x++) MazeX[y][x] = true;	
				break;//all parse end
			}
		}
		infile.close();
	}
	else
	{
		cout << "wrong file type to create Maze" << endl;
		return false;
	}

	/*put all the walls*/
	for (int i = 0; i < yNumber; i++)//MazeX
		for (int j = 0; j < xNumber; j++)
		{
			if(MazeX[j][i] == true)
				buildWall(i, j);
		}
	for (int i = 0; i < yNumber; i++)//MazeY
		for (int j = 0; j < xNumber; j++)
		{
			if(MazeY[j][i] == true)
				buildWall(j,i, true);
		}
	return true;
}

/**
 * print Maze messages in a teminal or save to a file
 * */
void Maze::printMaze(bool saveToFile)
{
	/*print from left-top of the Maze*/
	for (int y = yNumber; y > 0; --y)
	{
		cout << "+";
		for(int x = 0; x < yNumber; ++x)	
		{
			if(MazeX[y][x])	cout << "---+";	
			else cout << "   +";
		}
		cout << endl;

		for(int x = 0; x <= yNumber; ++x)	
		{
			if(MazeY[x][y-1]) cout << "|   ";
			else cout << "    ";
		}
		cout << endl;
	}

	/*last row must be true*/
	cout << "+";	
	for (int i = 0; i < yNumber; i++)
	{
			cout << "---+";	
	}
	cout << endl;	

	for(int x = 0; x < xNumber; x++)
	{
		for(int y = 0; y < yNumber; y++)
		{
			cout << MazeX[x][y] << " ";
		}
		cout << endl;
		for(int y = 0; y < yNumber; y++)
		{
			cout << MazeY[x][y] << " ";
		}
		cout << endl;
	}
}

void Maze::buildPillar(int x, int y)
{
	static int number;
	Entity* pillar = mSceneManager->createEntity(String("pillar")+StringConverter::toString(number), "pillar.mesh");
	SceneNode* piilarNode = mSceneManager->getRootSceneNode()->createChildSceneNode(Vector3(-144.0f + y*18.0f, -7, -144 + x * 18.0f));
	piilarNode->attachObject(pillar);
	piilarNode->setScale(0.5, 0.5, 0.5);
	number++;
}

void Maze::buildWall(int x, int y, bool rotate)
{
	int deltaX, deltaY;
	char direct;
	if(rotate == true)
	{
		deltaX = 0;
		deltaY = 9;
		direct = 'y';
	}
	else
	{
		deltaX = 9;
		deltaY = 0;
		direct = 'x';
	}
	Entity* wall = mSceneManager->createEntity( //name: wallx.y
			String("wall") + (direct) +
			StringConverter::toString(x) + "." + StringConverter::toString(y), "wall.mesh");
	SceneNode* wallNode = mSceneManager->getRootSceneNode()->\
			createChildSceneNode(Vector3(-144.0f + y * 18.0f + deltaY, -7, -144 + x * 18.0f + deltaX));
	wallNode->attachObject(wall);
	wallNode->setScale(0.5, 0.5, 0.5);
	if(rotate == true)
		wallNode->rotate(Vector3::UNIT_Y, Degree(90.0f), Node::TS_WORLD);
}

void Maze::destroyWall(int x, int y)
{

}

void Maze::injectMousePressed(const OIS::MouseEvent &arg, enum OIS::MouseButtonID id,
		SdkTrayManager* mTrayMgr, SceneManager* mSceneMgr, Camera* mCamera)
{
	/*Get the Wall clicked by mouse*/
	Ogre::Ray mouseRay = mTrayMgr->getCursorRay(mCamera);
	Ogre::RaySceneQuery *query = mSceneMgr->createRayQuery(mouseRay);
	query->setSortByDistance(true);
	Ogre::RaySceneQueryResult &fowardQuery = query->execute();
	Ogre::RaySceneQueryResult::iterator itr = fowardQuery.begin();
	for (; itr != fowardQuery.end(); itr++)
	{
		if(itr->movable && itr->movable->getName() != ""
				&& itr->movable->getName() != mCamera->getName()
				&& itr->movable->getName() != "PlaneEntity")
		{
			//cout << "Fdetected " << itr->movable->getName() << endl;
			pair<string, float> elm;
			elm.first = itr->movable->getName();
			elm.second = 6.0f;
			mWallStatus->push_back(elm);
			break;
		}
	}
	mSceneMgr->destroyQuery(query);
}

void Maze::frameRenderingQueued(const FrameEvent& evt)
{
	if(!mWallStatus->empty())
	{
		SceneNode* node;
		float delta = 6.0f * evt.timeSinceLastFrame;
		/*move the selected walls*/
		for(unsigned int i = 0; i < mWallStatus->size(); i++)
		{
			node = mSceneManager->getSceneNode(mWallStatus->operator [](i).first);
			if(mWallStatus->operator [](i).second <= delta)
				delta = mWallStatus->operator [](i).second;
			node->translate(0.0f, delta, 0.0f, Node::TS_WORLD);
			mWallStatus->operator [](i).second -= delta;
			if(mWallStatus->operator [](i).second < 0.001)
				mWallStatus->operator [](i).second = 0;
		}

		/*erase the wall number witch have been complished*/
		for(unsigned int i = 0; i < mWallStatus->size(); i++)
		{
			if (mWallStatus->operator [](i).second == 0)
			{
				std::vector<pair<string, float> >::iterator itr = mWallStatus->begin() + i - 1;
				mWallStatus->erase(itr);
			}
		}
	}
}
