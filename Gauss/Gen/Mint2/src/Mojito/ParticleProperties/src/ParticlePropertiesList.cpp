// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT
#include "Mint/ParticlePropertiesList.h"
#include "Mint/NamedParameter.h"

#include <string>
#include <fstream>
//#include <sstream>
#include <cstdio>
#include <stdlib.h>

using namespace std;
using namespace MINT;

ParticlePropertiesList* ParticlePropertiesList::ptr=0;
std::string ParticlePropertiesList::_MintDalitzSpecialParticles="MintDalitzSpecialParticles.cvs";

std::vector<std::string> ParticlePropertiesList::_dirList;


const ParticlePropertiesList* ParticlePropertiesList::getMe(){
  if(! ptr){
    ptr = new ParticlePropertiesList();
  }
  if(0 == ptr){
    cout << "ERROR in ParticlceProperties::getMe"
	 << " Couldn't get ParticlePropertiesList (i.e. myself)"
	 << "\n\t This is a serious problem. Will crash."
	 << endl;
    throw "No ParticlePropertiesList";
  }
  return ptr;
}

void ParticlePropertiesList::printCannotFindWarning(const std::string& where
						    , const std::string& what){
  cout << "WARNING in ParticlceProperties::" << where
       << " Got ParticlePropertiesList, but couldn't find "
       << what
       << endl;
}
void ParticlePropertiesList::printCannotFindWarning(const std::string& where
						    , int what){
  cout << "WARNING in ParticlceProperties::" << where
       << " Got ParticlePropertiesList, but couldn't find "
       << what
       << endl;
}

double ParticlePropertiesList::mass(const std::string& name) {
  const ParticleProperties* props = getMe()->get(name);
  if(0 == props){
    printCannotFindWarning("mass", name);
    return -9999;
  }    
  return props->mass();
}
double ParticlePropertiesList::mass(int pid) {
  const ParticleProperties* props = getMe()->get(pid);
  if(0 == props){
    printCannotFindWarning("mass", pid);
    return -9999;
  }    
  return props->mass();
}
double ParticlePropertiesList::width(const std::string& name) {
  const ParticleProperties* props = getMe()->get(name);
  if(0 == props){
    printCannotFindWarning("width", name);
    return -9999;
  }    
  return props->width();
}
double ParticlePropertiesList::width(int pid) {
  const ParticleProperties* props = getMe()->get(pid);
  if(0 == props){
    printCannotFindWarning("width", pid);
    return -9999;
  }    
  return props->width();
}


const std::vector<std::string>& ParticlePropertiesList::dirList(){
  if(_dirList.empty()) fillDirList();
  return _dirList;
}

void ParticlePropertiesList::fillDirList(){
  _dirList.clear();
  NamedParameter<std::string> userDir("ParticlePropertiesList::ParticlePropertiesDir"
					, (std::string) "");
  for(int i=0; i < userDir.size(); i++){
    if("" != userDir.getVal(i)){
      _dirList.push_back(userDir.getVal(i) + "/");
      //      cout << "just pushed back " << userDir.getVal() << endl;
    }
  }

//   Get directory to MINTROOT


  std::string MintRoot(".");
  char * Mintevn(0);
  Mintevn = getenv ("MINTROOT");
  if (NULL != Mintevn){
    MintRoot = Mintevn;
  }

  std::string DecFilesRoot("UNKNOWN");
  char * DecFiles;
  DecFiles = getenv ("DECFILESROOT");
  if (NULL != DecFiles){
    DecFilesRoot = DecFiles;
  }
  
  _dirList.push_back("");
  _dirList.push_back("./");
  _dirList.push_back(DecFilesRoot+"/MintData/");
  _dirList.push_back(MintRoot+"/src/Mojito/ParticleProperties/src/");
  _dirList.push_back("../../../Mojito/ParticleProperties/src/");
  _dirList.push_back("../../../../src/Mojito/ParticleProperties/src/");
  _dirList.push_back("../../ParticleProperties/src/");
  _dirList.push_back("../ParticleProperties/src/");
  _dirList.push_back("./ParticleProperties/src/");
  _dirList.push_back("../");
  _dirList.push_back("../../");
}


