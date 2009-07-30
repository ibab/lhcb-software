
//---------------------------------------------------------------------------------
/** @file RichBaseTrSegMakerFromRecoTracks.cpp
 *
 * Implementation file for class : Rich::Rec::BaseTrSegMakerFromRecoTracks
 *
 * CVS Log :-
 * $Id: RichBaseTrSegMakerFromRecoTracks.cpp,v 1.4 2009-07-30 11:25:33 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//---------------------------------------------------------------------------------

// local
#include "RichBaseTrSegMakerFromRecoTracks.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-------------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BaseTrSegMakerFromRecoTracks::
BaseTrSegMakerFromRecoTracks( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : Rich::Rec::ToolBase  ( type, name, parent          ),
    m_deBeam             ( Rich::NRiches               ),
    m_checkBeamP         ( Rich::NRadiatorTypes        ),
    m_usedRads           ( Rich::NRadiatorTypes, true  )
{
  // the interface
  declareInterface<ITrSegMaker>(this);

  // initialise
  m_deBeam[Rich::Rich1] = NULL;
  m_deBeam[Rich::Rich2] = NULL;

  // JOs

  m_checkBeamP[Rich::Aerogel]  = false;
  m_checkBeamP[Rich::Rich1Gas] = true;
  m_checkBeamP[Rich::Rich2Gas] = false;
  declareProperty( "CheckBeamPipe", m_checkBeamP );

  declareProperty( "UseRadiators",  m_usedRads   );
}

//=============================================================================
// Destructor
//=============================================================================
BaseTrSegMakerFromRecoTracks::~BaseTrSegMakerFromRecoTracks() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode BaseTrSegMakerFromRecoTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::ToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  if ( !usedRads(Rich::Aerogel) )
  {
    Warning("Track segments for Aerogel are disabled",StatusCode::SUCCESS).ignore();
  }
  if ( !usedRads(Rich::Rich1Gas) )
  {
    Warning("Track segments for Rich1Gas are disabled",StatusCode::SUCCESS).ignore();
  }
  if ( !usedRads(Rich::Rich2Gas) )
  {
    Warning("Track segments for Rich2Gas are disabled",StatusCode::SUCCESS).ignore();
  }

  info() << "Beam pipe intersection test (Aero/R1Gas/R2Gas) = " << m_checkBeamP << endmsg;
  if ( m_checkBeamP[Rich::Aerogel]  ) deBeam(Rich::Aerogel);
  if ( m_checkBeamP[Rich::Rich1Gas] ) deBeam(Rich::Rich1Gas);
  if ( m_checkBeamP[Rich::Rich2Gas] ) deBeam(Rich::Rich2Gas);
  
  return sc;
}

