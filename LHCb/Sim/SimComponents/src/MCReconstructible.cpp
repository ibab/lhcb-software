//-----------------------------------------------------------------------------
/** @file MCReconstructible.cpp
 *
 *  Implementation file for class : MCReconstructible
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 28/02/2007
 */
//-----------------------------------------------------------------------------

// local
#include "MCReconstructible.h"

// the data
#include "Event/MCParticle.h"
#include "Event/MCTrackGeomCriteria.h"
#include "CaloDet/DeCalorimeter.h"
#if !(defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L)
#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#endif
#include "LHCbMath/LHCbMath.h"

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( MCReconstructible )

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
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
  declareProperty("ChargedLong", m_chargedLongCriteria = {"hasVeloAndT"});
  declareProperty("ChargedUpstream", m_chargedUpstreamCriteria = {"hasVelo", "hasTT"});
  declareProperty("ChargedDownstream", m_chargedDownstreamCriteria = {"hasT", "hasTT"});
  declareProperty("ChargedVelo", m_chargedVeloCriteria = {"hasVelo"});
  declareProperty("ChargedTtrack", m_chargedTCriteria = {"hasT"});
#else
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
#endif
  declareProperty("NeutralEtMin", m_lowEt);
}

//=============================================================================
// Destructor
//=============================================================================
MCReconstructible::~MCReconstructible()
{
  // clean up
  delete m_tkInfo;
  for ( CriteriaMap::iterator iterC = m_critMap.begin();
	iterC != m_critMap.end(); ++iterC ) { delete iterC->second; }
  m_critMap.clear();
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode MCReconstructible::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  // tools
  m_mcSel = tool<IMCParticleSelector>( "MCParticleSelector", "Selector", this );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  using namespace LHCb::MC;
  m_critMap.insert(ChargedLong, new MCTrackGeomCriteria(m_chargedLongCriteria));
  m_critMap.insert(ChargedDownstream, new MCTrackGeomCriteria(m_chargedDownstreamCriteria));
  m_critMap.insert(ChargedUpstream, new MCTrackGeomCriteria(m_chargedUpstreamCriteria));
  m_critMap.insert(ChargedTtrack, new MCTrackGeomCriteria(m_chargedTCriteria));
  m_critMap.insert(ChargedVelo, new MCTrackGeomCriteria(m_chargedVeloCriteria));


  // Calorimeter geometry
  DeCalorimeter* m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  LHCb::CaloCellID refOut = LHCb::CaloCellID(2, 0, 6 ,0);
  LHCb::CaloCellID refInn = LHCb::CaloCellID(2, 2, 25 ,23);
  m_xECALInn = fabs(  m_calo->cellX(refInn)  );
  m_yECALInn = fabs(  m_calo->cellY(refInn)  );
  m_xECALOut = fabs(  m_calo->cellX(refOut)  );
  m_yECALOut = fabs(  m_calo->cellY(refOut)  );

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
  double pz = mcPart->momentum().pz();
  if( pz < 0 )return false;
  pz < 0 ? pz = std::min(pz, -LHCb::Math::lowTolerance) :
               pz = std::max(pz, LHCb::Math::lowTolerance);

  const double sx = mcPart->momentum().px()/pz;
  const double sy = mcPart->momentum().py()/pz;
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

        // n.b the order matters !
        if (m_critMap[ChargedLong]->accepted(mcTkInfo(),mcPart) == true)
          return ChargedLong;
        else if (m_critMap[ChargedUpstream]->accepted(mcTkInfo(),mcPart) == true )
        {
          return ChargedUpstream;
        }
        else if (m_critMap[ChargedDownstream]->accepted(mcTkInfo(),mcPart) == true)
        {
          return ChargedDownstream;
        }
        else if( m_critMap[ChargedVelo]->accepted(mcTkInfo(),mcPart) == true )
        {
          return ChargedVelo;
        }
        else if ( m_critMap[ChargedTtrack]->accepted(mcTkInfo(),mcPart) == true )
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


bool MCReconstructible:: isReconstructibleAs(
        const IMCReconstructible::RecCategory& category,
        const LHCb::MCParticle* mcPart ) const{

  // protect against the strip case
  if (category == NoClassification ){
    Warning("Called with no classification", StatusCode::SUCCESS);
    return false;
  }

  if (!inAcceptance(mcPart)) return false;

  const bool isCharged = mcPart->particleID().threeCharge() != 0;
  if (isCharged == true && category != Neutral && category != NotReconstructible){
    CriteriaMap::iterator criteria = m_critMap.find(category);
    if (criteria == m_critMap.end()) {
      Warning("Category not found - defaulting to false",StatusCode::SUCCESS);
      return false;
    }
    else {
      return criteria->second->accepted(mcTkInfo(),mcPart) && m_mcSel->accept(mcPart);
    }
  }
  else if (isCharged == false && category == Neutral) {
    return true;
  }
  else if (category == NotReconstructible){
    // stupid but true !
    return true;
  }

  return false;
}
