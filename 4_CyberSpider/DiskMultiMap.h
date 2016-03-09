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
		Iterator() {};
		// You may add additional constructors
		bool isValid() const {};
		Iterator& operator++() {};
		MultiMapTuple operator*() {};

	private:
		// Your private member declarations will go here
	};

	DiskMultiMap() {};
	~DiskMultiMap();
	bool createNew(const std::string& filename, unsigned int numBuckets);
	bool openExisting(const std::string& filename) {};
	void close();
	bool insert(const std::string& key, const std::string& value, const std::string& context) {};
	Iterator search(const std::string& key) {};
	int erase(const std::string& key, const std::string& value, const std::string& context) {};

private:
	BinaryFile bf;
	struct Node
	{
		MultiMapTuple mp;
		BinaryFile::Offset nextNode; 
	};
	// Your private member declarations will go here
};
#endif // DISKMULTIMAP_H_