FILE* ParticlePropertiesList::findFiles(){
  static int i=0;
  //  cout << " i = " << i << endl;
  FILE* f(0);
  if(0 == i){
    f = findThisFile(_fname);
    if(0 == f) findThisFile("mass_width.csv"); // making it backward compatible with old typo
    if(0 == f)i++;
  }
  if(1 == i){
    f = findThisFile(_MintDalitzSpecialParticles);
    if(0 == f) i++;
  }

  i = (i+1)%3;

  return f;
}
FILE* ParticlePropertiesList::findThisFile(const std::string& fname){
  bool verbose=true;
  for(std::vector<std::string>::const_iterator it = dirList().begin();
      it != dirList().end();
      it++){

	std::string tryThisDir = (*it);
    std::string fullFname = tryThisDir + fname;
    if(verbose)cout << "INFO in ParticlePropertiesList: trying: " << fullFname << endl;
    FILE* f=fopen(fullFname.c_str(), "r");
    if(0 != f){
      cout << "INFO in ParticlePropertiesList: Using this file: " 
	   << fullFname << endl;
      return f;
    }

  }
  cout << "ERROR in ParticlePropertiesList::findFile()"
       << "\n  > having trouble finding file"
       << "\n  > with name " << fname
       << "\n  > in any directory I could guess."
       << "\n  > This is hardcoded in ParticleProperties.C."
       << endl;
  return (FILE*) 0;
}

ParticlePropertiesList::ParticlePropertiesList(std::string fname_in){
  _fname=fname_in;

  if(! readFiles()){
    std::cout << "ERROR! in ParticleProperties constructor:"
	      << " Problem reading file " 
	      << _fname << std::endl;
  }
  
}

bool ParticlePropertiesList::readFiles(){
  bool shoutIfTwice=true;
  int counter=0;
  while(FILE* f = findFiles()){
    cout << "reading " << counter << "th file" << endl;
    if(0==f) return false;
    
    static const int lineLength=800;
    char line[lineLength]={'\0'};
    
    while(fgets(line, lineLength, f)){
      ParticleProperties P(line);
      if(! P.isValid()) continue;
      
      theList.push_back(P);
      if(P.hasDistinctAnti()){
	P.antiThis();
	theList.push_back(P);
      }
    }
    fclose(f);
  }

  // make maps for fast particle finding:
  for(std::list<ParticleProperties>::iterator it= theList.begin();
      it != theList.end(); it++){
    byName[it->name()] = it;
    if(shoutIfTwice && it->pdg_id() != 0){
      map<int, list<ParticleProperties>::iterator >::iterator found = byID.find(it->pdg_id());
      if(found != byID.end()){
	cout << "WARNING in  ParticlePropertiesList::readFiles "
	     << "pdg_id " << it->pdg_id() << " used twice, here: " 
	     << (found->second)->name()
	     << ", and here:\n " << it->name()
	     << endl;
	cout << " Important: Note that this is probably not be a MintDalitz error - "
	     << "\n some particles (the 'non-established' ones) have the same PDG code."
	     << "\n To use those realiably, you'll need to put your own copy of the"
	     << "\n mass_width.cvs file, put it into your run-directory, and edit the file"
	     << "\n so that your particle has a unique PID code."
	     << "\n -----------------------"
	     << endl;
      }
    }
    byID[it->pdg_id()] = it;
  }
  
  return true;
}

void ParticlePropertiesList::print(std::ostream& out) const{
  for(std::list<ParticleProperties>::const_iterator it= theList.begin();
      it != theList.end(); it++){
    it->print(out);
    out << "\n";
  }
}

const ParticleProperties* ParticlePropertiesList::get(const std::string& name) const{
     std::map<std::string, std::list<ParticleProperties>::iterator >::const_iterator it 
       = byName.find(name);
     if(it == byName.end()) return 0;
     
     return &(*(it->second));
}
const ParticleProperties* ParticlePropertiesList::get(int id) const{
     std::map<int, std::list<ParticleProperties>::iterator >::const_iterator it 
       = byID.find(id);
     if(it == byID.end()) return 0;
     
     return &(*(it->second));
}

std::ostream& operator<<(std::ostream& out, const ParticlePropertiesList& ppl){
  ppl.print(out);
  return out;
}

//
