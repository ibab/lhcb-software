// Include Files 

// from Event
#include "Event/RecHeader.h"
#include "Event/RawEvent.h"
#include "Event/RawBankReadoutStatus.h"
#include "Event/L0ProcessorData.h"
#include "Event/L0MuonData.h"
#include "Event/L0MuonInfo.h"
#include "Event/L0MuonError.h"

// from Kernel
#include "Kernel/MuonTileID.h"

#include "L0MuonKernel/ProcUtilities.h"
#include "L0MuonKernel/CtrlCandErrors.h"
#include "L0MuonKernel/ProcCandErrors.h"
#include "L0MuonKernel/ProcDataErrors.h"

// local
#include "L0MuonOutputs.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonOutputs
//
// 2007-07-09 : Julien Cogan
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0MuonOutputs )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonOutputs::L0MuonOutputs( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<L0MuonOutputs>(this);
  
  // Converters for the banks of the 2 TELL1s connected to the controller boards
  for (int i= 0; i<2; ++i) {
    m_ctrlCand[i]  =  new L0Muon::CtrlCandCnv(i);
  }
  // Converters for the banks of the 4 TELL1s connected to the processing boards
  for (int i= 0; i<4; ++i) {
    m_procCand[i] = new L0Muon::ProcCandCnv(i);
    m_procData[i] = new L0Muon::ProcDataCnv(i);
  }

  m_rawBankNorm = 0;
  m_nCandFinalNorm = 0;
  
  m_rawBankSizeTot= 0;
  m_nCandFinalTot = 0;
  m_nTriggeredEvt = 0;
  
  m_bankEventCounter  = 0;

  m_averagePt =0.;
  m_averageCh=0.;

  m_ctrlCandFlag       = true ;
  m_ctrlCandFlagBCSU   = true ;
  m_procCandFlag       = true ;
  m_procDataFlag       = true ;

  m_version = -1;
  m_mode = 0;
  m_compression = false;
  
}
//=============================================================================
// Destructor
//=============================================================================
L0MuonOutputs::~L0MuonOutputs() {
  // Converters for the banks of the 2 TELL1s connected to the controller boards
  for (int i= 0; i<2; ++i) {
    delete m_ctrlCand[i];
  }
  // Converters for the banks of the 4 TELL1s connected to the processing boards
  for (int i= 0; i<4; ++i) {
    delete m_procCand[i];
    delete m_procData[i];
  }
} 

//=============================================================================
 
StatusCode L0MuonOutputs::decodeRawBanks(std::string rawInputEvent , bool useRootInTES , bool statusOnTES)
{

  LHCb::RawEvent* rawEvt = getIfExists<LHCb::RawEvent>( rawInputEvent , useRootInTES); 
  if ( NULL == rawEvt ) 
    return Error("RawEvent not found at "+rawInputEvent,StatusCode::FAILURE,50);

  return decodeRawBanks(rawEvt,statusOnTES);
}

