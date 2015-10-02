// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT
#include "Mint/DiskResidentEventList.h"
#include "Mint/Utils.h"

#include "TNtupleD.h"
#include "TFile.h"

#include <iostream>
#include <fstream>

using namespace std;
using namespace MINT;

//int DiskResidentEventList::__maxBytes = 100000000; // 100 MB
//int DiskResidentEventList::__maxBytes = 50000000; // 50 MB
int DiskResidentEventList::__maxBytes = 10000000; // 10 MB
//int DiskResidentEventList::__maxBytes = 1000000; // 1 MB
//int DiskResidentEventList::__maxBytes = 1000;  // 1 kB

std::string DiskResidentEventList::cName(){
  return _cName;}

std::string DiskResidentEventList::ntpName(){
  return _ntpName;}

std::string DiskResidentEventList::generateFname(){
  string baseName = "DiskResidentEventList";
  string name = baseName;
  bool exists = true;
  ofstream checkFile;
  int i=0;
  do{
    name = baseName + "_" + anythingToString(i++) + ".root";
    checkFile.open( name.c_str(), ios::in );
    exists = ! checkFile.fail();
    checkFile.close();
  }while(exists);

  return name;
}

DiskResidentEventList::DiskResidentEventList()
  : _fname(generateFname())
  , _opt("UPDATE")
  , _f(0)//new TFile(generateFname().c_str(), "UPDATE"))
  , _ntp(0)
  , _scaleData(1)
  , _cName("DalitzEventList")
  , _ntpName("DalitzEventList")
{
  openFile();
  init();
}

DiskResidentEventList::DiskResidentEventList(const std::string& fname
											, int scale
		    								, const std::string& treeName
		    								, const std::string& opt)
  : _fname(fname)
  , _opt(opt)
  , _f(0)//new TFile(fname.c_str(), opt.c_str()))
  , _ntp(0)
  , _scaleData(scale)
  , _cName(treeName)
  , _ntpName(treeName+"NTP")
{
  bool dbThis=true;
  openFile();
  cout << "DiskResidentEventList created with file " << fname << endl;
  fromFile();
  if(dbThis)cout << " from file done, now init: " << endl;
  init();
  if(dbThis)cout << " finished creation" << endl;
}

DiskResidentEventList::DiskResidentEventList(const std::string& fname
		    								, const std::string& opt)
  : _fname(fname)
  , _opt(opt)
  , _f(0)//new TFile(fname.c_str(), opt.c_str()))
  , _ntp(0)
  , _scaleData(1)
  , _cName("DalitzEventList")
  , _ntpName("DalitzEventList")
{
  bool dbThis=false;
  openFile();
  cout << "DiskResidentEventList created with file " << fname << endl;
  fromFile();
  if(dbThis)cout << " from file done, now init: " << endl;
  init();
  if(dbThis)cout << " finished creation" << endl;
}


DiskResidentEventList::DiskResidentEventList(const IMinimalEventList<DalitzEvent>& otherList)
  : _fname(generateFname())
  , _opt("UPDATE")
  , _f(0)//new TFile(generateFname().c_str(), "UPDATE"))
  , _ntp(0)
  , _scaleData(1)
  , _cName("DalitzEventList")
  , _ntpName("DalitzEventList")
{
  openFile();
  Add(otherList);
  init();
}
/*DiskResidentEventList::DiskResidentEventList(const IMinimalEventList<IDalitzEvent>& otherList)
  : _fname(generateFname())
  , _opt("UPDATE")
  , _f(0)//new TFile(generateFname().c_str(), "UPDATE"))
  , _ntp(0)
  , _scaleData(1)
  , _cName("DalitzEventList")
  , _ntpName("DalitzEventList")
{
  openFile();
  Add(otherList);
  init();
  }*/
DiskResidentEventList::DiskResidentEventList(const IMinimalEventList<DalitzEvent>& otherList
					     , const std::string& newFname
					     , const std::string& opt
					     )
  : _fname(newFname)
  , _opt(opt)
  , _f(0)//new TFile(newFname.c_str(), opt.c_str()))
    //  , _counted_ntp(0)
  , _ntp(0)
  , _scaleData(1)
  , _cName("DalitzEventList")
  , _ntpName("DalitzEventList")
{
  openFile();
  cout << " copy with new filename : " << newFname << endl;
  cout << " file pointer: " << _f << endl;
  fromFile();
  Add(otherList);
  init();
}
/*DiskResidentEventList::DiskResidentEventList(const IMinimalEventList<IDalitzEvent>& otherList
					     , const std::string& newFname
					     , const std::string& opt
					     )
  : _fname(newFname)
  , _opt(opt)
  , _f(0)//new TFile(newFname.c_str(), opt.c_str()))
    //  , _counted_ntp(0)
  , _ntp(0)
  , _scaleData(1)
  , _cName("DalitzEventList")
  , _ntpName("DalitzEventList")
{
  openFile();
  cout << " copy with new filename : " << newFname << endl;
  cout << " file pointer: " << _f << endl;
  fromFile();
  Add(otherList);
  init();
  }*/

