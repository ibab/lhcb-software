// $Id: HltMoveVerticesForSwimming.cpp,v 1.11 2010-08-17 11:26:35 graven Exp $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "boost/foreach.hpp"

// from Event
#include "HltMoveVerticesForSwimming.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltMoveVerticesForSwimming
//
// 2008-02-12 : V. Gligorov
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltMoveVerticesForSwimming );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltMoveVerticesForSwimming::HltMoveVerticesForSwimming( const std::string& name,
                                                        ISvcLocator* pSvcLocator )
    : HltAlgorithm( name, pSvcLocator ), m_selections( *this )
{
    declareProperty( "BlifetimeErr", m_bLifetimeErr = -999999.0 );
    declareProperty( "Blifetime", m_bLifetime = -999999.0 );
    declareProperty( "Bip", m_bIP = -999999.0 );
    declareProperty( "BipChi2", m_bIPChi2 = -999999.0 );
    declareProperty( "BfdChi2", m_bFDChi2 = -999999.0 );
    declareProperty( "Bdira", m_bDIRA = -999999.0 );

    declareProperty( "BadEvent", m_badEvent = false );

    declareProperty( "SwimmingDistance", m_swimmingDistance = 1.0 );
    declareProperty( "Bcontainer", m_Bcontainer = "/Event/SeqD2KK/Phys/SelD2KK" );
    declareProperty( "OfflinePVs", m_offlinePVs = "/Event/Rec/Vertex/Primary" );

    declareProperty( "lifetimeFitter", m_lifeToolName = "PropertimeFitter" );
    declareProperty( "distanceCalculator",
                     m_distToolName = "LoKi::DistanceCalculator" );
    declareProperty( "relatedPVFinder", m_finderToolName = "GenericParticle2PVRelato"
                                                           "r__p2PVWithIPChi2_"
                                                           "OfflineDistanceCalculato"
                                                           "rName_/P2PVWithIPChi2" );
    m_selections.declareProperties();
}

