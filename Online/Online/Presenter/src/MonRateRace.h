#ifndef MONRATERACE_H_
#define MONRATERACE_H_

#include <string>

#include "dic.hxx"
#include "presenter.h"

// forward declaration
namespace boost {
  class mutex;
}

class MonRateRace: public DimInfo {
public:
  /// Constructor
	MonRateRace( const std::string & serviceName , 
               pres::EffServiceType serviceType);

  /// Destructor 
	virtual ~MonRateRace();

  void infoHandler();
  double currentValue();
  int currentTCK();
  std::string title();
  bool isRateValid();
  pres::EffServiceType effServiceType() { return m_serviceType; }
private:
  double m_value;
  int    m_tck;
  std::string m_title;
  boost::mutex* m_infoHandlerMutex;
  bool m_rateIsValid;
  pres::EffServiceType m_serviceType;
};

#endif /*MONRATERACE_H_*/
