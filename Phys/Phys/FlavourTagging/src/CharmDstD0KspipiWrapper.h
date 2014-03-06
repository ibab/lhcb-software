#ifndef CHARMDSTD0KSPIPIWRAPPER_H 
#define CHARMDSTD0KSPIPIWRAPPER_H 1

// Include files

// local
#include "TMVAWrapper.h"

namespace MyDstD0KspipiSpace {
  class ReadBDT;
}

/** @class CharmDstD0KspipiWrapper CharmDstD0KspipiWrapper.h
 *  
 *  Wrapper for D0 -> Kpipi0 classifier
 *
 *  @author Jack Timpson Wimberley
 *  @date   2014-02-18
 */
class CharmDstD0KspipiWrapper : public TMVAWrapper {
 public: 
  CharmDstD0KspipiWrapper(std::vector<std::string> &);
  ~CharmDstD0KspipiWrapper();
  double GetMvaValue(std::vector<double> const &);

 protected:

 private:
  MyDstD0KspipiSpace::ReadBDT* mcreader;

};
#endif // CHARMDSTD0KSPIPIWRAPPER_H
