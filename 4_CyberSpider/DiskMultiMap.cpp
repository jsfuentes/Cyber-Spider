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

int DiskMultiMap::erase(const std::string& key, const std::string& value, const
	std::string& context) 
{
	int numDeleted = 0;
	Header h;
	bf.read(h, 0);
	size_t hashedStr = hash < std::string>()(key) % h.numBuckets;
	BinaryFile::Offset bucketPlace = hashedStr*sizeof(BinaryFile::Offset) + sizeof(h);
	BinaryFile::Offset firstNodePlace;
	if (!bf.read(firstNodePlace, bucketPlace))
		cerr << "ERROR reading bucket in search ft";
	if (firstNodePlace == -1) //bucket empty
		return numDeleted;
	Node curNode;
	if (!bf.read(curNode, firstNodePlace))
		cerr << "ERROR reading bucket in search ft";
	Node prevNode = curNode;
	BinaryFile::Offset curNodePlace = firstNodePlace;
	BinaryFile::Offset prevNodePlace;
	while (curNodePlace != -1)
	{
		if (!bf.read(curNode, curNodePlace))
			cerr << "ERROR reading current Node in search ft";
		if(!strcmp(curNode.tp.context, context.c_str()) && !strcmp(curNode.tp.key, key.c_str()) && !strcmp(curNode.tp.value, value.c_str())) //if cur Node matches
		{
			if (firstNodePlace == curNodePlace) //if its the first Node
			{
				if (!bf.write(curNode.nextNode, bucketPlace)) //replaces buckets first
					cerr << "ERROR writing bucket in search ft";
				firstNodePlace = curNode.nextNode;
				curNode.nextNode = h.emptyHead;
				h.emptyHead = curNodePlace;
				curNodePlace = firstNodePlace;

			}
			else //if its within the list and not first node so prev has been intialized
			{
				prevNode.nextNode = curNode.nextNode;
				curNode.nextNode = h.emptyHead;
				h.emptyHead = curNodePlace;
				if (!bf.write(prevNode, prevNodePlace)) //replaces previous node with correct next
					cerr << "ERROR writing previous Node in search ft";
				if (!bf.write(curNode, curNodePlace)) //replaces current Node with correct next in the empty list
					cerr << "ERROR writing header in search ft";
				curNodePlace = prevNode.nextNode;
			}
			h.numOfNodes--;
			numDeleted++;
			if (!bf.write(h, 0)) //replaces header file with correct emptyHead & numOfNodes
				cerr << "ERROR writing header in search ft";
		}
		else {
			prevNodePlace = curNodePlace;
			prevNode = curNode;
			curNodePlace = curNode.nextNode;
		}
	}
	return numDeleted;
}

MultiMapTuple DiskMultiMap::Iterator::operator*()
{
	MultiMapTuple mmp;
	if (!isValid())
		return mmp; //returns unset mmp which since is made up of strings is all empty strings
	Node curNode;
	if (!m_bf->read(curNode, m_ptr))
		cerr << "ERROR failed to read curNode in * operator iterator";
	mmp.context = curNode.tp.context; //assuming c-string to string is good
	mmp.key = curNode.tp.key;
	mmp.value = curNode.tp.value;
	return mmp;
}

DiskMultiMap::Iterator& DiskMultiMap::Iterator::operator++()
{
	if (!isValid())
		return *this;
	Node originalNode;
	if (!m_bf->read(originalNode, m_ptr))
		cerr << "ERROR reading original Node In ++ iterator ft";
	Node curNode = originalNode;
	do {
		m_ptr = curNode.nextNode;
		if (m_ptr == -1) //if there is no next then just proceed to return 
			break;
		if (!m_bf->read(curNode, m_ptr))
			cerr << "ERROR reading current Node In ++ iterator ft";
	} while (strcmp(curNode.tp.key, originalNode.tp.key)); 
	//ensure the iterator points to the same type of key 
	return *this;
}

DiskMultiMap::Iterator DiskMultiMap::search(const std::string& key)
{
	Header h;
	bf.read(h, 0);
	size_t hashedStr = hash < std::string>()(key) % h.numBuckets;
	BinaryFile::Offset bucketPlace = hashedStr*sizeof(BinaryFile::Offset) + sizeof(h);
	BinaryFile::Offset curNodePlace;
	BinaryFile::Offset desiredPosition;
	if (!bf.read(curNodePlace, bucketPlace))
		cerr << "ERROR reading bucket in search ft";
	Node curNode;
	do
	{
		if (curNodePlace == -1) //no next value so not in list so return invalid/default iterator
		{
			Iterator i;
			return i;
		}
		if (!bf.read(curNode, curNodePlace))
			cerr << "ERROR reading bucket in search ft";
		desiredPosition = curNodePlace;
		curNodePlace = curNode.nextNode;
	} while (strcmp(curNode.tp.key, key.c_str())); 
	Iterator i(desiredPosition, &bf);
	return i;
}

