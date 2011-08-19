#ifndef PLOTSET_HH
#define PLOTSET_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT

#include <vector>
#include <string>

#include "TObject.h"
#include "TFile.h"

class PlotSet : public std::vector<TObject*>{

 public:
  PlotSet()
    : std::vector<TObject*>(){}

  PlotSet(const PlotSet& other)
    : std::vector<TObject*>(other)
    {}
  /* beware!!!! this does not do
     the memory management for
     the plots, just passes
     around a set of pointers!
  */

  void save(const std::string& filename);
};
#endif
//
