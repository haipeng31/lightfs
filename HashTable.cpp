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

void HashTable::insert(const string &key, const INodePtr &inodePtr)
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
	list<INodePtr>::iterator it = lookFor(table_[code], key);
	assert(it != table_[code].end());
	table_[code].erase(it);
	size_--;
}

HashTable::INodePtr HashTable::search(const string &key)
{
	int code = hashCode(key);
	list<INodePtr>::iterator it = lookFor(table_[code], key);
	if (it != table_[code].end()) {
		return (*it);
	} else {
		return INodePtr();
	}
}

int HashTable::size() const
{
	return size_;
}

list<HashTable::INodePtr>::iterator HashTable::lookFor(list<INodePtr> &inodeList, const string &key)
{
	list<INodePtr>::iterator it;
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

