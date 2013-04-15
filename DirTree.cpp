#include "DirTree.h"
#include "netlib/base/Logging.h"
#include <sstream>
#include <assert.h>

using mynet::Logger;
using namespace lightfs;
using std::stringstream;

DirTree::DirTree()
{
	DirINodePtr rootNode(new DirINode(string("/")));
	dirTable_.insert(string("/"), rootNode);
}

/*
 * 0 means success, -1 means dir exist
 * -2 means pdir doesn't exist
 */
int DirTree::addDir(const string &pdir, const string &dir)
{
	DirINodePtr pDirNode = dirTable_.search(pdir);
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

	DirINodePtr newDirNode = dirTable_.search(key);
	if (newDirNode != NULL) {
		LOG_TRACE << "New dir already exist";
		return -1;
	}
	newDirNode = DirINodePtr(new DirINode(key));

	/* add new hash table item */
	dirTable_.insert(key, newDirNode);
	assert(dirTable_.search(key) != NULL);

	/* add to the pdir's children list */
	pDirNode->addChild(newDirNode);
	
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
	DirINodePtr pDirNode = dirTable_.search(pdir);
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

	DirINodePtr childDirNode = dirTable_.search(key);
	if (childDirNode == NULL) {
		LOG_TRACE << "child dir doesn't exist";
		return -1;
	}

	if (childDirNode->childCnt() != 0) {
		LOG_TRACE << "The dir is not empty";
		return -3;
	}
	/* delete from the hash table */
	dirTable_.erase(key);

	/* delete from the pdir's children list */
	pDirNode->delChild(childDirNode);
	
	return 0;
}
	
const list<DirTree::INodePtr>& DirTree::readDir(const string &dir) const
{
	DirINodePtr inode = dirTable_.search(dir);
	if (inode == NULL) {
		LOG_TRACE << "Dir " << dir << " not found";
	}
	return inode->children();
}

int DirTree::addFile(const string &pdir, const string &file)
{
	DirINodePtr pDirNode = dirTable_.search(pdir);
	if (pDirNode == NULL) {
		LOG_TRACE << "Parent dir doesn't exist";
		return -2;
	}
	
	if (pDirNode->searchChild(file) != NULL) {
		LOG_TRACE << "file already exist";
		return -1;
	}
	INodePtr newnode(new FileINode(file));
	pDirNode->addChild(newnode);
	
	return 0;
}

int DirTree::delFile(const string &pdir, const string &file)
{
	DirINodePtr pDirNode = dirTable_.search(pdir);
	if (pDirNode == NULL) {
		LOG_TRACE << "Parent dir doesn't exist";
		return -2;
	}
	
	INodePtr node = pDirNode->searchChild(file);
	if (node == NULL) {
		LOG_TRACE << "file doesn't exist";
		return -1;
	}
	
	pDirNode->delChild(node);
	
	return 0;
}

	
int DirTree::addChunk(const string &path, ChunkId chunkId)
{
	FileINodePtr node = getFileINode(path);
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
	node->addChunk(chunkId);
	
	return 0;
}

int DirTree::delLastChunk(const string &path)
{
	FileINodePtr node = getFileINode(path);
	if (node == NULL) {
		LOG_TRACE << "file doesn't exits";
		return -1;
	}
	
	/* ask the chunkserver to delete the chunks */


	/* delete the chunk meta data */
	node->delLastChunk();

	return 0;
}

DirTree::ChunkId DirTree::getChunk(const string &path, int index)
{
	FileINodePtr node = getFileINode(path);
	assert(node != NULL);
	
	assert((index >= 0) && (index < node->chunkCnt()));
	
	return node->getChunk(index);
}

DirTree::ChunkId DirTree::getLastChunk(const string &path)
{
	FileINodePtr node = getFileINode(path);
	assert(node != NULL);
	
	return node->getChunk(node->chunkCnt()-1);
}

DirTree::FileINodePtr DirTree::getFileINode(const string &filePath) const
{
	int pos = filePath.find_last_of('/');
	string dir = filePath.substr(0, pos);
	if (dir == "") {
		dir = "/";
	}

	string file = filePath.substr(pos+1);

	DirINodePtr pdirNode = dirTable_.search(dir);
	if (pdirNode == NULL) {
		LOG_TRACE << "Dir doesn't exist";
		return FileINodePtr();
	}
	
	return pdirNode->searchChild(file);
}


DirTree::DirINodePtr DirTree::getDirINode(const string &dirPath) const
{
	return dirTable_.search(dirPath);
}

