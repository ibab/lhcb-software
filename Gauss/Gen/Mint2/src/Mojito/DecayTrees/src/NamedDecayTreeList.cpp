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
  bool dbThis=false;

  bool isBg=A_is_in_B("BgSpinZero",opt);
  bool isCLEO2012=A_is_in_B("CLEO2012",opt);

  addSimple(ai, opt);
  if(! isBg)addSimple(ai, opt + "BgSpinZero");     
  // if it already is a decay with the BgSpinZero option, then there's not point in adding it again.

    
  if(! isCLEO2012)addSimple(ai, opt + "CLEO2012");
  // if it already is a decay with the CLEO2012 option, then there's not point in adding it again.
  if(dbThis) cout << "NamedDecayTreeList::add: just added D->f\t" 
		  << ai.tree().oneLiner() << endl;

  DecayTree dt = ai.tree();
    
  if(dt.finalState().size()==4 && (dt.getDgtrTreePtr(0)->finalState().size()==3 || dt.getDgtrTreePtr(1)->finalState().size()==3) )
  {
      addSimple(ai, "SBW_" + opt);    
      addSimple(ai, "FermiPS_" + opt); 
      addSimple(ai, "HistoPS_" + opt);
  }  
    
  anti(dt);   // CP conjugate
  AmpInitialiser CPai(ai);
  CPai.setTree(dt);
  addSimple(CPai, opt);
  if(! isBg) addSimple(CPai, opt + "BgSpinZero");
  if(! isCLEO2012)addSimple(CPai, opt + "CLEO2012");
  if(dt.finalState().size()==4 && (dt.getDgtrTreePtr(0)->finalState().size()==3 || dt.getDgtrTreePtr(1)->finalState().size()==3) )
  {    
      addSimple(CPai, "SBW_" + opt);    
      addSimple(CPai, "FermiPS_" + opt); 
      addSimple(CPai, "HistoPS_" + opt);
  }
  if(dbThis) cout << "NamedDecayTreeList::add: just added Dbar->fbar\t" 
		  << dt.oneLiner() << endl;

  if("0" == dt.getVal().charge()){
    dt.getVal().antiThis(); // mum back to D0
    AmpInitialiser DtoCPai(ai);
    DtoCPai.setTree(dt);
    addSimple(DtoCPai, opt);
    if(! isBg) addSimple(DtoCPai, opt + "BgSpinZero");
    if(! isCLEO2012)addSimple(DtoCPai, opt + "CLEO2012");
    if(dt.finalState().size()==4 && (dt.getDgtrTreePtr(0)->finalState().size()==3 || dt.getDgtrTreePtr(1)->finalState().size()==3))
      { 
	addSimple(DtoCPai, "SBW_" + opt);    
	addSimple(DtoCPai, "FermiPS_" + opt); 
	addSimple(DtoCPai, "HistoPS_" + opt);
      }
    if(dbThis) cout << "NamedDecayTreeList::add: just added D->fbar\t" 
		    << dt.oneLiner() << endl;
    
    anti(dt); // and the CP conjugate of that, i.e. Dbar->original
    AmpInitialiser DbarToOriginal(ai);
    DbarToOriginal.setTree(dt);
    addSimple(DbarToOriginal, opt);
    if(! isBg) addSimple(DbarToOriginal, opt + "BgSpinZero");
    if(! isCLEO2012)addSimple(DbarToOriginal, opt + "CLEO2012");
    if(dt.finalState().size()==4 && (dt.getDgtrTreePtr(0)->finalState().size()==3 || dt.getDgtrTreePtr(1)->finalState().size()==3))
      {    
	addSimple(DbarToOriginal, "SBW_" + opt);    
	addSimple(DbarToOriginal, "FermiPS_" + opt); 
	addSimple(DbarToOriginal, "HistoPS_" + opt);
      }
    if(dbThis) cout << "NamedDecayTreeList::add: just added Dbar->f\t" 
		    << dt.oneLiner() << endl;
    
  }
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
  if(A_is_in_B("FermiPS_",opt)){
        nai.addLopt("FermiPS_");
  }  
  if(A_is_in_B("HistoPS_",opt)){
        nai.addLopt("HistoPS_");
  }  
  if(A_is_in_B("SBW_",opt)){
        nai.addLopt("SBW_");
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
  makeB02DDKList();
  makeBp2DDKList();  
  return _trees.size();
}

int NamedDecayTreeList::make4BodyList(){
  makeKpipiGammaList();
  makeJpsiKpipiList();
  makePsiKpipiList();
  makeKKpipiList();
  makeK3piList();
  makeKsPiPiPizeroList();
  make4PiList();

  return _trees.size();
}


// ========================= 3 Body Decays ======================
int NamedDecayTreeList::makeB02DDKList(){
    
    //B0->D- D0 K+
    DecayTree* dk=0;
    
    // B0->D- (D_s1(2536)->D0 K+)  
    dk = new DecayTree(511);
    dk->addDgtr(-411, 10433)->addDgtr(421, 321);
    this->add(*dk);
    delete dk;
    
    // B0->D- (D_s2*(2573)->D0 K+)  
    dk = new DecayTree(511);
    dk->addDgtr(-411, 435 )->addDgtr(421, 321);
    this->add(*dk);
    delete dk;
    
    // B0->D- (D_s1*(2700)->D0 K+)  
    dk = new DecayTree(511);
    dk->addDgtr(-411, 9999915)->addDgtr(421, 321);
    this->add(*dk);
    delete dk;
    
    // B0->D- (D_sJ*(2860)->D0 K+)  
    dk = new DecayTree(511);
    dk->addDgtr(-411,9999916 )->addDgtr(421, 321);
    this->add(*dk);
    delete dk;
    
    dk = new DecayTree(511);
    dk->addDgtr(-411,9999917 )->addDgtr(421, 321);
    this->add(*dk);
    delete dk;
    
    // B0->D- (D_sJ*(3040)->D0 K+)  
    dk = new DecayTree(511);
    dk->addDgtr(-411,9999918 )->addDgtr(421, 321);
    this->add(*dk);
    delete dk;
    
    dk = new DecayTree(511);
    dk->addDgtr(-411,9999919 )->addDgtr(421, 321);
    this->add(*dk);
    delete dk;
    
    //non resonant
    dk = new DecayTree(511);
    dk->addDgtr(-411, 9993)->addDgtr(421, 321);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp"));
    delete dk;
    
    dk = new DecayTree(511);
    dk->addDgtr(421, 9993)->addDgtr(-411, 321);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp"));
    delete dk;
    
    dk = new DecayTree(511);
    dk->addDgtr(321, 9993)->addDgtr(421, -411);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp"));
    delete dk;
    
    return _trees.size();
}

