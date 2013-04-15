#ifndef DIRTREE_H
#define DIRTREE_H

#include "HashTable.h"
#include "INode.h"
#include <list>
#include <memory>
using std::list;
using std::shared_ptr;
namespace lightfs {

class CheckPointer;
class DirTree {

public:
	typedef shared_ptr<INode> INodePtr;
	typedef shared_ptr<CheckPointer> CheckPointerPtr;
	typedef int64_t ChunkId;
	DirTree();

	/* directory operations */
	int addDir(const string &pdir, const string &dir);
	int delDir(const string &pdir, const string &dir);
	const list<INodePtr>& readDir(const string &dir);
	int addFile(const string &pdir, const string &file);
	int delFile(const string &pdir, const string &file);

	/* file(chunk) operations */
	int addChunk(const string &path, ChunkId chunkId);
	int delLastChunk(const string &path);
	ChunkId getChunk(const string &path, int index);
	ChunkId getLastChunk(const string &path);
	
	INodePtr getINode(const string &path);

	/* checkpoint the dir tree */
	//void checkPoint(const CheckPointerPtr &checkPointerPtr);
private:
	HashTable dirTable_;	
};

}
#endif
