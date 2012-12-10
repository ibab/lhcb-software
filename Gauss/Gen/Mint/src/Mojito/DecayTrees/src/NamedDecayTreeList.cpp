// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT
#include "Mint/NamedDecayTreeList.h"
#include "Mint/AmpInitialiser.h"
#include "Mint/DecayTreeComparisons.h"
#include "Mint/Utils.h"

#include <fstream>

using namespace std;
using namespace MINT;

AmpInitialiser NamedDecayTreeList::dummyAmpInitialiser;

NamedDecayTreeList* NamedDecayTreeList::__ptr=0;

NamedDecayTreeList*
NamedDecayTreeList::getMe(const std::string& printopt){
  bool verbose=false;
  if(0 == __ptr){
    std::string fname("AllKnownDecayTrees.txt");
    ofstream os(fname.c_str());
    __ptr = new NamedDecayTreeList;
    if(verbose){
      cout << "NamedDecayTreeList::getMe():"
	   << "\n\t> You just created a NamedDecayTreeList."
	   << "\n\t> You can find a list of all known decay trees in "
	   << "\n\t> " << fname << "." << endl;
    }
    __ptr->print(os);
    os << endl;
    os.close();
    if(printopt == "printAlways" || printopt == "printFirstTime"){
      __ptr->print();
      cout << endl;
    }
  }else if(printopt == "printAlways"){ // are you mad?
    __ptr->print();
    cout << endl;
  }
  return __ptr;
}
NamedDecayTreeList::NamedDecayTreeList(){
  makeDecayTreeList();       //      D->f
  //  add_D_to_CPConjugates_forAll(); // D->fbar
  //  add_CPConjugates_forAll(); //      Dbar -> fbar and Dbar ->f
  //  add_SpinZero_forAll(); // bgTest (for bg parameterisation)
  //  add_CLEO2012_forAll();
}
const AmpInitialiser& NamedDecayTreeList::find(const std::string& name
					       , bool& successFlag
					       ) const{
  return keyFinder(name, _trees, dummyAmpInitialiser, successFlag);
}

const AmpInitMap& NamedDecayTreeList::trees(const std::string& opt) const{
  if(A_is_in_B("NoBgSpinZero", opt)) return _treesNoBgSpinZero;
  else if(A_is_in_B("OnlyBgSpinZero", opt)) return _treesOnlyBgSpinZero;
  else if(A_is_in_B("NoCLEO2012", opt)) return _treesNoCLEO2012;
  else if(A_is_in_B("OnlyCLEO2012", opt)) return _treesOnlyCLEO2012;
  else if("" == opt || A_is_in_B("ALL", opt)) return _trees;
  else{
    cout << "WARNING in NamedDecayTreeList::trees : Unrecognises option "
	 << opt
	 << "\n will return default tree (\"ALL\") " 
	 << endl;
    return _trees;
  }
}

std::string NamedDecayTreeList::getLongestName() const{
  std::string longest="";
  for(AmpInitMap::const_iterator it=_trees.begin();
      it != _trees.end();
      it++){
    if(it->first.size() > longest.size()) longest = it->first;
  }
  return longest;
}

void NamedDecayTreeList::print(std::ostream& os) const{
  os << "\nNamedDecayTreeMap: Printing all decays I know about\n"
     <<   "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n"
     <<   "\n=============================" 
     << endl;
  int counter=0;
  for(AmpInitMap::const_iterator it=_trees.begin();
      it != _trees.end();
      it++, counter++){
    os << " (" << counter << ")"
       << "  \"" << it->first <<"\"\n";
    os << it->second.tree();
    os << "\n=============================";
    os << endl; 
  }  
}

void NamedDecayTreeList::add(const AmpInitialiser& ai, const std::string& opt){ 
  // formerly "multiAdd"
  // operation with default option:
  // For each decay 
  // D->f 
  // it also adds:  
  // Dbar -> f
  // D -> fbar
  // Dbar -> fbar
  //
  // and all that also in the version "BgSpinZero", and "CLEO2012".
  // BgSpinZero has a spin-factor of "1" (as if all particles inolved
  // had spin 0), which is what we usually use to parameterise
  // non-resonant background. CLEO2012 is uses exactly the lineshapes
  // uses for Lauren's KKpipi analysis (make sure you also use the
  // same mass-width file if you want to reproduce the results
  // exactly!)

  bool isBg=A_is_in_B("BgSpinZero",opt);
  bool isCLEO2012=A_is_in_B("CLEO2012",opt);

  addSimple(ai, opt);
  if(! isBg)addSimple(ai, opt + "BgSpinZero");     
  // if it already is a decay with the BgSpinZero option, then there's not point in adding it again.

  if(! isCLEO2012)addSimple(ai, opt + "CLEO2012");
  // if it already is a decay with the CLEO2012 option, then there's not point in adding it again.
  
  DecayTree      dt = ai.tree();
  anti(dt);   // CP conjugate
  AmpInitialiser CPai(ai);
  CPai.setTree(dt);
  addSimple(CPai, opt);
  if(! isBg) addSimple(CPai, opt + "BgSpinZero");
  if(! isCLEO2012)addSimple(ai, opt + "CLEO2012");

  dt.getVal().antiThis(); // mum back to D0
  AmpInitialiser DtoCPai(ai);
  DtoCPai.setTree(dt);
  addSimple(DtoCPai, opt);
  if(! isBg) addSimple(DtoCPai, opt + "BgSpinZero");
  if(! isCLEO2012)addSimple(ai, opt + "CLEO2012");

  anti(dt); // and the CP conjugate of that, i.e. Dbar->original
  AmpInitialiser DbarToOriginal(ai);
  DbarToOriginal.setTree(dt);
  addSimple(DbarToOriginal, opt);
  if(! isBg) addSimple(DbarToOriginal, opt + "BgSpinZero");
  if(! isCLEO2012)addSimple(ai, opt + "CLEO2012");

}
void NamedDecayTreeList::addSimple(const AmpInitialiser& ai, const std::string& opt){ // formerly "add"
  // Note: This will not allow any duplicates - if you try to add a
  // tree that already exits (same unique(!) name), it will replace
  // the previous entry.
  bool dbThis=false;
  
  //if(dbThis) cout << "adding tree with name " << name << endl;
  AmpInitialiser nai(ai);
  // NOT sure why I'm doing this rather complicated option reading and writng, rather
  // than simply adding opt to nai.. but maybe there was a reason.
  if(A_is_in_B("CLEO2012",opt)){
    nai.addLopt("CLEO2012");
  }
  if(A_is_in_B("BgSpinZero",opt)){
    nai.addLopt("BgSpinZero");
  }
  std::string name = nai.uniqueName();
  if(dbThis) cout << "adding tree with name " << name << endl;
  _trees[name] = nai;

  if(A_is_in_B("BgSpinZero",opt)) _treesOnlyBgSpinZero[name] = nai;
  else  _treesNoBgSpinZero[name] = nai;

  if(A_is_in_B("CLEO2012",opt))    _treesOnlyCLEO2012[name] = nai;
  else _treesNoCLEO2012[name]=nai;
}

/*
  These have become obsolete because "add" now acts as previously
  "multiAdd" and automatically adds all possible versions of the
  decay.  I've kept them here for now in case I need them at some
  point.

int NamedDecayTreeList::add_SpinZero_forAll(){ // bgTest
  //
  // The same D decaying
  // but with spin zero (for background)
  // Will add spin 0 amplitude with index [S] 
  // even if orignal decay was s-wave.

  for(AmpInitMap::const_iterator it=_trees.begin();
      it != _trees.end();
      it++){
    AmpInitialiser ai = it->second;
    DecayTree      dt = ai.tree();
    dt.getVal().setL(0);
    //ai.setTree(dt);
    addSimple(ai, "BgSpinZero");
  }
  return _trees.size();
}

int NamedDecayTreeList::add_CLEO2012_forAll(){
  //
  // The same D decaying
  // but with spin zero (for background)
  // Will add spin 0 amplitude with index [S] 
  // even if orignal decay was s-wave.

  for(AmpInitMap::const_iterator it=_trees.begin();
      it != _trees.end();
      it++){
    AmpInitialiser ai = it->second;
    DecayTree      dt = ai.tree();
    addSimple(ai, "CLEO2012");
  }
  return _trees.size();
}


int NamedDecayTreeList::add_D_to_CPConjugates_forAll(){
  //
  // The same neutral D decaying
  // decaying to the CP-conjugate decay.
  //
  // so for D->f, it adds D->fbar
  //
  // Might not always be physical, then leave
  // Amplitude at zero (which is its default value).
  //
  // note that, if this procedure for some reason
  // produces a decay chain that already exists,
  // this doesn't matter and won't be entered
  // twice due to the clever use of maps.
  //
  // This will do nothing for charged D (or B) decays.

  for(AmpInitMap::const_iterator it=_trees.begin();
      it != _trees.end();
      it++){
    AmpInitialiser ai = it->second;
    DecayTree      dt = ai.tree();
    if(dt.getVal().charge() != "0") continue; // --> way out
    
    anti(dt);
    dt.getVal().antiThis(); // mum back to D0
    ai.setTree(dt);
    addSimple(ai);
  }
  return _trees.size();
}

int NamedDecayTreeList::add_CPConjugates_forAll(){
  //
  //
  //     for D->f,  this adds Dbar->fbar
  // and for D->fbar, it adds Dbar->f
  //
  // note that, if this procedure for some reason
  // produces a decay chain that already exists,
  // this doesn't matter and won't be entered
  // twice due to the clever use of maps.
  //

  for(AmpInitMap::const_iterator it=_trees.begin();
      it != _trees.end();
      it++){
    AmpInitialiser ai = it->second;
    DecayTree      dt = ai.tree();
    anti(dt);
    ai.setTree(dt);
    addSimple(ai);
  }
  return _trees.size();
}

*/

