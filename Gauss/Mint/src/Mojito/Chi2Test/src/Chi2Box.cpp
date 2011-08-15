
#include "Mint/IDalitzEvent.h"
#include "Mint/Chi2Box.h"

using namespace std;

Chi2Box::Chi2Box() 
  : _area()
  , _nData(0)
  , _nMC(0)
  , _nWeightedMC(0.0)
  , _weightMC_Squared(0.0)
{}
Chi2Box::Chi2Box(const DalitzEventPattern& pat) 
  : _area(pat)
  , _nData(0)
  , _nMC(0)
  , _nWeightedMC(0.0)
  , _weightMC_Squared(0.0)
{
  enclosePhaseSpace();
}
Chi2Box::Chi2Box(const DalitzArea& area) 
  : _area(area)
  , _nData(0)
  , _nMC(0)
  , _nWeightedMC(0.0)
  , _weightMC_Squared(0.0)
{}
Chi2Box::Chi2Box(const Chi2Box& other) 
  : _area(other._area)
  , _nData(other._nData)
  , _nMC(other._nMC)
  , _nWeightedMC(other._nWeightedMC)
  , _weightMC_Squared(other._weightMC_Squared)
{}

void Chi2Box::enclosePhaseSpace(double safetyFactor){
  bool dbThis=true;
  if(dbThis) cout << "box: setting limits to phase space area" << endl;
  _area.setAllLimitsToPhaseSpaceArea(safetyFactor);
  if(dbThis) cout << "result: " << _area << endl;
}

std::vector<Chi2Box> Chi2Box::split(int n){
  
  std::vector<DalitzArea> va= _area.split_in_all_dimensions(n);
  std::vector<Chi2Box> vb(va.size());

  for(unsigned int i=0; i < va.size(); i++){
    Chi2Box b(va[i]);
    vb[i]=b;
  }
  return vb;
}

void Chi2Box::resetEventCounts(){
  _nData = 0;
  _nMC = 0;
  _nWeightedMC=0.0;
  _weightMC_Squared = 0.0;
}
void Chi2Box::resetAll(){
  enclosePhaseSpace();
  resetEventCounts();
}

bool Chi2Box::addData(const IDalitzEvent& evt){
  if(! _area.isInside(evt)) return false;
  _nData++;
  return true;
}
bool Chi2Box::addData(const IDalitzEvent* evt){
  bool dbThis=false;
  if(0 == evt) return false;
  if(dbThis) cout << "Chi2Box::addData for pointers called" << endl;
  if(! _area.isInside(*evt)) return false;
  if(dbThis){
    cout << "found data event inside area. This is the event:" << endl;
    evt->print();
  }

  _nData++;
  return true;
}
bool Chi2Box::addMC(const IDalitzEvent& evt, double weight){
  if(! _area.isInside(evt)) return false;
  _nMC++;
  _nWeightedMC += weight;
  return true;
}
bool Chi2Box::addMC(const IDalitzEvent* evt, double weight){
  bool dbThis=false;
  if(dbThis) cout << "Chi2Box::addMC for pointers called" << endl;
  if(0 == evt) return false;
  if(dbThis){
    cout << "...area inside for this event:" << endl;
    evt->print();
  }
  if(! _area.isInside(*evt)) return false;
  _nMC++;
  _nWeightedMC += weight;
  _weightMC_Squared += weight*weight;

  if(dbThis) cout << "Chi2Box::addMC returning; have nMC = "<< _nMC << endl;
  return true;
}

int Chi2Box::nData() const{
  return _nData;
}
int Chi2Box::nMC() const{
  return _nMC;
}
double Chi2Box::weightedMC() const{
  return _nWeightedMC;
}
double Chi2Box::weightedMC2() const{
  return _weightMC_Squared;
}
double Chi2Box::rmsMC(int Ntotal) const{
  return weightedMC2();
  /*
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
  return (msq - m*m) * dN;
  */
}
void Chi2Box::print(std::ostream& os) const{
  os << "box: with area " << _area;
}

std::ostream& operator<<(std::ostream& os, const Chi2Box& box){
  box.print(os);
  return os;
}
//