StatusCode L0MuonOutputs::decodeRawBanks(LHCb::RawEvent* rawEvt, bool statusOnTES)
{
  
  bool error_bank[6];
  for (int srcId=0; srcId<6; ++srcId) error_bank[srcId]=false;
  
  m_ctrlCandFlag     = false ;
  m_ctrlCandFlagBCSU = false ;
  m_procCandFlag     = false ;
  m_procDataFlag     = false ;
  
  m_l0EventNumber=-1;
  m_l0_B_Id=-1;

  m_version=-1;

  int rawBankSize = 0;

  LHCb::RawBankReadoutStatus ctrlCandStatus = LHCb::RawBankReadoutStatus(LHCb::RawBank::L0Muon );
  LHCb::RawBankReadoutStatus procCandStatus = LHCb::RawBankReadoutStatus(LHCb::RawBank::L0MuonProcCand );
  LHCb::RawBankReadoutStatus procDataStatus = LHCb::RawBankReadoutStatus(LHCb::RawBank::L0MuonProcData );
  
  // ======================
  //
  // L0Muon Error Banks
  //
  // ======================
  const std::vector<LHCb::RawBank*>& errbanks = rawEvt->banks( LHCb::RawBank::L0MuonError );
  for ( std::vector<LHCb::RawBank*>::const_iterator itBnk = errbanks.begin(); errbanks.end() != itBnk; ++itBnk ) {
    int srcID = (*itBnk)->sourceID();
    if( msgLevel(MSG::DEBUG) ) debug() << "L0MuonError bank found srcID="<<srcID<<endmsg;
    error_bank[srcID]=true;
  }

  // ======================
  //
  // L0Muon(CtrlCand) Banks
  //
  // ======================
  const std::vector<LHCb::RawBank*>& banks = rawEvt->banks( LHCb::RawBank::L0Muon );

  if( msgLevel(MSG::VERBOSE) ) verbose() << "decodeRawBanks: "<<banks.size()<<" L0Muon banks found"<<endmsg;

  if (banks.size()==0) {
    ctrlCandStatus.addStatus(0,LHCb::RawBankReadoutStatus::Missing);
  } else {
    for ( std::vector<LHCb::RawBank*>::const_iterator itBnk = banks.begin(); banks.end() != itBnk; ++itBnk ) {
      int srcID = (*itBnk)->sourceID();
      if (error_bank[srcID]) {
        ctrlCandStatus.addStatus(srcID,LHCb::RawBankReadoutStatus::ErrorBank);
      }
      if( LHCb::RawBank::MagicPattern != (*itBnk)->magic() ) {
        // report an error and return without decoding
        ctrlCandStatus.addStatus(srcID,LHCb::RawBankReadoutStatus::Corrupted);
        Error("L0MuonCtrlCand :  Magic pattern is wrong",StatusCode::FAILURE,50).ignore();
        continue;
      }
      int size = (*itBnk)->size()/4;
      if (size==0) {
        ctrlCandStatus.addStatus(srcID,LHCb::RawBankReadoutStatus::Empty);
        continue;
      }
      int decoding_status=0;
      int bankVersion  = (*itBnk)->version();
      m_version = bankVersion;
      std::vector<unsigned int> data;
      unsigned int* body = (*itBnk)->data();
      rawBankSize+=size;
      for ( int k = 0; size > k; ++k ) {
        data.push_back( *body++ );
      }
      if (bankVersion==0) 
      { // DC06 
        if (srcID>0) continue;
        if (srcID==0) {
          decoding_status=m_ctrlCand[0]->decodeBankDC06(data,bankVersion);
          decoding_status=m_ctrlCand[1]->decodeBankDC06(data,bankVersion);
          m_ctrlCandFlag=true;
          if( msgLevel(MSG::VERBOSE) ) verbose() << "decodeRawBanks: L0Muon bank (version "<< bankVersion <<" ) found"
                                                 <<", sourceID is "<< srcID <<", size is "<<size
                                                 <<" decoding status= "<<decoding_status<<endmsg;
          break; // only 1 bank
        }
      }// End  DC06
      else if (bankVersion==1) 
      { // V1 (never used ?)
        decoding_status=m_ctrlCand[ctrlSourceID(srcID)]->decodeBankFinalCandidates(data,bankVersion);
        if (decoding_status>0) m_ctrlCandFlag=true;
      } // End V1 
      else if (bankVersion>1) 
      { // V2 and greater
        L0Muon::CtrlCandCnv * ctrlCand=m_ctrlCand[ctrlSourceID(srcID)];
        ctrlCand->clearRef();
        ctrlCand->submitL0EventNumber(m_l0EventNumber);
        ctrlCand->submitL0_B_Id(m_l0_B_Id);        
        decoding_status=ctrlCand->decodeBank(data,bankVersion,m_mode);
        m_l0EventNumber=ctrlCand->ref_l0EventNumber();
        m_l0_B_Id=ctrlCand->ref_l0_B_Id();
        if (abs(decoding_status)>0) m_ctrlCandFlag=true;
        if (abs(decoding_status)>1) m_ctrlCandFlagBCSU=true;
        if (decoding_status>0) {
          ctrlCandStatus.addStatus(srcID,LHCb::RawBankReadoutStatus::OK);
        } else if (decoding_status==0) {
          ctrlCandStatus.addStatus(srcID,LHCb::RawBankReadoutStatus::Incomplete);
        } else {
          ctrlCandStatus.addStatus(srcID,LHCb::RawBankReadoutStatus::Corrupted);
        }
      } // End V2 and greater
      if( msgLevel(MSG::VERBOSE) ) verbose() << "decodeRawBanks: L0Muon bank (version "<< bankVersion <<" ) found"
                                             <<", sourceID is "<< srcID <<", size is "<<size
                                             <<" Q"<<(ctrlSourceID(srcID)+1)
                                             <<", decoding status= "<<decoding_status<<endmsg;
    }
  }

  // ======================
  //
  // L0MuonProcCand Banks
  //
  // ======================
  if (m_mode>0) {
    const std::vector<LHCb::RawBank*>& proccandbanks = rawEvt->banks( LHCb::RawBank::L0MuonProcCand );
    if (proccandbanks.size()==0) {
      procCandStatus.addStatus(0,LHCb::RawBankReadoutStatus::Missing);
    } else {
      m_procCandFlag =true;
      for ( std::vector<LHCb::RawBank*>::const_iterator itBnk = proccandbanks.begin(); proccandbanks.end() != itBnk; ++itBnk ) {
        int srcID = (*itBnk)->sourceID();
        if (error_bank[srcID]) {
          procCandStatus.addStatus(srcID,LHCb::RawBankReadoutStatus::ErrorBank);
        }
        if( LHCb::RawBank::MagicPattern != (*itBnk)->magic() ) {
          // report an error and return without decoding
          procCandStatus.addStatus(srcID,LHCb::RawBankReadoutStatus::Corrupted);
          Error("L0MuonProcCand :  Magic pattern is wrong",StatusCode::FAILURE,50).ignore();
          continue;
        }
        int size = (*itBnk)->size()/4;
        if (size==0) {
          procCandStatus.addStatus(srcID,LHCb::RawBankReadoutStatus::Empty);
          continue;
        }
        int decoding_status=0;
        int bankVersion  = (*itBnk)->version();
        m_version = bankVersion;
        std::vector<unsigned int> data;
        unsigned int* body = (*itBnk)->data();
        rawBankSize+=size;
        for ( int k = 0; size > k; ++k ) {
          data.push_back( *body++ );
        }
        L0Muon::ProcCandCnv * procCand=m_procCand[procSourceID(srcID)];
        procCand->clearRef();
        procCand->submitL0EventNumber(m_l0EventNumber);
        procCand->submitL0_B_Id(m_l0_B_Id);        
        decoding_status=procCand->decodeBank(data,bankVersion);
        //m_l0EventNumber=procCand->ref_l0EventNumber();
        //m_l0_B_Id=procCand->ref_l0_B_Id();
        if (decoding_status>0) {
          procCandStatus.addStatus(srcID,LHCb::RawBankReadoutStatus::OK);
        } else if (decoding_status==0) {
          procCandStatus.addStatus(srcID,LHCb::RawBankReadoutStatus::Incomplete);
        } else {
          procCandStatus.addStatus(srcID,LHCb::RawBankReadoutStatus::Corrupted);
        }
        if( msgLevel(MSG::VERBOSE) ) verbose() << "decodeRawBanks: L0MuonProcCand bank (version "<< bankVersion <<" ) found"
                                               <<", sourceID is "<< srcID <<", size is "<< size
                                               <<", Q"<<(procSourceID(srcID)+1)
                                               <<", decoding status= "<<decoding_status<<endmsg;
      }    
    }
  }
  
  // ======================
  //
  // L0MuonProcData Banks
  //
  // ======================
  if (m_mode>0) {
    const std::vector<LHCb::RawBank*>& procdatabanks = rawEvt->banks( LHCb::RawBank::L0MuonProcData );
    if (procdatabanks.size()==0) {
      if( msgLevel(MSG::DEBUG) ) debug()<<"decodeRawBanks: no banks in "<<LHCb::RawBank::L0MuonProcData<<endmsg;
      procDataStatus.addStatus(0,LHCb::RawBankReadoutStatus::Missing);
    } else {
      m_procDataFlag =true;
      for ( std::vector<LHCb::RawBank*>::const_iterator itBnk = procdatabanks.begin(); procdatabanks.end() != itBnk; ++itBnk ) {
        int srcID = (*itBnk)->sourceID();
        if (error_bank[srcID]) {
          procDataStatus.addStatus(srcID,LHCb::RawBankReadoutStatus::ErrorBank);
        }
        if( LHCb::RawBank::MagicPattern != (*itBnk)->magic() ) {
          // report an error and return without decoding
          procDataStatus.addStatus(srcID,LHCb::RawBankReadoutStatus::Corrupted);
          Error("L0MuonProcData :  Magic pattern is wrong",StatusCode::FAILURE,50).ignore();
          continue;
        }
        int size = (*itBnk)->size()/4;
        if (size==0) {
          procDataStatus.addStatus(srcID,LHCb::RawBankReadoutStatus::Empty);
          continue;
        }
        int decoding_status=0;
        int bankVersion  = (*itBnk)->version();
        m_version = bankVersion;
        std::vector<unsigned int> data;
        unsigned int* body = (*itBnk)->data();
        rawBankSize+=size;
        for ( int k = 0; size > k; ++k ) {
          data.push_back( *body++ );
        }
        decoding_status=m_procData[procSourceID(srcID)]->decodeBank(data,bankVersion);
        if (decoding_status>0) {
          procDataStatus.addStatus(srcID,LHCb::RawBankReadoutStatus::OK);
        } else {
          procDataStatus.addStatus(srcID,LHCb::RawBankReadoutStatus::Corrupted);
        }
        if( msgLevel(MSG::VERBOSE) ) verbose() << "decodeRawBanks: L0MuonProcData bank (version "<< bankVersion <<" ) found"
                                               <<", sourceID is "<< srcID <<", size is "<< size
                                               <<", Q"<<(procSourceID(srcID)+1)
                                               <<", decoding status= "<<decoding_status<< endmsg;
      }    
    }
  }


  if ( statusOnTES ) {
    LHCb::RawBankReadoutStatuss * L0MuonBanksStatuss = 
      getOrCreate<LHCb::RawBankReadoutStatuss,LHCb::RawBankReadoutStatuss>( LHCb::RawBankReadoutStatusLocation::Default );

    LHCb::RawBankReadoutStatus * status = L0MuonBanksStatuss-> object( ctrlCandStatus.key() ) ;
    if ( 0 == status ) {
      status = new LHCb::RawBankReadoutStatus( ctrlCandStatus ) ;
      L0MuonBanksStatuss-> insert( status ) ;
    } else {
      if ( status -> status() != ctrlCandStatus.status() ) {
        std::map< int , long >::iterator it ;
        for ( it = ctrlCandStatus.statusMap().begin() ; 
              it != ctrlCandStatus.statusMap().end() ; ++it ) {
          status -> addStatus( (*it).first , (*it).second ) ;
        }
      }
    }
    
    if (m_mode > 0) {
      status = L0MuonBanksStatuss-> object( procCandStatus.key() ) ;
      if ( 0 == status ) {
        status = new LHCb::RawBankReadoutStatus( procCandStatus ) ;
        L0MuonBanksStatuss-> insert( status ) ;
      } else {
        if ( status -> status() != procCandStatus.status() ) {
          std::map< int , long >::iterator it ;
          for ( it = procCandStatus.statusMap().begin() ; 
                it != procCandStatus.statusMap().end() ; ++it ) {
            status -> addStatus( (*it).first , (*it).second ) ;
          }
        }
      }

      status = L0MuonBanksStatuss-> object( procDataStatus.key() ) ;
      if ( 0 == status ) {
        status = new LHCb::RawBankReadoutStatus( procDataStatus ) ;
        L0MuonBanksStatuss-> insert( status ) ;
      } else {
        if ( status -> status() != procDataStatus.status() ) {
          std::map< int const, long >::iterator it ;
          for ( it = procDataStatus.statusMap().begin() ; 
                it != procDataStatus.statusMap().end() ; ++it ) {
            status -> addStatus( (*it).first , (*it).second ) ;
          }
        }
      }
    }
  }
  

  ++m_rawBankNorm;
  m_rawBankSizeTot += rawBankSize;

  
  return StatusCode::SUCCESS;

}



