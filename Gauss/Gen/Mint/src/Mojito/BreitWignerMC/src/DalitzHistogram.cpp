// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT
#include "Mint/DalitzHistogram.h"
#include "Mint/NamedParameter.h"

#include "Mint/CLHEPSystemOfUnits.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/Utils.h"

#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <sstream>

#include <sys/types.h>
#include <sys/stat.h>

#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TObjArray.h"
#include "TH1D.h"
#include "TH2D.h"

using namespace std;
using namespace MINT;

DalitzHistogram::DalitzHistogram() 
  : _units(GeV*GeV), _h(0), _c(), _pat()
  , _name("noName"), _tree(0), _patForTree()
{
}

DalitzHistogram::DalitzHistogram(const DalitzCoordSet& c
				 , const DalitzEventPattern& pat
				 , int bins
				 , double units
				 )
  : _units(units), _nbins(bins), _h(0), _c(), _pat()
  , _name("noName"), _tree(0), _patForTree()
{
  init(c, pat);
}

/*
DalitzHistogram::DalitzHistogram(TTree* t)
  : _units(GeV*GeV), _h(0), _c(), _pat()
  , _name("noName"), _tree(0), _patForTree()
{
  double *u = 0;
  int    *n = 0;
  TH1   *h = 0;
  std::vector<int> *c=0, *p=0;

  cout << "show event" << endl;
  t->Show(0);
  cout << "done that, setting addresses" << endl;
  t->SetBranchAddress("units", &u);
  t->SetBranchAddress("nbins", &n);
  //  t->SetBranchAddress("h", &h);
  t->SetBranchAddress("c", &c);
  t->SetBranchAddress("pat", &p);

  cout << "got to set addresses" << endl;

  _units = *u;
  _nbins = *n;
  counted_ptr<TH1> hptr((TH1*) h->Clone());
  _h = hptr;
  _c = *c;
  _pat = *p;
  //  t->SetBranchAddress("name", &_name);
  
  cout << "units before Get " << _units << endl;
  t->Show(0);
  cout << "units after Get " << _units << endl;
  
  DalitzCoordinate co(*c);
  _c = co;
  DalitzEventPattern pa(*p);
  _pat = pa;
  
  makeName();
}
*/

DalitzHistogram::DalitzHistogram(const DalitzHistogram& other)
  : _units(other._units)
  , _nbins(other._nbins)
  , _h(0)
  , _c(other._c)
  , _pat(other._pat)
  , _name(other._name)
  , _tree(0)
  , _patForTree(other._patForTree)
{
  bool dbThis=false;
  if(0 != other._h){
    _h = counted_ptr<TH1>((TH1*) other._h->Clone());
    _h->SetDirectory(0);
  }
  if(dbThis){
    cout << "DalitzHistogram copy ctor" << endl;
    cout << "pat before: " << other._pat
	 << ", and after " << _pat << endl;
  }
}

DalitzHistogram& DalitzHistogram::operator=(const DalitzHistogram& other){
  if(0 != other.histo()){
    _h = counted_ptr<TH1>((TH1*) other.histo()->Clone());
    _h->SetDirectory(0);
  }else{
    _h = counted_ptr<TH1>(0);
  }
  _nbins = other._nbins;
  _c     = other._c;
  _pat   = other._pat;
  _name  = other._name;
  _tree  = 0;
  _patForTree = other._patForTree;
  return *this;
}
std::string convertInt(int number)
{
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}
void DalitzHistogram::smartTitle(  const DalitzCoordSet& c_in
				 , const DalitzEventPattern& pat_in)
{
//	pat_in
	TString c_out = c_in.name();
	for (unsigned int i = 1; i< pat_in.size(); i++)
	{
		std::string sij = convertInt(i);
		c_out.ReplaceAll(sij,pat_in[i].name());
	}
	_smartTitle = c_out;
}

