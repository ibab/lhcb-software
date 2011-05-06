#ifndef INAMED_PARAMETER_HH
#define INAMED_PARAMETER_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT

#include <string>
#include "ParsedParameterLine.h"

#include <iostream>


namespace MINT{
class INamedParameter{
 protected:
  INamedParameter(){}
 public:
  virtual ~INamedParameter(){};

  virtual bool setFromParsedLine(const ParsedParameterLine& line)=0;
  virtual const std::string& name() const=0;

  void MattTest()
  {
	  std::cout << "############# Matt's Test #############" << std::endl;
	  std::cout << "############# Matt's Test #############" << std::endl;
	  std::cout << "############# Matt's Test #############" << std::endl;
	  std::cout << "############# Matt's Test #############" << std::endl;
	  std::cout << "############# Matt's Test #############" << std::endl;
	  std::cout << "############# Matt's Test #############" << std::endl;
	  std::cout << "############# Matt's Test #############" << std::endl;
	  std::cout << "############# Matt's Test #############" << std::endl;
	  std::cout << "############# Matt's Test #############" << std::endl;

  }

};
}//namespace MINT
#endif
//
