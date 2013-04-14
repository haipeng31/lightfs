#include "DirTree.h"
#include "netlib/base/Logging.h"
#include <sstream>
#include <assert.h>

using mynet::Logger;
using namespace lightfs;
using std::stringstream;

DirTree::DirTree()
{
	INodePtr rootNode(new DirINode(string("/")));
	dirTable_.insert(string("/"), rootNode);
}

/*
 * 0 means success, -1 means dir exist
 * -2 means pdir doesn't exist
 */
int DirTree::addDir(const string &pdir, const string &dir)
{
	INodePtr pDirNode = dirTable_.search(pdir);
	if (pDirNode == NULL) {
		LOG_TRACE << "Parent dir doesn't exist";
		return -2;
	}

	assert(pDirNode != NULL);
	
	/* if add dir in root dir, the key should pay some attention */
	string key = "";
	if (pdir == "/") {
		key = pdir + dir;
	} else {
		key = pdir + "/" + dir;
	}

	INodePtr newDirNode = dirTable_.search(key);
	if (newDirNode != NULL) {
		LOG_TRACE << "New dir already exist";
		return -1;
	}
	newDirNode = INodePtr(new DirINode(key));

	/* add new hash table item */
	dirTable_.insert(key, newDirNode);
	assert(dirTable_.search(key) != NULL);

	/* add to the pdir's children list */
	static_cast<DirINode *>(pDirNode.get())->addChild(newDirNode);
	
	return 0;
}	

/*
 * -2 means parent dir doesn't exist,
 * -1 means dir doesn't exits,
 * -3 means dir is not empty
 *  fix me. now we can only delete an empty dir,
 *  it's silly.
 */
int DirTree::delDir(const string &pdir, const string &dir)
{
	INodePtr pDirNode = dirTable_.search(pdir);
	if (pDirNode == NULL) {
		LOG_TRACE << "Parent dir doesn't exist";
		return -2;
	}

	assert(pDirNode != NULL);
	
	string key = "";
	if (pdir == "/") {
		key = pdir + dir;
	} else {
		key = pdir + "/" + dir;
	}

	INodePtr childDirNode = dirTable_.search(key);
	if (childDirNode == NULL) {
		LOG_TRACE << "child dir doesn't exist";
		return -1;
	}

	if (static_cast<DirINode *>(childDirNode.get())->childCnt() != 0) {
		LOG_TRACE << "The dir is not empty";
		return -3;
	}
	/* delete from the hash table */
	dirTable_.erase(key);

	/* delete from the pdir's children list */
	static_cast<DirINode *>(pDirNode.get())->delChild(childDirNode);
	
	return 0;
}
	
const list<DirTree::INodePtr>& DirTree::readDir(const string &dir)
{
	INodePtr inode = dirTable_.search(dir);
	if (inode == NULL) {
		LOG_TRACE << "Dir " << dir << " not found";
	}
	return static_cast<DirINode *>(inode.get())->children();
}

int DirTree::addFile(const string &pdir, const string &file)
{
	INodePtr pDirNode = dirTable_.search(pdir);
	if (pDirNode == NULL) {
		LOG_TRACE << "Parent dir doesn't exist";
		return -2;
	}
	
	if (static_cast<DirINode *>(pDirNode.get())->searchChild(file) != NULL) {
		LOG_TRACE << "file already exist";
		return -1;
	}
	INodePtr newnode(new FileINode(file));
	static_cast<DirINode *>(pDirNode.get())->addChild(newnode);
	
	return 0;
}

int DirTree::delFile(const string &pdir, const string &file)
{
	INodePtr pDirNode = dirTable_.search(pdir);
	if (pDirNode == NULL) {
		LOG_TRACE << "Parent dir doesn't exist";
		return -2;
	}
	
	INodePtr node = static_cast<DirINode *>(pDirNode.get())->searchChild(file);
	if (node == NULL) {
		LOG_TRACE << "file doesn't exist";
		return -1;
	}
	
	static_cast<DirINode *>(pDirNode.get())->delChild(node);
	
	return 0;
}

	
int DirTree::addChunk(const string &path, ChunkId chunkId)
{
	INodePtr node = getINode(path);
	if (node == NULL) {
		LOG_TRACE << "file doesn't exits";
		return -1;
	}
	/* generate the universal chunk id 
	long long chunkId = IDGenerator::genId();
	stringstream ss;
	ss << chunkId;
	//const string &chunkName = chunkId;

	ChunkInfo info(ss.str(), 3);

	* using the chunk server information to choose the chunk placements 
	 * I set three test chunkserver here, fix me
	 *
	for (int i = 0; i < 3; i++) {
		ChunkServerAddr addr("TestServer", "127.0.0.1", 5555);
		info.setChunkAddr(i, addr);
	}
	*/
	static_cast<FileINode *>(node.get())->addChunk(chunkId);
	
	return 0;
}

int DirTree::delLastChunk(const string &path)
{
	INodePtr node = getINode(path);
	if (node == NULL) {
		LOG_TRACE << "file doesn't exits";
		return -1;
	}
	
	/* ask the chunkserver to delete the chunks */


	/* delete the chunk meta data */
	static_cast<FileINode *>(node.get())->delLastChunk();

	return 0;
}

DirTree::ChunkId DirTree::getChunk(const string &path, int index)
{
	INodePtr node = getINode(path);
	assert(node != NULL);
	
	assert((index >= 0) && (index < static_cast<FileINode *>(node.get())->chunkCnt()));
	
	return static_cast<FileINode *>(node.get())->getChunk(index);
}

DirTree::ChunkId DirTree::getLastChunk(const string &path)
{
	INodePtr node = getINode(path);
	assert(node != NULL);
	
	FileINode *tempFileINode = static_cast<FileINode *>(node.get());
	return tempFileINode->getChunk(tempFileINode->chunkCnt()-1);
}

DirTree::INodePtr DirTree::getINode(const string &path)
{
	INodePtr dirnode = dirTable_.search(path);
	if (dirnode != NULL) {
		return dirnode;
	}
	int pos = path.find_last_of('/');
	string dir = path.substr(0, pos);
	if (dir == "") {
		dir = "/";
	}

	string file = path.substr(pos+1);

	INodePtr pdirnode = dirTable_.search(dir);
	if (pdirnode == NULL) {
		LOG_TRACE << "Dir doesn't exist";
		return INodePtr();
	}
	
	return static_cast<DirINode *>(pdirnode.get())->searchChild(file);
}

void DirTree::checkPoint(const CheckPointerPtr &checkPointerPtr)
{
	INodePtr rootNode = getINode("/");
	rootNode->checkPoint(checkPointerPtr, fp);
}
