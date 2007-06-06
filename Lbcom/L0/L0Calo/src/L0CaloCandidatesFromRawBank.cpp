// $Id: L0CaloCandidatesFromRawBank.cpp,v 1.4 2007-06-06 14:59:26 cattanem Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

#include "Event/L0DUBase.h"

// local
#include "L0CaloCandidatesFromRawBank.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0CaloCandidatesFromRawBank
//
// 2006-11-06 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0CaloCandidatesFromRawBank );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0CaloCandidatesFromRawBank::L0CaloCandidatesFromRawBank( const std::string& type,
                                                          const std::string& name,
                                                          const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<L0CaloCandidatesFromRawBank>(this);

}
//=============================================================================
// Destructor
//=============================================================================
L0CaloCandidatesFromRawBank::~L0CaloCandidatesFromRawBank() {}


//=========================================================================
//
//=========================================================================
StatusCode L0CaloCandidatesFromRawBank::initialize ( ) {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  // Retrieve the detector elements
  m_ecal = getDet<DeCalorimeter>(DeCalorimeterLocation::Ecal );
  m_hcal = getDet<DeCalorimeter>(DeCalorimeterLocation::Hcal );

  Condition* gain = m_ecal->condition( "Gain" );
  if ( 0 == gain ) {
    return Error( "Condition 'Gain' not found in Ecal" );
  }
  if ( gain->exists( "L0EtBin" ) ) {
    m_etScale = gain->paramAsDouble( "L0EtBin" );
  } else {
    return Error( "Parameter 'L0EtBin' not found in Ecal 'Gain'" );
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Convert the input banks to TES L0CaloCandidates, full and selected for L0DU
//=========================================================================
void L0CaloCandidatesFromRawBank::convertRawBankToTES( std::vector<std::vector<unsigned int> >& data,
                                                       std::string& nameFullInTES,
                                                       std::string& nameInTES){

  debug() << "... entering conversion" << endreq;
  // Assume that full path (including rootInTES) is given in nameInTES etc.
  LHCb::L0CaloCandidates* outFull = new LHCb::L0CaloCandidates();
  put( outFull, nameFullInTES, IgnoreRootInTES );
  LHCb::L0CaloCandidates* out     = new LHCb::L0CaloCandidates();
  put( out, nameInTES, IgnoreRootInTES );
  debug() << "Registered output in TES" << endreq;

  Gaudi::XYZPoint dummy( 0., 0., 0.);
  Gaudi::XYZPoint center( 0., 0., 0.);
  LHCb::L0CaloCandidate* myL0Cand;
  std::vector<LHCb::L0CaloCandidate*> bestCand;
  double tol;
  DeCalorimeter* det;

  for ( std::vector<std::vector<unsigned int> >::iterator itBnk = data.begin();
        data.end() != itBnk; ++itBnk ) {
    unsigned int* ptData = &(*(*itBnk).begin());
    int bankSize = (*itBnk).size();  //== is in bytes...
    debug() << "  Bank " << itBnk-data.begin() << " size " << bankSize << " words" << endreq;
    while ( 0 < bankSize-- ){
      int cand = (*ptData++);
      int type = cand>>24 & 0xFF;
      while ( bestCand.size() <= (unsigned int)type ) bestCand.push_back( 0 );

      if ( msgLevel( MSG::VERBOSE ) ) verbose() << format( "Data %8x ", cand );

      if ( L0DUBase::Fiber::CaloSumEt == type ) {
        int sumEt = cand & 0xFFFFFF;
        myL0Cand = new LHCb::L0CaloCandidate ( type , LHCb::CaloCellID(),
                                               sumEt, sumEt * m_etScale, dummy, 0. );
        outFull->add( myL0Cand );
        bestCand[type] = myL0Cand;
        if ( msgLevel( MSG::VERBOSE ) ) verbose() << *myL0Cand << endreq;
      } else if ( L0DUBase::Fiber::CaloSpdMult == type ) {
        int mult = cand & 0xFFFFFF;
        myL0Cand = new LHCb::L0CaloCandidate ( type , LHCb::CaloCellID(),
                                               mult, 0., dummy, 0. );
        outFull->add( myL0Cand );
        bestCand[type] = myL0Cand;
        if ( msgLevel( MSG::VERBOSE ) ) verbose() << *myL0Cand << endreq;
      } else {
        int rawId =  ( cand >>8 ) & 0xFFFF;
        if ( 2 == type ) {
          det = m_hcal;
          rawId |= 0xc000;  // HCAL
        } else {
          det = m_ecal;
          rawId |= 0x8000;  // ECAL
        }
        LHCb::CaloCellID id ( rawId );
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
        if ( msgLevel( MSG::VERBOSE ) ) verbose() << *myL0Cand << endreq;
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
}
//=============================================================================
