#ifndef TUPLETOOLTISTOS_H 
#define TUPLETOOLTISTOS_H 1

// Include files
// from Gaudi
#include "TupleToolTriggerBase.h"
#include "Kernel/IParticleTupleTool.h"            // Interface
#include "Kernel/ITriggerTisTos.h"

class IP2VVPartAngleCalculator;

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
 * If verbose is true the tool needs somehow to find a list of triggers to fill.
 * In this case it uses the base class TupleToolTriggerBase to sort everything out.
 *
 * If \b VerboseL0 = true
 * L0Decision_xxx : LHCb::L0DUReport->channelDecisionByName(xxx)
 *
 * If \b VerboseHlt1 = true
 * Hlt1_xxx_Decision : filled
 *
 * If \b VerboseHlt2 = true
 * Hlt2_xxx_Decision : filled
 *
 * Verbose flag is a shortcut to turn all verbosity on.
 *
 *  @author V. Gligorov
 *  @date   2008-04-09
 */

class TupleToolTISTOS : public TupleToolTriggerBase,
                        virtual public IParticleTupleTool
{

public:

  /// Standard constructor
  TupleToolTISTOS( const std::string& type,
                   const std::string& name,
                   const IInterface* parent);

  virtual ~TupleToolTISTOS( ); ///< Destructor

  StatusCode initialize() ;

protected:

  /// to make it possible to replace standard TisTos tools.
  std::string m_TriggerTisTosName;
  std::string m_L0TriggerTisTosName;
  
private:

  ITriggerTisTos* m_TriggerTisTosTool;
  ITriggerTisTos* m_L0TriggerTisTosTool;

private:

  virtual StatusCode fillBasic( const LHCb::Particle*
                                , const LHCb::Particle* P
                                , const std::string& head
                                , Tuples::Tuple& tuple );

  virtual StatusCode fillVerbose( const LHCb::Particle*
                                  , const LHCb::Particle* P
                                  , const std::string& head
                                  , Tuples::Tuple& tuple );

  std::string getName(const std::string& mystr);

private:

  virtual StatusCode fillBasic( Tuples::Tuple& T );

  virtual StatusCode fillVerbose( Tuples::Tuple& T );

private:

  std::vector<int> m_pidList; 
  bool checkPID;
  std::string m_hlt1Phys;
  std::string m_hlt2Phys;
  
};

#endif // TUPLETOOLTISTOS_H
