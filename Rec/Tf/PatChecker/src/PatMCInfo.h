// $Id: PatMCInfo.h,v 1.1.1.1 2007-08-22 15:38:27 smenzeme Exp $
#ifndef PATMCINFO_H 
#define PATMCINFO_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

#include "VeloDet/DeVelo.h"
#include "STDet/DeSTDetector.h"
#include "OTDet/DeOTDetector.h"
#include "PatTools/PatDataStore.h"

#include "Event/MCParticle.h"

static const InterfaceID IID_PatMCInfo ( "PatMCInfo", 1, 0 );

/** @class PatMCInfo PatMCInfo.h
 *  Tool to get MC information, and print it whenever needed
 *
 *  @author Olivier Callot
 *  @date   2005-03-24
 *  @update for A-Team framework 2007-08-20 SHM
 */

namespace Tf {

  class PatMCInfo : public GaudiTool {
  public: 
    
    // Return the interface ID
    static const InterfaceID& interfaceID() { return IID_PatMCInfo; }
    
    /// Standard constructor
    PatMCInfo( const std::string& type, 
	       const std::string& name,
	       const IInterface* parent);
    
    virtual ~PatMCInfo( ); ///< Destructor
    
    virtual StatusCode initialize();
    
    void printMCParticle( const LHCb::MCParticle* part );
    
    void printTrueMeasurements( const LHCb::MCParticle* part );
    
    bool isBChild( const LHCb::MCParticle* part );
    
    bool isGoodBChild( const LHCb::MCParticle* part );
    
  protected:
    
  private:
    IParticlePropertySvc* m_ppSvc;
    
    DeVelo* m_velo;
    PatVeloContainer*    m_veloContainer;
    PatTCoordContainer*  m_tContainer;
    PatTTCoordContainer* m_ttContainer;
  };
}
#endif // PATMCINFO_H
