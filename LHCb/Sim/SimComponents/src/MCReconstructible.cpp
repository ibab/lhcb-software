
//-----------------------------------------------------------------------------
/** @file MCReconstructible.cpp
 *
 *  Implementation file for class : MCReconstructible
 *
 *  CVS Log :-
 *  $Id: MCReconstructible.cpp,v 1.2 2007-03-01 20:24:56 jonrob Exp $
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
}

//=============================================================================
// Destructor
//=============================================================================
MCReconstructible::~MCReconstructible() 
{
  // clean up
  delete m_tkInfo;
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

  return sc;
}

// Method that handles various Gaudi "software events"
void MCReconstructible::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() )
  {
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

  // Base class MCParticle selection
  if ( m_mcSel->accept(mcPart) )
  {
    // Does MCParticle have a mother (CRJ : why this check ?)
    if (NULL != mcPart->mother() )
    {

      // Is the MCParticle charged or not
      if ( mcPart->particleID().threeCharge() != 0 )
      {

        // in charged acceptance ?
        if ( accept_charged(mcPart) )
        {
          // decide the type
          if      ( mcTkInfo().hasVeloAndT(mcPart) )
          {
            return ChargedLong;
          }
          else if ( mcTkInfo().hasVelo(mcPart) && mcTkInfo().hasTT(mcPart) )
          {
            return ChargedUpstream;
          }
          else if ( mcTkInfo().hasT(mcPart) && mcTkInfo().hasTT(mcPart) )
          {
            return ChargedDownstream;
          }
          else if ( mcTkInfo().hasVelo(mcPart) )
          {
            return ChargedVelo;
          }
          else if ( mcTkInfo().hasT(mcPart) )
          {
            return ChargedTtrack;
          }
        }
      }
      else // neutral
      {
        // in charged acceptance ?
        if ( accept_neutral(mcPart) )
        {
          return Neutral;
        }
      }

    } // has mother
  } // MCP selection

  // if get here, cannot reconstruct particle
  return NotReconstructible;
}
