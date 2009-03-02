// $Id: STNZSMonitor.cpp,v 1.1.1.1 2009-03-02 19:13:44 mtobin Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// local

#include "Event/RawBank.h"
#include "Event/RawEvent.h"
#include "Kernel/STTell1ID.h"
#include "GaudiKernel/Algorithm.h" 
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/MsgStream.h" 
#include "MDF/RawEventPrintout.h"
#include "MDF/MDFHeader.h"
#include "Event/STTELL1Data.h"
#include "Event/STTELL1EventInfo.h"
#include "STRawBankMap.h"
#include "Kernel/STDetSwitch.h"
#include "Kernel/ISTReadoutTool.h"
#include "Kernel/STTell1Board.h"
#include "Kernel/StripRepresentation.h"
#include "math.h"

#include <sstream>

#include "STNZSMonitor.h"

#include "Kernel/STTell1Board.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : STNZSMonitor
//
// 2007-12-12: Mathias Knecht
//-----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( STNZSMonitor );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
STNZSMonitor::STNZSMonitor( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
     
  declareProperty("InputLocation",m_inputLocation=STTELL1DataLocation::ITFull ); // STTELL1DataLoc::ITFull = "Raw/IT/Full"
  // i.e. data after decoding, before emu
  declareProperty("PedSubADCs",m_PedSubADCs=STTELL1DataLocation::ITSubPeds );  		 // data after Pedestal subst
  declareProperty("LCMSADCs",m_LCMSADCs=STTELL1DataLocation::ITLCMSADCs ); 			 // data after Common-Mode Suppression
  declareProperty("Disable2DHistograms",m_disable2D=false );
  declareProperty("readoutTool", m_readoutToolName = "ITReadoutTool");     
  declareProperty("detType", m_detType = "IT" );
  declareProperty("NoiseIncrement", m_NoiseIncr = 2000.0 );
  declareProperty("ConvergenceLimit", m_ConvLimit = 2000 ); // nr of events necessary for starting to look at LCMSDATA and
  // PEDSub values, has to be at least greater than the emulator
  // option : STTELL1LCMS.ConvergenceLimit
 
  declareProperty("OnlineMode", m_OnlineMode = false );
  


}
//=============================================================================
// Destructor
//=============================================================================
STNZSMonitor::~STNZSMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode STNZSMonitor::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
 
  if(m_detType == "IT"){
    info() << "You are using an Inner Tracker-style algorithm"<< endmsg;
  }
  else if(m_detType == "TT"){
    info() << "You are using a Trigger Turicensis-style algorithm"<< endmsg;
  }else{
    fatal() << "Algorithm only for IT or TT!! Either change your code, either change your sub-detector."<< endmsg;
    return StatusCode::FAILURE;
  }
 
  STDetSwitch::flip(m_detType,m_readoutToolName); //flips "IT"-"TT" when needed, to have IT-TT compatibility
  
  // readout tool
  m_readoutTool = tool<ISTReadoutTool>(m_readoutToolName,m_readoutToolName);
   
  // output location
  STDetSwitch::flip(m_detType,m_inputLocation); //  finds string "IT" or "TT" in m_inputlocation
  // and changes it according to m_det_Type, to ensure
  // the data is read from the correct location
 
  evt_counter=0;
  
  sc = service("HistogramDataSvc", m_histosvc, true ); // for online publishing of histograms
  
      
  evt_number = m_histosvc->book("evt_number", "evt_number", 1, 0, 1 );// histogram booking, number of events
  declareInfo( "evt_number", evt_number , "Number of Events");        // histogram publishing
  
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode STNZSMonitor::execute() {

  debug() << "==> Execute " << endmsg;
  evt_number->fill(0);
  ++evt_counter;

  debug() << "Event Counter: " << evt_counter << endmsg;
 
  // Get the container of the STFullData  
  STTELL1Datas* tell1container = get< STTELL1Datas>(m_inputLocation); // take STTELL1Datas (TELL1 container) at m_inpuLocation
  // i.e. after decoding, before emulation
  
 
  // Different objects in the different containers
  
  std::vector<STTELL1Data*> Board;   // Raw data associated to a single TELL1 board in the container
  std::vector<STTELL1Data*> LCMSADC; // CMS data object
  std::vector<STTELL1Data*> PEDSUB;  // Pedestal subscted object
 
  std::vector<STTELL1Data::Data> tell1data; // objects in the STFull container
  std::vector<STTELL1Data::Data> tell1header; // objects in the STFull container
  std::vector<STTELL1Data::Info> tell1evtinfo; // objects in the STFull container
  
  std::vector<STTELL1Data::Data> LCMSdata; // object in the CMS container
  std::vector<STTELL1Data::Data> PedSubdata; // object in the Pedestal Substracted container
  

 

  // Fill the TELL1 map : Loop over objects in the TELL1 container, and look the sourceID, 
  // make correspondance between sourceID (=key()) and index of the loop

  if(evt_counter ==1 ){
    STTELL1Datas::const_iterator Begin(tell1container -> begin()), End(tell1container -> end()); 
    q=0;
    for(STTELL1Datas::const_iterator it(Begin); it != End; it++, ++q){
      m_map[q]=(*it)->key();
    }
  }

  // resizings  of the boards, q-1 is the total number of TELL1 
  Board.resize(q+1);
  LCMSADC.resize(q+1);
  PEDSUB.resize(q+1);

  // resizing of the objects in the boards
  tell1header.resize(q+1);
  tell1data.resize(q+1);
  tell1evtinfo.resize(q+1);

  LCMSdata.resize(q+1);
  PedSubdata.resize(q+1);
  
  // Loop over all TELL1 boards in the container 
  for(unsigned int iBOARD=0; iBOARD<q; iBOARD++){   
    
    if(m_detType=="IT"){
      for(int kModule=0; kModule<8; kModule++) toberesettedIT[kModule]=false;
    }
    if(m_detType=="TT"){
      for(int kSector=0; kSector<6; kSector++) toberesettedTT[kSector]=false;
    }
  
    
    // take object (a TELL1 board) with key m_map (=sourceID) in tell1container
    Board[iBOARD] =  tell1container->object(m_map[iBOARD]);
    
    if (Board[iBOARD]== 0){
      continue;
    }else{
      debug() << "Tell1 board " << m_map[iBOARD] << " found" << endmsg;
    } 
    tell1header[iBOARD] = Board[iBOARD]->header(); // take header block of Board[iBOARD], defined in STTELL1Event.h
    tell1data[iBOARD] = Board[iBOARD]->data();      // take data block of Board[iBOARD], defined in STTELL1Event.h
    tell1evtinfo[iBOARD] = Board[iBOARD]->eventInfo();
    
    //##############################################################################################################
    if(evt_counter >= m_ConvLimit){ // if evt_counter>m_ConvLimit, then the LCMSData and PedSubData of the emulator
      // are available in the TES (pedestals are averaged over a nr of events= m_ConvLimit)
      
      STTELL1Datas* LCMSADCs = get< STTELL1Datas>(m_LCMSADCs);     
      STTELL1Datas* PedSubADCs = get< STTELL1Datas>(m_PedSubADCs);
     
      LCMSADC[iBOARD] =  LCMSADCs->object(m_map[iBOARD]);
      LCMSdata[iBOARD] = LCMSADC[iBOARD]->data();
      
      PEDSUB[iBOARD] =  PedSubADCs->object(m_map[iBOARD]);
      PedSubdata[iBOARD] = PEDSUB[iBOARD]->data();
      
      //############################################################ TO BE DONE ONCE PER ANALYSIS
      
      if( evt_counter==m_ConvLimit){ //  0-3 indexing the 4 EventInfo blocks of TELL1 index iBOARD
        // 0-23 indexing every TELL1 links. Sets an int value: 1 if link
        // is disabled, 0 if link active.
		
        for(int iPPFPGA=0; iPPFPGA<4; iPPFPGA++){			     // 0x20 hexadecimal values
          // retrieving Disable Info
          maskedpositive[iPPFPGA*6+0]= (tell1evtinfo[iBOARD][iPPFPGA]->OptLnkDisable()) & 0x1;
          maskedpositive[iPPFPGA*6+1]= ((tell1evtinfo[iBOARD][iPPFPGA]->OptLnkDisable()) & 0x2)/0x2;
          maskedpositive[iPPFPGA*6+2]= ((tell1evtinfo[iBOARD][iPPFPGA]->OptLnkDisable()) & 0x4)/0x4;
          maskedpositive[iPPFPGA*6+3]= ((tell1evtinfo[iBOARD][iPPFPGA]->OptLnkDisable()) & 0x8)/0x8;
          maskedpositive[iPPFPGA*6+4]= ((tell1evtinfo[iBOARD][iPPFPGA]->OptLnkDisable()) & 0x10)/0x10;
          maskedpositive[iPPFPGA*6+5]= ((tell1evtinfo[iBOARD][iPPFPGA]->OptLnkDisable()) & 0x20)/0x20;
        }
	
        // NAMES OF LADDERS FROM SOURCEID+MODULEID ################################################
        // FROM DAQ (tell1 ID + link ID within TELL1) TO  geometry
        STTell1Board* aBoard =  m_readoutTool->findByBoardID(STTell1ID(m_map[iBOARD])); 
  
        STTell1ID boardID = STTell1ID(m_map[iBOARD]);
        unsigned int subid = boardID.subID(); // 0-13
        unsigned int reg = boardID.region()+1;  // 1-2-3 for IT
        unsigned int stripfrac=0;
   
        STChannelID aChan[3072];
        for(int kChan=0; kChan<3072; kChan++){
          // aChan=one channelID for every channel in IT
          aChan[kChan] = (aBoard->DAQToOffline(stripfrac, STDAQ::v4, STDAQ::StripRepresentation(kChan))).first; 
        }
        // #######################################################
	
        //std::cout << aChan.station() << aChan.layer()           << aChan.detRegion() << aChan.sector() ;
        //             IT 1,2,3     IT 1,2,3,4 =x1,u,v,x2       IT 1,2,3,4 c,a,b,t    1,...,7 
        //             TT 1,2       TT 1,2  TTax1, TTau, TTbv, TTbx2      1,2,3      
        //
	
        // initializing the names of histograms associated to one TELL1
        std::ostringstream nameRawNoiseCumBoard;
        std::ostringstream nameCmsNoiseCumBoard;
        std::ostringstream nameCmsNoiseDistrBoard;
	
        std::ostringstream nameMeanCumBoard;
        std::ostringstream nameMeanDistrBoard;

        std::ostringstream namePCN;
        std::ostringstream nameOptLnkNoEvt; 
        std::ostringstream namePCNErr; 
        std::ostringstream nameSyncSizeError;
        std::ostringstream nameOptLnkNoClock; 
        std::ostringstream nameSyncRamFull;
        std::ostringstream nameTLKLnkLoss; 
	
        if(!m_OnlineMode){
   
          // creating the names associated to TELL1s
          //                         obvious      =station           
          nameRawNoiseCumBoard    << m_detType << reg << "/SourceID" << m_map[iBOARD] << "SubID" << subid 
                                  << "/RawNoiseCumulBoard"; 
          nameCmsNoiseCumBoard    << m_detType << reg << "/SourceID" << m_map[iBOARD] << "SubID" << subid 
                                  << "/CmsNoiseCumulBoard"; 
          nameCmsNoiseDistrBoard  << m_detType << reg << "/SourceID" << m_map[iBOARD] << "SubID" << subid 
                                  << "/CmsNoiseDistrBoard"; 
	  
          nameMeanCumBoard        << m_detType << reg << "/SourceID" << m_map[iBOARD] << "SubID" << subid << "/MeanCumulBoard"; 
          nameMeanDistrBoard      << m_detType << reg << "/SourceID" << m_map[iBOARD] << "SubID" << subid << "/MeanDistrBoard"; 
		  
          namePCN                 << m_detType << reg << "/SourceID" << m_map[iBOARD] << "SubID" << subid << "/PCNDistribution";
          nameOptLnkNoEvt         << m_detType << reg << "/SourceID" << m_map[iBOARD] << "SubID" << subid 
                                  << "/OptLinkNoEventError";
          namePCNErr              << m_detType << reg << "/SourceID" << m_map[iBOARD] << "SubID" << subid << "/PCNError";
          nameSyncSizeError       << m_detType << reg << "/SourceID" << m_map[iBOARD] << "SubID" << subid 
                                  << "/SyncEventSizeError";
          nameOptLnkNoClock       << m_detType << reg << "/SourceID" << m_map[iBOARD] << "SubID" << subid 
                                  << "/OptLinkNoClockError";
          nameSyncRamFull         << m_detType << reg << "/SourceID" << m_map[iBOARD] << "SubID" << subid << "/SyncRamFullError";
          nameTLKLnkLoss          << m_detType << reg << "/SourceID" << m_map[iBOARD] << "SubID" << subid << "/TLKLinkLossError";
        }else{
	
          nameRawNoiseCumBoard    << "RawNoiseCumulBoard_$/" << m_detType << reg << "/SourceID" << m_map[iBOARD] 
                                  << "SubID" << subid; 
          nameCmsNoiseCumBoard    << "CmsNoiseCumulBoard_$/" << m_detType << reg << "/SourceID" << m_map[iBOARD] 
                                  << "SubID" << subid; 
          nameCmsNoiseDistrBoard  << "CmsNoiseDistrBoard_$/" << m_detType << reg << "/SourceID" << m_map[iBOARD] 
                                  << "SubID" << subid; 
	  
          nameMeanCumBoard        << "MeanCumulBoard_$/"     << m_detType << reg << "/SourceID" << m_map[iBOARD] 
                                  << "SubID" << subid; 
          nameMeanDistrBoard      << "MeanDistrBoard_$/"     << m_detType << reg << "/SourceID" << m_map[iBOARD] 
                                  << "SubID" << subid; 
		  
          namePCN                 << "PCNDistribution_$/"    << m_detType << reg << "/SourceID" << m_map[iBOARD] 
                                  << "SubID" << subid;
          nameOptLnkNoEvt         << "OptLinkNoEventError_$/"<< m_detType << reg << "/SourceID" << m_map[iBOARD] 
                                  << "SubID" << subid;
          namePCNErr              << "PCNError_$/"           << m_detType << reg << "/SourceID" << m_map[iBOARD] 
                                  << "SubID" << subid;
          nameSyncSizeError       << "SyncEventSizeError_$/" << m_detType << reg << "/SourceID" << m_map[iBOARD] 
                                  << "SubID" << subid;
          nameOptLnkNoClock       << "OptLinkNoClockError_$/"<< m_detType << reg << "/SourceID" << m_map[iBOARD] 
                                  << "SubID" << subid;
          nameSyncRamFull         << "SyncRamFullError_$/"   << m_detType << reg << "/SourceID" << m_map[iBOARD] 
                                  << "SubID" << subid;
          nameTLKLnkLoss          << "TLKLinkLossError_$/"   << m_detType << reg << "/SourceID" << m_map[iBOARD] 
                                  << "SubID" << subid;
	
        }
        // HISTOGRAMS DECLARATIONS/DEFINITIONS ################################################
 
        // histograms name defined in header file
        if(m_disable2D == false){
          RAWNOISEBOARD[iBOARD] = m_histosvc->book( nameRawNoiseCumBoard.str(),nameRawNoiseCumBoard.str(),3072,0,3072,100,0,10 );
          declareInfo( nameRawNoiseCumBoard.str(),RAWNOISEBOARD[iBOARD],nameRawNoiseCumBoard.str());
    
          CMSNOISEBOARD[iBOARD] = m_histosvc->book( nameCmsNoiseCumBoard.str(),nameCmsNoiseCumBoard.str(),3072,0,3072,100,0,10 );
          declareInfo( nameCmsNoiseCumBoard.str(),  CMSNOISEBOARD[iBOARD] , nameCmsNoiseCumBoard.str());
   	 
          MEANBOARD[iBOARD] = m_histosvc->book( nameMeanCumBoard.str(), nameMeanCumBoard.str(), 3072, 0,  3072, 60, 0, 60);
          declareInfo( nameMeanCumBoard.str(),  MEANBOARD[iBOARD] , nameMeanCumBoard.str());
   
        }
	
        MEANDISTRBOARD[iBOARD] = m_histosvc->book( nameMeanDistrBoard.str(), nameMeanDistrBoard.str(), 60, 0, 60);
        declareInfo( nameMeanDistrBoard.str(),  MEANDISTRBOARD[iBOARD] , nameMeanDistrBoard.str());
     
        CMSNOISEDISTRBOARD[iBOARD] = m_histosvc->book( nameCmsNoiseDistrBoard.str(), nameCmsNoiseDistrBoard.str(), 50, 0, 10);
        declareInfo( nameCmsNoiseDistrBoard.str(),  CMSNOISEDISTRBOARD[iBOARD] , nameCmsNoiseDistrBoard.str());
	
        PCNDISTR[iBOARD] = m_histosvc->book( namePCN.str(), namePCN.str(), 24, 0, 24, 187, 0, 187 );
        declareInfo( namePCN.str(),  PCNDISTR[iBOARD] , namePCN.str());
   
        OPTLINKNOEVT[iBOARD] = m_histosvc->book( nameOptLnkNoEvt.str(), nameOptLnkNoEvt.str(), 24, 0, 24);
        declareInfo( nameOptLnkNoEvt.str(),  OPTLINKNOEVT[iBOARD] , nameOptLnkNoEvt.str());

        SYNCEVTSIZERROR[iBOARD] = m_histosvc->book( nameSyncSizeError.str(), nameSyncSizeError.str(), 24, 0, 24);
        declareInfo( nameSyncSizeError.str(),  SYNCEVTSIZERROR[iBOARD] , nameSyncSizeError.str());

        OPTLNKNOCLOCK[iBOARD] = m_histosvc->book( nameOptLnkNoClock.str(), nameOptLnkNoClock.str(), 24, 0, 24);
        declareInfo( nameOptLnkNoClock.str(),  OPTLNKNOCLOCK[iBOARD] , nameOptLnkNoClock.str());
   
        TLKLNKLOSS[iBOARD] = m_histosvc->book( nameTLKLnkLoss.str(), nameTLKLnkLoss.str(), 24, 0, 24);
        declareInfo( nameTLKLnkLoss.str(),  TLKLNKLOSS[iBOARD] , nameTLKLnkLoss.str());
   
        SYNCRAMFULL[iBOARD] = m_histosvc->book( nameSyncRamFull.str(), nameSyncRamFull.str(), 24, 0, 24);
        declareInfo( nameSyncRamFull.str(),  SYNCRAMFULL[iBOARD] , nameSyncRamFull.str());
  
        PCNERROR[iBOARD] = m_histosvc->book( namePCNErr.str(), namePCNErr.str(), 4,0,4);
        declareInfo( namePCNErr.str(),  PCNERROR[iBOARD] , namePCNErr.str());
 
        //################################################################################################################### 
 
        //Loop over links, find out which are the modules used (=not disabled), and create histograms accordingly
        for(int kLink=0; kLink<24; kLink++ ){
  
          // determine which links are used:
          if(maskedpositive[kLink]){
            m_useLink[iBOARD][kLink]=false; // don't use TELL1 iBoard link kLink
          }else{
            m_useLink[iBOARD][kLink]=true; // use TELL1 iBoard link kLink
          }
        }
	
        for(int kLink=0; kLink<24; kLink++ ){
	
          iBeetleIT=kLink % 3;
          iBeetleTT=kLink % 4;
          iModuleIT=kLink/3;
          iSectorTT=kLink/4;
	
          // initializing the names of histograms asociated to modules
          std::ostringstream nameRawADCDistrMod;
          std::ostringstream nameHeaderDistrMod;
          std::ostringstream nameCmsNoiseDistrMod;  
          std::ostringstream nameCmsNoiseNowMod;  
          std::ostringstream nameRawNoiseNowMod;  
          std::ostringstream nameLCMS; 
          std::ostringstream nameMeanAveragedMod;
          std::ostringstream nameSoverNMod;
          std::ostringstream namePedestalMod;
	  
          if((m_detType == "IT") && (iBeetleIT==0) && 
             (m_useLink[iBOARD][kLink] || m_useLink[iBOARD][kLink+1]  || m_useLink[iBOARD][kLink+2] )){
	    
            std::string box;
            std::string Layer;
            if(aChan[kLink*128].detRegion()==1){
              box="C";      
            }
            if(aChan[kLink*128].detRegion()==2){
              box="A";      
            }
            if(aChan[kLink*128].detRegion()==3){
              box="B";      
            }
            if(aChan[kLink*128].detRegion()==4){
              box="T";      
            }
            if(aChan[kLink*128].layer()==1){
              Layer="X1_";      
            }
            if(aChan[kLink*128].layer()==2){
              Layer="U_";      
            }
            if(aChan[kLink*128].layer()==3){
              Layer="V_";      
            }
            if(aChan[kLink*128].layer()==4){
              Layer="X2_";      
            }
            if(!m_OnlineMode){
              nameRawADCDistrMod      <<  m_detType  << reg   << "Box"   << box << "/Module" << Layer 
                                      << aChan[kLink*128].sector() << "/RawADCDistr";
              nameHeaderDistrMod      <<  m_detType  << reg   << "Box"   << box << "/Module" << Layer 
                                      << aChan[kLink*128].sector() << "/HeadersDistr"; 
              nameCmsNoiseDistrMod    <<  m_detType  << reg   << "Box"   << box << "/Module" << Layer 
                                      << aChan[kLink*128].sector() << "/CmsNoiseDistr";    
              nameCmsNoiseNowMod      <<  m_detType  << reg   << "Box"   << box << "/Module" << Layer 
                                      << aChan[kLink*128].sector() << "/CmsNoise";   
              nameRawNoiseNowMod      <<  m_detType  << reg   << "Box"   << box << "/Module" << Layer 
                                      << aChan[kLink*128].sector() << "/RawNoise";    
              nameLCMS                <<  m_detType  << reg   << "Box"   << box << "/Module" << Layer 
                                      << aChan[kLink*128].sector() << "/CmsADCs"; 
              nameMeanAveragedMod     <<  m_detType  << reg   << "Box"   << box << "/Module" << Layer 
                                      << aChan[kLink*128].sector() << "/MeanAveraged";    
              nameSoverNMod           <<  m_detType  << reg   << "Box"   << box << "/Module" << Layer 
                                      << aChan[kLink*128].sector() << "/SoverN";    
              namePedestalMod         <<  m_detType  << reg   << "Box"   << box << "/Module" << Layer 
                                      << aChan[kLink*128].sector() << "/Pedestal"; 
            }else{
              nameRawADCDistrMod      << "RawADCDistr_$/"   <<  m_detType  << reg   << "Box"  << box << "/Module" << Layer 
                                      << aChan[kLink*128].sector();
              nameHeaderDistrMod      << "HeadersDistr_$/"  <<  m_detType  << reg   << "Box"  << box << "/Module" << Layer 
                                      << aChan[kLink*128].sector(); 
              nameCmsNoiseDistrMod    << "CmsNoiseDistr_$/" <<  m_detType  << reg   << "Box"  << box << "/Module" << Layer 
                                      << aChan[kLink*128].sector();  
              nameCmsNoiseNowMod      << "CmsNoise_$/"      <<  m_detType  << reg   << "Box"  << box << "/Module" << Layer 
                                      << aChan[kLink*128].sector(); 
              nameRawNoiseNowMod      << "RawNoise_$/"      <<  m_detType  << reg   << "Box"  << box << "/Module" << Layer 
                                      << aChan[kLink*128].sector();
              nameLCMS                << "CmsADCs_$/"       <<  m_detType  << reg   << "Box"  << box << "/Module" << Layer 
                                      << aChan[kLink*128].sector(); 
              nameMeanAveragedMod     << "MeanAveraged_$/"  <<  m_detType  << reg   << "Box"  << box << "/Module" << Layer 
                                      << aChan[kLink*128].sector();  
              nameSoverNMod           << "SoverN_$/"        <<  m_detType  << reg   << "Box"  << box << "/Module" << Layer 
                                      << aChan[kLink*128].sector(); 
              namePedestalMod         << "Pedestal_$/"	 <<  m_detType  << reg   << "Box"  << box << "/Module" << Layer 
                                      << aChan[kLink*128].sector(); 
              
            }
	       
            // Raw ADC Distribution (all events)
            RAWADCIT[iBOARD][iModuleIT] = m_histosvc->book(nameRawADCDistrMod.str(), nameRawADCDistrMod.str(), 256, 0, 256 );
            declareInfo( nameRawADCDistrMod.str(),  RAWADCIT[iBOARD][iModuleIT] , nameRawADCDistrMod.str());
            
            // Headers Distribution (all events)
            HEADERIT[iBOARD][iModuleIT] = m_histosvc->book(nameHeaderDistrMod.str(), nameHeaderDistrMod.str(), 256, 0, 256 );
            declareInfo( nameHeaderDistrMod.str(),  HEADERIT[iBOARD][iModuleIT] , nameHeaderDistrMod.str());
            
            // Noise Distribution for module (Noise measurements)
            CMSNOISEDISTRMODIT[iBOARD][iModuleIT] = m_histosvc->book(nameCmsNoiseDistrMod.str(),nameCmsNoiseDistrMod.str(),
                                                                     50,0,10);
            declareInfo( nameCmsNoiseDistrMod.str(),  CMSNOISEDISTRMODIT[iBOARD][iModuleIT] , nameCmsNoiseDistrMod.str());
            
            // CMS Noise per strip, last measurement
            CMSNOISEIT[iBOARD][iModuleIT] = m_histosvc->book( nameCmsNoiseNowMod.str(), nameCmsNoiseNowMod.str(), 384, 0,  384 );
            declareInfo( nameCmsNoiseNowMod.str(),  CMSNOISEIT[iBOARD][iModuleIT] , nameCmsNoiseNowMod.str());
            
            // RAW Noise per strip, last measurement
            RAWNOISEIT[iBOARD][iModuleIT] = m_histosvc->book( nameRawNoiseNowMod.str(), nameRawNoiseNowMod.str(), 384, 0,  384 );
            declareInfo( nameRawNoiseNowMod.str(),  RAWNOISEIT[iBOARD][iModuleIT] , nameRawNoiseNowMod.str()); 
	    
            MEANIT[iBOARD][iModuleIT] = m_histosvc->book( nameMeanAveragedMod.str(),   nameMeanAveragedMod.str(),384, 0, 384 );
            declareInfo( nameMeanAveragedMod.str(),  MEANIT[iBOARD][iModuleIT] , nameMeanAveragedMod.str()); 
            
            SoverNIT[iBOARD][iModuleIT] = m_histosvc->book( nameSoverNMod.str(), nameSoverNMod.str(), 384, 0,  384 );  
            declareInfo( nameSoverNMod.str(),  SoverNIT[iBOARD][iModuleIT] , nameSoverNMod.str()); 
            
            PEDESTALIT[iBOARD][iModuleIT] = m_histosvc->book( namePedestalMod.str(), namePedestalMod.str(), 384, 0,  384 );  
            declareInfo( namePedestalMod.str(),  PEDESTALIT[iBOARD][iModuleIT] , namePedestalMod.str()); 
            
            // Each ADC cms substracted (all events)
            if(m_disable2D == false){
              LCMSIT[iBOARD][iModuleIT] = m_histosvc->book( nameLCMS.str(), nameLCMS.str(), 384, 0,  384,  101, -100, 100 );
              declareInfo( nameLCMS.str(),  LCMSIT[iBOARD][iModuleIT] , nameLCMS.str()); 
            }
            
            
            
          }//end of it used module
          
          if((m_detType == "TT") && (iBeetleTT==0) && 
            (m_useLink[iBOARD][kLink] || m_useLink[iBOARD][kLink+1] || m_useLink[iBOARD][kLink+2] || m_useLink[iBOARD][kLink+3])){
            
            if(!m_OnlineMode){
              nameRawADCDistrMod        <<  m_detType  << "TELL"<< m_map[iBOARD] << "/Sector " << iSectorTT << "/Raw ADC Distr.";
              nameHeaderDistrMod        <<  m_detType  << "TELL"<< m_map[iBOARD] << "/Sector " << iSectorTT << "/Headers Distr."; 
              nameCmsNoiseDistrMod      <<  m_detType  << "TELL"<< m_map[iBOARD] << "/Sector " << iSectorTT 
                                        << "/Cms Noise Distr.";
              nameCmsNoiseNowMod        <<  m_detType  << "TELL"<< m_map[iBOARD] << "/Sector " << iSectorTT << "/Cms Noise";   
              nameRawNoiseNowMod        <<  m_detType  << "TELL"<< m_map[iBOARD] << "/Sector " << iSectorTT << "/Raw Noise";    
              nameLCMS                  <<  m_detType  << "TELL"<< m_map[iBOARD] << "/Sector " << iSectorTT << "/Cms ADCs"; 
              nameMeanAveragedMod       <<  m_detType  << "TELL"<< m_map[iBOARD] << "/Sector " << iSectorTT << "/MeanAveraged";
              nameSoverNMod             <<  m_detType  << "TELL"<< m_map[iBOARD] << "/Sector " << iSectorTT << "/SoverN";    
              namePedestalMod           <<  m_detType  << "TELL"<< m_map[iBOARD] << "/Sector " << iSectorTT << "/Pedestal"; 
            }else{
              nameRawADCDistrMod       << "RawADCDistr_$/"   <<  m_detType  << "TELL"<< m_map[iBOARD] << "/Sector " << iSectorTT; 
              nameHeaderDistrMod       << "HeadersDistr_$/"  <<  m_detType  << "TELL"<< m_map[iBOARD] << "/Sector " << iSectorTT;
              nameCmsNoiseDistrMod     << "CmsNoiseDistr_$/" <<  m_detType  << "TELL"<< m_map[iBOARD] << "/Sector " << iSectorTT; 
              nameCmsNoiseNowMod       << "CmsNoise_$/"	  <<  m_detType  << "TELL"<< m_map[iBOARD] << "/Sector " << iSectorTT; 
              nameRawNoiseNowMod       << "RawNoise_$/"	  <<  m_detType  << "TELL"<< m_map[iBOARD] << "/Sector " << iSectorTT; 
              nameLCMS                 << "CmsADCs_$/"	  <<  m_detType  << "TELL"<< m_map[iBOARD] << "/Sector " << iSectorTT; 
              nameMeanAveragedMod      << "MeanAveraged_$/"  <<  m_detType  << "TELL"<< m_map[iBOARD] << "/Sector " << iSectorTT; 
              nameSoverNMod            << "SoverN_$/"	  <<  m_detType  << "TELL"<< m_map[iBOARD] << "/Sector " << iSectorTT; 
              namePedestalMod          << "Pedestal_$/"	  <<  m_detType  << "TELL"<< m_map[iBOARD] << "/Sector " << iSectorTT; 
            }
            
            // Raw ADC Distribution (all events)
            RAWADCTT[iBOARD][iSectorTT] = m_histosvc->book(nameRawADCDistrMod.str(), nameRawADCDistrMod.str(), 256, 0, 256 );
            declareInfo( nameRawADCDistrMod.str(),  RAWADCTT[iBOARD][iSectorTT] , nameRawADCDistrMod.str());
            
            // Headers Distribution (all events)	    
            HEADERTT[iBOARD][iSectorTT] = m_histosvc->book(nameHeaderDistrMod.str(), nameHeaderDistrMod.str(), 256, 0, 256 );
            declareInfo( nameHeaderDistrMod.str(),  HEADERTT[iBOARD][iSectorTT] , nameHeaderDistrMod.str());
            
            // Noise Distribution for module (Noise measurements)
            CMSNOISEDISTRMODTT[iBOARD][iSectorTT] = m_histosvc->book( nameCmsNoiseDistrMod.str(), nameCmsNoiseDistrMod.str(),
                                                                      50,0,10 );
            declareInfo( nameCmsNoiseDistrMod.str(),  CMSNOISEDISTRMODTT[iBOARD][iSectorTT] , nameCmsNoiseDistrMod.str());
            
            // CMS  Noise per strip, last measurement
            CMSNOISETT[iBOARD][iSectorTT] = m_histosvc->book( nameCmsNoiseNowMod.str(), nameCmsNoiseNowMod.str(), 512, 0,  512 );
            declareInfo( nameCmsNoiseNowMod.str(),  CMSNOISETT[iBOARD][iSectorTT] , nameCmsNoiseNowMod.str());
            
            // RAW Noise per strip, last measurement
            RAWNOISETT[iBOARD][iSectorTT] = m_histosvc->book( nameRawNoiseNowMod.str(), nameRawNoiseNowMod.str(), 512, 0,  512 );
            declareInfo( nameRawNoiseNowMod.str(),  RAWNOISETT[iBOARD][iSectorTT] , nameRawNoiseNowMod.str());
         
            // RAW Noise per strip, last measurement
            MEANTT[iBOARD][iSectorTT] = m_histosvc->book( nameMeanAveragedMod.str(), nameMeanAveragedMod.str(), 512, 0,  512 );
            declareInfo( nameMeanAveragedMod.str(),  MEANTT[iBOARD][iSectorTT] , nameMeanAveragedMod.str()); 
	 
            // RAW Noise per strip, last measurement
            SoverNTT[iBOARD][iSectorTT] = m_histosvc->book( nameSoverNMod.str(), nameSoverNMod.str(), 512, 0,  512 );
            declareInfo( nameSoverNMod.str(),  SoverNTT[iBOARD][iSectorTT] , nameSoverNMod.str()); 
	    
            PEDESTALTT[iBOARD][iSectorTT] = m_histosvc->book( namePedestalMod.str(), namePedestalMod.str(), 512, 0,  512 );  
            declareInfo( namePedestalMod.str(),  PEDESTALTT[iBOARD][iSectorTT] , namePedestalMod.str()); 

            if(m_disable2D == false){
              // Each ADC cms substracted (all events)
              LCMSTT[iBOARD][iSectorTT] = m_histosvc->book( nameLCMS.str(), nameLCMS.str(), 512, 0,  512,  101, -100, 100 );
              declareInfo( nameLCMS.str(),  LCMSTT[iBOARD][iSectorTT] , nameLCMS.str());
            }
          }//end of it used module
        }// end of loop over links
      }// end of if evt_counter =2000
      
      
      
      
      //#####################################################################################
      
      if(m_OnlineMode){
        if(((evt_counter-m_ConvLimit) % (int)m_NoiseIncr == 0) && evt_counter!=m_ConvLimit){
          
          for(int kLink=0; kLink<24; kLink++){  
            
            iModuleIT=kLink/3;
            iSectorTT=kLink/4;
            
            
            if(m_detType=="IT"){
              toberesettedIT[iModuleIT]=true;
            } 
            if(m_detType=="TT"){
              toberesettedTT[iSectorTT]=true;
            }
          }
        }
      }
      
	  
      for(int kLink=0; kLink<24; kLink++){
        
        iBeetleIT=kLink % 3;
        iBeetleTT=kLink % 4;
        iModuleIT=kLink/3;
        iSectorTT=kLink/4;
        
        
        if(m_useLink[iBOARD][kLink]){
          
          if(m_detType=="IT"){
      
            for(int kHeader=0; kHeader<12; kHeader++){ // kChannel is the header number, 3 per ports, 4 ports=12
              HEADERIT[iBOARD][iModuleIT]->fill(tell1header[iBOARD][kLink][kHeader]);
            }	    
            for(int kChannel=0; kChannel<128; kChannel++){// kChannel is the channel number, 128 per beetle
	  
              RAWADCIT[iBOARD][iModuleIT]->fill(tell1data[iBOARD][kLink][kChannel]);
        
              if(m_disable2D == false){
		
                LCMSIT[iBOARD][iModuleIT]->fill(iBeetleIT*128+kChannel,LCMSdata[iBOARD][kLink][kChannel]);
              }
            }
          }
      
          if(m_detType=="TT"){
            for(int kHeader=0; kHeader<12; kHeader++){ 
              HEADERTT[iBOARD][iSectorTT]->fill(tell1header[iBOARD][kLink][kHeader]);
            }
            for(int kChannel=0; kChannel<128; kChannel++){
	      
              RAWADCTT[iBOARD][iSectorTT]->fill(tell1data[iBOARD][kLink][kChannel]); 
	      
              if(m_disable2D == false){
		
                LCMSTT[iBOARD][iSectorTT]->fill(iBeetleTT*128+kChannel,LCMSdata[iBOARD][kLink][kChannel]);
              }
            }
          }
          // calculation of the noise
	  
          for(int kChannel=0; kChannel<128; kChannel++){ // kChannel is the channel number
            cmsADC[iBOARD][kLink][kChannel]+=LCMSdata[iBOARD][kLink][kChannel];                     // sum of the values 
            // sum of the squared of the values
            cmsADCsq[iBOARD][kLink][kChannel]+=LCMSdata[iBOARD][kLink][kChannel]*LCMSdata[iBOARD][kLink][kChannel];
            pedsubADC[iBOARD][kLink][kChannel]+=PedSubdata[iBOARD][kLink][kChannel];
            pedsubADCsq[iBOARD][kLink][kChannel]+=PedSubdata[iBOARD][kLink][kChannel]*PedSubdata[iBOARD][kLink][kChannel];
            pedestals[iBOARD][kLink][kChannel]=tell1data[iBOARD][kLink][kChannel]-LCMSdata[iBOARD][kLink][kChannel];
            if(LCMSdata[iBOARD][kLink][kChannel]>0){
              absADC[iBOARD][kLink][kChannel]+=LCMSdata[iBOARD][kLink][kChannel];
            }else{
              absADC[iBOARD][kLink][kChannel]+=(-1)*LCMSdata[iBOARD][kLink][kChannel];
            }
	    
          }
          
          if(m_OnlineMode){

            if(((evt_counter-m_ConvLimit) % (int)m_NoiseIncr == 0) && evt_counter!=m_ConvLimit){
	    
              for(int kChannel=0; kChannel<128; kChannel++){	    

                if( pedsubADCsq[iBOARD][kLink][kChannel]/m_NoiseIncr 
                    < (pedsubADC[iBOARD][kLink][kChannel]/m_NoiseIncr)*(pedsubADC[iBOARD][kLink][kChannel]/m_NoiseIncr) ) 
                  noise_PedSub[iBOARD][kLink][kChannel] = 0.0;
                else noise_PedSub[iBOARD][kLink][kChannel] = 
                       sqrt((pedsubADCsq[iBOARD][kLink][kChannel]/m_NoiseIncr)-(pedsubADC[iBOARD][kLink][kChannel]/m_NoiseIncr)
                            *(pedsubADC[iBOARD][kLink][kChannel]/m_NoiseIncr));
	      
                if( cmsADCsq[iBOARD][kLink][kChannel]/m_NoiseIncr 
                    < (cmsADC[iBOARD][kLink][kChannel]/m_NoiseIncr)*(cmsADC[iBOARD][kLink][kChannel]/m_NoiseIncr) ) 
                  noise_CMS[iBOARD][kLink][kChannel]=0.0;
                else noise_CMS[iBOARD][kLink][kChannel] = 
                       sqrt((cmsADCsq[iBOARD][kLink][kChannel]/m_NoiseIncr)
                            -(cmsADC[iBOARD][kLink][kChannel]/m_NoiseIncr)*(cmsADC[iBOARD][kLink][kChannel]/m_NoiseIncr));
	      
                mean[iBOARD][kLink][kChannel]=int(absADC[iBOARD][kLink][kChannel]/m_NoiseIncr);
	    	   
                CMSNOISEDISTRBOARD[iBOARD]->fill(noise_CMS[iBOARD][kLink][kChannel]);
                MEANDISTRBOARD[iBOARD]->fill(mean[iBOARD][kLink][kChannel]);
                if(m_disable2D == false){
                  CMSNOISEBOARD[iBOARD]->fill(kLink*128+kChannel,noise_CMS[iBOARD][kLink][kChannel]);
                  RAWNOISEBOARD[iBOARD]->fill(kLink*128+kChannel,noise_PedSub[iBOARD][kLink][kChannel]);
                  MEANBOARD[iBOARD]->fill(kLink*128+kChannel,mean[iBOARD][kLink][kChannel]);
                }
                if(m_detType=="IT"){
	      
                  CMSNOISEDISTRMODIT[iBOARD][iModuleIT]->fill(noise_CMS[iBOARD][kLink][kChannel]);

                  if(toberesettedIT[iModuleIT]==true){
                    CMSNOISEIT[iBOARD][iModuleIT]->reset();
                    RAWNOISEIT[iBOARD][iModuleIT]->reset();
                    MEANIT[iBOARD][iModuleIT]->reset();
                    PEDESTALIT[iBOARD][iModuleIT]->reset();
		    
                    toberesettedIT[iModuleIT]=false;
                  }
		    
		
		  
                  CMSNOISEIT[iBOARD][iModuleIT]->fill(iBeetleIT*128+kChannel,noise_CMS[iBOARD][kLink][kChannel]);
                  RAWNOISEIT[iBOARD][iModuleIT]->fill(iBeetleIT*128+kChannel,noise_PedSub[iBOARD][kLink][kChannel]);
                  MEANIT[iBOARD][iModuleIT]->fill(iBeetleIT*128+kChannel,mean[iBOARD][kLink][kChannel]);
	   
                  PEDESTALIT[iBOARD][iModuleIT]->fill(iBeetleIT*128+kChannel,pedestals[iBOARD][kLink][kChannel]);
	    
		 		
		
                }
	      	  
                if(m_detType=="TT"){
	      
                  CMSNOISEDISTRMODTT[iBOARD][iSectorTT]->fill(noise_CMS[iBOARD][kLink][kChannel]);
	      	     
                  if(toberesettedTT[iSectorTT]==true){
                    CMSNOISETT[iBOARD][iSectorTT]->reset();
                    RAWNOISETT[iBOARD][iSectorTT]->reset();
                    MEANTT[iBOARD][iSectorTT]->reset();
                    PEDESTALTT[iBOARD][iSectorTT]->reset();
	     
                    toberesettedTT[iSectorTT]=false;
                  }
		 
                  CMSNOISETT[iBOARD][iSectorTT]->fill(iBeetleTT*128+kChannel,noise_CMS[iBOARD][kLink][kChannel]);
                  RAWNOISETT[iBOARD][iSectorTT]->fill(iBeetleTT*128+kChannel,noise_PedSub[iBOARD][kLink][kChannel]);
                  MEANTT[iBOARD][iSectorTT]->fill(iBeetleTT*128+kChannel,mean[iBOARD][kLink][kChannel]);
                  PEDESTALTT[iBOARD][iSectorTT]->fill(iBeetleTT*128+kChannel,pedestals[iBOARD][kLink][kChannel]);
	    
				
                }
 
 
                // CLEAR THE VALUES
	   
                pedsubADC[iBOARD][kLink][kChannel]=0;
                pedsubADCsq[iBOARD][kLink][kChannel]=0;
 
                cmsADC[iBOARD][kLink][kChannel]=0;
                cmsADCsq[iBOARD][kLink][kChannel]=0;
                absADC[iBOARD][kLink][kChannel]=0;
              }// end of for
	
	
  
            }// end of if(((evt_counter-m_ConvLimit) % (int)m_NoiseIncr == 0) && evt_counter!=m_ConvLimit) 
          } // end if if(m_OnlineMode)
      
      
        }// end of if(m_useLink)
    
      }// end of loop over links
  
      // PP-FPGA loop
      for(int p=0; p<4; p++){
          
 
 
        if((tell1evtinfo[iBOARD][p]->pcnError())==1){
          PCNERROR[iBOARD]->fill(p);
        }
  
        //##########################
	
	
        if(m_useLink[iBOARD][p*6]){
  
 
  
          if(((tell1evtinfo[iBOARD][p]->OptLnkNoEvt() & 0x1 )==0x1 ) ){
            OPTLINKNOEVT[iBOARD]->fill(p*6);
          }
          if(((tell1evtinfo[iBOARD][p]->SyncEvtSizeError() & 0x1 )==0x1 ) ){
            SYNCEVTSIZERROR[iBOARD]->fill(p*6);
          }
          if(((tell1evtinfo[iBOARD][p]->OptLnkNoClock() & 0x1 )==0x1 ) ){
            OPTLNKNOCLOCK[iBOARD]->fill(p*6);
          }
          if(((tell1evtinfo[iBOARD][p]->tlkLnkLoss() & 0x1 )==0x1 ) ){
            TLKLNKLOSS[iBOARD]->fill(p*6);
          }
          if(((tell1evtinfo[iBOARD][p]->SyncRAMFull() & 0x1 )==0x1 ) ){
            SYNCRAMFULL[iBOARD]->fill(p*6);
          }
          
          PCNDISTR[iBOARD]->fill(p*6,tell1evtinfo[iBOARD][p]->pcnBeetle0());
          //IHEADIT[i]->fill(p*6,tell1evtinfo[i][p]->iHeaderBeetle0());
        }
        
        //##########################
        if(m_useLink[iBOARD][p*6+1]){  
          
          if((tell1evtinfo[iBOARD][p]->OptLnkNoEvt() & 0x2 )==0x2 ){
            OPTLINKNOEVT[iBOARD]->fill(p*6+1);
          }
          if(((tell1evtinfo[iBOARD][p]->SyncEvtSizeError() & 0x2 )==0x2 ) ){
            SYNCEVTSIZERROR[iBOARD]->fill(p*6+1);
          }
          if(((tell1evtinfo[iBOARD][p]->OptLnkNoClock() & 0x2 )==0x2 ) ){
            OPTLNKNOCLOCK[iBOARD]->fill(p*6+1);
          }
          if(((tell1evtinfo[iBOARD][p]->tlkLnkLoss() & 0x2 )==0x2 ) ){
            TLKLNKLOSS[iBOARD]->fill(p*6+1);
          }
          if(((tell1evtinfo[iBOARD][p]->SyncRAMFull() & 0x2 )==0x2 ) ){
            SYNCRAMFULL[iBOARD]->fill(p*6+1);
          }
          
          PCNDISTR[iBOARD]->fill(p*6+1,tell1evtinfo[iBOARD][p]->pcnBeetle1());
          //IHEADIT[i]->fill(p*6+1,tell1evtinfo[i][p]->iHeaderBeetle1());
        }
        //##########################
        if(m_useLink[iBOARD][p*6+2]){
  
          if((tell1evtinfo[iBOARD][p]->OptLnkNoEvt() & 0x4 )==0x4 ){
            OPTLINKNOEVT[iBOARD]->fill(p*6+2);
          } 
          if(((tell1evtinfo[iBOARD][p]->SyncEvtSizeError() & 0x4 )==0x4 ) ){
            SYNCEVTSIZERROR[iBOARD]->fill(p*6+2);
          }
          if(((tell1evtinfo[iBOARD][p]->OptLnkNoClock() & 0x4 )==0x4 ) ){
            OPTLNKNOCLOCK[iBOARD]->fill(p*6+2);
          }
          if(((tell1evtinfo[iBOARD][p]->tlkLnkLoss() & 0x4 )==0x4 ) ){
            TLKLNKLOSS[iBOARD]->fill(p*6+2);
          }
          if(((tell1evtinfo[iBOARD][p]->SyncRAMFull() & 0x4 )==0x4 ) ){
            SYNCRAMFULL[iBOARD]->fill(p*6+2);
          }
          
          PCNDISTR[iBOARD]->fill(p*6+2,tell1evtinfo[iBOARD][p]->pcnBeetle2());  
          // IHEADIT[i]->fill(p*6+2,tell1evtinfo[i][p]->iHeaderBeetle2());  
        }
        //##########################
        if(m_useLink[iBOARD][p*6+3]){
  
          if((tell1evtinfo[iBOARD][p]->OptLnkNoEvt() & 0x8 )==0x8 ){
            OPTLINKNOEVT[iBOARD]->fill(p*6+3);
          }
          if(((tell1evtinfo[iBOARD][p]->SyncEvtSizeError() & 0x8 )==0x8 ) ){
            SYNCEVTSIZERROR[iBOARD]->fill(p*6+3);
          }
  
          if(((tell1evtinfo[iBOARD][p]->OptLnkNoClock() & 0x8 )==0x8 ) ){
            OPTLNKNOCLOCK[iBOARD]->fill(p*6+3);
          }
          if(((tell1evtinfo[iBOARD][p]->tlkLnkLoss() & 0x8 )==0x8 ) ){
            TLKLNKLOSS[iBOARD]->fill(p*6+3);
          }
          if(((tell1evtinfo[iBOARD][p]->SyncRAMFull() & 0x8 )==0x8 ) ){
            SYNCRAMFULL[iBOARD]->fill(p*6+3);
          }
 
          PCNDISTR[iBOARD]->fill(p*6+3,tell1evtinfo[iBOARD][p]->pcnBeetle3());
          //IHEADIT[i]->fill(p*6+3,tell1evtinfo[i][p]->iHeaderBeetle3());  
        }
        //##########################
        if(m_useLink[iBOARD][p*6+4]){
  
          if((tell1evtinfo[iBOARD][p]->OptLnkNoEvt() & 0x10)==0x10){
            OPTLINKNOEVT[iBOARD]->fill(p*6+4);
          }
          if(((tell1evtinfo[iBOARD][p]->SyncEvtSizeError() & 0x10 )==0x10 ) ){
            SYNCEVTSIZERROR[iBOARD]->fill(p*6+4);
          }
          if(((tell1evtinfo[iBOARD][p]->OptLnkNoClock() & 0x10 )==0x10 ) ){
            OPTLNKNOCLOCK[iBOARD]->fill(p*6+4);
          }
          if(((tell1evtinfo[iBOARD][p]->tlkLnkLoss() & 0x10 )==0x10 ) ){
            TLKLNKLOSS[iBOARD]->fill(p*6+4);
          }
          if(((tell1evtinfo[iBOARD][p]->SyncRAMFull() & 0x10 )==0x10 ) ){
            SYNCRAMFULL[iBOARD]->fill(p*6+4);
          }
 
          PCNDISTR[iBOARD]->fill(p*6+4,tell1evtinfo[iBOARD][p]->pcnBeetle4());  
          //IHEADIT[i]->fill(p*6+4,tell1evtinfo[i][p]->iHeaderBeetle4());  
        }
        //##########################
        if(m_useLink[iBOARD][p*6+5]){
  
          if((tell1evtinfo[iBOARD][p]->OptLnkNoEvt() & 0x20)==0x20){
            OPTLINKNOEVT[iBOARD]->fill(p*6+5);
          }
          if(((tell1evtinfo[iBOARD][p]->SyncEvtSizeError() & 0x20 )==0x20 ) ){
            SYNCEVTSIZERROR[iBOARD]->fill(p*6+5);
          }
          if(((tell1evtinfo[iBOARD][p]->OptLnkNoClock() & 0x20 )==0x20 ) ){
            OPTLNKNOCLOCK[iBOARD]->fill(p*6+5);
          }
          if(((tell1evtinfo[iBOARD][p]->tlkLnkLoss() & 0x20 )==0x20 ) ){
            TLKLNKLOSS[iBOARD]->fill(p*6+5);
          }
          if(((tell1evtinfo[iBOARD][p]->SyncRAMFull() & 0x20 )==0x20 ) ){
            SYNCRAMFULL[iBOARD]->fill(p*6+5);
          }
 
          PCNDISTR[iBOARD]->fill(p*6+5,tell1evtinfo[iBOARD][p]->pcnBeetle5());
          //IHEADIT[i]->fill(p*6+5,tell1evtinfo[i][p]->iHeaderBeetle5());  
        }
 
  
      }// end of loop over PP-FPGA
    }// end of if evt_counter > 2002
   
   
  }// end of loop over boards
  return StatusCode::SUCCESS;
} 


 

