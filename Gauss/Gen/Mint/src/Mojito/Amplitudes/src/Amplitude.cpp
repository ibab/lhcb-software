// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT
#include "Mint/Amplitude.h"
#include "Mint/IDalitzEventAccess.h"
#include "Mint/SpinFactorMaker.h"
#include "Mint/LineshapeMaker.h"
#include "Mint/Utils.h"

#include "Mint/Permutator.h"

#include <algorithm>

using namespace std;
using namespace MINT;

Amplitude::Amplitude( const DecayTree& decay
		      , IDalitzEventAccess* events
		      , char SPD_Wave
		      , const std::string& lopt
		      ) 
  : IDalitzEventAccess()
  , DalitzEventAccess(events)
  , _associatingDecayTree(decay, (IDalitzEventAccess*) this)
  , _spinFactor(0)
  , _spd(SPD_Wave)
  , _lopt(lopt)
{
  createDependants();
}
Amplitude::Amplitude( const DecayTree& decay
		      , IDalitzEventList* events
		      , char SPD_Wave
		      , const std::string& lopt
		      ) 
  : IDalitzEventAccess()
  , DalitzEventAccess(events)
  , _associatingDecayTree(decay, (IDalitzEventAccess*) this)
  , _spinFactor(0)
  , _spd(SPD_Wave)
  , _lopt(lopt)
{
  createDependants();
}

Amplitude::Amplitude( const AmpInitialiser& ampInit
		      , IDalitzEventAccess* events
		      )
  : IDalitzEventAccess()
  , DalitzEventAccess(events)
  , _associatingDecayTree(ampInit.tree(),(IDalitzEventAccess*)this)
  , _spinFactor(0)
  , _spd(ampInit.SPD())
  , _lopt(ampInit.lopt())
{
  createDependants();
}
Amplitude::Amplitude( const AmpInitialiser& ampInit
		      , IDalitzEventList* events
		      )
  : IDalitzEventAccess()
  , DalitzEventAccess(events)
  , _associatingDecayTree(ampInit.tree(),(IDalitzEventAccess*)this)
  , _spinFactor(0)
  , _spd(ampInit.SPD())
  , _lopt(ampInit.lopt())
{
  createDependants();
}
Amplitude::Amplitude( const Amplitude& other)
  : IBasicEventAccess<IDalitzEvent>()
  , IEventAccess<IDalitzEvent>()
  , IDalitzEventAccess()
  , IReturnReal()
  , IGetRealEvent<IDalitzEvent>()
  , IReturnComplex()
  , DalitzEventAccess(other)
  , _associatingDecayTree(other._associatingDecayTree, this)
  , _spinFactor(0)
  , _spd(other._spd)
  , _lopt(other._lopt)
{
  // association is done, rest needs to be re-done.
  renew();
}
Amplitude::Amplitude( const Amplitude& other, IDalitzEventAccess* newEvents)
  : IDalitzEventAccess()
  , IReturnReal()
  , IReturnComplex()
  , DalitzEventAccess(newEvents)
  , _associatingDecayTree(other._associatingDecayTree, this)
  , _spinFactor(0)
  , _spd(other._spd)
  , _lopt(other._lopt)
{
  // association is done, rest needs to be re-done.
  renew();
}
Amplitude::Amplitude( const Amplitude& other, IDalitzEventList* newEvents)
  : IDalitzEventAccess()
  , IReturnReal()
  , IReturnComplex()
  , DalitzEventAccess(newEvents)
  , _associatingDecayTree(other._associatingDecayTree, this)
  , _spinFactor(0)
  , _spd(other._spd)
  , _lopt(other._lopt)
{
  // association is done, rest needs to be re-done.
  renew();
}

Amplitude::~Amplitude(){
  deleteDependants();
}

