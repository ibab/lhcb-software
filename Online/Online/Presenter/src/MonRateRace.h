#ifndef MONRATERACE_H_
#define MONRATERACE_H_

#include <string>
#include "dic.hxx"
#include <boost/thread/mutex.hpp>

class MonRateRace: public DimInfo
{
public:
	MonRateRace(const std::string & serviceName);
	virtual ~MonRateRace();
  void infoHandler();
  double currentValue(); // boost::recursive_mutex & dimMutex, boost::recursive_mutex & rootMutex
  std::string title(); // boost::recursive_mutex & dimMutex, boost::recursive_mutex & rootMutex
  bool isRateValid();
private:
  double m_value;
  std::string m_title;
  boost::mutex* m_infoHandlerMutex;
  bool m_rateIsValid;
  
};

#endif /*MONRATERACE_H_*/
