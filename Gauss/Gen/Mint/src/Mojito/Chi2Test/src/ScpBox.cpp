
#include "Mint/IDalitzEvent.h"
#include "Mint/ScpBox.h"

using namespace std;

ScpBox::ScpBox()
  : _area()
  , _nData(0)
  , _nMC(0)
  , _nWeightedMC(0.0)
  , _weightMC_Squared(0.0)
{}
ScpBox::ScpBox(const DalitzEventPattern& pat)
  : _area(pat)
  , _nData(0)
  , _nMC(0)
  , _nWeightedMC(0.0)
  , _weightMC_Squared(0.0)
{
  enclosePhaseSpace();
}
ScpBox::ScpBox(const DalitzArea& area)
  : _area(area)
  , _nData(0)
  , _nMC(0)
  , _nWeightedMC(0.0)
  , _weightMC_Squared(0.0)
{}
ScpBox::ScpBox(const ScpBox& other)
  : _area(other._area)
  , _nData(other._nData)
  , _nMC(other._nMC)
  , _nWeightedMC(other._nWeightedMC)
  , _weightMC_Squared(other._weightMC_Squared)
{}

void ScpBox::enclosePhaseSpace(double safetyFactor){
  bool dbThis=true;
  if(dbThis) cout << "box: setting limits to phase space area" << endl;
  _area.setAllLimitsToPhaseSpaceArea(safetyFactor);
  if(dbThis) cout << "result: " << _area << endl;
}

std::vector<ScpBox> ScpBox::split(int n){
  
  std::vector<DalitzArea> va= _area.split_in_all_dimensions(n);
  std::vector<ScpBox> vb(va.size());

  for(unsigned int i=0; i < va.size(); i++){
    ScpBox b(va[i]);
    vb[i]=b;
  }
  return vb;
}

void ScpBox::resetEventCounts(){
  _nData = 0;
  _nMC = 0;
  _nWeightedMC=0.0;
  _weightMC_Squared = 0.0;
}
void ScpBox::resetAll(){
  enclosePhaseSpace();
  resetEventCounts();
}

bool ScpBox::addData(const IDalitzEvent& evt){
  if(! _area.isInside(evt)) return false;
  _nData++;
  return true;
}
bool ScpBox::addData(const IDalitzEvent* evt){
  bool dbThis=false;
  if(0 == evt) return false;
  if(dbThis) cout << "ScpBox::addData for pointers called" << endl;
  if(! _area.isInside(*evt)) return false;
  if(dbThis){
    cout << "found data event inside area. This is the event:" << endl;
    evt->print();
  }

  _nData++;
  return true;
}
bool ScpBox::addMC(const IDalitzEvent& evt, double weight){
  if(! _area.isInside(evt)) return false;
  _nMC++;
  _nWeightedMC += weight;
  return true;
}
bool ScpBox::addMC(const IDalitzEvent* evt, double weight){
  bool dbThis=false;
  if(dbThis) cout << "ScpBox::addMC for pointers called" << endl;
  if(0 == evt) return false;
  if(dbThis){
    cout << "...area inside for this event:" << endl;
    evt->print();
  }
  if(! _area.isInside(*evt)) return false;
  _nMC++;
  _nWeightedMC += weight;
  _weightMC_Squared += weight*weight;

  if(dbThis) cout << "ScpBox::addMC returning; have nMC = "<< _nMC << endl;
  return true;
}

int ScpBox::nData() const{
  return _nData;
}
int ScpBox::nMC() const{
  return _nMC;
}
double ScpBox::weightedMC() const{
  return _nWeightedMC;
}
double ScpBox::weightedMC2() const{
  return _weightMC_Squared;
}
double ScpBox::rmsMC(int Ntotal) const{
  return weightedMC2();
  /*
  bool dbThis=false;
  double dN = (double) Ntotal;
  double msq = weightedMC2() /(dN);
  double m   = weightedMC()  /(dN);
  if(dbThis){
    cout << "ScpBoxSet::rmsMC() "
	 << " msq " << msq << " m " << m << " m*m " << m*m 
	 << " rms " << msq - m*m
	 << endl;
  }
  return (msq - m*m) * dN;
  */
}
void ScpBox::print(std::ostream& os) const{
  os << "box: with area " << _area;
}

std::ostream& operator<<(std::ostream& os, const ScpBox& box){
  box.print(os);
  return os;
}
//