int NamedDecayTreeList::makeDecayTreeList(){
  make3BodyList();
  make4BodyList();
  return _trees.size();
}

int NamedDecayTreeList::make3BodyList(){
  makeKsPiPiList();
  makeKPiPiZeroList();
  makeDplusToKKPiList();
  return _trees.size();
}

int NamedDecayTreeList::make4BodyList(){
  makeKKpipiList();
  makeK3piList();
  makeKsPiPiPizeroList();
  make4PiList();

  return _trees.size();
}

// ========================= 3 Body Decays ======================
int NamedDecayTreeList::makeKsPiPiList(){
/* according to list in
 %\cite{Poluektov:2006ia}
 \bibitem{Poluektov:2006ia}
 A.~Poluektov {\it et al.}  [Belle Collaboration],
  %``Measurement of phi(3) with Dalitz plot analysis of B+ --> D(*) K(*)+
  %decay,''
  Phys.\ Rev.\  D {\bf 73} (2006) 112009
  [arXiv:hep-ex/0604054].
  %%CITATION = PHRVA,D73,112009;%%

  NOTE: sigma_1 and sigma_2 added by hand to ParticleProperties file in:
  ParticleProperties/src/mass_width.csv
  I gave them the following PDG IDs : sigma_1 999001, sigma_2 9992
*/

  // D-> Ks pi pi

  //  bool dbThis=false;
  DecayTree* dk=0;
  

  // Ks sigma_1
  dk = new DecayTree(421);
  dk->addDgtr(310, 999001)->addDgtr(211, -211);
  this->add(*dk);
  delete dk;

  // Ks rho (770)
  dk = new DecayTree(421);
  dk->addDgtr(310, 113)->addDgtr(211, -211);
  this->add(AmpInitialiser(*dk, "GS"));
  this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
  this->add(*dk);
  delete dk;

  // Ks omega
  dk = new DecayTree(421);
  dk->addDgtr(310, 223)->addDgtr(211, -211);
  this->add(*dk);
  delete dk;

  // Ks f0(980) [not used in BaBar fit, I think]
  dk = new DecayTree(421);
  dk->addDgtr(310, 9010221)->addDgtr(211, -211);
  this->add(AmpInitialiser(*dk, "Flatte"));
  this->add(*dk);
  delete dk;

  // Ks omega ???
  dk = new DecayTree(421);
  dk->addDgtr(310, 9010221)->addDgtr(211, -211);
  this->add(*dk);
  delete dk;

  // Ks sigma_2
  dk = new DecayTree(421);
  dk->addDgtr(310, 999011)->addDgtr(211, -211);
  this->add(*dk);
  delete dk;

  // Ks f2(1270)
  dk = new DecayTree(421);
  dk->addDgtr(310, 225)->addDgtr(211, -211);
  this->add(*dk);
  delete dk;

  // Ks f0(1370)
  dk = new DecayTree(421);
  dk->addDgtr(310, 30221)->addDgtr(211, -211);
  this->add(*dk);
  delete dk;

  // Ks rho(1450)
  dk = new DecayTree(421);
  dk->addDgtr(310, 100113)->addDgtr(211, -211);
  this->add(AmpInitialiser(*dk, "GS"));
  this->add(*dk);
  delete dk;

  //* D->Kspipi with K*

  // K*(892)+ pi-
  dk = new DecayTree(421);
  dk->addDgtr(-211, 323)->addDgtr(310, +211);
  this->add(*dk);
  delete dk;
  // note: CP-conjugates (here: K*- pi+) are generated automatically

  // K*(1410)+ pi-
  dk = new DecayTree(421);
  dk->addDgtr(-211, 100323)->addDgtr(310, +211);
  this->add(*dk);
  delete dk;
  // CP-conjugate automatically

  // K0*(1430)+ pi-
  dk = new DecayTree(421);
  dk->addDgtr(-211, 10321)->addDgtr(310, 211);
  this->add(AmpInitialiser(*dk, "Lass"));// both options, Lass or BW
  this->add(*dk);
  delete dk;
  // CP-conjugate automatically

  // K2*(1430)+ pi-
  dk = new DecayTree(421);
  dk->addDgtr(-211, 325)->addDgtr(310, 211);
  this->add(*dk);
  delete dk;
  // CP-conjugate automatically

  // K2*(1680)+ pi-
  dk = new DecayTree(421);
  dk->addDgtr(-211, 30323)->addDgtr(310, 211);
  this->add(*dk);
  delete dk;
  // CP-conjugate automatically

  // non-resonant:
  dk = new DecayTree(421);
  dk->addDgtr(310, 211, -211);
  this->add(*dk);
  delete dk;

  return _trees.size();
}

int NamedDecayTreeList::makeKPiPiZeroList(){
  DecayTree* dk=0;

  /*
  // D->K*_3(1780)0bar(K- pi+) pi0
  dk = new DecayTree(421);
  dk->addDgtr(111, -317)->addDgtr(-321, 211);
  this->add(*dk);
  delete dk;
  // D->K*_3(1780)-(K- pi0) pi+
  dk = new DecayTree(421);
  dk->addDgtr(211, -327)->addDgtr(-321, 111);
  this->add(*dk);
  delete dk;
  (can't handle spin-3 resonances yet)
  */
  // D->K*(1680)0bar(K- pi+) pi0
  dk = new DecayTree(421);
  dk->addDgtr(111, -30313)->addDgtr(-321, 211);
  this->add(*dk);
  delete dk;

  // D->K*(1680)-(K- pi0) pi+
  dk = new DecayTree(421);
  dk->addDgtr(211, -30323)->addDgtr(-321, 111);
  this->add(*dk);
  delete dk;

  // D->rho(1700)+(pi+pi0) K-
  dk = new DecayTree(421);
  dk->addDgtr(-321, 30213)->addDgtr(211, 111);
  this->add(*dk);
  delete dk;

  // D->rho(1450)+(pi+pi0) K-
  dk = new DecayTree(421);
  dk->addDgtr(-321, 100213)->addDgtr(211, 111);
  this->add(*dk);
  delete dk;

  // D->K*_2(1430)0bar(K- pi+) pi0
  dk = new DecayTree(421);
  dk->addDgtr(111, -315)->addDgtr(-321, 211);
  this->add(*dk);
  delete dk;

  // D->K*_2(1430)-(K- pi0) pi+
  dk = new DecayTree(421);
  dk->addDgtr(211, -325)->addDgtr(-321, 111);
  this->add(*dk);
  delete dk;

  // D->K*_0(1430)0bar(K- pi+) pi0
  dk = new DecayTree(421);
  dk->addDgtr(111, -10311)->addDgtr(-321, 211);
  this->add(AmpInitialiser(*dk, "Lass"));// Lass
  this->add(*dk);// BW
  delete dk;

  // D->K*_0(1430)-(K- pi0) pi+
  dk = new DecayTree(421);
  dk->addDgtr(211, -10321)->addDgtr(-321, 111);
  this->add(*dk);
  delete dk;

  // D->K*bar(1410)0(K- pi+) pi0
  dk = new DecayTree(421);
  dk->addDgtr(111, -100313)->addDgtr(-321, 211);
  this->add(*dk);
  delete dk;

  // D->K*(1410)-(K- pi0) pi+
  dk = new DecayTree(421);
  dk->addDgtr(211, -100323)->addDgtr(-321, 111);
  this->add(*dk);
  delete dk;

  // D->K*(892)-(K-pi0) pi+
  dk = new DecayTree(421);
  dk->addDgtr(211, -323)->addDgtr(-321, 111);
  this->add(*dk);
  delete dk;

  // D->K*bar(892)0(K- pi+)
  dk = new DecayTree(421);
  dk->addDgtr(111, -313)->addDgtr(-321, 211);
  this->add(*dk);
  delete dk;

  // D->K- rho+(pi+ pi0)
  dk = new DecayTree(421);
  dk->addDgtr(-321, 213)->addDgtr(211, 111);
  this->add(*dk);
  delete dk;

  // non-resonant
  dk = new DecayTree(421);
  dk->addDgtr(-321, 211, 111);
  this->add(*dk);
  delete dk;


  return _trees.size();

}

