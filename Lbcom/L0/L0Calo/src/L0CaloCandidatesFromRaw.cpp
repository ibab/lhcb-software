// $Id: L0CaloCandidatesFromRaw.cpp,v 1.3 2004-03-30 15:35:27 ocallot Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 

// local
#include "L0CaloCandidatesFromRaw.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0CaloCandidatesFromRaw
//
// 2003-12-15 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<L0CaloCandidatesFromRaw>          s_factory ;
const        IAlgFactory& L0CaloCandidatesFromRawFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0CaloCandidatesFromRaw::L0CaloCandidatesFromRaw( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
{
  m_etScale = 20. * MeV;

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

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialise" << endreq;

  // Retrieve the ECAL detector element

  SmartDataPtr<DeCalorimeter> detEcal( detSvc() ,"/dd/Structure/LHCb/Ecal" ) ;
  if( 0 == detEcal ) {
    msg << MSG::ERROR << "Unable to retrieve ECAL Detector Element " << endreq;
    return StatusCode::FAILURE ;
  }

  m_ecal = (DeCalorimeter*) detEcal;

  // Retrieve the HCAL detector element

  SmartDataPtr<DeCalorimeter> detHcal( detSvc() , "/dd/Structure/LHCb/Hcal" );
  if( 0 == detHcal ) {
    msg << MSG::ERROR << "Unable to retrieve HCAL Detector Element " << endreq;
    return StatusCode::FAILURE ;
  }
  m_hcal = (DeCalorimeter*) detHcal;


  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0CaloCandidatesFromRaw::execute() {

  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Execute" << endreq;

  StatusCode sc;
 
  L0CaloCandidates* outFull = new L0CaloCandidates();
  sc = eventSvc()->registerObject( L0CaloCandidateLocation::Full + m_extension,
                                   outFull );
  if( sc.isFailure() ) {
    if( 0 != outFull ) { delete outFull; }
    msg << MSG::ERROR << "Unable to register the output container="
        <<  L0CaloCandidateLocation::Full + m_extension << endreq
        << "Status is " << sc << endreq;
    return sc ;
  }
  L0CaloCandidates* out = new L0CaloCandidates();
  sc = eventSvc()->registerObject( L0CaloCandidateLocation::Default + 
                                   m_extension, out );
  if( sc.isFailure() ) {
    if( 0 != out ) { delete out; }
    msg << MSG::ERROR << "Unable to register the output container="
        <<  L0CaloCandidateLocation::Default + m_extension << endreq
        << "Status is " << sc << endreq;
    return sc ;
  }
 

  SmartDataPtr<RawBuffer> buf ( eventSvc(), RawBufferLocation::Default );
  RawEvent rawEvt( buf );

  const std::vector<RawBank>& data = rawEvt.banks( RawBuffer::L0Calo );
  std::vector<RawBank>::const_iterator itBnk;

  HepPoint3D dummy( 0., 0., 0.);  
  HepPoint3D center( 0., 0., 0.);  
  L0CaloCandidate* myL0Cand;
  std::vector<L0CaloCandidate*> bestCand;
  double tol;
  DeCalorimeter* det;

  for ( itBnk = data.begin() ; data.end() != itBnk ; itBnk++ ) {
    raw_int* ptData = (*itBnk).data();
    int bankSize = (*itBnk).dataSize();
    while ( 0 < bankSize-- ){
      int cand = (*ptData++);
      int type = cand>>24 & 0xFF;
      while ( bestCand.size() <= (unsigned int)type ) bestCand.push_back( 0 );

      msg << MSG::VERBOSE 
          << format( "Data %8x Type %1d", cand, type ) << endreq;

      if ( L0Calo::SumEt == type ) {
        int sumEt = cand & 0xFFFFFF;
        myL0Cand = new L0CaloCandidate ( L0Calo::SumEt, CaloCellID(), 
                                         sumEt, sumEt * m_etScale, dummy, 0. );
        outFull->add( myL0Cand );
        bestCand[type] = myL0Cand;

     } else if ( L0Calo::SpdMult == type ) {
        int mult = cand & 0xFFFFFF;
        myL0Cand = new L0CaloCandidate ( L0Calo::SpdMult, CaloCellID(),
                                         mult, 0., dummy, 0. );
        outFull->add( myL0Cand );
        bestCand[type] = myL0Cand;

     } else {
       short rawId =  ( cand >>8 ) & 0xFFFF;
       CaloCellID id ( rawId );
       if ( 3 == id.calo() ) {
         det = m_hcal;
       } else {
         det = m_ecal;
       }
       if ( det->valid( id ) ) {
         center = det->cellCenter( id );
         tol    = det->cellSize( id ) * .5;
         center.setX( center.x() + tol );
         center.setY( center.y() + tol );
       } else {
         CaloCellID tmp( id.calo(), id.area(), id.row()+1, id.col()+1);
         center = det->cellCenter( tmp );
         tol    = det->cellSize( tmp ) * .5;
         center.setX( center.x() - tol );
         center.setY( center.y() - tol );
       }
       int et = cand & 0xFF;
       myL0Cand = new L0CaloCandidate ( type, id, et, et * m_etScale, 
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
      L0CaloCandidate* cand = bestCand[type];
      myL0Cand = new L0CaloCandidate ( cand->type(),
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
    SmartDataPtr<L0CaloCandidates> ref ( eventSvc(), 
                                         L0CaloCandidateLocation::Full );
    L0CaloCandidates::const_iterator itOld = ref->begin();
    L0CaloCandidates::const_iterator itNew = outFull->begin();
    while ( ref->end() != itOld && outFull->end() != itNew ) {
      if ( (*itOld)->type()     != (*itNew)->type()   ||
           //(*itOld)->id()       != (*itNew)->id()     ||
           (*itOld)->etCode()   != (*itNew)->etCode() ||
           (*itOld)->et()       != (*itNew)->et()     ||
           (*itOld)->posTol()   != (*itNew)->posTol() ||
           (*itOld)->position() != (*itNew)->position() ) {
        msg << MSG::INFO << "Error : old " << (*itOld) << endreq
            << "        new " << (*itNew) << endreq;
      } else {
        msg << MSG::DEBUG << "Entry OK " << itNew-outFull->begin() 
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

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
