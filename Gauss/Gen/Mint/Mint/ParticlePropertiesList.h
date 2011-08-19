#ifndef PARTICLEPROPERTIESLIST_HH
#define PARTICLEPROPERTIESLIST_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:04 GMT

#include <string>
#include <list>
#include <vector>
#include <map>
#include <iostream>

#include "Mint/ParticleProperties.h"

class ParticlePropertiesList{ // a singleton - ensures everyone
  // uses the same particle properties, which is initialised 
  // only once.  Saves space and time
  // and (maybe) increases consistency.
  // To get access do:
  // ParticlePropertiesList* PPL = ParticlePropertiesList::getMe();
  // and then:
  // ParticleProperties* pionProps = PPL->get("pion");
  // ParticleProperties* pionProps = PPL->get(221);
  // 

  static ParticlePropertiesList* ptr;
 

  ParticlePropertiesList(std::string fname_in="mass_width.cvs");
  static std::string _MintDalitzSpecialParticles;
 protected:
  static std::vector<std::string> _dirList;
  static const std::vector<std::string>& dirList();
  static void fillDirList();

  static void printCannotFindWarning(const std::string& where
				     , const std::string& what);
  static void printCannotFindWarning(const std::string& where
				     , int what);

  std::string _fname;
  FILE* findFiles();
  static FILE* findThisFile(const std::string& fname);
  bool readFiles();
  std::list<ParticleProperties> theList;
  std::map<std::string, std::list<ParticleProperties>::iterator > byName;
  std::map<int, std::list<ParticleProperties>::iterator > byID;

 public:
  static const ParticlePropertiesList* getMe();
  
  const ParticleProperties* get(const std::string& name) const;
  const ParticleProperties* get(int pdg_id) const;
  
  void print(std::ostream& out=std::cout) const;


  // fast fuss-free access:
  static double mass(const std::string& name);
  static double mass(int PDG);
  static double width(const std::string& name);
  static double width(int PDG);
  
};


std::ostream& operator<<(std::ostream& out, const ParticlePropertiesList& ppl);

#endif
//
