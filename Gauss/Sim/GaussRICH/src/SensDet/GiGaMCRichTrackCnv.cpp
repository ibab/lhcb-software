
/** @file GiGaRichTrackCnv.cpp
 *
 *  Implementation file for GiGa converter : GiGaRichTrackCnv
 *
 *  CVS History :
 *  $Id: GiGaMCRichTrackCnv.cpp,v 1.4 2004-07-30 14:18:23 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.3  2004/07/30 13:42:14  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-03-29
 */

// ============================================================================
#define GIGACNV_GiGaRichTrackCnv_CPP 1
// ============================================================================

// local
#include "GiGaMCRichTrackCnv.h"

// ======================================================================

static const  CnvFactory<GiGaRichTrackCnv>         s_Factory ;
const        ICnvFactory&GiGaRichTrackCnvFactory = s_Factory ;

// ========================================================================

GiGaRichTrackCnv::GiGaRichTrackCnv( ISvcLocator* Locator )
  : GiGaCnvBase( storageType() , classID() , Locator ) {

  setNameOfGiGaConversionService( IGiGaCnvSvcLocation::Kine );
  setConverterName              ( "GiGaMCRichTrackCnv"      );

  GiGaLeaf::Pars pars1;
  pars1.push_back("");
  declareObject( GiGaLeaf( MCRichTrackLocation::Default, objType(), pars1 ) );

};

// ======================================================================

GiGaRichTrackCnv::~GiGaRichTrackCnv() { }

// ======================================================================

const CLID& GiGaRichTrackCnv::classID()
{
  return MCRichTrack::classID();
}

// ======================================================================

const unsigned char GiGaRichTrackCnv::storageType()
{
  return GiGaKine_StorageType;
}

// ======================================================================

StatusCode GiGaRichTrackCnv::initialize() {

  // initialize the base class
  StatusCode sc = GiGaCnvBase::initialize();
  if ( sc.isFailure() ) return Error("Could not initialize the base class!",sc);

  // check for necessary services
  if( 0 == kineSvc() ) { return Error("IGiGaKineCnvSvc* points to NULL!"); }

  return StatusCode::SUCCESS;
}

// ======================================================================

StatusCode GiGaRichTrackCnv::finalize()
{
  return GiGaCnvBase::finalize();
}

// ======================================================================

StatusCode GiGaRichTrackCnv::createObj( IOpaqueAddress*  address ,
                                        DataObject*&     object  ) {

  object = 0;
  if ( 0 == address ) return Error("IOpaqueAddress* points to NULL!" );

  object = new MCRichTracks();
  StatusCode sc = updateObj( address, object );
  if ( !sc ) {
    if ( 0 != object ) { delete object; object = 0; }
    return Warning( "Could not create and update Object", sc );
  }

  return StatusCode::SUCCESS;
};

// ============================================================================

