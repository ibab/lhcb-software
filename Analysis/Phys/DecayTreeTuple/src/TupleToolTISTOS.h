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
 *  Saved values: (once for TIS and once for TOS) 
 *
 *  -99 : trigger decision not present
 *   -1 : the event did not pass the trigger
 *    0 : the event passed the trigger and was not TIS (TOS)
 *        with respect to the particle being tested
 *    1 : the event passed the trigger and was TIS (TOS)
 *        with respect to the particle being tested
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

  void dumpvs( const std::vector< std::string > & vs )
  {
    for( std::vector< std::string >::const_iterator s=vs.begin();s != vs.end(); ++s)
    {
      info() << " " << *s;
    }
  }

  void setBadTriggerDecision( bool& test, 
                              const std::string& head, 
                              Tuples::Tuple& tuple
                            )
  {

   test &= tuple->column( head+"_L0TIS", -99 );
   test &= tuple->column( head+"_L0TOS", -99 );
   test &= tuple->column( head+"_HLTALLEYSTIS", -99 );
   test &= tuple->column( head+"_HLTALLEYSTOS", -99 );
   test &= tuple->column( head+"_HLTSELTIS", -99 );
   test &= tuple->column( head+"_HLTSELTOS", -99); 

  }

  void saveTriggers( const std::vector< std::string > & vs, 
                     bool& test, 
                     std::string TISORTOS, 
                     int value, 
                     const std::string& head,
                     Tuples::Tuple& tuple 
                   )
  {
    for( std::vector< std::string >::const_iterator s=vs.begin();s != vs.end(); ++s)
    {
      test &= tuple->column( head+"_" + *s + TISORTOS, value );
    }
  }
 
  ITriggerTisTos* m_TriggerTisTosTool;
  IParticleDescendants* m_particleDescendants;

};
#endif // TUPLETOOLTISTOS_H