// D+ to K- K+ pi+ (Matthew Charles)
int NamedDecayTreeList::makeDplusToKKPiList() {
  DecayTree* dk=0;

  // Resonances in (K- pi+)
  // ----------------------

  // K*(892)0bar K+
  dk = new DecayTree(411);
  dk->addDgtr(321, -313)->addDgtr(-321, 211);
  this->add(*dk);
  delete dk;

  // K0*(1430)0bar K+
  dk = new DecayTree(411);
  dk->addDgtr(321, -10311)->addDgtr(-321, 211);
  this->add(*dk);
  delete dk;

  // K2*(1430)0bar K+
  dk = new DecayTree(411);
  dk->addDgtr(321, -315)->addDgtr(-321, 211);
  this->add(*dk);
  delete dk;

  // kappabar K+
  dk = new DecayTree(411);
  dk->addDgtr(321, -999041)->addDgtr(-321, 211);
  this->add(*dk);
  delete dk;

  // D+ -> (LASS in K- pi+) K+
  // To make Mojito use a LASS parameterization, we have to
  // choose K0*(1430) as the resonance, i.e.
  //    abs(pdg) = 10311 for K0*(1430)+
  //    abs(pdg) = 10321 for K0*(1430)0
  dk = new DecayTree(411);
  dk->addDgtr(321, -10311)->addDgtr(-321, 211);
  this->add(AmpInitialiser(*dk, "Lass"));
  delete dk;

  // Resonances in (K- K+)
  // ---------------------

  // phi(1020) pi+
  dk = new DecayTree(411);
  dk->addDgtr(211, 333)->addDgtr(321, -321);
  this->add(*dk);
  delete dk;

  // phi(1680) pi+
  dk = new DecayTree(411);
  dk->addDgtr(211, 100333)->addDgtr(321, -321);
  this->add(*dk);
  delete dk;

  // a0(1450)0 pi+
  dk = new DecayTree(411);
  dk->addDgtr(211, 10111)->addDgtr(321, -321);
  this->add(*dk);
  delete dk;

  // Non-resonant
  // ------------

  dk = new DecayTree(411);
  dk->addDgtr(321, -321, 211);
  this->add(*dk);
  delete dk;

  // Done
  return _trees.size();
}

