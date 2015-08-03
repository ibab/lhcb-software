// Include files
// local
#include "L0CaloCandidatesFromRawBank.h"

// from Event
#include "Event/L0DUBase.h"
#include "Event/RawBankReadoutStatus.h"

// From Calo
#include "CaloDet/DeCalorimeter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0CaloCandidatesFromRawBank
//
// 2006-11-06 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0CaloCandidatesFromRawBank )

struct SortL0CaloByEt { 
  bool operator() (const LHCb::L0CaloCandidate * c1 , 
                   const LHCb::L0CaloCandidate * c2) {
    return  c1->etCode() > c2->etCode() ; 
  }
};

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0CaloCandidatesFromRawBank::L0CaloCandidatesFromRawBank
( const std::string& type , const std::string& name,
  const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
  declareInterface< L0CaloCandidatesFromRawBank >( this ) ;
  // Store also the intermediate informations for debugging the Selection boards
  // such as the partial results of the hadron Selection Boards
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
  StatusCode sc = GaudiTool::initialize() ; 
  if ( sc.isFailure() ) return sc;  
  
  // Retrieve the detector elements
  m_ecal = getDet< DeCalorimeter >( DeCalorimeterLocation::Ecal ) ;
  m_hcal = getDet< DeCalorimeter >( DeCalorimeterLocation::Hcal ) ;
  
  Condition* gain = m_ecal->condition( "Gain" );
  if ( 0 == gain ) return Error( "Condition 'Gain' not found in Ecal" ) ;
  
  // Gain for Et conversion
  if ( gain -> exists( "L0EtBin" ) )
    m_etScale = gain -> paramAsDouble( "L0EtBin" ) ;
  else 
    return Error( "Parameter 'L0EtBin' not found in Ecal 'Gain'" );
  
  return StatusCode::SUCCESS;
}

