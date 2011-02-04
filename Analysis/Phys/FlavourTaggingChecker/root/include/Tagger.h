#ifndef Tagger_H
#define Tagger_H 1

#include <vector>
#include "Particle.h"

class Tagger;

typedef std::vector<Tagger*> Taggers;

/** @class GenericTool
 *
 *  Class representing the tagger object
 *
 *  @author Marco Musy
 *  @date   2010-07-15
 */

class Tagger {

 public:

  enum TaggerType{ none,
		   unknown,
		   OS_Muon,
		   OS_Electron,
		   OS_Kaon,
		   SS_Kaon,
		   SS_Pion,
		   jetCharge,
		   OS_jetCharge,
		   SS_jetCharge,
		   VtxCharge,
		   Topology
  };

  Tagger( ) {  
    this->reset();
  } ///< Constructor
  ~Tagger( ){} ///< Destructor

  void reset() {
    m_RawNNetOutput= m_Omega = 0.5;
    m_Decision=0, m_charge=0, m_Type=0;   
    m_taggerparts.clear();
  }///< resets tagger members

  double RawNNetOutput() { return m_RawNNetOutput; }///< return raw neural net output
  double omega()   { return m_Omega; }///< return estimated omega (user needs to calibrate is using data)
  int    decision(){ return m_Decision; }///< tagger decision on flavour
  double    charge()  { return m_charge; }///< tagger charge
  int    type()    { return m_Type; }///< tagger type

  /// return particles used by this tagger to make decision
  Particles taggerparts() { return m_taggerparts; };

  /// add this particle to container
  void addToTaggerParts ( Particle* a ) { m_taggerparts.push_back(a); }

  void setRawNNetOutput(double a)  { m_RawNNetOutput = a; }
  void setOmega(double a)   { m_Omega = a; }
  void setDecision(int a)   { m_Decision = a; }
  void setcharge(double a)     { m_charge = a; }
  void setType(int a)       { m_Type = a; }


  void Print() {
    if(DBGLEVEL<3 && this->type() ) {
      cout << "--- Tagger type=" << this->taggerparts().at(0)->ID()
	   << " decision="<< this->decision() 
	   << " RawNNet="<< this->RawNNetOutput()
	   << " omega="<< this->omega()
	   << " tagger_pt="<<this->taggerparts().at(0)->pt()
	   <<endl;
    }
  }///< dump tagger information

 private:
  double m_RawNNetOutput, m_Omega;
  int    m_Decision, m_Type;
  double m_charge;
  Particles m_taggerparts;

};

#endif 
