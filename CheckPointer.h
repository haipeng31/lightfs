#ifndef CHECKPOINTER_H
#define CHECKPOINTER_H

#include "DirTree.h"
#include <memory>
using std::shared_ptr;
using std::enable_shared_from_this;
namespace lightfs {

class CheckPointer : public enable_shared_from_this<CheckPointer> {
public:
	CheckPointer();

	int readTreeFromDisk(const string &checkPointFile);
	void writeTreeToDisk(const string &checkPointFile);

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
	FILE *fin_;
	FILE *fout_;
	State status_;
	DirTree dirTree_;
};

}
#endif
