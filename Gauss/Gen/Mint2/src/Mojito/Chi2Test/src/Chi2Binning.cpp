#include "Mint/IDalitzEventList.h"

#include "Mint/Chi2Binning.h"
#include "Mint/Chi2Box.h"
#include "Mint/Chi2BoxSet.h"

#include "Mint/DalitzHistoStackSet.h"

#include "TCanvas.h"
#include "TH1D.h"

#include <algorithm>
#include <iostream>

using namespace MINT;
using namespace std;

int* Chi2Binning::__colourPalette=0;
int  Chi2Binning::__Ncol=256;

void Chi2Binning::makeColourPaletteBlueGrey(){
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
void Chi2Binning::makeColourPaletteBlueWhite(){
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

  for(int i=0; i < __Ncol; i++) __colourPalette[i]=Fi + i;
}
void Chi2Binning::makeColourPaletteRGB(){
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
  
  for(int i=0; i < __Ncol; i++) __colourPalette[i]=Fi + i;
}
void Chi2Binning::makeColourPalette(){
  makeColourPaletteBlueGrey();
  //makeColourPaletteRGB();
  //makeColourPaletteBlueWhite();
}

int* Chi2Binning::getColourPalette(){
  if(0 == __colourPalette) makeColourPalette();
  return __colourPalette;
}

Chi2Binning::Chi2Binning()
  : _nData(0)
  , _nMC(0)
  , _totalMCWeight(0)
{
}
int Chi2Binning::createBinning(MINT::IMinimalEventList<DalitzEvent>* events
			       , int minPerBin
			       , int maxPerBin
			       ){

  if(0 == events) return 0;
  if(0 == events->size()) return 0;

  Chi2BoxSet boxes = splitBoxes(events, maxPerBin);

  mergeBoxes(boxes, minPerBin);

  return numBins();
}

int Chi2Binning::mergeBoxes(Chi2BoxSet& boxes, int minPerBin){
  lessByChi2BoxData sorter;
  sort(boxes.begin(), boxes.end(), sorter);
  // this way, the smalles boxes come first 
  // makes sure no empty box is left over at the end.

  Chi2BoxSet boxSet;
  for(unsigned int i=0; i < boxes.size(); i++){
    boxSet.add(boxes[i]);
    if(boxSet.nData() >= minPerBin){
      _boxSets.push_back(boxSet);
      boxSet.clear();
    }
  }
  
  return _boxSets.size();
}


Chi2BoxSet Chi2Binning::splitBoxes(MINT::IMinimalEventList<DalitzEvent>* events
				   , int maxPerBin
				   ) const{
  bool dbThis=false;
  if(dbThis){
    cout << "Chi2Binning::splitBoxes(MINT::IEventList<DalitzEvent>* events , int maxPerBin = " 
	 << maxPerBin << ") called" << endl;
  }

  Chi2BoxSet dummy;
  if(0 == events) return dummy;
  return splitBoxes(*events, maxPerBin);
}

Chi2BoxSet Chi2Binning::splitBoxes(MINT::IMinimalEventList<DalitzEvent>& events
				   , int maxPerBin
				   ) const{
  bool dbThis=false;
  Chi2BoxSet dummy;
  if(0 == events.size()) return dummy;
  const IDalitzEvent& evt0 = events.getEvent(0);
  
  Chi2Box box(evt0.eventPattern());

  Chi2BoxSet boxes(box.split(0));
  bool needToSplitMore=false;
  int counter=0;
  do{
    counter++;
    boxes.resetEventCounts();
    int failedSet=0, failedBox=0;
    
    for(unsigned int i=0; i < events.size(); i++){
      bool added = boxes.addData(events.getEvent(i));
      if(! added) failedSet++;
      if(! box.addData(events.getEvent(i))){
	failedBox++;
      }
    }
    if(failedSet > 0 || dbThis){
      cout << " Chi2Binning::splitBoxes: ERROR split level " << counter 
	   << ", num boxes: " << boxes.size()
	   << ", failedSet: " << failedSet
	   << ", failedBox: " << failedBox << endl;
    }

    Chi2BoxSet newBoxes;
    needToSplitMore=false;
    for(unsigned int i=0; i < boxes.size(); i++){
      if( boxes[i].nData() > maxPerBin ){
	needToSplitMore=true;
	newBoxes.add(boxes[i].split());
      }else{
	newBoxes.add(boxes[i]);
      }
    }
    boxes = newBoxes;
  }while(needToSplitMore);

  return boxes;
}

void Chi2Binning::resetEventCounts(){
  _nData=0;
  _nMC=0;
  _totalMCWeight=0.0;
  for(unsigned int i=0; i < _boxSets.size(); i++){
    _boxSets[i].resetEventCounts();
  }
}
void Chi2Binning::fillData(IMinimalEventList<DalitzEvent>& data){
  for(unsigned int k=0; k < data.size(); k++){
    bool foundBox=false;
    DalitzEvent evt(data.getEvent(k));
    for(unsigned int i=0; i < _boxSets.size(); i++){
      if(_boxSets[i].addData(evt)){
	foundBox=true;
	_nData++;
	break;
      }
    }
    if(! foundBox){
      cout << "WARNING in Chi2Binning::fillData:"
	   << "\n\t didn't find box for event " 
	   << endl;
      evt.print();
      cout << "compare to first event: " << endl;
      DalitzEvent firstEvt(data.getEvent(0));
      firstEvt.print();
      Chi2Box box(evt.eventPattern());
      cout << "would have fit into large? "
	   << box.addData(evt) << endl;
    }
  }
}
void Chi2Binning::fillMC(IMinimalEventList<DalitzEvent>& mc
			 , IDalitzPdf* pdf){
  bool dbThis=false;
  if(dbThis){
    cout << "Chi2Binning::fillMC called with "
	 << &mc << ", " <<  pdf << endl;
  }
  int printevery=10000;
  for(unsigned int k=0; k < mc.size(); k++){
    bool printit = (0 == k%printevery);
    if(printit) cout << "...fillMC filling event number " << k;
    DalitzEvent evt(mc.getEvent(k));
    double weight = pdf->getVal_noPs(evt) * evt.getWeight() / 
      evt.getGeneratorPdfRelativeToPhaseSpace();
    
    if(printit) cout << ", with weight " << weight << endl;
    int n=0;
    for(unsigned int i=0; i < _boxSets.size(); i++){
      if(_boxSets[i].addMC(evt, weight)){
	_totalMCWeight += weight;
	_nMC++;
	n++;
	break;
      }
    }
    if(dbThis){
      cout << "event " << k << " in " << n << " out of " << numBins()
	   << " box sets" << endl;
      if(0 == n) evt.print();
      cout << "mc weight now: " << _totalMCWeight << endl;
    }
  }
  if(dbThis) cout << "Total MC weight: " << _totalMCWeight << endl;
}

void Chi2Binning::sortByChi2(){
  lessByChi2BoxSetChi2 sorter;
  stable_sort(_boxSets.rbegin(), _boxSets.rend(), sorter);
}
double Chi2Binning::setEventsAndPdf(IMinimalEventList<DalitzEvent>* data
				    , IMinimalEventList<DalitzEvent>* mc
				    , IDalitzPdf* pdf
				    , IFastAmplitudeIntegrable* fas
				    ){
  bool dbThis=false;
  if(dbThis) cout << "Chi2Binning::setEventsAndPdf" << endl;
  if(0 == data) return -9999;
  if(0 == mc) return -9999;
  if(0 == pdf) return -9999;
  //(could do this: if 0 == pdf: mc generated with pdf from fit result.
  // But for now, leave it like this for safety)

  if(0 == numBins()) createBinning(data);
  if(0 != fas) setFas(fas);
  if(dbThis) cout << "...number of chi2 bins = " << numBins() << endl;
  resetEventCounts();
  if(dbThis) cout << "...about to fill in the data " << endl;
  fillData(data);
  if(dbThis) cout << "...done that - now the MC" << endl;
  fillMC(mc, pdf);
  if(dbThis) cout << "... fillMC done, now setting norm factors" << endl;
  setBoxesNormFactors();
  if(dbThis) cout << " done the norm factors, now sorting by chi2" << endl;
  sortByChi2();
  if(dbThis) cout << " Chi2Binning::setEventsAndPdf done" << endl;
  return 0;
}

void Chi2Binning::setFas(IFastAmplitudeIntegrable* fas){
  bool dbThis=true;

  const_counted_ptr<IIntegrationCalculator> 
    ic = fas->makeIntegrationCalculator();

  if(dbThis) {
    cout << "Chi2Binning::setFas: this is the fas:" << endl;
    fas->print(cout);
    cout << endl;
  }
  for(unsigned int i=0; i < _boxSets.size(); i++){
    counted_ptr<IIntegrationCalculator> fap(ic->clone_IIntegrationCalculator());
    _boxSets[i].setIIntegrationCalculator(fap);
  }
}

void Chi2Binning::print(std::ostream& os) const{

  int nDataCheck=0;
  double nMCCheck=0;
  double chi2sum=0;
  double sumVarData =0;
  double sumVarMC   =0;
  for(unsigned int i=0; i < _boxSets.size(); i++){
    int n_data =  _boxSets[i].nData();
    nDataCheck += n_data;
    double weight_mc = _boxSets[i].weightedMC() * normFactor();
    nMCCheck += weight_mc;
    
    double var_mc_noNorm = _boxSets[i].rmsMC(_nMC);
    double var_mc = var_mc_noNorm* normFactor()*normFactor();
    double varData_expected = weight_mc;
    sumVarData += varData_expected;
    sumVarMC   += var_mc;
    double chi2 = chi2_ofBin(i);
    chi2sum += chi2;

    os << " bin " << i;
    os << "  data " <<  _boxSets[i].nData();
    os << ", mcweight " << _boxSets[i].weightedMC() * normFactor();
    os << ", (nMC " << _boxSets[i].nMC() << ")";
    os << ", chi2 " << chi2 << endl;
    _boxSets[i].printBoxInfo(os);
  }
  os << "\n=========================================================="<< endl;
  os << "chi2 / nbins = " << chi2sum << " / " << numBins() 
     << " = " << chi2sum/numBins() << endl;
  os << "===========================================================" << endl;
  os << " total data weight: " << nDataCheck
     << " .. _nData = " << _nData
     << " should be same as normalised total MC weight = " << nMCCheck
     << " (un-normalised = " << _totalMCWeight << ")"
     << endl;
  if(sumVarMC > sumVarData){
    os << "Severe WARNING in Chi2Binning::getChi2_perBin()"
       << "\n error on difference between data and MC dominated by MC"
       << "\n mean data variance: " << sumVarData/numBins()
       << ", ... mc: " << sumVarMC/numBins()
       << ".\n Get more MC!"
       << endl;
  }  
}

double Chi2Binning::normFactor() const{
  return ((double)_nData)/_totalMCWeight;
}

void Chi2Binning::setBoxesNormFactors(){
  for(unsigned int i=0; i < _boxSets.size(); i++){
    _boxSets[i].setNormFactor(normFactor());
  }
}

double Chi2Binning::chi2_ofBin(unsigned int i) const{
  if(i > _boxSets.size()) return -9999;
  return _boxSets[i].chi2(normFactor());
}
double Chi2Binning::getChi2_perBin() const{
  if(_nMC <= 0) return -9999;
  if(_nData <=0 ) return -9999;
  if(_totalMCWeight <= 0.0) return -9999;

  double sum=0;
  for(unsigned int i=0; i < _boxSets.size(); i++){
    double chi2 = chi2_ofBin(i);
    sum += chi2;
  }

  return sum/numBins();
}
double Chi2Binning::getMaxChi2() const{
  double max=-1;
  for(unsigned int i=0; i < _boxSets.size(); i++){
    double chi2 = chi2_ofBin(i);
    if(chi2 > max) max=chi2;
  }
  return max;
}

int Chi2Binning::numBins() const{
  return _boxSets.size();
}
/*
void Chi2Binning::setHistoColours(){
  double maxChi2 = getMaxChi2();
  for(unsigned int i=0; i < _boxSets.size(); i++){
    double chi2 = chi2_ofBin(i);
    int colIndex = (chi2/maxChi2)*__Ncol;
    int col = (getColourPalette())[colIndex];
    _boxSets[i].setHistoColour(col);
  }
}
*/
void Chi2Binning::setHistoColours(){
  double maxChi2 = getMaxChi2();
  for(unsigned int i=0; i < _boxSets.size(); i++){
    double chi2 = chi2_ofBin(i);
    int colIndex = (chi2/maxChi2)*(__Ncol-1);
    int col = (getColourPalette())[colIndex];
    _boxSets[i].setHistoColour(col);
  }
}
DalitzHistoStackSet Chi2Binning::getDataHistoStack(){
  setHistoColours();
  DalitzHistoStackSet hstack;

  for(unsigned int i=0; i < _boxSets.size(); i++){
    hstack.add(_boxSets[i].histoData());
  }
  double mx = getMaxChi2();
  if(mx > 20) mx=20;
  hstack.setColourPalette(__Ncol, __colourPalette, mx);
  //cout << "printing newly made data histo stack" << endl;
  //hstack.draw("experimental_chi2DataHisto");
  return hstack;
}
DalitzHistoStackSet Chi2Binning::getMCHistoStack(){
  setHistoColours();
  DalitzHistoStackSet hstack;

  for(unsigned int i=0; i < _boxSets.size(); i++){
    hstack.add(_boxSets[i].histoMC());
  }
  double mx = getMaxChi2();
  if(mx > 20) mx=20;

  hstack.setColourPalette(__Ncol, __colourPalette, mx);
  //  cout << "printing newly made MC histo stack" << endl;
  // hstack.draw("experimental_chi2MCHisto");
  return hstack;
}

counted_ptr<TH1D> Chi2Binning::getChi2Distribution() const{
  int nbins=100;
  double from=0, to=getMaxChi2();
  cout << "from " << from << " to " << to << endl;
  counted_ptr<TH1D> h(new TH1D("chi2 distribution", "chi2 distribution"
			       , nbins, from, to));
  h->SetDirectory(0);
  for(unsigned int i=0; i < _boxSets.size(); i++){
    //cout << "filling chi2 " << _boxSets[i].chi2() << endl;
    h->Fill(_boxSets[i].chi2());
  }
  return h;
}
void Chi2Binning::drawChi2Distribution(const std::string& fname)const{
  TCanvas can;
  can.SetLogy();
  counted_ptr<TH1D> h(getChi2Distribution());
  h->Draw();
  can.Print(fname.c_str());
}

bool lessByChi2BoxData::operator()(const Chi2Box& a, const Chi2Box& b) const{

  return a.nData() < b.nData();
}

bool lessByChi2BoxSetChi2::operator()(const Chi2BoxSet& a, const Chi2BoxSet& b) const{

  return a.chi2() < b.chi2();
}

std::ostream& operator<<(std::ostream& os, const Chi2Binning& c2b){
  c2b.print(os);
  return os;
}
//
