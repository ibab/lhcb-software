#ifndef HEPMCEVENT_H
#define HEPMCEVENT_H 1


// --------------------------------------------------
//
// File:   Event/HepMCEvent.h
//
// AuthorList:
//         W. Pokorski: initial coding April 02 (based on ATLAS code)
// -------------------------------------------------------------------------

#include <string>
#include <vector>
#include <map>
#include "GaudiKernel/ContainedObject.h" 
#include "GaudiKernel/ObjectVector.h"
#include "HepMC/GenEvent.h"

static const CLID CLID_HepMCEvent = 2201;


class HepMCEvent : public ContainedObject {

  typedef unsigned int uint;
  typedef const unsigned int cuint;

 public:
  // Constructors and Destructor:-
  HepMCEvent();
  HepMCEvent(std::string generatorName, int ProcessId, int EventNum);
  HepMCEvent(const HepMCEvent& copy);
  virtual ~HepMCEvent();

  // Assignment
  HepMCEvent& operator = (const HepMCEvent& evt);

  // Public accessors:
  std::string generatorName() const {return m_generatorName;};
  HepMC::GenEvent* pGenEvt() const {return m_pEvt;};

  // Serialize for writing
  virtual StreamBuffer& serialize( StreamBuffer& s ) const;
  
  // Serialize for reading
  virtual StreamBuffer& serialize( StreamBuffer& s );

  // Needed by Event Service
  static  const CLID& classID()    { return CLID_HepMCEvent;}
  virtual const CLID& clID() const { return CLID_HepMCEvent;}

  int identifier() const { return m_identifier; }

 private:

  std::string m_generatorName;
  HepMC::GenEvent* m_pEvt;

  // For the serializers
  uint m_identifier;

  void write_vertex( const HepMC::GenVertex* v, StreamBuffer &s ) const;
  void write_particle( const HepMC::GenParticle* p, StreamBuffer &s ) const;

  HepMC::GenVertex* read_vertex( StreamBuffer &s );
  HepMC::GenParticle* read_particle (int* bar_end, int* bar_prod, StreamBuffer &s);

};


typedef ObjectVector<HepMCEvent>     HepMCEventVector;


#endif