//======================== 4 body decays ========================
int NamedDecayTreeList::makeKKpipiList(){
  bool dbThis=false;

  DecayTree* dk=0;

  // D->KK pipi

  // D->K1(1270)K-, K1(1270)->rho(770)K+
  dk = new DecayTree(421);
  dk->addDgtr(-321, 10323)->addDgtr(+321, 113)->addDgtr(211, -211);
  this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
  this->add(*dk);
  if(dbThis) cout << "first decay: " << *dk << endl;
  delete dk;

  // D->K1(1270)K-, K1(1270)->omega(782)K+
  dk = new DecayTree(421);
  dk->addDgtr(-321, 10323)->addDgtr(+321, 223)->addDgtr(211, -211);
  this->add(*dk);
  if(dbThis) cout << "first decay: " << *dk << endl;
  delete dk;


  //D->K1(1270)K-, K1(1270)->K0*(1430)pi+
  dk = new DecayTree(421);
  dk->addDgtr(-321, 10323)->addDgtr(211, 10311)->addDgtr(321, -211);
  this->add(AmpInitialiser(*dk, "Lass"));
  this->add(*dk);
  if(dbThis) cout << "2nd decay: " << *dk << endl;
  delete dk;

  //  name="D->K1(1270)K-, K1(1270)->K0*(892)pi+";
  dk = new DecayTree(421);
  dk->addDgtr(-321, 10323)->addDgtr(211, 313)->addDgtr(321, -211);
  this->add(*dk);
  if(dbThis) cout << "3rd decay: " << *dk << endl;
  delete dk;

  //  K1(1400) K-, K1(1400) -> K0*(892) pi+
  dk = new DecayTree(421);
  dk->addDgtr(-321, 20323)->addDgtr(211, 313)->addDgtr(321, -211);
  add(*dk);
  if(dbThis) cout << "4th decay: " << *dk << endl;
  delete dk;

  // K0*(892) K0*(892)~   
  dk = new DecayTree(421);
  dk->addDgtr( 313)->addDgtr( 321, -211);
  dk->addDgtr(-313)->addDgtr(-321,  211);
  add(*dk);
  if(dbThis) cout << "5th decay: " << *dk << endl;
  delete dk;

  // phi(1020) rho(770)
  dk = new DecayTree(421);
  dk->addDgtr( 333)->addDgtr( 321, -321);
  dk->addDgtr(-113)->addDgtr(-211,  211);
  add(AmpInitialiser(*dk, "RHO_OMEGA"));
  add(*dk);
  if(dbThis) cout << "6th decay: " << *dk << endl;
  delete dk;
  // phi(1020) rho(770) P wave
  dk = new DecayTree(421);
  dk->addDgtr( 333)->addDgtr( 321, -321);
  dk->addDgtr(-113)->addDgtr(-211,  211);
  dk->getVal().setL(1);
  add(AmpInitialiser(*dk, "RHO_OMEGA"));
  add(*dk);
  if(dbThis) cout << "6th decay: " << *dk << endl;
  delete dk;
  // phi(1020) rho(770) D-wave
  dk = new DecayTree(421);
  dk->addDgtr( 333)->addDgtr( 321, -321);
  dk->addDgtr(-113)->addDgtr(-211,  211);
  dk->getVal().setL(2);
  add(AmpInitialiser(*dk, "RHO_OMEGA"));
  add(*dk);
  if(dbThis) cout << "6th decay: " << *dk << endl;
  delete dk;

  // phi(1020) omega(782)
  dk = new DecayTree(421);
  dk->addDgtr( 333)->addDgtr( 321, -321);
  dk->addDgtr(223)->addDgtr(-211,  211);
  add(*dk);
  if(dbThis) cout << "6th decay: " << *dk << endl;
  delete dk;

  // phi(1020) f2(782)
  dk = new DecayTree(421);
  dk->addDgtr( 333)->addDgtr( 321, -321);
  dk->addDgtr( 225)->addDgtr(-211,  211);
  add(*dk);
  if(dbThis) cout << "6th decay: " << *dk << endl;
  delete dk;
  // phi(1020) f2(782) D wave
  dk = new DecayTree(421);
  dk->addDgtr( 333)->addDgtr( 321, -321);
  dk->getVal().setL(2);
  dk->addDgtr( 225)->addDgtr(-211,  211);
  add(*dk);
  if(dbThis) cout << "6th decay: " << *dk << endl;
  delete dk;

  // f2(782) + non-res S-wave
  dk = new DecayTree(421);
  dk->addDgtr(9981)->addDgtr( 321, -321);
  dk->addDgtr( 225)->addDgtr(-211,  211);
  add(*dk);
  if(dbThis) cout << "6th decay: " << *dk << endl;
  delete dk;

  // KK rho(770)
  dk = new DecayTree(421);
  dk->addDgtr( 321, -321);
  dk->addDgtr(-113)->addDgtr(-211,  211);
  add(AmpInitialiser(*dk, "RHO_OMEGA"));
  add(*dk);
  if(dbThis) cout << "7th decay: " << *dk << endl;
  delete dk;

  // KK omega(783)
  dk = new DecayTree(421);
  dk->addDgtr( 321, -321);
  dk->addDgtr(223)->addDgtr(-211,  211);
  add(*dk);
  if(dbThis) cout << "7th decay: " << *dk << endl;
  delete dk;

  // phi(1020) pi pi
  dk = new DecayTree(421);
  dk->addDgtr( 333)->addDgtr( 321, -321);
  dk->addDgtr(-211,  211);
  add(*dk);
  if(dbThis) cout << "8th decay: " << *dk << endl;
  delete dk;

  // K0*(892) K+ pi- 
  dk = new DecayTree(421);
  dk->addDgtr( 321, -211);
  dk->addDgtr(-313)->addDgtr(-321,  211);
  add(*dk);
  if(dbThis) cout << "9th decay: " << *dk << endl;
  delete dk;

  // K0*(892) K- pi+
  dk = new DecayTree(421);
  dk->addDgtr(-321, +211);
  dk->addDgtr( 313)->addDgtr( 321,  -211);
  add(*dk);
  if(dbThis) cout << "10th decay: " << *dk << endl;
  delete dk;

  // D0->f(0)(980) pipi
  dk = new DecayTree(421);
  dk->addDgtr( 9010221)->addDgtr( -321,  +321);
  dk->addDgtr( -211, 211);
  add(AmpInitialiser(*dk, "FocusFlatte"));
  add(AmpInitialiser(*dk, "Flatte"));
  add(*dk);
  if(dbThis) cout << "11th decay: " << *dk << endl;
  delete dk;
  

  // D0->f(0)(980) NonResVbar0(pi-,pi+)
  dk = new DecayTree(421);
  dk->addDgtr(-9993)->addDgtr( -211, 211);
  dk->addDgtr( 9010221)->addDgtr( -211,  +211);
  add(AmpInitialiser(*dk, "FocusFlatte"));
  add(AmpInitialiser(*dk, "Flatte"));
  add(*dk);
  if(dbThis) cout << "11th decay: " << *dk << endl;
  delete dk;


  // f(0)(980) KK
  dk = new DecayTree(421);
  dk->addDgtr( 9010221)->addDgtr( -211,  +211);
  dk->addDgtr( 321, -321);
  add(AmpInitialiser(*dk, "Flatte"));
  add(*dk);
  if(dbThis) cout << "12th decay: " << *dk << endl;
  delete dk;

  // adding non-resonant amplitudes
  
  // phi(1020) NonRes Scalar,
  dk = new DecayTree(421);
  dk->addDgtr( 333)->addDgtr( 321, -321);
  dk->addDgtr(9981)->addDgtr(-211,  211);
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;

  // phi(1020) NonRes V, in S-wave
  dk = new DecayTree(421);
  dk->addDgtr( 333)->addDgtr( 321, -321);
  dk->addDgtr(9993)->addDgtr(-211,  211);
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;

  // phi(1020) NonRes V in P wave laurens
  dk = new DecayTree(421);
  dk->addDgtr( 333)->addDgtr( 321, -321);
  dk->addDgtr(9993)->addDgtr(-211,  211);
  dk->getVal().setL(1);
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;

  // phi(1020) NonRes V, in D wave laurens
  dk = new DecayTree(421);
  dk->addDgtr( 333)->addDgtr( 321, -321);
  dk->addDgtr(9993)->addDgtr(-211,  211);
  dk->getVal().setL(2);
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;
  
   // NonRes S rho(770)
  dk = new DecayTree(421);
  dk->addDgtr( 9981)->addDgtr( 321, -321);
  dk->addDgtr(-113)->addDgtr(-211,  211);
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;

   // NonRes V rho(770) S wave laurens
  dk = new DecayTree(421);
  dk->addDgtr( 9993)->addDgtr( 321, -321);
  dk->addDgtr(-113)->addDgtr(-211,  211);
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;

   // NonRes V rho(770) P wave laurens
  dk = new DecayTree(421);
  dk->addDgtr( 9993)->addDgtr( 321, -321);
  dk->addDgtr(-113)->addDgtr(-211,  211);
  dk->getVal().setL(1);
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;
  
   // NonRes V rho(770) D wave  laurens
  dk = new DecayTree(421);
  dk->addDgtr( 9993)->addDgtr( 321, -321);
  dk->addDgtr(-113)->addDgtr(-211,  211);
  dk->getVal().setL(2);
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;
  
  // NonRes VS
  dk = new DecayTree(421);
  dk->addDgtr(9981)->addDgtr( 211, -211);
  dk->addDgtr(9993)->addDgtr(321,  -321);
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;

  // NonRes VV in S wave lauren
  dk = new DecayTree(421);
  dk->addDgtr( 9993)->addDgtr( 211, -211);
  dk->addDgtr(9993)->addDgtr(321,  -321);
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;

  // NonRes VV in P wave lauren
  dk = new DecayTree(421);
  dk->addDgtr( 9993)->addDgtr( 211, -211);
  dk->addDgtr(9993)->addDgtr(321,  -321);
  dk->getVal().setL(1);
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;
  
 // NonRes VV  D wave lauren
  dk = new DecayTree(421);
  dk->addDgtr( 9993)->addDgtr( 211, -211);
  dk->addDgtr(9993)->addDgtr(321,  -321);
  dk->getVal().setL(2);
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;
  
  // NonRes VS
  dk = new DecayTree(421);
  dk->addDgtr(9981)->addDgtr( 321, -211);
  dk->addDgtr(9993)->addDgtr(211,  -321);
  add(*dk);

  // non-Res VV in S wave
  dk = new DecayTree(421);
  dk->addDgtr( 9993)->addDgtr( 321, -211);
  dk->addDgtr(9993)->addDgtr(211,  -321);
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;

  // NonRes VV in  P wave lauren
  dk = new DecayTree(421);
  dk->addDgtr(9993)->addDgtr(321,  -211);
  dk->addDgtr(9993)->addDgtr(211,  -321);
  dk->getVal().setL(1);
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;
  
 // NonRes VV in D wave lauren
  dk = new DecayTree(421);
  dk->addDgtr( 9993)->addDgtr( 321, -211);
  dk->addDgtr(9993)->addDgtr(211,  -321);
  dk->getVal().setL(2);
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;
  
  // K0*(892) NonRes S
  dk = new DecayTree(421);
  dk->addDgtr( 313)->addDgtr( 321, -211);
  dk->addDgtr(9981)->addDgtr(-321,  211);
  add(*dk);
  if(dbThis) cout << "5th decay: " << *dk << endl;
  delete dk;
  
  // K0*(892) NonRes V in  S wave 
  dk = new DecayTree(421);
  dk->addDgtr( 313)->addDgtr( 321, -211);
  dk->addDgtr(9993)->addDgtr(-321,  211);
  add(*dk);
  if(dbThis) cout << "5th decay: " << *dk << endl;
  delete dk;
  
  // K0*(892) NonRes V in  P wave 
  dk = new DecayTree(421);
  dk->addDgtr( 313)->addDgtr( 321, -211);
  dk->addDgtr(9993)->addDgtr(-321,  211);
  dk->getVal().setL(1);
  add(*dk);
  if(dbThis) cout << "5th decay: " << *dk << endl;
  delete dk;
  
  // K0*(892) NonRes V  D wave 
  dk = new DecayTree(421);
  dk->addDgtr( 313)->addDgtr( 321, -211);
  dk->addDgtr(9993)->addDgtr(-321,  211);
  dk->getVal().setL(2);
  add(*dk);
  if(dbThis) cout << "5th decay: " << *dk << endl;
  delete dk;
  
   
  // adding the non-resonant tensors

 // phi(1020) NonRes Tensor P wave
  dk = new DecayTree(421);
  dk->addDgtr( 333)->addDgtr( 321, -321); // spin 1
  dk->addDgtr(9985)->addDgtr(-211,  211); // spin 2
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;
  
  // phi(1020) NonRes Tensor D wave
  dk = new DecayTree(421);
  dk->addDgtr( 333)->addDgtr( 321, -321);
  dk->addDgtr(9985)->addDgtr(-211,  211);
  dk->getVal().setL(2);
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;
  
   // NonRes Tensor rho(770) P wave
  dk = new DecayTree(421);
  dk->addDgtr( 9985)->addDgtr( 321, -321); // spin 2
  dk->addDgtr(-113)->addDgtr(-211,  211);  // spin 1
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;
  
   // NonRes Tensor rho(770) D wave  laurens
  dk = new DecayTree(421);
  dk->addDgtr( 9985)->addDgtr( 321, -321);
  dk->addDgtr(-113)->addDgtr(-211,  211);
  dk->getVal().setL(2);
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;
  
  // NonRes TV in P wave
  dk = new DecayTree(421);
  dk->addDgtr( 9985)->addDgtr( 211, -211);  // spin 2
  dk->addDgtr(9993)->addDgtr(321,  -321);  // spin 1
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;
  
  // NonRes VT in P wave lauren
  dk = new DecayTree(421);
  dk->addDgtr( 9993)->addDgtr( 211, -211);  // spin 1
  dk->addDgtr(9985)->addDgtr(321,  -321);  // spin 2
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;

 // NonRes TV  D wave lauren
  dk = new DecayTree(421);
  dk->addDgtr( 9985)->addDgtr( 211, -211);
  dk->addDgtr(9993)->addDgtr(321,  -321);
  dk->getVal().setL(2);
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;
// NonRes VT in D wave lauren
  dk = new DecayTree(421);
  dk->addDgtr( 9993)->addDgtr( 211, -211);
  dk->addDgtr(9985)->addDgtr(321,  -321);
  dk->getVal().setL(2);
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;
  
  // NonRes TV in P wave lauren
  dk = new DecayTree(421);
  dk->addDgtr( 9985)->addDgtr( 321, -211);
  dk->addDgtr(9993)->addDgtr(211,  -321);
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;
  
 // NonRes TV in D wave lauren
  dk = new DecayTree(421);
  dk->addDgtr( 9985)->addDgtr( 321, -211);
  dk->addDgtr(9993)->addDgtr(211,  -321);
  dk->getVal().setL(2);
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;
  
  
  // K0*(892) NonRes T in  P wave 
  dk = new DecayTree(421);
  dk->addDgtr( 313)->addDgtr( 321, -211);
  dk->addDgtr(9985)->addDgtr(-321,  211);
  add(*dk);
  if(dbThis) cout << "5th decay: " << *dk << endl;
  delete dk;
  
  // K0*(892) NonRes T  D wave 
  dk = new DecayTree(421);
  dk->addDgtr( 313)->addDgtr( 321, -211);
  dk->addDgtr(9985)->addDgtr(-321,  211);
  dk->getVal().setL(2);
  add(*dk);
  if(dbThis) cout << "5th decay: " << *dk << endl;
  delete dk;
  
//----- TS:
// NonRes TS
  dk = new DecayTree(421);
  dk->addDgtr(9985)->addDgtr( 211, -211);
  dk->addDgtr(9981)->addDgtr(321,  -321);
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;
// NonRes  TS
  dk = new DecayTree(421);
  dk->addDgtr(9981)->addDgtr( 211, -211);
  dk->addDgtr(9985)->addDgtr(321,  -321);
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk; 
  // NonRes  TS
  dk = new DecayTree(421);
  dk->addDgtr(9985)->addDgtr( 321, -211);
  dk->addDgtr(9981)->addDgtr(211,  -321);
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;
 
  /* non-resonant with topology D->R1 P1, R1->R2 P2, R2->P3 P4
	D->nonResP K-
           nonResP -> nonResVector pi+
                           nonResVector -> K+pi-
     (need to implement spin factor for this)
  dk = new DecayTree(421);
  dk->addDgtr(-321, 9991)->addDgtr(211, 9993)->addDgtr(321, -211);
  add(*dk);
  if(dbThis) cout << "somethingth decay: " << *dk << endl;
  delete dk;
  */

  // non-resonant KK pipi
  dk = new DecayTree(421);
  dk->addDgtr(321, -321, 211, -211);
  add(*dk);
  delete dk;

  return _trees.size();
}

