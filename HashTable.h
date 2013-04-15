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

class INode;

class HashTable {

public:
	typedef shared_ptr<INode> INodePtr;
	explicit HashTable(int allocSize = kAllocSize);
	~HashTable();

	void insert(const string &key, const INodePtr &inodePtr);
	void erase(const string &key);
	INodePtr search(const string &key) const;
	int size() const;

private:
	list<INodePtr>::iterator lookFor(list<INodePtr> &inodeList, const string &key);
	list<INodePtr>::const_iterator lookFor(const list<INodePtr> &inodeList, const string &key) const;
	int hashCode(const string &key) const;
	static const int kAllocSize = 10007;
	vector<list<INodePtr> > table_;
	int size_;
};
}
#endif