const std::string DalitzHistogram::smartTitle() const
{
	return _smartTitle;
}
void DalitzHistogram::init(const DalitzCoordSet& c_in
			   , const DalitzEventPattern& pat_in
			   , int bins
			   , double units
			   ){

  // this needs to change!
  _c   = c_in;
  _pat = pat_in;
  _nbins = bins;
  _units = units;
  _patForTree = _pat.getVectorOfInts();

  smartTitle(c_in,pat_in);


  double mi=0, ma=0;
  for(std::map<DalitzCoordKey, DalitzCoordinate>::iterator it = _c.begin();
      it != _c.end(); it++){
    double eps   = 0.1;
    
    mi = _pat.sijMin(it->second)/_units;
    ma = _pat.sijMax(it->second)/_units;
    
    mi -= (ma-mi)*eps;
    ma += (ma-mi)*eps;
    
    it->second.setMin(mi);
    it->second.setMax(ma);
  }

  makeName();

  counted_ptr<TH1D> local_h(new TH1D(name().c_str(), name().c_str()
				     , _nbins, mi, ma));
  local_h->SetDirectory(0);
  local_h->SetNameTitle(hname().c_str(), smartTitle().c_str());
  local_h->Sumw2();
  local_h->SetLineWidth(2);

  std::string unitsName;
  if     (_units == TeV*TeV) unitsName = "TeV^2";
  else if(_units == GeV*GeV) unitsName = "GeV^2";
  else if(_units == MeV*MeV) unitsName = "MeV^2";
  else if(_units == keV*keV) unitsName = "keV^2";
  else if(_units ==  eV* eV) unitsName = "eV^2";
  else{
    unitsName = anythingToString((int) (_units/(GeV*GeV))) + " GeV^2";
  }

  std::string XLabel = _c.name() + " [" + unitsName + "]";
  local_h->GetXaxis()->SetTitle(XLabel.c_str());

  double bw = (ma - mi)/((double)_nbins);
  char s[100]={'\0'};
  sprintf(s, "%f", bw);
  std::string st(s);
  std::string YLabel = "Events / " + st + " " + unitsName;

  local_h->GetYaxis()->SetTitle(YLabel.c_str());

  _h = local_h;

  return;
}
				 
				 
void DalitzHistogram::add(const DalitzHistogram& addMeToYou, double weight){
  bool dbThis=false;

  if(0 == addMeToYou.histo()){
    cout << "WARNING in DalitzHistogram::add(const DalitzHistogram&): "
	 << " Trying to add a DalitzHistogram w/o histogram to me"
	 << " I'll treat it as a zero (i.e. I'll do nothing)."
	 << endl;
    return;
  }
  if(0 == histo()){
    if(dbThis){
      cout << "WARNING in DalitzHistogram::add(const DalitzHistogram&): "
	   << " Trying to add a DalitzHistogram to me although I have"
	   << " no histogram myself - treating myself as zero."
	   << endl;
    }
    (*this) = addMeToYou;
    makeName();
    _h->SetNameTitle(hname().c_str(), htitle().c_str());
    // same as before, but repeated for robustness - in case
    // the other histogram had a funny name.
    return;
  }
  if(_c != addMeToYou._c){
    cout << "WARNING in DalitzHistogram::add(const DalitzHistogram&): "
	 << " adding histograms for different co-ordinates!"
	 << "\n\t" << _c << " != " << addMeToYou._c
	 << "\n\t I'll go ahead with it, but it might not be what you want."
	 << endl;
  }
  _h->Add((addMeToYou.histo().get()), weight);
  return;
}
void DalitzHistogram::multiply(const DalitzHistogram& multiplyWith){
  bool dbThis=false;

  if(0 == multiplyWith.histo()){
    cout << "WARNING in DalitzHistogram::multiply(const DalitzHistogram&): "
	 << " Trying to mulitply a DalitzHistogram w/o histogram to me"
	 << " I'll treat it as a zero (i.e. I'll clear myself of entries)."
	 << endl;
    this->clearHisto();
    return;
  }
  if(0 == histo()){
    if(dbThis){
      cout << "WARNING in DalitzHistogram::multiply(const DalitzHistogram&): "
	   << " Trying to multiply a DalitzHistogram to me although I have"
	   << " no histogram myself - treating myself as zero."
	   << endl;
    }
    _h = counted_ptr<TH1>((TH1*) multiplyWith.histo()->Clone());
    _h->SetDirectory(0);
    _c = multiplyWith._c;
    makeName();
    _h->SetNameTitle(hname().c_str(), htitle().c_str());
    _h->Clear();
    return;
  }
  if(_c != multiplyWith._c){
    cout << "WARNING in DalitzHistogram::multiply(const DalitzHistogram&): "
	 << " multiplying histograms for different co-ordinates!"
	 << "\n\t" << _c << " != " << multiplyWith._c
	 << "\n\t I'll go ahead with it, but it might not be what you want."
	 << endl;
  }
  _h->Multiply((multiplyWith.histo().get()));
  return;
}
void DalitzHistogram::divide(const DalitzHistogram& divideBy){
  bool dbThis=false;

  if(0 == divideBy.histo()){
    cout << "WARNING in DalitzHistogram::divide(const DalitzHistogram&): "
	 << " Trying to divide a DalitzHistogram w/o histogram to me"
	 << " I guess I should crash, but I'll do nothing."
	 << endl;
    //    this->clearHisto();
    return;
  }
  if(0 == histo()){
    if(dbThis){
      cout << "WARNING in DalitzHistogram::divide(const DalitzHistogram&): "
	   << " Trying to divide a DalitzHistogram to me although I have"
	   << " no histogram myself - treating myself as zero."
	   << endl;
    }
    _h = counted_ptr<TH1>((TH1*) divideBy.histo()->Clone());
    _h->SetDirectory(0);
    _c = divideBy._c;
    makeName();
    _h->SetNameTitle(hname().c_str(), htitle().c_str());
    _h->Clear();
    return;
  }
  if(_c != divideBy._c){
    cout << "WARNING in DalitzHistogram::divide(const DalitzHistogram&): "
	 << " divideing histograms for different co-ordinates!"
	 << "\n\t" << _c << " != " << divideBy._c
	 << "\n\t I'll go ahead with it, but it might not be what you want."
	 << endl;
  }
  _h->Divide((divideBy.histo().get()));
  return;
}
void DalitzHistogram::addEvent(const IDalitzEvent* evtPtr, double weight){
  if(0 == evtPtr) return;
  // needs to change!!
  _h->Fill(evtPtr->sij(_c.begin()->second)/_units, weight);
}

