// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT
#ifndef MINUIT_PARAMETER_SET
#define MINUIT_PARAMETER_SET

#include <vector>
#include <iostream>
#include "TMinuit.h"

#include "Mint/IMinuitParameter.h"


class TNtupleD;
class TFile;

namespace MINT{

class MinuitParameterSet{
 protected:
  std::vector<IMinuitParameter*> _parPtrList;
  static MinuitParameterSet* _defaultMinuitParameterSet;

  static const char ntpNameChars[];
  static const char prtNameChars[];
  static std::string prtToNtpName(const std::string& s_in);

  bool addToEnd(IMinuitParameter* parPtr);
  bool setAllIndices();
 public:
  static MinuitParameterSet* getDefaultSet();

  MinuitParameterSet();
  MinuitParameterSet(const MinuitParameterSet& other);

  MinuitParameterSet getFloating();

  bool add(IMinuitParameter* parPtr);
  bool unregister(IMinuitParameter* patPtr);

  unsigned int size()const;

  //  double getVal(int i) const;
  IMinuitParameter* getParPtr(unsigned int i);
  const IMinuitParameter* getParPtr(unsigned int i) const;


  // regarding the two routines below:
  // normally you don't want to use deleteListAndObjects()
  // This object is a collection of pointers
  // but it does not own the objects being
  // pointed to. However, if you created
  // the objects pointed to with 'new', this might
  // be a useful routine for memory management. Otherwise
  // use deleteListKeepObjects(). Or nothing at all.

  void deleteListAndObjects(); 
  // pitfall: delete objects in use
    void deleteListKeepObjects();
  // pitfall: potential memory leak

  TNtupleD* makeNewNtpForOwner(TFile*& ntpFile) const;
  std::string ntpNames() const;
  void fillNtp(TFile*& ntpFile, TNtupleD*& ntp) const;

  void print(std::ostream& os = std::cout) const;
  void printVariable(std::ostream& os = std::cout) const;
  void printResultVsInput(std::ostream& os = std::cout) const;
};

}//namespace MINT
#endif
//