//=============================================================================
//  Finalize
//=============================================================================
StatusCode STNZSMonitor::finalize() {
  
  debug() << "==> Finalize" << endmsg;
  
  if(!m_OnlineMode){  
    for(unsigned int iBOARD=0; iBOARD<q; iBOARD++){
      for(int kLink=0; kLink<24; kLink++){
      
        iBeetleIT=kLink % 3;
        iBeetleTT=kLink % 4;
        iModuleIT=kLink/3;
        iSectorTT=kLink/4;
        
        
        if ( m_useLink[iBOARD][kLink] ){
          
          for(int kChannel=0; kChannel<128; kChannel++){
            float evt_diff=evt_counter - m_ConvLimit;
            
            if( cmsADCsq[iBOARD][kLink][kChannel]/float(evt_diff) 
                < (cmsADC[iBOARD][kLink][kChannel]/float(evt_diff))*(cmsADC[iBOARD][kLink][kChannel]/float(evt_diff)) ) 
              noise_CMS[iBOARD][kLink][kChannel]=0.0;
            else noise_CMS[iBOARD][kLink][kChannel]
                   =sqrt((cmsADCsq[iBOARD][kLink][kChannel]/float(evt_diff))-(cmsADC[iBOARD][kLink][kChannel]/float(evt_diff))
                         *(cmsADC[iBOARD][kLink][kChannel]/float(evt_diff)));
	    
            mean[iBOARD][kLink][kChannel]=int(absADC[iBOARD][kLink][kChannel]/float(evt_diff));
	    
            if( cmsADCsq[iBOARD][kLink][kChannel]/float(evt_diff) 
                < (absADC[iBOARD][kLink][kChannel]/float(evt_diff))*(absADC[iBOARD][kLink][kChannel]/float(evt_diff)) ) 
              noise_Pulsed[iBOARD][kLink][kChannel]=0.0;
            else noise_Pulsed[iBOARD][kLink][kChannel] = 
                   sqrt((cmsADCsq[iBOARD][kLink][kChannel]/float(evt_diff))-(absADC[iBOARD][kLink][kChannel]/float(evt_diff))
                        *(absADC[iBOARD][kLink][kChannel]/float(evt_diff)));
	    
            if( pedsubADCsq[iBOARD][kLink][kChannel]/float(evt_diff) 
                < (pedsubADC[iBOARD][kLink][kChannel]/float(evt_diff))*(pedsubADC[iBOARD][kLink][kChannel]/float(evt_diff)) ) 
              noise_PedSub[iBOARD][kLink][kChannel]=0.0;
            else noise_PedSub[iBOARD][kLink][kChannel] = 
                  sqrt((pedsubADCsq[iBOARD][kLink][kChannel]/float(evt_diff))-(pedsubADC[iBOARD][kLink][kChannel]/float(evt_diff))
                       *(pedsubADC[iBOARD][kLink][kChannel]/float(evt_diff)));
                        
            CMSNOISEDISTRBOARD[iBOARD]->fill(noise_CMS[iBOARD][kLink][kChannel]);
            MEANDISTRBOARD[iBOARD]->fill(mean[iBOARD][kLink][kChannel]);
            if(m_disable2D == false){
              CMSNOISEBOARD[iBOARD]->fill(kLink*128+kChannel,noise_CMS[iBOARD][kLink][kChannel]);
              RAWNOISEBOARD[iBOARD]->fill(kLink*128+kChannel,noise_PedSub[iBOARD][kLink][kChannel]);
              MEANBOARD[iBOARD]->fill(kLink*128+kChannel,mean[iBOARD][kLink][kChannel]);
            }
            
            
            if(m_detType=="IT"){
              
              
              
              CMSNOISEIT[iBOARD][iModuleIT]->fill(iBeetleIT*128+kChannel,noise_CMS[iBOARD][kLink][kChannel]);
              MEANIT[iBOARD][iModuleIT]->fill(iBeetleIT*128+kChannel,mean[iBOARD][kLink][kChannel]);
              SoverNIT[iBOARD][iModuleIT]->
                fill(iBeetleIT*128+kChannel,mean[iBOARD][kLink][kChannel]/noise_Pulsed[iBOARD][kLink][kChannel]);
              PEDESTALIT[iBOARD][iModuleIT]->fill(iBeetleIT*128+kChannel,pedestals[iBOARD][kLink][kChannel]);
              RAWNOISEIT[iBOARD][iModuleIT]->fill(iBeetleIT*128+kChannel,noise_PedSub[iBOARD][kLink][kChannel]);
              
              
              CMSNOISEDISTRMODIT[iBOARD][iModuleIT]->fill(noise_CMS[iBOARD][kLink][kChannel]);
              
            }
            
            if(m_detType=="TT"){
              
              CMSNOISETT[iBOARD][iSectorTT]->fill(iBeetleTT*128+kChannel,noise_CMS[iBOARD][kLink][kChannel]);
              MEANTT[iBOARD][iSectorTT]->fill(iBeetleTT*128+kChannel,mean[iBOARD][kLink][kChannel]);
              
              SoverNTT[iBOARD][iSectorTT]
                ->fill(iBeetleTT*128+kChannel,mean[iBOARD][kLink][kChannel]/noise_Pulsed[iBOARD][kLink][kChannel]);
              
              PEDESTALTT[iBOARD][iSectorTT]->fill(iBeetleTT*128+kChannel,pedestals[iBOARD][kLink][kChannel]);
              RAWNOISETT[iBOARD][iSectorTT]->fill(iBeetleTT*128+kChannel,noise_PedSub[iBOARD][kLink][kChannel]);
              
              
              
              CMSNOISEDISTRMODTT[iBOARD][iSectorTT]->fill(noise_CMS[iBOARD][kLink][kChannel]);
              
              
            }
          }
        }
      }
    }
  }
  
  return GaudiAlgorithm::finalize(); 
  // must be called after all other actions
}

//=============================================================================

