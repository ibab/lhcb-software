// $Id: MCTupleToolPID.h,v 1.1 2009-11-19 12:22:06 rlambert Exp $
#ifndef MCTUPLETOOLPID_H 
#define MCTUPLETOOLPID_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IMCParticleTupleTool.h"            // Interface
#include "Event/MCParticle.h"            // Interface

/** @class MCTupleToolPID MCTupleToolPID.h
 *  
 *  Fill MC Particle 
 *
 * - head_ID : pid
 *
 * 
 * \sa MCTupleToolPID, DecayTreeTuple, MCDecayTreeTuple
 * 
 *  @author Rob Lambert
 *  @date   2009-11-19
 */

class MCTupleToolPID : public GaudiTool, 
                       virtual public IMCParticleTupleTool {
public: 
  /// Standard constructor
  MCTupleToolPID( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  virtual ~MCTupleToolPID( ); ///< Destructor
  virtual StatusCode initialize();

  virtual StatusCode fill( const LHCb::MCParticle*
			   , const LHCb::MCParticle*
			   , const std::string&
			   , Tuples::Tuple& );



};

#endif // MCTUPLETOOLPID_H