bool DiskMultiMap::insert(const std::string& key, const std::string& value,
	const std::string& context)
{
	cout << "B4: " << bf.fileLength() <<" ";
	if (key.length() > 120 || value.length() > 120 || context.length() > 120)
		return false;
	Header h;
	if (!bf.read(h, 0))
		return false;
	Tuple tuple;
	strcpy_s(tuple.context, context.c_str());
	strcpy_s(tuple.value, value.c_str());
	strcpy_s(tuple.key, key.c_str());
	size_t hashedStr = hash < std::string>()(key)% h.numBuckets;
	BinaryFile::Offset bucketPlace = hashedStr*sizeof(BinaryFile::Offset) + sizeof(h);
	BinaryFile::Offset firstNode;
	if (!bf.read(firstNode, bucketPlace)) //remember the first value of linked list
		return false;
	Node n;
	n.tp = tuple;
	n.nextNode = firstNode;
	//points new node to the original first value
	if (h.emptyHead == -1)
	{
		if (!bf.write(bf.fileLength(), bucketPlace))//points head node to new node at end of file
			return false; 
		if (!bf.write(n, bf.fileLength()))//adds node to end of file
			return false;
	}
	else
	{
		Node emptyNode; //grab empty node to get pointer to next empty node
		if (!bf.read(emptyNode, h.emptyHead)) //look at first empty node
			return false;
		if (!bf.write(h.emptyHead, bucketPlace))//points head bucket ptr to new node at previous empty head
			return false;
		if (!bf.write(n, h.emptyHead))//replace empty node
			return false;
		h.emptyHead = emptyNode.nextNode; //remember next empty node in header
	}
	cout << "AFTER: " << bf.fileLength() << endl;
	h.numOfNodes++;
	if (!bf.write(h, 0))
		return false;
	return true;
}

bool DiskMultiMap::createNew(const std::string& filename, unsigned int numBuckets)
{
	close();
	if (!bf.createNew(filename))
		return false;
	Header h;
	h.emptyHead = -1;
	h.numOfNodes = 0;
	h.numBuckets = numBuckets;
	if(!bf.write(h, 0))
		return false;
	for (int i = 0; i != numBuckets; i++)
		if(!bf.write(-1, bf.fileLength()))
			return false;
	return true;
}

void DiskMultiMap::close()
{
	if (bf.isOpen())
	{
		///* 
		Header h;
		bf.read(h, 0);
		cout << "Header SIZE; " << sizeof(h) << endl;
		cout << "Offset : " << sizeof(BinaryFile::Offset) << endl;
		cout << "Node : " << sizeof(Node) << endl;
		cout << "Total Bytes: " << sizeof(Header) + sizeof(BinaryFile::Offset) * h.numBuckets + sizeof(Node) * h.numOfNodes;
		cout << "FILE LENGTH: " << bf.fileLength() << endl;
		//*/
		bf.close();
	}
}

DiskMultiMap::~DiskMultiMap()
{
	close();
}

void printAll(DiskMultiMap* x, string key)
{
	DiskMultiMap::Iterator it = x->search(key);
	if (it.isValid())
	{
		cout << "I found at least 1 item with a key of " << key << endl;
		do
		{
			MultiMapTuple m = *it; // get the association
			cout << "The key is: " << m.key << endl;
			cout << "The value is: " << m.value << endl;
			cout << "The context is: " << m.context << endl;
			cout << endl;
			++it; // advance iterator to the next matching item
		} while (it.isValid());
	}
}

