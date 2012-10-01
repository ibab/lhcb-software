// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:55 GMT
#include "Mint/MinuitParameterSet.h"

#include "TFile.h"
#include "TNtupleD.h"
#include "Mint/Utils.h"

#include <algorithm>
#include <iostream>

#include "TDirectory.h"

using namespace std;
using namespace MINT;

//const char MinuitParameterSet::prtNameChars[] = { '+', '-', '*', '>', ',', '(', ')', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '\0' };
//const char MinuitParameterSet::ntpNameChars[] = { 'p', 'm', 's', '_', '_', '_', '_', 'a', 'b', 'c', 'c', 'e', 'f', 'g', 'h', 'i', 'j', '\0' };

const char MinuitParameterSet::prtNameChars[] = { '+', '-', '*', '>', ',', '(', ')', '[', ']', '\0'};
const char MinuitParameterSet::ntpNameChars[] = { '#', '~', 's', '_', '_', '_', '_', '_', '_', '\0'};

MinuitParameterSet* MinuitParameterSet::_defaultMinuitParameterSet=0;
MinuitParameterSet* MinuitParameterSet::getDefaultSet(){
  if(0 == _defaultMinuitParameterSet){
    _defaultMinuitParameterSet = new MinuitParameterSet();
  }
  return _defaultMinuitParameterSet;
}

MinuitParameterSet::MinuitParameterSet(){}
MinuitParameterSet::MinuitParameterSet(const MinuitParameterSet& other)
  : _parPtrList(other._parPtrList)
{}

MinuitParameterSet MinuitParameterSet::getFloating(){
  MinuitParameterSet floating;
  for(unsigned int i=0; i< size(); i++){
    if(0 != getParPtr(i)){
      floating.add(getParPtr(i));
    }
  }
  return floating;
}

bool MinuitParameterSet::addToEnd(IMinuitParameter* parPtr){
  bool dbThis=false;
  bool success=true;
  if(0 == parPtr) return false;
  if(dbThis){
    cout << "MinuitParameterSet::add "
	 << " adding parPtr " << parPtr->name() << endl;
  }
  _parPtrList.push_back(parPtr);
  success &= parPtr->setParSet(this);
  success &= parPtr->setParSetIndex(_parPtrList.size()-1);
  return success;
}
bool MinuitParameterSet::add(IMinuitParameter* parPtr){
  return addToEnd(parPtr);
}

bool MinuitParameterSet::unregister(IMinuitParameter* parPtr){

  if(_parPtrList.end() == find(_parPtrList.begin(), _parPtrList.end(), parPtr)){
    cout << "WARNING in MinuitParameterSet::unregister"
	 << " parPtr you want to unregister is not part of this list!"
	 << endl;
    return false;
  }

  parPtr->setParSetIndex(-9999);
  _parPtrList.erase(remove(_parPtrList.begin(), _parPtrList.end(), parPtr)
		    , _parPtrList.end()
		    );
  setAllIndices();
  return true;
}

bool MinuitParameterSet::setAllIndices(){
  bool success=true;
  for(unsigned int i=0; i < _parPtrList.size(); i++){
    success &= _parPtrList[i]->setParSetIndex(i);
  }
  return success;
}

unsigned int MinuitParameterSet::size()const{
  return _parPtrList.size();
}

IMinuitParameter* MinuitParameterSet::getParPtr(unsigned int i){
  if(i >= _parPtrList.size()) return 0;
  return _parPtrList[i];
}

const IMinuitParameter* MinuitParameterSet::getParPtr(unsigned int i) const{
  if(i >= _parPtrList.size()) return 0;
  return _parPtrList[i];
}

void MinuitParameterSet::deleteListAndObjects(){
  for(std::vector<IMinuitParameter*>::iterator it = _parPtrList.begin();
      it != _parPtrList.end(); it++){
    delete (*it);
  }
  _parPtrList.clear();
}

void MinuitParameterSet::deleteListKeepObjects(){
  _parPtrList.clear();
}

TNtupleD* MinuitParameterSet::makeNewNtpForOwner(TFile*& ntpFile) const{
  bool dbThis=false;
  if(dbThis){
    cout << "Hello from MinuitParameterSet::makeNewNtpForOwner" << endl;
  }
  gDirectory->cd();
  if(0 == ntpFile) ntpFile = new TFile("MinuitParameterSetFile.root"
				       , "RECREATE");
  ntpFile->cd();
  TNtupleD* ntp = new TNtupleD("MinuitParameterSetNtp"
			       , "MinuitParameterSetNtp"
			       , ntpNames().c_str());
  return ntp;
}

