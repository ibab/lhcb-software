//  ===========================================================================
#define GIGACNV_GiGaRichSegmentCnv_CPP 1
// ============================================================================

// local
#include "GiGaMCRichSegmentCnv.h"

// ======================================================================

static const  CnvFactory<GiGaRichSegmentCnv>         s_Factory ;
const        ICnvFactory&GiGaRichSegmentCnvFactory = s_Factory ;

// ========================================================================

// Containers and orting functions
namespace GiGaRich {

  struct HepPoint3DSortByZ
  {
    bool operator() ( const HepPoint3D & p1, const HepPoint3D & p2 ) const
    {
      return ( p1.z() < p2.z() );
    }
  };
  typedef std::set<HepPoint3D,GiGaRich::HepPoint3DSortByZ> TrajPoints;

  typedef std::pair<HepVector3D,HepPoint3D> MomentumAtZ;
  struct SortMomentumAtZ
  {
    bool operator() ( const MomentumAtZ & p1, const MomentumAtZ & p2 ) const
    {
      return ( p1.second.z() < p2.second.z() );
    }
  };
  typedef std::set<MomentumAtZ,GiGaRich::SortMomentumAtZ> MomentaAtZ;

}

// ======================================================================

GiGaRichSegmentCnv::GiGaRichSegmentCnv( ISvcLocator* Locator )
  : GiGaCnvBase( storageType() , classID() , Locator ),
    m_RichG4HitCollectionName(0) {

  setNameOfGiGaConversionService( IGiGaCnvSvcLocation::Hits ) ;
  setConverterName              ( "GiGaMCRichSegmentCnv" ) ;

  GiGaLeaf::Pars pars1;
  pars1.push_back("");
  declareObject( GiGaLeaf(MCRichSegmentLocation::Default, objType(), pars1) );

  m_RichG4HitCollectionName = new RichG4HitCollName();

};

// ======================================================================

GiGaRichSegmentCnv::~GiGaRichSegmentCnv()
{
  delete m_RichG4HitCollectionName;
};

// ======================================================================

const CLID& GiGaRichSegmentCnv::classID()
{
  return MCRichSegment::classID();
}

// ======================================================================

const unsigned char GiGaRichSegmentCnv::storageType()
{
  return GiGaHits_StorageType;
}

// ======================================================================

StatusCode GiGaRichSegmentCnv::initialize() {

  // initialize the base class
  StatusCode sc = GiGaCnvBase::initialize();
  if ( sc.isFailure() ) return Error("Could not initialize the base class!",sc);

  // check for necessary services
  if ( 0 == hitsSvc() ) return Error("IGiGaHitsCnvSvc* points to NULL!");

  return StatusCode::SUCCESS;
}

// ======================================================================

StatusCode GiGaRichSegmentCnv::finalize()
{
  return GiGaCnvBase::finalize();
}

// ======================================================================

StatusCode GiGaRichSegmentCnv::createObj( IOpaqueAddress*  address ,
                                          DataObject*&     object  ) {

  object = 0;
  if ( 0 == address ) return Error("IOpaqueAddress* points to NULL!" );

  object = new MCRichSegments();
  StatusCode sc = updateObj( address, object );
  if ( !sc ) {
    if ( 0 != object ) { delete object; object = 0; }
    return Warning( "Could not create and update Object", sc );
  }

  return StatusCode::SUCCESS;
};

// ============================================================================

