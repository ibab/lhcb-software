// Include files 

#include "boost/format.hpp"

// from Event
#include "Event/RecHeader.h"
#include "Event/RawEvent.h"
#include "Event/L0MuonData.h"
#include "Event/L0MuonInfo.h"
#include "Event/L0MuonProcError.h"
#include "Event/L0MuonCtrlError.h"

// local
#include "L0MuonFromRawTrans.h"

#include "L0MuonKernel/ProcUtilities.h"
#include "L0MuonKernel/L0MuonKernelFromXML.h"
#include "ProcessorKernel/RegisterFactory.h"

#include "SubstituteEnvVarInPath.h"
//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonFromRawTrans
//
// 2008-01-10 : Julien Cogan
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0MuonFromRawTrans )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonFromRawTrans::L0MuonFromRawTrans( const std::string& name,
                                        ISvcLocator* pSvcLocator) 
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "ConfigFile" , m_configfile="$PARAMFILESROOT/data/L0MuonKernel.xml" );
  declareProperty( "DumpErrors"  , m_dumpError=false      );

  m_version=0;
  m_l0EventNumber=-1;
  m_l0_B_Id=-1;
  
  // Converters for the banks of the 2 TELL1s connected to the controller boards
  for (int i= 0; i<2; ++i) {
    m_ctrlRaw[i] =  new L0Muon::CtrlRawCnv(i);
  }
  // Converters for the banks of the 4 TELL1s connected to the processing boards
  for (int i= 0; i<4; ++i) {
    m_procRaw[i] = new L0Muon::ProcRawCnv(i);
  }  

}
//=============================================================================
// Destructor
//=============================================================================
L0MuonFromRawTrans::~L0MuonFromRawTrans() {
  // Converters for the banks of the 2 TELL1s connected to the controller boards
  for (int i= 0; i<2; ++i) {
    delete m_ctrlRaw[i];
 }
  // Converters for the banks of the 4 TELL1s connected to the processing boards
  for (int i= 0; i<4; ++i) {
    delete m_procRaw[i];
  }  
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0MuonFromRawTrans::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  debug() << "==> Initialize" << endmsg;
  L0Muon::RegisterFactory::selectInstance(2);
  // Instanciate the MuonTrigger registers
  std::string xmlFileName = L0MuonUtils::SubstituteEnvVarInPath(m_configfile);
  info() <<  "Configuration file is " << xmlFileName << endmsg;
  L0Muon::L0MuonKernelFromXML(xmlFileName,false);

  m_version=0;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonFromRawTrans::execute() {

  L0Muon::RegisterFactory::selectInstance(2);
  StatusCode sc;

  // Loop over time slots
  for (int bx=-7; bx<8; ++bx){

    setProperty("RootInTES",timeSlot(bx));  
    if (!exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default )) continue;

    sc = decodeRawBanks();
    if ( sc.isFailure() ) {
      return Error("Error in decoding transparent bank",StatusCode::SUCCESS,10);
    }

    if (m_dumpError) {
      sc = dumpErrors();
      if ( sc.isFailure() ) {
        return Error("Error in dumpErrors",StatusCode::SUCCESS,10);
      }
    }

    sc = writeOnTES();
    if ( sc.isFailure() ) {
      return Error("Error in writeOnTES",StatusCode::SUCCESS,10);
    }

    sc = releaseRegisters();
    if ( sc.isFailure() ) {
      return Error("Error in releaseRegisters",StatusCode::SUCCESS,10);
    }
  
  } // End of loop over time slots
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonFromRawTrans::finalize() {

  debug() << "==> Finalize" << endmsg;

//   if (m_dumpError) {
  if (1) {
    std::string errorCounters;

    errorCounters+="\n--- L0Muon decoding error summary\n";
    for (int i= 0; i<2; ++i) {
      switch(i){
      case(0):
        errorCounters+="Side A : ";
        break;
      case(1):
        errorCounters+="Side C : ";
        break;
      }
      errorCounters+=(boost::format("%3d banks decoded : ") % m_ctrlRaw[i]->numberOfDecodedBanks()).str();
      if (m_ctrlRaw[i]->numberOfDecodedBanks()>0 ) {
        m_ctrlRaw[i]->dumpErrorCounters(errorCounters);
      } else errorCounters+="\n";
    }

    for (int i= 0; i<4; ++i) {
      errorCounters+=(boost::format("Q%d : %3d banks decoded : ") % (i+1) % m_procRaw[i]->numberOfDecodedBanks()).str();
      if (m_procRaw[i]->numberOfDecodedBanks()>0 ) {
        m_procRaw[i]->dumpErrorCounters(errorCounters);
      } else errorCounters+="\n";
    }
    errorCounters+="--- End of L0Muon decoding error summary\n";
  
    info() << errorCounters<<endmsg;
  }
  

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================


//=============================================================================
 
StatusCode L0MuonFromRawTrans::decodeRawBanks(){

  LHCb::RawEvent* rawEvt = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
  if (msgLevel( MSG::DEBUG )) debug() << "decodeRawBanks:  ==> got rawEvt " << endmsg;

  m_l0EventNumber=-1;
  m_l0_B_Id=-1;

  //   const std::vector<LHCb::RawBank*>& banks = rawEvt->banks( LHCb::RawBank::L0Muon );
  //  const std::vector<LHCb::RawBank*>& banks = rawEvt->banks( LHCb::RawBank::TT );
  const std::vector<LHCb::RawBank*>& banks = rawEvt->banks( LHCb::RawBank::L0MuonRaw );
  if (banks.size()!=0) {
    for ( std::vector<LHCb::RawBank*>::const_iterator itBnk = banks.begin(); banks.end() != itBnk; ++itBnk ) {
      int srcID = (*itBnk)->sourceID();
      int bankVersion  = (*itBnk)->version();
      m_version=bankVersion;
      std::vector<unsigned int> data;
      unsigned int* body = (*itBnk)->data();
      int size = (*itBnk)->size()/4;
      if (msgLevel( MSG::DEBUG ))
        debug() << "decodeRawBanks: L0Muon bank (version "<< bankVersion <<" ) found"
                <<", sourceID is "<< srcID <<", size is "<< size <<endmsg;
      for ( int k = 0; size > k; ++k ) {
        data.push_back( *body++ );
      }
      if (srcID==0) {
        m_ctrlRaw[1]->decodeBank(data,bankVersion,m_l0EventNumber,m_l0_B_Id);
        if (m_dumpError) {
          for (int ii= 0; ii<2; ++ii) {
            if (m_ctrlRaw[1]->decodingError(ii)) {
              info() << "decodeRawBanks: L0Muon bank (version "<< bankVersion <<" ) found"
                      <<", sourceID is "<< srcID <<", size is "<< size <<endmsg;
              m_ctrlRaw[1]->dump(data);
              break;
            }
          }
        }
      }
      if (srcID==5) {
        m_ctrlRaw[0]->decodeBank(data,bankVersion,m_l0EventNumber,m_l0_B_Id);
        if (m_dumpError) {
          for (int ii= 0; ii<2; ++ii) {
            if (m_ctrlRaw[0]->decodingError(ii)) {
              m_ctrlRaw[0]->dump(data);
              break;
            }
          }
        }
      }
       if (srcID==3){
        m_procRaw[0]->decodeBank(data,bankVersion,m_l0EventNumber,m_l0_B_Id);
        if (m_dumpError) {
          for (int ib= 0; ib<12; ++ib) {
            if (m_procRaw[0]->decodingError(ib)) {
              m_procRaw[0]->dump(data);
              break;
            }
          }
        }
      }
       if (srcID==4){
        m_procRaw[1]->decodeBank(data,bankVersion,m_l0EventNumber,m_l0_B_Id);
        if (m_dumpError) {
          for (int ib= 0; ib<12; ++ib) {
            if (m_procRaw[1]->decodingError(ib)) {
              m_procRaw[1]->dump(data);
              break;
            }
          }
        }
      }
      if (srcID==2) {
        m_procRaw[2]->decodeBank(data,bankVersion,m_l0EventNumber,m_l0_B_Id);      
        if (m_dumpError) {
          for (int ib= 0; ib<12; ++ib) {
            if (m_procRaw[2]->decodingError(ib)) {
              m_procRaw[2]->dump(data);
              break;
            }
          }
        }
      }
       if (srcID==1){
        m_procRaw[3]->decodeBank(data,bankVersion,m_l0EventNumber,m_l0_B_Id);
        if (m_dumpError) {
          for (int ib= 0; ib<12; ++ib) {
            if (m_procRaw[3]->decodingError(ib)) {
              m_procRaw[3]->dump(data);
              break;
            }
          }
        }
      }
     }
  }
  

  debug() << "decodeRawBanks:  ==> done" << endmsg;
  
  return StatusCode::SUCCESS;

}
StatusCode L0MuonFromRawTrans::dumpErrors()
{

  static int ievt=0;
  ++ievt;

  bool quarter_in_error[4];
  for (int iq=0; iq<4; ++iq) quarter_in_error[iq]=false;

  for (int i= 0; i<2; ++i) {// Loop over sides 
    for (int ii= 0; ii<2; ++ii) {// Loop over quarters in side
      int iq=i*2+ii;
      if (m_ctrlRaw[i]->decodingError(ii) || m_ctrlRaw[i]->inError(ii)) {
        quarter_in_error[iq]=true;
        continue;
      }
    } // End of loop over quarters in side
  } // End of loop over sides
  
  for (int iq=0; iq<4; ++iq) { // Loop over quarters
    for (int ib=0; ib<12; ++ib) { // Loop over processing boards
      if (m_procRaw[iq]->decodingError(ib) || m_procRaw[iq]->inError(ib)) {
        quarter_in_error[iq]=true;
        break;
      }
    } // End of loop over processing boards
  } // End of loop over quarters
  

  bool error=false;
  for (int iq=0; iq<4; ++iq) error|=quarter_in_error[iq];
  if (error){ // If an error occured
    std::cout<<boost::format("****** L0Muon Error report evt %d") % ievt<<std::endl;
    for (int i= 0; i<2; ++i) {// Loop over sides
      for (int ii= 0; ii<2; ++ii) {// Loop over quarters in side
        int iq=i*2+ii;
        if (!quarter_in_error[iq]) continue;
        std::cout<<boost::format("** L0Muon Error report for quarter Q%1d") % (iq+1)<<std::endl;
        m_ctrlRaw[i]->dumpErrorHeader(ii,"\t");
        m_ctrlRaw[i]->dumpErrorField(ii,"\t");
        m_procRaw[iq]->dumpErrorHeader(0,"\t     ");      
        for (int ib= 0; ib<12; ++ib) {
          m_procRaw[iq]->dumpErrorField(ib,(boost::format("\tPB%2d ") % (ib)).str());      
        }
      } // End of loop over quarters in side
    } // End of loop over sides
  } // End if an error occured
  
  return StatusCode::SUCCESS;
  
}

StatusCode L0MuonFromRawTrans::writeOnTES(){

  if ( (m_version<1) || (m_version>2) ) {
    return Error("Wrong version number",StatusCode::FAILURE,10);;
  }

  int static ievt=0;
  ++ievt;
  
  std::vector<L0Muon::PMuonCandidate> cands ;
  std::vector<L0Muon::PMuonCandidate>::iterator itcand;

  std::string location;
  location = LHCb::L0MuonCandidateLocation::Default + context();
  LHCb::L0MuonCandidates* pl0mcands = new LHCb::L0MuonCandidates();
  put(pl0mcands , location );
  if (msgLevel( MSG::DEBUG )) {
    debug() << "writeOnTES -------------------------"<< endmsg;
    debug() << "writeOnTES at "<< location << endmsg;
  }
  for (int i= 0; i<2; ++i) {
    if (!m_ctrlRaw[i]->isActiv()) continue;
    cands = m_ctrlRaw[i]->muonCandidates();
    if (msgLevel( MSG::DEBUG )) {
      debug() << "writeOnTES: side "<<i<< endmsg;
      debug() << "writeOnTES: => "<<cands.size()<<" candidates found"<< endmsg;
    }
    for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
      //       if (msgLevel( MSG::DEBUG )) debug() << "writeOnTES:\n"<<(*itcand)->dump("\t=> ")<< endmsg;
      LHCb::L0MuonCandidate* l0mcand = l0muoncandidate(*itcand,m_version);
      if (l0mcand==NULL) {
        if (msgLevel( MSG::ERROR)) error()<<"writeOnTES: ctrl side "<<i
                                          <<" count# "<<ievt<<"\n"<<(*itcand)->dump("\t=> ")<< endmsg;
        continue;
      }
      pl0mcands->insert(l0mcand);
    }
  } 

  location = LHCb::L0MuonCandidateLocation::BCSU +"fromCB"+ context();
  LHCb::L0MuonCandidates* pbcsucands = new LHCb::L0MuonCandidates();
  put(pbcsucands , location );
  if (msgLevel( MSG::DEBUG )) {
    debug() << "writeOnTES -------------------------"<< endmsg;
    debug() << "writeOnTES at "<< location << endmsg;
  }
  for (int i= 0; i<2; ++i) {
    if (!m_ctrlRaw[i]->isActiv()) continue;
    cands = m_ctrlRaw[i]->muonCandidatesBCSU();
//     if (msgLevel( MSG::DEBUG )) {
//       debug() << "writeOnTES: side "<<i<< endmsg;
//       debug() << "writeOnTES: => "<<cands.size()<<" candidates found (BCSU)"<< endmsg;
//     }
    for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
//       if( msgLevel(MSG::DEBUG) ) debug() << "writeOnTES:\n"<<(*itcand)->dump("\t=> ")<< endmsg;
      LHCb::L0MuonCandidate* l0mcand = l0muoncandidate(*itcand,m_version);
      if (l0mcand==NULL) {
        if (msgLevel( MSG::ERROR)) error()<<"writeOnTES: ctrl side "<<i
                                          <<" count# "<<ievt<<" BCSU from CB\n"<<(*itcand)->dump("\t=> ")<< endmsg;
        continue;
      }
      pbcsucands->insert(l0mcand);
    }
  }

  location = LHCb::L0MuonCandidateLocation::BCSU+"fromPB"  + context();
  LHCb::L0MuonCandidates* pbcsu2cands = new LHCb::L0MuonCandidates();
  put(pbcsu2cands , location );
//   if (msgLevel( MSG::DEBUG )) {
//     debug() << "writeOnTES -------------------------"<< endmsg;
//     debug() << "writeOnTES at "<< location << endmsg;
//   }
  for (int i= 0; i<4; ++i) {      
    if (!m_procRaw[i]->isActiv()) continue;
    cands = m_procRaw[i]->muonCandidatesBCSU();
//     if (msgLevel( MSG::DEBUG )) {
//       debug() << "writeOnTES: quarter "<<i<< endmsg;
//       debug() << "writeOnTES: => "<<cands.size()<<" candidates found (PU)"<< endmsg;
//     }
    for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
//       if (msgLevel( MSG::DEBUG )) debug() << "writeOnTES:\n"<<(*itcand)->dump("\t=> ")<< endmsg;
      LHCb::L0MuonCandidate* l0mcand = l0muoncandidate(*itcand,m_version);
      if (l0mcand==NULL) {
        if (msgLevel( MSG::ERROR)) error()<<"writeOnTES: proc Q"<<(i+1)
                                          <<" count# "<<ievt<<" BCSU from PB\n"<<(*itcand)->dump("\t=> ")<< endmsg;
        continue;
      }
      pbcsu2cands->insert(l0mcand);
    }
  }

  location = LHCb::L0MuonCandidateLocation::PU  + context();
  LHCb::L0MuonCandidates* ppucands = new LHCb::L0MuonCandidates();
  put(ppucands , location );
