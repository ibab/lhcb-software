// $Id: TupleToolDalitz.h,v 1.2 2009-02-18 18:41:04 pkoppenb Exp $
#ifndef TUPLETOOLDALITZ_H 
#define TUPLETOOLDALITZ_H 1

// Include files
// from Gaudi
#include "Kernel/IParticleTupleTool.h"     
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/Escape.h"
#include "Event/Particle.h" 
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

/** @class TupleToolDalitz TupleToolDalitz.h
 *  
 *  Fills square masses of all combinations of direct daughters
 *
 *  @author Patrick Koppenburg
 *  @date   2009-02-17
 */
class TupleToolDalitz : public GaudiTool, virtual public IParticleTupleTool {
public: 
  /// Standard constructor
  TupleToolDalitz( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~TupleToolDalitz( ); ///< Destructor

  /// The filling method
  virtual StatusCode fill( const LHCb::Particle*
			   , const LHCb::Particle*
			   , const std::string&
			   , Tuples::Tuple& );

protected:

private:
  // the functionality
#include "TupleToolDalitz.icpp"

};
#endif // TUPLETOOLDALITZ_H