int NamedDecayTreeList::makeBp2DDKList(){
    
    //B+->D0_bar D0 K+
    DecayTree* dk=0;
    
    // B+->D0_bar (D_s1(2536)->D0 K+)  
    dk = new DecayTree(521);
    dk->addDgtr(-421, 10433)->addDgtr(421, 321);
    this->add(*dk);
    delete dk;
    
    // B+->D0_bar (D_s2*(2573)->D0 K+)  
    dk = new DecayTree(521);
    dk->addDgtr(-421, 435)->addDgtr(421, 321);
    this->add(*dk);
    delete dk;
    
    // B+->D0_bar (D_s1*(2700)->D0 K+)  
    dk = new DecayTree(521);
    dk->addDgtr(-421, 9999915)->addDgtr(421, 321);
    this->add(*dk);
    delete dk;
    
    // B+->D0_bar (D_sJ*(2860)->D0 K+)  
    dk = new DecayTree(521);
    dk->addDgtr(-421,9999916 )->addDgtr(421, 321);
    this->add(*dk);
    delete dk;
    
    dk = new DecayTree(521);
    dk->addDgtr(-421,9999917 )->addDgtr(421, 321);
    this->add(*dk);
    delete dk;
    
    // B+->D0_bar (D_sJ*(3040)->D0 K+)  
    dk = new DecayTree(521);
    dk->addDgtr(-421,9999918 )->addDgtr(421, 321);
    this->add(*dk);
    delete dk;
    
    dk = new DecayTree(521);
    dk->addDgtr(-421,9999919 )->addDgtr(421, 321);
    this->add(*dk);
    delete dk;
    
    // B+->K+ (psi(3770)-> D D)  
    dk = new DecayTree(521);
    dk->addDgtr(321, 30443)->addDgtr(421, -421);
    this->add(*dk);
    delete dk;
    
    // B+->K+ (chi_c2-> D D)  
    dk = new DecayTree(521);
    dk->addDgtr(321, 100445)->addDgtr(421, -421);
    this->add(*dk);
    delete dk;
    
    // B+->K+ (psi(4040)-> D D)  
    dk = new DecayTree(521);
    dk->addDgtr(321, 9000443)->addDgtr(421, -421);
    this->add(*dk);
    delete dk;
    
    // B+->K+ (psi(4160)-> D D)  
    dk = new DecayTree(521);
    dk->addDgtr(321, 9010443)->addDgtr(421, -421);
    this->add(*dk);
    delete dk;
    
    // B+->K+ (psi(4415)-> D D)  
    dk = new DecayTree(521);
    dk->addDgtr(321,  9020443)->addDgtr(421, -421);
    this->add(*dk);
    delete dk;
    
    //non resonant
    dk = new DecayTree(521);
    dk->addDgtr(-421, 9993)->addDgtr(421, 321);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp"));
    delete dk;

    return _trees.size();
}


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

  // non-resonant S-wave
  dk = new DecayTree(411);
  dk->addDgtr(321, 9981)->addDgtr(-321, 211);
  this->add(*dk);
  this->add(AmpInitialiser(*dk, "TopHats"));
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

  // --

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
int NamedDecayTreeList::makePsiKpipiList(){
    
    DecayTree* dk=0;
    
    // B->psi K1(1270); K1->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 10323)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_10323_"));  
    dk->getVal().setL(1);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_10323_"));  
    dk->getVal().setL(2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_10323_"));  
    delete dk;
    
    dk = new DecayTree(521);
    DecayTree k1(10323);
    k1.getVal().setL(2);
    k1.addDgtr(211, 313)->addDgtr(321,-211);
    dk->addDgtr(100443);
    dk->addDgtr(&k1);
    this->add(*dk); 
    this->add(AmpInitialiser(*dk, "SBW_10323_"));   
    delete dk;

    // B->psi K1(1270); K1->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 10323)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "SBW_10323_GS"));
    this->add(AmpInitialiser(*dk, "SBW_10323_RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "VBW_10323_"));
    this->add(AmpInitialiser(*dk, "VBW_10323_GS"));
    this->add(AmpInitialiser(*dk, "VBW_10323_RHO_OMEGA"));
    dk->getVal().setL(1);
    this->add(*dk); 
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "SBW_10323_GS"));
    this->add(AmpInitialiser(*dk, "SBW_10323_RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "VBW_10323_"));
    this->add(AmpInitialiser(*dk, "VBW_10323_GS"));
    this->add(AmpInitialiser(*dk, "VBW_10323_RHO_OMEGA"));
    dk->getVal().setL(2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "SBW_10323_GS"));
    this->add(AmpInitialiser(*dk, "SBW_10323_RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "VBW_10323_"));
    this->add(AmpInitialiser(*dk, "VBW_10323_GS"));
    this->add(AmpInitialiser(*dk, "VBW_10323_RHO_OMEGA"));
    delete dk;
    
    // B->psi K1(1270); K1->K omega; omega->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 10323)->addDgtr(321, 223)->addDgtr(211,-211);
    this->add(*dk); 
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    delete dk;

    // B->psi K1(1270); K1-> K0^*(1430) pi; K0^*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 10323)->addDgtr(211, 10311)->addDgtr(321,-211);
    this->add(AmpInitialiser(*dk, "Lass"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "VBW_10323_"));
    delete dk;
    
    // B->psi K1(1400); K1->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 20323)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_20323_"));
    delete dk;

    // B->psi K1(1400); K1->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 20323)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "VBW_20323_"));
    this->add(AmpInitialiser(*dk, "VBW_20323_GS"));
    this->add(AmpInitialiser(*dk, "VBW_20323_RHO_OMEGA"));
    delete dk;

    // B->psi K*(1410); K*(1410)->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 100323)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_100323_"));   
    delete dk;
    
    // B->psi K*(1410); K*->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 100323)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_100323_GS"));   
    this->add(AmpInitialiser(*dk, "SBW_100323_"));  
    this->add(AmpInitialiser(*dk, "SBW_100323_RHO_OMEGA"));  
    this->add(AmpInitialiser(*dk, "VBW_100323_GS"));   
    this->add(AmpInitialiser(*dk, "VBW_100323_"));  
    this->add(AmpInitialiser(*dk, "VBW_100323_RHO_OMEGA"));  
    delete dk;
    
    // B->psi K2*(1430); K2->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 325)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);   
    this->add(AmpInitialiser(*dk, "SBW_325_"));
    delete dk;
    
    // B->psi K2*(1430); K2->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 325)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_325_"));
    this->add(AmpInitialiser(*dk, "SBW_325_GS"));
    this->add(AmpInitialiser(*dk, "SBW_325_RHO_OMEGA"));
    delete dk;
    
    // B->psi K(1460); K->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 100321)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);   
    this->add(AmpInitialiser(*dk, "SBW_100321_"));
    delete dk;
    
    // B->psi K(1460); K->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 100321)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_100321_"));
    this->add(AmpInitialiser(*dk, "SBW_100321_GS"));
    this->add(AmpInitialiser(*dk, "SBW_100321_RHO_OMEGA"));
    delete dk;
    
    // B->psi K(1460); K->K f0(980); f0->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 100321)->addDgtr(321, 9010221)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "Flatte"));
    this->add(*dk); 
    this->add(AmpInitialiser(*dk, "SBW_100321_"));    
    this->add(AmpInitialiser(*dk, "SBW_100321_Flatte"));    
    delete dk;
    
    // B->psi K(1460); K-> K0^*(1430) pi; K0^*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 100321)->addDgtr(211, 10311)->addDgtr(321,-211);
    this->add(AmpInitialiser(*dk, "Lass"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_100321_"));
    this->add(AmpInitialiser(*dk, "VBW_100321_"));
    delete dk;

    // B->psi K2(1580); K2->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 9000325)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);   
    this->add(AmpInitialiser(*dk, "SBW_9000325_"));
    delete dk;

    // B->psi K2(1580); K2->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 9000325)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_9000325_"));
    this->add(AmpInitialiser(*dk, "SBW_9000325_GS"));
    delete dk;

    // B->psi K2(1580); K2->K f0(980); f2->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 9000325)->addDgtr(321, 9010221)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "Flatte"));
    this->add(*dk); 
    this->add(AmpInitialiser(*dk, "SBW_9000325_"));    
    this->add(AmpInitialiser(*dk, "SBW_9000325_Flatte"));    
    delete dk;
    
    // B->psi K2(1580); K2->K f2(1270); f2->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 9000325)->addDgtr(321, 225)->addDgtr(211,-211);
    this->add(*dk);    
    this->add(AmpInitialiser(*dk, "SBW_"));   
    this->add(AmpInitialiser(*dk, "SBW_9000325_"));    
    delete dk;
    
    // B->psi K2(1580); K2->K2*(1430) pi; K2*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 9000325)->addDgtr(211, 315)->addDgtr(321,-211);
    this->add(*dk); 
    this->add(AmpInitialiser(*dk, "SBW_"));   
    this->add(AmpInitialiser(*dk, "SBW_9000325_"));    
    delete dk;

    // B->psi K1*(1680); K2->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 30323)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk); 
    this->add(AmpInitialiser(*dk, "SBW_30323_"));
    delete dk;
    
    // B->psi  K1*(1680); K2->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 30323)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_30323_"));
    this->add(AmpInitialiser(*dk, "SBW_30323_GS"));
    this->add(AmpInitialiser(*dk, "SBW_30323_RHO_OMEGA"));
    delete dk;

    // B->psi K2(1770); K2->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 10325)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_10325_"));
    delete dk;

    // B->psi  K2(1770); K2->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 10325)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);  
    this->add(AmpInitialiser(*dk, "SBW_10325_"));
    this->add(AmpInitialiser(*dk, "SBW_10325_GS"));
    delete dk;
    
    // B->psi K2(1770); K2->K f0(980); f2->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 10325)->addDgtr(321, 9010221)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "Flatte"));
    this->add(*dk); 
    this->add(AmpInitialiser(*dk, "SBW_10325_"));    
    this->add(AmpInitialiser(*dk, "SBW_10325_Flatte"));    
    delete dk;
    
    // B->psi K2(1770); K2->K f2(1270); f2->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 10325)->addDgtr(321, 225)->addDgtr(211,-211);
    this->add(*dk);    
    this->add(AmpInitialiser(*dk, "SBW_"));   
    this->add(AmpInitialiser(*dk, "SBW_10325_"));    
    delete dk;
    
    // B->psi K2(1770); K2->K2*(1430) pi; K2*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 10325)->addDgtr(211, 315)->addDgtr(321,-211);
    this->add(*dk); 
    this->add(AmpInitialiser(*dk, "SBW_"));   
    this->add(AmpInitialiser(*dk, "SBW_10325_"));    
    delete dk;
    
    //Charmonia amplitudes:
    //------------------------
    
    // B->X(3872) K; X-> psi rho
    dk = new DecayTree(521);
    dk->addDgtr(321, 9999991)->addDgtr(100443, 113)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_9999991_"));   
    this->add(AmpInitialiser(*dk, "SBW_9999991_GS"));  
    this->add(AmpInitialiser(*dk, "GS"));  
    delete dk;

    // B->psi(4160) K; psi-> psi pi pi 
    dk = new DecayTree(521);
    dk->addDgtr(321, 9010443)->addDgtr(100443, 9981)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_9010443_"));  
    this->add(AmpInitialiser(*dk, "3Body"));   
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(321, 9010443)->addDgtr(100443, 9993)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_9010443_")); 
    delete dk;
    
    // B->X(4260) K; X-> psi pi pi 
    dk = new DecayTree(521);
    dk->addDgtr(321, 9999992)->addDgtr(100443, 9981)->addDgtr(211,-211);
    this->add(*dk);  
    this->add(AmpInitialiser(*dk, "3Body"));   
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(321, 9999992)->addDgtr(100443, 9993)->addDgtr(211,-211);
    this->add(*dk); 
    delete dk;

    // B->X(4260) K; X-> psi rho
    dk = new DecayTree(521);
    dk->addDgtr(321, 9999992)->addDgtr(100443, 113)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_9999992_"));   
    this->add(AmpInitialiser(*dk, "SBW_9999992_GS"));  
    this->add(AmpInitialiser(*dk, "GS"));   
    this->add(AmpInitialiser(*dk, "LS_10_SBW_9999992_"));   
    this->add(AmpInitialiser(*dk, "LS_10_SBW_9999992_GS"));  
    this->add(AmpInitialiser(*dk, "LS_10_GS")); 
    this->add(AmpInitialiser(*dk, "LS_10_"));  
    this->add(AmpInitialiser(*dk, "LS_11_SBW_9999992_"));   
    this->add(AmpInitialiser(*dk, "LS_11_SBW_9999992_GS"));  
    this->add(AmpInitialiser(*dk, "LS_11_GS")); 
    this->add(AmpInitialiser(*dk, "LS_11_")); 
    this->add(AmpInitialiser(*dk, "LS_12_SBW_9999992_"));   
    this->add(AmpInitialiser(*dk, "LS_12_SBW_9999992_GS"));  
    this->add(AmpInitialiser(*dk, "LS_12_GS")); 
    this->add(AmpInitialiser(*dk, "LS_12_"));  
    delete dk;

    // B->K X(4260); X->f0 psi; f0->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(321, 9999992)->addDgtr(100443, 9010221)->addDgtr(211,-211);
    this->add(*dk); 
    this->add(AmpInitialiser(*dk, "SBW_9999992_")); 
    this->add(AmpInitialiser(*dk, "Flatte")); 
    this->add(AmpInitialiser(*dk, "SBW_9999992_Flatte")); 
    delete dk;
    
    dk = new DecayTree(521);
    dk->addDgtr(321, 9999992)->addDgtr(100443, 9000221)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "Bugg"));
    delete dk;
    
    // B->X(4660) K; X-> psi pi pi 
    dk = new DecayTree(521);
    dk->addDgtr(321, 9999914)->addDgtr(100443, 9981)->addDgtr(211,-211);
    this->add(*dk);  
    this->add(AmpInitialiser(*dk, "3Body"));   
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(321, 9999914)->addDgtr(100443, 9993)->addDgtr(211,-211);
    this->add(*dk); 
    delete dk;
    
    // B->K X(4260); X->f0 psi; f0->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(321, 9999992)->addDgtr(100443, 9010221)->addDgtr(211,-211);
    this->add(*dk);  
    this->add(AmpInitialiser(*dk, "SBW_9999992_")); 
    this->add(AmpInitialiser(*dk, "Flatte")); 
    this->add(AmpInitialiser(*dk, "SBW_9999992_Flatte")); 
    delete dk;
    
    //Exotics:
    //------------------------
    
    //B->K* Z(3900)V
    dk = new DecayTree(521);
    DecayTree Ks(313);
    Ks.addDgtr(321,-211);
    DecayTree Z(9999994);
    Z.addDgtr(100443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    dk = new DecayTree(521);
    Ks=DecayTree(9993);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999994);
    Z.addDgtr(100443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    //B->K* Z(3900)A
    dk = new DecayTree(521);
    Ks=DecayTree(313);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999993);
    Z.addDgtr(100443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    dk = new DecayTree(521);
    Ks=DecayTree(9993);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999993);
    Z.addDgtr(100443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    
    //B->K* Z(3900)P
    dk = new DecayTree(521);
    Ks=DecayTree(313);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999995);
    Z.addDgtr(100443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    //B->K* Z(4051)V
    dk = new DecayTree(521);
    Ks=DecayTree(313);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999912);
    Z.addDgtr(100443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    dk = new DecayTree(521);
    Ks=DecayTree(9993);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999912);
    Z.addDgtr(100443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    //B->K* Z(4051)A
    dk = new DecayTree(521);
    Ks=DecayTree(313);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999911);
    Z.addDgtr(100443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    dk = new DecayTree(521);
    Ks=DecayTree(9993);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999911);
    Z.addDgtr(100443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    //B->K* Z(4051)P
    dk = new DecayTree(521);
    Ks=DecayTree(313);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999913);
    Z.addDgtr(100443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    dk = new DecayTree(521);
    Ks=DecayTree(9993);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999913);
    Z.addDgtr(100443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    //B->K* Z(4239)
    dk = new DecayTree(521);
    Ks=DecayTree(313);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999996);
    Z.addDgtr(100443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    
    dk = new DecayTree(521);
    Ks=DecayTree(9993);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999996);
    Z.addDgtr(100443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    //B->K* Z(4430)
    dk = new DecayTree(521);
    Ks=DecayTree(313);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999997);
    Z.addDgtr(100443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    dk = new DecayTree(521);
    Ks=DecayTree(9993);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999997);
    Z.addDgtr(100443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    // B->K X(4260); X->Z pi; Z->psi pi  
    dk = new DecayTree(521);
    dk->addDgtr(321, 9999992)->addDgtr(211, 9999993)->addDgtr(100443,-211);
    this->add(*dk); 
    this->add(AmpInitialiser(*dk, "SBW_9999992_")); 
    this->add(AmpInitialiser(*dk, "SBW_9999992_VBW_9999993_")); 
    this->add(AmpInitialiser(*dk, "VBW_9999993_")); 
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(321, 9999992)->addDgtr(-211, 9999993)->addDgtr(100443,211);
    this->add(*dk); 
    this->add(AmpInitialiser(*dk, "SBW_9999992_")); 
    this->add(AmpInitialiser(*dk, "SBW_9999992_VBW_9999993_")); 
    this->add(AmpInitialiser(*dk, "VBW_9999993_")); 
    delete dk;
    
    dk = new DecayTree(521);
    dk->addDgtr(321, 9993)->addDgtr(211, 9999993)->addDgtr(100443,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_9999992_")); 
    this->add(AmpInitialiser(*dk, "SBW_9999992_VBW_9999993_")); 
    this->add(AmpInitialiser(*dk, "VBW_9999993_")); 
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(321, 9993)->addDgtr(-211, 9999993)->addDgtr(100443,211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_9999992_")); 
    this->add(AmpInitialiser(*dk, "SBW_9999992_VBW_9999993_")); 
    this->add(AmpInitialiser(*dk, "VBW_9999993_")); 
    delete dk;

    //B->rho Z(3900)V
    dk = new DecayTree(521);
    DecayTree rho(113);
    rho.addDgtr(211,-211);
    Z= DecayTree(9999994);
    Z.addDgtr(100443,321);
    dk->addDgtr(&rho);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;

    //B->rho Z(3900)A
    dk = new DecayTree(521);
    rho=DecayTree(113);
    rho.addDgtr(211,-211);
    Z=DecayTree(9999993);
    Z.addDgtr(100443,321);
    dk->addDgtr(&rho);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    //B->rho Z(3900)P
    dk = new DecayTree(521);
    rho=DecayTree(113);
    rho.addDgtr(211,-211);
    Z=DecayTree(9999995);
    Z.addDgtr(100443,321);
    dk->addDgtr(&rho);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    //B->rho Z(4051)V
    dk = new DecayTree(521);
    rho=DecayTree(113);
    rho.addDgtr(211,-211);
    Z=DecayTree(9999912);
    Z.addDgtr(100443,321);
    dk->addDgtr(&rho);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    //B->rho Z(4051)A
    dk = new DecayTree(521);
    rho=DecayTree(113);
    rho.addDgtr(211,-211);
    Z=DecayTree(9999911);
    Z.addDgtr(100443,211);
    dk->addDgtr(&rho);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    //B->rho Z(4051)P
    dk = new DecayTree(521);
    rho=DecayTree(113);
    rho.addDgtr(211,-211);
    Z=DecayTree(9999913);
    Z.addDgtr(100443,211);
    dk->addDgtr(&rho);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    // B->pi+ X(4260); X->Z+ pi-; Z+->psi K+  
    dk = new DecayTree(521);
    dk->addDgtr(211, 9999992)->addDgtr(-211, 9999993)->addDgtr(100443,321);
    this->add(*dk); 
    this->add(AmpInitialiser(*dk, "SBW_9999992_")); 
    delete dk;

    // B->pi- X(4260); X->Z- K+; Z-->psi pi-  
    dk = new DecayTree(521);
    dk->addDgtr(-211, 9999992)->addDgtr(321, 9999993)->addDgtr(100443,-211);
    this->add(*dk); 
    this->add(AmpInitialiser(*dk, "SBW_9999992_")); 
    delete dk;

    // B->pi+ X(4660); X->Z+ pi-; Z+->psi K+  
    dk = new DecayTree(521);
    dk->addDgtr(211, 9999914)->addDgtr(-211, 9999993)->addDgtr(100443,321);
    this->add(*dk); 
    this->add(AmpInitialiser(*dk, "SBW_9999914_")); 
    delete dk;

    // B->pi- X(4660); X->Z- K+; Z-->psi pi-  
    dk = new DecayTree(521);
    dk->addDgtr(-211, 9999914)->addDgtr(321, 9999993)->addDgtr(100443,-211);
    this->add(*dk); 
    this->add(AmpInitialiser(*dk, "SBW_9999914_")); 
    delete dk;

    // B->K X(4260); X->f0 psi; f0->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(321, 9999992)->addDgtr(100443, 9010221)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_9999992_")); 
    this->add(AmpInitialiser(*dk, "Flatte")); 
    this->add(AmpInitialiser(*dk, "SBW_9999992_Flatte")); 
    delete dk;

    // B->pi X(4260); X->K0* psi; K0*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(211, 9999992)->addDgtr(100443, 10311)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_9999992_")); 
    delete dk;

    // B->K X(4660); X->f0 psi; f0->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(321, 9999914)->addDgtr(100443, 9010221)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_9999992_")); 
    this->add(AmpInitialiser(*dk, "Flatte")); 
    this->add(AmpInitialiser(*dk, "SBW_9999992_Flatte")); 
    delete dk;

    // B->pi X(4660); X->K0* psi; K0*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(211, 9999914)->addDgtr(100443, 10311)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_9999992_")); 
    delete dk;

    // B->X(3872) K; X-> psi rho
    dk = new DecayTree(521);
    dk->addDgtr(321, 9999991)->addDgtr(100443, 113)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_9999991_"));   
    this->add(AmpInitialiser(*dk, "SBW_9999991_GS"));  
    this->add(AmpInitialiser(*dk, "GS"));  
    delete dk;
    
    //Single resonance amplitudes:
    //----------------------------
    
    // B->psi K1(1270); K1->K pi pi (non resonant) 
    dk = new DecayTree(521);
    dk->addDgtr(100443, 10323)->addDgtr(211, 9993)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_10323"));
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(100443, 10323)->addDgtr(321, 9993)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_10323"));
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(100443, 10323)->addDgtr(-211, 9993)->addDgtr(321,211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_10323"));
    delete dk;
    
    // B->psi K1(1270); K1->K pi pi (non resonant)
    dk = new DecayTree(521);
    dk->addDgtr(100443, 10323)->addDgtr(211,  9981)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "SBW_10323_NonRes"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_10323"));
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(100443, 10323)->addDgtr(321,  9981)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "SBW_10323_NonRes"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_10323"));
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(100443, 10323)->addDgtr(-211,  9981)->addDgtr(321,211);
    this->add(*dk);  
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "SBW_10323_NonRes"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_10323"));
    delete dk;
    
    // B->psi pi K*(892); K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 9983)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    
    // B->psi K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(100443, 9983)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    this->add(AmpInitialiser(*dk, "NonResExp_9983GS"));
    dk->getVal().setL(1);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    this->add(AmpInitialiser(*dk, "NonResExp_9983GS"));
    delete dk;
    
    dk = new DecayTree(521);
    dk->addDgtr(100443, 9993)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_9993"));
    this->add(AmpInitialiser(*dk, "NonResExp_9993GS"));
    delete dk;
    
    dk = new DecayTree(521);
    dk->addDgtr(321, 9993)->addDgtr(100443, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_9993"));
    this->add(AmpInitialiser(*dk, "NonResExp_9993GS"));
    delete dk;
    
    dk = new DecayTree(521);
    dk->addDgtr(321, 9983)->addDgtr(100443, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    this->add(AmpInitialiser(*dk, "NonResExp_9983GS"));
    delete dk;
    
    //B-> NV rho
    dk = new DecayTree(521);
    DecayTree r1(9993);
    r1.addDgtr(100443,321);
    DecayTree r2(113);
    r2.addDgtr(211,-211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResGS"));
    this->add(AmpInitialiser(*dk, "NonResExp_9993"));
    this->add(AmpInitialiser(*dk, "NonResExp_9993GS"));
    delete dk;
    
    //B-> NA rho
    dk = new DecayTree(521);
    r1=DecayTree(9983);
    r1.addDgtr(100443,321);
    r2=DecayTree(113);
    r2.addDgtr(211,-211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResGS"));
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    this->add(AmpInitialiser(*dk, "NonResExp_9983GS"));
    delete dk;
    
    //B-> NP rho
    dk = new DecayTree(521);
    r1=DecayTree(9991);
    r1.addDgtr(100443,321);
    r2=DecayTree(113);
    r2.addDgtr(211,-211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResGS"));
    this->add(AmpInitialiser(*dk, "NonResExp_9991"));
    this->add(AmpInitialiser(*dk, "NonResExp_9991GS"));
    delete dk;

    //Non resonant amplitudes:
    //------------------------
    
    // B->psi (K pi pi)_S
    dk = new DecayTree(521);
    dk->addDgtr(100443, 9991)->addDgtr(321,9981)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    
    //B-> psi NA ; NA-> NV P
    dk = new DecayTree(521);
    dk->addDgtr(100443, 9983)->addDgtr(321, 9993)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp"));
    this->add(*dk);
    dk->getVal().setL(1);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp"));
    this->add(*dk);
    
    dk = new DecayTree(521);
    dk->addDgtr(100443, 9983)->addDgtr(211, 9993)->addDgtr(321,-211);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(*dk);
    dk->getVal().setL(1);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(*dk);
    
    dk = new DecayTree(521);
    dk->addDgtr(100443, 9983)->addDgtr(-211, 9993)->addDgtr(321,211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    dk->getVal().setL(1);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));    
    delete dk;
    
    //B-> psi NV ; NV-> NV P
    dk = new DecayTree(521);
    dk->addDgtr(100443, 9993)->addDgtr(321, 9993)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(*dk);
    dk->getVal().setL(1);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(*dk);
    
    dk = new DecayTree(521);
    dk->addDgtr(100443, 9993)->addDgtr(211, 9993)->addDgtr(321,-211);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(*dk);
    dk->getVal().setL(1);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(*dk);
    
    dk = new DecayTree(521);
    dk->addDgtr(100443, 9993)->addDgtr(-211, 9993)->addDgtr(321,211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    dk->getVal().setL(1);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    
    delete dk;
    
    //B-> P NV ; NV-> Jpsi NS
    dk = new DecayTree(521);
    dk->addDgtr(321, 9993)->addDgtr(100443, 9981)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(*dk);
    delete dk;
    
    dk = new DecayTree(521);
    dk->addDgtr(211, 9993)->addDgtr(100443, 9981)->addDgtr(321,-211);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(*dk);
    delete dk;
    
    dk = new DecayTree(521);
    dk->addDgtr(-211, 9993)->addDgtr(100443, 9981)->addDgtr(321,211);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(*dk);
    delete dk;
    
    //B-> psi NA ; NA-> NS P
    dk = new DecayTree(521);
    dk->addDgtr(100443, 9983)->addDgtr(321, 9981)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    dk = new DecayTree(521);
    dk->addDgtr(100443, 9983)->addDgtr(211, 9981)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    dk = new DecayTree(521);
    dk->addDgtr(100443, 9983)->addDgtr(-211, 9981)->addDgtr(321,211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    
    //B-> P NV ; NV-> psi NS
    dk = new DecayTree(521);
    dk->addDgtr(321, 9993)->addDgtr(100443, 9981)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    dk = new DecayTree(521);
    dk->addDgtr(211, 9993)->addDgtr(100443, 9981)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    dk = new DecayTree(521);
    dk->addDgtr(-211, 9993)->addDgtr(100443, 9981)->addDgtr(321,211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    
    //B-> NV NV
    dk = new DecayTree(521);
    r1=DecayTree(9993);
    r1.addDgtr(100443,321);
    r2=DecayTree(9993);
    r2.addDgtr(211,-211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    dk = new DecayTree(521);
    r1=DecayTree(9993);
    r1.addDgtr(100443,211);
    r2=DecayTree(9993);
    r2.addDgtr(321,-211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    dk = new DecayTree(521);
    r1=DecayTree(9993);
    r1.addDgtr(100443,-211);
    r2=DecayTree(9993);
    r2.addDgtr(321,211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    
    //B-> NA NV
    dk = new DecayTree(521);
    r1=DecayTree(9983);
    r1.addDgtr(100443,321);
    r2=DecayTree(9993);
    r2.addDgtr(211,-211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    dk = new DecayTree(521);
    r1=DecayTree(9983);
    r1.addDgtr(100443,211);
    r2=DecayTree(9993);
    r2.addDgtr(321,-211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    dk = new DecayTree(521);
    r1=DecayTree(9983);
    r1.addDgtr(100443,-211);
    r2=DecayTree(9993);
    r2.addDgtr(321,211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    
    //B-> NA NS
    dk = new DecayTree(521);
    r1=DecayTree(9983);
    r1.addDgtr(100443,321);
    r2=DecayTree(9981);
    r2.addDgtr(211,-211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    dk = new DecayTree(521);
    r1=DecayTree(9983);
    r1.addDgtr(100443,211);
    r2=DecayTree(9981);
    r2.addDgtr(321,-211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    dk = new DecayTree(521);
    r1=DecayTree(9983);
    r1.addDgtr(100443,-211);
    r2=DecayTree(9981);
    r2.addDgtr(321,211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    
    //B-> NP NS
    dk = new DecayTree(521);
    r1=DecayTree(9991);
    r1.addDgtr(100443,321);
    r2=DecayTree(9981);
    r2.addDgtr(211,-211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    dk = new DecayTree(521);
    r1=DecayTree(9991);
    r1.addDgtr(100443,211);
    r2=DecayTree(9981);
    r2.addDgtr(321,-211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    dk = new DecayTree(521);
    r1=DecayTree(9991);
    r1.addDgtr(100443,-211);
    r2=DecayTree(9981);
    r2.addDgtr(321,211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;


    // Done
    return _trees.size();


}


int NamedDecayTreeList::makeKpipiGammaList(){
    
    DecayTree* dk=0;
    
    // B->gamma K1(1270); K1->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 10323)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_10323_"));  
    dk->getVal().setL(1);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_10323_"));  
    dk->getVal().setL(2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_10323_"));  
    delete dk;

    // B->gamma K1(1270); K1->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 10323)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "SBW_10323_GS"));
    this->add(AmpInitialiser(*dk, "SBW_10323_RHO_OMEGA"));
    dk->getVal().setL(1);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));  
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "SBW_10323_GS"));
    this->add(AmpInitialiser(*dk, "SBW_10323_RHO_OMEGA"));
    dk->getVal().setL(2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "SBW_10323_GS"));
    this->add(AmpInitialiser(*dk, "SBW_10323_RHO_OMEGA"));
    delete dk;

    // B->gamma K1(1270); K1-> K0^*(1430) pi; K0^*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 10323)->addDgtr(211, 10311)->addDgtr(321,-211);
    this->add(AmpInitialiser(*dk, "Lass"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS")); 
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    delete dk;
    
    // B->gamma K1(1400); K1->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 20323)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS")); 
    this->add(AmpInitialiser(*dk, "SBW_20323_"));
    delete dk;

    // B->gamma K1(1400); K1->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 20323)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    delete dk;

    // B->gamma K*(1410); K*(1410)->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 100323)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_100323_"));   
    delete dk;
    
    // B->gamma K*(1410); K*->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 100323)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));  
    this->add(AmpInitialiser(*dk, "SBW_100323_GS"));   
    this->add(AmpInitialiser(*dk, "SBW_100323_"));  
    this->add(AmpInitialiser(*dk, "SBW_100323_RHO_OMEGA"));  
    delete dk;
    
    // B->gamma K2*(1430); K2->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 325)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_325_"));
    delete dk;
    
    // B->gamma  K2*(1430); K2->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 325)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS")); 
    this->add(AmpInitialiser(*dk, "SBW_325_"));
    this->add(AmpInitialiser(*dk, "SBW_325_GS"));
    this->add(AmpInitialiser(*dk, "SBW_325_RHO_OMEGA"));
    delete dk;

    // B->gamma K2(1580); K2->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 9000325)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9000325_"));
    delete dk;

    // B->gamma K2(1580); K2->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 9000325)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS")); 
    this->add(AmpInitialiser(*dk, "SBW_9000325_"));
    this->add(AmpInitialiser(*dk, "SBW_9000325_GS"));
    delete dk;

    // B->gamma K2(1770); K2->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 10325)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_10325_"));
    delete dk;

    // B->gamma  K2(1770); K2->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 10325)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_10325_"));
    this->add(AmpInitialiser(*dk, "SBW_10325_GS"));
    delete dk;
    
    // B->gamma K2(1770); K2->K f0(980); f2->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 10325)->addDgtr(321, 9010221)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "Flatte"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_10325_"));    
    this->add(AmpInitialiser(*dk, "SBW_10325_Flatte"));    
    delete dk;
    
    // B->gamma K2(1770); K2->K f2(1270); f2->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 10325)->addDgtr(321, 225)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_"));   
    this->add(AmpInitialiser(*dk, "SBW_10325_"));    
    delete dk;
    
    // B->gamma K2(1770); K2->K2*(1430) pi; K2*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 10325)->addDgtr(211, 315)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_"));   
    this->add(AmpInitialiser(*dk, "SBW_10325_"));    
    delete dk;
    
    // B->gamma K2(1820); K2->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 20325)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_20325_"));
    delete dk;
    
    // B->gamma  K2(1820); K2->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 20325)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_20325_"));
    this->add(AmpInitialiser(*dk, "SBW_20325_GS"));
    delete dk;
    
    // B->gamma K2(1820); K2->K omega; omega->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 20325)->addDgtr(321, 223)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    delete dk;
    
    // B->gamma K2(1820); K2->K f0(980); f2->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 20325)->addDgtr(321, 9010221)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "Flatte"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_20325_"));    
    this->add(AmpInitialiser(*dk, "SBW_20325_Flatte"));    
    delete dk;
    
    // B->gamma K2(1820); K2->K f2(1270); f2->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 20325)->addDgtr(321, 225)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_"));   
    this->add(AmpInitialiser(*dk, "SBW_20325_"));    
    delete dk;
    
    // B->gamma K2(1820); K2->K2*(1430) pi; K2*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 20325)->addDgtr(211, 315)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_"));   
    this->add(AmpInitialiser(*dk, "SBW_20325_"));    
    delete dk;

    // B->gamma K2*(1980); K2->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 100325)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));
    this->add(AmpInitialiser(*dk, "SBW_100325_"));
    delete dk;
    
    // B->gamma K2*(1980); K2->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 100325)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS")); 
    this->add(AmpInitialiser(*dk, "SBW_100325_"));
    this->add(AmpInitialiser(*dk, "SBW_100325_GS"));
    delete dk;
    
    // B->gamma K2(2250); K2->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 9010325)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));  
    this->add(AmpInitialiser(*dk, "SBW_9010325_"));
    delete dk;
    
    // B->gamma K2(2250); K2->K1*(1410) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 9010325)->addDgtr(211, 100313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));       
    this->add(AmpInitialiser(*dk, "SBW_9010325_"));
    delete dk;
    
    // B->gammma  K2(2250); K2->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 9010325)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_9010325_"));
    this->add(AmpInitialiser(*dk, "SBW_9010325_GS"));
    delete dk;
    
    // B->gamma K2(2250); K2->K f0(980); f2->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 9010325)->addDgtr(321, 9010221)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "Flatte"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9010325_"));    
    this->add(AmpInitialiser(*dk, "SBW_9010325_Flatte"));  
    delete dk;
    
    // B->gamma K2(2250); K2->K f2(1270); f2->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 9010325)->addDgtr(321, 225)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_"));   
    this->add(AmpInitialiser(*dk, "SBW_9010325_"));    
    delete dk;
    
    // B->gamma K2(2250); K2->K2*(1430) pi; K2*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(22, 9010325)->addDgtr(211, 315)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_"));   
    this->add(AmpInitialiser(*dk, "SBW_9010325_"));    
    delete dk;

    //Non resonant amplitudes:
    //------------------------
    
    // B->gamma (K pi pi)_S
    dk = new DecayTree(521);
    dk->addDgtr(22, 9991)->addDgtr(321,9981)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "PLUSNonRes"));
    this->add(AmpInitialiser(*dk, "MINUSNonRes")); 
    delete dk;
    
    //B->gamma NA ; NA-> NV P
    dk = new DecayTree(521);
    dk->addDgtr(22, 9983)->addDgtr(321, 9993)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));  
    this->add(AmpInitialiser(*dk, "PLUSNonRes"));
    this->add(AmpInitialiser(*dk, "MINUSNonRes")); 
    this->add(*dk);
    delete dk;

    dk = new DecayTree(521);
    dk->addDgtr(22, 9983)->addDgtr(211, 9993)->addDgtr(321,-211);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "PLUSNonRes"));
    this->add(AmpInitialiser(*dk, "MINUSNonRes")); 
    this->add(*dk);
    delete dk;

    dk = new DecayTree(521);
    dk->addDgtr(22, 9983)->addDgtr(-211, 9993)->addDgtr(321,211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "PLUSNonRes"));
    this->add(AmpInitialiser(*dk, "MINUSNonRes"));    
    delete dk;

    // Done
    return _trees.size();
}

int NamedDecayTreeList::makeJpsiKpipiList(){
    
    DecayTree* dk=0;
    
    // B->J/psi K1(1270); K1->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 10323)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_10323_"));   
    delete dk;
    
    dk = new DecayTree(521);
    DecayTree k1(10323);
    k1.getVal().setL(2);
    k1.addDgtr(211, 313)->addDgtr(321,-211);
    dk->addDgtr(443);
    dk->addDgtr(&k1);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_10323_"));   
    delete dk;
    
    dk = new DecayTree(521);
    dk->addDgtr(443, 10323)->addDgtr(211, 313)->addDgtr(321,-211);
    dk->getVal().setL(1);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS")); 
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    delete dk;
    
    dk = new DecayTree(521);
    dk->addDgtr(443, 10323)->addDgtr(211, 313)->addDgtr(321,-211);
    dk->getVal().setL(2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));  
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    delete dk;
    
    // B->J/psi K1(1270); K1->K1*(1410) pi; K1*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 10323)->addDgtr(211, 100313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    delete dk;
    
    dk = new DecayTree(521);
    k1=DecayTree(10323);
    k1.getVal().setL(2);
    k1.addDgtr(211, 313)->addDgtr(100321,-211);
    dk->addDgtr(443);
    dk->addDgtr(&k1);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    delete dk;
    
    dk = new DecayTree(521);
    dk->addDgtr(443, 10323)->addDgtr(211, 100313)->addDgtr(321,-211);
    dk->getVal().setL(1);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    delete dk;
    
    dk = new DecayTree(521);
    dk->addDgtr(443, 10323)->addDgtr(211, 100313)->addDgtr(321,-211);
    dk->getVal().setL(2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    delete dk;
    
    // B->J/psi K1(1270); K1->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 10323)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "SBW_10323_321_"));
    this->add(AmpInitialiser(*dk, "SBW_321_"));
    this->add(AmpInitialiser(*dk, "SBW_10323_GS"));
    this->add(AmpInitialiser(*dk, "SBW_10323_RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "VBW_10323_"));
    this->add(AmpInitialiser(*dk, "VBW_10323_GS"));
    this->add(AmpInitialiser(*dk, "VBW_10323_RHO_OMEGA"));
    //this->add(AmpInitialiser(*dk, "VBW_321_"));
    dk->getVal().setL(1);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));  
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "SBW_10323_GS"));
    this->add(AmpInitialiser(*dk, "SBW_10323_RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "VBW_10323_"));
    this->add(AmpInitialiser(*dk, "VBW_10323_GS"));
    this->add(AmpInitialiser(*dk, "VBW_10323_RHO_OMEGA"));
    dk->getVal().setL(2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "SBW_10323_GS"));
    this->add(AmpInitialiser(*dk, "SBW_10323_RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "VBW_10323_"));
    this->add(AmpInitialiser(*dk, "VBW_10323_GS"));
    this->add(AmpInitialiser(*dk, "VBW_10323_RHO_OMEGA"));
    delete dk;
    
    dk = new DecayTree(521);
    DecayTree K1(10323);
    K1.getVal().setL(2);
    K1.addDgtr(321, 113)->addDgtr(211,-211);
    dk->addDgtr(443);
    dk->addDgtr(&K1);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));     
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "SBW_10323_GS"));
    this->add(AmpInitialiser(*dk, "SBW_10323_RHO_OMEGA")); 
    delete dk;
    
    // B->J/psi K1(1270); K1->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 10323)->addDgtr(321, 113)->addDgtr(-211,211);    // shouldn't make a difference to previous ones
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "SBW_10323_321_"));
    this->add(AmpInitialiser(*dk, "SBW_321_"));
    this->add(AmpInitialiser(*dk, "SBW_10323_GS"));
    this->add(AmpInitialiser(*dk, "SBW_10323_RHO_OMEGA"));
    dk->getVal().setL(1);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));  
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "SBW_10323_GS"));
    this->add(AmpInitialiser(*dk, "SBW_10323_RHO_OMEGA"));
    dk->getVal().setL(2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "SBW_10323_GS"));
    this->add(AmpInitialiser(*dk, "SBW_10323_RHO_OMEGA"));
    delete dk;
    
    // B->J/psi K1(1270); K1->K omega; omega->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 10323)->addDgtr(321, 223)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    delete dk;
    
    // B->J/psi K1(1270); K1->K f0; f0->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 10323)->addDgtr(321, 9000221)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));  
    this->add(AmpInitialiser(*dk, "Bugg"));
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(443, 10323)->addDgtr(321, 9010221)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS")); 
    this->add(AmpInitialiser(*dk, "FocusFlatte"));
    this->add(AmpInitialiser(*dk, "Flatte"));
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "SBW_10323_Flatte"));
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(443, 10323)->addDgtr(321, 30221)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "FocusFlatte"));
    this->add(AmpInitialiser(*dk, "Flatte"));
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "SBW_10323_Flatte"));
    this->add(AmpInitialiser(*dk, "SBW_10323_VBW_30221_"));
    this->add(AmpInitialiser(*dk, "VBW_30221_"));
    delete dk;
    
    // B->J/Psi K1(1270); K1-> K0^*(1430) pi; K0^*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 10323)->addDgtr(211, 10311)->addDgtr(321,-211);
    this->add(AmpInitialiser(*dk, "Lass"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS")); 
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "SBW_10323_Lass"));
    this->add(AmpInitialiser(*dk, "VBW_10311_"));
    this->add(AmpInitialiser(*dk, "VBW_10311_Lass"));
    this->add(AmpInitialiser(*dk, "VBW_10323_"));
    this->add(AmpInitialiser(*dk, "VBW_10323_Lass"));
    this->add(AmpInitialiser(*dk, "SBW_10323_VBW_10311_"));
    this->add(AmpInitialiser(*dk, "SBW_10323_VBW_10311_Lass"));
    delete dk;
    
    // B->J/Psi K1(1270); K1-> kappa(800) pi;   
    dk = new DecayTree(521);
    dk->addDgtr(443, 10323)->addDgtr(211, 900311)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
     this->add(AmpInitialiser(*dk, "SBW_10323_"));
    delete dk;
    
    // B->J/psi K1(1400); K1->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 20323)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS")); 
    this->add(AmpInitialiser(*dk, "SBW_20323_"));
    dk->getVal().setL(1);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS")); 
    this->add(AmpInitialiser(*dk, "SBW_20323_"));
    dk->getVal().setL(2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS")); 
    this->add(AmpInitialiser(*dk, "SBW_20323_"));
    delete dk;
    
    dk = new DecayTree(521);
    K1=DecayTree(20323);
    K1.getVal().setL(2);
    K1.addDgtr(211, 313)->addDgtr(321,-211);
    dk->addDgtr(443);
    dk->addDgtr(&K1);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));     
    this->add(AmpInitialiser(*dk, "SBW_20323_"));
    delete dk;
    
    // B->J/psi K1(1400); K1->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 20323)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_20323_"));
    this->add(AmpInitialiser(*dk, "SBW_20323_GS"));
    dk->getVal().setL(1);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));  
    this->add(AmpInitialiser(*dk, "SBW_20323_"));
    this->add(AmpInitialiser(*dk, "SBW_20323_GS"));
    dk->getVal().setL(2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_20323_"));
    this->add(AmpInitialiser(*dk, "SBW_20323_GS"));
    delete dk;
    
    dk = new DecayTree(521);
    K1=DecayTree(20323);
    K1.getVal().setL(2);
    K1.addDgtr(321, 113)->addDgtr(211,-211);
    dk->addDgtr(443);
    dk->addDgtr(&K1);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));     
    this->add(AmpInitialiser(*dk, "SBW_20323_"));
    this->add(AmpInitialiser(*dk, "SBW_20323_GS"));
    this->add(AmpInitialiser(*dk, "SBW_20323_RHO_OMEGA")); 
    delete dk;
    
    // B->J/psi K1(1400); K1->K omega; omega->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 20323)->addDgtr(321, 223)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    delete dk;//Non resonant amplitudes:
    //------------------------
    
    // B->J/psi (K pi pi)_S
    dk = new DecayTree(521);
    dk->addDgtr(443, 9991)->addDgtr(321,9981)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_9991"));
    this->add(AmpInitialiser(*dk, "NonResExp_9981"));
    this->add(AmpInitialiser(*dk, "NonResExp_9991_9981"));
    this->add(AmpInitialiser(*dk, "NonResPow_9991"));
    this->add(AmpInitialiser(*dk, "NonResPow_9981"));
    this->add(AmpInitialiser(*dk, "NonResPow_9991_9981"));
    delete dk;
    
    //B-> J/psi NA ; NA-> NV P
    dk = new DecayTree(521);
    dk->addDgtr(443, 9983)->addDgtr(321, 9993)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResPhsp"));
    this->add(*dk);
    dk->getVal().setL(1);
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResPhsp"));
    this->add(*dk);
    
    dk = new DecayTree(521);
    dk->addDgtr(443, 9983)->addDgtr(211, 9993)->addDgtr(321,-211);
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(*dk);
    dk->getVal().setL(1);
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(*dk);

    dk = new DecayTree(521);
    dk->addDgtr(443, 9983)->addDgtr(-211, 9993)->addDgtr(321,211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    dk->getVal().setL(1);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    this->add(AmpInitialiser(*dk, "NonRes"));

    delete dk;
    
    // B->J/psi K1(1400); K1->K f0; f0->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 20323)->addDgtr(321, 9000221)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));  
    this->add(AmpInitialiser(*dk, "Bugg"));
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(443, 20323)->addDgtr(321, 9010221)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS")); 
    this->add(AmpInitialiser(*dk, "FocusFlatte"));
    this->add(AmpInitialiser(*dk, "Flatte"));
    this->add(AmpInitialiser(*dk, "SBW_20323_"));
    this->add(AmpInitialiser(*dk, "SBW_20323_Flatte"));
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(443, 20323)->addDgtr(321, 30221)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "FocusFlatte"));
    this->add(AmpInitialiser(*dk, "Flatte"));
    this->add(AmpInitialiser(*dk, "SBW_20323_"));
    this->add(AmpInitialiser(*dk, "SBW_20323_Flatte"));
    this->add(AmpInitialiser(*dk, "SBW_20323_VBW_30221_"));
    this->add(AmpInitialiser(*dk, "VBW_30221_"));
    delete dk;
    
    // B->J/Psi K1(1400); K1-> K0^*(1430) pi; K0^*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 20323)->addDgtr(211, 10311)->addDgtr(321,-211);
    this->add(AmpInitialiser(*dk, "Lass"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS")); 
    this->add(AmpInitialiser(*dk, "SBW_20323_"));
    this->add(AmpInitialiser(*dk, "SBW_20323_Lass"));
    this->add(AmpInitialiser(*dk, "VBW_10311_"));
    this->add(AmpInitialiser(*dk, "VBW_10311_Lass"));
    this->add(AmpInitialiser(*dk, "SBW_20323_VBW_10311_"));
    this->add(AmpInitialiser(*dk, "SBW_20323_VBW_10311_Lass"));
    delete dk;
    
    // B->J/psi K*(1410); K*(1410)->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 100323)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_"));   
    this->add(AmpInitialiser(*dk, "SBW_100323_"));   

    delete dk;
    
    // B->J/psi K*(1410); K*->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 100323)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));  
    this->add(AmpInitialiser(*dk, "SBW_100323_GS"));   
    this->add(AmpInitialiser(*dk, "SBW_100323_"));  
    this->add(AmpInitialiser(*dk, "SBW_100323_RHO_OMEGA"));  
    delete dk;
    
    // B->psi K(1460); K->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 100321)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);   
    this->add(AmpInitialiser(*dk, "SBW_100321_"));
    delete dk;
    
    // B->psi K(1460); K->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 100321)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_100321_"));
    this->add(AmpInitialiser(*dk, "SBW_100321_GS"));
    this->add(AmpInitialiser(*dk, "SBW_100321_RHO_OMEGA"));
    delete dk;
    
    // B->psi K(1460); K->K f0(980); f0->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 100321)->addDgtr(321, 9010221)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "Flatte"));
    this->add(*dk); 
    this->add(AmpInitialiser(*dk, "SBW_100321_"));    
    this->add(AmpInitialiser(*dk, "SBW_100321_Flatte"));    
    delete dk;
    
    // B->psi K(1460); K-> K0^*(1430) pi; K0^*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 100321)->addDgtr(211, 10311)->addDgtr(321,-211);
    this->add(AmpInitialiser(*dk, "Lass"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_100321_"));
    this->add(AmpInitialiser(*dk, "VBW_100321_"));
    delete dk;

    
    // B->J/psi K2(1580); K2->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 9000325)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9000325_"));
    dk->getVal().setL(2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9000325_"));
    delete dk;
    
    // B->J/psi K2(1580); K2->K1*(1410) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 9000325)->addDgtr(211, 100313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS")); 
    this->add(AmpInitialiser(*dk, "SBW_9000325_"));
    dk->getVal().setL(2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9000325_"));
    delete dk;
    
    // B->J/psi  K2(1580); K2->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 9000325)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS")); 
    this->add(AmpInitialiser(*dk, "SBW_9000325_"));
    this->add(AmpInitialiser(*dk, "SBW_9000325_GS"));
    dk->getVal().setL(2);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9000325_"));
    this->add(AmpInitialiser(*dk, "SBW_9000325_GS"));
    delete dk;
    
    // B->J/psi K2(1580); K2->K omega; omega->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 9000325)->addDgtr(321, 223)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    delete dk;
    
    // B->J/psi K2(1580); K2->K f0(980); f2->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 9000325)->addDgtr(321, 9010221)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "FocusFlatte"));
    this->add(AmpInitialiser(*dk, "Flatte"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9000325_"));    
    this->add(AmpInitialiser(*dk, "SBW_9000325_FocusFlatte"));    
    this->add(AmpInitialiser(*dk, "SBW_9000325_Flatte"));    
    
    delete dk;
    
    // B->J/psi K2(1580); K2->K f2(1270); f2->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 9000325)->addDgtr(321, 225)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_"));   
    this->add(AmpInitialiser(*dk, "SBW_9000325_"));    
    this->add(AmpInitialiser(*dk, "SBW_9000325_225_"));
    this->add(AmpInitialiser(*dk, "SBW_225_"));    
    delete dk;
    
    // B->J/psi K2(1580); K2->K2*(1430) pi; K2*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 9000325)->addDgtr(211, 315)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_"));   
    this->add(AmpInitialiser(*dk, "SBW_9000325_"));    
    this->add(AmpInitialiser(*dk, "SBW_9000325_315_"));
    this->add(AmpInitialiser(*dk, "SBW_9000325_")); 
    
    // B->J/psi K2(1770); K2->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 10325)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_10325_"));
    dk->getVal().setL(2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));  
    this->add(AmpInitialiser(*dk, "SBW_10325_"));
    delete dk;
    
    // B->J/psi K2(1770); K2->K1*(1410) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 10325)->addDgtr(211, 100313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_10325_"));
    dk->getVal().setL(2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_10325_"));
    delete dk;
    
    // B->J/psi  K2(1770); K2->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 10325)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_10325_"));
    this->add(AmpInitialiser(*dk, "SBW_10325_GS"));
    dk->getVal().setL(2);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_10325_"));
    this->add(AmpInitialiser(*dk, "SBW_10325_GS"));
    delete dk;
    
    // B->J/psi K2(1770); K2->K omega; omega->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 10325)->addDgtr(321, 223)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    delete dk;
        
    // B->J/psi K2(1770); K2->K f0(980); f2->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 10325)->addDgtr(321, 9010221)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "FocusFlatte"));
    this->add(AmpInitialiser(*dk, "Flatte"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_10325_"));    
    this->add(AmpInitialiser(*dk, "SBW_10325_FocusFlatte"));    
    this->add(AmpInitialiser(*dk, "SBW_10325_Flatte"));    
    delete dk;
    
    // B->J/psi K2(1770); K2->K f2(1270); f2->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 10325)->addDgtr(321, 225)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_"));   
    this->add(AmpInitialiser(*dk, "SBW_10325_"));    
    this->add(AmpInitialiser(*dk, "SBW_10325_225_"));
    this->add(AmpInitialiser(*dk, "SBW_225_"));    
    this->add(AmpInitialiser(*dk, "VBW_10325_"));   
    delete dk;
    
    // B->J/psi K2(1770); K2->K2*(1430) pi; K2*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 10325)->addDgtr(211, 315)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_"));   
    this->add(AmpInitialiser(*dk, "SBW_10325_"));    
    this->add(AmpInitialiser(*dk, "SBW_10325_315_"));
    this->add(AmpInitialiser(*dk, "SBW_315_")); 
    delete dk;
    
    // B->J/psi K2(1820); K2->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 20325)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_20325_"));
    dk->getVal().setL(2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS")); 
    this->add(AmpInitialiser(*dk, "SBW_20325_"));
    delete dk;
    
    // B->J/psi K2(1820); K2->K1*(1410) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 20325)->addDgtr(211, 100313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_20325_"));
    dk->getVal().setL(2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_20325_"));
    delete dk;
    
    // B->J/psi  K2(1820); K2->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 20325)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_20325_"));
    this->add(AmpInitialiser(*dk, "SBW_20325_GS"));
    dk->getVal().setL(2);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_20325_"));
    this->add(AmpInitialiser(*dk, "SBW_20325_GS"));
    delete dk;
    
    // B->J/psi K2(1820); K2->K omega; omega->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 20325)->addDgtr(321, 223)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    delete dk;
    
    // B->J/psi K2(1820); K2->K f0(980); f2->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 20325)->addDgtr(321, 9010221)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "FocusFlatte"));
    this->add(AmpInitialiser(*dk, "Flatte"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_20325_"));    
    this->add(AmpInitialiser(*dk, "SBW_20325_FocusFlatte"));    
    this->add(AmpInitialiser(*dk, "SBW_20325_Flatte"));    
    delete dk;
    
    // B->J/psi K2(1820); K2->K f2(1270); f2->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 20325)->addDgtr(321, 225)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_"));   
    this->add(AmpInitialiser(*dk, "SBW_20325_"));    
    this->add(AmpInitialiser(*dk, "SBW_20325_225_"));
    this->add(AmpInitialiser(*dk, "SBW_225_"));    
    delete dk;
    
    // B->J/psi K2(1820); K2->K2*(1430) pi; K2*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 20325)->addDgtr(211, 315)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_"));   
    this->add(AmpInitialiser(*dk, "SBW_20325_"));    
    this->add(AmpInitialiser(*dk, "SBW_20325_315_"));
    this->add(AmpInitialiser(*dk, "SBW_315_")); 
    delete dk;
    
    // B->J/psi K2(2250); K2->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 9010325)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));  
    this->add(AmpInitialiser(*dk, "SBW_9010325_"));
    this->add(AmpInitialiser(*dk, "VBW_9010325_"));
    dk->getVal().setL(2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS")); 
    this->add(AmpInitialiser(*dk, "SBW_9010325_"));
    this->add(AmpInitialiser(*dk, "VBW_9010325_"));
    delete dk;
    
    // B->J/psi K2(2250); K2->K1*(1410) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 9010325)->addDgtr(211, 100313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_9010325_"));
    this->add(AmpInitialiser(*dk, "VBW_9010325_"));
    dk->getVal().setL(2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_9010325_"));
    this->add(AmpInitialiser(*dk, "VBW_9010325_"));
    delete dk;
    
    // B->J/psi  K2(2250); K2->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 9010325)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_9010325_"));
    this->add(AmpInitialiser(*dk, "SBW_9010325_GS"));
    this->add(AmpInitialiser(*dk, "VBW_9010325_"));
    this->add(AmpInitialiser(*dk, "VBW_9010325_GS"));
    dk->getVal().setL(2);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_9010325_"));
    this->add(AmpInitialiser(*dk, "SBW_9010325_GS"));
    this->add(AmpInitialiser(*dk, "VBW_9010325_"));
    this->add(AmpInitialiser(*dk, "VBW_9010325_GS"));
    delete dk;
    
    // B->J/psi K2(2250); K2->K omega; omega->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 9010325)->addDgtr(321, 223)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    delete dk;
    
    // B->J/psi K2(2250); K2->K f0(980); f2->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 9010325)->addDgtr(321, 9010221)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "FocusFlatte"));
    this->add(AmpInitialiser(*dk, "Flatte"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9010325_"));    
    this->add(AmpInitialiser(*dk, "SBW_9010325_FocusFlatte"));    
    this->add(AmpInitialiser(*dk, "SBW_9010325_Flatte"));  
    delete dk;
    
    // B->J/psi K2(2250); K2->K f2(1270); f2->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 9010325)->addDgtr(321, 225)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_"));   
    this->add(AmpInitialiser(*dk, "SBW_9010325_"));    
    this->add(AmpInitialiser(*dk, "SBW_9010325_225_"));
    this->add(AmpInitialiser(*dk, "SBW_225_")); 
    this->add(AmpInitialiser(*dk, "VBW_9010325_"));
    delete dk;
    
    // B->J/psi K2(2250); K2->K2*(1430) pi; K2*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 9010325)->addDgtr(211, 315)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_"));   
    this->add(AmpInitialiser(*dk, "SBW_9010325_"));    
    this->add(AmpInitialiser(*dk, "SBW_9010325_315_"));
    this->add(AmpInitialiser(*dk, "SBW_315_")); 
    this->add(AmpInitialiser(*dk, "VBW_9010325_"));
    delete dk;
    
    // B->J/psi K2*(1430); K2->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 325)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_325_"));
    delete dk;
    
    // B->J/psi  K2*(1430); K2->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 325)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS")); 
    this->add(AmpInitialiser(*dk, "SBW_325_"));
    this->add(AmpInitialiser(*dk, "SBW_325_GS"));
    this->add(AmpInitialiser(*dk, "SBW_325_RHO_OMEGA"));
    delete dk;
    
    // B->J/psi K2*(1430); K2->K omega; omega->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 325)->addDgtr(321, 223)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));  
    this->add(AmpInitialiser(*dk, "SBW_325_"));
    delete dk;
    
    // B->J/psi K2*(1980); K2->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 100325)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));
    this->add(AmpInitialiser(*dk, "SBW_100325_"));
    delete dk;
    
    // B->J/psi  K2*(1980); K2->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 100325)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS")); 
    this->add(AmpInitialiser(*dk, "SBW_100325_"));
    this->add(AmpInitialiser(*dk, "SBW_100325_GS"));
    delete dk;
    
    // B->J/psi K2*(1980); K2->K omega; omega->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 100325)->addDgtr(321, 223)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));  
    this->add(AmpInitialiser(*dk, "SBW_100325_"));
    delete dk;

    // B->J/psi K1*(1680); K2->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 30323)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_30323_"));
    delete dk;
    
    // B->J/psi  K1*(1680); K2->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 30323)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));
    this->add(AmpInitialiser(*dk, "SBW_30323_"));
    this->add(AmpInitialiser(*dk, "SBW_30323_GS"));
    this->add(AmpInitialiser(*dk, "SBW_30323_RHO_OMEGA"));
    delete dk;
    
    // B->J/psi K1*(1680); K2->K omega; omega->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 30323)->addDgtr(321, 223)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    delete dk;
    
    // B->J/psi K1(1650); K1->K*(892) pi; K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 9000323)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    delete dk;
    
    dk = new DecayTree(521);
    k1=DecayTree(9000323);
    k1.getVal().setL(2);
    k1.addDgtr(211, 313)->addDgtr(321,-211);
    dk->addDgtr(443);
    dk->addDgtr(&k1);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    delete dk;
    
    dk = new DecayTree(521);
    dk->addDgtr(443, 9000323)->addDgtr(211, 313)->addDgtr(321,-211);
    dk->getVal().setL(1);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    delete dk;
    
    dk = new DecayTree(521);
    dk->addDgtr(443, 9000323)->addDgtr(211, 313)->addDgtr(321,-211);
    dk->getVal().setL(2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    delete dk;
    
    // B->J/psi K1(1650); K1->K1*(1410) pi; K1*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 9000323)->addDgtr(211, 100313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    delete dk;
    
    dk = new DecayTree(521);
    k1=DecayTree(9000323);
    k1.getVal().setL(2);
    k1.addDgtr(211, 313)->addDgtr(100321,-211);
    dk->addDgtr(443);
    dk->addDgtr(&k1);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    delete dk;
    
    dk = new DecayTree(521);
    dk->addDgtr(443, 9000323)->addDgtr(211, 100313)->addDgtr(321,-211);
    dk->getVal().setL(1);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    delete dk;
    
    dk = new DecayTree(521);
    dk->addDgtr(443, 9000323)->addDgtr(211, 100313)->addDgtr(321,-211);
    dk->getVal().setL(2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    delete dk;
    
    // B->J/psi K1(1650); K1->K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 9000323)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    dk->getVal().setL(1);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));  
    dk->getVal().setL(2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    delete dk;
    
    dk = new DecayTree(521);
    K1=DecayTree(10323);
    K1.getVal().setL(2);
    K1.addDgtr(321, 113)->addDgtr(211,-211);
    dk->addDgtr(443);
    dk->addDgtr(&K1);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    delete dk;
    
    // B->J/psi K1(1650); K1->K omega; omega->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 9000323)->addDgtr(321, 223)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    delete dk;
    
    // B->J/psi K1(1650); K1->K f0; f0->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 9000323)->addDgtr(321, 9000221)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));  
    this->add(AmpInitialiser(*dk, "Bugg"));
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(443, 9000323)->addDgtr(321, 9010221)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS")); 
    this->add(AmpInitialiser(*dk, "FocusFlatte"));
    this->add(AmpInitialiser(*dk, "Flatte"));
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(443, 9000323)->addDgtr(321, 30221)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "FocusFlatte"));
    this->add(AmpInitialiser(*dk, "Flatte"));
    delete dk;
    
    // B->J/Psi K1(1650); K1-> K0^*(1430) pi; K0^*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 9000323)->addDgtr(211, 10311)->addDgtr(321,-211);
    this->add(AmpInitialiser(*dk, "Lass"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    delete dk;
    
    // B->J/Psi K1(1650); K1-> kappa(800) pi; K0^*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 9000323)->addDgtr(211, 900311)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    delete dk;
    
    // B->psi(2S) K; psi(2S)-> J/psi pi pi (non resonant)  
    dk = new DecayTree(521);
    dk->addDgtr(321, 100443)->addDgtr(443, 9981)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "3Body"));  
    this->add(AmpInitialiser(*dk, "SBW_100443_3Body"));   
    this->add(AmpInitialiser(*dk, "SBW_100443_"));  
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(321, 9100443)->addDgtr(443, 9981)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "3Body")); 
    this->add(AmpInitialiser(*dk, "SBW_9100443_3Body"));   
    this->add(AmpInitialiser(*dk, "SBW_9100443_"));   
    delete dk;
    
    // B->X(3872) K; X-> J/psi pi pi 
    dk = new DecayTree(521);
    dk->addDgtr(321, 9999991)->addDgtr(443, 9993)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "3Body"));   
    delete dk;
    // B->X(3872) K; X-> J/psi rho
    dk = new DecayTree(521);
    dk->addDgtr(321, 9999991)->addDgtr(443, 113)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));  
    this->add(AmpInitialiser(*dk, "SBW_9999991_"));   
    this->add(AmpInitialiser(*dk, "SBW_9999991_GS"));  
    this->add(AmpInitialiser(*dk, "GS"));  
    delete dk;
    
    // B->X(4260) K; X-> J/psi pi pi 
    dk = new DecayTree(521);
    dk->addDgtr(321, 9999992)->addDgtr(443, 9981)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "3Body"));   
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(321, 9999992)->addDgtr(443, 9993)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    delete dk;
    // B->X(4260) K; X-> J/psi rho
    dk = new DecayTree(521);
    dk->addDgtr(321, 9999992)->addDgtr(443, 113)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9999992_"));   
    this->add(AmpInitialiser(*dk, "SBW_9999992_GS"));  
    this->add(AmpInitialiser(*dk, "GS"));   
    this->add(AmpInitialiser(*dk, "LS_10_SBW_9999992_"));   
    this->add(AmpInitialiser(*dk, "LS_10_SBW_9999992_GS"));  
    this->add(AmpInitialiser(*dk, "LS_10_GS")); 
    this->add(AmpInitialiser(*dk, "LS_10_"));  
    this->add(AmpInitialiser(*dk, "LS_11_SBW_9999992_"));   
    this->add(AmpInitialiser(*dk, "LS_11_SBW_9999992_GS"));  
    this->add(AmpInitialiser(*dk, "LS_11_GS")); 
    this->add(AmpInitialiser(*dk, "LS_11_")); 
    this->add(AmpInitialiser(*dk, "LS_12_SBW_9999992_"));   
    this->add(AmpInitialiser(*dk, "LS_12_SBW_9999992_GS"));  
    this->add(AmpInitialiser(*dk, "LS_12_GS")); 
    this->add(AmpInitialiser(*dk, "LS_12_"));  
    delete dk;
    
    // B->K X(4260); X->Z pi; Z->J/psi pi  
    dk = new DecayTree(521);
    dk->addDgtr(321, 9999992)->addDgtr(211, 9999993)->addDgtr(443,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9999992_")); 
    this->add(AmpInitialiser(*dk, "SBW_9999992_VBW_9999993_")); 
    this->add(AmpInitialiser(*dk, "VBW_9999993_")); 
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(321, 9999992)->addDgtr(-211, 9999993)->addDgtr(443,211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9999992_")); 
    this->add(AmpInitialiser(*dk, "SBW_9999992_VBW_9999993_")); 
    this->add(AmpInitialiser(*dk, "VBW_9999993_")); 
    delete dk;
    
    dk = new DecayTree(521);
    dk->addDgtr(321, 9993)->addDgtr(211, 9999993)->addDgtr(443,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9999992_")); 
    this->add(AmpInitialiser(*dk, "SBW_9999992_VBW_9999993_")); 
    this->add(AmpInitialiser(*dk, "VBW_9999993_")); 
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(321, 9993)->addDgtr(-211, 9999993)->addDgtr(443,211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9999992_")); 
    this->add(AmpInitialiser(*dk, "SBW_9999992_VBW_9999993_")); 
    this->add(AmpInitialiser(*dk, "VBW_9999993_")); 
    delete dk;
    
    // B->K X(4260); X->f0 J/psi; f0->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(321, 9999992)->addDgtr(443, 9010221)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9999992_")); 
    this->add(AmpInitialiser(*dk, "Flatte")); 
    this->add(AmpInitialiser(*dk, "SBW_9999992_Flatte")); 
    delete dk;
    
    dk = new DecayTree(521);
    dk->addDgtr(321, 9999992)->addDgtr(443, 9000221)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "Bugg"));
    delete dk;
    
    // B->psi(4040) K; psi-> J/psi pi pi 
    dk = new DecayTree(521);
    dk->addDgtr(321, 9000443)->addDgtr(443, 9981)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_9000443_"));
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "3Body"));   
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(321, 9000443)->addDgtr(443, 9993)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_9000443_"));
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    delete dk;
    // B->psi(4040) K; psi(4040)-> J/psi rho
    dk = new DecayTree(521);
    dk->addDgtr(321, 9000443)->addDgtr(443, 113)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9000443_"));   
    this->add(AmpInitialiser(*dk, "SBW_9000443_GS"));  
    this->add(AmpInitialiser(*dk, "GS"));   
    this->add(AmpInitialiser(*dk, "LS_10_SBW_9000443_"));   
    this->add(AmpInitialiser(*dk, "LS_10_SBW_9000443_GS"));  
    this->add(AmpInitialiser(*dk, "LS_10_GS")); 
    this->add(AmpInitialiser(*dk, "LS_10_"));  
    this->add(AmpInitialiser(*dk, "LS_11_SBW_9000443_"));   
    this->add(AmpInitialiser(*dk, "LS_11_SBW_9000443_GS"));  
    this->add(AmpInitialiser(*dk, "LS_11_GS")); 
    this->add(AmpInitialiser(*dk, "LS_11_")); 
    this->add(AmpInitialiser(*dk, "LS_12_SBW_9000443_"));   
    this->add(AmpInitialiser(*dk, "LS_12_SBW_9000443_GS"));  
    this->add(AmpInitialiser(*dk, "LS_12_GS")); 
    this->add(AmpInitialiser(*dk, "LS_12_"));  
    delete dk;
    // B->K psi(4040)); psi(4040)->Z pi; Z->J/psi pi  
    dk = new DecayTree(521);
    dk->addDgtr(321, 9000443)->addDgtr(211, 9999993)->addDgtr(443,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9000443_"));
    this->add(AmpInitialiser(*dk, "SBW_9000443_VBW_9000443_")); 
    this->add(AmpInitialiser(*dk, "VBW_9000443_")); 
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(321, 9000443)->addDgtr(-211, 9999993)->addDgtr(443,211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9000443_")); 
    this->add(AmpInitialiser(*dk, "SBW_9000443_VBW_9999993_")); 
    this->add(AmpInitialiser(*dk, "VBW_9999993_")); 
    delete dk;
    // B->K psi(4040); psi(4040)->f0 J/psi; f0->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(321, 9000443)->addDgtr(443, 9010221)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9000443_")); 
    this->add(AmpInitialiser(*dk, "Flatte")); 
    this->add(AmpInitialiser(*dk, "SBW_9000443_Flatte")); 
    delete dk;
    //
    // B->psi(4160) K; psi-> J/psi pi pi 
    dk = new DecayTree(521);
    dk->addDgtr(321, 9010443)->addDgtr(443, 9981)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_9010443_"));
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "3Body"));   
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(321, 9010443)->addDgtr(443, 9993)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "SBW_9010443_"));
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    delete dk;
    // B->psi(4160) K; psi-> J/psi rho
    dk = new DecayTree(521);
    dk->addDgtr(321, 9010443)->addDgtr(443, 113)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9010443_"));   
    this->add(AmpInitialiser(*dk, "SBW_9010443_GS"));  
    this->add(AmpInitialiser(*dk, "GS"));   
    this->add(AmpInitialiser(*dk, "LS_10_SBW_9010443_"));   
    this->add(AmpInitialiser(*dk, "LS_10_SBW_9010443_GS"));  
    this->add(AmpInitialiser(*dk, "LS_10_GS")); 
    this->add(AmpInitialiser(*dk, "LS_10_"));  
    this->add(AmpInitialiser(*dk, "LS_11_SBW_9010443_"));   
    this->add(AmpInitialiser(*dk, "LS_11_SBW_9010443_GS"));  
    this->add(AmpInitialiser(*dk, "LS_11_GS")); 
    this->add(AmpInitialiser(*dk, "LS_11_")); 
    this->add(AmpInitialiser(*dk, "LS_12_SBW_9010443_"));   
    this->add(AmpInitialiser(*dk, "LS_12_SBW_9010443_GS"));  
    this->add(AmpInitialiser(*dk, "LS_12_GS")); 
    this->add(AmpInitialiser(*dk, "LS_12_"));  
    delete dk;
    // B->K psi(4160); psi->Z pi; Z->J/psi pi  
    dk = new DecayTree(521);
    dk->addDgtr(321, 9010443)->addDgtr(211, 9999993)->addDgtr(443,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9010443_")); 
    this->add(AmpInitialiser(*dk, "SBW_9010443_VBW_9999993_")); 
    this->add(AmpInitialiser(*dk, "VBW_9999993_")); 
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(321, 9010443)->addDgtr(-211, 9999993)->addDgtr(443,211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9010443_")); 
    this->add(AmpInitialiser(*dk, "SBW_90104432_VBW_9999993_")); 
    this->add(AmpInitialiser(*dk, "VBW_9999993_")); 
    delete dk;
    // B->K psi(4160); psi->f0 J/psi; f0->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(321, 9010443)->addDgtr(443, 9010221)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9010443_")); 
    this->add(AmpInitialiser(*dk, "Flatte")); 
    this->add(AmpInitialiser(*dk, "SBW_9010443_Flatte")); 
    delete dk;
    //
    // B->psi(4415) K; psi-> J/psi pi pi 
    dk = new DecayTree(521);
    dk->addDgtr(321, 9020443)->addDgtr(443, 9981)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "3Body"));   
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(321, 9020443)->addDgtr(443, 9993)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    delete dk;
    // B->psi(4415) K; psi-> J/psi rho
    dk = new DecayTree(521);
    dk->addDgtr(321, 9020443)->addDgtr(443, 113)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9999992_"));   
    this->add(AmpInitialiser(*dk, "SBW_9999992_GS"));  
    this->add(AmpInitialiser(*dk, "GS"));   
    this->add(AmpInitialiser(*dk, "LS_10_SBW_9999992_"));   
    this->add(AmpInitialiser(*dk, "LS_10_SBW_9999992_GS"));  
    this->add(AmpInitialiser(*dk, "LS_10_GS")); 
    this->add(AmpInitialiser(*dk, "LS_10_"));  
    this->add(AmpInitialiser(*dk, "LS_11_SBW_9999992_"));   
    this->add(AmpInitialiser(*dk, "LS_11_SBW_9999992_GS"));  
    this->add(AmpInitialiser(*dk, "LS_11_GS")); 
    this->add(AmpInitialiser(*dk, "LS_11_")); 
    this->add(AmpInitialiser(*dk, "LS_12_SBW_9999992_"));   
    this->add(AmpInitialiser(*dk, "LS_12_SBW_9999992_GS"));  
    this->add(AmpInitialiser(*dk, "LS_12_GS")); 
    this->add(AmpInitialiser(*dk, "LS_12_"));  
    delete dk;
    // B->K psi(4415); psi->Z pi; Z->J/psi pi  
    dk = new DecayTree(521);
    dk->addDgtr(321, 9020443)->addDgtr(211, 9999993)->addDgtr(443,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9999992_")); 
    this->add(AmpInitialiser(*dk, "SBW_9999992_VBW_9999993_")); 
    this->add(AmpInitialiser(*dk, "VBW_9999993_")); 
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(321, 9020443)->addDgtr(-211, 9999993)->addDgtr(443,211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9999992_")); 
    this->add(AmpInitialiser(*dk, "SBW_9999992_VBW_9999993_")); 
    this->add(AmpInitialiser(*dk, "VBW_9999993_")); 
    delete dk;
    // B->K psi(4415); psi->f0 J/psi; f0->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(321, 9020443)->addDgtr(443, 9010221)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    this->add(AmpInitialiser(*dk, "SBW_9999992_")); 
    this->add(AmpInitialiser(*dk, "Flatte")); 
    this->add(AmpInitialiser(*dk, "SBW_9999992_Flatte")); 
    delete dk;
    
    
    //B->K* Z(3900)V
    dk = new DecayTree(521);
    DecayTree Ks(313);
    Ks.addDgtr(321,-211);
    DecayTree Z(9999994);
    Z.addDgtr(443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    dk = new DecayTree(521);
    Ks=DecayTree(9993);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999994);
    Z.addDgtr(443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;

    //B->K* Z(3900)A
    dk = new DecayTree(521);
    Ks=DecayTree(313);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999993);
    Z.addDgtr(443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    dk = new DecayTree(521);
    Ks=DecayTree(9993);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999993);
    Z.addDgtr(443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    
    //B->K* Z(3900)P
    dk = new DecayTree(521);
    Ks=DecayTree(313);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999995);
    Z.addDgtr(443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    //B->K* Z(4051)V
    dk = new DecayTree(521);
    Ks=DecayTree(313);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999912);
    Z.addDgtr(443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    dk = new DecayTree(521);
    Ks=DecayTree(9993);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999912);
    Z.addDgtr(443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    //B->K* Z(4051)A
    dk = new DecayTree(521);
    Ks=DecayTree(313);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999911);
    Z.addDgtr(443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    dk = new DecayTree(521);
    Ks=DecayTree(9993);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999911);
    Z.addDgtr(443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    //B->K* Z(4051)P
    dk = new DecayTree(521);
    Ks=DecayTree(313);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999913);
    Z.addDgtr(443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    dk = new DecayTree(521);
    Ks=DecayTree(9993);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999913);
    Z.addDgtr(443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;

    //B->K* Z(4239)
    dk = new DecayTree(521);
    Ks=DecayTree(313);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999996);
    Z.addDgtr(443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    
    dk = new DecayTree(521);
    Ks=DecayTree(9993);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999996);
    Z.addDgtr(443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;

    //B->K* Z(4430)
    dk = new DecayTree(521);
    Ks=DecayTree(313);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999997);
    Z.addDgtr(443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    
    dk = new DecayTree(521);
    Ks=DecayTree(9993);
    Ks.addDgtr(321,-211);
    Z=DecayTree(9999997);
    Z.addDgtr(443,211);
    dk->addDgtr(&Ks);
    dk->addDgtr(&Z);
    this->add(*dk);
    delete dk;
    

    //Non resonant amplitudes:
    //------------------------
    
    // B->psi (K pi pi)_S
    dk = new DecayTree(521);
    dk->addDgtr(443, 9991)->addDgtr(321,9981)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_9991"));
    this->add(AmpInitialiser(*dk, "NonResExp_9981"));
    this->add(AmpInitialiser(*dk, "NonResExp_9991_9981"));
    this->add(AmpInitialiser(*dk, "NonResPow_9991"));
    this->add(AmpInitialiser(*dk, "NonResPow_9981"));
    this->add(AmpInitialiser(*dk, "NonResPow_9991_9981"));
    delete dk;
    
    //B-> J/psi NA ; NA-> NV P
    dk = new DecayTree(521);
    dk->addDgtr(443, 9983)->addDgtr(321, 9993)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResPhsp"));
    this->add(*dk);
    dk->getVal().setL(1);
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResPhsp"));
    this->add(*dk);
    
    dk = new DecayTree(521);
    dk->addDgtr(443, 9983)->addDgtr(211, 9993)->addDgtr(321,-211);
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(*dk);
    dk->getVal().setL(1);
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(*dk);

    dk = new DecayTree(521);
    dk->addDgtr(443, 9983)->addDgtr(-211, 9993)->addDgtr(321,211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    dk->getVal().setL(1);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    this->add(AmpInitialiser(*dk, "NonRes"));

    delete dk;
    
    //B-> J/psi NV ; NV-> NV P
    dk = new DecayTree(521);
    dk->addDgtr(443, 9993)->addDgtr(321, 9993)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "NonResExp_9993"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResPhsp"));
    this->add(*dk);
    dk->getVal().setL(1);
    this->add(AmpInitialiser(*dk, "NonResExp_9993"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResPhsp"));
    this->add(*dk);
    
    dk = new DecayTree(521);
    dk->addDgtr(443, 9993)->addDgtr(211, 9993)->addDgtr(321,-211);
    this->add(AmpInitialiser(*dk, "NonResExp_9993"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(*dk);
    dk->getVal().setL(1);
    this->add(AmpInitialiser(*dk, "NonResExp_9993"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(*dk);
    
    dk = new DecayTree(521);
    dk->addDgtr(443, 9993)->addDgtr(-211, 9993)->addDgtr(321,211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonResExp_9993"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    dk->getVal().setL(1);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonResExp_9993"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    
    delete dk;
    
    //B-> P NV ; NV-> Jpsi NS
    dk = new DecayTree(521);
    dk->addDgtr(321, 9993)->addDgtr(443, 9981)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "NonResExp_9993"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResPhsp"));
    this->add(*dk);
    delete dk;
    
    dk = new DecayTree(521);
    dk->addDgtr(211, 9993)->addDgtr(443, 9981)->addDgtr(321,-211);
    this->add(AmpInitialiser(*dk, "NonResExp_9993"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResPhsp"));
    this->add(*dk);
    delete dk;
    
    dk = new DecayTree(521);
    dk->addDgtr(-211, 9993)->addDgtr(443, 9981)->addDgtr(321,211);
    this->add(AmpInitialiser(*dk, "NonResExp_9993"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResPhsp"));
    this->add(*dk);
    delete dk;

    //B-> J/psi NA ; NA-> NS P
    dk = new DecayTree(521);
    dk->addDgtr(443, 9983)->addDgtr(321, 9981)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    dk = new DecayTree(521);
    dk->addDgtr(443, 9983)->addDgtr(211, 9981)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    dk = new DecayTree(521);
    dk->addDgtr(443, 9983)->addDgtr(-211, 9981)->addDgtr(321,211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    delete dk;

    //B-> P NV ; NV-> J/psi NS
    dk = new DecayTree(521);
    dk->addDgtr(321, 9993)->addDgtr(443, 9981)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    dk = new DecayTree(521);
    dk->addDgtr(211, 9993)->addDgtr(443, 9981)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    dk = new DecayTree(521);
    dk->addDgtr(-211, 9993)->addDgtr(443, 9981)->addDgtr(321,211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;

    //B-> NV NV
    dk = new DecayTree(521);
    DecayTree r1(9993);
    r1.addDgtr(443,321);
    DecayTree r2(9993);
    r2.addDgtr(211,-211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    dk = new DecayTree(521);
    r1=DecayTree(9993);
    r1.addDgtr(443,211);
    r2=DecayTree(9993);
    r2.addDgtr(321,-211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    dk = new DecayTree(521);
    r1=DecayTree(9993);
    r1.addDgtr(443,-211);
    r2=DecayTree(9993);
    r2.addDgtr(321,211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    
    //B-> NA NV
    dk = new DecayTree(521);
    r1=DecayTree(9983);
    r1.addDgtr(443,321);
    r2=DecayTree(9993);
    r2.addDgtr(211,-211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    dk = new DecayTree(521);
    r1=DecayTree(9983);
    r1.addDgtr(443,211);
    r2=DecayTree(9993);
    r2.addDgtr(321,-211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    dk = new DecayTree(521);
    r1=DecayTree(9983);
    r1.addDgtr(443,-211);
    r2=DecayTree(9993);
    r2.addDgtr(321,211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    
    //B-> NA NS
    dk = new DecayTree(521);
    r1=DecayTree(9983);
    r1.addDgtr(443,321);
    r2=DecayTree(9981);
    r2.addDgtr(211,-211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    dk = new DecayTree(521);
    r1=DecayTree(9983);
    r1.addDgtr(443,211);
    r2=DecayTree(9981);
    r2.addDgtr(321,-211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    dk = new DecayTree(521);
    r1=DecayTree(9983);
    r1.addDgtr(443,-211);
    r2=DecayTree(9981);
    r2.addDgtr(321,211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    
    //B-> NP NS
    dk = new DecayTree(521);
    r1=DecayTree(9991);
    r1.addDgtr(443,321);
    r2=DecayTree(9981);
    r2.addDgtr(211,-211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    dk = new DecayTree(521);
    r1=DecayTree(9991);
    r1.addDgtr(443,211);
    r2=DecayTree(9981);
    r2.addDgtr(321,-211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    dk = new DecayTree(521);
    r1=DecayTree(9991);
    r1.addDgtr(443,-211);
    r2=DecayTree(9981);
    r2.addDgtr(321,211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    delete dk;
    
    /*
    //B->J/psi K pi pi (non resonant)
    dk = new DecayTree(521);
    dk->addDgtr(321, 9993)->addDgtr(443, 9981)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));   
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(221, 9993)->addDgtr(443, 9981)->addDgtr(321,-211);
    this->add(*dk);
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(-211, 9993)->addDgtr(443, 9981)->addDgtr(211,321);
    this->add(*dk);
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(443, 9983)->addDgtr(211, 9993)->addDgtr(321,-211);
    this->add(*dk);
    delete dk;
    */
    
    //Single resonance amplitudes:
    //----------------------------
    
    // B->J/psi K1(1270); K1->K pi pi (non resonant) 
    dk = new DecayTree(521);
    dk->addDgtr(443, 10323)->addDgtr(211, 9993)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));  
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_10323"));
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(443, 10323)->addDgtr(321, 9993)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));  
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_10323"));
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(443, 10323)->addDgtr(-211, 9993)->addDgtr(321,211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));  
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_10323"));
    delete dk;
    
    // B->J/psi K1(1270); K1->K pi pi (non resonant)
    dk = new DecayTree(521);
    dk->addDgtr(443, 10323)->addDgtr(211,  9981)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "SBW_10323_NonRes"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_10323"));
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(443, 10323)->addDgtr(321,  9981)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "SBW_10323_NonRes"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_10323"));
    delete dk;
    dk = new DecayTree(521);
    dk->addDgtr(443, 10323)->addDgtr(-211,  9981)->addDgtr(321,211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "PLUS"));
    this->add(AmpInitialiser(*dk, "ZERO"));
    this->add(AmpInitialiser(*dk, "MINUS"));    
    this->add(AmpInitialiser(*dk, "SBW_10323_"));
    this->add(AmpInitialiser(*dk, "SBW_10323_NonRes"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_10323"));
    delete dk;
    
    // B->J/psi K f2(1270); f2->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 9975)->addDgtr(321, 225)->addDgtr(211,-211);
    this->add(*dk); 
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_9975"));
    
    // B->J/psi pi K2*(1430); K2*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 9975)->addDgtr(211, 315)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_9975"));
    delete dk;
    
    // B->J/psi pi K*(892); K*->K pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 9983)->addDgtr(211, 313)->addDgtr(321,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    
    // B->J/psi K rho(770); rho->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(443, 9983)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    this->add(AmpInitialiser(*dk, "NonResExp_9983GS"));
    dk->getVal().setL(1);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    this->add(AmpInitialiser(*dk, "NonResExp_9983GS"));
    delete dk;

    dk = new DecayTree(521);
    dk->addDgtr(443, 9993)->addDgtr(321, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_9993"));
    this->add(AmpInitialiser(*dk, "NonResExp_9993GS"));
    delete dk;
    
    dk = new DecayTree(521);
    dk->addDgtr(321, 9993)->addDgtr(443, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_9993"));
    this->add(AmpInitialiser(*dk, "NonResExp_9993GS"));
    delete dk;
    
    dk = new DecayTree(521);
    dk->addDgtr(321, 9983)->addDgtr(443, 113)->addDgtr(211,-211);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    this->add(AmpInitialiser(*dk, "NonResExp_9983GS"));
    delete dk;
    
    
    
    //B-> NV rho
    dk = new DecayTree(521);
    r1=DecayTree(9993);
    r1.addDgtr(443,321);
    r2=DecayTree(113);
    r2.addDgtr(211,-211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResGS"));
    this->add(AmpInitialiser(*dk, "NonResExp_9993"));
    this->add(AmpInitialiser(*dk, "NonResExp_9993GS"));
    delete dk;
        
    //B-> NA rho
    dk = new DecayTree(521);
    r1=DecayTree(9983);
    r1.addDgtr(443,321);
    r2=DecayTree(113);
    r2.addDgtr(211,-211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResGS"));
    this->add(AmpInitialiser(*dk, "NonResExp_9983"));
    this->add(AmpInitialiser(*dk, "NonResExp_9983GS"));
    delete dk;
    
    //B-> NP rho
    dk = new DecayTree(521);
    r1=DecayTree(9991);
    r1.addDgtr(443,321);
    r2=DecayTree(113);
    r2.addDgtr(211,-211);
    dk->addDgtr(&r1);
    dk->addDgtr(&r2);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "GS"));
    this->add(AmpInitialiser(*dk, "NonRes"));
    this->add(AmpInitialiser(*dk, "NonResGS"));
    this->add(AmpInitialiser(*dk, "NonResExp_9991"));
    this->add(AmpInitialiser(*dk, "NonResExp_9991GS"));
    delete dk;
    
    
    // B->K J/psi f0->pi pi  
    dk = new DecayTree(521);
    dk->addDgtr(321, 9993)->addDgtr(443, 9010221)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "Flatte")); 
    delete dk;
    
    dk = new DecayTree(521);
    dk->addDgtr(443, 9983)->addDgtr(321, 9010221)->addDgtr(211,-211);
    this->add(*dk);
    this->add(AmpInitialiser(*dk, "Flatte")); 
    delete dk;

    
    // Done
    return _trees.size();
}

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
	D->nonResP K-nonResP -> nonResVector pi+
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

  //Added by Matt 30/01/13
  //  name="D->K*(1410)+K-, K*(1410)+->K0*(892)pi+";
  dk = new DecayTree(421);
  dk->addDgtr(-321, 100323)->addDgtr(211, 313)->addDgtr(321, -211);
  this->add(*dk);
  if(dbThis) cout << "K*(1410) decay: " << *dk << endl;
  delete dk;

  // K0*(892) K0*(892)~    [P-wave]
  dk = new DecayTree(421);
  dk->addDgtr( 313)->addDgtr( 321, -211);
  dk->addDgtr(-313)->addDgtr(-321,  211);
  dk->getVal().setL(1);
  add(*dk);
  if(dbThis) cout << "K*0 K*0bar decay " << *dk << endl;
  delete dk;


  // K0*(892) Non-Res-V0   [P-wave]
  dk = new DecayTree(421);
  dk->addDgtr( 313)->addDgtr( 321, -211);
  dk->addDgtr(9993)->addDgtr(-321,211);
  dk->getVal().setL(1);
  add(*dk);
  if(dbThis) cout << "K0*(892) Non-Res-V0   [P-wave]" << *dk << endl;
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

  
 // Added by Molina on Wednesday 13th January 2010
 // pi- K1(1270)+, K1(1270)->pi+ K*(892)0 pi+, K*(892)0->pi+ K-
 // this decay makes no sense -leave it in for backward compatibility for now
 dk = new DecayTree(421); // mark for removal.
 dk->addDgtr(-211, 10323)->addDgtr(211, 313)->addDgtr(211, -321);
 add(*dk);
 if(dbThis) cout << "18th decay: " << *dk << endl;
 delete dk;
   

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
  DecayTree* a1=0;
    
  //D0 -> NV NT
  dk = new DecayTree(421);
  dk->addDgtr(9993)->addDgtr(211, -211);
  dk->addDgtr(9985)->addDgtr(211, -211);
  add(*dk);
  add(AmpInitialiser(*dk, "NonRes"));
  add(AmpInitialiser(*dk, "NonResExp"));
  delete dk;  
    
  //D0 -> NV NS
  dk = new DecayTree(421);
  dk->addDgtr(9993)->addDgtr(211, -211);
  dk->addDgtr(9981)->addDgtr(211, -211);
  add(*dk);
  add(AmpInitialiser(*dk, "NonRes"));
  add(AmpInitialiser(*dk, "NonResExp"));
  delete dk;    
    
  //D0 -> NV NV
  dk = new DecayTree(421);
  dk->addDgtr(9993)->addDgtr(211, -211);
  dk->addDgtr(9993)->addDgtr(211, -211);
  add(*dk);
  add(AmpInitialiser(*dk, "NonRes"));
  add(AmpInitialiser(*dk, "NonResExp"));
  delete dk;   

  //D0 -> NA pi-, NA->NV pi+
  dk = new DecayTree(421);
  dk->addDgtr(-211, 9983)->addDgtr(211, 9993)->addDgtr(211, -211);
  add(*dk);
  add(AmpInitialiser(*dk, "NonRes"));
  add(AmpInitialiser(*dk, "NonResExp"));
  delete dk;  
    
  //D0 -> a1(1260)pi-, a1(1260)->rho(770)pi+, rho->pi+pi-   [S-WAVE]
  dk = new DecayTree(421);
  dk->addDgtr(-211, 20213)->addDgtr(211, 113)->addDgtr(211, -211);
  add(*dk);  
  this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
  this->add(AmpInitialiser(*dk, "GS")); 
  if(dbThis) cout << "first decay: " << *dk << endl;
  delete dk;

  //D0 -> a1(1260)pi-, a1(1260)->rho(770)pi+, rho->pi+pi-   [D-WAVE decay of a1]
  dk = new DecayTree(421);
  a1 = new DecayTree(20213);
  a1->getVal().setL(2);
  a1->addDgtr(211, 113)->addDgtr(211, -211);
  dk->addDgtr(-211), dk->addDgtr(a1);
  add(*dk);
  this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
  this->add(AmpInitialiser(*dk, "GS"));   
  if(dbThis) cout << "first decay: " << *dk << endl;
  delete a1;
  delete dk;
    
  //D0 -> a1(1260)pi-, a1(1260)->rho(1450)pi+, rho->pi+pi-   [S-WAVE]
  dk = new DecayTree(421);
  dk->addDgtr(-211, 20213)->addDgtr(211, 100113)->addDgtr(211, -211);
  add(*dk);
  this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
  this->add(AmpInitialiser(*dk, "GS"));
  if(dbThis) cout << "first decay rho(1450): " << *dk << endl;
  delete dk;
    
  //D0 -> a1(1260)pi-, a1(1260)->rho(1450)pi+, rho->pi+pi-   [D-WAVE decay of a1]
  dk = new DecayTree(421);
  a1 = new DecayTree(20213);
  a1->getVal().setL(2);
  a1->addDgtr(211, 100113)->addDgtr(211, -211);
  dk->addDgtr(-211), dk->addDgtr(a1);
  add(*dk);
  this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
  this->add(AmpInitialiser(*dk, "GS"));   
  if(dbThis) cout << "first decay rho(1450): " << *dk << endl;
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

  //D0 -> a1(1260)pi-, a1(1260)->sigma_1 pi+
  dk = new DecayTree(421);
  dk->addDgtr(-211, 20213)->addDgtr(211,999001)->addDgtr(211, -211);
  add(*dk);
  this->add(AmpInitialiser(*dk, "Bugg"));
  if(dbThis) cout << "second decay: " << *dk << endl;
  delete dk;
    
  //D0 -> a1(1260)pi-, a1(1260)->f2(1270) pi+, rho->pi+pi-
  dk = new DecayTree(421);
  dk->addDgtr(-211, 20213)->addDgtr(211,223)->addDgtr(211, -211);
  add(*dk);
  delete dk;
    
  //D0 -> a1(1260)pi-, a1(1260)->f0(1370) pi+, rho->pi+pi-
  dk = new DecayTree(421);
  dk->addDgtr(-211, 20213)->addDgtr(211,10221)->addDgtr(211, -211);
  add(*dk);
  delete dk;
        
    //D0 -> a1(1640)pi-, a1->rho(770)pi+, rho->pi+pi-   [S-WAVE]
    dk = new DecayTree(421);
    dk->addDgtr(-211, 920213)->addDgtr(211, 113)->addDgtr(211, -211);
    add(*dk);  
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS")); 
    if(dbThis) cout << "first decay: " << *dk << endl;
    delete dk;
    
    //D0 -> a1(1640)pi-, a1->rho(770)pi+, rho->pi+pi-   [D-WAVE decay of a1]
    dk = new DecayTree(421);
    a1 = new DecayTree(920213);
    a1->getVal().setL(2);
    a1->addDgtr(211, 113)->addDgtr(211, -211);
    dk->addDgtr(-211), dk->addDgtr(a1);
    add(*dk);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));   
    if(dbThis) cout << "first decay: " << *dk << endl;
    delete a1;
    delete dk;
    
    //D0 -> a1(1640)pi-, a1(1640)->rho(1450)pi+, rho->pi+pi-   [S-WAVE]
    dk = new DecayTree(421);
    dk->addDgtr(-211, 920213)->addDgtr(211, 100113)->addDgtr(211, -211);
    add(*dk);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    if(dbThis) cout << "first decay rho(1450): " << *dk << endl;
    delete dk;
    
    //D0 -> a1(1640)pi-, a1(1640)->rho(1450)pi+, rho->pi+pi-   [D-WAVE decay of a1]
    dk = new DecayTree(421);
    a1 = new DecayTree(920213);
    a1->getVal().setL(2);
    a1->addDgtr(211, 100113)->addDgtr(211, -211);
    dk->addDgtr(-211), dk->addDgtr(a1);
    add(*dk);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));  
    if(dbThis) cout << "first decay rho(1450): " << *dk << endl;
    delete a1;
    delete dk;
    
    //D0 -> a1(1640)pi-, a1(1640)->sigma_1 pi+, rho->pi+pi-
    dk = new DecayTree(421);
    dk->addDgtr(-211, 920213)->addDgtr(211,999001)->addDgtr(211, -211);
    add(*dk);
    this->add(AmpInitialiser(*dk, "Bugg"));
    if(dbThis) cout << "second decay: " << *dk << endl;
    delete dk;
    
    //D0 -> a1(1640)pi-, a1(1640)->f2(1270) pi+, rho->pi+pi-
    dk = new DecayTree(421);
    dk->addDgtr(-211, 920213)->addDgtr(211,223)->addDgtr(211, -211);
    add(*dk);
    delete dk;
    
    //D0 -> a1(1640)pi-, a1(1640)->f0(1370) pi+
    dk = new DecayTree(421);
    dk->addDgtr(-211, 920213)->addDgtr(211,10221)->addDgtr(211, -211);
    add(*dk);
    delete dk;
    
    //D0 -> a1Bkg pi-, a1->rho(770)pi+, rho->pi+pi-   [S-WAVE]
    dk = new DecayTree(421);
    dk->addDgtr(-211, 9920213)->addDgtr(211, 113)->addDgtr(211, -211);
    add(*dk); 
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS")); 
    if(dbThis) cout << "first decay: " << *dk << endl;
    delete dk;
    
    //D0 -> a1Bkg pi-, a1->rho(770)pi+, rho->pi+pi-   [D-WAVE decay of a1]
    dk = new DecayTree(421);
    a1 = new DecayTree(9920213);
    a1->getVal().setL(2);
    a1->addDgtr(211, 113)->addDgtr(211, -211);
    dk->addDgtr(-211), dk->addDgtr(a1);
    add(*dk);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));  
    if(dbThis) cout << "first decay: " << *dk << endl;
    delete a1;
    delete dk;
    
    //D0 -> a1Bkg pi-, a1->rho(1450)pi+, rho->pi+pi-   [S-WAVE]
    dk = new DecayTree(421);
    dk->addDgtr(-211, 9920213)->addDgtr(211, 100113)->addDgtr(211, -211);
    add(*dk);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));
    if(dbThis) cout << "first decay rho(1450): " << *dk << endl;
    delete dk;
    
    //D0 -> a1Bkg pi-, a1->rho(1450)pi+, rho->pi+pi-   [D-WAVE decay of a1]
    dk = new DecayTree(421);
    a1 = new DecayTree(920213);
    a1->getVal().setL(2);
    a1->addDgtr(211, 100113)->addDgtr(211, -211);
    dk->addDgtr(-211), dk->addDgtr(a1);
    add(*dk);
    this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
    this->add(AmpInitialiser(*dk, "GS"));    
    if(dbThis) cout << "first decay rho(1450): " << *dk << endl;
    delete a1;
    delete dk;
    
    //D0 -> a1Bkg pi-, a1(1640)->sigma_1 pi+, rho->pi+pi-
    dk = new DecayTree(421);
    dk->addDgtr(-211, 9920213)->addDgtr(211,999001)->addDgtr(211, -211);
    add(*dk);
    this->add(AmpInitialiser(*dk, "Bugg"));
    if(dbThis) cout << "second decay: " << *dk << endl;
    delete dk;
    
    //D0 -> a1Bkg pi-, a1(1640)->f2(1270) pi+, rho->pi+pi-
    dk = new DecayTree(421);
    dk->addDgtr(-211, 9920213)->addDgtr(211,223)->addDgtr(211, -211);
    add(*dk);
    delete dk;
    
    //D0 -> a1Bkg pi-, a1(1640)->f0(1370) pi+
    dk = new DecayTree(421);
    dk->addDgtr(-211, 9920213)->addDgtr(211,10221)->addDgtr(211, -211);
    add(*dk);
    delete dk;

  //D0 -> a2(1320)pi-, a2(1230)->pi+pi+pi-
  //dk = new DecayTree(421);
  //dk->addDgtr(-211, 215)->addDgtr(211,-211,211);
  //add(*dk);
  //delete dk;
    
  //D0 -> rho(770)rho(770), rho(770)->pi+pi-  [S-WAVE]
  dk = new DecayTree(421);
  dk->addDgtr(113)->addDgtr(211, -211);
  dk->addDgtr(113)->addDgtr(211, -211);
  add(*dk);
  this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
  this->add(AmpInitialiser(*dk, "GS"));    
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
  this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
  this->add(AmpInitialiser(*dk, "GS"));  
  if(dbThis) cout << "third decay: " << *dk << endl;
  delete dk;

  //D0 -> rho(770)rho(770), rho(770)->pi+pi-  [D-WAVE]
  dk = new DecayTree(421);
  dk->addDgtr(113)->addDgtr(211, -211);
  dk->addDgtr(113)->addDgtr(211, -211);
  dk->getVal().setL(2);
  add(*dk);
  this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
  this->add(AmpInitialiser(*dk, "GS")); 
  if(dbThis) cout << "third decay: " << *dk << endl;
  delete dk;

  //D0 -> f0(980)pi-pi+, f0(980)->pi+pi-
  //Adding no resonant decays to help with the spin factors
  dk = new DecayTree(421);
  dk->addDgtr(9010221)->addDgtr(211, -211);
  dk->addDgtr(9010221)->addDgtr(211, -211);
  // dk->getVal().setL(0);
  add(*dk);
  add(AmpInitialiser(*dk, "Flatte"));
  if(dbThis) cout << "fourth decay: " << *dk << endl;
  delete dk;

  //D0 -> f2(1270)pi-pi+, f2(1270)->pi+pi-
  //Adding no resonant decays to help with the spin factors
  /*dk = new DecayTree(421);
  dk->addDgtr(225)->addDgtr(211, -211);
  dk->addDgtr(225)->addDgtr(211, -211);
  //dk->getVal().setL(0);
  add(*dk);
  if(dbThis) cout << "fifth decay: " << *dk << endl;
  delete dk;
  */
  //D0 -> sigma_1 pi-pi+, sigma_1->pi+pi-
  dk = new DecayTree(421);
  dk->addDgtr(999001)->addDgtr(211, -211);
  dk->addDgtr(999001)->addDgtr(211, -211);
  add(*dk);
  this->add(AmpInitialiser(*dk, "Bugg"));
  if(dbThis) cout << "sixth decay: " << *dk << endl;
  delete dk;
    
  //D0 -> pi+pi-pi+pi-  Added by JBenton
  dk = new DecayTree(421);
  dk->addDgtr(211, -211, 211, -211);
  add(*dk);
  if(dbThis) cout << "seventh decay: " << *dk << endl;
  delete dk;
    
  //D0 -> a1(1260)pi-, a1(1260)->omega(782) pi+, rho->pi+pi- Added by JBenton
  dk = new DecayTree(421);
  dk->addDgtr(-211, 20213)->addDgtr(211,223)->addDgtr(211, -211);
  add(*dk);
  if(dbThis) cout << "eighth decay: " << *dk << endl;
  delete dk;

  //D0 -> omega(782)omega(782), omega(782)->pi+pi- Added by JBenton
  dk = new DecayTree(421);
  dk->addDgtr(223)->addDgtr(211, -211);
  dk->addDgtr(223)->addDgtr(211, -211);
  add(*dk);
  if(dbThis) cout << "ninth decay: " << *dk << endl;
  delete dk;    
  
  //D0 -> omega(782)rho(770), rho(770)->pi+pi-  Added by JBenton
  dk = new DecayTree(421);
  dk->addDgtr(223)->addDgtr(211, -211);
  dk->addDgtr(113)->addDgtr(211, -211);
  add(*dk);
  if(dbThis) cout << "ninth decay: " << *dk << endl;
  delete dk;
    
  //D0 -> sigma_1 rho(770), rho(770)->pi+pi-  Added by JBenton
  dk = new DecayTree(421);
  dk->addDgtr(999001)->addDgtr(211, -211);
  dk->addDgtr(113)->addDgtr(211, -211);
  add(*dk);
  this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
  this->add(AmpInitialiser(*dk, "GS"));  
  this->add(AmpInitialiser(*dk, "Bugg"));
  this->add(AmpInitialiser(*dk, "BuggGS"));
  this->add(AmpInitialiser(*dk, "BuggRHO_OMEGA"));

  if(dbThis) cout << "tenth decay: " << *dk << endl;
  delete dk;  
    
  //D0 -> rho(770) pi-pi+, rho(770)->NonRes pi+pi- Added by JBenton
  dk = new DecayTree(421);
  dk->addDgtr(113)->addDgtr(211, -211);
  dk->addDgtr(9993)->addDgtr(211, -211);
  add(*dk);
  add(AmpInitialiser(*dk, "NonResExp"));
  this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
  this->add(AmpInitialiser(*dk, "GS"));  
  if(dbThis) cout << "eleventh decay: " << *dk << endl;
  delete dk;
    
  //D0 -> omega(782) pi-pi+, omega(782)->NonRes pi+pi- Added by JBenton
  dk = new DecayTree(421);
  dk->addDgtr(223)->addDgtr(211, -211);
  dk->addDgtr(9993)->addDgtr(211, -211);
  add(*dk);
  if(dbThis) cout << "twelfth decay: " << *dk << endl;
  delete dk;
    
  // D0 -> f0(1370) pi-pi+, fo(1370)->NonRes pi+pi-
  dk = new DecayTree(421);
  dk->addDgtr(30221)->addDgtr(211, -211);
  dk->addDgtr(9993)->addDgtr(211, -211);
  add(*dk);
  add(AmpInitialiser(*dk, "NonResExp"));
  delete dk;
    
  // D0 -> rho(1450) pi-pi+, rho(1450)->NonRes pi+pi-
  dk = new DecayTree(421);
  dk->addDgtr(100113)->addDgtr(211, -211);
  dk->addDgtr(9993)->addDgtr(211, -211);
  add(*dk);
  this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
  this->add(AmpInitialiser(*dk, "GS"));    
  delete dk;
  
  // added on 9 Oct 2013, using new/refurbished spin factors (mainly tensor/pseudo tensor)

  // D0 -> pi(1300)+ pi-, pi(1300)+ ->rho(770) pi+, rhp(770) -> pi+pi-
  dk = new DecayTree(421);
  dk->addDgtr(-211,100211)->addDgtr(211, 113)->addDgtr(211, -211);
  add(*dk);
  this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
  this->add(AmpInitialiser(*dk, "GS"));  
  delete dk;

  // D0 -> pi(1300)+ pi-, pi(1300)+ ->NS pi+
  dk = new DecayTree(421);
  dk->addDgtr(-211,100211)->addDgtr(211, 9981)->addDgtr(211, -211);
  add(*dk);
  add(AmpInitialiser(*dk, "NonResExp"));
  delete dk;
    
  // D0 -> pi(1300)+ pi-, pi(1300)+ ->NV pi+
  dk = new DecayTree(421);
  dk->addDgtr(-211,100211)->addDgtr(211, 9993)->addDgtr(211, -211);
  add(*dk);
  delete dk;

  // D-> f2(1270) f2(1270) 
  dk = new DecayTree(421);
  dk->addDgtr(225)->addDgtr(211, -211);
  dk->addDgtr(225)->addDgtr(211, -211);
  add(*dk);
  dk->getVal().setL(1);
  add(*dk);
  dk->getVal().setL(2);
  add(*dk);
  delete dk;

  // D->a2(1320) pi, a2(1320)->rho pi, rho->pi pi
  dk = new DecayTree(421);
  dk->addDgtr(-211, 215)->addDgtr(211, 113)->addDgtr(-211, 211);
  add(*dk);
  this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
  this->add(AmpInitialiser(*dk, "GS"));  
  delete dk;

  // D->a2(1320) pi, a2(1320)->f2(1270) pi, f2-> rho pi
  // (kinematically challenged, but broad resonances)
  dk = new DecayTree(421);
  dk->addDgtr(-211, 215)->addDgtr(211, 225)->addDgtr(-211, 211);
  add(*dk);
  delete dk;

  // D->a2(1700) pi, a2(1700)->f2(1270) pi, f2-> rho pi
  dk = new DecayTree(421);
  dk->addDgtr(-211, 100215)->addDgtr(211, 225)->addDgtr(-211, 211);
  add(*dk);
  delete dk;

  // pseudo tensor modes with pi2(1670)
  // D -> pi2(1670) pi, pi2->f2(1270) pi, f2->pipi
  dk = new DecayTree(421);
  dk->addDgtr(-211, 10215)->addDgtr(211, 225)->addDgtr(211, -211);
  add(*dk);
  delete dk;

  // D -> pi2(1670) pi, pi2->f0(600) pi, f0(600)->pipi
  dk = new DecayTree(421);
  dk->addDgtr(-211, 10215)->addDgtr(211, 9000221)->addDgtr(211, -211);
  this->add(AmpInitialiser(*dk, "Bugg"));
  add(*dk);
  delete dk;

  // D -> pi2(1670) pi, pi2->rho pi
  dk = new DecayTree(421);
  dk->addDgtr(-211, 10215)->addDgtr(211, 113)->addDgtr(211, -211);
  add(*dk);
  this->add(AmpInitialiser(*dk, "RHO_OMEGA"));
  this->add(AmpInitialiser(*dk, "GS"));  
  delete dk;

  // D -> pi(1300) pi, pi(1300)->f(600) pi (trivial spin factor)
  dk = new DecayTree(421);
  dk->addDgtr(-211, 100211)->addDgtr(211, 9000221)->addDgtr(-211,211);
  this->add(AmpInitialiser(*dk, "Bugg"));
  add(*dk);
  delete dk;

  //  D->a1(1260) pi, a1 -> f2(1270) pi 
  dk = new DecayTree(421);
  dk->addDgtr(-211, 20213)->addDgtr(211, 225)->addDgtr(211, -211);
  add(*dk);
  delete dk;

 
  // D0 -> omega3(1670)- pi+, omega3(1670)- ->rho(770) pi-, rhp(770) -> pi+pi-
  /*dk = new DecayTree(421);
  dk->addDgtr(-211,227)->addDgtr(211, 113)->addDgtr(211, -211);
  add(*dk);
  delete dk;
  */  
  return _trees.size();
}

std::ostream& operator<<(std::ostream& os, const NamedDecayTreeList& ndtl){
  ndtl.print(os);
  return os;
}

//