//   if (msgLevel( MSG::DEBUG )) {
//     debug() << "writeOnTES -------------------------"<< endmsg;
//     debug() << "writeOnTES at "<< location << endmsg;
//   }
  for (int i= 0; i<4; ++i) {      
    if (!m_procRaw[i]->isActiv()) continue;
    cands = m_procRaw[i]->muonCandidatesPU();
//     if (msgLevel( MSG::DEBUG )) {
//       debug() << "writeOnTES: quarter "<<i<< endmsg;
//       debug() << "writeOnTES: => "<<cands.size()<<" candidates found (PU)"<< endmsg;
//     }
    for ( itcand = cands.begin();itcand!=cands.end();++itcand ) {
//       if (msgLevel( MSG::DEBUG )) debug() << "writeOnTES:\n"<<(*itcand)->dump("\t=> ")<< endmsg;
      LHCb::L0MuonCandidate* l0mcand = l0muoncandidate(*itcand,m_version);
      if (l0mcand==NULL) {
        if (msgLevel( MSG::ERROR)) error()<<"writeOnTES: proc Q"<<(i+1)
                                          <<" count# "<<ievt<<" PU\n"<<(*itcand)->dump("\t=> ")<< endmsg;
        continue;
      }
      ppucands->insert(l0mcand);
    }
  }


  location = LHCb::L0MuonDataLocation::Default + context();
  LHCb::L0MuonDatas* pdata = new LHCb::L0MuonDatas();
  put(pdata , location );