std::string MinuitParameterSet::prtToNtpName(const std::string& s_in){
  std::string s(s_in);
  for(unsigned int i=1; i < s.size(); i++){
    // cout << "... " << s[i-1] << s[i];
    if(s[i-1] == '-' && s[i] == '>'){
      s[i-1] = 't'; s[i] = 'o';
    }
  }

  for(int i=0; prtNameChars[i] != '\0'; i++){
    replace(s.begin(), s.end(), prtNameChars[i], ntpNameChars[i]);
  }
  return s;
}

std::string MinuitParameterSet::ntpNames() const{
  bool dbThis=false;
  std::string str="";
  int n=0;
  for(unsigned int i=0; i < size(); i++){
    if(0 == getParPtr(i)) continue;
    if(0 != getParPtr(i)->iFixInit()) continue;
    std::string name = "p" + anythingToString(i) + "_" 
      + prtToNtpName(getParPtr(i)->name());
    str += (name + "_mean" + ":"); n++;
    str += (name + "_init" + ":"); n++;
    str += (name + "_err:");       n++;
    str += (name + "_pull");       n++;
    if(i != size()-1) str += ":";
  }
  if(dbThis){
    cout << "MinuitParameterSet::ntpNames():"
	 << " made " << n << " names " << str << endl;
    cout << " this becomes: prtToNtpName(str) " 
	 << prtToNtpName(str) << endl;
  }
  //return str;
  return prtToNtpName(str);
}
void MinuitParameterSet::fillNtp(TFile*& ntpFile, TNtupleD*& ntp) const{
  bool dbThis=false;
  if(dbThis){
    cout << "hello from MinuitParameterSet::fillNtp " << endl;
    cout << " you called me with: " << ntp << endl;
  }
  
  if(0 == ntp) ntp = makeNewNtpForOwner(ntpFile);
  if(dbThis) cout << "got an ntuple ptr: " << ntp << endl;
  Double_t* array = new Double_t[size()*4];
  if(dbThis) cout << "made array" << endl;
  int n=0;
  for(unsigned int i=0; i < size(); i++){
    if(0 == getParPtr(i)) continue;
    if(0 != getParPtr(i)->iFixInit()) continue;
    array[n++] = getParPtr(i)->mean();
    array[n++] = getParPtr(i)->meanInit();
    array[n++] = getParPtr(i)->err();
    Double_t pull=-9999.0;
    if(getParPtr(i)->err() > 0){
      pull = (getParPtr(i)->mean() - getParPtr(i)->meanInit())/getParPtr(i)->err();
    }
    array[n++] = pull;
  }
  if(dbThis){
    cout << "checking how ntuple behaves with GetEntries()" << endl;
    cout << " ntp->GetEntries() = " << ntp->GetEntries() << endl;
  }
  if(dbThis)cout << "filled the array, now putting this into the ntp" << endl;
  ntp->Fill(array);
  if(dbThis)cout << "filled into the ntuple, now deleting array" << endl;
  delete[] array;
  if(dbThis)cout << "done. returning" << endl;
  return;
}


void MinuitParameterSet::print(std::ostream& os) const{
  for(unsigned int i=0; i < size(); i++){
    if(0 == getParPtr(i)) continue;
    os << '\n';
    getParPtr(i)->print(os);
  }
}
void MinuitParameterSet::printVariable(std::ostream& os) const{
  for(unsigned int i=0; i < size(); i++){
    if(0 == getParPtr(i)) continue;
    if(getParPtr(i)->iFixInit() == 1) continue;
    os << '\n';
    getParPtr(i)->print(os);
  }
}
void MinuitParameterSet::printResultVsInput(std::ostream& os) const{
  os << "\n ======== MinuitParameterSet::(FitResult - StartValue)/error ========\n";
  for(unsigned int i=0; i < this->size(); i++){
    if(0 == getParPtr(i)) continue;
    if(getParPtr(i)->iFixInit() == 1) continue;
    os << '\n';
    getParPtr(i)->printResultVsInput(os);
  }
  os << "\n ===================================================================="
     << endl;
}

//
