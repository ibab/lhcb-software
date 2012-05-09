#include "Mint/IDalitzEventList.h"

#include "Mint/ScpBinning.h"
#include "Mint/ScpBox.h"
#include "Mint/ScpBoxSet.h"

#include "Mint/DalitzHistoStackSet.h"

#include "TCanvas.h"
#include "TH1D.h"
#include <stdio.h>
#include <math.h>

#include <algorithm>
#include <iostream>

//#define DEBUG
using namespace MINT;
using namespace std;

int* ScpBinning::__colourPalette=0;
int  ScpBinning::__Ncol=100;

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
  static const int NRGBs = 3;
  if(0 != __colourPalette) delete[] __colourPalette;
  __colourPalette = new int[__Ncol];

	  Double_t stops[NRGBs] = { 0.00, 0.50, 1.00};
	  Double_t red[NRGBs]   = { 1.00, 0.00, 0.00};
	  Double_t white[NRGBs] = { 0.00, 1.00, 0.00};
	  Double_t blue[NRGBs]  = { 0.00, 0.00, 1.00};
	  Int_t Fi = TColor::CreateGradientColorTable(NRGBs
						    , stops
						    , red, white, blue
						    , __Ncol);

	  for(int i=0; i < __Ncol; i++) __colourPalette[i]=Fi+i;
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

}
void ScpBinning::makeColourPalette(){
  makeColourPaletteBlueGrey();
//  makeColourPaletteRGB();
//  makeColourPaletteBlueWhite();
}

int* ScpBinning::getColourPalette(){
  if(0 == __colourPalette) makeColourPalette();
  return __colourPalette;
}

ScpBinning::ScpBinning()
  : _nData(0)
  , _nDataCC(0)
  , _totalMCWeight(0)
  , m_norm(1.0)
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
  if (minPerBin < 20) minPerBin = 20; // Saftey factor as method not valid for few entries


  ScpBoxSet boxes = splitBoxes(events, maxPerBin);

  double norm = this->normFactor();
  lessByScpBoxScp sorter;
  sorter.SetNorm(norm);
  sort(boxes.begin(), boxes.end(), sorter);

//  lessByScpBoxData sorter;
//  sort(boxes.begin(), boxes.end(), sorter);

//
//  ScpBoxSet boxes_SCP_Plus;
//  ScpBoxSet boxes_SCP_Minus;
//
//  for(unsigned int i=0; i < boxes.size(); i++){
//	  if (boxes[i].scp(norm) < 0)
//	  {
//		  boxes_SCP_Minus.add(boxes[i]);
//	  }
//	  else
//	  {
//		  boxes_SCP_Plus.add(boxes[i]);
//	  }
//  }
//    mergeBoxes(boxes_SCP_Plus, minPerBin);
//    mergeBoxes(boxes_SCP_Minus, minPerBin);
//

  mergeBoxes(boxes, minPerBin);

  return numBins();

}



int ScpBinning::mergeBoxes(ScpBoxSet& boxes, int minPerBin){
   lessByScpBoxData sorter;
   sort(boxes.begin(), boxes.end(), sorter);
   double norm = this->normFactor();


//   lessByScpBoxScp sorter;
//   sorter.SetNorm(norm);
//   sort(boxes.begin(), boxes.end(), sorter);

  // this way, the smalles boxes come first 
  // makes sure no empty box is left over at the end.

  double SCP = 0;
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
//    	IDalitzEvent* Evt = data->getEvent();
//    	      if (Evt->phaseSpace() != 0)
	      if (1 == 1)
    	      {
    	    	  if(_boxSets[i].addData(data->getEvent())){
	//    	  data->getEvent()->print();
	//    	  TLorentzVector vec = data->getEvent()->p(1);
	//    	  double Px = vec.Px();
	//    	  double Py = vec.Py();
	//    	  double Pz = vec.Pz();

	//    	  std::cout << "Px: " << Px << " Py: " << Py << " Pz: " << Pz << std::endl;
			  foundBox=true;
			  _nData++;
			  break;
		  }
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
      IDalitzEvent* Evt = data->getEvent();
//      if (Evt->phaseSpace() != 0)
//      {
      if (1 == 1)
	      {
    	  if(_boxSets[i].addMC(data->getEvent(),1.0)){
    		  foundBox=true;
    		  _nDataCC++;
    		  break;
    	  }
      }
    }
  }
}


