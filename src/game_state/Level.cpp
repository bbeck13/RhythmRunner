#include	"Level.h"



Level::Level(std::shared_ptr<sf::Music>	music,

													std::shared_ptr<Octree>	tree,

													std::shared_ptr<std::vector<Aquila::SignalSource>>	sources)	{

		this->music	=	music;

		this->tree	=	tree;

		this->sources	=	sources;

}



Level::~Level()	{}



std::shared_ptr<sf::Music>	Level::getMusic()	{

		return	music;

}



std::shared_ptr<Octree>	Level::getTree()	{

		return	tree;

}



std::shared_ptr<std::vector<std::shared_ptr<GameObject>>>	Level::getObjects()	{

		return	tree->getObjects();

}



void	Level::AddItem(std::shared_ptr<GameObject>	object)	{

		tree->getObjects()->push_back(object);

		tree	=	std::make_shared<Octree>(tree->getObjects());

		//	braken

		//	this->tree->insert(object);

}



void	Level::SetTree(std::shared_ptr<Octree>	tree)	{

		this->tree	=	tree;

}
