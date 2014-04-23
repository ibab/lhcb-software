#include "Mint/DalitzHistoStackSet.h"
#include "TCanvas.h"
#include "TList.h"
#include "TPaletteAxis.h"
#include "TStyle.h"

using namespace std;
using namespace MINT;

DalitzHistoStackSet::DalitzHistoStackSet()
  : DalitzSet<THStack>()
  , _ncol(0)
  , _palette(0)
  , _max(0)
  , _min(0)
  , _h2ForPalette(0)
{
}

DalitzHistoStackSet::DalitzHistoStackSet(const DalitzHistoStackSet& other)
  : DalitzSet<THStack>(other)
  , _histograms(other._histograms)
  , _h2ForPalette(other._h2ForPalette)
{
  setColourPalette(other._ncol, other._palette, other._max);
}

MINT::counted_ptr<THStack> 
DalitzHistoStackSet::makeHStack(const DalitzCoordSet& coord) const{
  std::string thisName = name() + "_stack " + coord.name();
  counted_ptr<THStack> ptr(new THStack(thisName.c_str(),thisName.c_str()));
  return ptr;
}

void DalitzHistoStackSet::add(const DalitzCoordSet& coord
			      , const counted_ptr<TH1>& histo){
  if(0 == histo) return;
  if(0 == (*this)[coord]){
    (*this)[coord] = makeHStack(coord);
  }
  _histograms.push_back(histo);
  ((*this)[coord])->Add(histo.get());
}
void DalitzHistoStackSet::add(const DalitzHistogram& histo){
  add(histo.coord(), histo.histo());
}
void DalitzHistoStackSet::add(const DalitzHistoSet& hL){
  for(map< DalitzCoordSet, DalitzHistogram>::const_iterator it = hL.begin();
      it != hL.end();
      it++){
    this->add(it->second);
  }
  return;
}

void DalitzHistoStackSet::makeH2ForPalette() const{
  if(_ncol <= 0 || 0 == _palette){
    counted_ptr<TH2F> ptr(0);
    _h2ForPalette = ptr;
  }
  gStyle->SetPalette(_ncol, _palette);
  gStyle->SetNumberContours(_ncol);


  counted_ptr<TH2F> h2(new TH2F("blub", "bla", 2, 0, 2, 2, 0, 2));
  h2->Fill(0.5, 0.5, 0.5*_min);
  h2->Fill(1.5, 1.5, _min);
  h2->Fill(0.5, 1.5, 0.5*_max);
  h2->Fill(1.5, 0.5, _max);
  TCanvas can;
  h2->Draw("COLZ");
  can.Print("colourScale.eps");
  _h2ForPalette = h2;
}

MINT::counted_ptr<TH2F> DalitzHistoStackSet::getH2ForPalette() const{
  if(0 == _h2ForPalette) makeH2ForPalette();
  return _h2ForPalette;
}


TPaletteAxis* DalitzHistoStackSet::getPaletteAxis() const{
  if(0 == getH2ForPalette()) return 0;
  return (TPaletteAxis*) getH2ForPalette()->GetListOfFunctions()->FindObject("palette");
}

bool DalitzHistoStackSet::draw(const std::string& baseName
			  , const std::string& drawOpt
			  , const std::string& format
			  ) const{
  bool sc=true;

  TPaletteAxis* pa = getPaletteAxis();

  cout << "Hello from DalitzHistoStackSet::draw"
       << " with " << this->size() << " histo stacks" << endl;

  for(map< DalitzCoordSet, counted_ptr<THStack> >::const_iterator 
	it = this->begin();
      it != this->end();
      it++){
    if(0 == it->second){
      cout << "it->second is 0" << endl;
      continue;
    }
    TCanvas can;
    DalitzCoordSet c(it->first);
    string fname = baseName + c.nameFileSave() + "." + format;
  
    it->second->Draw(drawOpt.c_str());
    cout << "drew histostack of size " << it->second->GetHists()->GetSize()  
	 << endl;
    if(0 != pa) pa->Draw("SAME");
    can.Print(fname.c_str());
    cout << "printed histo to file " << fname << endl;
  }
  return sc;
}

void DalitzHistoStackSet::setColourPalette(int nCol, int* pal, double max, double min){
  if(0 == pal) return;
  if(0 == nCol) return;

  if(0 != _palette) delete[] _palette;
  _ncol=nCol;
  _palette = new int[_ncol];
  for(int i=0; i < _ncol; i++) _palette[i] = pal[i];
  _max = max;
  _min = min;
}

DalitzHistoStackSet::~DalitzHistoStackSet(){
  if(0 != _palette) delete[] _palette;
}

//
//