StatusCode L0MuonOutputs::writeRawBanks(){
  
  int rawBankSize = 0;
  int ievt;
  
  LHCb::RecHeader* evt = getIfExists<LHCb::RecHeader> (LHCb::RecHeaderLocation::Default);
  if ( NULL != evt ) {
    ievt = int(evt->evtNumber());
  } else {
    ievt = m_bankEventCounter;
    ++m_bankEventCounter;
  }  
  
  LHCb::RawEvent* raw = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
  
  std::vector<unsigned int> data;

  // L0Muon DC06 (only 5 TELL1s where foreseen)
  if (m_version==0) { // Bank version =0

    data=DC06RawBanks();
    raw->addBank(0, LHCb::RawBank::L0Muon,0,data);
    rawBankSize += data.size();
    if( msgLevel(MSG::VERBOSE) ) verbose() << "writeRawBanks: L0Muon bank written (DC06) size is "
                                           << data.size() <<endmsg;
    ++m_rawBankNorm;
    m_rawBankSizeTot += rawBankSize;
    return StatusCode::SUCCESS;

  } else if (m_version==1) {// Bank version =1 (2 banks where foreseen for the controller boards)

    // L0Muon(CtrlCand) (always there - light, standard and debug modes)
    for (int i= 0; i<2; ++i) {
      m_ctrlCand[i]->rawBankFinalCandidates(data,ievt);
      raw->addBank(i, LHCb::RawBank::L0Muon,m_version,data);
      if( msgLevel(MSG::VERBOSE) ) verbose() << "writeRawBanks: L0Muon bank written (version "
                                             << m_version<<" ) size is "<< data.size() <<endmsg;
      rawBankSize += data.size();
    }

    // L0MuonProcCand (always there - light, standard and debug modes)
    for (int i= 0; i<4; ++i) {
      m_procCand[i]->rawBank(data,ievt,m_version,m_compression);
      raw->addBank(i, LHCb::RawBank::L0MuonProcCand,m_version,data);
      if( msgLevel(MSG::VERBOSE) ) verbose() << "writeRawBanks: L0MuonProcCand bank written (version "
                                             << m_version<<" ) size is "<< data.size() <<endmsg;
      rawBankSize += data.size();
    }

    // L0MuonProcData (only for standard and debug modes)
    if (m_mode>0) {
      for (int i= 0; i<4; ++i) {
        m_procData[i]->rawBank(data,m_version,m_mode,m_compression);
        raw->addBank(i, LHCb::RawBank::L0MuonProcData,m_version,data);
        if( msgLevel(MSG::VERBOSE) ) debug() << "writeRawBanks: L0MuonProcData bank written (version "
                                               << m_version<<" ) size is "<< data.size() <<endmsg;
        rawBankSize += data.size();
      }
    } 

    
  } else { // Bank version >1

    int encoding_status;
    
    // L0Muon (always there - light, standard and debug modes)
    for (int i= 0; i<2; ++i) {
      encoding_status = m_ctrlCand[i]->rawBank(data,ievt,m_version,m_mode,m_compression);
      if (encoding_status<1) {
        if( msgLevel(MSG::DEBUG) ) debug() << "writeRawBanks: L0Muon bank "
                                           << "with srcID= "<<ctrlSourceID_inv(i)
                                           <<" ( "
                                           <<"version "<< m_version
                                           <<", mode "<< m_mode
                                           <<", compression "<< m_compression
                                           <<" )"
                                           <<" size is "<< data.size() 
                                           <<" -> encoding error: status= "<<encoding_status
                                           <<endmsg;
        continue;
      }
      raw->addBank(ctrlSourceID_inv(i), LHCb::RawBank::L0Muon,m_version,data);
      if( msgLevel(MSG::VERBOSE) ) verbose() << "writeRawBanks: L0Muon bank written "
                                             << "with srcID= "<<ctrlSourceID_inv(i)
                                             <<" ( "
                                             <<"version "<< m_version
                                             <<", mode "<< m_mode
                                             <<", compression "<< m_compression
                                             <<" )"
                                             <<" size is "<< data.size() 
                                             <<" -> encoding error: status= "<<encoding_status
                                             <<endmsg;
      rawBankSize += data.size();
    }

    // L0MuonProcCand (only for standard and debug modes)
    if (m_mode>0) {
      for (int i= 0; i<4; ++i) {
        encoding_status = m_procCand[i]->rawBank(data,ievt,m_version,m_compression);
        if (encoding_status<1) {
          if( msgLevel(MSG::DEBUG) ) debug() << "writeRawBanks: L0MuonProcCand bank "
                                             << "with srcID= "<<procSourceID_inv(i)
                                             <<" ( "
                                             <<"version "<< m_version
                                             <<", mode "<< m_mode
                                             <<", compression "<< m_compression
                                             <<" )"
                                             <<" size is "<< data.size() 
                                             <<" -> encoding error: status= "<<encoding_status
                                             <<endmsg;
          continue;
        }
        raw->addBank(procSourceID_inv(i), LHCb::RawBank::L0MuonProcCand,m_version,data);
        if( msgLevel(MSG::VERBOSE) ) verbose() << "writeRawBanks: L0MuonProcCand bank written "
                                               << "with srcID= "<<procSourceID_inv(i)
                                               <<" ( "
                                               <<"version "<< m_version
                                               <<", mode "<< m_mode
                                               <<", compression "<< m_compression
                                               <<" )"
                                               <<" size is "<< data.size() 
                                               <<" -> encoding error: status= "<<encoding_status
                                               <<endmsg;
        rawBankSize += data.size();
      }
    }
    
    // L0MuonProcData (only for standard and debug modes)
    if (m_mode>0) {
      for (int i= 0; i<4; ++i) {
        encoding_status = m_procData[i]->rawBank(data,m_version,m_mode,m_compression);
        if (encoding_status<1) {
          if( msgLevel(MSG::DEBUG) ) debug() << "writeRawBanks: L0MuonProcData bank "
                                             << "with srcID= "<<procSourceID_inv(i)
                                             <<" ( "
                                             <<"version "<< m_version
                                             <<", mode "<< m_mode
                                             <<", compression "<< m_compression
                                             <<" )"
                                             <<" size is "<< data.size() 
                                             <<" -> encoding error: status= "<<encoding_status
                                             <<endmsg;
          continue;
        }
        raw->addBank(procSourceID_inv(i), LHCb::RawBank::L0MuonProcData,m_version,data);
        if( msgLevel(MSG::VERBOSE) ) verbose() << "writeRawBanks: L0MuonProcData bank written "
                                               << "with srcID= "<<procSourceID_inv(i)
                                               <<" ( "
                                               <<"version "<< m_version
                                               <<", mode "<< m_mode
                                               <<", compression "<< m_compression
                                               <<" )"
                                               <<" size is "<< data.size() 
                                               <<" -> encoding error: status= "<<encoding_status
                                               <<endmsg;
        rawBankSize += data.size();
      }
    } 

  }
  
  
  ++m_rawBankNorm;
  m_rawBankSizeTot += rawBankSize;

  return StatusCode::SUCCESS;
  
}

