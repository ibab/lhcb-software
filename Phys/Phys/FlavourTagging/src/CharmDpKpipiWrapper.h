#ifndef CHARMDPKPIPIWRAPPER_H 
#define CHARMDPKPIPIWRAPPER_H 1

// Include files

// local
#include "TMVAWrapper.h"

namespace MyDpKpipiSpace {
  class ReadBDT;
}

/** @class CharmDpKpipiWrapper CharmDpKpipiWrapper.h
 *  
 *  Wrapper for Dp -> Kpipi classifier
 *
 *  @author Jack Timpson Wimberley
 *  @date   2014-02-18
 */
class CharmDpKpipiWrapper : public TMVAWrapper {
 public: 
  CharmDpKpipiWrapper(std::vector<std::string> &);
  ~CharmDpKpipiWrapper();
  double GetMvaValue(std::vector<double> const &);

 protected:

 private:
  MyDpKpipiSpace::ReadBDT* mcreader;

};
#endif // CHARMDPKPIPIWRAPPER_H
