// $Id: L0CaloCandidatesFromRawBank.cpp,v 1.9 2008-03-27 23:42:11 robbep Exp $
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
  // Store also the intermediate informations for debugging the Selection boards
  declareProperty( "DebugDecoding" , m_doDebugDecoding = false ) ;
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
                                                       std::string& nameInTES,
                                                       const int version){

  if ( msgLevel( MSG::DEBUG ) ) 
    debug() << "L0CaloCandidatesFromRawBank ... entering conversion" << endreq;
  
  // Assume that full path (including rootInTES) is given in nameInTES etc.
  LHCb::L0CaloCandidates* outFull = new LHCb::L0CaloCandidates();
  put( outFull, nameFullInTES, IgnoreRootInTES );
  LHCb::L0CaloCandidates* out     = new LHCb::L0CaloCandidates();

  put( out, nameInTES, IgnoreRootInTES );
  if ( msgLevel( MSG::DEBUG ) ) 
    debug() << "L0CaloCandidatesFromRawBank Registered output in TES" << endreq;
  
  Gaudi::XYZPoint dummy( 0., 0., 0.);
  Gaudi::XYZPoint center( 0., 0., 0.);
  LHCb::L0CaloCandidate* myL0Cand( 0 ) ;
  std::vector<LHCb::L0CaloCandidate*> bestCand;
  double tol;
  DeCalorimeter* det;

  std::map< int , LHCb::L0CaloCandidate * > mapHadrons ;
  mapHadrons.clear() ;

  for ( std::vector<std::vector<unsigned int> >::iterator itBnk = data.begin(); 
        data.end() != itBnk; ++itBnk ) {
    unsigned int* ptData = &(*(*itBnk).begin());
    int bankSize = (*itBnk).size();  //== is in bytes...
    if ( msgLevel( MSG::DEBUG ) ) 
      debug() << " L0CaloCandidatesFromRawBank Bank " << itBnk-data.begin() 
              << " size " << bankSize << " words " << endreq ;
    while ( 0 < bankSize-- ){
      int cand = (*ptData++);
      unsigned short type  = ( cand & 0x0F000000 ) >> 24 ;
      unsigned short slave = ( cand & 0x60000000 ) >> 29 ;
      unsigned short io    = ( cand & 0x80000000 ) >> 31 ;
      
      if ( msgLevel( MSG::DEBUG ) )
        debug() << " io= "<< io << " slave= " << slave 
                << " type= " << type << endreq ;
      
      while ( bestCand.size() <= (unsigned int)type ) bestCand.push_back( 0 );
      if ( msgLevel( MSG::VERBOSE ) ) verbose() << format( "Data %8x ", cand )<<endreq;
      
      if ( L0DUBase::CaloType::SumEt == type ) {
        // CaloSumEt Case 
        int sumEt = cand & 0xFFFFFF;

        if ( version > 0 ) {
          if ( 0 == slave ) {
            if ( 1 == io ) type = L0DUBase::CaloType::SumEt ;
          } else if ( 1 == slave ) {
            if ( 1 == io ) type = L0DUBase::CaloType::SumEtSlave1Out ;
            else type = L0DUBase::CaloType::SumEtSlave1In ;
          } else if ( 2 == slave ) {
            if ( 1 == io ) type = L0DUBase::CaloType::SumEtSlave2Out ;
            else type = L0DUBase::CaloType::SumEtSlave2In ;
          }
        }

        if ( L0DUBase::CaloType::SumEt == type ) {
          if ( 0 == bestCand[ type ] ) { 
            myL0Cand = new LHCb::L0CaloCandidate ( type , LHCb::CaloCellID() ,
                                                   sumEt , sumEt * m_etScale , 
                                                   dummy , 0. ) ;
            // Do not include Sum Et in OutFull, only in default
            //          outFull -> add( myL0Cand ) ;
            bestCand[type] = myL0Cand ;
            if ( msgLevel( MSG::DEBUG ) )
              debug() << " outFull CaloSumEt = " << *myL0Cand << endreq ;
          } else {
            warning() << "SumEt candidate already filled" << endreq ;
          }
        } else {
          if ( m_doDebugDecoding ) { 
            myL0Cand = new LHCb::L0CaloCandidate ( type , LHCb::CaloCellID() ,
                                                   sumEt , sumEt * m_etScale , 
                                                   dummy , 0. );
            outFull -> add( myL0Cand );
            if ( msgLevel( MSG::DEBUG ) ) 
              debug() << " outFull CaloSumEt = " << *myL0Cand << endreq ;
          }
        }
      } else if ( L0DUBase::CaloType::SpdMult == type ) {
        int mult = 0 ; 
        if ( version > 0 ) {
          if ( 1 == io )  mult = ( cand & 0x3FFF ) ;
          if ( 0 == io ) {
            mult = ( cand & 0x3FF ) ;
            // a utiliser + tard 	  address =( cand & 0x3C00 ) >> 10 ;
          }
        } else {
          mult = cand & 0xFFFFFF ;
        }

        if ( m_doDebugDecoding == 1 ) { 
          if (io == 0 ) { 
            myL0Cand = new LHCb::L0CaloCandidate ( type , LHCb::CaloCellID(),
                                                   mult, 0., dummy, 0. );
            outFull -> add( myL0Cand );
            if ( msgLevel( MSG::DEBUG ) ) debug() << " outFull CaloSpdMult = " <<*myL0Cand << endreq;
          }
        }

        if ( ( 1 == io ) || ( 0 == version ) ) { 
          if ( 0 == bestCand[ type ] ) {
            myL0Cand = new LHCb::L0CaloCandidate ( type , LHCb::CaloCellID() ,
                                                   mult, 0., dummy, 0. ) ;
            bestCand[ type ] = myL0Cand ;
            // Do not include in outFull, only in Default
            //          outFull -> add( myL0Cand ) ;
            if ( msgLevel( MSG::VERBOSE ) ) verbose() <<" Candidate SPD = " << *myL0Cand << endreq;
          } else {
            warning() << "SPD Multiplicity candidate is already filled !" << endreq ;
          }
        }
        
      } else {
        int rawId =  ( cand >>8 ) & 0xFFFF;
        if ( L0DUBase::CaloType::Hadron == type ) {
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
          info() << "Non valid CELL Id" << endreq ;
          LHCb::CaloCellID tmp( id.calo(), id.area(), id.row()+1, id.col()+1);
          center = det->cellCenter( tmp );
          tol    = det->cellSize( tmp ) * .5;
          center.SetX( center.x() - tol );
          center.SetY( center.y() - tol );
        }
        int et = cand & 0xFF;
        
        bool hadronType = false ;
        
        if ( L0DUBase::CaloType::Hadron == type ) {
          hadronType = true ;
          if ( 1 == slave ) {
            if ( 1 == io ) type = L0DUBase::CaloType::HadronSlave1Out ;
            else type = L0DUBase::CaloType::HadronSlave1In ;
          } else if ( 2 == slave ) {
            if ( 1 == io ) type = L0DUBase::CaloType::HadronSlave2Out ;
            else type = L0DUBase::CaloType::HadronSlave2In ;
          }
        }

        if ( hadronType ) {
          if ( ( ( 1 == io ) && ( 0 == slave ) ) || ( 0 == version ) )  { 
            myL0Cand = new LHCb::L0CaloCandidate ( type, id, et, et * m_etScale,
                                                   center, tol );
            if ( version > 0 ) { 
              if ( 0 == bestCand[ type ] ) {
                bestCand[ type ] = myL0Cand ;
              } else {
                warning() << "Hadron candidate already filled !" << endreq ;
                delete myL0Cand ;
              }
            } else { 
              outFull -> add( myL0Cand ) ;
              if ( 0 == bestCand[type] ) {
                bestCand[type] = myL0Cand;
              } else if ( et > bestCand[type]->etCode() ) {
                bestCand[type] = myL0Cand;
              }
            }
          } else if ( ( 0 == io ) && ( 0 == slave ) ) { 
            // If Debugging, store all duplicates:
            if ( m_doDebugDecoding ) {
              myL0Cand = new LHCb::L0CaloCandidate ( type, id, et, 
                                                     et * m_etScale, center, 
                                                     tol );
              outFull -> add( myL0Cand ) ;
            } else { 
              // INPUT Hadrons: select highest duplicate
              myL0Cand = new LHCb::L0CaloCandidate ( type, id, et, 
                                                     et * m_etScale, center, 
                                                     tol );
              std::map< int , LHCb::L0CaloCandidate * >::const_iterator it = 
                mapHadrons.find( rawId ) ;
              if ( mapHadrons.end() == it ) 
                mapHadrons[ rawId ] = myL0Cand ;
              else {
                if ( (*it).second -> et() < myL0Cand -> et() ) {
                  LHCb::L0CaloCandidate * old = it -> second ;
                  mapHadrons.erase( it -> first ) ;
                  mapHadrons[ rawId ] = myL0Cand ;
                  delete old ;
                } else {
                  delete myL0Cand ;
                }
              }
            }
          } else {
            // PARTIAL RESULTS Of Selection Boards: store only for debug
            if ( m_doDebugDecoding ) {
              myL0Cand = new LHCb::L0CaloCandidate ( type, id, et, 
                                                     et * m_etScale, center, 
                                                     tol );
              outFull -> add( myL0Cand ) ;
            }
          }   
        } else {
          myL0Cand = new LHCb::L0CaloCandidate ( type, id, et, 
                                                 et * m_etScale, center, 
                                                 tol );
          if ( ( 0 == io ) || ( 0 == version ) ) {
            outFull -> add( myL0Cand ) ;
            if ( msgLevel( MSG::DEBUG ) ) debug() << " outFull le reste type = " << type 
                                                  << " " << *myL0Cand << endreq ;
            if ( 0 == version ) {
              if ( 0 == bestCand[type] ) {
                bestCand[type] = myL0Cand;
              } else if ( et > bestCand[type]->etCode() ) {
                bestCand[type] = myL0Cand;
              }
            }      
          } else {
            if ( 0 == bestCand[ type ] ) {
              bestCand[ type ] = myL0Cand ;
            } else {
              warning() << "Electromagnetic candidate already filled !" << endreq ;
              delete myL0Cand ;
            }
          }
        }
        if ( msgLevel( MSG::VERBOSE ) ) verbose() << *myL0Cand << endreq;
      }
    }
  }

  // Now store all hadrons removing duplicates (if not debugging)
  if ( ! m_doDebugDecoding ) {
    std::map< int , LHCb::L0CaloCandidate * >::const_iterator it ;
    for ( it = mapHadrons.begin() ; it != mapHadrons.end() ; ++it ) {
      outFull -> add( it -> second ) ;
    }
  }
  //=== Produce the selected candidate's table

  for ( unsigned int type = 0 ; bestCand.size() > type ; type++ ) {
    if ( 0 != bestCand[type] ) {
      if ( 0 == version ) {
        if ( ( L0DUBase::CaloType::SumEt != type ) && 
             ( L0DUBase::CaloType::SpdMult != type ) ) {
          LHCb::L0CaloCandidate* cand = bestCand[type];
          myL0Cand = new LHCb::L0CaloCandidate ( cand->type(),
                                                 cand->id(),
                                                 cand->etCode(),
                                                 cand->et(),
                                                 cand->position(),
                                                 cand->posTol() );
        
          if ( msgLevel( MSG::DEBUG ) ) 
            debug() << "out bestCand type = "<<type<<*myL0Cand << endreq;
          out -> add( myL0Cand );
        } else {
          out -> add( bestCand[ type ] ) ;
        }
      } else {
        out -> add( bestCand[ type ] ) ;
      }
    }
  }
}
//=============================================================================