void ScpBinning::sortByScp(){
  lessByScpBoxSetScp sorter;
  sorter.SetNorm(m_norm);
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
  if(dbThis) cout << "...number of scp bins = " << numBins() << endl;
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
  bool dbThis=false;

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
  int nBoxes = 0;
  for(unsigned int i=0; i < _boxSets.size(); i++){
    int n_data =  _boxSets[i].nData();
    nBoxes = nBoxes + _boxSets[i].size();
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
  os <<	" Number of Boxes "<<  nBoxes
     << " Number of Sets " << _boxSets.size() << endl;
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
	#ifdef DEBUG
  std::cout << "Scp of bin " << i << " " << _boxSets[i].scp(normFactor());
  std::cout << " box size: "<< _boxSets.size() << std::endl;
	#endif
  return _boxSets[i].scp(normFactor());
}

int ScpBinning::Diff_ofBin(unsigned int i) const{
  if(i > _boxSets.size()) return -9999;
  int diff = _boxSets[i].nData() - _boxSets[i].nMC();
  return diff;
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

double ScpBinning::getScpErr_perBin(double mean) const{
  if(_nDataCC <= 0) return -9999;
  if(_nData <=0 ) return -9999;

  double sum=0;
  for(unsigned int i=0; i < _boxSets.size(); i++){
    double scp = scp_ofBin(i);
    sum = sum + (scp - mean)*(scp - mean);
  }
  return sqrt(sum/numBins());
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

	double max=-9999;
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
double round(double d)
{
  return floor(d + 0.5);
}
void ScpBinning::setHistoColours(){
  double maxScp;
  double minScp;
  maxScp = 5.0;
  minScp = -5.0;
  if (getMaxScp()>maxScp){
	  maxScp = getMaxScp()+0.5;
	  minScp = -maxScp;

  }
  if (getMinScp()<minScp){
	  minScp = getMinScp()-0.5;
	  maxScp = -minScp;
  }

  for(unsigned int i=0; i < _boxSets.size(); i++){
    double scp = scp_ofBin(i);
    int colIndex = ((scp-minScp)/(maxScp-minScp))*((__Ncol));
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
  double mx;
  double min;
  mx = 5.0;
  min = -5.0;
  if (getMaxScp()>mx){
	  mx = getMaxScp()+0.5;
	  min = -mx;
  }
  if (getMinScp()<min){
	  min = getMinScp()-0.5;
	  mx = -min;

  }
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
  std::cout << getMaxScp() << " Min: " << getMinScp() << std::endl;
//  double from=-1*getMaxScp(), to=getMaxScp();
  double from=-5.0, to=5.0;
  if (getMaxScp()>to){
	  to = getMaxScp()+1;
	  from = -to;
  }
  if (getMinScp()<from){
	  from = getMinScp()-1;
	  to = -from;
  }

  //  double from=-2, to=2;

//  cout << "from " << from << " to " << to << endl;
  counted_ptr<TH1D> h(new TH1D("Scp distribution", "Scp distribution"
			       , nbins, from, to));
  h->SetDirectory(0);
  for(unsigned int i=0; i < _boxSets.size(); i++){
//    cout << "filling scp " << i << " "<< _boxSets[i].scp() << endl;
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


void ScpBinning::SubtractBackgroundData(IDalitzEventList* data){
  data->Start();
  while(data->Next()){
    bool foundBox=false;
    for(unsigned int i=0; i < _boxSets.size(); i++){
      if(_boxSets[i].subtractData(data->getEvent())){
    	  foundBox=true;
    	  _nData--;
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

void ScpBinning::SubtractBackgroundDataCC(IDalitzEventList* data){
  data->Start();
  while(data->Next()){
    bool foundBox=false;
    for(unsigned int i=0; i < _boxSets.size(); i++){
      if(_boxSets[i].subtractMC(data->getEvent(),1.0)){
	foundBox=true;
	_nDataCC--;
	break;
      }
    }
  }
}

double ScpBinning::setBackgroundEvents(IDalitzEventList* data
				    , IDalitzEventList* dataCC
				    ){
  bool dbThis=false;
  if(dbThis) cout << "ScpBinning::setEventsAndPdf" << endl;
  if(0 == data) return -9999;
  if(0 == dataCC) return -9999;
  //(could do this: if 0 == pdf: mc generated with pdf from fit result.
  // But for now, leave it like this for safety)
//  if(0 != fas) setFas(fas);
  if(dbThis) cout << "...number of scp bins = " << numBins() << endl;
  if(dbThis) cout << "...about to fill in the data " << endl;
  SubtractBackgroundData(data);
  if(dbThis) cout << "...done that - now the MC" << endl;
  SubtractBackgroundDataCC(dataCC);
  if(dbThis) cout << "... fillMC done, now setting norm factors" << endl;
  if(dbThis) cout << " done the norm factors, now sorting by chi2" << endl;
  sortByScp();
  if(dbThis) cout << " ScpBinning::setEventsAndPdf done" << endl;
  return 0;
}

int ScpBinning::NEntires()
{
	return _nData + _nDataCC;
}


int ScpBinning::SetBinning(const char* binningFileName)
{
	 bool dbThis = true;
	 TFile* in_Boxfile = new TFile(binningFileName,"READ");
	 TTree* Binning_tree;
	 Binning_tree=dynamic_cast<TTree*>(in_Boxfile->Get("SCPBinning"));

	 double s_12_min, s_23_min, s_34_min, s_123_min, s_234_min;

	 Binning_tree->SetBranchAddress( "s_12_min",&s_12_min);
	 Binning_tree->SetBranchAddress( "s_23_min",&s_23_min);
	 Binning_tree->SetBranchAddress( "s_34_min",&s_34_min);
	 Binning_tree->SetBranchAddress( "s_123_min",&s_123_min);
	 Binning_tree->SetBranchAddress( "s_234_min",&s_234_min);

	 double s_12_max, s_23_max, s_34_max, s_123_max, s_234_max;

	 Binning_tree->SetBranchAddress( "s_12_max",&s_12_max);
	 Binning_tree->SetBranchAddress( "s_23_max",&s_23_max);
	 Binning_tree->SetBranchAddress( "s_34_max",&s_34_max);
	 Binning_tree->SetBranchAddress( "s_123_max",&s_123_max);
	 Binning_tree->SetBranchAddress( "s_234_max",&s_234_max);

	 int BoxSetNum;
	 Binning_tree->SetBranchAddress( "BoxSetNum",&BoxSetNum);

	 NamedParameter<int> EventPattern("Event Pattern", 421, -211, 211, 211, -211);
	 DalitzEventPattern pdg(EventPattern.getVector());


	 int boxes = Binning_tree->GetEntries();

	 std::vector<pair<ScpBox*, int> > BoxAndSetNumber;

	 int NumBoxSets = -999;

	 for (int entry = 0; entry < boxes; entry++)
	 {
		 Binning_tree->GetEntry(entry);
		 pair<ScpBox*, int> BoxNumPair;

		 if (dbThis)
		 {
			 std::cout << "BoxSet " <<  (int)BoxSetNum << std::endl;
			 std::cout << "min s_12 " <<  s_12_min << std::endl;
			 std::cout << "max s_23 " <<  s_23_max << std::endl;
			 std::cout << "min s_123 " <<  s_123_min << std::endl;
		 }

//		 //Dalitz Coordinates
		 DalitzCoordinate* d_12 = new DalitzCoordinate(1,2);
		 DalitzCoordinate* d_23 = new DalitzCoordinate(2,3);
		 DalitzCoordinate* d_34 = new DalitzCoordinate(3,4);
		 DalitzCoordinate* d_123 = new DalitzCoordinate(1,2,3);
		 DalitzCoordinate* d_234 = new DalitzCoordinate(2,3,4);

		 d_12->setMinMax(s_12_min,s_12_max);
		 d_23->setMinMax(s_23_min,s_23_max);
		 d_34->setMinMax(s_34_min,s_34_max);
		 d_123->setMinMax(s_123_min,s_123_max);
		 d_234->setMinMax(s_234_min,s_234_max);

		 if (dbThis)
		 {
			 std::cout << *d_12 << std::endl;
			 std::cout << *d_23 << std::endl;
		 }

		 // Dalitz Area
		 DalitzArea area(pdg);

		 // Set bins

		 area._s12.setMinMax(s_12_min,s_12_max );
		 area._s23.setMinMax(s_23_min,s_23_max );
		 area._t01.setMinMax(s_123_min,s_123_max );
		 area._s34.setMinMax(s_34_min,s_34_max );
		 area._t40.setMinMax(s_234_min,s_234_max );

		 ScpBox* box = new ScpBox(area);

		 if ((int)BoxSetNum > NumBoxSets)
		 {
			 NumBoxSets = (int)BoxSetNum;
			 ScpBoxSet boxSet;
			 _boxSets.push_back(boxSet);
		 }
		 _boxSets[(int)BoxSetNum].add(*box);

	 }
	 if (dbThis)
	 {
		 std::cout << "Size: " << NumBoxSets << "+1 = " << _boxSets.size() << std::endl;
	 }

	 in_Boxfile->Close();
	 return 1;
}

void ScpBinning::saveBinning(const char* binningFileName)
{
		 TFile* out_Boxfile = new TFile(binningFileName,"RECREATE");
		 TTree *out_BoxTree = new TTree("SCPBinning","SCPBinning");

		 double s_12_min, s_23_min, s_34_min, s_123_min, s_234_min;

		 out_BoxTree->Branch( "s_12_min",&s_12_min);
		 out_BoxTree->Branch( "s_23_min",&s_23_min);
		 out_BoxTree->Branch( "s_34_min",&s_34_min);
		 out_BoxTree->Branch( "s_123_min",&s_123_min);
		 out_BoxTree->Branch( "s_234_min",&s_234_min);

		 double s_12_max, s_23_max, s_34_max, s_123_max, s_234_max;

		 out_BoxTree->Branch( "s_12_max",&s_12_max);
		 out_BoxTree->Branch( "s_23_max",&s_23_max);
		 out_BoxTree->Branch( "s_34_max",&s_34_max);
		 out_BoxTree->Branch( "s_123_max",&s_123_max);
		 out_BoxTree->Branch( "s_234_max",&s_234_max);

		 int BoxSetNum;
		 out_BoxTree->Branch( "BoxSetNum",&BoxSetNum);

		 int size = (int)(_boxSets).size();

		 for (int i = 0; i< size; i++)
		 {

			 int boxsetSize = (_boxSets[i]).size();

			 BoxSetNum = i;

			 for (int j = 0; j< boxsetSize; j++)
			 {
			  s_123_min = (_boxSets)[i][j].area()._t01.min();
			  s_123_max = (_boxSets)[i][j].area()._t01.max();

			  s_12_min = (_boxSets)[i][j].area()._s12.min();
			  s_12_max = (_boxSets)[i][j].area()._s12.max();

			  s_23_min = (_boxSets)[i][j].area()._s23.min();
			  s_23_max = (_boxSets)[i][j].area()._s23.max();

			  s_34_min = (_boxSets)[i][j].area()._s34.min();
			  s_34_max = (_boxSets)[i][j].area()._s34.max();

			  s_234_min = (_boxSets)[i][j].area()._t40.min();
			  s_234_max = (_boxSets)[i][j].area()._t40.max();
			  out_BoxTree->Fill();
			 }
		 }
		 out_Boxfile->Write();
		 out_Boxfile->Close();
}

double ScpBinning::getMinEntries() const{
	double min=9999;
	  for(unsigned int i=0; i < _boxSets.size(); i++){
	    double entries = _boxSets[i].nData();
	    double entriesCC = _boxSets[i].nMC();
	    if((entries) < (min)) min=entries;
	    if((entriesCC) < (min)) min=entriesCC;

	  }

	  return min;
}

void ScpBinning::saveAsNTuple(const char* tuplsFileName, IDalitzEventList* data)
{
	TFile *out_file = new TFile(tuplsFileName,"RECREATE");
	TTree* out_tree = new TTree("SCPVariables","SCPVariables");
	float SCP = 0;
	float PxD0 = 0;

	TBranch *newBranch = out_tree->Branch("SCP", &SCP, "SCP/F");
//	TBranch *newBranchPXD0 = out_tree->Branch("PXD0", &PxD0, "PXD0/F");
//	TBranch *newBranchPYD0 = out_tree->Branch("PYD0", &PyD0, "PYD0/F");

//	TBranch *newBranchPYD0 = out_tree->Branch("PYD0", &PyD0, "PYD0/F");


	data->Start();
	while(data->Next()){
		const IDalitzEvent* Devt = data->getEvent();
		TLorentzVector p0(Devt->p(0));
		TLorentzVector p1(Devt->p(1));
		TLorentzVector p2(Devt->p(2));
		TLorentzVector p3(Devt->p(3));
		TLorentzVector p4(Devt->p(4));
		PxD0 = p0.Px();
		SCP = Scp(Devt);
		#ifdef DEBUG
		std::cout << "SCP for bin " << SCP << endl;
		#endif

		out_tree->Fill();
	  }
	out_tree->Write();
	out_file->Close();
}

double ScpBinning::Scp(const IDalitzEvent* Devt)
{
	double ScpOfEvt = 0;
	for(unsigned int i=0; i < _boxSets.size(); i++){
	      if(_boxSets[i].boxSet(Devt))
	      {
	    	  ScpOfEvt = scp_ofBin(i);
	    	  break;
	      }
	}
	 return ScpOfEvt;

}

bool lessByScpBoxSetScp::operator()(const ScpBoxSet& a, const ScpBoxSet& b) const{
  return (a.scp(m_norm)) < (b.scp(m_norm));
}

bool lessByScpBoxData::operator()(const ScpBox& a, const ScpBox& b) const{

  return a.nData() < b.nData();
}

bool lessByScpBoxScp::operator()(const ScpBox& a, const ScpBox& b) const{
  return a.scp(m_norm) < b.scp(m_norm);
}


std::ostream& operator<<(std::ostream& os, const ScpBinning& c2b){
  c2b.print(os);
  return os;
}
//