bool Amplitude::deleteDependants(){
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
  _spinFactor = SpinFactorMaker(theDecay(), this, _spd, _lopt);
  if(dbThis) cout << "got this spin factor: " << _spinFactor->name() << endl;
  if(0 == _spinFactor) return false;
  bool cl = createLineshapes();
  if(0 == cl) return false;
  return true;
}
bool Amplitude::renew(){
  deleteDependants();
  return createDependants();
}
bool Amplitude::resetTree(const DecayTree& dt){
  AssociatingDecayTree newTree(dt, this);
  _associatingDecayTree = newTree;
  return renew();
}
bool Amplitude::CPConjugate(){
  DecayTree dt = theDecay();
  anti(dt);
  return resetTree(dt);
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
    return createLineshapes(& theDecay());
  }
  //  cout << "         >> " << treePtr->oneLiner() << endl;

  bool success=true;
  if(treePtr->nDgtr() >= 2){
    LineshapeList.push_back(LineshapeMaker(treePtr, this, _lopt));
    if(dbThis){
      cout << "Amplitude::createLineshapes: just added lineshape: ";
      LineshapeList.back()->print(cout);
      cout << endl;
    }
  }
  for(int i=0; i< treePtr->nDgtr(); i++){
    success &= createLineshapes(treePtr->getDgtrTreePtr(i));
  }
  return success;
}
bool Amplitude::deleteLineshapes(){
  for(std::vector<ILineshape*>::iterator it = LineshapeList.begin();
      it != LineshapeList.end(); it++){
    if(0 != *it) delete (*it);
  }
  LineshapeList.clear();
  return true;
}

double Amplitude::SpinFactorValue(){
  bool dbThis=false;
  if(dbThis) {
    cout << "amplitude " << name()
	 << " calling spin factor: "
	 << spinFactor()->name() << endl;
  }
  return spinFactor()->getVal();
}

std::complex<double> Amplitude::LineshapeProduct(){
  bool dbThis=false;
  std::complex<double> prod=1;
  if(dbThis){
    cout << "Amplitude::LineshapeProduct() for " << name() << endl;
    cout << " ...calling lineshapes: " << endl;
  }
  for(std::vector<ILineshape*>::iterator it = LineshapeList.begin();
      it != LineshapeList.end(); it++){
    if(dbThis && 0 != *it) cout << (*it)->name() << ", " << (*it)->getVal();
    if(0 != *it) prod *= (*it)->getVal();
  }
  if(dbThis) cout << " done." << endl;
  return prod;
}
std::complex<double> Amplitude::LineshapeProductAtResonance(){
  //  return 1;//
  std::complex<double> prod=1;

  for(std::vector<ILineshape*>::iterator it = LineshapeList.begin();
      it != LineshapeList.end(); it++){
    if(0 != *it) prod *= (*it)->getValAtResonance();
  }
  return prod;
}

