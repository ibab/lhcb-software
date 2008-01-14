#ifndef OFFLINE_H_
#define OFFLINE_H_

#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "presenter.h"

class Offline
{
public:
	Offline();
	virtual ~Offline();
  boost::filesystem::path findFile(const boost::filesystem::path & dirPath,
                                   const std::string & fileName);
  void listDirectory();                
private:
  boost::filesystem::path m_path;
};

#endif /*OFFLINE_H_*/
