#include "DirTree.h"
#include "netlib/base/Logging.h"
#include <assert.h>
#include <sstream>
using std::stringstream;
using mynet::Logger;
using namespace lightfs;

int main()
{
	DirTree dirTree;

	/* add dir */
	for (int i = 0; i < 10; i++) {
		stringstream ss;
		ss << i;
		dirTree.addDir("/", ss.str());
	}

	/* check add dir */
	list<DirTree::INodePtr> childs = dirTree.readDir("/");
	for (list<DirTree::INodePtr>::iterator lit = childs.begin(); lit != childs.end(); lit++) {
		LOG_TRACE << (*lit)->key(); 
	}
	
	for (int i = 0; i < 9; i++) {
		stringstream ss;
		ss << i;
		dirTree.delDir("/", ss.str());
	}

	childs = dirTree.readDir("/");
	for (list<DirTree::INodePtr>::iterator lit = childs.begin(); lit != childs.end(); lit++) {
		LOG_TRACE << (*lit)->key(); 
	}
	
	LOG_TRACE << "finish dir operations";

	for (int i = 20; i < 30; i++) {
		stringstream ss;
		ss << i;
		dirTree.addFile("/9", ss.str());
	}

	childs = dirTree.readDir("/9");
	for (list<DirTree::INodePtr>::iterator lit = childs.begin(); lit != childs.end(); lit++) {
		LOG_TRACE << (*lit)->key(); 
	}

	for (int i = 20; i < 25; i++) {
		stringstream ss;
		ss << i;
		dirTree.delFile("/9", ss.str());
	}

	childs = dirTree.readDir("/9");
	for (list<DirTree::INodePtr>::iterator lit = childs.begin(); lit != childs.end(); lit++) {
		LOG_TRACE << (*lit)->key(); 
	}

	LOG_TRACE << "finish file operations";

	return 0;
}
