#include "Mint/IDalitzEventList.h"

#include "Mint/ScpBinning.h"
#include "Mint/ScpBox.h"
#include "Mint/ScpBoxSet.h"

#include "Mint/DalitzHistoStackSet.h"

#include "TCanvas.h"
#include "TH1D.h"

#include <algorithm>
#include <iostream>

using namespace MINT;
using namespace std;

int* ScpBinning::__colourPalette=0;
int  ScpBinning::__Ncol=256;

void ScpBinning::makeColourPaletteBlueGrey(){
  static const int NRGBs = 7;
  if(0 != __colourPalette) delete[] __colourPalette;
  __colourPalette = new int[__Ncol];

  Double_t stops[NRGBs] = { 0.00, 0.10, 0.25, 0.45, 0.60, 0.75, 1.00 };
  Double_t red[NRGBs]   = { 0.85, 0.00, 0.00, 0.00, 0.80, 0.80, 0.99 };
  Double_t green[NRGBs] = { 0.85, 0.80, 0.40, 0.40, 0.80, 0.00, 0.00 };
  Double_t blue[NRGBs]  = { 0.85, 0.85, 0.80, 0.00, 0.00, 0.00, 0.00 };
  Int_t Fi = TColor::CreateGradientColorTable(NRGBs
					    , stops
					    , red, green, blue
					    , __Ncol);

  for(int i=0; i < __Ncol; i++) __colourPalette[i]=Fi + i;
}
void ScpBinning::makeColourPaletteBlueWhite(){
  static const int NRGBs = 7;
  if(0 != __colourPalette) delete[] __colourPalette;
  __colourPalette = new int[__Ncol];

  Double_t stops[NRGBs] = { 0.00, 0.10, 0.25, 0.45, 0.60, 0.75, 1.00 };
  Double_t red[NRGBs]   = { 1.00, 0.00, 0.00, 0.00, 0.97, 0.97, 0.10 };
  Double_t green[NRGBs] = { 1.00, 0.97, 0.30, 0.40, 0.97, 0.00, 0.00 };
  Double_t blue[NRGBs]  = { 1.00, 0.97, 0.97, 0.00, 0.00, 0.00, 0.00 };
  Int_t Fi = TColor::CreateGradientColorTable(NRGBs
					    , stops
					    , red, green, blue
					    , __Ncol);

//  for(int i=0; i < __Ncol; i++) __colourPalette[i]=Fi + i;
  for(int i=-1*__Ncol/2; i < __Ncol/2; i++) __colourPalette[i]=Fi + i + __Ncol/2;

  std::cout << "Fi2: " << Fi << std::endl;

}
void ScpBinning::makeColourPaletteRGB(){
  static const int NRGBs = 5;
  
  if(0 != __colourPalette) delete[] __colourPalette;
  __colourPalette = new int[__Ncol];
  
  Double_t red[NRGBs]   = {0., 0.0, 1.0, 1.0, 1.0};
  Double_t green[NRGBs] = {0., 0.0, 0.0, 1.0, 1.0};
  Double_t blue[NRGBs]  = {0., 1.0, 0.0, 0.0, 1.0};
  Double_t stops[NRGBs] = {0., .25, .50, .75, 1.0};
  Int_t Fi = TColor::CreateGradientColorTable(NRGBs
					      , stops
					      , red, green, blue
					      , __Ncol);
  
//  for(int i=0; i < __Ncol; i++) __colourPalette[i]=Fi + i;
  for(int i=-1*__Ncol/2; i < __Ncol/2; i++) __colourPalette[i]=Fi + i + __Ncol/2;

  std::cout << "Fi3: " << Fi << std::endl;

}
void ScpBinning::makeColourPalette(){
  makeColourPaletteBlueGrey();
  //makeColourPaletteRGB();
  //makeColourPaletteBlueWhite();
}

int* ScpBinning::getColourPalette(){
  if(0 == __colourPalette) makeColourPalette();
  return __colourPalette;
}

ScpBinning::ScpBinning()
  : _nData(0)
  , _nDataCC(0)
  , _totalMCWeight(0)
  , m_norm(10)
{
}
int ScpBinning::createBinning(IDalitzEventList* events
			       , int minPerBin
			       , int maxPerBin
			       ){

  if(0 == events) return 0;
  if(0 == events->size()) return 0;
  const IDalitzEvent* evt0 = events->getREvent(0);
  if(0 == evt0) return 0;

  ScpBoxSet boxes = splitBoxes(events, maxPerBin);

  mergeBoxes(boxes, minPerBin);

  return numBins();
}