StatusCode GiGaRichTrackCnv::updateObj ( IOpaqueAddress*  address ,
                                         DataObject*      object  ) {

  if ( 0 == address ) { return Error(" IOpaqueAddress* points to NULL"); }
  if ( 0 == object  ) { return Error(" DataObject* points to NULL"    ); }
  MCRichTracks * richTracks = dynamic_cast<MCRichTracks*> ( object );
  if ( 0 == richTracks ) {
    return Error( " DataObject*(of type '" + GiGaUtil::ObjTypeName(object) +
                  "*') is not 'MCRichTracks*'! "   );
  }

  // Initialise
  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Creating MCRichTracks" << endreq;
  richTracks->clear();

  // make sure MCParticles have been converted
  SmartDataPtr<MCParticles> mcParts( evtSvc(), MCParticleLocation::Default );
  if ( mcParts->empty() ) { return Warning("Empty MCParticles !"); }

  // Locate the MCRichSegments
  SmartDataPtr<MCRichSegments> segments( evtSvc(), MCRichSegmentLocation::Default ) ;

  // get the references between MCParticles and Geant4 TrackIDs
  const GiGaKineRefTable & table = kineSvc()->table();

  // retrieve the  trajectory container from GiGa Service
  G4TrajectoryContainer* trajectories = 0 ;
  try {

    // get trajectories from GiGa
    *gigaSvc() >> trajectories;
    if ( 0 == trajectories ) { return Error("No G4TrajectoryContainer* object is found !"); }

    // loop over trajectories and form links from MCP to trajectories
    typedef std::map<const MCParticle*, const GiGaTrajectory*> MCPartToGiGaTraj;
    MCPartToGiGaTraj mcpToTraj;
    TrajectoryVector * tv = trajectories->GetVector();
    for ( TrajectoryVector::const_iterator iTr = tv->begin();
          tv->end() != iTr; ++iTr ) {
      // get the GiGa trajectory
      const GiGaTrajectory * traj = gigaTrajectory(*iTr);
      if ( !traj ) continue;
      mcpToTraj[table[traj->trackID()].particle()] = traj;
    }

    // map of vectors to store segments associated to each MCParticle
    typedef const MCParticle* HitListKey;
    typedef std::vector<MCRichSegment*> SegmentList;
    typedef std::map< HitListKey, SegmentList > SortedSegments;
    SortedSegments sortedSegs;

    // Iterate over segments and sort according to MCParticle
    for ( MCRichSegments::iterator iSeg = segments->begin();
          iSeg != segments->end(); ++iSeg ) {
      if ( !(*iSeg) ) {
        msg << MSG::WARNING << "Null RichRecSegment pointer" << endreq;
        continue;
      }
      if ( !(*iSeg)->mcParticle() ) {
        msg << MSG::WARNING << "RichRecSegment has null MCParticle pointer" << endreq;
        continue;
      }
      sortedSegs[(*iSeg)->mcParticle()].push_back( *iSeg );
    }

    // Loop over sorted segments
    for ( SortedSegments::iterator iList = sortedSegs.begin();
          iList != sortedSegs.end(); ++iList ) {
      const MCParticle * mcPart = (*iList).first;
      if ( !mcPart ) continue;

      // new MCRichTrack
      MCRichTrack * mcTrack = new MCRichTrack();
      richTracks->insert( mcTrack, mcPart->key() );

      // data
      mcTrack->setMcParticle( mcPart );

      msg << MSG::DEBUG
          << "Creating MCRichTrack for MCParticle " << mcPart->key()
          << endreq;

      // Loop over segments for this track
      for ( SegmentList::iterator iSeg = (*iList).second.begin();
            iSeg != (*iList).second.end(); ++iSeg ) {
        if ( !(*iSeg) ) continue;
        mcTrack->addToMcSegments( *iSeg );
        (*iSeg)->setMCRichTrack( mcTrack );
        msg << MSG::DEBUG << " Adding " << (*iSeg)->radiator()
            << " MCRichSegment " << (*iSeg)->key() << endreq;
      }

      // Locate GiGaTrajectory for this MCParticle
      const GiGaTrajectory * traj = mcpToTraj[mcPart];
      if ( !traj ) {
        msg << MSG::WARNING
            << "Failed to find trajectory for MCParticle " << mcPart->key()
            << endreq;
        continue;
      }

      // Loop over points
      msg << MSG::DEBUG
          << " Total number of trajectory points = " << traj->GetPointEntries()
          << endreq;
      for ( int iPoint = 0; iPoint < traj->GetPointEntries(); ++iPoint ) {

        const G4VTrajectoryPoint * G4point = traj->point(iPoint);
        const HepVector3D & point = G4point->GetPosition();
        msg << MSG::VERBOSE << "  TrajPoint " << iPoint << " "
            << point << endreq;

      }

    }

  } // end try block

  // catch exceptions
  catch( const GaudiException& Excp )
  {
    richTracks->clear();
    return Exception( "updateObj: ", Excp ) ;
  }
  catch( const std::exception& Excp )
  {
    richTracks->clear();
    return Exception( "updateObj: ", Excp ) ;
  }
  catch( ...  )
  {
    richTracks->clear();
    return Exception( "updateObj: " ) ;
  }

  msg << MSG::DEBUG << "Done." << endreq;
  return StatusCode::SUCCESS;
};
