#ifndef TUPLETOOLTISTOS_H 
#define TUPLETOOLTISTOS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticleTupleTool.h"            // Interface
#include "Kernel/ITriggerTisTos.h" //
#include "Kernel/IParticleDescendants.h"
/** @class TupleToolTISTOS TupleToolTISTOS.h
 *
 *  This is an adaptation of the TISTOS example by Tomasz
 *  Skwarnicki for the DecayTreeTuple framework  
 *
 *  It saves the trigger TIS/TOS decisions for each 
 *  particle for the L0, and each HLT Alley and HltSelection
 *
 *  Saved values for each trigger:  
 *
 *  Decision : the trigger decision, 1 is pass, 0 is fail
 *  TIS      : was the event tis? 1 is yes, 0 is no.  
 *  TOS      : was the event tos? 1 is yes, 0 is no.
 *
 *  @author V. Gligorov
 *  @date   2008-04-09
 */
class IP2VVPartAngleCalculator;

class TupleToolTISTOS : public GaudiTool, virtual public IParticleTupleTool {
public: 
  /// Standard constructor
  TupleToolTISTOS( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~TupleToolTISTOS( ); ///< Destructor

  StatusCode initialize() ;

  virtual StatusCode fill( const LHCb::Particle*
			   , const LHCb::Particle*
			   , const std::string&
			   , Tuples::Tuple& );

protected:
  
private:

  ITriggerTisTos* m_TriggerTisTosTool;
  IParticleDescendants* m_particleDescendants;

  bool m_verboseL0;     ///< get details on L0
  bool m_verboseHlt1;   ///< get details on Hlt1
  bool m_verboseHlt2;   ///< get details on Hlt2

  // do not allow trigger names to mutate from event-to-event
  std::vector<std::string> m_l0;
  std::vector<std::string> m_hlt1;
  std::vector<std::string> m_hlt2;

};
#endif // TUPLETOOLTISTOS_H