StatusCode GiGaRichSegmentCnv::updateObj ( IOpaqueAddress*  address ,
                                           DataObject*      object  ) {

  if ( 0 == address ) { return Error(" IOpaqueAddress* points to NULL"); }
  if ( 0 == object  ) { return Error(" DataObject* points to NULL"    ); }
  MCRichSegments * segments = dynamic_cast<MCRichSegments*> ( object );
  if ( 0 == segments ) {
    return Error( " DataObject*(of type '" + GiGaUtil::ObjTypeName(object) +
                  "*') is not 'MCRichSegments*'! "   );
  }

  // Initialise
  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Creating MCRichSegments" << endreq;
  segments->clear();

  // Locate the MCRichOpticalPhotons
  SmartDataPtr<MCRichOpticalPhotons> photons( evtSvc(),
                                              MCRichOpticalPhotonLocation::Default ) ;
  if ( !photons ) {
    return Warning( "Could not locate MCRichOpticalPhotons from GiGaRichSegmentCnv" );
  }

  // get the registry
  const IRegistry* registry = address->registry();
  if ( 0 == registry ) { return Error("IRegistry* points to NULL!"); }
  const std::string & location = registry->identifier();
  //  { // try to locate MCParticles explicitely
  // just to force the loading of the reference table
  std::string mcpath = ( location.substr( 0, location.find( "/MC" ) ) + "/" +
                         MCParticleLocation::Default );
  // get MCparticles
  const MCParticles* mcps = get( dataProvider() , mcpath , mcps );
  if ( 0 == mcps ) {
    return Error("Can not locate MCparticles at '" + mcpath + "'");
  }

  G4HCofThisEvent* hitscollections = 0 ;

  try {

    // retrieve the hits container from GiGa Service

    // get hitscollections from GiGa

    *gigaSvc() >> hitscollections;

    if ( 0 != hitscollections ) {

      // map of vectors to store Hits associated to each MCParticle
      typedef std::pair< const MCParticle*, Rich::RadiatorType> HitDataListKey;
      typedef std::pair< const RichG4Hit*, int > HitDataType;
      typedef std::vector< HitDataType > G4HitList;
      typedef std::map< HitDataListKey, G4HitList > HitDataList;
      HitDataList sortedHits;

      // note this key is need for consistency with MCRichHit converter
      int globalKey = 0;
      for ( int iii=0; iii<m_RichG4HitCollectionName->RichHCSize(); ++iii ) {
        std::string colName = m_RichG4HitCollectionName->RichHCName(iii);

        G4SDManager * fSDM = G4SDManager::GetSDMpointer();
        if ( !fSDM ) return Error( "NULL G4SDManager pointer !!" );
        int collectionID = fSDM->GetCollectionID(colName);
        if ( -1 == collectionID ) return StatusCode::SUCCESS;

        RichG4HitsCollection * myCollection =
          dynamic_cast<RichG4HitsCollection*>(hitscollections->GetHC(collectionID));
        if ( 0 == myCollection ) return StatusCode::SUCCESS;

        // Reference table between G4 tracks/trajectories and MC particles
        const GiGaKineRefTable & table = hitsSvc()->table();

        // loop over hits and sort them
        for ( int ihit = 0; ihit < myCollection->entries(); ++ihit ) {

          // Pointer to G4 hit
          RichG4Hit * g4hit = (*myCollection)[ihit];

          if ( g4hit->GetRadiatorNumber() >= 0 ) {
            Rich::RadiatorType rad =
              static_cast<Rich::RadiatorType>(g4hit->GetRadiatorNumber());

            // get MCParticle information
            const int traid = g4hit->GetTrackID();
            const MCParticle * mcPart = table[traid].particle();
            if ( !mcPart ) {
              msg << MSG::WARNING
                  << "No pointer to MCParticle for "
                  << " MCRichHit associated to trackID: "
                  << iii << "  " << ihit << "   " << traid << endreq;
              continue;
            }

            // add to sorted list
            HitDataListKey key(mcPart,rad);
            HitDataType data(g4hit,globalKey);
            sortedHits[key].push_back( data );

          } else {
            msg << MSG::DEBUG
                << "RichG4Hit " << ihit << " has no radiator information !"
                << endreq;
          }

          ++globalKey;
        } // end loop over this collection

      } // end loop over all collections

      // loop over each set of hits associated to a single MCParticle and radiator
      for ( HitDataList::const_iterator iList = sortedHits.begin();
            iList != sortedHits.end(); ++iList ) {
        const MCParticle * mcPart    = ((*iList).first).first;
        const Rich::RadiatorType rad = ((*iList).first).second;
        const G4HitList & g4hitList  = (*iList).second;

        // New MCRichSegment
        MCRichSegment * mcSeg = new MCRichSegment();
        segments->insert( mcSeg );

        // set information
        mcSeg->setRadiator( rad );
        mcSeg->setRich( Rich::CF4 == rad ? Rich::Rich2 : Rich::Rich1 );
        mcSeg->setMcParticle( mcPart );

        msg << MSG::DEBUG
            << "Creating MCRichSegment " << mcSeg->key()
            << " for MCParticle " << mcPart->key()
            << " in " << rad << endreq;

        // loop over hits
        GiGaRich::TrajPoints trajPoints;
        GiGaRich::MomentaAtZ momenta;
        for ( G4HitList::const_iterator iHit = g4hitList.begin();
              iHit != g4hitList.end(); ++iHit ) {

          msg << MSG::VERBOSE
              << " Hit : Prod-pnt  " << (HepPoint3D)(*iHit).first->GetPhotEmisPt() << endreq
              << "     : Pre-step  " << (HepPoint3D)(*iHit).first->ChTrackCkvPreStepPos() << endreq
              << "     : post-step " << (HepPoint3D)(*iHit).first->ChTrackCkvPostStepPos() << endreq
              << "     : tk Mon.   " << (HepVector3D)(*iHit).first->ChTrackMomVect() << endreq;

          // Add step points to collection of trajectory points
          trajPoints.insert( (*iHit).first->ChTrackCkvPreStepPos()  );
          trajPoints.insert( (*iHit).first->ChTrackCkvPostStepPos() );

          // Add momentum to list of momenta
          momenta.insert( GiGaRich::MomentumAtZ( (*iHit).first->ChTrackMomVect(),
                                                 (*iHit).first->ChTrackCkvPreStepPos() )  );
          momenta.insert( GiGaRich::MomentumAtZ( (*iHit).first->ChTrackMomVect(),
                                                 (*iHit).first->ChTrackCkvPostStepPos() ) );

          // Add associated MCRichOpticalPhoton to this segment
          mcSeg->addToMCRichOpticalPhotons( photons->object((*iHit).second) );

        }

        // Finally, loop over sorted trajectory points and add to segment in order
        msg << MSG::DEBUG 
            << " Found " << trajPoints.size() << " trajectory points" << endreq;
        //double lastZ = 999999;
        for ( GiGaRich::TrajPoints::const_iterator iPoint = trajPoints.begin();
              iPoint != trajPoints.end(); ++iPoint ) {
          //if ( fabs(lastZ-(*iPoint).z()) > 1e-2 ) {
            mcSeg->trajectoryPoints().push_back( (*iPoint) );
            msg << MSG::DEBUG << "  Added trajectory point    " << *iPoint << endreq;
            // lastZ = (*iPoint).z();
            //} else { 
            //msg << MSG::DEBUG << "  Rejected trajectory point " << *iPoint << endreq;
            //}
        }
        msg << MSG::DEBUG 
            << " Found " << momenta.size() << " momentum points" << endreq;
        for ( GiGaRich::MomentaAtZ::const_iterator iMom = momenta.begin();
              iMom != momenta.end(); ++iMom ) {
          mcSeg->trajectoryMomenta().push_back( (*iMom).first );
          mcSeg->trajectoryMomentaPoints().push_back( (*iMom).second );
          msg << MSG::DEBUG
              << "  Added trajectory momentum " << (*iMom).first 
              << " at " << (*iMom).second << endreq;
        }

      }

    } else {
      msg << MSG::INFO << "No RichG4Hits to be converted" << endreq;
      return StatusCode::SUCCESS;
    }

  } // end try

  catch( const GaudiException& Excp ) {
    segments->clear();
    return Exception( "updateObj: ", Excp ) ;
  }
  catch( const std::exception& Excp ) {
    segments->clear();
    return Exception( "updateObj: ", Excp ) ;
  }
  catch( ... ) {
    segments->clear();
    return Exception( "updateObj: " ) ;
  }

  msg << MSG::DEBUG << "Done." << endreq;
  return StatusCode::SUCCESS;
};