StatusCode L0MuonOutputs::writeOnTES(std::string l0context , std::string taeInTes){

  int nCandFinal  = 0;
  double sumPt = 0;
  double sumCh = 0;
 
  std::vector<L0Muon::PMuonCandidate> cands ;
  std::vector<L0Muon::PMuonCandidate>::iterator itcand;
  std::string location;

  location = taeInTes + LHCb::L0MuonCandidateLocation::Default + l0context;
  LHCb::L0MuonCandidates* pl0mcands = new LHCb::L0MuonCandidates();
  put(pl0mcands , location );
  // Candidates selected by the controller boards (should always be there)
  if (m_ctrlCandFlag) {
    for (int i= 0; i<2; ++i) {
      for (int iq=0; iq<2; ++iq){
        if( msgLevel(MSG::VERBOSE) ) verbose() << "writeOnTES: "<<" side"<<i<<" Q"<<(i*2+iq)<<endmsg;
        // Do not write the candidates on TES if an error occured in the decoding
        if (m_ctrlCand[i]->decodingError(iq)>0) continue;
        cands = m_ctrlCand[i]->muonCandidates(iq);
        if( msgLevel(MSG::VERBOSE) ) verbose() << "writeOnTES: "<<" side"<<i<<" Q"<<(i*2+iq)
                                               <<" nb of cand= "<<cands.size()<<endmsg;
        nCandFinal+=cands.size();
        for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
          LHCb::L0MuonCandidate* l0mcand = l0muoncandidate(*itcand);
          if (l0mcand==NULL) {
            if (msgLevel( MSG::ERROR)) error()<<"writeOnTES: ctrl side "<<i
                                              <<"\n"<<(*itcand)->dump("\t=> ")<< endmsg;
            continue;
          }
          double pt = l0mcand->pt();
          sumPt += fabs(pt);
          sumCh += (pt>0. ? +1 : -1);      
          pl0mcands->insert(l0mcand);
        }
      }
    }
    if (m_nCandFinalTot+nCandFinal>0) {
      m_averagePt  = ((m_averagePt*m_nCandFinalTot)+sumPt)/(m_nCandFinalTot+nCandFinal); 
      m_averageCh  = ((m_averageCh*m_nCandFinalTot)+sumCh)/(m_nCandFinalTot+nCandFinal); 
    }
    m_nCandFinalTot+=nCandFinal;
    if (nCandFinal>0) ++m_nTriggeredEvt;
    ++m_nCandFinalNorm;
  }

  // Candidates selected by the BCSUs (seen by the controller boards)
  if (m_ctrlCandFlagBCSU && (m_mode>1) ) {
    location = taeInTes + LHCb::L0MuonCandidateLocation::CtrlBCSU + l0context;
    LHCb::L0MuonCandidates* pbcsucands = new LHCb::L0MuonCandidates();
    put(pbcsucands , location );
    for (int i= 0; i<2; ++i) {
      for (int iq=0; iq<2; ++iq){
        // Do not write the candidates on TES if an error occured in the decoding
        if (m_ctrlCand[i]->decodingError(iq)>0) continue;
        cands = m_ctrlCand[i]->muonCandidatesBCSU(iq);
        for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
          LHCb::L0MuonCandidate* l0mcand = l0muoncandidate(*itcand);
          if (l0mcand==NULL) {
            if (msgLevel( MSG::ERROR)) error()<<"writeOnTES: ctrl side "<<i<<" BCSU from CB"
                                              <<"\n"<<(*itcand)->dump("\t=> ")<< endmsg;
            continue;
          }
          pbcsucands->insert(l0mcand);
        }
      }
    }
  }

  // Candidates selected by the BCSUs (sent by the processing boards)
  if (m_procCandFlag && (m_mode>0) ) {
    location = taeInTes + LHCb::L0MuonCandidateLocation::BCSU + l0context;
    LHCb::L0MuonCandidates* ppucands = new LHCb::L0MuonCandidates();
    put(ppucands , location );
    for (int i= 0; i<4; ++i) {      
      for (int ib = 0; ib<12; ++ib) {
        // Do not write the candidates on TES if an error occured in the decoding
        if (m_procCand[i]->decodingError(ib)>0) continue;
        cands = m_procCand[i]->muonCandidatesBCSU(ib);
        for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
          LHCb::L0MuonCandidate* l0mcand = l0muoncandidate(*itcand);
          if (l0mcand==NULL) {
            if (msgLevel( MSG::ERROR)) error()<<"writeOnTES: proc Q"<<(i+1)<<" BCSU from PB"
                                              <<"\n"<<(*itcand)->dump("\t=> ")<< endmsg;
            continue;
          }
          ppucands->insert(l0mcand);
        }
      }
    }
  }
  
  // Candidates found by the PUs
  if (m_procCandFlag && (m_mode>0) ) {
    location = taeInTes + LHCb::L0MuonCandidateLocation::PU  + l0context;
    LHCb::L0MuonCandidates* ppucands = new LHCb::L0MuonCandidates();
    put(ppucands , location );
    for (int i= 0; i<4; ++i) {      
      for (int ib = 0; ib<12; ++ib) {
        // Do not write the candidates on TES if an error occured in the decoding
        if (m_procCand[i]->decodingError(ib)>0) continue;
        cands = m_procCand[i]->muonCandidatesPU(ib);
        for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
          LHCb::L0MuonCandidate* l0mcand = l0muoncandidate(*itcand);
          if (l0mcand==NULL) {
            if (msgLevel( MSG::ERROR)) error()<<"writeOnTES: proc Q"<<(i+1)<<" PU"
                                              <<"\n"<<(*itcand)->dump("\t=> ")<< endmsg;
            continue;
          }
          ppucands->insert(l0mcand);
        }
      }
    }
  }
  
  // Data received by the PUs
  if (m_procDataFlag && (m_mode>0) ){
    location = taeInTes + LHCb::L0MuonDataLocation::Default + l0context;
    LHCb::L0MuonDatas* pdata = new LHCb::L0MuonDatas();
    put(pdata , location );
    for (int i= 0; i<4; ++i) {
      std::vector<LHCb::MuonTileID>  pus = m_procData[i]->pus();
      for (std::vector<LHCb::MuonTileID>::iterator itpu=pus.begin(); itpu!=pus.end(); ++itpu){
        std::vector<LHCb::MuonTileID> ols = m_procData[i]->ols(*itpu);
        std::vector<LHCb::MuonTileID> neighs;
        if (m_mode>1) neighs = m_procData[i]->neighs(*itpu);
        LHCb::L0MuonData *l0muondata = new LHCb::L0MuonData(*itpu, ols, neighs);
        pdata->insert(l0muondata);
      }
    }
  }
  

  // Errors
  location = taeInTes + LHCb::L0MuonErrorLocation::ProcPU + l0context;
  LHCb::L0MuonErrors* pl0merrors_proc_pu = new LHCb::L0MuonErrors();
  put(pl0merrors_proc_pu, location );

  location = taeInTes + LHCb::L0MuonErrorLocation::ProcBCSU + l0context;
  LHCb::L0MuonErrors* pl0merrors_proc_bcsu = new LHCb::L0MuonErrors();
  put(pl0merrors_proc_bcsu, location );

  location = taeInTes + LHCb::L0MuonErrorLocation::CtrlBCSU + l0context;
  LHCb::L0MuonErrors* pl0merrors_ctrl_bcsu = new LHCb::L0MuonErrors();
  put(pl0merrors_ctrl_bcsu, location );

  location = taeInTes + LHCb::L0MuonErrorLocation::CtrlCUSU + l0context;
  LHCb::L0MuonErrors* pl0merrors_ctrl_cusu = new LHCb::L0MuonErrors();
  put(pl0merrors_ctrl_cusu, location );

  int status=0;
  
  // Controller boards
  for (int is= 0; is<2; ++is) 
  { // Loop over sides (A&C)
    for (int i=0; i<2; ++i) 
    { // Loop over the quarters in side
      int iq=is*2+i;
      if ( m_ctrlCand[is]->inError(i) ) 
      { // Error detected in CU or SU
        LHCb::MuonTileID mid(0);
        mid.setQuarter(iq);
        LHCb::L0MuonError * l0merror = new LHCb::L0MuonError(mid);
        l0merror->setDecoding(m_ctrlCand[is]->decodingError(i));
        l0merror->setHardware(m_ctrlCand[is]->hardwareError(i));
        l0merror->setBcid(m_ctrlCand[is]->bcidError(i));
        l0merror->setStatus(m_ctrlCand[is]->statusError(i));
        pl0merrors_ctrl_cusu->insert(l0merror);
        
        int stat = m_ctrlCand[is]->statusError(i);
        if (3==(stat&0x3)) status|=stat;
        else status|=(stat&0xC);

      } // End of Error detected on a CU or SU
      for (int ib=0; ib<12; ++ib)
      { // Loop over processing boards in quarter
        if ( m_ctrlCand[is]->inError(i,ib) ) 
        { // Error detected on a BCSU 
          LHCb::MuonTileID mid=m_ctrlCand[is]->mid_BCSU(i,ib);
          LHCb::L0MuonError * l0merror = new LHCb::L0MuonError(mid);
          l0merror->setDecoding(m_ctrlCand[is]->decodingError(i));
          l0merror->setHardware(m_ctrlCand[is]->hardwareError(i,ib));
          l0merror->setBcid(m_ctrlCand[is]->bcidError(i,ib));
          l0merror->setStatus(m_ctrlCand[is]->statusError(i,ib));
          pl0merrors_ctrl_bcsu->insert(l0merror);
        } // End of error detected on a BCSU
      } // End of loop over processing boards in quarter
    } // End of loop over the quarters in side
  } // End of loop over sides (A&C)

  // Processing boards
  for (int iq= 0; iq<4; ++iq) 
  { // Loop over quarters 
    for (int ib=0; ib<12; ++ib)
    { // Loop over processing boards in quarter
      if ( m_procCand[iq]->inError(ib) ) 
      { // Error detected on a BCSU
        LHCb::MuonTileID mid=m_procCand[iq]->mid_BCSU(ib);
        LHCb::L0MuonError * l0merror = new LHCb::L0MuonError(mid);
        l0merror->setDecoding(m_procCand[iq]->decodingError(ib));
        l0merror->setHardware(m_procCand[iq]->hardwareError(ib));
        l0merror->setBcid(m_procCand[iq]->bcidError(ib));
        l0merror->setStatus(m_procCand[iq]->statusError(ib));
        pl0merrors_proc_bcsu->insert(l0merror);
      } // End of Error detected on a BCSU
      for (int ipu=0; ipu<4; ++ipu) 
      { // Loop over PU in a board
        if ( (m_procCand[iq]->inError(ib,ipu)) || (m_procData[iq]->inError(ib,ipu)) ) 
        { // Error detected on a PU
          LHCb::MuonTileID mid=m_procCand[iq]->mid_PU(ib,ipu);
          LHCb::L0MuonError * l0merror = new LHCb::L0MuonError(mid);
          l0merror->setDecoding(m_procCand[iq]->decodingError(ib)+m_procData[iq]->decodingError(ib));
          l0merror->setHardware(m_procData[iq]->hardwareError(ib,ipu));
          l0merror->setBcid(m_procCand[iq]->bcidError(ib,ipu));
          l0merror->setStatus(m_procCand[iq]->statusError(ib,ipu));
          pl0merrors_proc_pu->insert(l0merror);
        } // End of error detected on a PU
      } // End of loop over PU in a board
    } // End of loop over processing boards in quarter
  } // End of loop over quarters


  // Info
  location = taeInTes + LHCb::L0MuonInfoLocation::Default + l0context;
  LHCb::L0MuonInfo* pl0minfo = new LHCb::L0MuonInfo(m_l0EventNumber,m_l0_B_Id,status);
  put(pl0minfo, location );
  
  return StatusCode::SUCCESS;
}