//   if (msgLevel( MSG::DEBUG )) {
//     debug() << "writeOnTES -------------------------"<< endmsg;
//     debug() << "writeOnTES at "<< location << endmsg;
//   }
  for (int i= 0; i<4; ++i) {
    if (!m_procRaw[i]->isActiv()) continue;
    if (msgLevel( MSG::DEBUG )) debug() << "writeOnTES: "<<rootInTES()<<" Q"<<(i+1)<< endmsg;
    std::vector<LHCb::MuonTileID>  pus = m_procRaw[i]->pus();   
    if (msgLevel( MSG::DEBUG )) debug() << "writeOnTES: "<<rootInTES()<<" #of pus "<<pus.size()<< endmsg;
    for (std::vector<LHCb::MuonTileID>::iterator itpu=pus.begin(); itpu!=pus.end(); ++itpu){
      debug() << "writeOnTES:  "<<rootInTES()<<"---- pu = "<<itpu->toString()<< endmsg;
      std::vector<LHCb::MuonTileID> ols = m_procRaw[i]->ols(*itpu);
      debug() << "writeOnTES:  "<<rootInTES()<<"ols length = "<<ols.size()<< endmsg;
      std::vector<LHCb::MuonTileID> neighs = m_procRaw[i]->neighs(*itpu);
      debug() << "writeOnTES:  "<<rootInTES()<<"neighs length = "<<neighs.size()<< endmsg;
      debug() << "writeOnTES:  "<<rootInTES()<<"---- end pu "<< endmsg;
      LHCb::L0MuonData *l0muondata = new LHCb::L0MuonData(*itpu, ols, neighs);
      pdata->insert(l0muondata);
    }
  }

  location = LHCb::L0MuonInfoLocation::Default + context();
  LHCb::L0MuonInfo* pl0minfo = new LHCb::L0MuonInfo();
  pl0minfo->setL0EventNumber(m_l0EventNumber);
  pl0minfo->setL0_B_Id(m_l0_B_Id);
  put(pl0minfo, location );
