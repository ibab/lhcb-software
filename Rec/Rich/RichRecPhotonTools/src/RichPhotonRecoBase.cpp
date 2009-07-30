
//-----------------------------------------------------------------------------
/** @file RichPhotonRecoBase.cpp
 *
 * Implementation file for class : Rich::Rec::PhotonRecoBase
 *
 * CVS Log :-
 * $Id: RichPhotonRecoBase.cpp,v 1.2 2009-07-30 11:20:00 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2003-11-14
 */
//-----------------------------------------------------------------------------

// local
#include "RichPhotonRecoBase.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PhotonRecoBase::
PhotonRecoBase( const std::string& type,
                const std::string& name,
                const IInterface* parent )
  : Rich::Rec::ToolBase   ( type, name, parent ),
    m_checkPhotCrossSides ( Rich::NRadiatorTypes, false ),
    m_ckFudge             ( Rich::NRadiatorTypes, 0     )
{
  // declare interface
  declareInterface<IPhotonReconstruction>(this);

  // JOs
  m_checkPhotCrossSides[Rich::Aerogel]  = false;
  m_checkPhotCrossSides[Rich::Rich1Gas] = true;
  m_checkPhotCrossSides[Rich::Rich2Gas] = true;
  declareProperty( "CheckSideCrossing", m_checkPhotCrossSides );
  declareProperty( "CKThetaQuartzRefractCorrections", m_ckFudge );

}

//=============================================================================
// Destructor
//=============================================================================
PhotonRecoBase::~PhotonRecoBase() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode PhotonRecoBase::initialize()
{
  // initialise base class
  const StatusCode sc = Rich::Rec::ToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // loop over radiators
  for ( Rich::Radiators::const_iterator rad = Rich::radiators().begin();
        rad != Rich::radiators().end(); ++rad )
  {
    if ( m_checkPhotCrossSides[*rad] )
    {      info() << "Will reject photons that cross sides in " << *rad << endmsg; }

    // fudge factor warning
    if ( std::fabs(m_ckFudge[*rad]) > 1e-7 )
    {
      info() << "Applying " << Rich::text(*rad) 
             << " CK theta correction factor : " << m_ckFudge[*rad] << endmsg;
    }
  }

  return sc;
}

//=============================================================================
// Finalize
//=============================================================================
StatusCode PhotonRecoBase::finalize()
{
  return Rich::Rec::ToolBase::finalize();
}
