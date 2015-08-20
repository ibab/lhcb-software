// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT
#include "Mint/Amplitude.h"
#include "Mint/SpinFactorMaker.h"
#include "Mint/LineshapeMaker.h"
#include "Mint/Utils.h"

#include "Mint/Permutator.h"

#include <algorithm>

using namespace std;
using namespace MINT;

Amplitude::Amplitude( const DecayTree& decay
		      , char SPD_Wave
		      , const std::string& lopt
		      , const std::vector<double>& numOpts
		      , IFitParRegister* daddy
		      ) 
  : FitParDependent(daddy)
  , _associatingDecayTree(decay)
  , _spinFactor(0)
  , _spd(SPD_Wave)
  , _lopt(lopt)
  , _numOpts(numOpts)
  , _pat()
  , _init(false)
{
  //createDependants();
}

Amplitude::Amplitude( const AmpInitialiser& ampInit
		      , IFitParRegister* daddy
		      )
  : FitParDependent(daddy)
  , _associatingDecayTree(ampInit.tree())
  , _spinFactor(0)
  , _spd(ampInit.SPD())
  , _lopt(ampInit.lopt())
  , _numOpts(ampInit.numOpts())
  , _pat()
  , _init(false)
{
  //createDependants();
}

Amplitude::Amplitude( const Amplitude& other
		      , IFitParRegister* newDaddy)
//: IReturnRealForEvent<IDalitzEvent>()
  : FitParDependent(other, newDaddy)
  , IReturnComplexForEvent<IDalitzEvent>()
  , _associatingDecayTree(other._associatingDecayTree)
  , _spinFactor(0)
  , _spd(other._spd)
  , _lopt(other._lopt)
  , _numOpts(other._numOpts)
  , _pat(other._pat)
  , _init(false)
{
  // association is done, rest needs to be re-done.
  deleteDependants();
}

Amplitude::~Amplitude(){
  deleteDependants();
}

bool Amplitude::deleteDependants(){
  _init=false;
  this->removeAllFitParDependencies(); // risky
  deleteLineshapes();
  if(0 != _spinFactor) delete _spinFactor;
  _spinFactor=0;
  return true;
}
bool Amplitude::createDependants(){
  bool dbThis=false;
  if(dbThis){
    cout << "Amplitude::createDependants, for " << name()
	 << ". Calling SpinFactorMake with _spd = " << _spd
	 << " and _lopt = " << _lopt << endl;
      }
  _spinFactor = SpinFactorMaker(theBareDecay(), _spd, _lopt);
  if(dbThis) cout << "got this spin factor: " << _spinFactor->name() << endl;
  if(0 == _spinFactor) return false;
  bool cl = createLineshapes();

  cout << "Amplitude::createDependants() after creating lineshape, I depend on these fitParameters:"
       << endl;
  this->listFitParDependencies(cout);

  if(0 == cl) return false;
  return true;
}
bool Amplitude::renew(){
  deleteDependants();
  _init = createDependants();
  if(! _init){
    cout << "ERROR: Amplitude::renew() failed" << endl;
  }
  return _init;
}
bool Amplitude::resetTree(const DecayTree& dt){
  AssociatingDecayTree newTree(dt);
  _associatingDecayTree = newTree;
  return deleteDependants();
}
bool Amplitude::CPConjugate(){
  DecayTree dt = theBareDecay();
  anti(dt);
  return resetTree(dt);
}

bool Amplitude::addLineshape(ILineshape* lsPtr){
  if(0 == lsPtr) return false;
  this->registerFitParDependence(*lsPtr);
  _LineshapeList.push_back(lsPtr);
  return true;
}

bool Amplitude::createLineshapes(const const_counted_ptr<AssociatedDecayTree>& 
				 counted_tree_ptr){
  return createLineshapes(counted_tree_ptr.get());
}
bool Amplitude::createLineshapes(const AssociatedDecayTree* treePtr){
  bool dbThis=false;
  if(0==treePtr) {
    /*
    cout << "Amplitude::createLineshapes for treePtr: \n" 
	 << theDecay()
	 << "\n first call in recursive operation" << endl;
    */
    if(_pat.empty()){
      cout << "Amplitude::createLineshapes: cannot create line-shapes"
	   << " unless _pat is set. Bailing out." 
	   << endl;
      throw "no lineshapes without pattern";
    }
    return createLineshapes(& theDecay(_pat));
  }
  //  cout << "         >> " << treePtr->oneLiner() << endl;

  bool success=true;
  if(treePtr->nDgtr() >= 2){
    addLineshape(LineshapeMaker(treePtr, _lopt, _numOpts));
    //    LineshapeList.push_back(LineshapeMaker(treePtr, _lopt));
    if(dbThis){
      cout << "Amplitude::createLineshapes: just added lineshape: ";
      _LineshapeList.back()->print(cout);
      cout << endl;
    }
  }
  for(int i=0; i< treePtr->nDgtr(); i++){
    success &= createLineshapes(treePtr->getDgtrTreePtr(i));
  }
  return success;
}
bool Amplitude::deleteLineshapes(){
  for(std::vector<ILineshape*>::iterator it = _LineshapeList.begin();
      it != _LineshapeList.end(); it++){
    if(0 != *it) delete (*it);
  }
  _LineshapeList.clear();
  return true;
}