std::complex<double> Amplitude::LineshapeProductSmootherLarger(){
  //  return 1;//
  std::complex<double> prod=1;

  for(std::vector<ILineshape*>::iterator it = LineshapeList.begin();
      it != LineshapeList.end(); it++){
    if(0 != *it) prod *= (*it)->getSmootherLargerVal();
  }
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
  double maxHeight = norm(this->getValAtResonance());
  double maxHeight_2 = maxHeight;
  if(LineshapeList.size() >= 2){
    maxHeight_2 *=  exp(-0.5*(nSigma*nSigma*0.75));
  }
  std::vector<DalitzCoordinate> limits;
  for(std::vector<ILineshape*>::iterator it = LineshapeList.begin();
      it != LineshapeList.end(); it++){
    DalitzCoordinate coord ((*it)->getDalitzCoordinate(nSigma).mapMe(perm) );
    if(coord.size() <= 3){ // otherwise it's the D itself
      DalitzBox box(pat, coord);
      box.setName(this->theDecay().oneLiner() 
		  + " limit: " + coord.name()
		  + " " + anythingToString((int)nSigma) + " sigma");
      box.encloseInPhaseSpaceArea();
      box.setGuessedHeight(maxHeight_2);
      v.push_back(box);

      limits.push_back( coord );
    }
  }
  DalitzBox lastBox(pat, limits);
  lastBox.setGuessedHeight(norm(this->getValAtResonance()));
  cout << " just set box height, it's " << lastBox.guessedHeight() << endl;
  lastBox.setName(this->theDecay().oneLiner() 
	      + " all limits"
	      + " " + anythingToString((int)nSigma) + " sigma");
  cout << " .. now it is " << lastBox.guessedHeight() << endl;
  lastBox.encloseInPhaseSpaceArea();
  cout << "Amp " << name() << " made box with guessed height " 
       << norm(this->getValAtResonance())
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
  for(std::vector<ILineshape*>::iterator it = LineshapeList.begin();
      it != LineshapeList.end(); it++){
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
  box.setName(this->theDecay().oneLiner() 
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

std::complex<double> Amplitude::getOnePermutationsVal(){
  bool dbThis=false;

  double          sf = SpinFactorValue();
  complex<double> ls = LineshapeProduct();
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

    cout << theDecay().oneLiner() << ":"
	 << "\n   >  spinFactor       " << sf
	 << "\n   >  LineshapeProduct " << ls
	 << "\n   >  Returning:       " << returnVal
	 << endl;
    cout << "-----------------------------------------" << endl;
  }
  return returnVal;
}
std::complex<double> Amplitude::getOnePermutationsSmootherLargerVal(){
  bool dbThis=false;

  double          sf = SpinFactorValue();
  complex<double> ls = LineshapeProductSmootherLarger();
  complex<double> returnVal =  sf*ls;

  if(dbThis){
    /*
    if(LineshapeList.size() >=2){
      complex<double> evtGen = LineshapeList[1]->EvtGenValue();
      complex<double> diff = returnVal - evtGen;
      if(abs(diff) > 1.e-10){
	cout << " EvtGenCheck: " << returnVal << " - " << evtGen
	     << " = " << diff << endl;
      }
    }
    */
    cout << theDecay().oneLiner() << " smoother/larger:"
	 << "\n   >  spinFactor       " << sf
	 << "\n   >  LineshapeProduct " << ls
	 << "\n   >  Returning:       " << returnVal
	 << endl;
    cout << "-----------------------------------------" << endl;
  }
  return returnVal;
}

std::complex<double> Amplitude::getVal(IDalitzEvent* evt){
  //  bool dbthis=false;
  this->setEvent(evt);
  std::complex<double> result = this->getVal();
  this->resetEventRecord();
  return result;
}

std::complex<double> Amplitude::getVal(){
  //bool dbThis=false;

  /*
  if(dbThis) cout << "Amplitude::getVal() starting amp " 
		  << name() << endl;
  */
  complex<double> sum=0;
  if(0 == getEvent()) return 0;

  if(! getEvent()->eventPattern().compatibleWithFinalState(getTreePattern())) return 0;

  for(int i=0; i < getEvent()->numPermutations(); i++){
    getEvent()->setPermutationIndex(i);
    complex<double> thisVal= getOnePermutationsVal();
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
  getEvent()->setPermutationIndex(0);
  sum /= sqrt((double) getEvent()->numPermutations());

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

std::complex<double> Amplitude::getSmootherLargerVal(IDalitzEvent* evt){
  //  bool dbthis=false;
  this->setEvent(evt);
  std::complex<double> result = this->getSmootherLargerVal();
  this->resetEventRecord();
  return result;
}

std::complex<double> Amplitude::getSmootherLargerVal(){
  bool dbThis=false;

  if(dbThis) cout << "Amplitude::getSmootherLargerVal() starting amp " 
		  << name() << endl;

  complex<double> sum=0;
  if(0 == getEvent()) return 0;
  if(! getEvent()->eventPattern().compatibleWithFinalState(theDecay())) return 0;

  for(int i=0; i < getEvent()->numPermutations(); i++){
    getEvent()->setPermutationIndex(i);
    complex<double> thisVal= getOnePermutationsSmootherLargerVal();
    if(dbThis){
      cout << " permutation " << i
	   << " makes event look like this: ";
      getEvent()->print();
      cout << "\n gets thisVal " << thisVal << endl;
    }
    sum += thisVal;
  }
  getEvent()->setPermutationIndex(0);
  sum /= sqrt((double) getEvent()->numPermutations());
  if(dbThis && abs(sum) > sqrt((double) 1000)){
    cout << " Amplitude : " << (*this)
	 << " returning a large value: " << sum
	 << endl;
  }
  
  if(dbThis){
    cout << "Amplitude::getSmootherLargerVal() for " 
	 << name() << endl;
    cout << " returning " << sum << endl;
    cout << "==========================================" << endl;
  }
  return sum;
}

std::complex<double> Amplitude::getValAtResonance(){
  std::complex<double> ls = LineshapeProductAtResonance();
  //  cout << "using Amplitude::getValAtResonance()" << endl;
  return ls;
}

/*
double Amplitude::gaussProb(){
  bool dbthis=false;

  double ls = LineshapeGaussProduct();
  if(dbthis){
    cout << theDecay().oneLiner() << ":"
	 << "\n   >  LineshapeGaussProduct " << ls
	 << "\n   >  Returning:       " << ls
	 << endl;
  }
  return ls;
}
*/

std::string Amplitude::name() const{
  return (std::string) "(" 
    + _spd 
    + " wave) with opt " + _lopt + "; "
    + theDecay().oneLiner();
}

void Amplitude::print(std::ostream& out) const{
  out << "(" << _spd << " wave)\n" << theDecay();
}

std::ostream& operator<<(std::ostream& out, const Amplitude& amp){
  amp.print(out);
  return out;
}

//
