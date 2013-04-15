#include "HashTable.h"
#include "INode.h"
#include <assert.h>
using namespace lightfs;

HashTable::HashTable(int allocSize)
 :  size_(0),
	table_(allocSize)
{
}

HashTable::~HashTable()
{
}

void HashTable::insert(const string &key, const DirINodePtr &inodePtr)
{
	int code = hashCode(key);
	if (table_[code].size() == 0) {
		table_[code].push_back(inodePtr);
	} else {
		/* make sure the key doesn't exist */
		assert(lookFor(table_[code], inodePtr->key()) == table_[code].end());

		table_[code].push_back(inodePtr);
	}
	size_++;
}

void HashTable::erase(const string &key)
{
	int code = hashCode(key);
	list<DirINodePtr>::iterator it = lookFor(table_[code], key);
	assert(it != table_[code].end());
	table_[code].erase(it);
	size_--;
}

HashTable::DirINodePtr HashTable::search(const string &key) const
{
	int code = hashCode(key);
	list<DirINodePtr>::const_iterator it = lookFor(table_[code], key);
	if (it != table_[code].end()) {
		return (*it);
	} else {
		return DirINodePtr();
	}
}

int HashTable::size() const
{
	return size_;
}

list<HashTable::DirINodePtr>::iterator HashTable::lookFor(list<DirINodePtr> &inodeList, const string &key)
{
	list<DirINodePtr>::iterator it;
	for (it = inodeList.begin(); it != inodeList.end(); it++) {
		if ((*it)->key() == key) {
			return it;
		}
	}

	return inodeList.end();
}

list<HashTable::DirINodePtr>::const_iterator HashTable::lookFor(const list<DirINodePtr> &inodeList, const string &key) const
{
	list<DirINodePtr>::const_iterator it;
	for (it = inodeList.begin(); it != inodeList.end(); it++) {
		if ((*it)->key() == key) {
			return it;
		}
	}

	return inodeList.end();
}
/*
 * BKDR hash function
 */
int HashTable::hashCode(const string &key) const
{
	unsigned int seed = 131;
	unsigned int hash = 0;

	for (unsigned int i = 0; i < key.size(); i++) {
		hash = hash * seed + key[i];
	}

	return (hash & 0x7FFFFFFF) % kAllocSize;
}

