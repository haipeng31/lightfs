#include "INode.h"
#include "CheckPointer.h"
using namespace lightfs;

INode::INode(const string &key)
 :  key_(key),
	size_(0),
	atime_(time(NULL)),
	mtime_(time(NULL)),
	ctime_(time(NULL)),
	auth_(0777),
	uid_(0),
	gid_(0)
{
}

const string& INode::key() const
{
	return key_;
}

int INode::size() const 
{
	return size_;
}


time_t INode::atime() const
{
	return atime_;
}

time_t INode::mtime() const
{
	return mtime_;
}

time_t INode::ctime() const 
{
	return ctime_;
}

void INode::updateAtime()
{
	atime_ = time(NULL);
}

void INode::updateMtime()
{
	mtime_ = time(NULL);
}

void INode::updateCtime()
{
	ctime_ = time(NULL);
}

int INode::auth() const
{
	return auth_;
}

void INode::setAuth(int auth)
{
	auth_ = auth;
}

int INode::uid() const
{
	return uid_;
}

void INode::setUid(int uid)
{
	uid_ = uid;
}

int INode::gid() const
{
	return gid_;
}

void INode::setGid(int gid)
{
	gid_ = gid;
}

DirINode::DirINode(const string &key)
 :	INode(key)
{
}

const list<DirINode::INodePtr>& DirINode::children() const
{
	return children_;
}

void DirINode::addChild(const INodePtr &childPtr)
{
	children_.push_back(childPtr);
}

void DirINode::delChild(const INodePtr &childPtr)
{
	children_.remove(childPtr);
}

const DirINode::FileINodePtr DirINode::searchChild(const string &key) const
{
	list<INodePtr>::const_iterator lit;
	for (lit = children_.begin(); lit != children_.end(); lit++) {
		if ((*lit)->key() == key) {
			return (*lit);
		}
	}

	return FileINodePtr();
}

int DirINode::childCnt() const
{
	return children_.size();
}

void DirINode::checkPoint(const CheckPointerPtr &checkPointerPtr)
{
	checkPointerPtr->checkPointDir(shared_from_this());
}

FileINode::FileINode(const string &key)
 :	INode(key)
{
}

const vector<FileINode::ChunkId>& FileINode::chunks() const 
{
	return chunks_;
}

void FileINode::addChunk(ChunkId chunkId)
{
	chunks_.push_back(chunkId);
}

void FileINode::delLastChunk()
{
	assert(chunks_.size() >= 0);
	chunks_.pop_back();
}

FileINode::ChunkId FileINode::getChunk(int index) const
{
	assert((index >= 0) && (index < chunks_.size()));
	return chunks_[index];
}

int FileINode::chunkCnt() const
{
	return chunks_.size();
}

void FileINode::checkPoint(const CheckPointerPtr &checkPointerPtr)
{
	checkPointerPtr->checkPointFile(shared_from_this());
}

/*
int INode::mode()
{
	int type;
	if (type_ == Dir) {
		type = S_IFDIR;
	} else {
		type = S_IFREG;
	}

	return type + auth_;
}

int INode::fileType()
{
	if (type_ == Dir) {
		return S_IFDIR;
	} else {
		return S_IFREG;
	}
}
*/

