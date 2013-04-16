#include "DirTree.h"
#include "CheckPointer.h"
#include "netlib/base/Logging.h"
#include <assert.h>
#include <sstream>
using std::stringstream;
using mynet::Logger;
using namespace lightfs;

int main()
{
	shared_ptr<CheckPointer> checkPointerPtr(new CheckPointer());
	checkPointerPtr->readTreeFromDisk("checkpoint.in");
	checkPointerPtr->executeLog("logfile");
	DirTree dirTree = checkPointerPtr->dirTree();

	/* add dir */
	
	for (int i = 0; i < 10; i++) {
		stringstream ss;
		ss << i;
		dirTree.addDir("/", ss.str());
	}

	for (int i = 20; i < 30; i++) {
		stringstream ss;
		ss << i;
		dirTree.addFile("/9", ss.str());
	}

	for (int i = 30; i < 40; i++) {
		dirTree.addChunk("/9/20", i);
	}
	
	checkPointerPtr->writeTreeToDisk("checkpoint.out");
	LOG_TRACE << "finish test";
	return 0;
}