void DalitzHistogram::scale(double sf){
  if(0 == _h) return;
  _h->Scale(sf);
}
void DalitzHistogram::setTitle(const std::string& title){
  _h->SetTitle(title.c_str());
}
void DalitzHistogram::clearHisto(){
  if(0 == _h) return;
    _h->Clear();
  //_h->Scale(0.0);
}

bool DalitzHistogram::draw(const std::string& baseName // =""
			   , const std::string& drawOpt// =""
			   , const std::string& format// = "eps"
			   ) const{
  if(0 == _h) return false;
  string fname = baseName + _c.nameFileSave() + "." + format;
  TCanvas can;
  _h->Draw(drawOpt.c_str());
  can.Print(fname.c_str());
  return true;
}

void DalitzHistogram::makeName(){
  _name = _c.name();
}

/*
TTree* DalitzHistogram::makeTree() const{
  _tree = new TTree((name()).c_str(), (name()).c_str());
  _tree->Branch("units", const_cast<double*> (&_units));
  _tree->Branch("nbins", const_cast<int*>(&_nbins));
  cout << "TH1 pointer: " << (TH1*) const_cast<TH1*>( (TH1*) (_h.get()))
       << endl;
  _tree->Branch("h", (TH1*) const_cast<TH1*>( (TH1*) (_h.get())));
  _tree->Branch("c", const_cast<std::vector<int>*>((std::vector<int>*)&_c));
  _tree->Branch("pat", const_cast<std::vector<int>*>(&_patForTree));
  return _tree;
}
TTree* DalitzHistogram::ttree() const{
  //  if(0 == _tree) makeTree();
  return _tree;
}

bool DalitzHistogram::writeTree() const{
  makeTree();
  ttree()->Fill();

  ttree()->Write();
  cout << "showin tree just written" << endl;
  ttree()->Show();
  //_h->Write();
  return true;
}
*/
bool DalitzHistogram::makeDirectory(const std::string& asSubdirOf)const{
  /*
    A mode is created from or'd permission bit masks defined
     in <sys/stat.h>:
           #define S_IRWXU 0000700     RWX mask for owner 
           #define S_IRUSR 0000400     R for owner 
           #define S_IWUSR 0000200     W for owner 
           #define S_IXUSR 0000100     X for owner 

           #define S_IRWXG 0000070     RWX mask for group 
           #define S_IRGRP 0000040     R for group 
           #define S_IWGRP 0000020     W for group 
           #define S_IXGRP 0000010     X for group 

           #define S_IRWXO 0000007     RWX mask for other 
           #define S_IROTH 0000004     R for other 
           #define S_IWOTH 0000002     W for other 
           #define S_IXOTH 0000001     X for other 

           #define S_ISUID 0004000     set user id on execution 
           #define S_ISGID 0002000     set group id on execution 
           #define S_ISVTX 0001000     save swapped text even after use
   */

  mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO 
              | S_ISUID | S_ISGID; 
  // see above for meaning. I want everybody to be allowed to read/write/exec.
  // Not sure about the last two bits.

  int zeroForSuccess = 0;
  zeroForSuccess |= mkdir( (asSubdirOf ).c_str(), mode );
  zeroForSuccess |= mkdir( (asSubdirOf + "/" + dirName() ).c_str(), mode );
  return (0 == zeroForSuccess);
}