int main()
{
	DiskMultiMap x;
	x.createNew("myhashtable.dat", 100); // empty, with 100 buckets
	x.insert("hmm.exe", "faq.exe", "m52902");
	x.insert("hmm.exe", "pfft.exe", "m10001");
	x.insert("blah.exe", "bletch.exe", "m0003");
	x.erase("hmm.exe", "pfft.exe", "m10001");
	x.erase("blah.exe", "bletch.exe", "m0003");
	x.erase("hmm.exe", "faq.exe", "m52902");
	x.insert("hmm.exe", "pfft.exe", "m529afd2");
	x.insert("hmm.exe", "pfft.exe", "m52902");
	x.insert("help.exe", "ireallyneed.exe", "m124");
	x.insert("help.exe", "rplze.exe", "m124");
	x.erase("hmm.exe", "pfft.exe", "m52902");
	x.insert("hmm.exe", "pfft.exe", "m52902");
	printAll(&x, "hmm.exe");
										 /*x.insert("hmm.exe", "pfft.exe", "m52902");
	x.insert("hmm.exe", "pfft.exe", "m52902");
	x.insert("hmm.exe", "pfft.exe", "m10001");
	x.insert("blah.exe", "bletch.exe", "m0003");
	x.search("JFLKJ:L");
	printAll(&x, "blah.exe");
	printAll(&x, "hmm.exe");
	cout << "ERASED BLAH: " << endl;
	x.erase("blah.exe", "bletch.exe", "m0003");
	printAll(&x, "blah.exe");
	itt = x.search("hmm.exe");
	if (itt.isValid())
	{
		cout << "I found at least 1 item with a key of hmm.exe\n";
		do
		{
			MultiMapTuple m = *itt; // get the association
			cout << "The key is: " << m.key << endl;
			cout << "The value is: " << m.value << endl;
			cout << "The context is: " << m.context << endl;
			cout << endl;
			++itt; // advance iterator to the next matching item
		} while (itt.isValid());
	}
	cout << "ERASED A HMM: \n";
	x.erase("hmm.exe", "pfft.exe", "m52902");
	it = x.search("blah.exe");
	if (it.isValid())
	{
		cout << "I found at least 1 item with a key of blah.exe\n";
		do
		{
			MultiMapTuple m = *it; // get the association
			cout << "The key is: " << m.key << endl;
			cout << "The value is: " << m.value << endl;
			cout << "The context is: " << m.context << endl;
			cout << endl;
			++it; // advance iterator to the next matching item
		} while (it.isValid());
	}
	itt = x.search("hmm.exe");
	if (itt.isValid())
	{
		cout << "I found at least 1 item with a key of hmm.exe\n";
		do
		{
			MultiMapTuple m = *itt; // get the association
			cout << "The key is: " << m.key << endl;
			cout << "The value is: " << m.value << endl;
			cout << "The context is: " << m.context << endl;
			cout << endl;
			++itt; // advance iterator to the next matching item
		} while (itt.isValid());
	}
	cout << "ERASED A HMM: \n";
	x.erase("hmm.exe", "pfft.exe", "m10001");
	x.erase("hmm.exe", "pfft.exe", "m10001");
	it = x.search("blah.exe");
	if (it.isValid())
	{
		cout << "I found at least 1 item with a key of blah.exe\n";
		do
		{
			MultiMapTuple m = *it; // get the association
			cout << "The key is: " << m.key << endl;
			cout << "The value is: " << m.value << endl;
			cout << "The context is: " << m.context << endl;
			cout << endl;
			++it; // advance iterator to the next matching item
		} while (it.isValid());
	}
	itt = x.search("hmm.exe");
	if (itt.isValid())
	{
		cout << "I found at least 1 item with a key of hmm.exe\n";
		do
		{
			MultiMapTuple m = *itt; // get the association
			cout << "The key is: " << m.key << endl;
			cout << "The value is: " << m.value << endl;
			cout << "The context is: " << m.context << endl;
			cout << endl;
			++itt; // advance iterator to the next matching item
		} while (itt.isValid());
	}
	cout << "INSERTED A HMM";
	x.insert("hmm.exe", "pfft.exe", "m10001");
	it = x.search("blah.exe");
	if (it.isValid())
	{
		cout << "I found at least 1 item with a key of blah.exe\n";
		do
		{
			MultiMapTuple m = *it; // get the association
			cout << "The key is: " << m.key << endl;
			cout << "The value is: " << m.value << endl;
			cout << "The context is: " << m.context << endl;
			cout << endl;
			++it; // advance iterator to the next matching item
		} while (it.isValid());
	}
	itt = x.search("hmm.exe");
	if (itt.isValid())
	{
		cout << "I found at least 1 item with a key of hmm.exe\n";
		do
		{
			MultiMapTuple m = *itt; // get the association
			cout << "The key is: " << m.key << endl;
			cout << "The value is: " << m.value << endl;
			cout << "The context is: " << m.context << endl;
			cout << endl;
			++itt; // advance iterator to the next matching item
		} while (itt.isValid());
	}
	x.insert("hmm.exe", "pffcvxt.exe", "m529d02");
	x.insert("hmm.exe", "pfasdft.exe", "m100sda01");
	x.insert("blah.exe", "bledsach.exe", "m00afdaf3");
	x.insert("thisistest.exe", "plez.exe", "m000000");
	x.insert("thisistest.exe", "ibeg.exe", "m0a0000");
	printAll(&x, "blah.exe");
	printAll(&x, "hmm.exe");
	printAll(&x, "thisistest.exe");
	*/
}