int ScpBinning::mergeBoxes(ScpBoxSet& boxes, int minPerBin){
  lessByScpBoxData sorter;
  sort(boxes.begin(), boxes.end(), sorter);
  // this way, the smalles boxes come first 
  // makes sure no empty box is left over at the end.

  ScpBoxSet boxSet;
  for(unsigned int i=0; i < boxes.size(); i++){
    boxSet.add(boxes[i]);
    if(boxSet.nData() >= minPerBin){
      _boxSets.push_back(boxSet);
      boxSet.clear();
    }
  }
  
  return _boxSets.size();
}


ScpBoxSet ScpBinning::splitBoxes(IDalitzEventList* events
				   , int maxPerBin
				   ) const{
  bool dbThis=false;
  ScpBoxSet dummy;
  if(0 == events) return dummy;
  if(0 == events->size()) return dummy;
  const IDalitzEvent* evt0 = events->getREvent(0);
  if(0 == evt0) return dummy;
  
  ScpBox box(evt0->eventPattern());

  std::cout << "Event Pattern " << evt0->eventPattern() << std::endl;
  ScpBoxSet boxes(box.split(0));
  bool needToSplitMore=false;
  int counter=0;
  do{
    counter++;
    boxes.resetEventCounts();
    int failedSet=0, failedBox=0;
    events->Start();
    while(events->Next()){
      bool added = boxes.addData(events->getEvent());

      if(! added) failedSet++;
      if(! box.addData(events->getEvent())){
    	  failedBox++;
      }
    }
    if(failedSet > 0 || dbThis){
      cout << " ScpBinning::splitBoxes: ERROR split level " << counter
	   << ", num boxes: " << boxes.size()
	   << ", failedSet: " << failedSet
	   << ", failedBox: " << failedBox << endl;
    }

    ScpBoxSet newBoxes;
    needToSplitMore=false;
    for(unsigned int i=0; i < boxes.size(); i++){
      if( boxes[i].nData() > maxPerBin ){
    	  needToSplitMore=true;
    	  newBoxes.add(boxes[i].split());
      }
      else{
    	  newBoxes.add(boxes[i]);
      }
    }
    boxes = newBoxes;
  }while(needToSplitMore);

  return boxes;
}

void ScpBinning::resetEventCounts(){
  _nData=0;
  _nDataCC=0;
  _totalMCWeight=0.0;
  for(unsigned int i=0; i < _boxSets.size(); i++){
    _boxSets[i].resetEventCounts();
  }
}
void ScpBinning::fillData(IDalitzEventList* data){
  data->Start();
  while(data->Next()){
    bool foundBox=false;
    for(unsigned int i=0; i < _boxSets.size(); i++){
      if(_boxSets[i].addData(data->getEvent())){
    	  foundBox=true;
    	  _nData++;
    	  break;
      }
    }
    if(! foundBox){
      cout << "WARNING in ScpBinning::fillData:"
	   << "\n\t didn't find box for event " 
	   << endl;
//      data->getEvent()->print();
      cout << "compare to first event: " << endl;
//      data->getREvent(0)->print();
      ScpBox box(data->getEvent()->eventPattern());
      cout << "would have fit into large? "
	   << box.addData(data->getEvent()) << endl;
    }
  }
}

void ScpBinning::fillDataCC(IDalitzEventList* data){
  data->Start();
  while(data->Next()){
    bool foundBox=false;
    for(unsigned int i=0; i < _boxSets.size(); i++){
      if(_boxSets[i].addMC(data->getEvent(),1.0)){
	foundBox=true;
	_nDataCC++;
	break;
      }
    }
  }
}


void ScpBinning::sortByScp(){
  lessByScpBoxSetScp sorter;
  stable_sort(_boxSets.rbegin(), _boxSets.rend(), sorter);
}

