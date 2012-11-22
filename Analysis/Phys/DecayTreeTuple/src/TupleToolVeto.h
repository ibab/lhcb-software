// $Id: TupleToolVeto.h
#ifndef _TUPLETOOLVETO_H
#define _TUPLETOOLVETO_H 1

// Include files
// from Gaudi
#include "DecayTreeTupleBase/TupleToolBase.h"
// Interfaces
#include "Kernel/IParticleTupleTool.h"
#include "Kernel/IParticleVeto.h"
#include <memory>

//============================================================================

class TupleToolVeto : public TupleToolBase, virtual public IParticleTupleTool 
{
  
public:

  /// Standard constructor
  TupleToolVeto( const std::string& type,
                 const std::string& name,
                 const IInterface* parent );

  /// Initialise
  StatusCode initialize();

  /// Destructor
  virtual ~TupleToolVeto( ){} 

public:

  virtual StatusCode fill( const LHCb::Particle*, 
                           const LHCb::Particle*,
                           const std::string&,
                           Tuples::Tuple& );
private:

  IParticleVeto* m_check;
  std::string m_part;
  unsigned int m_pid;
  std::map<std::string,std::vector<std::string> > m_veto;
  std::map<std::string,std::vector<std::string> > m_vetoOther;

};

#endif // _TUPLETOOLVETO_H
