// Include files
// -------------

// From Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// Local
#include "PatVertexState.h"
#include "PatPV3D.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatPV3D
//
// 2004-02-17 : Eduardo Rodrigues
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PatPV3D );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatPV3D::PatPV3D( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) 
  , m_vertexSearchNotPossible   ( 0 )
  , m_pvSeedTool             ( 0 )
{
  declareProperty( "ZMin",                         m_zMin =   - 250.0 * Gaudi::Units::mm);
  declareProperty( "ZMax",                         m_zMax =     250.0 * Gaudi::Units::mm);
  declareProperty( "VertexHalfWidth",              m_vertexHalfWidth =   3.*Gaudi::Units::mm);
  declareProperty( "DzCloseVertex",                m_dzCloseVertex =    20.*Gaudi::Units::mm);
  declareProperty( "MaxChi2Normalized",            m_chi2Max =  0.3);
  declareProperty( "MinNumberTracksForVertex",     m_minNbTracksVertex =  6 );
  declareProperty( "MinNumberTracksForCloseVertex",m_minNbTracksCloseVertex = 6 );
  declareProperty( "MinNumberBackTracksForVertex", m_minNbBackTracksVertex = 0 );
  declareProperty( "MaxTransDist",                 m_maxTransDistSquared =  10. * Gaudi::Units::mm);
  declareProperty( "MinDistanceBetweenVertices",   m_minDistBetweenVertices =  1.5 * Gaudi::Units::mm);
  declareProperty( "ZErrorMax",                    m_zErrorMax = 1.* Gaudi::Units::mm  );
  declareProperty( "ImproveVertexLocation",        m_improveVertexLocation =  true);
  declareProperty( "Chi2NormalizedCutImproveVertexLocation", m_chi2PerTrackCut =  0.1);
  declareProperty( "ModifyTrackErrors",            m_modifyTrackErrors = true);
  declareProperty( "DetectorResolutionCoeff",      m_detectorResolutionCoeff =15e-3 );
  declareProperty( "MultipleScatteringCoeff",      m_multipleScatteringCoeff =0.0001 ) ;
  declareProperty( "SaveSeedsAsPV",                m_saveSeedsAsPV =  false);
  declareProperty( "x0MS",                         m_x0MS          =  0.01          );
  declareProperty( "InputTracksName"    , m_inputTracksName     =  LHCb::TrackLocation::Velo);
  declareProperty( "OutputVerticesName" , m_outputVerticesName  =  LHCb::RecVertexLocation::Velo3D );

}