/*
DiskResidentEventList::DiskResidentEventList(TNtupleD* ntp)
  : _currentEvent(0)
  , _f(0)
  , _counted_ntp(0)
  , _ntp(ntp)
{
  init();
}
*/

DiskResidentEventList::DiskResidentEventList(const DalitzEventPattern& pat)
  : _fname(generateFname())
  , _opt("UPDATE")
  , _f(0)//new TFile(generateFname().c_str(), "RECREATE"))
  , _ntp(0)
  , _scaleData(1)
  , _cName("DalitzEventList")
  , _ntpName("DalitzEventList")
{
  openFile();
  DalitzEvent evt(pat);
  makeNtp(evt); // evt won't be saved, just to use "makeNtupleVarname" (clumsy)
  init();
}

DiskResidentEventList::DiskResidentEventList(const DalitzEventPattern& pat
					     , const std::string& fname
					     , const std::string& opt
					     )
  : _fname(fname)
  , _opt(opt)
  , _f(0)//new TFile(fname.c_str(), opt.c_str()))
  , _ntp(0)
  , _scaleData(1)
  , _cName("DalitzEventList")
  , _ntpName("DalitzEventList")
{
  openFile();
  fromFile();
  DalitzEvent evt(pat);
  if(0 == _ntp) makeNtp(evt);
  init();
}

DiskResidentEventList::~DiskResidentEventList(){
  this->Close();
}

bool DiskResidentEventList::openFile(){
  bool dbThis=true;
  if(dbThis){
    cout << "Hello from DiskResidentEventList::openFile()" << endl;
    cout << " calling: TFile(" 
	 << _fname << ", " << _opt << ", " << _fname << ")"
	 << endl;
  }
  if(0 == _f) _f = new TFile(_fname.c_str(), _opt.c_str(), _fname.c_str());
  if(dbThis){
    cout << " got _f = " << _f << endl;
    cout << " _f->ls() " << endl;
    _f->ls();
    cout << "?" << endl;
  }
  
  if(0 == _f) return makeNewFile();
  if(_f->IsZombie()) return makeNewFile();
  if(! _f->IsOpen()) return makeNewFile();
  if(! _f->IsWritable()) {
    //return makeNewFile();
    cout << "Warning in DiskResidentEventList: file "
	 << _fname << " not writable!"
	 << endl;
  }

//  if (1 ==1) return makeNewFile();
  return true;
}
bool DiskResidentEventList::fromFile(){
  bool dbThis=true;
  if(dbThis) cout << "DiskResidentEventList::fromFile() called" << endl;
  if(0 == _f) makeNewFile();
  if(_f->IsZombie()){
    cout << "WARNING in DiskResidentEventList::fromFile():"
	 << " current file is zombie - making new one" << endl;
    makeNewFile();
  }
  if(dbThis) cout << " opened file" << endl;
  if(dbThis) _f->ls(); cout << " that's in it" << endl;
  _f->cd();
  if(dbThis){
    cout << " cd'ed to file " << endl;
    cout << " now calling " <<  "_f->Get(" << cName() << ")" << endl;
  }
  _ntp = (TNtupleD*) _f->Get(cName().c_str());
  if(dbThis) cout << " got ntuple : " << _ntp << endl;
  if(0 == _ntp){
    return false;
  }else{
    if(dbThis){
      cout << "Success in DiskResidentEventList::fromFile(): "
	   << " found ntuple: " << _ntp->ClassName() << ", " << cName()
	   << " in file: ";
      _f->Print();
      cout << endl;
    }
  }
  if(dbThis) cout << " now calling init" << endl;
  init();
  if(dbThis) cout << "DiskResidentEventList::fromFile() returning 'true'" 
		  << endl;
  return true;
}

bool DiskResidentEventList::init(){
  if(0 != _ntp) _ntp->SetAutoSave(__maxBytes);
  return (0 != _ntp);
}

unsigned int DiskResidentEventList::size() const{
  if(0 == _ntp) return 0;
  return _ntp->GetEntries()/_scaleData;
}
bool DiskResidentEventList::empty() const{
  return (0 == this->size());
}

bool DiskResidentEventList::makeNewFile(){
  _f = new TFile(generateFname().c_str(), "RECREATE");
  // owned by gDirectory, I don't delete this myself.
  return (0 != _f);
}

