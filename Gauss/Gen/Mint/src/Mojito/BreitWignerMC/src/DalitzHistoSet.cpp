// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:58 GMT
#include "Mint/DalitzHistoSet.h"

#include "Mint/AllPossibleSij.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/NamedParameter.h"

#include "TTree.h"
#include "TFile.h"
#include "TLegend.h"

#include <sys/types.h>
#include <sys/stat.h>

#include <fstream>

using namespace std;
using namespace MINT;

DalitzHistoSet::DalitzHistoSet() 
  : _tree(0) 
{
  makeName();
}

DalitzHistoSet::DalitzHistoSet(const DalitzHistoSet& other)
  :  std::map< DalitzCoordSet, DalitzHistogram>( other)
  , _tree(0)
{
  makeName();
}

void DalitzHistoSet::makeHistograms(const DalitzEventPattern& pat){

  int nDaughters = pat.numDaughters();
  AllPossibleSij sijList(nDaughters);
  
  for(namedVMap::const_iterator it = sijList.begin();
      it != sijList.end(); it++){
    DalitzHistogram h(it->second, pat);
    (*this)[it->second] = h;
  }

  return;
}
 
void DalitzHistoSet::add(const DalitzHistogram& histo, double weight){
  (*this)[histo.coord()].add(histo, weight);
  return;
}
void DalitzHistoSet::add(const DalitzHistoSet& hL, double weight){
  for(map< DalitzCoordSet, DalitzHistogram>::const_iterator it = hL.begin();
      it != hL.end();
      it++){
    this->add(it->second, weight);
  }
  return;
}
void DalitzHistoSet::multiply(const DalitzHistogram& histo){
  (*this)[histo.coord()].multiply(histo);
  return;
}
void DalitzHistoSet::multiply(const DalitzHistoSet& hL){
  for(map< DalitzCoordSet, DalitzHistogram>::const_iterator it = hL.begin();
      it != hL.end();
      it++){
    this->multiply(it->second);
  }
  return;
}
void DalitzHistoSet::divide(const DalitzHistogram& histo){
  (*this)[histo.coord()].divide(histo);
  return;
}
void DalitzHistoSet::divide(const DalitzHistoSet& hL){
  for(map< DalitzCoordSet, DalitzHistogram>::const_iterator it = hL.begin();
      it != hL.end();
      it++){
    this->divide(it->second);
  }
  return;
}

void DalitzHistoSet::addEvent(const IDalitzEvent* evtPtr, double weight){
  if(0 == evtPtr) return;
  int ndgtr(evtPtr->eventPattern().numDaughters());
  if(this->empty() && ndgtr > 2) makeHistograms(evtPtr->eventPattern());

  for(map< DalitzCoordSet, DalitzHistogram>::iterator 
	it = this->begin();
      it != this->end();
      it++){
    it->second.addEvent(evtPtr, weight);
  }
  return;
}

void DalitzHistoSet::scale(double sf){
  if(this->empty()) return;
  for(map< DalitzCoordSet, DalitzHistogram>::iterator 
	it = this->begin();
      it != this->end();
      it++){
    it->second.scale(sf);
  }
  return;
}

void DalitzHistoSet::setNormFactor(double sf){
  if(this->empty()) return;
  for(map< DalitzCoordSet, DalitzHistogram>::iterator 
	it = this->begin();
      it != this->end();
      it++){
    it->second.setNormFactor(sf);
  }
  return;
}

void DalitzHistoSet::setTitle(const std::string& title){
  if(this->empty()) return;
  for(map< DalitzCoordSet, DalitzHistogram>::iterator 
	it = this->begin();
      it != this->end();
      it++){
    it->second.setTitle(title);
  }
  return;
}
void DalitzHistoSet::setFillColour(Color_t fcolor){
  if(this->empty()) return;
  for(map< DalitzCoordSet, DalitzHistogram>::iterator 
	it = this->begin();
      it != this->end();
      it++){
    it->second.histo()->SetFillColor(fcolor);
  }
  return;
}
void DalitzHistoSet::setLineColour(Color_t fcolor){
  if(this->empty()) return;
  for(map< DalitzCoordSet, DalitzHistogram>::iterator 
	it = this->begin();
      it != this->end();
      it++){
    it->second.histo()->SetLineColor(fcolor);
    it->second.histo()->SetLineWidth(2);
  }
  return;
}



void DalitzHistoSet::clearAllHistos(){
  if(this->empty()) return;
  for(map< DalitzCoordSet, DalitzHistogram>::iterator 
	it = this->begin();
      it != this->end();
      it++){
    it->second.clearHisto();
  }
  return;
}

const std::string& DalitzHistoSet::makeName(){
  _name = "DalitzHistoSet";
  return _name;
}

const std::string& DalitzHistoSet::name() const{
  return _name;
}

const std::string& DalitzHistoSet::dirName() const{
  return _name;
}

