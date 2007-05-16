//-----------------------------------------------------------------------------
/** @file MCReconstructible.cpp
 *
 *  Implementation file for class : MCReconstructible
 *
 *  CVS Log :-
 *  $Id: MCReconstructible.cpp,v 1.8 2007-05-16 15:02:18 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 28/02/2007
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "MCReconstructible.h"

// the data
#include "Event/MCParticle.h"
#include "Event/MCTrackGeomCriteria.h"

#include <boost/assign/list_of.hpp> // for 'map_list_of()'

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( MCReconstructible );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCReconstructible::MCReconstructible( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool  ( type, name , parent      )
  // misc CALO params. Hopefully to go into specific CALO reconstructibility tool
  , m_zECAL    ( 12696.0*Gaudi::Units::mm )
  , m_xECALInn ( 363.3*Gaudi::Units::mm   )
  , m_yECALInn ( 363.3*Gaudi::Units::mm   )
  , m_xECALOut ( 3757.2*Gaudi::Units::mm  )
  , m_yECALOut ( 3030.0*Gaudi::Units::mm  )
  , m_lowEt    ( 200*Gaudi::Units::MeV    )
  // charged track info object
  , m_tkInfo   ( NULL )
  , m_mcSel    ( NULL )
{
  // Interface
  declareInterface<IMCReconstructible>(this);
  // job options
  declareProperty( "AllowPrimaryParticles", m_allowPrimary = true );
  declareProperty("ChargedLong", m_chargedLongCriteria = 
		  boost::assign::list_of("hasVeloAndT"));
  declareProperty("ChargedUpstream", m_chargedUpstreamCriteria = 
		  boost::assign::list_of("hasVelo")("hasTT"));
  declareProperty("ChargedDownstream", m_chargedDownstreamCriteria = 
		  boost::assign::list_of("hasT")("hasTT"));
  declareProperty("ChargedVelo", m_chargedVeloCriteria = 
		  boost::assign::list_of("hasVelo"));
  declareProperty("ChargedTtrack", m_chargedTCriteria = 
		  boost::assign::list_of("hasT"));
}

//=============================================================================
// Destructor
//=============================================================================
MCReconstructible::~MCReconstructible()
{
  // clean up
  delete m_tkInfo;
  delete m_chargedLong;
  delete m_chargedDownstream;
  delete m_chargedUpstream;
  delete m_chargedT;
  delete m_chargedVelo;
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode MCReconstructible::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  // tools
  m_mcSel = tool<IMCParticleSelector>( "MCParticleSelector" );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  using namespace LHCb::MC;
  m_chargedLong = new MCTrackGeomCriteria(m_chargedLongCriteria);
  m_chargedDownstream = new MCTrackGeomCriteria(m_chargedDownstreamCriteria);
  m_chargedUpstream = new MCTrackGeomCriteria(m_chargedUpstreamCriteria);
  m_chargedT = new MCTrackGeomCriteria(m_chargedTCriteria);
  m_chargedVelo = new MCTrackGeomCriteria(m_chargedVeloCriteria);

  return sc;
}

// Method that handles various Gaudi "software events"
void MCReconstructible::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() )
  {
    delete m_tkInfo;
    m_tkInfo = NULL;
  }
}

bool MCReconstructible::inAcceptance( const LHCb::MCParticle* mcPart ) const
{
  return ( !mcPart ? false : ( 0 == mcPart->particleID().threeCharge() ?
                               accept_neutral(mcPart) : accept_charged(mcPart) ) );
}

//====================================================================
// Check if a neutral MCParticle is within the geomtrical acceptance
//====================================================================
bool MCReconstructible::accept_neutral( const LHCb::MCParticle* mcPart ) const
{
  // Acceptance stuff for neutrals
  // Temporary home here whilst a proper CALO tool is being prepared

  bool acc = false;
  if( !mcPart->originVertex() ) return acc;
  const double x  = mcPart->originVertex()->position().x();
  const double y  = mcPart->originVertex()->position().y();
  const double z  = mcPart->originVertex()->position().z();
  const double sx = mcPart->momentum().px()/mcPart->momentum().pz();
  const double sy = mcPart->momentum().py()/mcPart->momentum().pz();
  //debug() << "accepted " << x << " " << y << " " << z << " & " << sx << " " << sy << endmsg ;

  //  if( (mcPart->particleID().threeCharge()) == 0 ) {
  // For neutral particles, the ECAL front has to be hit
  // ---------------------------------------------------
  const double xECAL = x + sx * ( m_zECAL - z );
  const double yECAL = y + sy * ( m_zECAL - z );
  if( ((fabs(xECAL) <= m_xECALOut) && (fabs(yECAL) <= m_yECALOut))
      && ((fabs(xECAL) >= m_xECALInn) || (fabs(yECAL) >= m_yECALInn ))
      ) {
    if( mcPart->pt() >= m_lowEt ) {
      acc = true;
    }
  }

  return acc;
}

//====================================================================
// Check if a charged MCParticle is within the geomtrical acceptance
//====================================================================
bool MCReconstructible::accept_charged( const LHCb::MCParticle * mcPart ) const
{
  return ( mcTkInfo().accVelo(mcPart) ||
           mcTkInfo().accTT(mcPart)   ||
           mcTkInfo().accT(mcPart)     );
}

//=============================================================================
//  Method to check if a particle is reconstructible
//=============================================================================
IMCReconstructible::RecCategory
MCReconstructible::reconstructible( const LHCb::MCParticle* mcPart ) const
{
  if ( !mcPart ) return NoClassification;

  // acceptance ?
  if ( !inAcceptance(mcPart) ) return OutsideAcceptance;

  // Base class MCParticle selection
  if ( m_mcSel->accept(mcPart) )
  {
    // Does MCParticle have a mother
    if ( m_allowPrimary || NULL != mcPart->mother() )
    {

      // charged or neutral
      const bool isCharged = mcPart->particleID().threeCharge() != 0;  
      if ( isCharged )
      {

        // decide the type
        if ( m_chargedLong->accepted(mcTkInfo(),mcPart) == true )
        {
          return ChargedLong;
        }
        else if (m_chargedUpstream->accepted(mcTkInfo(),mcPart) == true )
        {
          return ChargedUpstream;
        }
        else if (m_chargedDownstream->accepted(mcTkInfo(),mcPart) == true)
        {
          return ChargedDownstream;
        }
        else if ( m_chargedVelo->accepted(mcTkInfo(),mcPart) == true )
        {
          return ChargedVelo;
        }
        else if ( m_chargedT->accepted(mcTkInfo(),mcPart) == true )
        {
          return ChargedTtrack;
        }

      }
      else // neutral
      {

        // only one type at the moment
        return Neutral;

      }

    } // has mother
  } // MCP selection

  // if get here, cannot reconstruct particle
  return NotReconstructible;
}
