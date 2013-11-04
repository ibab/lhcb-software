#ifndef DECAYTREEITEM_HH
#define DECAYTREEITEM_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:03 GMT

#include "Mint/ParticlePropertiesList.h"
#include <iostream>
#include <sstream>
#include <string>

class DecayTreeItem{ 
  // conversion functions make it behave mostly
  // like an int (then it's just _pdg_id).
  // the "_asi" is important in the fit.
  // it associates a final state particle in the
  // decay tree to a particle number (1-3 or 1-4) in
  // the event record.
 public: 
  int _pdg_id;

  int _L_angMom; // angular momentum in decay
  //         (default: -9999; the fitter then uses automatically
  //         the smallest _L_angMom compatible with L conservation and P
  //         conservation - i.e. we assume strong decays.)

  DecayTreeItem(int i=0);
  DecayTreeItem(const DecayTreeItem& other);
  DecayTreeItem& operator=(const DecayTreeItem& other);

  virtual ~DecayTreeItem(){}
  operator int(){
    return _pdg_id;
  }
  operator int() const{
    return _pdg_id;
  }
  int L()const {return _L_angMom;}
  char L_as_SPD()const;
  void setL(int l){ _L_angMom = l;}
  void unSetL(){ _L_angMom = -9999;}

  void setPDG(int id);

  const ParticleProperties* props() const;
  std::string name() const;
  std::string SVPAT() const;
  double mass() const;
  bool isNonResonant() const;
  double width() const;
  std::string charge() const;
  bool hasDistinctAnti() const;
  int pdg() const;
  std::string J() const;

  bool ChargeConThis();
  bool antiThis(){return ChargeConThis();}

  virtual void print(std::ostream& out = std::cout) const;
  virtual void printShort(std::ostream& out = std::cout) const;


  DecayTreeItem operator-() const;
  DecayTreeItem operator+() const;
};

std::ostream& operator<<(std::ostream& out
			 ,  const DecayTreeItem& dti);
std::stringstream& operator<<(std::stringstream& out
			      , const DecayTreeItem& dti);

// this is a hack - ther must be a better solution
std::stringstream& operator<<(std::stringstream& out
			      , int i);


#endif
//