StatusCode L0MuonOutputs::writeL0ProcessorData(){

  unsigned long cu[4],su[4];
  unsigned long  index[4];
  for (int iq=0;iq<4;iq++) {
    cu[iq] = L0DUBase::EmptyData ;
    su[iq] = L0DUBase::EmptyData ;
    index[iq] = 0;
  }
  
  for (int i= 0; i<2; ++i) {
    std::vector<L0Muon::PMuonCandidate> cands = m_ctrlCand[i]->muonCandidates();
    for ( std::vector<L0Muon::PMuonCandidate>::iterator itcand = cands.begin();itcand!=cands.end();++itcand ) {      
      int quarter = (*itcand)->quarter();
      int addM3 = ((*itcand)->colM3()&0x3) + (((*itcand)->rowM3()<<5)&0x7C);
      if (msgLevel( MSG::VERBOSE )) {
        verbose() << "writeL0ProcessorData: \tquarter= "<< quarter  << endmsg;
        verbose() << "writeL0ProcessorData: \tpT= "<< (*itcand)->pT()  << endmsg;
        verbose() << "writeL0ProcessorData: \tPU= "<< (*itcand)->pu()  << endmsg;
        verbose() << "writeL0ProcessorData: \tboard= "<< (*itcand)->board()  << endmsg;
        verbose() << "writeL0ProcessorData: \taddM3= "<< addM3  << endmsg;
        verbose() << "writeL0ProcessorData: \tcharge= "<< (*itcand)->charge()  << endmsg;
        verbose() << "writeL0ProcessorData: --- "<< endmsg;
      }
    
      if (index[quarter]==0) {
        cu[quarter] += ( (*itcand)->pT()     << L0DUBase::Muon::Pt1::Shift) & L0DUBase::Muon::Pt1::Mask;
        cu[quarter] += (      addM3          << L0DUBase::Muon::Address1::Shift) & L0DUBase::Muon::Address1::Mask;
        su[quarter] += ( (*itcand)->pu()     << L0DUBase::Muon::Pu1::Shift) & L0DUBase::Muon::Pu1::Mask;
        su[quarter] += ( (*itcand)->board()  << L0DUBase::Muon::Pb1::Shift) & L0DUBase::Muon::Pb1::Mask;
        su[quarter] += ( (*itcand)->charge() << L0DUBase::Muon::Sign1::Shift) & L0DUBase::Muon::Sign1::Mask;  
      } else {
        cu[quarter] += ( (*itcand)->pT()     << L0DUBase::Muon::Pt2::Shift) & L0DUBase::Muon::Pt2::Mask;
        cu[quarter] += (      addM3          << L0DUBase::Muon::Address2::Shift) & L0DUBase::Muon::Address2::Mask;
        su[quarter] += ( (*itcand)->pu()     << L0DUBase::Muon::Pu2::Shift) & L0DUBase::Muon::Pu2::Mask;
        su[quarter] += ( (*itcand)->board()  << L0DUBase::Muon::Pb2::Shift) & L0DUBase::Muon::Pb2::Mask;
        su[quarter] += ( (*itcand)->charge() << L0DUBase::Muon::Sign2::Shift) & L0DUBase::Muon::Sign2::Mask;  
      }
      index[quarter]++;
    }
  }
  
  if (msgLevel( MSG::VERBOSE )) {
    for (int iq=0;iq<4;iq++) {
      verbose() << "writeL0ProcessorData: number of candidates = "<< index[iq] << endmsg;;
      verbose() << "writeL0ProcessorData: cu["<<iq<<"] = " << cu[iq] << endmsg;;
      verbose() << "writeL0ProcessorData: su["<<iq<<"] = " << su[iq] << endmsg;;
    }
  }
  

  // Fill the keyedObject for the L0DU
  LHCb::L0ProcessorData* l0CU0 = new LHCb::L0ProcessorData( L0DUBase::Fiber::MuonCU0 , cu[0]); 
  LHCb::L0ProcessorData* l0SU0 = new LHCb::L0ProcessorData( L0DUBase::Fiber::MuonSU0 , su[0]); 
  LHCb::L0ProcessorData* l0CU1 = new LHCb::L0ProcessorData( L0DUBase::Fiber::MuonCU1 , cu[1]); 
  LHCb::L0ProcessorData* l0SU1 = new LHCb::L0ProcessorData( L0DUBase::Fiber::MuonSU1 , su[1]); 
  LHCb::L0ProcessorData* l0CU2 = new LHCb::L0ProcessorData( L0DUBase::Fiber::MuonCU2 , cu[2]); 
  LHCb::L0ProcessorData* l0SU2 = new LHCb::L0ProcessorData( L0DUBase::Fiber::MuonSU2 , su[2]); 
  LHCb::L0ProcessorData* l0CU3 = new LHCb::L0ProcessorData( L0DUBase::Fiber::MuonCU3 , cu[3]); 
  LHCb::L0ProcessorData* l0SU3 = new LHCb::L0ProcessorData( L0DUBase::Fiber::MuonSU3 , su[3]); 
  LHCb::L0ProcessorDatas* l0MuonDatas = new LHCb::L0ProcessorDatas();
  l0MuonDatas->insert( l0CU0 ) ;
  l0MuonDatas->insert( l0SU0 ) ; 
  l0MuonDatas->insert( l0CU1 ) ;
  l0MuonDatas->insert( l0SU1 ) ; 
  l0MuonDatas->insert( l0CU2 ) ;
  l0MuonDatas->insert( l0SU2 ) ; 
  l0MuonDatas->insert( l0CU3 ) ;
  l0MuonDatas->insert( l0SU3 ) ; 
  put(  l0MuonDatas ,  LHCb::L0ProcessorDataLocation::Muon );

  return StatusCode::SUCCESS;
}

