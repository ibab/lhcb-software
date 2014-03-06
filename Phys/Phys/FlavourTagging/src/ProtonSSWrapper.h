#ifndef PROTONSSWRAPPER_H 
#define PROTONSSWRAPPER_H 1

// Include files

// local
#include "TMVAWrapper.h"

namespace MyProtonSSSpace 
{
  class ReadssProton;
}

/** @class ProtonSSWrapper ProtonSSWrapper.h
 *
 *  Wrapper for SS Proton BDT algorithm  
 *
 *  @author Jack Timpson Wimberley
 *  @date   2014-02-19
 */
class ProtonSSWrapper : public TMVAWrapper {
public: 
  ProtonSSWrapper(std::vector<std::string> &);
  ~ProtonSSWrapper();
  double GetMvaValue(std::vector<double> const &);

private:
  MyProtonSSSpace::ReadssProton* reader;
  
};
#endif // PROTONSSWRAPPER_H
