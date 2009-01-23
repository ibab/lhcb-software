// $Id: MCTupleToolAngles.h,v 1.2 2009-01-23 10:54:36 pkoppenb Exp $
#ifndef MCTUPLETOOLANGLES_H 
#define MCTUPLETOOLANGLES_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IMCParticleTupleTool.h"            // Interface
#include "Math/Boost.h"
#include "GaudiKernel/Vector4DTypes.h"


/** @class MCTupleToolAngles MCTupleToolAngles.h
 *  
 *  Fill MC Particle with decay angle in mother frame
 *
 * - head_TRUECosTheta : angle in mother's frame
 * 
 * \sa TupleToolAngles, DecayTreeTuple, MCDecayTreeTuple
 * 
 *  @author Patrick Koppenburg
 *  @date   2009-01-19
 */

class MCTupleToolAngles : public GaudiTool, virtual public IMCParticleTupleTool {
public: 
  /// Standard constructor
  MCTupleToolAngles( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  virtual ~MCTupleToolAngles( ); ///< Destructor
  virtual StatusCode initialize();
  virtual StatusCode fill( const LHCb::MCParticle*
			   , const LHCb::MCParticle*
			   , const std::string&
			   , Tuples::Tuple& );


protected:

private:
#include "Theta.icpp"
private:

};
#endif // MCTUPLETOOLANGLES_H
