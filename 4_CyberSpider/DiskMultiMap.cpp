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

MultiMapTuple DiskMultiMap::Iterator::operator*()
{
	Node curNode;
	m_bf->read(curNode, m_ptr);
	MultiMapTuple mmp;
	mmp.context = curNode.tp.context; //assuming c-string to string is good
	mmp.key = curNode.tp.key;
	mmp.value = curNode.tp.value;
	return mmp;
}

DiskMultiMap::Iterator& DiskMultiMap::Iterator::operator++()
{
	Node curNode;
	m_bf->read(curNode, m_ptr);
	m_ptr = curNode.nextNode;
	return *this;
}

DiskMultiMap::Iterator DiskMultiMap::search(const std::string& key)
{
	Header h;
	bf.read(h, 0);
	size_t hashedStr = hash < std::string>()(key) % h.numBuckets;
	BinaryFile::Offset bucketPlace = hashedStr*sizeof(BinaryFile::Offset) + sizeof(h);
	Iterator i(bucketPlace, &bf);
	while(true)
	{
		if ((*i).key == key)
			return i;
		Node curNode;
		bf.read(curNode, bucketPlace);
		/*if(curNode.nextNode == -1)
			return false;
		curNode.nextNode*/
	}
}

bool DiskMultiMap::insert(const std::string& key, const std::string& value,
	const std::string& context)
{
	if (key.length() > 120 || value.length() > 120 || context.length() > 120)
		return false;
	Header h;
	bf.read(h, 0);
	Tuple tuple;
	strcpy_s(tuple.context, context.c_str());
	strcpy_s(tuple.value, value.c_str());
	strcpy_s(tuple.key, key.c_str());
	size_t hashedStr = hash < std::string>()(key)% h.numBuckets;
	BinaryFile::Offset bucketPlace = hashedStr*sizeof(BinaryFile::Offset) + sizeof(h);
	BinaryFile::Offset firstNode;
	bf.read(firstNode, bucketPlace); //remember the first value of linked list
	Node n;
	n.tp = tuple;
	n.nextNode = firstNode;
	//points new node to the original first value
	if (h.emptyHead == -1)
	{
		cout << "Node-1: " << bf.fileLength();
		if (!bf.write(bf.fileLength(), bucketPlace))
			cout << "Failed to write"; //points head node with new node
		if (!bf.write(n, bf.fileLength()))
			cout << "Failed to Write";
		cout << "Node+1: " << bf.fileLength() << endl;
	}
	else
	{
		

	}
	return true;
}

bool DiskMultiMap::createNew(const std::string& filename, unsigned int numBuckets)
{
	close();
	if (!bf.createNew(filename))
		return false;
	cout << "B4:: " << bf.fileLength() << endl;
	Header h;
	h.emptyHead = -1;
	h.numOfNodes = 0;
	h.numBuckets = numBuckets;
	bf.write(h, 0);
	for (int i = 0; i != numBuckets; i++)
	{
		cout << "B4:: " << bf.fileLength();
		bf.write(-1, bf.fileLength());
		cout << "AFTER : " << bf.fileLength() << endl;
	}
	return true;
}

void DiskMultiMap::close()
{
	if (bf.isOpen())
	{
		cout << "Header SIZE; " << sizeof(Header) << endl;
		cout << "Offset : " << sizeof(BinaryFile::Offset) << endl;
		cout << "Node : " << sizeof(Node) << endl;
		cout << "Total Bytes: " << sizeof(Header) + sizeof(BinaryFile::Offset) * 20 + sizeof(Node) * 4;
		cout << "FILE LENGTH: " << bf.fileLength() << endl;
		bf.close();
	}
}

DiskMultiMap::~DiskMultiMap()
{
	close();
}

int main()
{
	DiskMultiMap x;
	x.createNew("txt.dat", 20);
	x.insert("hmm.exe", "pfft.exe", "m52902");
	x.insert("hmm.exe", "pfft.exe", "m52902");
	x.insert("hmm.exe", "pfft.exe", "m10001");
	x.insert("blah.exe", "bletch.exe", "m0003");
	x.close();
}