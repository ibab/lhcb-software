// author: Philippe d'Argent (p.dargent@cern.ch)
#include "Mint/ResonanceProperties.h"
#include "Mint/CLHEPPhysicalConstants.h"
#include "Mint/CLHEPSystemOfUnits.h"
#include "Mint/NamedParameter.h"
#include "Mint/Utils.h"
#include "Mint/FitParameter.h"
#include "Mint/ParticlePropertiesList.h"

#include <cstdio>
#include <cstring>
#include <algorithm>

using namespace std;
using namespace MINT;

ResonanceProperties::ResonanceProperties(int pid): 
    _pid(pid)
    , _fitMass("mass_"+MINT::anythingToString(abs(pid)))
    , _fitWidth("width_"+MINT::anythingToString(abs(pid)))
    , _fitRadius("radius_"+MINT::anythingToString(abs(pid)))
    , _fitAlpha("alpha_"+MINT::anythingToString(abs(pid)))
{ 

        //fix to PDG value if not explicitly initiallised 
        const ParticlePropertiesList* PPL = ParticlePropertiesList::getMe();
        if(_fitMass.iFixInit() && _fitMass== 0) _fitMass = PPL->mass(pid);
        if(_fitWidth.iFixInit() && _fitWidth== 0) _fitWidth = PPL->width(pid);    
        if(_fitRadius.iFixInit() && _fitRadius== 0) _fitRadius = PPL->get(pid)->radius();   
        //std:: cout << _fitMass << endl;
        //std:: cout << _fitWidth << endl;
}

ResonanceProperties::ResonanceProperties(std::string name): 
    _pid(-1)
    , _fitMass("mass_"+name)
    , _fitWidth("width_"+name)
    , _fitRadius("radius_"+name)
    , _fitAlpha("alpha_"+name)
{ 
    
    //fix to PDG value if not explicitly initiallised 
    const ParticlePropertiesList* PPL = ParticlePropertiesList::getMe();
    _pid=abs(PPL->get(name)->pdg_id());
    if(_fitMass.iFixInit() && _fitMass== 0) _fitMass = PPL->get(name)->mass();
    if(_fitWidth.iFixInit() && _fitWidth== 0) _fitWidth = PPL->get(name)->width();    
    if(_fitRadius.iFixInit() && _fitRadius== 0) _fitRadius = PPL->get(name)->radius();   
    if(_fitAlpha.iFixInit() && _fitAlpha== 0) _fitAlpha = 1.;   
    //std:: cout << _fitMass << endl;
    //std:: cout << _fitWidth << endl;
    //std:: cout << _pid << endl;

}




//
