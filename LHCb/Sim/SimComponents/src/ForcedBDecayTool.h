// $Id: ForcedBDecayTool.h,v 1.1 2007-02-26 17:28:47 cattanem Exp $
#ifndef FORCEDBDECAYTOOL_H 
#define FORCEDBDECAYTOOL_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

//from Event
#include "Event/MCParticle.h"
#include "Event/GenHeader.h"

//from Kernel (MCEvent)
#include "Kernel/IForcedBDecayTool.h"            // Interface

/** @class ForcedBDecayTool ForcedBDecayTool.h Kernel/ForcedBDecayTool.h
 *  
 *  v1.0
 *  @author Marco Musy
 *  @date   2006-10-02
 */
class ForcedBDecayTool : public GaudiTool, 
	virtual public IForcedBDecayTool {
public:
  /// Standard constructor
  ForcedBDecayTool( const std::string& type, 
		    const std::string& name,
		    const IInterface* parent );

  virtual ~ForcedBDecayTool( ); ///< Destructor

  /// Initialize
  StatusCode initialize();

  /// Finalize
  StatusCode finalize();
  
  const LHCb::MCParticle* forcedB(void);

private:

  LHCb::MCParticle* associatedofHEP(HepMC::GenParticle* ) ;

};
#endif // FORCEDBDECAYTOOL_H
