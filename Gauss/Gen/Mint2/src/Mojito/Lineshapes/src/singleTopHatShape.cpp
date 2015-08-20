#include <sstream>

#include "Mint/singleTopHatShape.h"
//#include "Mint/Utils.h"
#include "Mint/FlatFct.h"

using namespace std;
using namespace MINT;

singleTopHatShape::singleTopHatShape( const AssociatedDecayTree& decay
				      , double mini, double maxi)
  : _genFct(0)
  , _theDecay(decay)
  , _min_sij(mini)
  , _max_sij(maxi)
{
  if(_theDecay.nDgtr() > 2 && ! (startOfDecayChain())){
    std::cout << "WARNING: singleTopHatShape can only properly handle"
	      << "\n    > two body decays."
	      << "\n    > This: " << _theDecay.oneLiner()
	      << "\n    > is a " << _theDecay.nDgtr() << " body decay."
	      << "\n    > Please improve me."
	      << std::endl;
    std::cout << " startOfDecayChain()? " << startOfDecayChain()
	      << std::endl;
  }
}
singleTopHatShape::singleTopHatShape(const singleTopHatShape& other)
  : ILineshape()
  , _genFct(other._genFct)
  , _theDecay(other._theDecay)
  , _min_sij(other._min_sij)
  , _max_sij(other._max_sij)
{
}

DalitzCoordinate singleTopHatShape::getDalitzCoordinate(double ) const{
  DalitzCoordinate coord(_theDecay.getVal().asi());
  coord.setMinMax(_min_sij, _max_sij);
  return coord;
}

double singleTopHatShape::mumsRecoMass2(IDalitzEvent& evt) const{
  std::vector<int> asi = _theDecay.getVal().asi();
  if(asi.size() < 2){
    cout << "ERROR in singleTopHatShape::mumsRecoMass2() "
	 << _theDecay.getVal()._pdg_id
	 << ", " << _theDecay.getVal().name()
	 << "  decays into " << asi.size() << " particles?\n" 
	 << _theDecay
	 << endl;
  }
  double mumsRM = evt.sij(asi);
  return mumsRM;
}
std::complex<double> singleTopHatShape::getValue(IDalitzEvent& evt) const{
  double m = mumsRecoMass2(evt);
  if(m >= min() && m < max()) return 1;
  else return 0;
}
std::complex<double> singleTopHatShape::getVal(IDalitzEvent& evt){
  return getValue(evt);
}


void singleTopHatShape::makeGeneratingFunction() const{
  if(! _genFct){
    counted_ptr<FlatFct> fptr(new FlatFct(getDalitzCoordinate()));
    fptr->setBoxLimits(min(), max());
    fptr->setLimits(min(), max());

    _genFct = (counted_ptr<IGenFct>) fptr;
  }
}

counted_ptr<IGenFct> singleTopHatShape::generatingFunction() const{
  if(! _genFct){
    makeGeneratingFunction();
  }
  return _genFct;
}

std::string singleTopHatShape::name() const{
  std::stringstream strm;
  strm << "singleTopHatShape[" 
       << min() << "," << max() << "]("
       << _theDecay.oneLiner() << ")";
  std::string ing;
  strm >> ing;
  return ing;
}

void singleTopHatShape::print(IDalitzEvent& evt, std::ostream& out) const{
  out << name()
      << "\n\t> co-ordinate: " << getDalitzCoordinate()
      << "\n\t> min, max sij: " << min() << ", " << max()
      << "\n\t> sij: " << mumsRecoMass2(evt)
      << "\n\t> This is the decay I'm looking at:"
      << "\n" << _theDecay
      << " startOfDecayChain? " << startOfDecayChain()
      << ", getValue " << getValue(evt);
}
void singleTopHatShape::print(std::ostream& out) const{
  out << name()
      << "\n\t> co-ordinate: " << getDalitzCoordinate()
      << "\n\t> min, max sij: " << min() << ", " << max()
      << "\n\t> This is the decay I'm looking at:"
      << "\n" << _theDecay
      << " startOfDecayChain? " << startOfDecayChain();
}


// ------
std::ostream& operator<<(std::ostream& out, const singleTopHatShape& ths){
  ths.print(out);
  return out;
}

// ------


