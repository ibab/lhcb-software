#include "EventCounter.h"
#include "Mint/CLHEPSystemOfUnits.h"

#include "TFile.h"
#include "TCanvas.h"
#include "TStyle.h"

#include <iostream>
#include <cmath>

using namespace std;

EventCounter::EventCounter(std::string name)
  : _numEvents(0)
  , _eventSum(0.0)
  , _weightSum(0.0)
  , _weightSumSq(0.0)
  , _ptmin(0*GeV), _ptmax(8*GeV)
  , _ymin(2), _ymax(4.5)
  , _nbins_pt(8)
  , _nbins_y(5)
  , _name(name)
  , _uncertaintyE(-1)
  , _uncertaintyW(-1)
  , _h((_name+"_unweighted").c_str(), (_name+"_unweighted").c_str()
       , _nbins_pt, _ptmin, _ptmax, _nbins_y, _ymin, _ymax)
  , _hw((_name+"_weighted").c_str(), (_name+"_weighted").c_str()
       , _nbins_pt, _ptmin, _ptmax, _nbins_y, _ymin, _ymax)
{
  _h.Sumw2();
  _hw.Sumw2();
}
EventCounter::~EventCounter(){
  finish();
}

EventCounter::EventCounter(const EventCounter& other
			   , const std::string& newName)
  : _numEvents(other._numEvents)
  , _eventSum(other._eventSum)
  , _weightSum(other._weightSum)
  , _weightSumSq(other._weightSumSq)
  , _ptmin(other._ptmin), _ptmax(other._ptmax)
  , _ymin(other._ymin), _ymax(other._ymax)
  , _nbins_pt(other._nbins_pt)
  , _nbins_y(other._nbins_y)
  , _name(  ( ("" == newName) ? other._name : newName) )
  , _uncertaintyE(other._uncertaintyE)
  , _uncertaintyW(other._uncertaintyW)
  , _h(other._h)
  , _hw(other._hw)
{

}

void EventCounter::addEvent(double pt, double y, double w){
  _numEvents +=1;
  _eventSum +=1;
  _weightSum +=w;
  _weightSumSq += w*w;

  _h.Fill(pt, y, 1);
  _hw.Fill(pt, y, w);
}

double EventCounter::uncertaintyW() const{
  if(_uncertaintyW < 0){
    return uncertaintyWFromEventNumbers();
  }else{
    return _uncertaintyW;
  }
}
double EventCounter::uncertaintyE() const{
  if(_uncertaintyE < 0){
    return uncertaintyEFromEventNumbers();
  }else{
    return _uncertaintyE;
  }
}
double EventCounter::uncertaintyWFromEventNumbers() const{
  //double meanW  = _weightSum/_numEvents;
  //double meanW2 = _weightSumSq/_numEvents;
  //double rmsOnMean   = (meanW2 - meanW*meanW)/_numEvents;
  // we want N*mean -> sigma^2 = (N*sigma_on_mean)^2 + (mean*sqrtN)^2
  //double rms = _numEvents*meanW*meanW + rmsOnMean*_numEvents*_NumEvents;
  double rms = _weightSumSq;//meanW2*_numEvents;
  return sqrt(rms);
}
double EventCounter::uncertaintyEFromEventNumbers() const{
  return sqrt(_eventSum);
}

double EventCounter::sigmaRatio(double N, double sigN, double D, double sigD) const{

  return sqrt((sigN/N)*(sigN/N) 
	      + (sigD/D)*(sigD/D) 
	      )*N/D;
}
void EventCounter::divide(const EventCounter& other){

  double sigE = uncertaintyE();
  double sigW = uncertaintyW();
 
  double o_sigE = other.uncertaintyE();
  double o_sigW = other.uncertaintyW();

  double r_E = _eventSum / other._eventSum;
  double r_W = _weightSum / other._weightSum;

  double sigma_ratioE = sigmaRatio(_eventSum, sigE, other._eventSum, o_sigE);

  double sigma_ratioW = sigmaRatio(_weightSum, sigW, other._weightSum, o_sigW);
    
  _eventSum = r_E;
  _weightSum = r_W;
  
  _uncertaintyE = sigma_ratioE;
  _uncertaintyW = sigma_ratioW;

  double sigma_ratioRatio = sigmaRatio(r_W, sigma_ratioW, r_E, sigma_ratioE);

  cout << _name << " divide: events: " << _eventSum  << " +/- " << sigma_ratioE << endl;
  cout << _name << "        weights: " << _weightSum << " +/- " << sigma_ratioW << endl;
  if(0 != _eventSum){
    cout << _name << " divide: ratio: " << _weightSum/_eventSum << " +/- " << sigma_ratioRatio << endl;
  }
  _h.Divide(&(other._h));
  _hw.Divide(&(other._hw));
}

void EventCounter::drawAndSave(){
  gStyle->SetPalette(1);
  TH2D hr(_hw);
  hr.SetNameTitle("ratio_weight_by_noweights"
		  , "ratio: weighted/noweights");
  hr.Divide(&_h);

  string fname = _name + "noWeights.eps";
  TCanvas can;
  _h.Draw("COLZ");
  _h.Draw("TEXT SAME");
  can.Print(fname.c_str());

  fname = _name + "withWeights.eps";
  TCanvas canw;
  _hw.Draw("COLZ");
  _hw.Draw("TEXT SAME");
  canw.Print(fname.c_str());

  fname = _name + "ratio.eps";
  TCanvas canr;
  hr.Draw("COLZ");
  hr.Draw("E TEXT SAME");
  canr.Print(fname.c_str());
  
  TFile* f = new TFile( rootFname().c_str(), "RECREATE");
  f->cd();
  _h.Write();
  _hw.Write();
  hr.Write();
  f->Close();
}
void EventCounter::finish(){
  drawAndSave();
}
