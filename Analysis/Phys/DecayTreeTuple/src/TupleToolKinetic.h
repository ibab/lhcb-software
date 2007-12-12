// $Id: TupleToolKinetic.h,v 1.1.1.1 2007-12-12 17:46:43 pkoppenb Exp $
#ifndef JBOREL_TUPLETOOLKINETIC_H
#define JBOREL_TUPLETOOLKINETIC_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticleTupleTool.h"            // Interface

/** @class TupleToolKinetic TupleToolKinetic.h jborel/TupleToolKinetic.h
 *
 *
 *  @author Jeremie Borel
 *  @date   2007-11-07
 */
class TupleToolKinetic : public GaudiTool, virtual public IParticleTupleTool {
public:
  /// Standard constructor
  TupleToolKinetic( const std::string& type,
		    const std::string& name,
		    const IInterface* parent);

  virtual ~TupleToolKinetic(){}; ///< Destructor

  virtual StatusCode fill( const LHCb::Particle*
			   , const LHCb::Particle*
			   , const std::string&
			   , Tuples::Tuple& );
};

#endif // JBOREL_TUPLETOOLKINETIC_H