std::complex<double> Amplitude::SpinFactorValue(IDalitzEvent& evt){
  bool dbThis=false;
  if(dbThis) {
    cout << "amplitude " << name()
	 << " calling spin factor: "
	 << spinFactor()->name() << endl;
  }
  if(0 == spinFactor()) initialise(evt.eventPattern());
  if(dbThis){
    cout << " spin factor value is: "
	 << spinFactor()->getVal(evt) << endl;
  }
  return spinFactor()->ComplexVal(evt);
}

std::complex<double> Amplitude::LineshapeProduct(IDalitzEvent& evt){
  //bool dbThis=false;
  std::complex<double> prod=1;
  /*
   if(dbThis){
    cout << "Amplitude::LineshapeProduct() for " << name() << endl;
    cout << " ...calling lineshapes: " << endl;
  }
   */
  for(std::vector<ILineshape*>::iterator it = _LineshapeList.begin();
      it != _LineshapeList.end(); it++){
    //if(dbThis && 0 != *it) cout << (*it)->name() << ", " << (*it)->getVal(evt);
    if(0 != *it) prod *= (*it)->getVal(evt);
  }
  //if(dbThis) cout << " done. Returning " << prod << endl;
  return prod;
}

DalitzBoxSet Amplitude::MakeBoxes(const DalitzEventPattern& pat
				  , double nSigma){
  Permutator perms(pat);
  DalitzBoxSet v;

  
  for(unsigned int i=0; i < perms.size(); i++){
    v.add(MakeBox(pat, perms[i], nSigma));
  }

  return v;
}

DalitzBoxSet Amplitude::MakeBox(const DalitzEventPattern& pat
				, const Permutation& perm
				, double nSigma
				){
  cout << "making box for " << (*this) << endl;
  DalitzBoxSet v;
  double maxHeight = 1;// norm(this->getValAtResonance());
  double maxHeight_2 = maxHeight;
  if(_LineshapeList.size() >= 2){
    maxHeight_2 *=  exp(-0.5*(nSigma*nSigma*0.75));
  }
  std::vector<DalitzCoordinate> limits;
  for(std::vector<ILineshape*>::iterator it = _LineshapeList.begin();
      it != _LineshapeList.end(); it++){
    DalitzCoordinate coord ((*it)->getDalitzCoordinate(nSigma).mapMe(perm) );
    if(coord.size() <= 3){ // otherwise it's the D itself
      DalitzBox box(pat, coord);
      box.setName(this->theBareDecay().oneLiner() 
		  + " limit: " + coord.name()
		  + " " + anythingToString((int)nSigma) + " sigma");
      box.encloseInPhaseSpaceArea();
      box.setGuessedHeight(maxHeight_2);
      v.push_back(box);

      limits.push_back( coord );
    }
  }
  DalitzBox lastBox(pat, limits);
  lastBox.setGuessedHeight(1);//norm(this->getValAtResonance()));
  cout << " just set box height, it's " << lastBox.guessedHeight() << endl;
  lastBox.setName(this->theBareDecay().oneLiner() 
	      + " all limits"
	      + " " + anythingToString((int)nSigma) + " sigma");
  cout << " .. now it is " << lastBox.guessedHeight() << endl;
  lastBox.encloseInPhaseSpaceArea();
  cout << "Amp " << name() << " made box with guessed height " 
       << 1//norm(1)//this->getValAtResonance())
       << " = " << lastBox.guessedHeight()
       << "\n   that's the one" << lastBox
       << endl;
  v.push_back(lastBox);
  return v;
}


DalitzBWBoxSet Amplitude::MakeBWBoxes(const DalitzEventPattern& pat
				      , TRandom* rnd
				      ){
  bool dbThis=false;
  Permutator perms(pat);
  DalitzBWBoxSet v(rnd);

  for(unsigned int i=0; i < perms.size(); i++){
    DalitzBWBox box(MakeBWBox(pat, perms[i], rnd));
    box.height() = boxFactor()/perms.size();
    if(dbThis){ 
      cout << "setting boxHeight to " << box.height()
	   << " = " << boxFactor() << " / " << perms.size()
	   << endl;
    }
    v.add(box);
  }

  return v;
}

