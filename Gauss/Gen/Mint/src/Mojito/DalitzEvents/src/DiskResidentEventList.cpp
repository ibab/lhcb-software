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
  : _currentEvent(0)
  , _rEvent(0)
  , _fname(generateFname())
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
  : _currentEvent(0)
  , _rEvent(0)
  , _fname(fname)
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
  : _currentEvent(0)
  , _rEvent(0)
  , _fname(fname)
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


DiskResidentEventList::DiskResidentEventList(const IDalitzEventList& otherList)
  : _currentEvent(0)
  , _rEvent(0)
  , _fname(generateFname())
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
DiskResidentEventList::DiskResidentEventList(const IDalitzEventList& otherList
					     , const std::string& newFname
					     , const std::string& opt
					     )
  : _currentEvent(0)
  , _rEvent(0)
  , _fname(newFname)
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
  : _currentEvent(new DalitzEvent(pat))
  , _rEvent(0)
  , _fname(generateFname())
  , _opt("UPDATE")
  , _f(0)//new TFile(generateFname().c_str(), "RECREATE"))
  , _ntp(0)
  , _scaleData(1)
  , _cName("DalitzEventList")
  , _ntpName("DalitzEventList")
{
  openFile();
  makeNtp(*_currentEvent);
  init();
}

DiskResidentEventList::DiskResidentEventList(const DalitzEventPattern& pat
					     , const std::string& fname
					     , const std::string& opt
					     )
  : _currentEvent(new DalitzEvent(pat))
  , _rEvent(0)
  , _fname(fname)
  , _opt(opt)
  , _f(0)//new TFile(fname.c_str(), opt.c_str()))
  , _ntp(0)
  , _scaleData(1)
  , _cName("DalitzEventList")
  , _ntpName("DalitzEventList")
{
  openFile();
  fromFile();
  if(0 == _ntp) makeNtp(*_currentEvent);
  init();
}

DiskResidentEventList::~DiskResidentEventList(){
  this->Close();
}

bool DiskResidentEventList::openFile(){
  if(0 == _f) _f = new TFile(_fname.c_str(), _opt.c_str(), _fname.c_str());
  if(0 == _f) return makeNewFile();
  if(_f->IsZombie()) return makeNewFile();
  if(! _f->IsOpen()) return makeNewFile();
  if(! _f->IsWritable()) return makeNewFile();
//  if (1 ==1) return makeNewFile();
  return true;
}
bool DiskResidentEventList::fromFile(){
  bool dbThis=false;
  if(dbThis) cout << "DiskResidentEventList::fromFile() called" << endl;
  if(0 == _f) makeNewFile();
  if(_f->IsZombie()){
    cout << "WARNING in DiskResidentEventList::fromFile():"
	 << " current file is zombie - making new one" << endl;
    makeNewFile();
  }
  if(dbThis) cout << " opened file" << endl;
  _f->cd();
  if(dbThis) cout << " cd'ed to file " << endl;
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
  return (_initialised = Start_noInit() && 0 != _ntp);
}

unsigned int DiskResidentEventList::size() const{
  if(0 == _ntp) return 0;
  return _ntp->GetEntries()/_scaleData;
}
bool DiskResidentEventList::empty() const{
  return (0 == this->size());
}

bool DiskResidentEventList::Start(){
  if(! _initialised) init();
  return Start_noInit();
}

bool DiskResidentEventList::Start_noInit(){
  if(this->empty()) return false;
  currentPosition = nextPosition = 0;
  if(0 != _ntp){
    _ntp->GetEvent(currentPosition);
    _currentEvent = counted_ptr<DalitzEvent>(new DalitzEvent(_ntp));
  }else{
    return false;
  }
  // the 1st call to Next() gives 1st event.
  //else return false;
  return true;
}

