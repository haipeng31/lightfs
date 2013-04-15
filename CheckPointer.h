#ifndef CHECKPOINTER_H
#define CHECKPOINTER_H

#include "DirTree.h"
#include "INode.h"
#include <memory>
#include <queue>
using std::queue;
using std::shared_ptr;
using std::enable_shared_from_this;
namespace lightfs {

class CheckPointer : public enable_shared_from_this<CheckPointer> {
public:
	typedef shared_ptr<INode> INodePtr;
	CheckPointer();

	int readTreeFromDisk(const string &checkPointFile);
	int writeTreeToDisk(const string &checkPointFile);

	void executeLog(const string &logFile);

	/* used for debugging */
	DirTree &dirTree();
	
	void checkPointDir(const shared_ptr<DirINode> &dirINode);
	void checkPointFile(const shared_ptr<FileINode> &fileINode);

private:
	enum State {
		kInitialized,
		kFinishRead,
		kFinishUpdate,
		kFinishWrite
	};

	struct QueueEntity {
		INodePtr node_;
		string parentDir_;
	};

	FILE *fin_;
	FILE *fout_;
	queue<QueueEntity> checkPointQueue_;
	string parentDir_;
	State status_;
	DirTree dirTree_;
};

}
#endif
