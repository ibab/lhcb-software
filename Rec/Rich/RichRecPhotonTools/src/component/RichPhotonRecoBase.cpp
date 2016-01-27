
//-----------------------------------------------------------------------------
/** @file RichPhotonRecoBase.cpp
 *
 * Implementation file for class : Rich::Rec::PhotonRecoBase
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2003-11-14
 */
//-----------------------------------------------------------------------------

// local
#include "RichPhotonRecoBase.h"

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Rich::Rec::PhotonRecoBase::PhotonRecoBase( const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent )
  : Rich::Rec::ToolBase ( type, name, parent ),
    m_ckBiasCorrs( Rich::NRadiatorTypes, 0.0 ),
    m_deBeam     ( Rich::NRiches,    nullptr )
{
  // declare interface
  declareInterface<IPhotonReconstruction>(this);
  // JOs                                     Aero   R1Gas  R2Gas
  declareProperty( "CheckSideCrossing", 
                   m_checkPhotCrossSides = { false, false, false } );
  declareProperty( "CKThetaQuartzRefractCorrections", 
                   m_ckJOCorrs           = { 0.0,   0.0,   0.0   } );
}

//=============================================================================
// Destructor
//=============================================================================
Rich::Rec::PhotonRecoBase::~PhotonRecoBase() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode Rich::Rec::PhotonRecoBase::initialize()
{
  // initialise base class
  const StatusCode sc = Rich::Rec::ToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // get tools
  acquireTool( "RichPhotonEmissionPoint", m_emissPoint );

  // loop over radiators
  for ( const auto& rad : Rich::radiators() )
  {
    if ( m_checkPhotCrossSides[rad] )
    { info() << "Will reject photons that cross sides in " << rad << endmsg; }

    // fudge factor warning
    if ( std::fabs(m_ckJOCorrs[rad]) > 1e-7 )
    {
      info() << "Applying " << Rich::text(rad) 
             << " CK theta correction factor : " << m_ckJOCorrs[rad] << endmsg;
    }
  }

  return sc;
}

//=============================================================================