StatusCode L0MuonOutputs::releaseRegisters(){

  for (int i= 0; i<2; ++i) m_ctrlCand[i]->release();
  for (int i= 0; i<4; ++i) m_procCand[i]->release();
  for (int i= 0; i<4; ++i) m_procData[i]->release();

  return StatusCode::SUCCESS;
}

std::vector<unsigned int> L0MuonOutputs::DC06RawBanks(){
  //
  // Special encoder for DC06 since only 5 TELLS were foreseen at this time
  //
  
  int size= 31;
  int max = 32;

  std::vector<unsigned int> data;
  int word=0;

  // 1st word 
  // [13- 0] : evt number -> to be filled !!
  // [27-14] : evt number -> to be filled !!
  // [31-28] : error (CU) = 0

  data.push_back(word);

  // 2nd word
  // [ 3- 0] : error (SU) = 0
  // [ 7- 4] : number of candidates
  // [32- 8] : beginning of first candidate
  int ncandidates=0;
  for (int i= 0; i<2; ++i) {
    ncandidates += m_ctrlCand[i]->muonCandidates().size();
  }
  word=( (ncandidates<<4) & 0xF0);

  // End of 2nd word and next words ...
  int cursor = 8;
  bool finished=false;
  // Loop over the 2 converters
  for (int i= 0; i<2; ++i) {
    std::vector<unsigned int> cands;
    m_ctrlCand[i]->rawBankDC06(cands);
    // Loop over the candidates
    for ( std::vector<unsigned int>::iterator itcand = cands.begin();itcand!=cands.end();++itcand ) {
      unsigned int shifted = ( ( (*itcand) <<     cursor   ) & 0xFFFFFFFF );
      unsigned int left    = ( ( (*itcand) >> (max-cursor) ) & 0xFFFFFFFF );
      word+=shifted;
      cursor+=size;
      finished=true;
      if (cursor>=max) {
        data.push_back(word);
        finished=false;
        cursor-=max;
        word = left;
      }
    }
  }
  if (!finished) data.push_back(word);

  return data;
}

LHCb::L0MuonCandidate* L0MuonOutputs::l0muoncandidate(L0Muon::PMuonCandidate cand)
{
  bool debugFlag = false;
  //  if (msgLevel( MSG::VERBOSE )) debug=true;
  
  std::vector<LHCb::MuonTileID> pads = L0Muon::add2pads(cand->quarter(),cand->board(),cand->pu(),
                                                        cand->colM3(),cand->rowM3(), cand->offM2(),cand->offM1(),
                                                        m_version,debugFlag);
  if (pads.size()==0) {
    if (msgLevel( MSG::ERROR)) error()<<"Wrong address for candidate"<<endmsg;
    return NULL;
  }
  std::vector<double> kine = L0Muon::kine(pads[0],pads[1],m_version,debugFlag);

  int encodedPT=cand->pT()+((cand->charge()<<7)&0x80);
  double pt = cand->pT()*40.;
  
  //   debug()<<" L0MuonOutputs::l0muoncandidate "<<l0context
  //          <<std::hex
  //          <<" encodedPT=0x"<<encodedPT
  //          <<" candpT= 0x"<<cand->pT()
  //          <<std::dec
  //          <<" charge= "<<cand->charge() 
  //          <<" pt= "<<kine[0]<<endmsg;
  LHCb::L0MuonCandidate *pl0muoncandidate = new LHCb::L0MuonCandidate(pt, kine[1], kine[2], pads, encodedPT);
  return pl0muoncandidate;
}


void L0MuonOutputs::statTot(MsgStream & os) const{
  if (0!=m_nCandFinalNorm)
  {  
    os << "- Total number of candidates                 : "
       <<format("%8d",m_nCandFinalTot) <<endmsg;
    os << "- Number of Bx with at least 1 candidate     : "
       <<format("%8d",m_nTriggeredEvt) 
       <<format("  (%6.2f%%)",(100.*m_nTriggeredEvt)/m_nCandFinalNorm) << endmsg;
    os << "- Number of candidate per Bx                 : "
       <<format("      %5.2f",(1.*m_nCandFinalTot)/m_nCandFinalNorm) << endmsg;
    os << "- Average Pt of the candidates               : "
       <<format("    %7.2f MeV",m_averagePt)<<endmsg;
    os << "- Average charge of the candidates           : "
       <<format("      %5.2f (+/-%4.2f)",m_averageCh,sqrt((1-(m_averageCh*m_averageCh))/m_nCandFinalNorm))<<endmsg;
  }
  
  if (0!=m_rawBankNorm)
  {  
    os << "- Average raw banks size                     : "
       <<format("  %8.1f Bytes",(4.*m_rawBankSizeTot)/m_rawBankNorm)<<endmsg;
  }

}

