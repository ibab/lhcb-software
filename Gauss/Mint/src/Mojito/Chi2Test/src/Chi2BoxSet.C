#include "IDalitzEvent.h"
#include "Chi2BoxSet.h"
#include "IIntegrationCalculator.h"

#include "FitAmpSum.h"

using namespace std;
using namespace MINT;

Chi2BoxSet::Chi2BoxSet()
  : vector<Chi2Box>()
  , _integCalc(0)
{}
Chi2BoxSet::Chi2BoxSet(const DalitzEventPattern& pat
		       , const counted_ptr<IIntegrationCalculator>& integPtr)
  : vector<Chi2Box>()
  , _integCalc(integPtr)
{
  Chi2Box b(pat);
  add(b);
}
Chi2BoxSet::Chi2BoxSet(const DalitzArea& area
		       , const counted_ptr<IIntegrationCalculator>& integPtr)
  : vector<Chi2Box>()
  , _integCalc(integPtr)
{
  Chi2Box b(area);
  add(b);
}
Chi2BoxSet::Chi2BoxSet(const vector<Chi2Box>& other)
  : vector<Chi2Box>(other)
  , _integCalc(0)
{}
Chi2BoxSet::Chi2BoxSet(const Chi2BoxSet& other)
  : vector<Chi2Box>(other)
  , _integCalc(0)
{
  if(0 != other._integCalc){
    counted_ptr<IIntegrationCalculator> newCP(other._integCalc->clone_IIntegrationCalculator());
    _integCalc = newCP;
  }
}

void Chi2BoxSet::add(const Chi2Box& box){
  this->push_back(box);
}

void Chi2BoxSet::add(const Chi2BoxSet& boxSet){
  for(unsigned int i=0; i<boxSet.size(); i++){
    this->add(boxSet[i]);
  }
}

void Chi2BoxSet::resetEventCounts(){
  for(unsigned int i=0; i < this->size(); i++){
    (*this)[i].resetEventCounts();
  }
}
bool Chi2BoxSet::addData(const IDalitzEvent& evt){
  for(unsigned int i=0; i < this->size(); i++){
    if ((*this)[i].addData(evt)) return true;
  }
  return false;
}
bool Chi2BoxSet::addData(const IDalitzEvent* evt){
  for(unsigned int i=0; i < this->size(); i++){
    if ((*this)[i].addData(evt)) return true;
  }
  return false;
}
bool Chi2BoxSet::addMC(IDalitzEvent& evt, double weight){
  for(unsigned int i=0; i < this->size(); i++){
    if((*this)[i].addMC(evt, weight)){
      if(0 != _integCalc)_integCalc->addEvent(&evt);
      return true;
    }
  }
  return false;
}
bool Chi2BoxSet::addMC(IDalitzEvent* evt, double weight){
  bool dbThis=false;
  if(dbThis) {
    cout << "Chi2BoxSet::addMC for pointers called with evt = " 
	 << evt << endl;
  }
  for(unsigned int i=0; i < this->size(); i++){
    if((*this)[i].addMC(evt, weight)){
      if(0 != _integCalc)_integCalc->addEvent(evt);
      return true;
    }
  }
  return false;
}
void Chi2BoxSet::printBoxInfo(std::ostream& os) const{
  os << "Chi2BoxSet with " << this->size() << " sub-box";
  if(this->size() > 1) os << "es";
  os << ": ";
  for(unsigned int i=0; i < this->size(); i++){
    os << "\n " << i << ") " << (*this)[i];
  }
  if(0 != _integCalc){
    FitFractionList f = _integCalc->getFractions();
    f.sortByMagnitudeDecending();
    os << "\n sorted fractions in this box\n"
       << f << endl;
  }
  os << "-------------------------------------\n" << endl;

}
int Chi2BoxSet::nData() const{
  int sum=0;
  for(unsigned int i=0; i < this->size(); i++){
    sum += (*this)[i].nData();
  }
  return sum;
}
int Chi2BoxSet::nMC() const{
  int sum=0;
  for(unsigned int i=0; i < this->size(); i++){
    sum += (*this)[i].nMC();
  }
  return sum;
}
double Chi2BoxSet::weightedMC() const{
  double sum=0;
  for(unsigned int i=0; i < this->size(); i++){
    sum += (*this)[i].weightedMC();
  }
  return sum;
}
double Chi2BoxSet::weightedMC2() const{
  double sum=0;
  for(unsigned int i=0; i < this->size(); i++){
    sum += (*this)[i].weightedMC();
  }
  return sum;
}
double Chi2BoxSet::rmsMC(int Ntotal) const{
  bool dbThis=false;
  double dN = (double) Ntotal;
  double msq = weightedMC2() /(dN);
  double m   = weightedMC()  /(dN);
  if(dbThis){
    cout << "Chi2BoxSet::rmsMC() "
	 << " msq " << msq << " m " << m << " m*m " << m*m 
	 << " rms " << msq - m*m
	 << endl;
  }
  // this treats events that didn't get into the
  // box as events with weight 0.
  // The variance on the weights is
  // Var(w) = msq - m*m
  // The variance on the sum of weights
  // is therefore (msq - m*m) * dN
  return (msq - m*m) * dN;
}
//