bool DiskResidentEventList::Next(){ // 1st call to Next gives first event!
  // a bit complicated, but insures save deleting
  if(this->empty()) return false;
  if(! _initialised) init();
  
  currentPosition = nextPosition;
  if(currentPosition == size()){
    Start();
    return false;
  }
  _ntp->GetEvent(currentPosition);
  _currentEvent = counted_ptr<DalitzEvent>(new DalitzEvent(_ntp));
  nextPosition++;
  return true;
}

bool DiskResidentEventList::curryCheck(){
  if(this->empty())return false;
  if(! _initialised) init();
  if(currentPosition >= size()) Start();
  if(currentPosition >= size()) return false;
  return true;
}

const IDalitzEvent* DiskResidentEventList::getEvent() const{
  if(this->empty()) return 0;
  return (IDalitzEvent*) _currentEvent.get();
}
IDalitzEvent* DiskResidentEventList::getEvent(){
  if(! curryCheck()) return 0;
  return (IDalitzEvent*)  _currentEvent.get();
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

bool DiskResidentEventList::Add(const IDalitzEvent* evt){
  DalitzEvent nevt(evt);
  return Add(nevt);
}
bool DiskResidentEventList::Add(const counted_ptr<IDalitzEvent>& evt){
  DalitzEvent nevt(evt.get());
  return Add(nevt);
}
bool DiskResidentEventList::Add(const IDalitzEventList& otherList){
  if(0 == otherList.size()) return false;
  for(unsigned int i=0; i < otherList.size(); i++){
    Add(otherList.getREvent(i));
  }
  return true;
}
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

counted_ptr<IDalitzEvent> DiskResidentEventList::getEventCopy(unsigned int i) const{
  if(i > size()){
    cout << "FATAL ERROR in DiskResidentEventList::getEventCopy(unsigned int i)"
	 << " index i=" << i << " out of range " << size()
	 << endl;
    throw "index out of range";
  }
  _ntp->GetEvent(i);
  return counted_ptr<IDalitzEvent>(new DalitzEvent(_ntp));
}
const IDalitzEvent* DiskResidentEventList::getREvent(unsigned int i) const{
  if(i > size()){
    cout << "FATAL ERROR in DiskResidentEventList::getREvent(unsigned int i)"
	 << " index i=" << i << " out of range " << size()
	 << endl;
    throw "index out of range";
  }
  _ntp->GetEvent(i);
  _rEvent = counted_ptr<DalitzEvent>(new DalitzEvent(_ntp));
  return _rEvent.get();
}
DalitzEvent DiskResidentEventList::operator()(unsigned int i){
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
    hs.addEvent(this->getREvent(i));
  }
  return hs;
}

DalitzHistoSet DiskResidentEventList::weightedHistoSet() const{
  // mainly for diagnostics
  DalitzHistoSet hs;
  for(unsigned int i=0; i< this->size(); i++){
    hs.addEvent(this->getREvent(i), this->getREvent(i)->getWeight());
  }
  return hs;
}
DalitzHistoSet DiskResidentEventList::reWeightedHistoSet(IGetDalitzEvent* w) const{
  // mainly for diagnostics
  DalitzHistoSet hs;
  if(0 == w) return hs;
  for(unsigned int i=0; i< this->size(); i++){
    w->setEvent(this->getEventCopy(i).get());
    hs.addEvent(this->getREvent(i), w->RealVal());
    w->resetEventRecord();
  }
  return hs;
}

DalitzHistoSet DiskResidentEventList::weighedReWeightedHistoSet(IGetDalitzEvent* w) const{
  // mainly for diagnostics
  DalitzHistoSet hs;
  if(0 == w) return hs;
  for(unsigned int i=0; i< this->size(); i++){
    w->setEvent(this->getEventCopy(i).get());
    hs.addEvent(this->getREvent(i), w->RealVal() * (this->getREvent(i)->getWeight()));
    w->resetEventRecord();
  }
  return hs;
}

bool DiskResidentEventList::makePlots(const std::string& filename){
  histoSet().save(filename);
  return true;
}

//

