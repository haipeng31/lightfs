#include "CheckPointer.h"
#include "netlib/base/Logging.h"
#include <assert.h>
#include <stdio.h>
#include <vector>

using mynet::Logger;
using std::vector;

using namespace lightfs;

CheckPointer::CheckPointer()
 :	fin_(NULL),
	fout_(NULL),
	checkPointQueue_(),
	parentDir_(),
	status_(kInitialized),
	dirTree_()
{
}

int CheckPointer::readTreeFromDisk(const string &checkPointFile)
{
	assert(status_ == kInitialized);
	
	fin_ = fopen(checkPointFile.c_str(), "r");
	if (fin_ == NULL) {
		LOG_TRACE << "open file error";
		return -1;
	}

	// do the thing
	while (1) {
		int type;
		if (fscanf(fin_, "%d", &type) != 1) {
			break;
		}
		if (type == 0) {          // Dir INode
			DirINodePtr dirINodePtr = buildDir();
			if (dirINodePtr->key() == "/") {
				continue;
			}
			if (dirINodePtr == NULL) {
				LOG_TRACE << "file format error";
				return -1;
			}
			LOG_TRACE << "parentdir is " << parentDir_;
			if (dirTree_.addDirNode(parentDir_, dirINodePtr) == -1) {
				LOG_TRACE << "error when add dir node";
				return -1;
			}
		} else if (type == 1) {   // File INode
			FileINodePtr fileINodePtr = buildFile();
			if (fileINodePtr == NULL) {
				LOG_TRACE << "file format error";
				return -1;
			}
			if (dirTree_.addFileNode(parentDir_, fileINodePtr) == -1) {
				LOG_TRACE << "error when add file node";
				return -1;
			}
		} else {
			LOG_TRACE << "file format error";
			return -1;
		}
	}
	
	fclose(fin_);
	status_ = kFinishRead;
	return 0;
}

/* 
 * maybe i should use RAII to encapsulate fout.
 * fix me.
 */
int CheckPointer::writeTreeToDisk(const string &checkPointFile)
{
	assert(status_ == kFinishUpdate);
	
	fout_ = fopen(checkPointFile.c_str(), "w");
	if (fout_ == NULL) {
		LOG_TRACE << "open " << checkPointFile << " error";
		return -1;
	}

	/*
	 * use visitor disign patter here,then i don't
	 * need to casting.I use level traverse to traverse the 
	 * directory tree.
	 */
	//dirTree_.checkPoint(shared_from_this());
	string rootPath = "/";
	INodePtr rootNode = INodePtr(dirTree_.getDirINode(rootPath));
	QueueEntity queueEntity;
	queueEntity.node_ = rootNode;
	queueEntity.parentDir_ = "";     // This will not be used
	checkPointQueue_.push(queueEntity);
	
	while (checkPointQueue_.size() > 0) {
		QueueEntity queueEntity = checkPointQueue_.front();
		parentDir_ = queueEntity.parentDir_;
		(queueEntity.node_)->checkPoint(shared_from_this());
		checkPointQueue_.pop();
	}

	status_ = kFinishWrite;
	
	fclose(fout_);
	return 0;
}

void CheckPointer::executeLog(const string &logFile)
{
	assert(status_ == kFinishRead);

	// do the thing
	
	status_ = kFinishUpdate;
}

/* used for debugging */
DirTree &CheckPointer::dirTree()
{
	return dirTree_;
}


void CheckPointer::checkPointDir(const shared_ptr<DirINode> &dirINode)
{
	/* checkpoint the information of the dir */
	fprintf(fout_, "%d %s %ld %ld %ld %d %d %d %d\n", 0, dirINode->key().c_str(),
				dirINode->atime(), dirINode->mtime(), dirINode->ctime(),
				dirINode->size(), dirINode->auth(), dirINode->uid(), dirINode->gid());
	/* push the subdir and subfile to the queue */
	list<INodePtr> children = dirINode->children();
	for (list<INodePtr>::iterator lit = children.begin(); lit != children.end(); lit++) {
		QueueEntity queueEntity;
		queueEntity.node_ = *lit;
		queueEntity.parentDir_ = dirINode->key();
		checkPointQueue_.push(queueEntity);
	}
}

void CheckPointer::checkPointFile(const shared_ptr<FileINode> &fileINode)
{
	/* checkpoint the information of the file */
	string filePath = ((parentDir_ == "/") ? (parentDir_ + fileINode->key()) : (parentDir_ + "/" + fileINode->key()));
	fprintf(fout_, "%d %s %ld %ld %ld %d %d %d %d", 1, filePath.c_str(),
				fileINode->atime(), fileINode->mtime(), fileINode->ctime(),
				fileINode->size(), fileINode->auth(), fileINode->uid(), fileINode->gid());
	
	vector<FileINode::ChunkId> chunkIds = fileINode->chunks();
	fprintf(fout_, " %d", chunkIds.size());
	for (size_t i = 0; i < chunkIds.size(); i++) {
		fprintf(fout_, " %lld", chunkIds[i]);
	}
	fprintf(fout_, "\n");
}

CheckPointer::DirINodePtr CheckPointer::buildDir()
{
	char path[kMaxPath];
	fscanf(fin_, "%s", path);
	string pathStr(path);
	DirINodePtr dirINodePtr(new DirINode(pathStr));
	
	if (dirINodePtr->initFromDisk(fin_) == 0) {
		parentDir_ = getParentDir(pathStr);
		return dirINodePtr;
	} else {
		return DirINodePtr();
	}
}

CheckPointer::FileINodePtr CheckPointer::buildFile()
{
	char path[kMaxPath];
	fscanf(fin_, "%s", path);
	string pathStr(path);
	parentDir_ = getParentDir(pathStr);
	string fileName = getName(pathStr);
	FileINodePtr fileINodePtr(new FileINode(fileName));
	
	if (fileINodePtr->initFromDisk(fin_) == 0) {
		return fileINodePtr;
	} else {
		return FileINodePtr();
	}

}

string CheckPointer::getParentDir(const string &path)
{
	size_t pos = path.find_last_of('/');
	string parentDir = path.substr(0, pos);
	if (parentDir == "") {
		return "/";
	} else {
		return path.substr(0, pos);
	}
}

string CheckPointer::getName(const string &path)
{
	size_t pos = path.find_last_of('/');
	return path.substr(pos+1);
}