//=========================================================================
//  Convert the input banks to TES L0CaloCandidates, full and selected for 
//  L0DU
//=========================================================================
void L0CaloCandidatesFromRawBank::convertRawBankToTES
( std::vector< std::vector< unsigned int > >& data,
  LHCb::L0CaloCandidates * outFull , 
  LHCb::L0CaloCandidates * out ,
  const int version , 
  LHCb::RawBankReadoutStatus & readoutStatus ) {
  
  // temporary containers (ordered by Et)
  std::multiset< LHCb::L0CaloCandidate * , SortL0CaloByEt > temporaryOutFull ;
  std::multiset< LHCb::L0CaloCandidate * , SortL0CaloByEt > temporaryOut ;

  if ( msgLevel( MSG::DEBUG ) ) 
    debug() << "L0CaloCandidatesFromRawBank ... entering conversion" 
            << " for version " << version << endmsg ;
  
  Gaudi::XYZPoint dummy( 0. , 0. , 0. ) , center( 0. , 0. , 0. ) ;
  LHCb::L0CaloCandidate * myL0Cand( 0 ) ;
  std::vector< LHCb::L0CaloCandidate * > bestCand ;
  double tol ;
  DeCalorimeter * det ;
  
  std::map< int , LHCb::L0CaloCandidate * > mapHadrons ;
  mapHadrons.clear() ;
  
  unsigned int * ptData( 0 ) ;
  int bankSize( 0 ) , cand( 0 ) , sumEt( 0 ) , mult( 0 ) , rawId( 0 ) , source( 0 ), 
	  et( 0 ) ;
  unsigned short mask( 0 ) , type( 0 ) , slave( 0 ) , io( 0 ) ;
  LHCb::CaloCellID id ;
  bool hadronType( false ) ;
	  
  for ( std::vector<std::vector<unsigned int> >::iterator itBnk = data.begin(); 
        data.end() != itBnk; ++itBnk ) {

    ptData = &(*(*itBnk).begin()) ;
    bankSize = (*itBnk).size() ;  //== is in bytes...

    if ( msgLevel( MSG::DEBUG ) ) 
      debug() << " L0CaloCandidatesFromRawBank Bank " << itBnk-data.begin() 
              << " size " << bankSize << " words " << endmsg ;

    while ( 0 < bankSize-- ) {

      cand = (*ptData++);
      mask  = ( cand & 0x10000000 ) >> 28 ;

      type  = ( cand & 0x0F000000 ) >> 24 ;
      slave = ( cand & 0x60000000 ) >> 29 ;
      io    = ( cand & 0x80000000 ) >> 31 ;

      // Do not store candidates which are flagged as masked
      if ( 1 == mask ) { 
        if ( ( L0DUBase::CaloType::SumEt == type ) || 
             ( L0DUBase::CaloType::Hadron == type ) ) 
          readoutStatus.addStatus( 0 , LHCb::RawBankReadoutStatus::Incomplete ) ;
        else 
          readoutStatus.addStatus( 1 , LHCb::RawBankReadoutStatus::Incomplete ) ;
        continue ;
      }
      
      if ( msgLevel( MSG::DEBUG ) )
        debug() << " io= "<< io << " slave= " << slave  << " type= " << type 
                << endmsg ;
      
      // add a new "best candidate" if there is not enough
      while ( bestCand.size() <= (unsigned int) type ) bestCand.push_back( 0 );

      if ( msgLevel( MSG::VERBOSE ) ) 
        verbose() << format( "Data %8x ", cand ) << endmsg ;
      
      switch ( type ) {
      case L0DUBase::CaloType::SumEt: 
        sumEt = cand & 0xFFFFFF;

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

        // Total HCAL SumEt -> adds in "Best Candidate" container
        // If more that one, this is an error -> bank status set to corrupted 
        // Save SumEt only in default container, not in full not to have it 
        // twice
        if ( L0DUBase::CaloType::SumEt == type ) {
          if ( 0 == bestCand[ type ] ) {
            bestCand[ type ]  = new LHCb::L0CaloCandidate ( type , LHCb::CaloCellID() ,
                                                            sumEt , sumEt * m_etScale , 
                                                            dummy , 0. ) ;
          } else {
            warning() << "SumEt candidate already filled" << endmsg ;
            readoutStatus.addStatus( 0 , LHCb::RawBankReadoutStatus::Corrupted ) ;
          }
        } else {
          // The other cases (intermediate sum per Hadron Selection Board
          // are only saved when we do debugDecoding, in the full container
          if ( m_doDebugDecoding )  
            temporaryOutFull.insert( new LHCb::L0CaloCandidate ( type , LHCb::CaloCellID() ,
                                                                 sumEt , sumEt * m_etScale , 
                                                                 dummy , 0. ) ) ;
        }
        break ;
      case L0DUBase::CaloType::SpdMult:
        mult = 0 ; 
        // Get SPD multiplicity (total and all 16 inputs)
        if ( version > 0 ) {
          if ( 1 == io ) mult = ( cand & 0x3FFF ) ;
          if ( 0 == io ) mult = ( cand &  0x3FF ) ;
        } else mult = cand & 0xFFFFFF ;

        if ( m_doDebugDecoding )
          // Save intermediate multiplicities only when doing debug decoding, and save
          // in Full
          if ( 0 == io )
            temporaryOutFull.insert( new LHCb::L0CaloCandidate ( type , LHCb::CaloCellID(),
                                                                 mult , 0. , dummy , 0. ) ) ;
        
        // Save total multiplicity in default container, not in full
        if ( ( 1 == io ) || ( 0 == version ) ) { 
          if ( 0 == bestCand[ type ] ) {
            bestCand[ type ] = new LHCb::L0CaloCandidate ( type , LHCb::CaloCellID() ,
                                                           mult , 0. , dummy , 0. ) ;
          } else {
            warning() << "SPD Multiplicity candidate is already filled !" 
                      << endmsg ;
            readoutStatus.addStatus( 1 , LHCb::RawBankReadoutStatus::Corrupted ) ;
          }
        }
        break ;
      default:
        // All the other cases (electron, photon, pi0L, pi0G, hadron)
        rawId =  ( cand >>8 ) & 0xFFFF ;
        source = 0 ;
        if ( L0DUBase::CaloType::Hadron == type ) {
          det = m_hcal;
          rawId |= 0xc000;  // HCAL
        } else {
          source = 1 ;
          det = m_ecal;
          rawId |= 0x8000;  // ECAL
        }
	    
        id.setAll( rawId ) ;
	    
        if ( det->valid( id ) ) {
          center = det->cellCenter( id ) ;
          tol    = det->cellSize( id ) * .5 ;
          center.SetX( center.x() + tol ) ;
          center.SetY( center.y() + tol ) ;
        } else {
          if ( 0 == version ) { 
            if( msgLevel(MSG::DEBUG) ) debug() << "Non valid CELL Id" << endmsg ;
            LHCb::CaloCellID tmp( id.calo() , id.area() , id.row()+1 , id.col()+1) ;
            center = det -> cellCenter( tmp ) ;
            tol    = det -> cellSize( tmp ) * .5 ;
            center.SetX( center.x() - tol );
            center.SetY( center.y() - tol );
          } else {
            warning() << "Non valid CELL Id. RAW = " << std::hex << cand 
                      << endmsg ;
            readoutStatus.addStatus( source , LHCb::RawBankReadoutStatus::Corrupted ) ;
            continue ;
          }
        }
        
        et = cand & 0xFF;
        hadronType = false ;
        
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
                warning() << "Hadron candidate already filled !" << endmsg ;
                delete myL0Cand ;
                myL0Cand = 0;
                readoutStatus.addStatus( 0 , LHCb::RawBankReadoutStatus::Corrupted ) ;
              }
            } else { 
              temporaryOutFull.insert( myL0Cand ) ;
              if ( 0 == bestCand[type] ) bestCand[ type ] = myL0Cand ;
              else if ( et > bestCand[type]->etCode() ) bestCand[type] = myL0Cand ;
            }
          } else if ( ( 0 == io ) && ( 0 == slave ) ) { 
            // If Debugging, store all duplicates:
            if ( m_doDebugDecoding ) {
              myL0Cand = new LHCb::L0CaloCandidate ( type, id, et, 
                                                     et * m_etScale, center, 
                                                     tol );
              temporaryOutFull.insert( myL0Cand ) ;
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
                  myL0Cand = 0;
                }
              }
            }
          } else {
            // PARTIAL RESULTS Of Selection Boards: store only for debug
            if ( m_doDebugDecoding ) {
              myL0Cand = new LHCb::L0CaloCandidate ( type, id, et, 
                                                     et * m_etScale, center, 
                                                     tol );
              temporaryOutFull.insert( myL0Cand ) ;
            }
          }   
        } else {
          myL0Cand = new LHCb::L0CaloCandidate ( type, id, et, 
                                                 et * m_etScale, center, 
                                                 tol );
          if ( ( 0 == io ) || ( 0 == version ) ) {
            temporaryOutFull.insert( myL0Cand ) ;
            if ( 0 == version ) {
              if ( 0 == bestCand[type] ) {
                bestCand[type] = myL0Cand;
              } else if ( et > bestCand[type]->etCode() ) {
                bestCand[type] = myL0Cand;
              }
            }      
          } else {
            if ( 0 == bestCand[ type ] ) bestCand[ type ] = myL0Cand ;
            else {
              warning() << "Electromagnetic candidate already filled !" 
                        << endmsg ;
              readoutStatus.addStatus( 1 , LHCb::RawBankReadoutStatus::Corrupted ) ;
              delete myL0Cand ;
              myL0Cand = 0;
            }
          }
        }

        if ( msgLevel( MSG::VERBOSE ) ) {
          if ( 0 != myL0Cand ) verbose() << *myL0Cand << endmsg;
          else verbose() << "Null L0CaloCandidate" << endmsg ;
        }
      }
    }
  }

  // Now store all hadrons removing duplicates (if not debugging)
  if ( ! m_doDebugDecoding ) {
    std::map< int , LHCb::L0CaloCandidate * >::const_iterator it ;
    for ( it = mapHadrons.begin() ; it != mapHadrons.end() ; ++it ) {
      temporaryOutFull.insert( it -> second ) ;
    }
  }

  //=== Produce the selected candidate's table
  for ( unsigned int type = 0 ; bestCand.size() > type ; ++type ) {
    if ( 0 != bestCand[ type ] ) {
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
            debug() << "out bestCand type = " << type << *myL0Cand << endmsg ;
          temporaryOut.insert( myL0Cand );
        } else {
          temporaryOut.insert( bestCand[ type ] ) ;
        }
      } else {
        if ( msgLevel( MSG::DEBUG ) ) 
          debug() << "out bestCand type = " << type << " " << *bestCand[type]
                  << endmsg;
        temporaryOut.insert( bestCand[ type ] ) ;
      }
    }
  }

  // now copy the ordered containers into the final ones:
  std::multiset< LHCb::L0CaloCandidate * >::iterator it ;
  for ( it = temporaryOut.begin() ; it != temporaryOut.end() ; ++it ) 
    out -> add( *it ) ;
  for ( it = temporaryOutFull.begin() ; 
        it != temporaryOutFull.end() ; ++it ) 
    outFull -> add( *it ) ;
}
//=============================================================================