bool DalitzHistogram::saveAsDir(const std::string& asSubdirOf) const{
  makeDirectory(asSubdirOf); // ignore error codes from mkdir, they could
  saveValues(asSubdirOf);
  saveHisto(asSubdirOf);
  return true;
}
bool DalitzHistogram::retrieveFromDir(const std::string& asSubdirOf
				      , const char* theName){
  bool success = true;
  if(0 != theName) _name = theName;
  success |= retrieveValues(asSubdirOf, theName);
  
  success |= retrieveHisto(asSubdirOf, theName);
  return success;
}
const std::string& DalitzHistogram::dirName() const{
  return name();
}
const std::string DalitzHistogram::hname() const{
  std::string s;
  for(std::string::const_iterator it = name().begin();
      it != name().end(); it++){
    if('(' == *it || ')'== *it) s += "_";
    else if(',' == *it) s += "_";
    else s+= *it;
  }
  return s;
}
const std::string& DalitzHistogram::htitle() const{
  return name();
}
std::string DalitzHistogram::valueFileName(const std::string& asSubdirOf
					   , const char* theName)const{
  std::string dir;
  if(0 == theName){
    dir=dirName();
  }else{
    dir=theName;
  }
  return asSubdirOf + "/" + dir + "/value.txt";
}
std::string DalitzHistogram::histoFileName(const std::string& asSubdirOf
					   , const char* theName)const{
  std::string dir;
  if(0 == theName){
    dir=dirName();
  }else{
    dir=theName;
  }
  return asSubdirOf + "/" + dir + "/histo.root";
}

bool DalitzHistogram::saveValues(const std::string& inDir) const{

  NamedParameter<double> n_units("units"
				 , NamedParameterBase::QUIET);
  NamedParameter<int> n_nbins("nbins"
				 ,NamedParameterBase::QUIET);
  NamedParameter<int> n_c("c"
			  , NamedParameterBase::QUIET);
  NamedParameter<int> n_pat("pat"
			    , NamedParameterBase::QUIET);
  NamedParameter<std::string> n_name("name"
				     , NamedParameterBase::QUIET);
 
  n_units = _units;
  n_nbins  = _nbins;
  n_c     = (std::vector<int>) _c.begin()->first; // needs to change
  //  cout << "pat = " << _pat << endl;
  n_pat   = _pat.getVectorOfInts();
  // cout << "n_pat = " << n_pat << endl;
  n_name  = _name;

  std::string fname = valueFileName(inDir);
  ofstream os(fname.c_str());
  if(os.bad()){
    cout << "ERROR in DalitzHistogram::saveValues of \n\t" << name()
	 << "\n\t unable to create file: "
	 << "\n\t" << fname << endl;
    return false;
  }
  os << name() 
     << '\n' << n_units
     << '\n' << n_nbins
     << '\n' << n_c 
     << '\n' << n_pat
     << '\n' << n_name
     << endl;
  os.close();
  return true;
}
bool DalitzHistogram::retrieveValues(const std::string& fromDirectory
				     , const char* theName){
  bool dbThis=false;
  std::string fname = valueFileName(fromDirectory, theName);
  if(dbThis)cout << "trying fname = " << fname << endl;
  NamedParameter<double> n_units("units"
				 , fname.c_str()
				 //, NamedParameterBase::QUIET
				 );
  n_units.reloadFile(fname.c_str());
  NamedParameter<int> n_nbins("nbins"
			      ,fname.c_str()
			      //, NamedParameterBase::QUIET
			      );
  NamedParameter<int> n_c("c"
			  , fname.c_str()
			  //, NamedParameterBase::QUIET
			  );
  NamedParameter<int> n_pat("pat"
			    , fname.c_str()
			    //, NamedParameterBase::QUIET
			    );
  NamedParameter<std::string> n_name("name"
				     , fname.c_str()
				     //, NamedParameterBase::QUIET
				     );

  _units = n_units;
  _nbins = n_nbins;
  DalitzCoordinate co(n_c.getVector());
  _c = (DalitzCoordSet) co;
  DalitzEventPattern pa(n_pat.getVector());
  _pat = pa;
  //cout << "after reading in, the pattern is " << _pat << endl;
  _name = n_name;

  return true;
}

