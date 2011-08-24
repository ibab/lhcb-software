#include "Mint/IDalitzEvent.h"
#include "Mint/Chi2BoxSet.h"
#include "Mint/IIntegrationCalculator.h"

#include "Mint/FitAmpSum.h"

using namespace std;
using namespace MINT;
  
Chi2BoxSet::Chi2BoxSet()
  : vector<Chi2Box>()
  , _integCalc(0)
  , _normFactor(1)
{
}
Chi2BoxSet::Chi2BoxSet(const DalitzEventPattern& pat
		       , const counted_ptr<IIntegrationCalculator>& integPtr)
  : vector<Chi2Box>()
  , _integCalc(integPtr)
  , _normFactor(1)
{
  Chi2Box b(pat);
  add(b);
}
Chi2BoxSet::Chi2BoxSet(const DalitzArea& area
		       , const counted_ptr<IIntegrationCalculator>& integPtr)
  : vector<Chi2Box>()
  , _integCalc(integPtr)
  , _normFactor(1)
{
  Chi2Box b(area);
  add(b);
}
Chi2BoxSet::Chi2BoxSet(const vector<Chi2Box>& other)
  : vector<Chi2Box>(other)
  , _integCalc(0)
  , _normFactor(1)
{
}
Chi2BoxSet::Chi2BoxSet(const Chi2BoxSet& other)
  : vector<Chi2Box>(other)
  , _integCalc(0)
  , _histoData(other._histoData)
  , _histoMC(other._histoMC)
  , _normFactor(other._normFactor)
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
    if ((*this)[i].addData(evt)){
      _histoData.addEvent(&evt);
      return true;
    }
  }
  return false;
}
bool Chi2BoxSet::addData(const IDalitzEvent* evt){
  for(unsigned int i=0; i < this->size(); i++){
    if ((*this)[i].addData(evt)){
      _histoData.addEvent(evt);
      return true;
    }
  }
  return false;
}
bool Chi2BoxSet::addMC(IDalitzEvent& evt, double weight){
  for(unsigned int i=0; i < this->size(); i++){
    if((*this)[i].addMC(evt, weight)){
      _histoMC.addEvent(&evt, weight);
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
      _histoMC.addEvent(evt, weight);
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
  os << "\n--------------------------------------------------------\n" << endl;

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
    sum += (*this)[i].weightedMC2();
  }
  return sum;
}

// keep changing my mind which one is right...
double Chi2BoxSet::rmsMC(int ) const{
  return weightedMC2();
}

/*
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
*/

void Chi2BoxSet::setHistoColour(Color_t fcolor){
  setFillColour(fcolor);
  setLineColour(fcolor);
}
void Chi2BoxSet::setFillColour(Color_t fcolor){
  histoData().setFillColour(fcolor);
  histoMC().setFillColour(fcolor);
}
void Chi2BoxSet::setLineColour(Color_t fcolor){
  histoData().setLineColour(fcolor);
  histoMC().setLineColour(fcolor);
}

DalitzHistoSet& Chi2BoxSet::histoData(){
  return _histoData;
}
const DalitzHistoSet& Chi2BoxSet::histoData()const {
  return _histoData;
}
DalitzHistoSet& Chi2BoxSet::histoMC(){
  return _histoMC;
}
const DalitzHistoSet& Chi2BoxSet::histoMC()const {
  return _histoMC;
}

double Chi2BoxSet::chi2(double normFactorPassed) const{
  double nf=normFactorPassed;
  if(nf < 0){
    nf = normFactor();
  }

  int n_data =  this->nData();
  double weight_mc = this->weightedMC() * nf;

  double var_mc = this->weightedMC2() * nf*nf;
  double varData_expected = weight_mc;
  
  double varData;
  if(0 != varData_expected) varData = varData_expected;
  else varData = n_data;
  
  double var = varData + var_mc;

  double delta_N = n_data - weight_mc;
  double dNSq = delta_N * delta_N;
  double chi2;
  if( dNSq < var * 1.e-20) chi2=0; // catches legit. 0-entry cases
  else chi2 = dNSq / var;
  
  return chi2;
}

std::ostream& operator<<(std::ostream& os, const Chi2BoxSet& c2bs){
  c2bs.print(os);
  return os;
}
//