bool DalitzHistoSet::save(const std::string& filename) const{
  TFile f(filename.c_str(), "RECREATE");
  f.cd();
  for(map< DalitzCoordSet, DalitzHistogram>::const_iterator 
	it = this->begin();
      it != this->end();
      it++){
    if(0 == it->second.histo()) continue;
    it->second.histo()->Write();
  }
  f.Close();
  return true;
}

bool DalitzHistoSet::makeDirectory(const std::string& asSubdirOf)const{
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

std::string DalitzHistoSet::fullDirListFname(const std::string& 
					     asSubdirOf) const{
  return asSubdirOf + "/" + dirName() + "/" + "directoryList.txt";
}
bool DalitzHistoSet::saveAsDir(const std::string& asSubdirOf) const{
  bool dbThis=false;

  bool sc=true;

  makeDirectory(asSubdirOf);
  std::string dir = asSubdirOf + "/" + dirName();
  
  std::vector<std::string> directoryList;
  for(map< DalitzCoordSet, DalitzHistogram>::const_iterator 
	it = this->begin();
      it != this->end();
      it++){
    sc &= it->second.saveAsDir(dir);
    directoryList.push_back(it->second.dirName());
  }
  
  NamedParameter<std::string> dirNames("DirectoryList"
				       , NamedParameterBase::QUIET);
  dirNames = directoryList;

  ofstream os( fullDirListFname(asSubdirOf).c_str() );
  if(os.bad()){
    cout << "ERROR in DalitzHistoSet::saveAsDir of \n\t" << name()
	 << "\n\t unable to create file: "
	 << "\n\t" << fullDirListFname() << endl;
    return false;
  }
  os << name()
     << '\n' << dirNames
     << endl;
  os.close();

  if(dbThis){
    cout << "I've just written this: "
	 << name()
	 << '\n' << dirNames
	 << endl;
    cout << "to this file: " << fullDirListFname(asSubdirOf) << endl;
  }
  return sc;
}

bool DalitzHistoSet::retrieveFromDir(const std::string& asSubdirOf){
  bool dbThis=false;
  bool sc=true;

  makeDirectory(asSubdirOf);
  //path = asSubdirOf + "/" + dirName();
  NamedParameter<std::string> dirNames("DirectoryList"
				       , fullDirListFname(asSubdirOf).c_str()
				       );
  dirNames.reloadFile(fullDirListFname(asSubdirOf).c_str());
  std::string path=asSubdirOf + "/" + dirName();
  for(int i=0; i < dirNames.size(); i++){
    DalitzHistogram hs;    
    hs.retrieveFromDir( path, dirNames.getVal(i).c_str() );
    if(dbThis) cout << "hs.pattern()" << hs.pattern() << endl;
    this->add(hs);
  }
  if(dbThis){
    cout << "after adding:" << endl;
    for(std::map< DalitzCoordSet, DalitzHistogram>::const_iterator it =
	  this->begin(); it != this->end(); it++){
      cout << "\n" << it->second.pattern() << endl;
    }

  }
  return sc;
}

bool DalitzHistoSet::draw(const std::string& baseName
			  , const std::string& drawOpt
			  , const std::string& format
			  ) const{
  bool sc=true;
  for(map< DalitzCoordSet, DalitzHistogram>::const_iterator 
	it = this->begin();
      it != this->end();
      it++){
    sc &= it->second.draw(baseName, drawOpt, format);
  }
  return sc;
}

bool DalitzHistoSet::drawWithFit(const DalitzHistoSet& fit
				 , const std::string& baseName // =""
				 , const std::string& format // ="eps"
				 , const std::string& fitDrawOpt // ="HIST C SAME"
				 ) const{
  bool sc=true;
  for(map< DalitzCoordSet, DalitzHistogram>::const_iterator 
	it = this->begin();
      it != this->end();
      it++){
    map< DalitzCoordSet, DalitzHistogram >::const_iterator jt 
      = fit.find(it->first);
    if(jt == fit.end()) continue;
    sc &= it->second.drawWithFit(jt->second, baseName, format, fitDrawOpt);
  } 
  return sc;
}

bool DalitzHistoSet::drawWithFitAndEachAmps(
				   DalitzHistoSet& data
				 , DalitzHistoSet& fit
				 , std::vector<DalitzHistoSet>& amps
				 , const std::string& baseName // =""
           , const std::string& // format // ="eps"
           , const std::string& // fitDrawOpt // ="HIST C SAME"
           ) const{
  bool sc=true;
  std::vector<DalitzHistoSet>::iterator amps_it;

  for(map< DalitzCoordSet, DalitzHistogram>::iterator
	it = data.begin();
      it != data.end();
      it++){

	int counter = 0;
    TCanvas can;

    map< DalitzCoordSet, DalitzHistogram >::iterator jt
      = fit.find(it->first);
    if(jt == fit.end()) continue;
    MINT::counted_ptr<TH1> dataHisto = (it->second).getHisto();
    std::cout << it->first << endl;

    TLegend* leg = new TLegend(0.5 //xmin
       				,0.5	//y-min
       				,0.98	//x-max
       			        ,0.935
				,""); //y-max //coordinates are fractions

    leg->SetTextSize(0.05);
    if ((it->first).name() == "sij(2,3,4)" ||
    	(it->first).name() == "sij(1,3,4)" ||
    	(it->first).name() == "sij(1,2,4)" ||
    	(it->first).name() == "sij(1,2,3)" )
    {
    	leg->SetX1(0.1);
    	leg->SetX2(0.4);
    }

	(*dataHisto).Draw("E1");
	leg->AddEntry(&(*dataHisto),"data","lep");  // "l" means line

	MINT::counted_ptr<TH1> FitHisto = jt->second.getHisto();
	(*FitHisto).SetLineColor(1+counter);
	(*FitHisto).Draw("HIST C SAME");
	leg->AddEntry(&(*FitHisto),"Fit","l");  // "l" means line

	// suppress warning double NEntries = (*FitHisto).GetEntries();
  (*FitHisto).GetEntries();
	// suppress warning double NormFactor = (*FitHisto).GetNormFactor();
  (*FitHisto).GetNormFactor();

    MINT::counted_ptr<TH1> histo;

    for (amps_it = amps.begin(); amps_it < amps.end(); amps_it++)
    {
	  map< DalitzCoordSet, DalitzHistogram >::iterator it2
	      = (*amps_it).find(it->first);

	  counter++;
	  histo = it2->second.getHisto();

	  if (counter == 9) counter++; //Remove white colour
	  	  (*histo).SetLineColor(1+counter);

		 const char* title = (*histo).GetTitle();
		 leg->AddEntry(&(*histo),title,"l");  // "l" means line

		 (*histo).Draw("HIST C SAME");
		 // suppress warning double NormFactorAmp = 
     (*histo).GetNormFactor();
    }

    std::string SaveName = it->first.name();
    TString SaveFull = baseName+SaveName+".pdf";
    SaveFull.ReplaceAll("(","");
    SaveFull.ReplaceAll(")","");
    SaveFull.ReplaceAll(",","_");
	leg->SetFillColor(4000);
	leg->SetShadowColor(4000);
	leg->Draw();
	can.Print(SaveFull);
  }
  return sc;
}



bool DalitzHistoSet::drawWithFitNorm(const DalitzHistoSet& fit
				     , const std::string& baseName // =""
				     , const std::string& format // ="eps"
				     , const std::string& fitDrawOpt // ="HIST C SAME"
				     ) const{
  DalitzHistoSet me(*this);
  DalitzHistoSet you(fit);
  if(0 != me.integral())  me  /= me.integral();
  if(0 != you.integral()) you /= you.integral();
  return me.drawWithFit(you, baseName, format, fitDrawOpt);
}
double DalitzHistoSet::integral() const{
  if(this->empty()) return 0;
  return this->begin()->second.integral(); // should all be the same.
}
// operators 
DalitzHistoSet& DalitzHistoSet::operator*=(double sf){
  this->scale(sf);
  return *this;
}
DalitzHistoSet DalitzHistoSet::operator*(double sf) const{
  DalitzHistoSet newSet(*this);
  newSet *= sf;
  return newSet;
}
DalitzHistoSet operator*(double sf, const DalitzHistoSet& dhs){
  DalitzHistoSet newSet(dhs);
  newSet *= sf;
  return newSet;
}

DalitzHistoSet& DalitzHistoSet::operator/=(double sf){
  this->scale(1./sf);
  return *this;
}
DalitzHistoSet DalitzHistoSet::operator/(double sf) const{
  DalitzHistoSet newSet(*this);
  newSet /= sf;
  return newSet;
}

DalitzHistoSet& DalitzHistoSet::operator+=(const DalitzHistoSet& rhs){
  this->add(rhs);
  return *this;
}
DalitzHistoSet DalitzHistoSet::operator+(const DalitzHistoSet& rhs) const{
  DalitzHistoSet newSet(*this);
  newSet += rhs;
  return newSet;
}
DalitzHistoSet& DalitzHistoSet::operator-=(const DalitzHistoSet& rhs){
  this->add(rhs, -1.0);
  return *this;
}
DalitzHistoSet DalitzHistoSet::operator-(const DalitzHistoSet& rhs) const{
  DalitzHistoSet newSet(*this);
  newSet -= rhs;
  return newSet;
}

DalitzHistoSet& DalitzHistoSet::operator*=(const DalitzHistoSet& rhs){
  this->multiply(rhs);
  return *this;
}
DalitzHistoSet DalitzHistoSet::operator*(const DalitzHistoSet& rhs) const{
  DalitzHistoSet newSet(*this);
  newSet *= rhs;
  return newSet;
}
DalitzHistoSet& DalitzHistoSet::operator/=(const DalitzHistoSet& rhs){
  this->divide(rhs);
  return *this;
}
DalitzHistoSet DalitzHistoSet::operator/(const DalitzHistoSet& rhs) const{
  DalitzHistoSet newSet(*this);
  newSet /= rhs;
  return newSet;
}

//