int NamedDecayTreeList::makeK3piList(){
 bool dbThis=false;

 DecayTree* dk=0;
 DecayTree* R1=0;
 // non-resonant K-pipipi
 dk = new DecayTree(421);
 dk->addDgtr(-321, 211, 211, -211);
 add(*dk);
 delete dk;

 // K- a1(1260)
 dk = new DecayTree(421);
 dk->addDgtr( -321);
 R1 = new DecayTree(20213); // the a1
 R1->addDgtr(211,  113)->addDgtr( 211,-211);
 dk->addDgtr(R1);
 add(AmpInitialiser(*dk, "RHO_OMEGA"));
 add(*dk);
 delete dk;
 // now the one where the a1->rho pi is D-wave:
 dk = new DecayTree(421);
 dk->addDgtr( -321);
 R1->getVal().setL(2);
 dk->addDgtr(R1);
 add(*dk);
 if(dbThis) cout << "12th decay: " << *dk << endl;
 delete dk;
 delete R1; // (dk stored a copy)
 
 // K-resonances -------------
 // ^^^^^^^^^^^^
 //
 // K1 1270 channels:
 // pi K1(1270)->K rho
 dk = new DecayTree(421);
 R1 = new DecayTree(-10323);
 // dk->addDgtr(211 , -10323)->addDgtr( -321,113  )->addDgtr( 211,-211);
 dk->addDgtr(211);
 R1->addDgtr( -321,113  )->addDgtr( 211,-211);
 dk->addDgtr(R1);
 add(AmpInitialiser(*dk, "RHO_OMEGA"));
 add(*dk);
 delete dk;
 // now K1->rho as D wave
 dk = new DecayTree(421);
 dk->addDgtr(211);
 R1->getVal().setL(2);
 dk->addDgtr(R1);
 add(*dk);
 if(dbThis) cout << "13th decay: " << *dk << endl;
 delete dk;
 delete R1;

 // the following, with the +10323 instead of -10323, is actually a typo, 
 // but kept for backward compatibility:
 // pi K1(1270)
 dk = new DecayTree(421);
 dk->addDgtr(211 , 10323)->addDgtr( -321,113  )->addDgtr( 211,-211);
 add(AmpInitialiser(*dk, "RHO_OMEGA"));
 add(*dk);
 if(dbThis) cout << "13th decay: " << *dk << endl;
 delete dk;

 // pi K1(1270)->omega K
 dk = new DecayTree(421);
 dk->addDgtr(211 , -10323)->addDgtr( -321, 223)->addDgtr( 211,-211);
 add(*dk);
 if(dbThis) cout << "13th decay: " << *dk << endl;
 
 // Added by Molina on Friday 29th January 2010
 // K(1)(1270)(->rho(->pi+,pi-),K-) pi+
 dk = new DecayTree(421);
 R1 = new DecayTree(10313);
 R1->addDgtr(-321,113)->addDgtr(211,-211);
 dk->addDgtr(211);
 dk->addDgtr(R1);
 add(*dk);
 delete dk;
 // now the same with K1->D wave
 dk = new DecayTree(421);
 R1->getVal().setL(2);
 dk->addDgtr(211);
 dk->addDgtr(R1);
 add(*dk);
 if(dbThis) cout << "61st decay: " << *dk << endl;
 delete dk;
 delete R1;

 // pi K1(1270)->f0(1370) K
 dk = new DecayTree(421);
 dk->addDgtr(211 , -10323)->addDgtr( -321, 30221)->addDgtr( 211,-211);
 add(AmpInitialiser(*dk, "RHO_OMEGA"));
 add(*dk);
 if(dbThis) cout << "13th decay: " << *dk << endl;
 delete dk;

 // pi K1(1270)->"non-res 3-body"
 // could be anything. I model it as...
 // K1 -> K Scalar, Scalar->pipi...
 dk = new DecayTree(421);
 dk->addDgtr(211 , -10323)->addDgtr( -321, 9981)->addDgtr( 211,-211);
 add(*dk);
 if(dbThis) cout << "13th decay: " << *dk << endl;
 delete dk;

 // ... and K1 -> pi- Scalar, Scalar->K- pi+
 dk = new DecayTree(421);
 dk->addDgtr(211 , -10323)->addDgtr( -211, 9981)->addDgtr( 211,-321);
 add(*dk);
 if(dbThis) cout << "13th decay: " << *dk << endl;
 delete dk;

 // same with nonResV
 dk = new DecayTree(421);
 R1 = new DecayTree(-10323);
 dk->addDgtr(211);
 R1->addDgtr( -321, 9993)->addDgtr( 211,-211);
 dk->addDgtr(R1);
 add(*dk);
 delete dk;
 // now the same with K1->D wave
 dk = new DecayTree(421);
 R1->getVal().setL(2);
 dk->addDgtr(211);
 dk->addDgtr(R1);
 add(*dk);
 if(dbThis) cout << "13th decay: " << *dk << endl;
 delete dk;
 delete R1;

 // same with nonResV
 dk = new DecayTree(421);
 R1 = new DecayTree(-10323);
 dk->addDgtr(211);
 R1->addDgtr( -211, 9993)->addDgtr( 211,-321);;
 dk->addDgtr(R1);
 add(*dk);
 delete dk;
 // now the same with K1->D wave
 dk = new DecayTree(421);
 R1->getVal().setL(2);
 dk->addDgtr(211);
 dk->addDgtr(R1);
 add(*dk);
 if(dbThis) cout << "13th decay: " << *dk << endl;
 delete dk;
 delete R1;


 // pi K1(1270)->K(1430)->Kpi // K1->K(1430)pi can only be spin 1 (or 3, 5...)
 dk = new DecayTree(421);
 dk->addDgtr(211 , -10323)->addDgtr( -211, -10311)->addDgtr( 211, -321);
 add(AmpInitialiser(*dk, "RHO_OMEGA"));
 add(*dk);
 if(dbThis) cout << "13th decay: " << *dk << endl;
 delete dk;

 // pi K1(1400)->K*(892)pi
 dk = new DecayTree(421);
 R1 = new DecayTree(-20323);
 dk->addDgtr(211);
 R1->addDgtr( -211, -313)->addDgtr( 211, -321);
 dk->addDgtr(R1);
 add(*dk);
 delete dk;
 // now the same with K1->D wave
 R1->getVal().setL(2);
 dk = new DecayTree(421);
 dk->addDgtr(211);
 dk->addDgtr(R1);
 add(*dk);
 if(dbThis) cout << "13th decay: " << *dk << endl;
 delete dk;
 delete R1;

 // K*bar(892) rho S, P, D-wave
 dk = new DecayTree(421);
 dk->addDgtr( -313)->addDgtr( -321,  +211);
 dk->addDgtr(113)->addDgtr(-211,  211);
 add(AmpInitialiser(*dk, "RHO_OMEGA"));
 add(AmpInitialiser(*dk, ""));
 dk->getVal().setL(1);
 add(AmpInitialiser(*dk, "RHO_OMEGA"));
 add(AmpInitialiser(*dk, ""));
 dk->getVal().setL(2);
 add(AmpInitialiser(*dk, "RHO_OMEGA"));
 add(AmpInitialiser(*dk, ""));
 if(dbThis) cout << "16th decay: " << *dk << endl;
 delete dk;

 // K*bar(892) omega(782) S, P, D
 dk = new DecayTree(421);
 dk->addDgtr(-313)->addDgtr(-321,211);
 dk->addDgtr(223)->addDgtr(211,-211);
 add(*dk);
 dk->getVal().setL(1);
 add(*dk);
 dk->getVal().setL(2);
 add(*dk);
 if(dbThis) cout << "57th decay: " << *dk << endl;
 delete dk;


 // K*bar(892) non-res scalar
 dk = new DecayTree(421);
 dk->addDgtr( -313)->addDgtr( -321,  +211);
 dk->addDgtr(9981)->addDgtr(-211,  211);
 add(*dk);
 delete dk;

 // K*bar(892) non-res vector (in relative S, P, D wave)
 dk = new DecayTree(421);
 dk->addDgtr( -313)->addDgtr( -321,  +211);
 dk->addDgtr(9993)->addDgtr(-211,  211);
 add(*dk);
 dk->getVal().setL(1);
 add(*dk);
 dk->getVal().setL(2);
 add(*dk);
 delete dk;

 /*
 // Added by Molina on Wednesday 13th January 2010
 // pi- K1(1270)+, K1(1270)->pi+ K*(892)0 pi+, K*(892)0->pi+ K-
 // this decay makes no sense -leave it in for backward compatibility for now
 dk = new DecayTree(421); // mark for removal.
 dk->addDgtr(-211, 10323)->addDgtr(211, 313)->addDgtr(211, -321);
 add(*dk);
 if(dbThis) cout << "18th decay: " << *dk << endl;
 delete dk;
 */

 // that makes more sense:
 // pi+ K1(1270)-, K1(1270)-->pi+ K*(892)0bar, K*(892)0bar->pi+ K-
 // this decay makes no sense -leave it in for backward compatibility for now
 dk = new DecayTree(421);
 dk->addDgtr(211, -10323)->addDgtr(-211, -313)->addDgtr(211, -321);
 add(*dk);
 if(dbThis) cout << "18th decay: " << *dk << endl;
 delete dk;



// Added by Molina on Thursday 14th January 2010
// K*bar(892) f0(980)
 dk = new DecayTree(421);
 dk->addDgtr(-313)->addDgtr(-321,211);
 dk->addDgtr(9010221)->addDgtr(211,-211);
 add(*dk);
 if(dbThis) cout << "20th decay: " << *dk << endl;
 delete dk;

 // Added by Molina on Thursday 14th January 2010
 // K*bar(892) f2(1270)  P, D wave
 dk = new DecayTree(421);
 dk->addDgtr(-313)->addDgtr(-321,211);
 dk->addDgtr( 225)->addDgtr(211,-211);
 // dk->getVal().setL(1);
 add(*dk);
 dk->getVal().setL(2);
 add(*dk);
 if(dbThis) cout << "21st decay: " << *dk << endl;
 delete dk;


 // D0->f(2)(1270)0(->pi+,pi-),Kappa-bar(->K-,pi+)
  dk = new DecayTree(421);
  dk->addDgtr(-999041)->addDgtr( -321,  +211);
  dk->addDgtr(    225)->addDgtr( -211,  +211);
  add(*dk);
  if(dbThis) cout << "18th decay: " << *dk << endl;

  // D0->f(2)(1270)0(->pi+,pi-),NonResV0(->pi-,pi+)
  dk = new DecayTree(421);
  dk->addDgtr(225)->addDgtr( -211,  +211);
  dk->addDgtr(9993)->addDgtr(-211,  +211);
  add(*dk);

  // D0->f(2)(1270)0(->pi+,pi-),NonResT0(->pi-,pi+)
//  dk = new DecayTree(421);
//  dk->addDgtr(225)->addDgtr( -211,  +211);
//  dk->addDgtr(9985)->addDgtr(-211,  +211);
//  add(*dk);


  // D0->f(2)(1430)0(->pi+,pi-),K*(892)bar0(->K-,pi+) P, D
  dk = new DecayTree(421);
  dk->addDgtr( -313)->addDgtr( -321,  +211);
  dk->addDgtr(9000225)->addDgtr(-211,  211);
  add(*dk);
  dk->getVal().setL(2);
  add(*dk);
  if(dbThis) cout << "18th decay: " << *dk << endl;
  delete dk;

 //// Added by Molina on Thursday 14th January 2010
 //// K*bar(892) f2(1270)    F wave
 //dk = new DecayTree(421);
 //dk->addDgtr(-313)->addDgtr(-321,211);
 //dk->addDgtr(225)->addDgtr(211,-211);
 //dk->getVal().setL(2);
 //add(*dk);
 //if(dbThis) cout << "23rd decay: " << *dk << endl;
 //delete dk;

 // Added by Molina on Thursday 14th January 2010
 // K*bar(892) f0(1370)
 dk = new DecayTree(421);
 dk->addDgtr(-313)->addDgtr(-321,211);
 dk->addDgtr(30221)->addDgtr(211,-211);
 add(*dk);
 if(dbThis) cout << "24th decay: " << *dk << endl;
 delete dk;

 // Added by Molina on Thursday 14th January 2010
 // K*bar(1410) NonResS(pi+ pi-)
 dk = new DecayTree(421);
 dk->addDgtr(-100313)->addDgtr(-321,211);
 dk->addDgtr(9981)->addDgtr(211,-211);
 add(*dk);
 if(dbThis) cout << "25th decay: " << *dk << endl;
 delete dk;

 // Added by Molina on Thursday 14th January 2010
 // K*bar(1430)0 NonResS(pi+ pi-)
 //dk = new DecayTree(421);
 //dk->addDgtr(-10311)->addDgtr(-321,211);						Not possible because D0 is a pseudoscalar so it can't decay
 //dk->addDgtr(9981)->addDgtr(211,-211);						into 2 scalars (K(0)*(1430) and NonResS are scalars).
 //add(*dk);
 //if(dbThis) cout << "26th decay: " << *dk << endl;
 //delete dk;

 // Added by Molina on Thursday 14th January 2010
 // K*bar(1430)2 NonResS(pi+ pi-)
 dk = new DecayTree(421);
 dk->addDgtr(-315)->addDgtr(-321,211);
 dk->addDgtr(9981)->addDgtr(211,-211);
 add(*dk);
 if(dbThis) cout << "27th decay: " << *dk << endl;
 delete dk;

 // Added by Molina on Thursday 14th January 2010
 // K*bar(1680) NonResS(pi+ pi-)
 dk = new DecayTree(421);
 dk->addDgtr(-30313)->addDgtr(-321,211);
 dk->addDgtr(9981)->addDgtr(211,-211);
 add(*dk);
 if(dbThis) cout << "28th decay: " << *dk << endl;
 delete dk;
	
 // Added by Molina on Friday 15th January 2010
 // K*bar(1430)0 rho(770)(pi+ pi-)
 dk = new DecayTree(421);
 dk->addDgtr(-10311)->addDgtr(-321,211);
 dk->addDgtr(113)->addDgtr(211,-211);
 add(*dk);
	cout << " name of mystery decay " << uniqueName(*dk) << endl;
 if(dbThis) cout << "29th decay: " << *dk << endl;
 delete dk;
 
  // K*bar(892) sigma_1
 dk = new DecayTree(421);
 dk->addDgtr(-313)->addDgtr(-321,211);
 dk->addDgtr(999001)->addDgtr(211,-211);
 add(*dk);
 if(dbThis) cout << "60th decay: " << *dk << endl;
 delete dk;

 // K*bar(892) sigma_2
 dk = new DecayTree(421);
 dk->addDgtr(-313)->addDgtr(-321,211);
 dk->addDgtr(999011)->addDgtr(211,-211);
 add(*dk);
 if(dbThis) cout << "60th decay: " << *dk << endl;
 delete dk;

 // K*bar(892) + non res:
 // ^^^^^^^^^^^^^^^^^^^^^
 //
 // Added by Molina on Friday 29th January 2010
 // K*bar(892) NonResS
 dk = new DecayTree(421);
 dk->addDgtr(-313)->addDgtr(-321,211);
 dk->addDgtr(9981)->addDgtr(211,-211);
 add(*dk);
 if(dbThis) cout << "60th decay: " << *dk << endl;
 delete dk;

 // Added by Molina on Monday 18th January 2010
 // K*bar(892)0 NonResV(pi+ pi-) S, P, D
 dk = new DecayTree(421);
 dk->addDgtr(-313)->addDgtr(-321,211);
 dk->addDgtr(9993)->addDgtr(211,-211);
 add(*dk);
 dk->getVal().setL(1);
 add(*dk);
 dk->getVal().setL(2);
 add(*dk);
 if(dbThis) cout << "30th decay: " << *dk << endl;
 delete dk;
	
 // Added by Molina on Monday 18th January 2010
 // K*bar(892)0 NonResT(pi+ pi-), P, D
 dk = new DecayTree(421);
 dk->addDgtr(-313)->addDgtr(-321,211);
 dk->addDgtr(9985)->addDgtr(211,-211);
 add(*dk);
 dk->getVal().setL(2);
 add(*dk);
 if(dbThis) cout << "33th decay: " << *dk << endl;
 delete dk;
 

 // kappba rho
 dk = new DecayTree(421);
 dk->addDgtr(-999041)->addDgtr( -321,  +211);
 dk->addDgtr(113)->addDgtr(-211,  211); 
 add(*dk);
 if(dbThis) cout << "16th decay: " << *dk << endl;
 delete dk;

 // rho(770) + nonres
 // Added by Molina on Thursday 29th April 2010
 // rho(770)(->pi+ pi-) NonResS(K- pi+)
 dk = new DecayTree(421);
 dk->addDgtr(113)->addDgtr(211,-211);
 dk->addDgtr(9981)->addDgtr(-321,211);
 add(*dk);
 if(dbThis) cout << "62nd decay: " << *dk << endl;
 delete dk;
	
 // Added by Molina on Monday 18th January 2010
 // rho(770) NonResV (to K- pi+) S, P, D
 dk = new DecayTree(421);
 dk->addDgtr(9993)->addDgtr(-321,211);
 dk->addDgtr(113)->addDgtr(211,-211);
 add(*dk);
 dk->getVal().setL(1);
 add(*dk);
 dk->getVal().setL(2);
 add(*dk);
 if(dbThis) cout << "35th decay: " << *dk << endl;
 delete dk;
		
 // Added by Molina on Monday 18th January 2010
 // rho(770) NonResT (to K- pi+) P, D
 dk = new DecayTree(421);
 dk->addDgtr(9985)->addDgtr(-321,211);
 dk->addDgtr(113)->addDgtr(211,-211);
 add(*dk);
 dk->getVal().setL(2);
 add(*dk);
 if(dbThis) cout << "38th decay: " << *dk << endl;
 delete dk;

 // omega + nonres
 // omega(782)(->pi+ pi-) NonResS(K- pi+)
 dk = new DecayTree(421);
 dk->addDgtr(223)->addDgtr(211,-211);
 dk->addDgtr(9981)->addDgtr(-321,211);
 add(*dk);
 if(dbThis) cout << "62nd decay: " << *dk << endl;
 delete dk;
	
 // omega(782) NonResV (to K- pi+) S, P, D
 dk = new DecayTree(421);
 dk->addDgtr(9993)->addDgtr(-321,211);
 dk->addDgtr(223)->addDgtr(211,-211);
 add(*dk);
 dk->getVal().setL(1);
 add(*dk);
 dk->getVal().setL(2);
 add(*dk);
 if(dbThis) cout << "35th decay: " << *dk << endl;
 delete dk;
		
 // omega(782) NonResT (to K- pi+) P, D
 dk = new DecayTree(421);
 dk->addDgtr(9985)->addDgtr(-321,211);
 dk->addDgtr(223)->addDgtr(211,-211);
 add(*dk);
 dk->getVal().setL(2);
 add(*dk);
 if(dbThis) cout << "38th decay: " << *dk << endl;
 delete dk;


 // Added by Molina on Monday 18th January 2010
 // K*bar(1410) NonResV(pi+ pi-) S, P, D
 dk = new DecayTree(421);
 dk->addDgtr(-100313)->addDgtr(-321,211);
 dk->addDgtr(9993)->addDgtr(211,-211);
 add(*dk);
 dk->getVal().setL(1);
 add(*dk);
 dk->getVal().setL(2);
 add(*dk);
 if(dbThis) cout << "40th decay: " << *dk << endl;
 delete dk;

	
 // Added by Molina on Monday 18th January 2010
 // K*bar(1410) NonResT(pi+ pi-) P, D
 dk = new DecayTree(421);
 dk->addDgtr(-100313)->addDgtr(-321,211);
 dk->addDgtr(9985)->addDgtr(211,-211);
 add(*dk);
 dk->getVal().setL(2);
 add(*dk);
 if(dbThis) cout << "43th decay: " << *dk << endl;
 delete dk;
	
 // Added by Molina on Monday 18th January 2010
 // K*bar(1430)0 NonResV(pi+ pi-)
 dk = new DecayTree(421);
 dk->addDgtr(-10311)->addDgtr(-321,211);
 dk->addDgtr(9993)->addDgtr(211,-211);
 add(*dk);
 if(dbThis) cout << "45th decay: " << *dk << endl;
 delete dk;

 // Added by Molina on Monday 18th January 2010
 // K*bar(1430)0 NonResT(pi+ pi-)
 dk = new DecayTree(421);
 dk->addDgtr(-10311)->addDgtr(-321,211);
 dk->addDgtr(9985)->addDgtr(211,-211);
 add(*dk);
 if(dbThis) cout << "48th decay: " << *dk << endl;
 delete dk;
	
 // Added by Molina on Monday 18th January 2010
 // K*bar(1430)2 NonResV(pi+ pi-) P, D
 dk = new DecayTree(421);
 dk->addDgtr(-315)->addDgtr(-321,211);
 dk->addDgtr(9993)->addDgtr(211,-211);
 add(*dk);
 dk->getVal().setL(2);
 add(*dk);
 if(dbThis) cout << "50th decay: " << *dk << endl;
 delete dk;
	
 // Added by Molina on Monday 18th January 2010
 // K*bar(1680) NonResV(pi+ pi-) S, P, D
 dk = new DecayTree(421);
 dk->addDgtr(-30313)->addDgtr(-321,211);
 dk->addDgtr(9993)->addDgtr(211,-211);
 add(*dk);
 dk->getVal().setL(1);
 add(*dk);
 dk->getVal().setL(2);
 add(*dk);
 if(dbThis) cout << "52th decay: " << *dk << endl;
 delete dk;

 // Added by Molina on Monday 18th January 2010
 // K*bar(1680) NonResT(pi+ pi-) P, D
 dk = new DecayTree(421);
 dk->addDgtr(-30313)->addDgtr(-321,211);
 dk->addDgtr(9985)->addDgtr(211,-211);
 add(*dk);
 dk->getVal().setL(2);
 add(*dk);
 if(dbThis) cout << "55th decay: " << *dk << endl;
 delete dk;
	
 // Added by Molina on Monday 18th January 2010
 // K*bar(892) rho(1450) S, P, D
 dk = new DecayTree(421);
 dk->addDgtr(-313)->addDgtr(-321,211);
 dk->addDgtr(100113)->addDgtr(211,-211);
 add(*dk);
 dk->getVal().setL(1);
 add(*dk);
 dk->getVal().setL(2);
 add(*dk);
 if(dbThis) cout << "57th decay: " << *dk << endl;
 delete dk;	
	
 // Added by Molina on Friday 29th January 2010
 // K(1)(1270)(->rho(->pi+,pi-),K-) pi+
 dk = new DecayTree(421);
 dk->addDgtr(10313)->addDgtr(-321,113)->addDgtr(211,-211);
 dk->addDgtr(211);
 add(*dk);
 if(dbThis) cout << "61st decay: " << *dk << endl;
 delete dk;

 return _trees.size();
}


