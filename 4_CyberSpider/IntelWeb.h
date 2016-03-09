#ifndef INTELWEB_H_
#define INTELWEB_H_

#include "InteractionTuple.h"
#include <string>
#include <vector>

class IntelWeb
{
public:
	IntelWeb() {};
	~IntelWeb() {};
	bool createNew(const std::string& filePrefix, unsigned int maxDataItems) { return false; };
	bool openExisting(const std::string& filePrefix) { return false; };
	void close() {};
	bool ingest(const std::string& telemetryFile) { return false; };
	unsigned int crawl(const std::vector<std::string>& indicators,
		unsigned int minPrevalenceToBeGood,
		std::vector<std::string>& badEntitiesFound,
		std::vector<InteractionTuple>& badInteractions
		) { return 2;};
	bool purge(const std::string& entity) { return false; };

private:
	// Your private member declarations will go here
};

#endif // INTELWEB_H_
