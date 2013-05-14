// $Id: BDecayTool.h,v 1.2 2007-05-29 08:48:16 cattanem Exp $
#ifndef BDECAYTOOL_H 
#define BDECAYTOOL_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

//from Event
#include "Event/MCParticle.h"
#include "Event/GenHeader.h"

// Interface
//#include "MCInterfaces/IBDecayTool.h"
#include "MCInterfaces/IMCDecayFinder.h"
#include "IBDecayTool.h"

/** @class BDecayTool BDecayTool.h
 *  
 */

class BDecayTool : public GaudiTool, 
	virtual public IBDecayTool {
public:
  /// Standard constructor
  BDecayTool( const std::string& type, 
		    const std::string& name,
		    const IInterface* parent );

  virtual ~BDecayTool( ); ///< Destructor

  /// Initialize
  StatusCode initialize();

  /// Finalize
  StatusCode finalize();
  
  const LHCb::MCParticle* forcedB(void);

  //IMCDecayFinder* mcDecayFinder() const;
  IMCDecayFinder* m_mcDecay;         //< the DecayTypeFinder tool


private:

  //IMCDecayFinder* m_mcDecay;         //< the DecayTypeFinder tool

  //LHCb::MCParticle* associatedofHEP(HepMC::GenParticle* ) ;

};
#endif // BDECAYTOOL_H