bool DiskResidentEventList::makeNtp(const DalitzEvent& evt){
  if(0 == _f){
    cout << "WARNING DiskResidentEventList::makeNtp - no file. "
	 << "That shouldn't happen. I'll have one made." << endl;
    makeNewFile();
  }
  _f->cd();
  _ntp = (TNtupleD*) _f->FindObject(ntpName().c_str());
  if(0 == _ntp){
    _ntp = new TNtupleD(cName().c_str(), ntpName().c_str()
			, evt.makeNtupleVarnames().c_str()
			);
    _ntp->SetDirectory(0);
  }
  if(0 != _ntp){
    _ntp->SetDirectory(_f);
    _ntp->SetAutoSave(__maxBytes);
  }
  return 0 != _ntp;
}
bool DiskResidentEventList::Add(const DalitzEvent& evt){
  if(0 == _f) makeNewFile();
  _f->cd();
  if(0 == _ntp) makeNtp(evt);
  int arraySize = evt.ntupleVarArraySize();
  Double_t *array = new Double_t[arraySize];
  
  bool success = evt.fillNtupleVarArray(array, arraySize);
  if(! success){
    cout << "ERROR in DiskResidentEventList::Add(const DalitzEvent& evt)"
	 << ", call to DalitzEvent::fillNtupleVarArray"
	 << " returned failure for event:\n" << evt
	 << endl;
  }else{
    _f = _ntp->GetCurrentFile(); 
    _f->cd();
    _ntp->Fill(array);
    _f = _ntp->GetCurrentFile(); 
    // above lines: to pick up when TNtuple (TTree) changes
    // files in case the current one is too big. All this
    // rubbish is necessary because of root's crazy way of
    // handling this.
  }
  delete[] array;
  if(! _initialised) init();
  return true;
}

bool DiskResidentEventList::Add(const IDalitzEvent& evt){
  DalitzEvent nevt(&evt);
  return Add(nevt);
}
bool DiskResidentEventList::Add(const IDalitzEvent* evt){
  DalitzEvent nevt(evt);
  return Add(nevt);
}
bool DiskResidentEventList::Add(const counted_ptr<IDalitzEvent>& evt){
  DalitzEvent nevt(evt.get());
  return Add(nevt);
}
bool DiskResidentEventList::Add(const IMinimalEventList<DalitzEvent>& otherList){
  if(0 == otherList.size()) return false;
  for(unsigned int i=0; i < otherList.size(); i++){
    Add(otherList.getEvent(i));
  }
  return true;
}
/*bool DiskResidentEventList::Add(const IMinimalEventList<IDalitzEvent>& otherList){
  if(0 == otherList.size()) return false;
  for(unsigned int i=0; i < otherList.size(); i++){
    Add(otherList.getEvent(i));
  }
  return true;
  }*/
bool DiskResidentEventList::save(){
  if(0 == _ntp) return false;
  if(0 == _f) return false;
  bool success=true;
  _f->cd();
  success &= _ntp->AutoSave();
  return success;
}

bool DiskResidentEventList::Close(){
  bool dbThis=false;
  bool success = save();
  if(dbThis && (! success)){
    cout << "DiskResidentEventList::Close() failure in saving when closing" 
	 << endl;
  }
  if(0 != _f){
    _f->cd();
    if(0 != _ntp){
      _ntp->Write();
      _ntp->Delete();
    }
    _f->Close();
    _f->Delete("all"); // debug june 2011 (leave in for now)
  }
  _f   = 0;
  _ntp = 0;
  return success;
}

//DalitzEvent DiskResidentEventList::operator[](unsigned int i) const{
DalitzEvent DiskResidentEventList::getEvent(unsigned int i) const{
  if(i > size()){
    cout << "FATAL ERROR in DiskResidentEventList::operator()"
	 << " index i=" << i << " out of range " << size()
	 << endl;
    throw "index out of range";
  }
  _ntp->GetEvent(i);
  return DalitzEvent(_ntp);
}

DalitzHistoSet DiskResidentEventList::histoSet() const{
  DalitzHistoSet hs;
  for(unsigned int i=0; i< this->size(); i++){
    hs.addEvent(this->getEvent(i));
  }
  return hs;
}

DalitzHistoSet DiskResidentEventList::weightedHistoSet() const{
  // mainly for diagnostics
  DalitzHistoSet hs;
  for(unsigned int i=0; i< this->size(); i++){
    DalitzEvent evt(this->getEvent(i));
    hs.addEvent(evt, evt.getWeight());
  }
  return hs;
}
DalitzHistoSet DiskResidentEventList::reWeightedHistoSet(IReturnRealForEvent<IDalitzEvent>* w) const{
  // mainly for diagnostics
  DalitzHistoSet hs;
  if(0 == w) return hs;
  for(unsigned int i=0; i< this->size(); i++){
    DalitzEvent evt( this->getEvent(i));
    hs.addEvent(evt, w->RealVal(evt));
  }
  return hs;
}

DalitzHistoSet DiskResidentEventList::weighedReWeightedHistoSet(IReturnRealForEvent<IDalitzEvent>* w) const{
  // mainly for diagnostics
  DalitzHistoSet hs;
  if(0 == w) return hs;
  for(unsigned int i=0; i< this->size(); i++){
    DalitzEvent evt(this->getEvent(i));
    hs.addEvent(evt, w->RealVal(evt) * (evt.getWeight()));
  }
  return hs;
}

bool DiskResidentEventList::makePlots(const std::string& filename){
  histoSet().save(filename);
  return true;
}

//

