#ifndef CHARMDPKSPIWRAPPER_H 
#define CHARMDPKSPIWRAPPER_H 1

// Include files

// local
#include "TMVAWrapper.h"

namespace MyDpKspiSpace {
  class ReadBDT;
}

/** @class CharmDpKspiWrapper CharmDpKspiWrapper.h
 *  
 *  Wrapper for Dp -> Kspi classifier
 *
 *  @author Jack Timpson Wimberley
 *  @date   2014-02-18
 */
class CharmDpKspiWrapper : public TMVAWrapper {
 public: 
  CharmDpKspiWrapper(std::vector<std::string> &);
  ~CharmDpKspiWrapper();
  double GetMvaValue(std::vector<double> const &);

 protected:

 private:
  MyDpKspiSpace::ReadBDT* mcreader;

};
#endif // CHARMDPKSPIWRAPPER_H