DalitzBWBox Amplitude::MakeBWBox(const DalitzEventPattern& pat
				 , const Permutation& perm
				 , TRandom* rnd
				 ){
  bool dbThis=false;
  if(dbThis) cout << "making bw-box for " << (*this) << endl;

  std::vector<counted_ptr<IGenFct> > limits;
  for(std::vector<ILineshape*>::iterator it = _LineshapeList.begin();
      it != _LineshapeList.end(); it++){
    if(dbThis){
      cout << "adding co-ordinate " 
	   << (*it)->getDalitzCoordinate()
	   << " to box." << endl;
    }
    DalitzCoordinate coord ((*it)->getDalitzCoordinate().mapMe(perm) );
    counted_ptr<IGenFct> fct( (*it)->generatingFunction() );
    fct->setCoordinate(coord);
    if((int) coord.size() < pat.numDaughters()){ 
      // otherwise it's the D itself (coord.size is the number of 
      // indices in s_ij..)
      // so s12 or s23 have 2, s123 has 3, s1234 has 4.)
      limits.push_back(fct);
    }
  }
  DalitzBWBox box(pat, limits, 0, rnd);
  box.setName(this->theBareDecay().oneLiner() 
	      + " BW");
  return box;
}


/*
double Amplitude::LineshapeGaussProduct(){
  //  return 1;//
  double prod=1;

  for(std::vector<ILineshape*>::iterator it = LineshapeList.begin();
      it != LineshapeList.end(); it++){
    if(0 != *it) prod *= (*it)->gaussianApprox().getVal();
  }
  return prod;
}
*/

std::complex<double> Amplitude::getOnePermutationsVal(IDalitzEvent& evt){
  bool dbThis=false;

  std::complex<double> sf = SpinFactorValue(evt);
  complex<double> ls = LineshapeProduct(evt);
  complex<double> returnVal =  sf*ls;

  if(dbThis){
    //if(LineshapeList.size() >=2){
    //  complex<double> evtGen = LineshapeList[1]->EvtGenValue();
    //  complex<double> diff = returnVal - evtGen;
    //  if(abs(diff) > 1.e-10){
    //	cout << " EvtGenCheck: " << returnVal << " - " << evtGen
    //	     << " = " << diff << endl;
    //  }
    // }

    cout << theBareDecay().oneLiner() << ":"
	 << "\n   >  spinFactor       " << sf
	 << "\n   >  LineshapeProduct " << ls
	 << "\n   >  Returning:       " << returnVal
	 << endl;
    cout << "-----------------------------------------" << endl;
  }
  return returnVal;
}

std::complex<double> Amplitude::getVal(IDalitzEvent* evt){
  if(0 == evt) return 0;
  return this->getVal(*evt);
}

std::complex<double> Amplitude::getNewVal(IDalitzEvent& evt){
  //bool dbThis=false;

  initialiseIfNeeded(evt.eventPattern());

  complex<double> sum=0;

  if(! evt.eventPattern().compatibleWithFinalState(getTreePattern())) return 0;

  for(int i=0; i < evt.numPermutations(); i++){
    evt.setPermutationIndex(i);
    complex<double> thisVal= getOnePermutationsVal(evt);
    /*
    if(dbThis){
      cout << " permutation " << i
	   << " makes event look like this: ";
      getEvent()->print();
      cout << "\n gets thisVal " << thisVal << endl;
    }
    */
    sum += thisVal;
  }
  evt.setPermutationIndex(0);
  sum /= sqrt((double) evt.numPermutations());

  /*
  if(dbThis && abs(sum) > sqrt((double) 1000)){
    cout << " Amplitude : " << (*this)
	 << " returning a large value: " << sum
	 << endl;
  }
  
  if(dbThis){
    cout << "Amplitude::getVal() for " 
	 << name() << endl;
    cout << " returning " << sum << endl;
    cout << "==========================================" << endl;
  }
  */
  return sum;
}


std::string Amplitude::name() const{
  return (std::string) "(" 
    + _spd 
    + " wave) with opt " + _lopt + "; "
    + theBareDecay().oneLiner();
}

void Amplitude::print(std::ostream& out) const{
  out << "(" << _spd << " wave)\n" << theBareDecay();
}

std::ostream& operator<<(std::ostream& out, const Amplitude& amp){
  amp.print(out);
  return out;
}

//
