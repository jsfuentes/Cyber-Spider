#ifndef DISKMULTIMAP_H_
#define DISKMULTIMAP_H_

#include <string>
#include "MultiMapTuple.h"
#include "BinaryFile.h"
#include "MultiMapTuple.h"

class DiskMultiMap
{
public:

	class Iterator
	{
	public:
		Iterator():m_ptr(-1), m_bf(nullptr) {};
		Iterator(BinaryFile::Offset ptr, BinaryFile* bf) :m_ptr(ptr), m_bf(bf) { };
		// You may add additional constructors
		bool isValid() const { return (m_ptr == -1 ? false : true); };
		Iterator& operator++();
		MultiMapTuple operator*();

	private:
		BinaryFile::Offset m_ptr;
		BinaryFile* m_bf;
		// Your private member declarations will go here
	};

	DiskMultiMap() {};
	~DiskMultiMap();
	bool createNew(const std::string& filename, unsigned int numBuckets);
	bool openExisting(const std::string& filename) {};
	void close();
	bool insert(const std::string& key, const std::string& value, const std::string& context);
	Iterator search(const std::string& key);
	int erase(const std::string& key, const std::string& value, const std::string& context) {};

private:
	BinaryFile bf;
	struct Tuple
	{
		char key[121];
		char value[121];
		char context[121];
	};
	struct Header
	{
		unsigned int numOfNodes;
		unsigned int numBuckets;
		BinaryFile::Offset emptyHead;
	};
	struct Node
	{
		Tuple tp;
		BinaryFile::Offset nextNode; 
	};
	// Your private member declarations will go here
};
#endif // DISKMULTIMAP_H_