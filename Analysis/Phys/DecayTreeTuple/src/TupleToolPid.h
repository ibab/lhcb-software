// $Id: TupleToolPid.h,v 1.1.1.1 2007-12-12 17:46:43 pkoppenb Exp $
#ifndef JBOREL_TUPLETOOLPID_H
#define JBOREL_TUPLETOOLPID_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticleTupleTool.h"            // Interface

/** @class TupleToolPid TupleToolPid.h jborel/TupleToolPid.h
 *
 * \brief DLL and PID information to be stored in a Tuple
 *
 * \sa DecayTreeTuple
 *
 * For all particles:
 * - head_ID : particleID().pid();
 *
 * For the long lived particles (isBasicParticle()).
 * - head_PIDe : LHCb::ProtoParticle::CombDLLe
 * - head_PIDmu : LHCb::ProtoParticle::CombDLLmu
 * - head_PIDK : LHCb::ProtoParticle::CombDLLk
 * - head_PIDp : LHCb::ProtoParticle::CombDLLp
 *
 *  @author Jeremie Borel
 *  @date   2007-11-07
 */
class TupleToolPid : public GaudiTool, virtual public IParticleTupleTool {
public:
  /// Standard constructor
  TupleToolPid( const std::string& type,
		     const std::string& name,
		     const IInterface* parent);

  virtual ~TupleToolPid(){}; ///< Destructor

  virtual StatusCode fill( const LHCb::Particle*, const LHCb::Particle*
			   , const std::string&, Tuples::Tuple& );
};

#endif // JBOREL_TUPLETOOLPID_H