//void ScpBinning::sortByScp(){
//  lessByScpBoxSetScp sorter;
//  stable_sort(_boxSets.rbegin(), _boxSets.rend(), sorter);
//}
double ScpBinning::setEvents(IDalitzEventList* data
				    , IDalitzEventList* dataCC
				    ){
  bool dbThis=false;
  if(dbThis) cout << "ScpBinning::setEventsAndPdf" << endl;
  if(0 == data) return -9999;
  if(0 == dataCC) return -9999;
  //(could do this: if 0 == pdf: mc generated with pdf from fit result.
  // But for now, leave it like this for safety)

  if(0 == numBins()) createBinning(data);
//  if(0 != fas) setFas(fas);
  if(dbThis) cout << "...number of chi2 bins = " << numBins() << endl;
  resetEventCounts();
  if(dbThis) cout << "...about to fill in the data " << endl;
  fillData(data);
  if(dbThis) cout << "...done that - now the MC" << endl;
  fillDataCC(dataCC);
  if(dbThis) cout << "... fillMC done, now setting norm factors" << endl;
  setBoxesNormFactors();
  if(dbThis) cout << " done the norm factors, now sorting by chi2" << endl;
  sortByScp();
  if(dbThis) cout << " ScpBinning::setEventsAndPdf done" << endl;
  return 0;
}

void ScpBinning::setFas(IFastAmplitudeIntegrable* fas){
  bool dbThis=true;

  const_counted_ptr<IIntegrationCalculator> 
    ic = fas->makeIntegrationCalculator();

  if(dbThis) {
    cout << "ScpBinning::setFas: this is the fas:" << endl;
    fas->print(cout);
    cout << endl;
  }
  for(unsigned int i=0; i < _boxSets.size(); i++){
    counted_ptr<IIntegrationCalculator> fap(ic->clone_IIntegrationCalculator());
    _boxSets[i].setIIntegrationCalculator(fap);
  }
}

void ScpBinning::print(std::ostream& os) const{

  int nDataCheck=0;
  double nMCCheck=0;
  double scpsum=0;
  double sumVarData =0;
  double sumVarMC   =0;
  for(unsigned int i=0; i < _boxSets.size(); i++){
    int n_data =  _boxSets[i].nData();
    nDataCheck += n_data;
    double weight_mc = _boxSets[i].weightedMC() * normFactor();
    nMCCheck += weight_mc;
    
    double var_mc_noNorm = _boxSets[i].rmsMC(_nDataCC);
    double var_mc = var_mc_noNorm* normFactor()*normFactor();
    double varData_expected = weight_mc;
    sumVarData += varData_expected;
    sumVarMC   += var_mc;
    double scp = scp_ofBin(i);
    scpsum += scp;

    os << " bin " << i;
    os << "  data " <<  _boxSets[i].nData();
    os << ", mcweight " << _boxSets[i].weightedMC() * normFactor();
    os << ", (nMC " << _boxSets[i].nMC() << ")";
    os << ", scp " << scp << endl;
    _boxSets[i].printBoxInfo(os);
  }
  os << "\n=========================================================="<< endl;
  os << "scp / nbins = " << scpsum << " / " << numBins()
     << " = " << scpsum/numBins() << endl;
  os << "===========================================================" << endl;
  os << " total data weight: " << nDataCheck
     << " .. _nData = " << _nData
     << " should be same as normalised total MC weight = " << nMCCheck
     << " (un-normalised = " << _totalMCWeight << ")"
     << endl;
  if(sumVarMC > sumVarData){
    os << "Severe WARNING in ScpBinning::getscp_perBin()"
       << "\n error on difference between data and MC dominated by MC"
       << "\n mean data variance: " << sumVarData/numBins()
       << ", ... mc: " << sumVarMC/numBins()
       << ".\n Get more MC!"
       << endl;
  }  
}

double ScpBinning::normFactor() const{
//  return ((double)_nData)/_totalMCWeight;
	return m_norm;
}

void ScpBinning::setBoxesNormFactors(){
  for(unsigned int i=0; i < _boxSets.size(); i++){
    _boxSets[i].setNormFactor(normFactor());
  }
}


double ScpBinning::scp_ofBin(unsigned int i) const{
  if(i > _boxSets.size()) return -9999;
  return _boxSets[i].scp(normFactor());
}

double ScpBinning::getScp_perBin() const{
  if(_nDataCC <= 0) return -9999;
  if(_nData <=0 ) return -9999;

  double sum=0;
  for(unsigned int i=0; i < _boxSets.size(); i++){
    double scp = scp_ofBin(i);
    sum += scp;
  }

  return sum/numBins();
}

double ScpBinning::getMinScp() const{
	double min=9999;
	  for(unsigned int i=0; i < _boxSets.size(); i++){
	    double scp = scp_ofBin(i);
	    if((scp) < (min)) min=scp;
	  }

	  return min;
}
double ScpBinning::getMaxScp() const{

	double max=-1;
	  for(unsigned int i=0; i < _boxSets.size(); i++){
	    double scp = scp_ofBin(i);
	    if((scp) > (max)) max=scp;
	  }
	  std::cout << "Max SCP " << max << std::endl;

	  return max;
}

