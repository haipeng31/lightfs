/*
 * author: haipeng31
 * date: 2013/4/2
 *
 * class INode is the base class representing the metadata of a dir or file,
 * class DirNode has a list of sub file and sub dir,
 * class FileINode has a vector of chunk id.
 */

#ifndef INODE_H
#define INODE_H

#include <time.h>
#include <string>
#include <list>
#include <vector>
#include <sys/types.h>		// for int64_t
#include <memory>
#include <assert.h>
//#include "CheckPointer.h"
using std::string;
using std::list;
using std::vector;
using std::shared_ptr;
using std::enable_shared_from_this;
namespace lightfs {

class CheckPointer;
class INode {
public:
	typedef shared_ptr<CheckPointer> CheckPointerPtr;
	explicit INode(const string &key);
	virtual ~INode() { }

	const string &key() const;
	time_t atime() const;
	time_t mtime() const;
	time_t ctime() const;
	void updateAtime();
	void updateMtime();
	void updateCtime();
	
	int size() const;
	void setSize(int size);
	int auth() const;
	void setAuth(int auth);
	int uid() const;
	void setUid(int uid);
	int gid() const;
	void setGid(int gid);
	
	/* do checkpoint */
	virtual void checkPoint(const CheckPointerPtr &) = 0;
	/* initialize from disk file */
	virtual int initFromDisk(FILE *fin);

private:
	string key_;

	/* time attributes */
	time_t atime_;
	time_t mtime_;
	time_t ctime_;
	
	int size_;

	/* authority attribute, not used now */
	int auth_;

	int uid_;
	int gid_;
};

class FileINode;
class DirINode : public INode, public enable_shared_from_this<DirINode> {
public:
	typedef shared_ptr<INode> INodePtr;
	typedef shared_ptr<FileINode> FileINodePtr;
	explicit DirINode(const string &key);
	
	void addChild(const INodePtr &childPtr);
	void delChild(const INodePtr &childPtr);
	const FileINodePtr searchChild(const string &key) const;
	const list<INodePtr> &children() const;
	int childCnt() const;

	void checkPoint(const CheckPointerPtr &checkPointerPtr); 
private:
	list<INodePtr> children_;
};

class FileINode : public INode, public enable_shared_from_this<FileINode> {
public:
	typedef int64_t ChunkId;
	explicit FileINode(const string &key);
	
	void addChunk(ChunkId chunkId);
	void delLastChunk();
	//void delChunk(ChunkId chunkId);
	ChunkId getChunk(int index) const;
	const vector<ChunkId> &chunks() const;
	int chunkCnt() const;
	
	void checkPoint(const CheckPointerPtr &checkPointerPtr);
	int initFromDisk(FILE *fin);
private:
	vector<ChunkId> chunks_;
};

}
#endif
