#include "Mint/IDalitzEvent.h"
#include "Mint/ScpBoxSet.h"
#include "Mint/IIntegrationCalculator.h"
#include "Mint/FitAmpSum.h"

using namespace std;
using namespace MINT;

ScpBoxSet::ScpBoxSet()
  : vector<ScpBox>()
  , _integCalc(0)
  , _normFactor(1)
{
}
ScpBoxSet::ScpBoxSet(const DalitzEventPattern& pat
		       , const counted_ptr<IIntegrationCalculator>& integPtr)
  : vector<ScpBox>()
  , _integCalc(integPtr)
  , _normFactor(1)
{
  ScpBox b(pat);
  add(b);
}
ScpBoxSet::ScpBoxSet(const DalitzArea& area
		       , const counted_ptr<IIntegrationCalculator>& integPtr)
  : vector<ScpBox>()
  , _integCalc(integPtr)
  , _normFactor(1)
{
  ScpBox b(area);
  add(b);
}
ScpBoxSet::ScpBoxSet(const vector<ScpBox>& other)
  : vector<ScpBox>(other)
  , _integCalc(0)
  , _normFactor(1)
{
}
ScpBoxSet::ScpBoxSet(const ScpBoxSet& other)
  : vector<ScpBox>(other)
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

void ScpBoxSet::add(const ScpBox& box){
  this->push_back(box);
}

void ScpBoxSet::add(const ScpBoxSet& boxSet){
  for(unsigned int i=0; i<boxSet.size(); i++){
    this->add(boxSet[i]);
  }
}

void ScpBoxSet::resetEventCounts(){
  for(unsigned int i=0; i < this->size(); i++){
    (*this)[i].resetEventCounts();
  }
}
bool ScpBoxSet::addData(const IDalitzEvent& evt, double weight){
  for(unsigned int i=0; i < this->size(); i++){
    if ((*this)[i].addData(evt, weight)){
      _histoData.addEvent(&evt, weight);
      return true;
    }
  }
  return false;
}
bool ScpBoxSet::addData(const IDalitzEvent* evt, double weight){
  for(unsigned int i=0; i < this->size(); i++){
    if ((*this)[i].addData(evt, weight)){
      _histoData.addEvent(evt, weight);
      return true;
    }
  }
  return false;
}

bool ScpBoxSet::addData(int entries){
	std::cout << "Adding Data " << entries << std::endl;
	std::cout << "Size " << this->size() << std::endl;

  for(unsigned int i=0; i < this->size(); i++){
	  std::cout << "HERE" << std::endl;
    if ((*this)[i].addData(entries)){
      return true;
    }
  }
  return false;
}