//=============================================================================
// Destructor
//=============================================================================
PatPV3D::~PatPV3D() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PatPV3D::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (!sc) return sc;

  debug() << "==> Initialise" << endmsg;

  
  info() << "========= Algorithm Parameters ============"    << endmsg
         << " ZMin                           = " << m_zMin << " mm" << endmsg
         << " ZMax                           = " << m_zMax << " mm" << endmsg
         << " VertexHalfWidth                = " << m_vertexHalfWidth << " mm" << endmsg
         << " MaxChi2Normalized              = " << m_chi2Max << endmsg
         << " MinNbTracksVertex              = " << m_minNbTracksVertex << endmsg
         << " MinNbBackTracksVertex          = " << m_minNbBackTracksVertex << endmsg
         << " MaxTransDist                   = " << m_maxTransDistSquared << endmsg
         << " MinDistBetweenVertices         = " << m_minDistBetweenVertices << endmsg
         << " ZErrorMax                      = " << m_zErrorMax << endmsg
         << " ImproveVertexLocation          = " << m_improveVertexLocation << endmsg
         << " Chi2PerTrackCut                = " << m_chi2PerTrackCut << endmsg
         << " ModifyTrackErrors              = " << m_modifyTrackErrors << endmsg
         << " DetectorResolutionCoeff        = " << m_detectorResolutionCoeff << endmsg
         << " MultipleScatteringCoeff        = " << m_multipleScatteringCoeff << endmsg
         << "===========================================" << endmsg;

  // -> square as errors are quadratically summed
  m_detectorResolutionCoeff *= m_detectorResolutionCoeff;
  m_multipleScatteringCoeff *= m_multipleScatteringCoeff;

  m_maxTransDistSquared *= m_maxTransDistSquared;

  // Access PVSeedTool
  m_pvSeedTool = tool<PVSeedTool>( "PVSeedTool", this );
  if( !m_pvSeedTool ) {
    err() << "Unable to retrieve the PVSeedTool" << endmsg;
    return  StatusCode::FAILURE;
  }

  //== Get Velo detector element, to get the R sector angle
  m_velo = getDet<DeVelo>( DeVeloLocation::Default );

  m_vclusters.reserve(500);
  m_lotravec.reserve(500);

  double X0 = m_x0MS;
  m_scatCons = (13.6*sqrt(X0)*(1.+0.038*log(X0)));


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatPV3D::execute() {

  MsgStream msg(msgSvc(), name());
  bool isDebug   = msg.level() <= MSG::DEBUG;

  if ( isDebug ) {
    debug() << "==> Execute" << endmsg;
  }

  m_inputTracks   = get<LHCb::Tracks>( m_inputTracksName );
  m_outputVertices  = new LHCb::RecVertices();
  put(m_outputVertices, m_outputVerticesName);

  m_vclusters.clear();
  m_lotravec.clear();

  ////////////////////////////////////
  // Flag tracks
  ////////////////////////////////////

  StatusCode sc = flagTracks();
  if (sc != StatusCode::SUCCESS) return sc;

  /////////////////////////////////
  // Get zseeds of PV candidates //
  /////////////////////////////////

  std::vector<double> zseeds;
  sc = searchVertexSeeds(zseeds);
  if (sc != StatusCode::SUCCESS) return sc;  

  // to minitor quality of zseeds only.
  if(m_saveSeedsAsPV) {
    storeDummyVertices(zseeds);
    return StatusCode::SUCCESS;
  }

  /////////////////////
  // Search vertices //
  /////////////////////

  PatVertexStates myTrgVertices;
  sc = searchVertices(zseeds, myTrgVertices);
  if (sc != StatusCode::SUCCESS) return sc;

  ////////////////////
  // Clean vertices //
  ////////////////////

  sc = cleanVertices(myTrgVertices);
  if (!sc) return sc;

  //////////////////////
  // Improve vertices //
  //////////////////////

  if( m_improveVertexLocation ) {    
    sc = improveVertices(m_chi2PerTrackCut, myTrgVertices);
    if (sc != StatusCode::SUCCESS) return sc;
  }

  ////////////////////////
  // Store the vertices //
  ////////////////////////

  sc = storeVertices(myTrgVertices);
  if (sc != StatusCode::SUCCESS) return sc;
  
  setFilterPassed(!myTrgVertices.empty());

  return StatusCode::SUCCESS;
}
//=============================================================================
// Flag Tracks
//=============================================================================
StatusCode PatPV3D::flagTracks() {

  // Flag tracks outside the z-range for vertices
  double nbTracksInsideZRange = 0;
  for ( LHCb::Tracks::const_iterator pItr = m_inputTracks->begin(); 
        m_inputTracks->end() != pItr; pItr++ ) {
    LHCb::Track* track = (*pItr);
    if ( track->checkFlag( LHCb::Track::Invalid  ) ) continue;

    double z = track -> firstState().z();

    double transDistSquared = 
      track->firstState().x() * track->firstState().x() +
      track->firstState().y() * track->firstState().y();

    if( z > m_zMax || z < m_zMin || 
        transDistSquared > m_maxTransDistSquared ) {
      continue;  // Not to be used...
    } else {
      localTrack ltr;
      ltr.pTr3d = track;
      ltr.vertexNb = 0;
      ltr.x  = track->firstState().x();
      ltr.y  = track->firstState().y();
      ltr.z  = track->firstState().z();
      ltr.tx = track->firstState().tx();
      ltr.ty = track->firstState().ty();

      ltr.errX2 =  track->firstState().errX2();
      ltr.errY2 =  track->firstState().errY2();

      // use special parametrization of tracks errors if required
      if( m_modifyTrackErrors ) {
        double ex2 = 0.;
        double ey2 = 0.;
        StatusCode sc = recalculateTrackError(track, ex2, ey2);
        if (sc == StatusCode::SUCCESS ) {
           ltr.errX2 =  ex2;
           ltr.errY2 =  ey2;
	}   
      }

      ltr.backward = false;
      if (track->checkFlag( LHCb::Track::Backward ) ) ltr.backward = true;

      m_lotravec.push_back(ltr);
      nbTracksInsideZRange++;

      if( msgLevel( MSG::DEBUG ) ) {
        debug() << "Local track x,y,z,ex,ey " 
                << ltr.x << " " << ltr.y << " " << ltr.z << " | " 
                << ltr.tx << " " << ltr.ty << " | "  
                << ltr.errX2 << " " << ltr.errY2 << " "  
                << endmsg;
      }
    }
  }

  if( 2 > nbTracksInsideZRange ) {
    if( msgLevel( MSG::DEBUG ) ) {
      debug() << "Not enough VELO tracks available for vertexing" << endmsg;
    }    
    m_vertexSearchNotPossible ++;
  }


  if ( 0 < m_inputTracks->size() && msgLevel(MSG::DEBUG) ) debug() 
    << "Fraction of VELO Tracks kept for vertexing: "
    << nbTracksInsideZRange << "/" << m_inputTracks->size() << "("
    << nbTracksInsideZRange / m_inputTracks->size() * 100. << "%)"
    << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
//   recalculateTrackErrors
//=============================================================================
StatusCode PatPV3D::recalculateTrackError(LHCb::Track* track, double &ex2, double &ey2) {

    double z = track->firstState().z();

    double dist, distVELO;
    int nbSta;
    computeErrorParameters( track, z, dist, nbSta, distVELO );

    double distRatio = dist / distVELO;

    double slX = track->firstState().tx();
    double slY = track->firstState().ty();

    double tanTheta = slX * slX + slY * slY;

    double cosTheta = 1 / sqrt( 1 + tanTheta );

    // Error coefficients tuned by hand
    double detectorResolution = m_detectorResolutionCoeff * 
      ( distRatio * distRatio + ( 1 + distRatio ) * ( 1 + distRatio ) );
      
    // The sinTheta dependence does not appear
    // => p used instead of pt
    // Reason: difficulty to get good vertex resolution with pt
    double multipleScattering = m_multipleScatteringCoeff * 
      nbSta / cosTheta * dist * dist / cosTheta / cosTheta;
      
    ex2 = track->firstState().errX2() + detectorResolution + multipleScattering;
      
    ey2 = track->firstState().errY2() + detectorResolution + multipleScattering;

    return StatusCode::SUCCESS;
}

//=============================================================================
// Search Vertices
//=============================================================================
StatusCode PatPV3D::searchVertexSeeds(std::vector<double> &zseeds) {

  
  // create clusters; initial cluster = track
  std::vector<localTrack>::iterator ilot;

  for ( ilot = m_lotravec.begin(); m_lotravec.end() != ilot; ++ilot ) {

    if( 0 != ilot->vertexNb ) continue;

    // calculate errors for cluster finding: z and sigmaz**2
    
    double sigsq; 
    double z = ilot->z;     
    m_pvSeedTool->errorForPVSeedFinding(ilot->tx, ilot->ty, sigsq);

    vtxCluster clu;
    clu.z = z;
    clu.sigsq = sigsq;
    clu.sigsqmin = clu.sigsq;
    clu.ntracks = 1;
    m_vclusters.push_back(clu);

  }

  // find PV z seeds 
  // zseeds come back sorted according to multiplicity (high mult first)

  if( msgLevel( MSG::DEBUG ) ) {
    debug() << m_vclusters.size() << " 3D tracks used for seeding" << endmsg;
  }
  m_pvSeedTool->findClusters(m_vclusters, zseeds);
    
  if( msgLevel( MSG::DEBUG ) ) {
    debug() << zseeds.size() << " z seeds found: " << zseeds << endmsg;
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Search Vertices
//=============================================================================
StatusCode PatPV3D::searchVertices(std::vector<double> &zseeds, PatVertexStates& myTrgVertices) {

  
  unsigned int vertexID = 0;

  std::vector<double>::iterator itzseed;
  for (itzseed = zseeds.begin(); itzseed != zseeds.end(); itzseed++) {

    std::vector<localTrack*> content;

    double zseed = *itzseed;

    PatVertexState* currentVertex = new PatVertexState();

    if( msgLevel( MSG::DEBUG ) ) {
      verbose() << "   - new PatVertexState created: ID = "
                << vertexID << endmsg;
    }

    vertexID++;
    currentVertex->setVertexID( vertexID );

    int count = 0;


    std::vector<localTrack>::iterator ilot;

    for ( ilot = m_lotravec.begin(); m_lotravec.end() != ilot; ++ilot ) {
      if( 0 != ilot->vertexNb ) continue;

      double zTr = ilot->z;
      double dz = fabs(zTr-zseed);
      double slX = ilot->tx;
      double slY = ilot->ty;
      double tanTheta2 = slX * slX + slY * slY;
      double errz2tr = (ilot->errX2 + ilot->errY2) / tanTheta2;
      
      double chi2tr = dz*dz/errz2tr;
      double chi2trcut = 6.*6.; // with some safety margin
     
      //      if( (dz<m_vertexHalfWidth && chi2tr < chi2trcut) || (dz<0.5*Gaudi::Units::mm) ) {
      if( dz<m_vertexHalfWidth && chi2tr < chi2trcut) {
        localTrack* plotra = &(*ilot);
        currentVertex->addTrackInTrgVertex( plotra );
        content.push_back(plotra);
        if( msgLevel( MSG::DEBUG ) ) {
          verbose() << "     - added trk # " << ++count << endmsg;
        }
      }      

    }

    int nbTracksInVertex = currentVertex->nbTracksInTrgVertex();
    int nbBackTracksInVertex = currentVertex->nbBackTracksInTrgVertex();

    if( msgLevel( MSG::DEBUG ) ) {
      debug() << vertexID << " : # tracks in vertex = " << nbTracksInVertex
              << ", with " 
              << nbBackTracksInVertex << " backward " << endmsg;
    }

    if( (nbTracksInVertex < m_minNbTracksVertex) || 
        (nbBackTracksInVertex < m_minNbBackTracksVertex) ) {
      // bad candidate for vertex. Reset tracks and delete PatVertexState

      std::vector<localTrack*>::iterator pilo;
      for( pilo  = content.begin() ; pilo != content.end() ; pilo++ ) {
        if( vertexID == (*pilo)->vertexNb ) (*pilo)->vertexNb = 0;
      }
      if( msgLevel( MSG::DEBUG ) ) {
        debug() << vertexID << " : Too small vertex " << nbTracksInVertex<< " < "
                << m_minNbTracksVertex << " , " << nbBackTracksInVertex<< endmsg;
      }      
      if(currentVertex) {
        delete(currentVertex);
      } else {
        warning() << "Inconsitency. Pointer to be deleted is 0!" << endmsg;
      }
      continue;
    }

    currentVertex->computeTrgVertex();

    double chi2Max;
    localTrack* trackMaxChi2;

    currentVertex->computeChi2( chi2Max, trackMaxChi2 );

    if( msgLevel( MSG::DEBUG ) ) {
      debug() << vertexID << " : chi2 = " << currentVertex->chi2() << endmsg;
      debug() << vertexID << " : position = " << currentVertex->position() << endmsg;
    }    

    // check if isolated and set stronger chi2cut if not

    bool closevtx = false;
    for( PatVertexStates::iterator trgVtxIt = myTrgVertices.begin() ;         
         trgVtxIt != myTrgVertices.end() ; trgVtxIt ++ ) {

      PatVertexState* tvtx = *trgVtxIt;
      if( tvtx->isUseful() ) {
        double dz = fabs( currentVertex->position().z() - tvtx->position().z() );
        if ( dz < m_dzCloseVertex ) closevtx = true;
      }
    }

    int minNbTracks = m_minNbTracksVertex;
    if (closevtx)  minNbTracks = m_minNbTracksCloseVertex;

    double chi2cut = m_chi2Max;

    while( chi2Max > chi2cut && 
           minNbTracks < currentVertex->nbTracksInTrgVertex() &&
           m_minNbBackTracksVertex <= currentVertex->nbBackTracksInTrgVertex()) {

      currentVertex->rejectTrack( trackMaxChi2 );
      currentVertex->computeTrgVertex();
      currentVertex->computeChi2( chi2Max, trackMaxChi2 );

    }

    if( msgLevel( MSG::DEBUG ) ) {
      debug() << vertexID << " : after rejection = " 
              << currentVertex->nbTracksInTrgVertex() << ", "
              << currentVertex->nbBackTracksInTrgVertex() << endmsg; 

      debug() << vertexID << " chi2Max vtx  -  chi2Max cut:  " << chi2Max
              << " - " << chi2cut << endmsg;

      if( chi2Max > chi2cut || minNbTracks > currentVertex->nbTracksInTrgVertex()) {
        std::vector<localTrack*>::iterator pilo;
        for( pilo  = content.begin() ; 
             pilo != content.end() ; pilo++ ) {
          if( vertexID == (*pilo)->vertexNb ) (*pilo)->vertexNb=0;
      }
        debug() << vertexID << " Not good enough vertex" << endmsg; 
        continue;
      }
    
      debug() << vertexID << " pushing back " 
              << currentVertex->position() << endmsg ;
    }
    
    myTrgVertices.push_back( currentVertex );
  }

  return StatusCode::SUCCESS;

}
//=============================================================================
// Clean Vertices
//=============================================================================
StatusCode PatPV3D::cleanVertices(PatVertexStates& myTrgVertices) {
  
  if( msgLevel( MSG::DEBUG ) ) {
    debug() << "Clean vertices" << endmsg;
  }

  //  detect and glue two close vertices
  bool check_close_vertices = true;
  while( check_close_vertices ) {
    check_close_vertices = false;
    for( PatVertexStates::iterator trgVtxIt = myTrgVertices.begin() ;
         trgVtxIt != myTrgVertices.end() ; trgVtxIt ++ ) {
      PatVertexState* first = *trgVtxIt;
      if( ! first->isUseful() ) continue;
      for( PatVertexStates::iterator trgVtxIt2  = trgVtxIt + 1 ;
           trgVtxIt2 != myTrgVertices.end() ; trgVtxIt2 ++ ) {
        PatVertexState* second = *trgVtxIt2;
        if( ! first->isUseful() ||
            ! second->isUseful() ) continue;
  
        if( msgLevel( MSG::DEBUG ) ) {
          debug() <<  first->position().z() << " " 
                  << second->position().z() << " z1 z2 " << endmsg;
        }

        PatVertexState* vgood = 0;
        PatVertexState* vbad = 0;
  
        if( fabs( second->position().z() - first->position().z() )
            < m_minDistBetweenVertices ) {
  
  	// check if close vertices looking at xy distance
  
          double errsq_x1 = first->getEstimatedErrorX2();
          double errsq_y1 = first->getEstimatedErrorY2();
          double errsq_z1 = first->getEstimatedErrorZ2();
  
          double errsq_x2 = second->getEstimatedErrorX2();
          double errsq_y2 = second->getEstimatedErrorY2();
          double errsq_z2 = second->getEstimatedErrorZ2();
  
          double dx = fabs( second->position().x() - first->position().x() );
          double dy = fabs( second->position().y() - first->position().y() );
          double dz = fabs( second->position().z() - first->position().z() );
  
          double chi2_x = dx*dx/(errsq_x1+errsq_x2);
          double chi2_y = dy*dy/(errsq_y1+errsq_y2);
          double chi2_z = dz*dz/(errsq_z1+errsq_z2);
  
  	  // definition of close vertices
          if( msgLevel( MSG::DEBUG ) ) {
            debug() << chi2_x << " " << chi2_y << " " 
                    << chi2_z << " chi2 x y z" << endmsg;
          }
          if( (chi2_z<25. && chi2_x<9. && chi2_y<9.) || chi2_z<9. || dz < 0.5*Gaudi::Units::mm) {
            check_close_vertices = true;
            vgood = first;
            vbad = second;
            if(first->nbTracksInTrgVertex() < second->nbTracksInTrgVertex() ) {
              vgood = second;
              vbad = first;
            }  
        
            if( msgLevel( MSG::DEBUG ) ) {
              debug() << "Too close vertices at " << second->position().z() 
                      << " and " << first->position().z() << endmsg;
            }

            std::vector<localTrack*>* content = vbad->trgVertexContent();
            unsigned int vertexID = vbad->vertexID();
            std::vector<localTrack*>::iterator it;
            int nbAddedTracks = 0;
            for( it  = content->begin() ;
                 it != content->end() ; it ++ ) {
              if( vertexID == (*it)->vertexNb ) {
                nbAddedTracks ++;
                vgood->addTrackInTrgVertex( (*it) );
              }
            }
            vgood->computeTrgVertex();
            double chi2Max;
            localTrack* trackMaxChi2;
            vgood->computeChi2( chi2Max, trackMaxChi2 );
            vbad->setUsefulness( false );
          }
        } 
      }
    }
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
// Improve Vertices
//=============================================================================
StatusCode PatPV3D::improveVertices(double chi2PerTrack, 
                                                PatVertexStates& myTrgVertices) {
  if( msgLevel( MSG::DEBUG ) ) {
    debug() << "Improve vertices" << endmsg;
  }
  for( PatVertexStates::iterator trgVtxIt = myTrgVertices.begin() ;
       trgVtxIt != myTrgVertices.end() ; trgVtxIt ++ ) {
    PatVertexState* currentVertex = *trgVtxIt;
    double chi2Max;
    localTrack* trackMaxChi2;
      
    if( ! currentVertex->isUseful() ) continue;
      
    currentVertex->computeChi2( chi2Max, trackMaxChi2 );
    while ( m_minNbTracksVertex < currentVertex->nbTracksInTrgVertex() &&  
            m_minNbBackTracksVertex < currentVertex->nbBackTracksInTrgVertex() &&
            chi2Max > chi2PerTrack ) {
      currentVertex->rejectTrack( trackMaxChi2 );
      currentVertex->computeTrgVertex();
      currentVertex->computeChi2( chi2Max, trackMaxChi2 );
    }
    if( msgLevel( MSG::DEBUG ) ) {
      debug() << "Improved vertex at " << currentVertex->position() 
              << ", chi2= " << chi2Max << endmsg ;
    } 
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
// Store vertices
//=============================================================================
StatusCode PatPV3D::storeVertices(PatVertexStates& myTrgVertices) {

  if( msgLevel( MSG::DEBUG ) ) debug() << "Sort vertices" << endmsg;
  
  std::sort( myTrgVertices.begin(), myTrgVertices.end(), 
             PatVertexState::sortTrgVertexByTrackNum() );

  ////////////////////////
  // Store the vertices //
  ////////////////////////

  if( msgLevel( MSG::DEBUG ) ) debug() << "Store vertices" << endmsg;
  int nbTrgVertices = 0;

  for( PatVertexStates::iterator trgVtxIt = myTrgVertices.begin() ;
       trgVtxIt != myTrgVertices.end() ; trgVtxIt ++ ) {
    if( !(*trgVtxIt)->isUseful() )
      continue;
    nbTrgVertices ++;    
  }
  
  if( msgLevel( MSG::DEBUG ) ) {
    debug() << "Number of Trg vertices found = " 
            << nbTrgVertices << endmsg;
  }

  for( PatVertexStates::iterator trgVtxIt = myTrgVertices.begin() ;
       trgVtxIt != myTrgVertices.end() ;  trgVtxIt ++ ) {
    PatVertexState* myTrgVertex = *trgVtxIt;
    if( ! myTrgVertex->isUseful() ) continue;
    LHCb::RecVertex* tVertex = new LHCb::RecVertex();
    tVertex -> clearTracks();
    tVertex -> setPosition( myTrgVertex->position() );
    tVertex -> setCovMatrix( *(myTrgVertex->getErrorMatrix()) );
    std::vector<localTrack*>* trks = myTrgVertex -> trgVertexContent();
    std::vector<localTrack*>::iterator itT;
    for ( itT = trks->begin();
          trks->end() != itT; itT++ ) {
      LHCb::Track* trk = (*itT)->pTr3d;
      tVertex -> addToTracks ( trk );
    }
    tVertex->setNDoF( (int) 2 * myTrgVertex->nbTracksUsedForVertexing() -3 );
    tVertex -> setChi2( myTrgVertex->chi2() );
    tVertex->setTechnique(LHCb::RecVertex::Primary);
    m_outputVertices->insert(tVertex);
    
  }

  // delete PatVertexStates
  for( PatVertexStates::iterator trgVtxIt = myTrgVertices.begin() ;
       trgVtxIt != myTrgVertices.end() ;  trgVtxIt ++ ) {
    delete (*trgVtxIt);
  }

  return StatusCode::SUCCESS;
}


void PatPV3D::computeErrorParameters( LHCb::Track* track, double z,
                                      double & minZDistToVELOCluster,
                                      int & nbStaBeforeLastVELOCluster,
                                      double & distFirstLastVELOCluster ) {

    double minzDist = 10000;

    int nbFirstSta = 84;
    int nbLastSta  = 0;

    double zMax = - 100000;
    double zMin = 100000;

    std::vector<LHCb::LHCbID>::const_iterator it;


    for ( it = track->lhcbIDs().begin();
          track->lhcbIDs().end() != it; it++ ) {
      if ( ! it->isVelo() ) continue;
      LHCb::VeloChannelID id      = (*it).veloID();
      int           sensor  = id.sensor();
      double        zSensor = m_velo -> sensor(id)->z();
      double        dist    = fabs( zSensor - z );

      if( dist < minzDist )
        minzDist = dist;

      if ( sensor < nbFirstSta )
        nbFirstSta = sensor;
      else if ( sensor > nbLastSta )
        nbLastSta = sensor;

      if ( zSensor > zMax )
        zMax = zSensor;
      else if ( zSensor < zMin )
        zMin = zSensor;
    }

    minZDistToVELOCluster      = minzDist;
    nbStaBeforeLastVELOCluster = int(float(nbLastSta-nbFirstSta)/4.);
    distFirstLastVELOCluster   = zMax - zMin;
  }


void PatPV3D:: storeDummyVertices(std::vector<double> &zseeds) {

  std::vector<double>::iterator itzseed;
  for (itzseed = zseeds.begin(); itzseed != zseeds.end(); itzseed++) {
    LHCb::RecVertex* tVertex = new LHCb::RecVertex();
    tVertex -> clearTracks();
    
    Gaudi::XYZPoint xyz(0.,0.,*itzseed);
    tVertex -> setPosition( xyz );
    Gaudi::SymMatrix3x3 errMat;
    tVertex -> setCovMatrix(errMat);
    tVertex->setNDoF(1);
    tVertex -> setChi2(99999.);
    std::vector<localTrack>::iterator itT;
    for ( itT = m_lotravec.begin(); m_lotravec.end() != itT; itT++ ) {
      if(fabs(itT->z - *itzseed)<3.*Gaudi::Units::mm) {
        LHCb::Track* trk = itT->pTr3d;
        tVertex -> addToTracks ( trk );
      }
    }
    tVertex->setTechnique(LHCb::RecVertex::Primary);
    m_outputVertices->insert(tVertex);
  }
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatPV3D::finalize() {

  debug() << "==> Finalize" << endmsg;

  info() << "vertexSearchNotPossible = " 
         << m_vertexSearchNotPossible << endmsg;
  
  return GaudiAlgorithm::finalize();

}
