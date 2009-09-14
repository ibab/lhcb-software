#ifndef MONRATERACE_H_
#define MONRATERACE_H_

#include <string>
#include <boost/thread/mutex.hpp>

#include "dic.hxx"
#include "presenter.h"

class MonRateRace: public DimInfo
{
public:
	MonRateRace(const std::string & serviceName, pres::EffServiceType serviceType);
	virtual ~MonRateRace();
  void infoHandler();
  double currentValue();
  std::string title();
  bool isRateValid();
  pres::EffServiceType effServiceType() { return m_serviceType; }
private:
  double m_value;
  std::string m_title;
  boost::mutex* m_infoHandlerMutex;
  bool m_rateIsValid;
  pres::EffServiceType m_serviceType;
  
};

#endif /*MONRATERACE_H_*/
