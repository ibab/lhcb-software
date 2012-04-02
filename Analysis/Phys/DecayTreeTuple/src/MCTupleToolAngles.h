// $Id: MCTupleToolAngles.h,v 1.3 2010-01-26 15:39:25 rlambert Exp $
#ifndef MCTUPLETOOLANGLES_H
#define MCTUPLETOOLANGLES_H 1

// Include files
// from Gaudi
#include "TupleToolBase.h"
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

class MCTupleToolAngles : public TupleToolBase,
                          virtual public IMCParticleTupleTool
{

public:

  /// Standard constructor
  MCTupleToolAngles( const std::string& type,
                     const std::string& name,
                     const IInterface* parent);

  virtual ~MCTupleToolAngles( ); ///< Destructor

  virtual StatusCode fill( const LHCb::MCParticle*
                           , const LHCb::MCParticle*
                           , const std::string&
                           , Tuples::Tuple& );

private:

#include "Theta.icpp"

};

#endif // MCTUPLETOOLANGLES_H