bool ScpBoxSet::addMC(IDalitzEvent& evt, double weight){
  for(unsigned int i=0; i < this->size(); i++){
    if((*this)[i].addMC(evt, weight)){
      _histoMC.addEvent(&evt, weight);
      if(0 != _integCalc)_integCalc->addEvent(&evt);
      return true;
    }
  }
  return false;
}
bool ScpBoxSet::addMC(IDalitzEvent* evt, double weight){
  bool dbThis=false;
  if(dbThis) {
    cout << "ScpBoxSet::addMC for pointers called with evt = "
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

bool ScpBoxSet::addMC(int entries){
  for(unsigned int i=0; i < this->size(); i++){
    if((*this)[i].addMC(entries)){
      return true;
    }
  }
  return false;
}

void ScpBoxSet::printBoxInfo(std::ostream& os) const{
  os << "ScpBoxSet with " << this->size() << " sub-box";
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
int ScpBoxSet::nData() const{
  int sum=0;
  for(unsigned int i=0; i < this->size(); i++){
    sum += (*this)[i].nData();
  }
  return sum;
}
double ScpBoxSet::weightedData() const{
  double sum=0;
  for(unsigned int i=0; i < this->size(); i++){
    sum += (*this)[i].weightedData();
  }
  return sum;
}
int ScpBoxSet::nMC() const{
  int sum=0;
  for(unsigned int i=0; i < this->size(); i++){
    sum += (*this)[i].nMC();
  }
  return sum;
}
double ScpBoxSet::weightedMC() const{
  double sum=0;
  for(unsigned int i=0; i < this->size(); i++){
    sum += (*this)[i].weightedMC();
  }
  return sum;
}
double ScpBoxSet::weightedMC2() const{
  double sum=0;
  for(unsigned int i=0; i < this->size(); i++){
    sum += (*this)[i].weightedMC2();
  }
  return sum;
}

// keep changing my mind which one is right...
double ScpBoxSet::rmsMC(int ) const{
  return weightedMC2();
}

/*
double ScpBoxSet::rmsMC(int Ntotal) const{
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
  // this treats events that didn't get into the
  // box as events with weight 0.
  // The variance on the weights is
  // Var(w) = msq - m*m
  // The variance on the sum of weights
  // is therefore (msq - m*m) * dN
  return (msq - m*m) * dN;
}
*/

void ScpBoxSet::setHistoColour(Color_t fcolor){
  setFillColour(fcolor);
  setLineColour(fcolor);
}
void ScpBoxSet::setFillColour(Color_t fcolor){
  histoData().setFillColour(fcolor);
  histoMC().setFillColour(fcolor);
}
void ScpBoxSet::setLineColour(Color_t fcolor){
  histoData().setLineColour(fcolor);
  histoMC().setLineColour(fcolor);
}

DalitzHistoSet& ScpBoxSet::histoData(){
  return _histoData;
}
const DalitzHistoSet& ScpBoxSet::histoData()const {
  return _histoData;
}
DalitzHistoSet& ScpBoxSet::histoMC(){
  return _histoMC;
}
const DalitzHistoSet& ScpBoxSet::histoMC()const {
  return _histoMC;
}


std::ostream& operator<<(std::ostream& os, const ScpBoxSet& c2bs){
  c2bs.print(os);
  return os;
}

double ScpBoxSet::scp(double normFactorPassed) const{


  int n_data =  this->weightedData();
  double n_dataCC = this->weightedMC(); //replaced with weighted data

  bool db = false;

  if (db == true)
  {
	  if (n_data < 10 || n_dataCC< 10)
	  {
		  std::cout << "WARNING: BOX with " << n_data << " data entries " << std::endl;
		  std::cout << "WARNING: BOX with " << n_dataCC << " dataCC entries " << std::endl;
	  }
  }

  double alpha = (normFactorPassed);

//  alpha = normFactorPassed;
  double scp = (n_data-(alpha*n_dataCC))/sqrt(n_data+(alpha*alpha*n_dataCC));


  if (n_data == 0 || n_dataCC == 0)
  {
	  scp = -9999;
  }
  return scp;
}

bool ScpBoxSet::subtractData(const IDalitzEvent& evt, double weight){
  for(unsigned int i=0; i < this->size(); i++){
    if ((*this)[i].subtractData(evt, weight)){
      return true;
    }
  }
  return false;
}
bool ScpBoxSet::subtractData(const IDalitzEvent* evt, double weight){
  for(unsigned int i=0; i < this->size(); i++){
    if ((*this)[i].subtractData(evt, weight)){
      return true;
    }
  }
  return false;
}
bool ScpBoxSet::subtractMC(IDalitzEvent& evt, double weight){
  for(unsigned int i=0; i < this->size(); i++){
    if((*this)[i].subtractMC(evt, weight)){
      return true;
    }
  }
  return false;
}
bool ScpBoxSet::subtractMC(IDalitzEvent* evt, double weight){
  bool dbThis=false;
  if(dbThis) {
    cout << "ScpBoxSet::subtractMC for pointers called with evt = "
	 << evt << endl;
  }
  for(unsigned int i=0; i < this->size(); i++){
    if((*this)[i].subtractMC(evt, weight)){
      return true;
    }
  }
  return false;
}




bool ScpBoxSet::boxSet(const IDalitzEvent* Devt)
{
	 for(unsigned int i=0; i < this->size(); i++){
		 if((*this)[i].thisBox(Devt))
		 {
		    	return true;
		 }
	 }
	return false;
}

void ScpBoxSet::save(){
	//get points

//	 double t01min, s12min, s23min, s34min, t40min;
//	 double t01max, s12max, s23max, s34max, t40max;
//
//	 TFile* out_Boxfile = new TFile("OutputBinning.root","RECREATE");
//	 TTree *out_BoxTree = new TTree("SCPBox","SCPBox");
//
//	 out_BoxTree->Branch( "t01min",&t01min, "t01min");
//	 out_BoxTree->Branch( "t01max",&t01max, "t01max");
//	 out_BoxTree->Branch( "s12min",&s12min, "s12min");
//	 out_BoxTree->Branch( "s12max",&s12max, "s12max");
//	 out_BoxTree->Branch( "s23min",&s23min, "s23min");
//	 out_BoxTree->Branch( "s23max",&s23max, "s23max");
//	 out_BoxTree->Branch( "s34min",&s34min, "s34min");
//	 out_BoxTree->Branch( "s34max",&s34max, "s34max");
//	 out_BoxTree->Branch( "t40min",&t40min, "t40min");
//	 out_BoxTree->Branch( "t40max",&t40max, "t40max");
//
//	 for (int i = 0; i< (*this).size(); i++)
//	 {
////		 (*this)[i].print();
//		  t01min = (*this)[i].area()._t01.min();
//		  t01max = (*this)[i].area()._t01.max();
//
//		  s12min = (*this)[i].area()._s12.min();
//		  s12max = (*this)[i].area()._s12.max();
//
//		  s23min = (*this)[i].area()._s23.min();
//		  s23max = (*this)[i].area()._s23.max();
//
//		  s34min = (*this)[i].area()._s34.min();
//		  s34max = (*this)[i].area()._s34.max();
//
//		  t40min = (*this)[i].area()._t40.min();
//		  t40max = (*this)[i].area()._t40.max();
//		  out_BoxTree->Fill();
//	 }
//	 out_Boxfile->Write();
//	 out_Boxfile->Close();

}

//
//double ScpBoxSet::scpErr(double normFactorPassed) const{
//
//  int n_data =  this->nData();
//  double n_dataCC = this->nMC();
//
//
//
//  double alpha = (n_data/n_dataCC);
//
//  alpha = normFactorPassed;
//
//  double scp = (n_data-(alpha*n_dataCC))/sqrt(n_data+(alpha*alpha*n_dataCC));
//
//  return scp;
//}


