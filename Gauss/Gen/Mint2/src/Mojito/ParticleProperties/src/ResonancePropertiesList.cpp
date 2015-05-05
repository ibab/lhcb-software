// author: Philippe d'Argent (p.dargent@çern.ch)
#include "Mint/ResonancePropertiesList.h"
#include "Mint/ParticlePropertiesList.h"
#include "Mint/NamedParameter.h"
#include "Mint/CLHEPSystemOfUnits.h"

#include <string>
#include <fstream>
//#include <sstream>
#include <cstdio>
#include <stdlib.h>

using namespace std;
using namespace MINT;

ResonancePropertiesList* ResonancePropertiesList::ptr=0;


ResonancePropertiesList* ResonancePropertiesList::getMe(){
  if(! ptr){
    ptr = new ResonancePropertiesList();
  }
  if(0 == ptr){
    cout << "ERROR in ResonanceProperties::getMe"
	 << " Couldn't get ResonancePropertiesList (i.e. myself)"
	 << "\n\t This is a serious problem. Will crash."
	 << endl;
    throw "No ResonancePropertiesList";
  }
  return ptr;
}

/*
double ParticlePropertiesList::mass(int pid) {
  const ResonanceProperties* props = getMe()->get(pid);
  if(0 == props){
    printCannotFindWarning("mass", pid);
    return -9999;
  }    
  return props->mass();
}


double ParticlePropertiesList::width(int pid) {
  const ParticleProperties* props = getMe()->get(pid);
  if(0 == props){
    printCannotFindWarning("width", pid);
    return -9999;
  }    
  return props->width();
}
*/


ResonancePropertiesList::ResonancePropertiesList(): _radius("BW_radius") //, _rp(10323)
{   
    //fix BW radius to default value if not explicitly initiallised 
    if(_radius.iFixInit() && _radius== 0) _radius = 1.5/GeV;
    /*
    const ParticlePropertiesList* PPL = ParticlePropertiesList::getMe();
    const std::list<ParticleProperties> theParticleList = PPL->getList();
    for(std::list<ParticleProperties>::const_iterator it= theParticleList.begin();it != theParticleList.end(); it++){
        if(it->pdg_id>0)
        ResonanceProperties* rp= new ResonanceProperties(it->pdg_id());
        theList.push_back(rp);
    }
    */
}

void ResonancePropertiesList::AddToList(ResonanceProperties* rp){
    //theList.push_back(rp);
    byID[rp->pid()] = rp;
}

const ResonanceProperties* ResonancePropertiesList::get(int id) const{
    std::map<int, ResonanceProperties*>::const_iterator it = byID.find(id);
    if(it == byID.end()) return 0;
    return it->second;
}



/*
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
*/


//
