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
	typedef shared_ptr<FileINode> FileINodePtr;
	typedef shared_ptr<DirINode> DirINodePtr;
	typedef int64_t ChunkId;
	DirTree();

	/* directory operations */
	int addDir(const string &pdir, const string &dir);
	int delDir(const string &pdir, const string &dir);
	const list<INodePtr>& readDir(const string &dir) const;
	int addFile(const string &pdir, const string &file);
	int delFile(const string &pdir, const string &file);

	/* file(chunk) operations */
	int addChunk(const string &path, ChunkId chunkId);
	int delLastChunk(const string &path);
	ChunkId getChunk(const string &path, int index);
	ChunkId getLastChunk(const string &path);
	
	FileINodePtr getFileINode(const string &filePath) const;
	DirINodePtr getDirINode(const string &dirPath) const;
private:
	HashTable dirTable_;	
};

}
#endif
