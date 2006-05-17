// $Id: L0CaloCandidatesFromRaw.cpp,v 1.8 2006-05-17 16:11:28 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Event/L0DUBase.h"

// local
#include "L0CaloCandidatesFromRaw.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0CaloCandidatesFromRaw
//
// 2003-12-15 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( L0CaloCandidatesFromRaw );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0CaloCandidatesFromRaw::L0CaloCandidatesFromRaw( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  m_etScale = 20. * Gaudi::Units::MeV;

  declareProperty( "Extension",      m_extension = "" );
}
//=============================================================================
// Destructor
//=============================================================================
L0CaloCandidatesFromRaw::~L0CaloCandidatesFromRaw() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode L0CaloCandidatesFromRaw::initialize() {
 StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  // Retrieve the detector elements

  m_ecal = getDet<DeCalorimeter>(DeCalorimeterLocation::Ecal );
  m_hcal = getDet<DeCalorimeter>(DeCalorimeterLocation::Hcal );

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0CaloCandidatesFromRaw::execute() {

  debug() << "==> Execute" << endmsg;

  LHCb::L0CaloCandidates* outFull = new LHCb::L0CaloCandidates();
  put( outFull, LHCb::L0CaloCandidateLocation::Full + m_extension );
  LHCb::L0CaloCandidates* out     = new LHCb::L0CaloCandidates();
  put( out,     LHCb::L0CaloCandidateLocation::Default + m_extension );

  LHCb::RawEvent* rawEvt = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );

  const std::vector<LHCb::RawBank*>& data = rawEvt->banks( LHCb::RawBank::L0Calo );
  std::vector<LHCb::RawBank*>::const_iterator itBnk;

  Gaudi::XYZPoint dummy( 0., 0., 0.);  
  Gaudi::XYZPoint center( 0., 0., 0.);  
  LHCb::L0CaloCandidate* myL0Cand;
  std::vector<LHCb::L0CaloCandidate*> bestCand;
  double tol;
  DeCalorimeter* det;

  for ( itBnk = data.begin() ; data.end() != itBnk ; itBnk++ ) {
    unsigned int* ptData = (*itBnk)->data();
    int bankSize = (*itBnk)->size()/4;  //== is in bytes...
    debug() << "  Bank " << (*itBnk)->sourceID() << " size " << bankSize << " words" << endreq;
    while ( 0 < bankSize-- ){
      int cand = (*ptData++);
      int type = cand>>24 & 0xFF;
      while ( bestCand.size() <= (unsigned int)type ) bestCand.push_back( 0 );

      verbose() << format( "Data %8x Type %1d", cand, type ) << endreq;

      if ( L0DUBase::Fiber::CaloSumEt == type ) {
        int sumEt = cand & 0xFFFFFF;
        myL0Cand = new LHCb::L0CaloCandidate ( type , LHCb::CaloCellID(), 
                                               sumEt, sumEt * m_etScale, dummy, 0. );
        outFull->add( myL0Cand );
        bestCand[type] = myL0Cand;

     } else if ( L0DUBase::Fiber::CaloSpdMult == type ) {
        int mult = cand & 0xFFFFFF;
        myL0Cand = new LHCb::L0CaloCandidate ( type , LHCb::CaloCellID(),
                                               mult, 0., dummy, 0. );
        outFull->add( myL0Cand );
        bestCand[type] = myL0Cand;

     } else {
       short rawId =  ( cand >>8 ) & 0xFFFF;
       LHCb::CaloCellID id ( rawId );
       if ( 3 == id.calo() ) {
         det = m_hcal;
       } else {
         det = m_ecal;
       }
       if ( det->valid( id ) ) {
         center = det->cellCenter( id );
         tol    = det->cellSize( id ) * .5;
         center.SetX( center.x() + tol );
         center.SetY( center.y() + tol );
       } else {
         LHCb::CaloCellID tmp( id.calo(), id.area(), id.row()+1, id.col()+1);
         center = det->cellCenter( tmp );
         tol    = det->cellSize( tmp ) * .5;

         center.SetX( center.x() - tol );
         center.SetY( center.y() - tol );
       }
       int et = cand & 0xFF;
       myL0Cand = new LHCb::L0CaloCandidate ( type, id, et, et * m_etScale, 
                                              center, tol );
       outFull->add( myL0Cand );
       if ( 0 == bestCand[type] ) {
         bestCand[type] = myL0Cand;
       } else if ( et > bestCand[type]->etCode() ) {
         bestCand[type] = myL0Cand;
       }
     }
    }
  }

  //=== Produce the selected candidate's table

  for ( unsigned int type = 0 ; bestCand.size() > type ; type++ ) {
    if ( 0 != bestCand[type] ) {
      LHCb::L0CaloCandidate* cand = bestCand[type];
      myL0Cand = new LHCb::L0CaloCandidate ( cand->type(),
                                             cand->id(),
                                             cand->etCode(),
                                             cand->et(),
                                             cand->position(),
                                             cand->posTol() );
      out->add( myL0Cand );
    }
  }

  
  if ( "" != m_extension ) {
    //== Compare 
    LHCb::L0CaloCandidates* ref = get<LHCb::L0CaloCandidates>( LHCb::L0CaloCandidateLocation::Full );
    LHCb::L0CaloCandidates::const_iterator itOld = ref->begin();
    LHCb::L0CaloCandidates::const_iterator itNew = outFull->begin();
    while ( ref->end() != itOld && outFull->end() != itNew ) {
      if ( (*itOld)->type()     != (*itNew)->type()   ||
           //(*itOld)->id()       != (*itNew)->id()     ||
           (*itOld)->etCode()   != (*itNew)->etCode() ||
           (*itOld)->et()       != (*itNew)->et()     ||
           (*itOld)->posTol()   != (*itNew)->posTol() ||
           (*itOld)->position() != (*itNew)->position() ) {
        info() << "Error : old " << (*itOld) << endreq
               << "        new " << (*itNew) << endreq;
      } else {
        debug() << "Entry OK " << itNew-outFull->begin() 
                << " type " << (*itNew)->type() << endreq;
      }
      itOld++;
      itNew++;
    }
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0CaloCandidatesFromRaw::finalize() {
  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