bool DalitzHistogram::saveHisto(const std::string& inDir) const{
  std::string fn = histoFileName(inDir);
  TFile f(fn.c_str(), "RECREATE");
  _h->Write();
  f.Close();
  return true;
}

bool DalitzHistogram::retrieveHisto(const std::string& fromDir
				    , const char* theName){
  bool dbThis=false;
  std::string fn = histoFileName(fromDir, theName);
  //  _name = theName();
  TFile f(fn.c_str(), "READ");
  TH1* th = (TH1*) f.Get(hname().c_str());
  if(0 == th){
    cout << "ERROR in DalitzHistogram::retrieveHisto"
	 << "\n\t can't find histogram " << hname()
	 << "\n\t in file " << fn << endl;
    throw "errorRetrievingHisto";
  }
  counted_ptr<TH1> cth((TH1*) th->Clone());
  cth->SetDirectory(0);
  _h = cth;
  f.Close();
  if(dbThis){
    cout << "DalitzHistogram::retrieveHisto"
	 << "\n\t retrieved histogram " << hname()
	 << "\n\t in file " << fn << endl;
    cout << "\t the ptr " << _h.get() << endl;
    cout << "\t the name " << _h->GetTitle() << endl;
  }
  return true;
}

bool DalitzHistogram::drawWithFit(const DalitzHistogram& fit
				  , const std::string& baseName // =""
				  , const std::string& format
				  , const std::string& fitDrawOpt
				  ) const{
  if(0 == _h) return false;
  string fname = baseName + _c.nameFileSave() + "." + format;
  TCanvas can;

  counted_ptr<TH1> h_c( (TH1*) histo()->Clone());
  counted_ptr<TH1> fit_c( (TH1*) fit.histo()->Clone());

  double maxiThis = h_c->GetMaximum();//Stored();
  double maxiThat = fit_c->GetMaximum();//Stored();

  if(maxiThat > maxiThis) h_c->SetMaximum(maxiThat*1.05);
  //  else h_c->SetMaximum(maxiThis);

  h_c->Draw("E1");

  fit_c->SetLineWidth(3);
  fit_c->SetLineColor(2);
  fit_c->SetMarkerColor(2);

  fit_c->Draw(fitDrawOpt.c_str());
  can.Print(fname.c_str());
  return true;
}

bool DalitzHistogram::drawWithFit( TCanvas& can
			      , const DalitzHistogram& fit
				  , const std::string& baseName // =""
				  , const std::string& format
				  , const std::string& fitDrawOpt
				  ) const{
  if(0 == _h) return false;
  string fname = baseName + _c.nameFileSave() + "." + format;

  counted_ptr<TH1> h_c( (TH1*) histo()->Clone());
  counted_ptr<TH1> fit_c( (TH1*) fit.histo()->Clone());

  double maxiThis = h_c->GetMaximum();//Stored();
  double maxiThat = fit_c->GetMaximum();//Stored();

  if(maxiThat > maxiThis) h_c->SetMaximum(maxiThat*1.05);
  //  else h_c->SetMaximum(maxiThis);

  h_c->Draw("E1");

  fit_c->SetLineWidth(3);
  fit_c->SetLineColor(2);
  fit_c->SetMarkerColor(2);

  fit_c->Draw(fitDrawOpt.c_str());
  can.Print(fname.c_str());
  return true;
}
MINT::counted_ptr<TH1> DalitzHistogram::getHisto(){
	return _h;
}

bool DalitzHistogram::drawEachAmp( TCanvas& // can
			   , const std::string& baseName // =""
			   , const std::string& drawOpt// =""
			   , const std::string& format// = "eps"
			   ) const{
  if(0 == _h) return false;
  string fname = baseName + _c.nameFileSave() + "." + format;
  _h->Draw(drawOpt.c_str());
//  can.Print(fname.c_str());
  return true;
}


double DalitzHistogram::integral() const{
  if(0 == _h) return 0;
  return _h->Integral();
}

void DalitzCoordSet::print(std::ostream& os) const{
  os << this->name();
}


// ----

std::ostream& operator<<(std::ostream& os, const DalitzCoordSet& dcs){
  dcs.print(os);
  return os;
}

// =====