void L0MuonOutputs::errorSummary(MsgStream & msg) const{

  msg<<"--- L0Muon decoding error summary"<<endmsg;
  for (int i= 0; i<2; ++i) {
    for (int ii=0; ii<2; ++ii) {
      const L0Muon::CtrlCandErrors *ctrlCandError = m_ctrlCand[i]->errors(ii);
      
      std::string err;

      if (!ctrlCandError->present.strCounter(err)) break;

      if (ii==0) {
        msg<<"L0MuonCtrlCand "<<"Side ";
        switch (i){
        case 0:
          msg<<"A";
          break;
        case 1:
          msg<<"C";
          break;
        }
        msg<<" -> "<<err<<endmsg;
      }

      if (ctrlCandError->sumCounters()==0) continue;
      
      msg<<"L0MuonCtrlCand "<<"Q"<<(i*2+ii+1)<<" ERROR "<<endmsg;
      if (ctrlCandError->decoding.strCounter(err)) msg<<"  "<<err<<endmsg;
      if (ctrlCandError->l0EventNumber.strCounter(err)) msg<<"  "<<err<<endmsg;
      if (ctrlCandError->l0_B_Id.strCounter(err)) msg<<"  "<<err<<endmsg;
      if (ctrlCandError->errJ.strCounter(err)) msg<<"  "<<err<<endmsg;
      if (ctrlCandError->errK.strCounter(err)) msg<<"  "<<err<<endmsg;
      if (ctrlCandError->best_bcid.strCounter(err)) msg<<"  "<<err<<endmsg;
      if (ctrlCandError->best_status.strCounter(err)) msg<<"  "<<err<<endmsg;
      for (int ib=0; ib<12; ++ib) {
        if (ctrlCandError->bcsu_bcid[ib].strCounter(err)) msg<<"  "<<err<<endmsg;
        if (ctrlCandError->bcsu_bcid_mismatch[ib].strCounter(err)) msg<<"  "<<err<<endmsg;
        if (ctrlCandError->bcsu_status[ib].strCounter(err)) msg<<"  "<<err<<endmsg;
        for (int ich=0; ich<2; ++ich) {
          if (ctrlCandError->link[ib][ich].strCounter(err)) msg<<"  "<<err<<endmsg;
        }
      }
    }
  }

  
  
  for (int i= 0; i<4; ++i) {
    for (int ib=0; ib<12; ++ib) {
      const L0Muon::ProcCandErrors *procCandError = m_procCand[i]->errors(ib);

      std::string err;

      if (!procCandError->present.strCounter(err)) break;

      if (ib==0) msg<<"L0MuonProcCand "<<"Q"<<(i+1)<<"     -> "<<err<<endmsg;
      
      if (procCandError->sumCounters()==0) continue;

      msg<<"L0MuonProcCand "<<"Q"<<(i+1);
      msg<<" PB";
      msg.width(2);
      msg<<ib;
      msg<<" ERROR "<<endmsg;

      if (procCandError->decoding.strCounter(err)) msg<<"  "<<err<<endmsg;
      if (procCandError->l0EventNumber.strCounter(err)) msg<<"  "<<err<<endmsg;
      if (procCandError->l0_B_Id.strCounter(err)) msg<<"  "<<err<<endmsg;
      if (procCandError->errJ.strCounter(err)) msg<<"  "<<err<<endmsg;
      if (procCandError->errK.strCounter(err)) msg<<"  "<<err<<endmsg;
      if (procCandError->errH.strCounter(err)) msg<<"  "<<err<<endmsg;
      if (procCandError->errF.strCounter(err)) msg<<"  "<<err<<endmsg;
      if (procCandError->bcsu_status.strCounter(err)) msg<<"  "<<err<<endmsg;
      if (procCandError->bcsu_bcid.strCounter(err)) msg<<"  "<<err<<endmsg;
      for (int ipu=0; ipu<4; ++ipu){
        if (procCandError->pus_status[ipu].strCounter(err)) msg<<"  "<<err<<endmsg;
        if (procCandError->pus_bcid[ipu].strCounter(err)) msg<<"  "<<err<<endmsg;
      }
    }
  }

  for (int i= 0; i<4; ++i) {
    for (int ib=0; ib<12; ++ib) {
      const L0Muon::ProcDataErrors *procDataError = m_procData[i]->errors(ib);
      
      std::string err;
      
      if (!procDataError->present.strCounter(err)) break;
      
      if (ib==0) msg<<"L0MuonProcData "<<"Q"<<(i+1)<<"     -> "<<err<<endmsg;

      if (procDataError->sumCounters()==0) continue;

      msg<<"L0MuonProcData "<<"Q"<<(i+1)<<" PB";
      msg.width(2);
      msg<<ib;
      msg<<" ERROR "<<endmsg;
      
      if (procDataError->decoding.strCounter(err)) msg<<"  "<<err<<endmsg;
      for (int ipu=0; ipu<4; ++ipu){
        if (procDataError->opt_link[ipu].strCounter(err)) msg<<"  "<<err<<endmsg;
        if (procDataError->par_link[ipu].strCounter(err)) msg<<"  "<<err<<endmsg;
        if (procDataError->ser_link[ipu].strCounter(err)) msg<<"  "<<err<<endmsg;
      }
    }
  }

  msg<<"--- End of L0Muon decoding error summary"<<endmsg;
  
}

