#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <list>
#include <string>
#include <memory>
#include <vector>
using std::list;
using std::string;
using std::vector;
using std::shared_ptr;

namespace lightfs {

class DirINode;

class HashTable {

public:
	typedef shared_ptr<DirINode> DirINodePtr;
	explicit HashTable(int allocSize = kAllocSize);
	~HashTable();

	void insert(const string &key, const DirINodePtr &inodePtr);
	void erase(const string &key);
	DirINodePtr search(const string &key) const;
	int size() const;

private:
	list<DirINodePtr>::iterator lookFor(list<DirINodePtr> &inodeList, const string &key);
	list<DirINodePtr>::const_iterator lookFor(const list<DirINodePtr> &inodeList, const string &key) const;
	int hashCode(const string &key) const;
	static const int kAllocSize = 10007;
	vector<list<DirINodePtr> > table_;
	int size_;
};
}
#endif
