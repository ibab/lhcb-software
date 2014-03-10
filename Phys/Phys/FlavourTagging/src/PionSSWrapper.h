#ifndef PIONSSWRAPPER_H 
#define PIONSSWRAPPER_H 1

// Include files

// local
#include "TMVAWrapper.h"

namespace MyPionSSSpace 
{
  class ReadssPion;
}

/** @class PionSSWrapper PionSSWrapper.h
 *
 *  Wrapper for SS Pion BDT algorithm  
 *
 *  @author Jack Timpson Wimberley
 *  @date   2014-02-19
 */
class PionSSWrapper : public TMVAWrapper {
public: 
  PionSSWrapper(std::vector<std::string> &);
  virtual ~PionSSWrapper();
  double GetMvaValue(std::vector<double> const &);

private:
  MyPionSSSpace::ReadssPion* reader;
  
};
#endif // PIONSSWRAPPER_H