//=========================================================================
// create the decay of the type D0->Kspipipiz
//=========================================================================
int NamedDecayTreeList::makeKsPiPiPizeroList() {
  // provided by Loic Esteve
  bool dbThis=false;
  int decayCtr = 1;
  DecayTree* dk=0;

  if (dbThis) {
     cout << "----------------------------------------" << endl;
     cout << "Now doing D0 -> KS pi pi pi0 decays" << endl;
     cout << "----------------------------------------" << endl;
  }

  // non-resonant Kspipipiz
  dk = new DecayTree(421);
  dk->addDgtr(310, 211, -211, 111);
  add(*dk);
  if(dbThis) cout << "Decay #" << decayCtr++ << endl << *dk << endl;
  delete dk;

// K*-(892) rho+(770) 
  dk = new DecayTree(421);
  dk->addDgtr(-323)->addDgtr(310, -211);
  dk->addDgtr(213)->addDgtr(211, 111);
// how to put longitudinal vs transverse here ?
  add(*dk);
  if(dbThis) cout << "Decay #" << decayCtr++ << endl << *dk << endl;
  delete dk;

// K0bar omega
// Mint doesn't know how to build the amplitude for this one since omega -> 3pi 
// I am using the three amplitudes omega -> (rhopi)0 for now 
// Thu Oct 15 16:09:56 2009
  // dk = new DecayTree(421);
  // dk->addDgtr(310,223)->addDgtr(211, -211, 111);
  // if(dbThis) cout << "Decay #" << decayCtr++ << endl << *dk << endl;
  // add(*dk);
  // delete dk;

// K0bar omega, omega -> rho0 pi0
  dk = new DecayTree(421);
  dk->addDgtr(310,223)->addDgtr(111, 113)->addDgtr(211,-211);
  if(dbThis) cout << "Decay #" << decayCtr++ << endl << *dk << endl;
  add(*dk);
  delete dk;

// K0bar omega, omega -> rho+ pi-
  dk = new DecayTree(421);
  dk->addDgtr(310,223)->addDgtr(-211, 213)->addDgtr(211,111);
  if(dbThis) cout << "Decay #" << decayCtr++ << endl << *dk << endl;
  add(*dk);
  delete dk;

// K0bar omega, omega -> rho- pi+
  dk = new DecayTree(421);
  dk->addDgtr(310,223)->addDgtr(211, -213)->addDgtr(-211,111);
  if(dbThis) cout << "Decay #" << decayCtr++ << endl << *dk << endl;
  add(*dk);
  delete dk;


// K*0bar rho0 
  dk = new DecayTree(421);
  dk->addDgtr(-313)->addDgtr(310, 111);
  dk->addDgtr(113)->addDgtr(211, -211);
// how to specify transverse amplitude here ?  I could just put S and
// D waves for now just playing around but this will need some fixing
// one day if I am to use the MARK-III numbers for realistic events
  add(*dk);
  if(dbThis) cout << "Decay #" << decayCtr++ << endl << *dk << endl;
  delete dk;

  //D->K1(1270)-pi+, without any subresonant structure
  // dk = new DecayTree(421);
  // dk->addDgtr(211, -10323)->addDgtr(-211, 310, 111);
  // add(*dk);
  // if(dbThis) cout << "Decay #" << decayCtr++ << endl << *dk << endl;
  // delete dk;

  //D->K1(1270)-pi+, K1(1270)->K0*(1430)pi-
  dk = new DecayTree(421);
  dk->addDgtr(211, -10323)->addDgtr(-211, -10311)->addDgtr(310, 111);
  add(*dk);
  if(dbThis) cout << "Decay #" << decayCtr++ << endl << *dk << endl;
  delete dk;

  //D->K1(1270)-pi+, K1(1270)->K0*(1430)-pi0
  dk = new DecayTree(421);
  dk->addDgtr(211, -10323)->addDgtr(111, -10321)->addDgtr(310, -211);
  add(*dk);
  if(dbThis) cout << "Decay #" << decayCtr++ << endl << *dk << endl;
  delete dk;

  //  name="D->K1(1270)-pi+, K1(1270)->K0*(892)pi-";
  dk = new DecayTree(421);
  dk->addDgtr(211, -10323)->addDgtr(-211, -313)->addDgtr(310, 111);
  add(*dk);
  if(dbThis) cout << "Decay #" << decayCtr++ << endl << *dk << endl;
  delete dk;

  //  name="D->K1(1270)-pi+, K1(1270)->K*(892)- pi0";
  dk = new DecayTree(421);
  dk->addDgtr(211, -10323)->addDgtr(111, -323)->addDgtr(310, -211);
  add(*dk);
  if(dbThis) cout << "Decay #" << decayCtr++ << endl << *dk << endl;
  delete dk;

  //  name="D->K1(1270)-pi+, K1(1270)->rho-K0";
  dk = new DecayTree(421);
  dk->addDgtr(211, -10323)->addDgtr(310, -213)->addDgtr(-211, 111);
  add(*dk);
  if(dbThis) cout << "Decay #" << decayCtr++ << endl << *dk << endl;
  delete dk;

// K*0bar pi+ pi-
  dk = new DecayTree(421);
  dk->addDgtr(211);
  dk->addDgtr(-211,-313)->addDgtr(310, 111);
  add(*dk);
  if(dbThis) cout << "Decay #" << decayCtr++ << endl << *dk << endl;
  delete dk;

  return _trees.size();

}
//=========================================================================
// create the decay of the type D0-> pi pi pi pi
//=========================================================================
int NamedDecayTreeList::make4PiList() {
// T.McKetterick
bool dbThis=false;
 
DecayTree* dk=0;

//D0 -> a1(1260)pi-, a1(1260)->rho(770)pi+, rho->pi+pi-   [S-WAVE]
  dk = new DecayTree(421);
  dk->addDgtr(-211, 20213)->addDgtr(211, 113)->addDgtr(211, -211);
  add(*dk);
  if(dbThis) cout << "first decay: " << *dk << endl;
  delete dk;

//D0 -> a1(1260)pi-, a1(1260)->rho(770)pi+, rho->pi+pi-   [D-WAVE decay of a1]
  dk = new DecayTree(421);
  DecayTree* a1 = new DecayTree(20213);
  a1->getVal().setL(2);
  a1->addDgtr(211, 113)->addDgtr(211, -211);
  dk->addDgtr(-211), dk->addDgtr(a1);
  add(*dk);
  if(dbThis) cout << "first decay: " << *dk << endl;
  delete a1;
  delete dk;

//This has been commented out because it doesn't make
//sense that there is an overall spin 2 decaying to an overall spin1
//D0 -> a1(1260)pi-, a1(1260)->rho(770)pi+, rho->pi+pi-   [D-WAVE]
//  dk = new DecayTree(421);
//  dk->addDgtr(-211, 20213)->addDgtr(211,113)->addDgtr(211, -211);
//  dk->getVal().setL(2);
//  add(*dk);
//  if(dbThis) cout << "first decay: " << *dk << endl;
//  delete dk;

//D0 -> a1(1260)pi-, a1(1260)->sigma_1 pi+, rho->pi+pi- 
  dk = new DecayTree(421);
  dk->addDgtr(-211, 20213)->addDgtr(211,999001)->addDgtr(211, -211);
  add(*dk);
  if(dbThis) cout << "second decay: " << *dk << endl;
  delete dk;

//D0 -> rho(770)rho(770), rho(770)->pi+pi-  [S-WAVE]
  dk = new DecayTree(421);
  dk->addDgtr(113)->addDgtr(211, -211);
  dk->addDgtr(113)->addDgtr(211, -211);
  add(*dk);
  if(dbThis) cout << "third decay: " << *dk << endl;
  delete dk;

//  //Dbar0 -> rho(770)rho(770), rho(770)->pi+pi-  [S-WAVE]
//
//  dk = new DecayTree(-421);
//  dk->addDgtr(113)->addDgtr(211, -211);
//  dk->addDgtr(113)->addDgtr(211, -211);
//  add(*dk);
//  if(dbThis) cout << "third decay: " << *dk << endl;
//  delete dk;


//D0 -> rho(770)rho(770), rho(770)->pi+pi-  [P-WAVE]
  dk = new DecayTree(421);
  dk->addDgtr(113)->addDgtr(211, -211);
  dk->addDgtr(113)->addDgtr(211, -211);
  dk->getVal().setL(1);
  add(*dk);
  if(dbThis) cout << "third decay: " << *dk << endl;
  delete dk;

//D0 -> rho(770)rho(770), rho(770)->pi+pi-  [D-WAVE]
  dk = new DecayTree(421);
  dk->addDgtr(113)->addDgtr(211, -211);
  dk->addDgtr(113)->addDgtr(211, -211);
  dk->getVal().setL(2);
  add(*dk);
  if(dbThis) cout << "third decay: " << *dk << endl;
  delete dk;

//D0 -> f0(980)pi-pi+, f0(980)->pi+pi-  
//Adding no resonant decays to help with the spin factors
  dk = new DecayTree(421);
  dk->addDgtr(9010221)->addDgtr(211, -211);
  dk->addDgtr(9993)->addDgtr(211, -211);
  add(*dk);
  if(dbThis) cout << "fourth decay: " << *dk << endl;
  delete dk;

//D0 -> f2(1270)pi-pi+, f2(1270)->pi+pi-
//Adding no resonant decays to help with the spin factors  
  dk = new DecayTree(421);
  dk->addDgtr(225)->addDgtr(211, -211);
  dk->addDgtr(9993)->addDgtr(211, -211);
  add(*dk);
  if(dbThis) cout << "fifth decay: " << *dk << endl;
  delete dk;

//D0 -> sigma_1 pi-pi+, sigma_1->pi+pi-  
  dk = new DecayTree(421);
  dk->addDgtr(999001)->addDgtr(211, -211);
  dk->addDgtr(211, -211);
  add(*dk);
  if(dbThis) cout << "sixth decay: " << *dk << endl;
  delete dk;

return _trees.size();
}

std::ostream& operator<<(std::ostream& os, const NamedDecayTreeList& ndtl){
  ndtl.print(os);
  return os;
}

//