//=============================================================================
// Destructor
//=============================================================================
HltMoveVerticesForSwimming::~HltMoveVerticesForSwimming()
{
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltMoveVerticesForSwimming::initialize()
{

    StatusCode sc = HltAlgorithm::initialize(); // must be executed first
    if ( sc.isFailure() ) return sc; // error printed already by GaudiAlgorithm

    m_selections.retrieveSelections();
    m_selections.registerSelection();

    m_fit = tool<ILifetimeFitter>( m_lifeToolName, this );
    if ( !m_fit ) {
        Error( "Unable to retrieve the ILifetimeFitter tool" );
        return StatusCode::FAILURE;
    }

    m_dist = tool<IDistanceCalculator>( m_distToolName, this );
    if ( !m_dist ) {
        Error( "Unable to retrieve the IDistanceCalculator tool" );
        return StatusCode::FAILURE;
    }

    m_finder = tool<IRelatedPVFinder>( m_finderToolName, this );
    if ( !m_finder ) {
        Error( "Unable to retrieve the IRelatedPVFinder tool" );
        return StatusCode::FAILURE;
    }

    return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltMoveVerticesForSwimming::execute()
{

    StatusCode sc = StatusCode::SUCCESS;

    m_selections.output()->clean(); // TODO: is this really needed?

    // Reinitialize the properties
    m_bLifetimeErr = -999999.0;
    m_bLifetime = -999999.0;
    m_bIP = -999999.0;
    m_bIPChi2 = -999999.0;
    m_bFDChi2 = -999999.0;
    m_bDIRA = -999999.0;
    m_badEvent = false;

    if ( msgLevel( MSG::DEBUG ) )
        debug() << "About to get the offline particles" << endmsg;

    Particle::Range pars =
        getIfExists<Particle::Range>( m_Bcontainer + "/Particles" );

    // Check if particles exist
    if ( !pars ) return sc;

    if ( msgLevel( MSG::DEBUG ) ) {
        verbose() << " particles found " << pars.size() << endmsg;
    }

    if ( msgLevel( MSG::DEBUG ) )
        debug() << "About to check if only one particle in the event" << endmsg;

    // If more than one particle quit!
    if ( pars.size() != 1 ) return sc;
    if ( msgLevel( MSG::DEBUG ) )
        debug() << "About to check if there are any PVs in the event!" << endmsg;
    // If no PVs quit!
    if ( m_selections.input<1>()->empty() ) return sc;

    // Print the online PVs for debug
    if ( msgLevel( MSG::DEBUG ) ) {
        debug() << "Printing out the input vertices" << endmsg;
        BOOST_FOREACH( const LHCb::RecVertex * v, *m_selections.input<1>() )
        {
            debug() << *v << endmsg;
        }
    }

    /*
    debug() << "Check if there are any offline PVs associated to the signal particle"
    << endmsg;
    //If no associated offline PVs quit!
    if (!exist<LHCb::Relation1D<LHCb::Particle,LHCb::VertexBase>
    >(m_Bcontainer+"/Particle2VertexRelations")) return sc;
    //Get the relations
    debug() << "Get the relations table!" << endmsg;
    LHCb::Relation1D<LHCb::Particle,LHCb::VertexBase>* rels =
      get<LHCb::Relation1D<LHCb::Particle,LHCb::VertexBase>
    >(m_Bcontainer+"/Particle2VertexRelations");
    //Get the PV
    debug() << "Get the offline PV associated to the signal" << endmsg;
    LHCb::VertexBase* offPV = rels->relations().begin()->to();
    */

    // Now get the offline PVs
    if ( msgLevel( MSG::DEBUG ) )
        debug() << "About to get the offline PVs" << endmsg;
    const LHCb::RecVertex::Range offPVs =
        getIfExists<LHCb::RecVertex::Range>( m_offlinePVs );

    if ( msgLevel( MSG::DEBUG ) )
        debug() << "About to get the offline PVs" << endmsg;
    if ( !offPVs ) return sc;
    if ( msgLevel( MSG::DEBUG ) )
        debug() << "Offline PVs found, grabbing them" << endmsg;

    // Print the offline PVs for debug
    if ( msgLevel( MSG::DEBUG ) ) {
        debug() << "Printing out the offline vertices" << endmsg;
        BOOST_FOREACH( const LHCb::RecVertex * v, offPVs )
        {
            debug() << *v << endmsg;
        }
    }

    // Safety paranoia check: are there any offline PVs?
    if ( offPVs.size() == 0 ) {
        // There are no offline PVs found in the event which should never happen
        // if it does, for any reason, mark the event as bad and move on
        warning() << "Found an event with no offline PVs, should never happen"
                  << endmsg;
        warning() << "Marking the event as bad and exiting gracefully" << endmsg;
        m_badEvent = true;
        return sc;
    }
    // Now get the best PV for the particle
    if ( msgLevel( MSG::DEBUG ) ) debug() << "About to get the related PV" << endmsg;
    const LHCb::VertexBase* offPV = m_finder->relatedPV(
        pars[0], LHCb::RecVertex::ConstVector( offPVs.begin(), offPVs.end() ) );
    LHCb::VertexBase* offPV_Clone = offPV->clone();

    sc = move_PVs( pars[0], offPV_Clone );

    delete offPV_Clone;

    int ncan = m_selections.output()->size();
    if ( msgLevel( MSG::DEBUG ) ) debug() << " candidates found " << ncan << endmsg;
    return sc;
}
//=============================================================================
StatusCode HltMoveVerticesForSwimming::move_PVs( const LHCb::Particle* myB,
                                                 LHCb::VertexBase* offPV )
{
    // Move the PVs

    StatusCode sc = StatusCode::SUCCESS;

    /*
    double vtxchi2,minvtxchi2 = 999999999.;
    const LHCb::RecVertex* bestVertex;
    //Get the online PV matching best to the offline PV
    BOOST_FOREACH(const LHCb::RecVertex* vertex, *m_selections.input<1>() ) {
      vtxchi2 = sqrt  (
                          pow(vertex->position().x() - offPV->position().x(),2) +
                          pow(vertex->position().y() - offPV->position().y(),2) +
                          pow(vertex->position().z() - offPV->position().z(),2)
                      );
      if (vtxchi2<minvtxchi2) {
          minvtxchi2 = vtxchi2;
          bestVertex = vertex;
      }
    }
    */

    // I know this shouldn't modify the input vertex in the long run,
    // but for now it is easier to do it like this
    BOOST_FOREACH( const LHCb::RecVertex * vertex, *m_selections.input<1>() )
    {
        if ( msgLevel( MSG::DEBUG ) ) {
            debug() << "The primary vertex is at " << vertex << endmsg;
            debug() << "With X coordinate " << vertex->position().X() << endmsg;
            debug() << "With Y coordinate " << vertex->position().Y() << endmsg;
            debug() << "With Z coordinate " << vertex->position().Z() << endmsg;
        }

        const_cast<LHCb::RecVertex*>( vertex )->setPosition(
            vertex->position() + m_swimmingDistance * myB->slopes().Unit() );
        m_selections.output()->push_back( vertex );

        if ( msgLevel( MSG::DEBUG ) ) {
            debug() << "The new vertex" << endmsg;
            debug() << "With X coordinate " << vertex->position().X() << endmsg;
            debug() << "With Y coordinate " << vertex->position().Y() << endmsg;
            debug() << "With Z coordinate " << vertex->position().Z() << endmsg;
        }
    }
    // Also move the clone of the offline PV to compute the new lifetime
    offPV->setPosition( offPV->position() +
                        m_swimmingDistance * myB->slopes().Unit() );

    double pt, ept, chi2, ip, ipchi2, fd, fdchi2 = -99999999.;
    sc = m_fit->fit( *offPV, *myB, pt, ept, chi2 );
    if ( !sc ) {
        warning() << "The lifetime fit failed!!" << endmsg;
    }
    if ( msgLevel( MSG::DEBUG ) )
        debug() << "The lifetime of the signal candidate is now = " << pt << endmsg;
    m_bLifetime = pt;
    m_bLifetimeErr = ept;

    sc = m_dist->distance( myB, offPV, ip, ipchi2 );
    sc = m_dist->distance( offPV, myB->endVertex(), fd, fdchi2 );
    if ( !sc ) {
        warning() << "The distance calculator failed!!" << endmsg;
    }
    if ( msgLevel( MSG::DEBUG ) )
        debug() << "The IP of the signal candidate is now = " << ip << endmsg;
    m_bIP = ip;
    m_bIPChi2 = ipchi2;
    m_bFDChi2 = fdchi2;

    // Now compute the pointing angle as well for good measure!
    const LHCb::Vertex* evtx = myB->endVertex();
    if ( !evtx ) {
        Warning( "Can't retrieve the end vertex of the B!" );
        return sc;
    }
    Gaudi::XYZVector A = myB->momentum().Vect();
    Gaudi::XYZVector B = evtx->position() - offPV->position();
    m_bDIRA = A.Dot( B ) / std::sqrt( A.Mag2() * B.Mag2() );

    return sc;
}