//   if (msgLevel( MSG::DEBUG )) {
//     debug() << "writeOnTES -------------------------"<< endmsg;
//     debug() << "writeOnTES at "<< location << endmsg;
//   }

  location = LHCb::L0MuonCtrlErrorLocation::Default + context();
  LHCb::L0MuonCtrlErrors* pl0mctrlerrors = new LHCb::L0MuonCtrlErrors();
  put(pl0mctrlerrors, location );
//   if (msgLevel( MSG::DEBUG )) {
//     debug() << "writeOnTES -------------------------"<< endmsg;
//     debug() << "writeOnTES at "<< location << endmsg;
//   }
  for (int i= 0; i<2; ++i) {
    if (!m_ctrlRaw[i]->isActiv()) continue;
    for (int ii=0; ii<2; ++ii){
      int quarter = i*2+ii;
      LHCb::L0MuonCtrlError * l0mctrlerror = new LHCb::L0MuonCtrlError(quarter);
      // Fill errors
      if (m_ctrlRaw[i]->decodingError(ii) ) l0mctrlerror->setHeader(4);
      else if (m_ctrlRaw[i]->inError(ii) ) l0mctrlerror->setHeader(2);
      else l0mctrlerror->setHeader(1);
      pl0mctrlerrors->insert(l0mctrlerror);
    }
  }

  location = LHCb::L0MuonProcErrorLocation::Default + context();
  LHCb::L0MuonProcErrors* pl0mprocerrors = new LHCb::L0MuonProcErrors();
  put(pl0mprocerrors, location );