void L0MuonOutputs::errors(MsgStream & msg) const{

  bool first=true;
  
  msg<<"--- L0Muon decoding error"<<endmsg;
  for (int i= 0; i<2; ++i) {
    for (int ii=0; ii<2; ++ii) {

      bool error=false;
      if (m_ctrlCand[i]->inError(ii)) error=true;
      for (int ib=0; ib<12; ++ib){
        if (m_ctrlCand[i]->inError(ii,ib)) {
          error=true;
          break;
        }
      }
      if ( (!error) && (msg.level()!=MSG::VERBOSE) ) continue;
      
      const L0Muon::CtrlCandErrors *ctrlCandError = m_ctrlCand[i]->errors(ii);

      if (ctrlCandError->present.value()==0) continue;
      
      if (first) {
        msg<<"Errors-L0MuonCtrlCand Q# : ";
        msg<<" decoding --  evt bid JK bids - best --  ";
        msg.width(36);
        msg<<"        link errors 0-11         ";
        msg<<" ";
        msg.width(12);
        msg<<" mismatchs ";
        msg<<" - ";
        msg.width(12);
        msg<<" bcids 0-11 ";
        msg<<" - ";
        msg.width(12);
        msg<<" status 0-11 ";
        
        msg<<endmsg;
        first=false;
      }
      
      msg<<"Errors-L0MuonCtrlCand "<<"Q";
      msg.width(1);
      msg<<(i*2+ii+1);
      msg<<" : ";
      
      msg<<ctrlCandError->decoding;
      msg<<" -- ";
      msg<<ctrlCandError->l0EventNumber;
      msg<<ctrlCandError->l0_B_Id;
      msg<<" ";
      msg<<ctrlCandError->errJ;
      msg<<ctrlCandError->errK;
      msg<<" ";
      for (int ich=0; ich<2; ++ich) {
        msg<<ctrlCandError->ctrl_bcid[ich];
      }
      msg<<" - ";
      msg<<ctrlCandError->best_bcid     ;
      msg<<ctrlCandError->best_status;
      msg<<" -- ";
      msg<<" ";
      for (int ib=0; ib<12; ++ib) {
        for (int ich=0; ich<2; ++ich) {
          msg<<ctrlCandError->link[ib][ich];
        }
        msg<<" ";
      }
      msg<<" ";
      for (int ib=0; ib<12; ++ib) {
        msg<<ctrlCandError->bcsu_bcid_mismatch[ib];
      }
      msg<<" - ";
      for (int ib=0; ib<12; ++ib) {
        msg<<ctrlCandError->bcsu_bcid[ib];
      }
      msg<<" - ";
      for (int ib=0; ib<12; ++ib) {
        msg<<ctrlCandError->bcsu_status[ib];
      }
      msg<<endmsg;
    }
  }

  first = true;
  for (int i= 0; i<4; ++i) {
    for (int ib=0; ib<12; ++ib) {
      
      bool error=false;
      if (m_procCand[i]->inError(ib)) error=true;
      for (int ipu=0; ipu<4; ++ipu){
        if (m_procCand[i]->inError(ib,ipu)) {
          error=true;
          break;
        }
      }
      if ( (!error) && (msg.level()!=MSG::VERBOSE) ) continue;
      
      const L0Muon::ProcCandErrors *procCandError = m_procCand[i]->errors(ib);
      
      if (procCandError->present.value()==0) continue;

      if (first) {
        msg<<"Errors-L0MuonProcCand Q# PB#  : ";
        msg<<" decoding --  evt bid JKHF --  bcids  -  status";
        msg<<endmsg;
        first=false;
      }
      
      msg<<"Errors-L0MuonProcCand "<<"Q";
      msg.width(1);
      msg<<(i+1);
      msg<<" PB";
      msg.width(2);
      msg<<ib;
      msg<<" : ";

      msg<<procCandError->decoding;
      msg<<" -- ";      
      msg<<procCandError->l0EventNumber;
      msg<<procCandError->l0_B_Id;
      msg<<" ";
      msg<<procCandError->errJ;
      msg<<procCandError->errK;
      msg<<procCandError->errH;
      msg<<procCandError->errF;
      msg<<" -- ";
      msg<<procCandError->bcsu_bcid;
      msg<<" ";
      for (int ipu=0; ipu<4; ++ipu){
        msg<<procCandError->pus_bcid[ipu];
      }
      msg<<" - ";
      msg<<procCandError->bcsu_status;
      msg<<" ";
      for (int ipu=0; ipu<4; ++ipu){
        msg<<procCandError->pus_status[ipu];
      }
      
      msg<<endmsg;
    }
  }

  first=true;
  for (int i= 0; i<4; ++i) {
    for (int ib=0; ib<12; ++ib) {
      
      for (int ipu=0; ipu<4; ++ipu){
 
        if ( (!m_procData[i]->inError(ib,ipu)) && (msg.level()!=MSG::VERBOSE) ) continue;
        
        const L0Muon::ProcDataErrors *procDataError = m_procData[i]->errors(ib);
        
        if (procDataError->present.value()==0) continue;
      
        if (first) {
          msg<<"Errors-L0MuonProcData Q# PB#  PU# : ";
          msg<<" decoding -- opt par ser";
          msg<<endmsg;
          first=false;
        }
      
      msg<<"Errors-L0MuonProcData "<<"Q";
      msg.width(1);
      msg<<(i+1);
      msg<<" PB";
      msg.width(2);
      msg<<ib;
      msg<<" PU";
      msg.width(1);
      msg<<ipu;
      msg<<" : ";

      msg<<procDataError->decoding;
      msg<<" - ";      
      msg<<procDataError->opt_link[ipu];
      msg<<" ";
      msg<<procDataError->par_link[ipu];
      msg<<" ";
      msg<<procDataError->ser_link[ipu];
      msg<<endmsg;
      }
      
    }
  }
  
  msg<<"--- End of L0Muon decoding error"<<endmsg;
  
}

void L0MuonOutputs::monitorBanks(MsgStream & msg) {

  int nCandFinal  = 0;
  double sumPt = 0;
  double sumCh = 0;
 
  std::vector<L0Muon::PMuonCandidate> cands ;
  std::vector<L0Muon::PMuonCandidate>::iterator itcand;
  std::string location;
    
  // Candidates selected by the controller boards
  // L0Muon (always there - light, standard and debug modes)
  msg << "monitorBanks -------------------------"<< endmsg;
  for (int i= 0; i<2; ++i) {
    msg << "monitorBanks: side "<<i<< endmsg;
    m_ctrlCand[i]->dump("\t=> ");
    cands = m_ctrlCand[i]->muonCandidates();
    nCandFinal+=cands.size();
    msg << "monitorBanks: => "<<cands.size()<<" candidates found"<< endmsg;
    for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
      msg << "monitorBanks:\n"<<(*itcand)->dump("\t=> ")<< endmsg;
    }
  }
  if (m_nCandFinalTot+nCandFinal>0) {
    m_averagePt  = ((m_averagePt*m_nCandFinalTot)+sumPt)/(m_nCandFinalTot+nCandFinal); 
    m_averageCh  = ((m_averageCh*m_nCandFinalTot)+sumCh)/(m_nCandFinalTot+nCandFinal); 
  }
  m_nCandFinalTot+=nCandFinal;
  if (nCandFinal>0) ++m_nTriggeredEvt;
  ++m_nCandFinalNorm;

  // Candidates selected by the BCSUs
  msg << "monitorBanks -------------------------"<< endmsg;
  for (int i= 0; i<2; ++i) {
    msg << "monitorBanks: side "<<i<< endmsg;
    cands = m_ctrlCand[i]->muonCandidatesBCSU();
    msg << "monitorBanks: => "<<cands.size()<<" candidates found (BCSU)"<< endmsg;
    for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
      msg << "monitorBanks:\n"<<(*itcand)->dump("\t=> ")<< endmsg;
    }
  }

  // Candidates found by the PUs
  msg << "monitorBanks -------------------------"<< endmsg;
  for (int i= 0; i<4; ++i) {
    msg << "monitorBanks: quarter "<<i<< endmsg;
    cands = m_procCand[i]->muonCandidatesPU();
    msg << "monitorBanks: => "<<cands.size()<<" candidates found (PU)"<< endmsg;
    for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
      msg << "monitorBanks:\n"<<(*itcand)->dump("\t=> ")<< endmsg;
    }
  }

  // Data received by the PUs
  msg << "monitorBanks -------------------------"<< endmsg;
  for (int i= 0; i<4; ++i) {
    msg << "monitorBanks: quarter "<<i<< endmsg;
    std::vector<LHCb::MuonTileID>  pus = m_procData[i]->pus();
    for (std::vector<LHCb::MuonTileID>::iterator itpu=pus.begin(); itpu!=pus.end(); ++itpu){
      //msg << "monitorBanks: pu = "<<itpu->toString()<< endmsg;
      std::vector<LHCb::MuonTileID> ols = m_procData[i]->ols(*itpu);
      //msg << "monitorBanks: ols length = "<<ols.size()<< endmsg;
      std::vector<LHCb::MuonTileID> neighs = m_procData[i]->neighs(*itpu);
      //msg << "monitorBanks: neighs length = "<<neighs.size()<< endmsg;
    }
  }

}

