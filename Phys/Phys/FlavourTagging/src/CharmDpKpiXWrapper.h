#ifndef CHARMDPKPIXWRAPPER_H 
#define CHARMDPKPIXWRAPPER_H 1

// Include files

// local
#include "TMVAWrapper.h"

namespace MyDpKpiXSpace {
  class ReadBDT;
}

/** @class CharmDpKpiXWrapper CharmDpKpiXWrapper.h
 *  
 *  Wrapper for Dp -> KpiX classifier
 *
 *  @author Jack Timpson Wimberley
 *  @date   2014-02-18
 */
class CharmDpKpiXWrapper : public TMVAWrapper {
 public: 
  CharmDpKpiXWrapper(std::vector<std::string> &);
  ~CharmDpKpiXWrapper();
  double GetMvaValue(std::vector<double> const &);

 protected:

 private:
  MyDpKpiXSpace::ReadBDT* mcreader;

};
#endif // CHARMDPKPIXWRAPPER_H
