// $Id: TupleToolAngles.h,v 1.1 2009-01-23 10:54:36 pkoppenb Exp $
#ifndef TUPLETOOLANGLES_H 
#define TUPLETOOLANGLES_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticleTupleTool.h"            // Interface
#include "Math/Boost.h"
#include "GaudiKernel/Vector4DTypes.h"


/** @class TupleToolAngles TupleToolAngles.h
 *  
 *  Fill MC Particle with decay angle in mother frame
 *
 * - head_CosTheta : angle in mother's frame
 * 
 * \sa MCTupleToolAngles, DecayTreeTuple, MCDecayTreeTuple
 * 
 *  @author Patrick Koppenburg
 *  @date   2009-01-21
 */

class TupleToolAngles : public GaudiTool, virtual public IParticleTupleTool {
public: 
  /// Standard constructor
  TupleToolAngles( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  virtual ~TupleToolAngles( ); ///< Destructor
  virtual StatusCode initialize();
  virtual StatusCode fill( const LHCb::Particle*
			   , const LHCb::Particle*
			   , const std::string&
			   , Tuples::Tuple& );


protected:

private:
#include "Theta.icpp"
private:

};
#endif // TUPLETOOLANGLES_H
