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

	// do the thing
	
	status_ = kFinishRead;
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
	fscanf("%s", path);
	string pathStr(path);
	DirINodePtr dirINodePtr(new DirINode(pathStr));
	
	if (dirINodePtr->buildFromDisk(fin_) == 0) {
		parentDir_ = getParentDir(pathStr);
		return dirINodePtr;
	} else {
		return DirINodePtr();
	}
}

CheckPointer::FileINodePtr CheckPointer::buildFile()
{
	char path[kMaxPath];
	fscanf("%s", path);
	string pathStr(path);
	parentDir_ = getParentDir(pathStr);
	string fileName = getFileName(pathStr);
	FileINodePtr fileINodePtr(new FileINode(fileName));
	
	if (fileINodePtr->buildFromDisk(fin_) == 0) {
		parentDir_ = getParentDir(path);
		return fileINodePtr;
	} else {
		return FileINodePtr();
	}

}
