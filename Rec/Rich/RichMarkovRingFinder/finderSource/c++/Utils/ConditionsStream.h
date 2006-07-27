
#ifndef LESTER_CONDITIONSSTREAM_H
#define LESTER_CONDITIONSSTREAM_H

// fwd dec
#include "ConditionsStream.fwd" 

// includes
#include <iostream>
#include <sstream>
#include <string>

// declaration
class ConditionsStream {
// ConditionsStream() {};
// ConditionsStream(const ConditionsStream & other) {};
// ConditionsStream & operator=(const ConditionsStream & other) { return *this; };
// virtual ~ConditionsStream() {};
  bool pending;
  std::ostringstream m_ostr;
private:
  static const long wrapAtChars = 256;
  ConditionsStream() : pending(false) {
  };
public:
  static ConditionsStream & instance() {
    static ConditionsStream s;
    return s;
  };
  template<class T>
  ConditionsStream & operator<<(const T & other) {
    m_ostr << other;
    pending = true;
    return *this;
  }
  void flushTo(std::ostream & os) {
    if (pending) {
      pending = false;
      std::istringstream iss(m_ostr.str());
      char buf[wrapAtChars];
      while(  !(iss.getline(buf, wrapAtChars).eof())  ) {
	iss.clear(); // to remove the failbit that is being set for unclear reasons.  If the failbit were not being set, could replace the while line with:
	// while(iss.getline(buf, wrapAtChars)) {
	os << "# CONDITIONS [" << buf << "]" << std::endl; 
      };
      
      // reset the ostringstream
      m_ostr.str("");
    };
  };
};

#endif

