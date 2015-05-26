#ifndef RESONANCEPROPERTIESLIST_HH
#define RESONANCEPROPERTIESLIST_HH
// author: Philippe d'Argent (p.dargent@cern.ch)

#include <string>
#include <list>
#include <vector>
#include <map>
#include <iostream>

#include "Mint/ParticleProperties.h"
#include "Mint/ResonanceProperties.h"
#include "Mint/FitParameter.h"

class ResonancePropertiesList{ // a singleton - ensures everyone
  // uses the same particle properties, which is initialised 
  // only once.  Saves space and time
  // and (maybe) increases consistency.
  // To get access do:
  // ParticlePropertiesList* PPL = ParticlePropertiesList::getMe();
  // and then:
  // ParticleProperties* pionProps = PPL->get("pion");
  // ParticleProperties* pionProps = PPL->get(221);
  // 

  static ResonancePropertiesList* ptr;
  ResonancePropertiesList();

 protected:
  
  MINT::FitParameter _radius;
  //ResonanceProperties _rp;  
  std::vector<ResonanceProperties*> theList;
  //std::map<std::string, std::list<ResonanceProperties>::iterator > byName;
  std::map<int, ResonanceProperties* > byID;

 public:
  static ResonancePropertiesList* getMe();
  void AddToList(ResonanceProperties* rp);
    
  //const ResonanceProperties* get(const std::string& name) const;
  const ResonanceProperties* get(int i) const;
  const MINT::FitParameter& radius() const {return _radius;}

  //void print(std::ostream& out=std::cout) const;

    /*
  // fast fuss-free access:
  static double mass(const std::string& name);
  static double mass(int PDG);
  static double width(const std::string& name);
  static double width(int PDG);
  */
};


//std::ostream& operator<<(std::ostream& out, const ParticlePropertiesList& ppl);

#endif
//
