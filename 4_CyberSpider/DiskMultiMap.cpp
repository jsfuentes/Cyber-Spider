/*
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
	~DiskMultiMap() {};
	bool createNew(const std::string& filename, unsigned int numBuckets) {};
	bool openExisting(const std::string& filename) {};
	void close() {};
	bool insert(const std::string& key, const std::string& value, 
		const std::string& context) {};
	Iterator search(const std::string& key) {};
	int erase(const std::string& key, const std::string& value, const 
		std::string& context) {};
*/

#include "DiskMultiMap.h"
#include <functional>
using namespace std;

bool DiskMultiMap::createNew(const std::string& filename, unsigned int numBuckets)
{

}

void DiskMultiMap::close()
{
	if (bf.isOpen())
		bf.close();
}

DiskMultiMap::~DiskMultiMap()
{
	close();
}