int ScpBinning::numBins() const{
  return _boxSets.size();
}
/*
void ScpBinning::setHistoColours(){
  double maxChi2 = getMaxChi2();
  for(unsigned int i=0; i < _boxSets.size(); i++){
    double chi2 = chi2_ofBin(i);
    int colIndex = (chi2/maxChi2)*__Ncol;
    int col = (getColourPalette())[colIndex];
    _boxSets[i].setHistoColour(col);
  }
}
*/
void ScpBinning::setHistoColours(){
  double maxScp = getMaxScp();
  double minScp = getMinScp();
  maxScp = 5.0;
  minScp = -5.0;
  for(unsigned int i=0; i < _boxSets.size(); i++){
    double scp = scp_ofBin(i);
    int colIndex = (((scp-minScp)/(maxScp-minScp))*((__Ncol)-1));
    int col = (getColourPalette())[colIndex];
    _boxSets[i].setHistoColour(col);
  }
}
DalitzHistoStackSet ScpBinning::getDataHistoStack(){
  setHistoColours();
  DalitzHistoStackSet hstack;

  for(unsigned int i=0; i < _boxSets.size(); i++){
//	  std::cout << "Adding data: " << _boxSets[i].histoData()
    hstack.add(_boxSets[i].histoData());
  }
  double mx = getMaxScp();
  double min = getMinScp();
  mx = 5.0;
  min = -5.0;
  double diff = mx - min;

  if(mx > 20) mx=20;

  hstack.setColourPalette(__Ncol, __colourPalette, mx, min);
  //cout << "printing newly made data histo stack" << endl;
  //hstack.draw("experimental_chi2DataHisto");
  return hstack;
}
DalitzHistoStackSet ScpBinning::getMCHistoStack(){
  setHistoColours();
  DalitzHistoStackSet hstack;

  for(unsigned int i=0; i < _boxSets.size(); i++){
    hstack.add(_boxSets[i].histoMC());
  }
  double mx = getMaxScp();
  double min = getMinScp();

  if(mx > 20) mx=20;

  hstack.setColourPalette(__Ncol, __colourPalette, mx, min);
  //  cout << "printing newly made MC histo stack" << endl;
  // hstack.draw("experimental_chi2MCHisto");
  return hstack;
}

counted_ptr<TH1D> ScpBinning::getScpDistribution() const{
  int nbins=40;
//  double from=-1*getMaxScp(), to=getMaxScp();
  double from=-5.0, to=5.0;

  //  double from=-2, to=2;

  cout << "from " << from << " to " << to << endl;
  counted_ptr<TH1D> h(new TH1D("Scp distribution", "Scp distribution"
			       , nbins, from, to));
  h->SetDirectory(0);
  for(unsigned int i=0; i < _boxSets.size(); i++){
    //cout << "filling chi2 " << _boxSets[i].chi2() << endl;
    h->Fill(_boxSets[i].scp(m_norm));
  }
  return h;
}
void ScpBinning::drawScpDistribution(const std::string& fname)const{
  TCanvas can;
//  can.SetLogy();
  counted_ptr<TH1D> h(getScpDistribution());
  h->Draw();
  can.Print(fname.c_str());
}


double ScpBinning::Chi2() const{
	double chi2 = 0;
	double norm = this->normFactor();

	for(unsigned int i=0; i < _boxSets.size(); i++){
		//cout << "filling chi2 " << _boxSets[i].chi2() << endl;
		chi2 = chi2 + (_boxSets[i].scp(norm))*(_boxSets[i].scp(norm));
	}
	return chi2;
}

double ScpBinning::Prob() const
{
	double chi2 = this->Chi2();
	double prob = TMath::Prob(chi2,(int)_boxSets.size()-1);
	return prob;
}
double ScpBinning::Prob(double chi2) const
{
	double prob = TMath::Prob(chi2,_boxSets.size()-1);
	return prob;
}

int ScpBinning::ndof() const
{
	return _boxSets.size()-1;
}

bool lessByScpBoxSetScp::operator()(const ScpBoxSet& a, const ScpBoxSet& b) const{

  return fabs(a.scp()) < fabs(b.scp());
}

bool lessByScpBoxData::operator()(const ScpBox& a, const ScpBox& b) const{

  return a.nData() < b.nData();
}


std::ostream& operator<<(std::ostream& os, const ScpBinning& c2b){
  c2b.print(os);
  return os;
}
//