//   if (msgLevel( MSG::DEBUG )) {
//     debug() << "writeOnTES -------------------------"<< endmsg;
//     debug() << "writeOnTES at "<< location << endmsg;
//   }
  for (int iq= 0; iq<4; ++iq) {
    if (!m_procRaw[iq]->isActiv()) continue;
    for (int ib=0; ib<12; ++ib){
      LHCb::L0MuonProcError * l0mprocerror = new LHCb::L0MuonProcError(m_procRaw[iq]->mid_BCSU(ib),ib);
      // Fill errors
      if (m_procRaw[iq]->decodingError(ib) ) l0mprocerror->setHeader(4);
      else if (m_procRaw[iq]->inError(ib) ) l0mprocerror->setHeader(2);
      else l0mprocerror->setHeader(1);
      pl0mprocerrors->insert(l0mprocerror);
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode L0MuonFromRawTrans::releaseRegisters(){

  for (int i= 0; i<2; ++i) m_ctrlRaw[i]->release();
  for (int i= 0; i<4; ++i) m_procRaw[i]->release();

  return StatusCode::SUCCESS;
}

LHCb::L0MuonCandidate* L0MuonFromRawTrans::l0muoncandidate(L0Muon::PMuonCandidate cand,int procVersion)
{
  bool debug = false;
//   if (msgLevel( MSG::VERBOSE )) debug=true;
    
  std::vector<LHCb::MuonTileID> pads = L0Muon::add2pads(cand->quarter(),cand->board(),cand->pu(),
                                                        cand->colM3(),cand->rowM3(), cand->offM2(),cand->offM1(),
                                                        procVersion,debug);
  if (pads.size()==0) {
    error()<<"wrong address for candidate "<<endmsg;
    return NULL;
  }
  
  std::vector<double> kine = L0Muon::kine(pads[0],pads[1],procVersion,debug);
  //debug()<<"l0muoncandidate Seed"<<pads[2].toString()<<" M2: "<<pads[1].toString()<<" M1: "<<pads[0].toString()<< endmsg;
  LHCb::L0MuonCandidate *pl0muoncandidate = new LHCb::L0MuonCandidate(kine[0], kine[1], kine[2], pads,cand->pT());
  return pl0muoncandidate;
}

