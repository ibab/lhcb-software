// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT
#include "Mint/NamedParameterStream.h"

#include <string>
using namespace MINT;

NamedParameterStream& operator>>(NamedParameterStream& eam
				 , std::string& ing
				 ){
  std::getline(eam,ing);
  return eam;
}
