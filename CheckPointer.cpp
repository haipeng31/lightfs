#include "CheckPointer.h"
#include "netlib/base/Logging.h"
#include <assert.h>
#include <stdio.h>

using namespace lightfs;

CheckPointer::CheckPointer()
 :	dirTree_(),
	status_(kInited),
	fin_(NULL),
	fout_(NULL)
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
void CheckPointer::writeTreeToDisk(const string &checkPointFile)
{
	assert(status_ == kFinishUpdate);
	
	fout_ = fopen(checkPointFile, "w");
	if (fout == NULL) {
		LOG_TRACE << "open " << checkPointFile << " error";
		return 0;
	}

	/*
	 * use visitor disign patter here,then i don't
	 * need to casting
	 */
	dirTree_.checkPoint(shared_from_this());	
	status_ = kFinishWrite;
	
	fclose(fout);
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
}

void CheckPointer::checkPointFile(const shared_ptr<FileINode> &fileINode)
{
}
