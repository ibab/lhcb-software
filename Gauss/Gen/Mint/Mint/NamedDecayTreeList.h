#ifndef NAMED_DECAY_TREE_LIST_HH
#define NAMED_DECAY_TREE_LIST_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT

#include <map>
#include <string>
#include <iostream>

#include "Mint/DecayTree.h"
#include "Mint/DalitzEventPattern.h"
#include "Mint/AmpInitialiser.h"

typedef std::map<std::string, AmpInitialiser> AmpInitMap;

class NamedDecayTreeList{
  NamedDecayTreeList();
  static NamedDecayTreeList* __ptr;
 protected:
  static AmpInitialiser dummyAmpInitialiser;
  AmpInitMap _trees;             // all
  AmpInitMap _treesOnlyBgSpinZero; // for background
  AmpInitMap _treesNoBgSpinZero; // for signal
  AmpInitMap _treesOnlyCLEO2012;
  AmpInitMap _treesNoCLEO2012;
    
  int make3BodyList();
  int make4BodyList();

  // --- 3 body ---
  int makeKsPiPiList();
  int makeKPiPiZeroList();
  int makeDplusToKKPiList();
  // --- 4 body ---
  int makeKKpipiList();
  int makeK3piList();
  int makeKsPiPiPizeroList();
  int make4PiList();
  // ---------------

  //int add_SpinZero_forAll(); // bgTest
  //int add_D_to_CPConjugates_forAll();
  //int add_CPConjugates_forAll();
  //int add_CLEO2012_forAll();
 public:
  const AmpInitMap& trees(const std::string& opt = "ALL") const;
  // knows options: "ALL", "NoBgSpinZero", "OnlyBgSpinZero"

  unsigned int size(const std::string& opt = "ALL") const{
    return trees(opt).size();}

  ~NamedDecayTreeList(){
    std::cout << "NamedDecayTreeList destructor - should "
	 << "not have been called." << std::endl;
  }

  static NamedDecayTreeList*
    getMe(const std::string& printopt="printNever");
  // possible options: "printNever", "printFirstTime", "printAlways"
  void  add(const AmpInitialiser& tree, const std::string& opt="ALL");
  void  addSimple(const AmpInitialiser& tree, const std::string& opt="ALL");
  void  multiAdd(const AmpInitialiser& tree, const std::string& opt="ALL"){
    return add(tree, opt);}// for backward compatibility, only.


  const AmpInitialiser& find(const std::string& name
			, bool& successFlag) const;
  
  const AmpInitialiser& find(const std::string& name) const{
    bool dummy;
    return find(name, dummy);
  }

  std::string getLongestName() const;

  int makeDecayTreeList();

  void print(std::ostream& os = std::cout) const;
};

std::ostream& operator<<(std::ostream&, const NamedDecayTreeList& ndtl);

#endif
//
