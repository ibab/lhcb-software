// $Id: MCReconstructible.cpp,v 1.1 2007-01-12 14:16:40 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "MCReconstructible.h"

// the data
#include "Event/MCParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCReconstructible
//
// 2004-04-06 : Christian Jacoby
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( MCReconstructible );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCReconstructible::MCReconstructible( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_zECAL(12696.0*Gaudi::Units::mm)
  , m_xECALInn(363.3*Gaudi::Units::mm)
  , m_yECALInn(363.3*Gaudi::Units::mm)
  , m_xECALOut(3757.2*Gaudi::Units::mm)
  , m_yECALOut(3030.0*Gaudi::Units::mm)
  , m_lowEt(200*Gaudi::Units::MeV)
{
  declareInterface<IMCReconstructible>(this);

}
//=============================================================================
// Destructor
//=============================================================================
MCReconstructible::~MCReconstructible() {}; 

//=============================================================================


//=============================================================================
// Initialize
//=============================================================================
StatusCode MCReconstructible::initialize() {

  StatusCode sc = GaudiTool::initialize() ;
  if (!sc) return sc;

  debug() << ">>> Initialize" << endreq;

  return sc;
   
};
 
//=============================================================================
// Finalize
//=============================================================================
StatusCode MCReconstructible::finalize()
{
  
  debug() << ">>> Finalize" << endreq;
 


  return GaudiTool::finalize();
}

//====================================================================
// Check if a neutral MCParticle is within the geomtrical acceptance
//====================================================================
bool MCReconstructible::accepted( const LHCb::MCParticle* part ) {
  bool acc = false;
  if( !part->originVertex() ) return acc;
  double x = part->originVertex()->position().x();
  double y = part->originVertex()->position().y();
  double z = part->originVertex()->position().z();
  double sx = part->momentum().px()/part->momentum().pz();
  double sy = part->momentum().py()/part->momentum().pz();  
  debug() << "accpted " << x << " " << y << " " << z << " & " << sx << " " << sy << endmsg ;

  //  if( (part->particleID().threeCharge()) == 0 ) {
    // For neutral particles, the ECAL front has to be hit
    // ---------------------------------------------------
    double xECAL = x + sx * ( m_zECAL - z );
    double yECAL = y + sy * ( m_zECAL - z );
    if( ((fabs(xECAL) <= m_xECALOut) && (fabs(yECAL) <= m_yECALOut))
        && ((fabs(xECAL) >= m_xECALInn) || (fabs(yECAL) >= m_yECALInn ))
        ) {
      if( part->pt() >= m_lowEt ) {
        acc = true;
      }
    }
    //  }
  return acc;
};
//=============================================================================
//  Method to check if a particle is reconstructible
//=============================================================================
IMCReconstructible::RecblCategory MCReconstructible::reconstructible( const LHCb::MCParticle* iMCstable )
{

  if (iMCstable->particleID().threeCharge() != 0){ ///< for charged
    if (NULL != iMCstable->mother() ) {

      /* not yet available
      /// for long tracks
      if (trInfo.hasVeloAndT(iMCstable)) {
        return IMCReconstructible::Long;
      }
      
      /// for downstream tracks (TTT)
      if (trInfo.hasTT(iMCstable) && trInfo.hasT(iMCstable)) {
        return IMCReconstructible::TTT;
      }
      
      /// for upstream tracks (VTT)
      if (trInfo.hasVelo(iMCstable) && trInfo.hasTT(iMCstable)) {
        return IMCReconstructible::VTT;
      }
      */
      /// @todo Remove TEMPORARY hard-coded patch
      debug() << iMCstable->p() << endmsg ;
      if ( (iMCstable->p()>1*Gaudi::Units::GeV) &&
           accepted(iMCstable)){
        return IMCReconstructible::Long;
      }

    }
  }
  else { ///< for neutrals
    if (NULL != iMCstable->mother() ) {
      
      if (accepted(iMCstable)){
        return IMCReconstructible::Neutral;
      }
    }
  }
  
  return IMCReconstructible::NotRec;
  
}


