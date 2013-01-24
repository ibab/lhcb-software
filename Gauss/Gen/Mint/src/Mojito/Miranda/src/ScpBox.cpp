
#include "Mint/IDalitzEvent.h"
#include "Mint/ScpBox.h"

using namespace std;

ScpBox::ScpBox()
: _area()
, _nData(0)
, _nMC(0)
, _nWeightedMC(0.0)
, _nWeightedData(0.0)
, _weightMC_Squared(0.0)
, _nWeightedBkg(0.0)
, _nWeightedBkgCC(0.0)
{}
ScpBox::ScpBox(const DalitzEventPattern& pat)
: _area(pat)
, _nData(0)
, _nMC(0)
, _nWeightedMC(0.0)
, _nWeightedData(0.0)
, _weightMC_Squared(0.0)
, _nWeightedBkg(0.0)
, _nWeightedBkgCC(0.0)
{
  enclosePhaseSpace();
}
ScpBox::ScpBox(const DalitzArea& area)
: _area(area)
, _nData(0)
, _nMC(0)
, _nWeightedMC(0.0)
, _nWeightedData(0.0)
, _weightMC_Squared(0.0)
, _nWeightedBkg(0.0)
, _nWeightedBkgCC(0.0)
{}
ScpBox::ScpBox(const ScpBox& other)
: _area(other._area)
, _nData(other._nData)
, _nMC(other._nMC)
, _nWeightedMC(other._nWeightedMC)
, _nWeightedData(other._nWeightedData)
, _weightMC_Squared(other._weightMC_Squared)
, _nWeightedBkg(other._nWeightedBkg)
, _nWeightedBkgCC(other._nWeightedBkgCC)
{}

void ScpBox::enclosePhaseSpace(double safetyFactor){
  bool dbThis=false;
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
  _nWeightedData=0.0;
  _weightMC_Squared = 0.0;
}
void ScpBox::resetAll(){
  enclosePhaseSpace();
  resetEventCounts();
}

bool ScpBox::addData(const IDalitzEvent& evt, double weight){
  if(! _area.isInside(evt)) return false;
  _nData++;
  _nWeightedData += weight;

  return true;
}

bool ScpBox::thisBox(const IDalitzEvent* evt)
{
  if(! _area.isInside(*evt)) return false;
  return true;
}
bool ScpBox::addData(const IDalitzEvent* evt, double weight){
  bool dbThis=false;
  if(0 == evt) return false;
  if(dbThis) cout << "ScpBox::addData for pointers called" << endl;
  if(! _area.isInside(*evt)) return false;
  if(dbThis){
      cout << "found data event inside area. This is the event:" << endl;
      evt->print();
  }
  _nData++;
  _nWeightedData += weight;
  return true;
}

bool ScpBox::addData(int entries){
  bool dbThis = true;
  if (dbThis) std::cout << "Adding: " << entries << std::endl;
  _nData = _nData + entries;
  return true;
}

bool ScpBox::addMC(int entries){
  _nMC = _nMC + entries;
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
double ScpBox::weightedData() const{
  return _nWeightedData;
}
double ScpBox::weightedBkg() const{
  return _nWeightedBkg;
}
double ScpBox::weightedBkgCC() const{
  return _nWeightedBkgCC;
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


bool ScpBox::subtractData(const IDalitzEvent& evt, double weight){
  if(! _area.isInside(evt)) return false;
  _nBkg++;
  _nWeightedBkg= _nWeightedBkg + weight;
  return true;
}

bool ScpBox::subtractData(const IDalitzEvent* evt, double weight){
  bool dbThis=false;
  if(0 == evt) return false;
  if(dbThis) cout << "ScpBox::subtractData for pointers called" << endl;
  if(! _area.isInside(*evt)) return false;
  if(dbThis){
      cout << "found data event inside area. This is the event:" << endl;
      evt->print();
  }
  _nWeightedBkg= _nWeightedBkg + weight;
  _nBkg++;

  return true;
}

bool ScpBox::subtractMC(const IDalitzEvent& evt, double weight){
  if(! _area.isInside(evt)) return false;
  _nBkgCC++;
  _nWeightedBkgCC= _nWeightedBkgCC + weight;

  return true;
}

bool ScpBox::subtractMC(const IDalitzEvent* evt, double weight){
  bool dbThis=false;
  if(dbThis) cout << "ScpBox::subtractMC for pointers called" << endl;
  if(0 == evt) return false;
  if(dbThis){
      cout << "...area inside for this event:" << endl;
      evt->print();
  }
  if(! _area.isInside(*evt)) return false;
  _nBkgCC++;
  _nWeightedBkgCC= _nWeightedBkgCC + weight;

//  _weightMC_Squared -= weight*weight;

  if(dbThis) cout << "ScpBox::subtractMC returning; have nMC = "<< _nMC << endl;
  return true;
}

double ScpBox::scp(double normFactorPassed) const{

  double n_data =  this->weightedData();
  double n_dataCC = this->weightedMC();

  double n_Bkg =  this->weightedBkg();
  double n_BkgCC = this->weightedBkgCC();

  double ErrDataSq = n_data+n_Bkg;
  double ErrDataCCSq = n_dataCC+n_BkgCC;

  double scp;
  double alpha = (normFactorPassed);
  if (n_data == 0 || n_dataCC == 0 )
    {
      scp = 0;
    }
  else{
      scp = ((n_data-n_Bkg)-(alpha*(n_dataCC-n_BkgCC)))/sqrt(ErrDataSq+(alpha*alpha*ErrDataCCSq));
  }
  return scp;
}

DalitzArea ScpBox::area()
{
  return _area;
}


void ScpBox::print(std::ostream& os) const{

  //  std::map<DalitzCoordKey, DalitzCoordinate*>::iterator it;
  //  std::map<DalitzCoordKey, DalitzCoordinate*> Coord = _area._coords;
  //  std::vector<int>::iterator vecit;
  //
  os << "box: with area " << _area;
  //  os << "box: with cordinates " << _area._t01;
  //  _area._t01.print();
  //  _area._t01.nameFileSave();
  //  for (it = Coord.begin(); it != Coord.end(); it++)
  //  {
  //	  std::vector<int> vec = (*it).first;
  //	  for (vecit = vec.begin(); vecit != vec.end(); vecit++)
  //	  {
  //		  os << "Corodinates" << (*vecit) << " " << *((*it).second);
  //	  }
  //  }
}

std::ostream& operator<<(std::ostream& os, const ScpBox& box){
  box.print(os);
  return os;
}
//

