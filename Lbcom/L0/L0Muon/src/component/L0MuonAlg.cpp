// $Id: L0MuonAlg.cpp,v 1.1 2006-10-20 10:37:28 jucogan Exp $
#include <algorithm>
#include <math.h>
#include <set>
#include <boost/dynamic_bitset.hpp>

#include "L0MuonAlg.h"

#include "ProcessorKernel/Property.h"
#include "ProcessorKernel/RegisterFactory.h"
#include "ProcessorKernel/Register.h"
#include "ProcessorKernel/TileRegister.h"
#include "L0MuonKernel/L0MuonKernelFromXML.h"
#include "L0MuonKernel/DecodeRawBufferCtrl.h"
#include "L0MuonKernel/DecodeRawBufferProc.h"
#include "L0MuonKernel/Utilities.h"


/// Gaudi interfaces
#include "GaudiKernel/IChronoStatSvc.h"

/// Gaudi factories for algorithms 
#include "GaudiKernel/AlgFactory.h"

/// Utility classes
#include "GaudiKernel/IToolSvc.h"   

// from MuonEvent
#include "Event/MuonDigit.h"
// from L0Event
#include "Event/L0MuonBase.h"
#include "Event/L0ProcessorData.h"

// from DAQEvent
#include "Event/RawEvent.h"


static const AlgFactory<L0MuonAlg>  Factory;
const IAlgFactory& L0MuonAlgFactory = Factory;

L0MuonAlg::L0MuonAlg(const std::string& name,
                                         ISvcLocator* pSvcLocator) 
  :GaudiAlgorithm(name, pSvcLocator)
 
{
  
  m_muonBuffer = 0;

  m_configfile="L0MuonKernel.xml";

  m_foiXSize.push_back(4); // 0-> Xfoi in M1
  m_foiXSize.push_back(5); // 1-> Xfoi in M2
  m_foiXSize.push_back(0); // 2-> Xfoi in M3
  m_foiXSize.push_back(2); // 3-> Xfoi in M4
  m_foiXSize.push_back(4); // 4-> Xfoi in M5

  m_foiYSize.push_back(0); // 0-> Yfoi in M1
  m_foiYSize.push_back(0); // 1-> Yfoi in M2
  m_foiYSize.push_back(0); // 2-> Yfoi in M3
  m_foiYSize.push_back(1); // 3-> Yfoi in M4
  m_foiYSize.push_back(1); // 4-> Yfoi in M5

  m_ptParameters.push_back(545.);  // 0-> Z(MAG)-Z(VTX)
  m_ptParameters.push_back(665.5); // 1-> Z(M1)-Z(MAG) 
  m_ptParameters.push_back(309.5); // 2-> Z(M2)-Z(M1)  
  m_ptParameters.push_back(1.39);  // 3-> alpha ? (kick ?)
  m_ptParameters.push_back(1210.); // 4-> Z position of M1 
  m_ptParameters.push_back(1527.); // 5-> Z position of M2 
  m_ptParameters.push_back(1647.); // 6-> Z position of M3 
  m_ptParameters.push_back(40.);   // 7-> Precision on PT (MeV)
  m_ptParameters.push_back(7.);    // 8-> Number of bins in PT
  
  declareProperty("ConfigFile"         , m_configfile);
  declareProperty("FoiXSize"           , m_foiXSize);
  declareProperty("FoiYSize"           , m_foiYSize);
  declareProperty("PtParameters"       , m_ptParameters); 
  declareProperty("StoreInBuffer"      , m_storeFlag = true);  
  declareProperty("IgnoreM1"           , m_ignoreM1 = false );
  declareProperty("DebugMode"          , m_debug = false );
  declareProperty("BufferCompression"  , m_zip = true );
  declareProperty("L0BufferPath"       , m_l0BufPath = "./" );
}


StatusCode L0MuonAlg::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  IChronoStatSvc * svc = chronoSvc();
  svc->chronoStart("L0MuonTrigger Initialize");

  // Set the layouts used to fill the OLs
  setLayouts();
  
  // Instanciate the MuonTrigger Units and Registers
  std::string xmlFileName = m_configfile;
  info() <<  "XML file = " << xmlFileName << endmsg;
  m_muontriggerunit = dynamic_cast< L0Muon::MuonTriggerUnit * >(L0Muon::L0MuonKernelFromXML(xmlFileName));
  debug() <<  "MuonTrigger build from xml "<< endmsg;

  // Set debug mode
  if (m_debug) {
    //m_muontriggerunit->dumpUnitTree();
    m_muontriggerunit->setDebugMode();
    info() <<  "MuonTrigger debug mode "<< endmsg;
  }
  
  // Set properties
  int nprop = setL0MuonProperties();
  debug() <<  "MuonTrigger properties set ("<< nprop <<")"<< endmsg;

  // Initialize
  m_muontriggerunit->initialize();
  debug() <<  "MuonTrigger initialized"<< endmsg;

  // Init counters for final report
  m_totNumCand = 0;
  m_totTrigEvt = 0;
  m_totRawSize = 0;
  m_totEvent   = 0;
    
  svc->chronoStop("L0MuonTrigger Initialize");
  svc->chronoDelta("L0MuonTrigger Initialize",IChronoStatSvc::KERNEL);
  svc->chronoPrint("L0MuonTrigger Initialize");
  

  return StatusCode::SUCCESS;			  					  		      
}



StatusCode L0MuonAlg::execute()
{

  debug() << "** Start execution:" << endreq;
  IChronoStatSvc * svc = chronoSvc();
  svc->chronoStart("L0MuonTrigger Execute");

  debug() << "Chrono service started" << endreq;
  StatusCode sc;

  debug() << "Read data from digits ..." << endreq; 
  sc = fillOLsfromDigits();

  // Measure the time needed for loading OL's only
  //return StatusCode::SUCCESS;

  if( sc.isFailure() ) {
    error() << "Failed to load OLs" << endreq;
    return sc;
  }  

  // Preexecution phase: data exchange between PUs
  debug() << "Preexecute of MuonKernel units ..." << endreq;
  m_muontriggerunit->preexecute();

  // Execution phase: search for candidates and fill output registers
  debug() << "Execution of MuonKernel units ..." << endreq;
  m_muontriggerunit->execute();

  // Fill the Raw Event container
  int rawSize=0;
  if ( m_storeFlag ) {
    debug() << "Fill RawEvent ..." << endreq;
    rawSize=fillRawEvent();
    if (rawSize<0) return  StatusCode::FAILURE;
  }

  // Fill the container for the L0DU (L0ProcessorData)
  debug() << "Fill L0ProcessorData ..." << endreq;
  int ncand = fillL0ProcessorData();
  if (ncand<0) return  StatusCode::FAILURE;

  // Increment counters
  m_totNumCand+=ncand;
  if (ncand>0) m_totTrigEvt++;
  m_totEvent++;
  m_totRawSize += rawSize;

  // Debug output
  debug() << "Size of the RawEvent bank: rawSize= "<<rawSize <<" (32 bits words) "<< endreq;
  debug() << "Number of candidates sent to L0DU: ncand= "<<ncand << endreq;
  //if (ncand>0) debug() << "Candidates sent to L0DU are: " << dumpCandidates() << endreq;
  debug() << "Raw registers: " <<dumpRawRegisters() << endreq;

  // Postexecution phase: reset registers
  debug() << "Postexecution of MuonKernel units ..." << endreq;
  m_muontriggerunit->postexecute();

  debug() << "Execution done." << endreq;

  //std::cout << "Leaving after crate loop" << std::endl;
  //return StatusCode::SUCCESS;
   
  svc->chronoStop("L0MuonTrigger Execute");
  svc->chronoDelta("L0MuonTrigger Execute", IChronoStatSvc::KERNEL);
  //if( MSG::DEBUG >= log.level() ) svc->chronoPrint("L0MuonTrigger Execute");

  return StatusCode::SUCCESS;
  
}


StatusCode L0MuonAlg::finalize()
{


  delete m_muontriggerunit;

  //  if( 0 != m_iTileXYZTool ) {
  //  toolSvc()->releaseTool( m_iTileXYZTool );
  // m_iTileXYZTool = 0;
  //  }

  info() << "- -------------------------------------------------------------------"<<endmsg;
  info() << "- ==============> Final summary of the L0Muon trigger <=============="<<endmsg;
  info() << "- Total number of events processed           : "
         <<format("%8d",m_totEvent)<<endmsg;
  if (0!=m_totEvent)
  {  
    info() << "- Total number of candidates                 : "
           <<format("%8d",m_totNumCand) <<endmsg;
    info() << "- Number of events with at least 1 candidate : "
           <<format("%8d",m_totTrigEvt) 
           <<format("  (%6.2f\%)",(100.*m_totTrigEvt)/m_totEvent) << endmsg;
    info() << "- Number of candidate per event              : "
           <<format("      %5.2f",(1.*m_totNumCand)/m_totEvent) << endmsg;
    info() << "- Average bank size                          : "
           <<format("  %8.1f Bytes",(4.*m_totRawSize)/m_totEvent)<<endmsg;
  }
  info() << "- -------------------------------------------------------------------"<<endmsg;
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}


void L0MuonAlg::setLayouts()
{
  m_layout=MuonSystemLayout(MuonStationLayout(MuonLayout(24,8)),
                            MuonStationLayout(MuonLayout(48,8)),
                            MuonStationLayout(MuonLayout(48,8)),
                            MuonStationLayout(MuonLayout(12,8)),
                            MuonStationLayout(MuonLayout(12,8)));

  m_ollayout = MuonSystemLayout(MuonStationLayout(MuonLayout(2,4)),
                                MuonStationLayout(MuonLayout(4,1),
                                                  MuonLayout(4,2),
                                                  MuonLayout(2,2),
                                                  MuonLayout(2,2)),
                                MuonStationLayout(MuonLayout(4,1),
                                                  MuonLayout(4,2),
                                                  MuonLayout(2,2),
                                                  MuonLayout(2,2)),
                                MuonStationLayout(MuonLayout(2,2)),
                                MuonStationLayout(MuonLayout(2,2)));


  m_lulayout=MuonSystemLayout(MuonStationLayout(MuonLayout(0,0)),
                              MuonStationLayout(MuonLayout(8,1),
                                                MuonLayout(4,2),
                                                MuonLayout(2,2),
                                                MuonLayout(2,2)),
                              MuonStationLayout(MuonLayout(8,1),
                                                MuonLayout(4,2),
                                                MuonLayout(2,2),
                                                MuonLayout(2,2)),
                              MuonStationLayout(MuonLayout(0,0),
                                                MuonLayout(4,2),
                                                MuonLayout(2,2),
                                                MuonLayout(2,2)),
                              MuonStationLayout(MuonLayout(0,0),
                                                MuonLayout(4,2),
                                                MuonLayout(2,2),
                                                MuonLayout(2,2)));	


  m_stripH =MuonSystemLayout(MuonStationLayout(MuonLayout(0,0)),
                             MuonStationLayout(MuonLayout(8,8),
                                               MuonLayout(4,8),
                                               MuonLayout(2,8),
                                               MuonLayout(2,8)),
                             MuonStationLayout(MuonLayout(8,8),
                                               MuonLayout(4,8),
                                               MuonLayout(2,8),
                                               MuonLayout(2,8)),
                             MuonStationLayout(MuonLayout(0,0),
                                               MuonLayout(4,8),
                                               MuonLayout(2,8),
                                               MuonLayout(2,8)),
                             MuonStationLayout(MuonLayout(0,0),
                                               MuonLayout(4,8),
                                               MuonLayout(2,8),
                                               MuonLayout(2,8)));

  m_stripV =MuonSystemLayout(MuonStationLayout(MuonLayout(0,0)),
                             MuonStationLayout(MuonLayout(48,1),
                                               MuonLayout(48,2),
                                               MuonLayout(48,2),
                                               MuonLayout(48,2)),
                             MuonStationLayout(MuonLayout(48,1),
                                               MuonLayout(48,2),
                                               MuonLayout(48,2),
                                               MuonLayout(48,2)),
                             MuonStationLayout(MuonLayout(0,0),
                                               MuonLayout(12,2),
                                               MuonLayout(12,2),
                                               MuonLayout(12,2)),
                             MuonStationLayout(MuonLayout(0,0),
                                               MuonLayout(12,2),
                                               MuonLayout(12,2),
                                               MuonLayout(12,2)));
}


int L0MuonAlg::setL0MuonProperties()
{

  // Translate the gaudi Algorithms properties into the L0MuonKernel properties:

  std::map<std::string,L0Muon::Property> properties;
  std::string prop;

  prop="";
  for (std::vector<double>::iterator iparam =  m_ptParameters.begin();iparam!= m_ptParameters.end();iparam++) {
    char buf[128];
    sprintf(buf,"%f",(*iparam));
    prop+=buf;
    iparam++;
    if( iparam!=  m_ptParameters.end()) prop+=",";
    iparam--;
  }
  properties["ptParameters"]   = L0Muon::Property(prop);  

  prop="";
  for (std::vector<int>::iterator ifoi =  m_foiXSize.begin();ifoi!= m_foiXSize.end();ifoi++) {
    char buf[128];
    sprintf(buf,"%d",(*ifoi));
    prop+=buf;
    ifoi++;
    if( ifoi!=  m_foiXSize.end()) prop+=",";
    ifoi--;
  }
  properties["foiXSize"]       = L0Muon::Property(prop);

  prop="";
  for (std::vector<int>::iterator ifoi =  m_foiYSize.begin();ifoi!= m_foiYSize.end();ifoi++) {
    char buf[128];
    sprintf(buf,"%d",(*ifoi));
    prop+=buf;
    ifoi++;
    if( ifoi!=  m_foiYSize.end()) prop+=",";
    ifoi--;
  }
  properties["foiYSize"]       = L0Muon::Property(prop);

  properties["ignoreM1"]       = m_ignoreM1 ? L0Muon::Property("1") : L0Muon::Property("0");

  properties["BufferCompression"]= m_zip ? L0Muon::Property("1") : L0Muon::Property("0");

  properties["L0BufferPath"]= m_l0BufPath;

  info() << "MuonTriggerUnit properties are:"<<endreq;
  for (std::map<std::string,L0Muon::Property>::iterator ip= properties.begin(); ip!=properties.end();ip++){
    info() << " "<< (*ip).first << " = "<< ((*ip).second).value() <<endreq;
  }

  // Set the properties of the MuonTriggerUnit 
  m_muontriggerunit->setProperties(properties);

  return properties.size();
}

StatusCode L0MuonAlg::fillOLsfromDigits()
{
  debug() << "fillOLsfromDigits:  IN "<<endmsg;

  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();
  
  //===============================
  // get Muon digitisations
  //===============================

  std::vector<LHCb::MuonTileID> ddigits;
  if (exist<LHCb::MuonDigits>( LHCb::MuonDigitLocation::MuonDigit ) )
  {
    // The MuonDigits are on the TES (Boole); 
    // get them and fill MuonTileID list
    LHCb::MuonDigits* digits = get<LHCb::MuonDigits>( LHCb::MuonDigitLocation::MuonDigit );  
    debug() << "fillOLsfromDigits:   digits="<< digits <<endmsg;
    LHCb::MuonDigits::const_iterator did;
    for( did = digits->begin() ; did != digits->end() ; did++ ){  
      LHCb::MuonTileID mkey = (*did)->key();
      ddigits.push_back(mkey);    
    }

  } 
  else 
  {
    // The MuonDigits are not on the TES (DaVinci); 
    // use the MuonRawBufer tool to produce them from the raw buffer
    if(!m_muonBuffer) {
      // First call: initialize the pointer to the Muon Raw Buffer Interface
      m_muonBuffer=tool<IMuonRawBuffer>("MuonRawBuffer");
      if(!m_muonBuffer) {    
        error() << "No pointer to muon raw buffer interface tool"<<m_muonBuffer<<endmsg;
        return StatusCode::FAILURE;
      }
    }
    debug() << "m_muonBuffer "<<m_muonBuffer<<endmsg;
    ddigits=m_muonBuffer->getTile();
  }  

  debug() << "fillOLsfromDigits:  ddigits.size()= "<<ddigits.size()<<endmsg;

  std::vector<LHCb::MuonTileID>::const_iterator id;

  for( id = ddigits.begin() ; id != ddigits.end() ; id++ ){
    LHCb::MuonTileID mkey = *id;
    int sta = mkey.station(); 
    LHCb::MuonTileID olID = m_ollayout.contains(mkey);
    char bufnm[1024];
    //sprintf(bufnm,"(R%d,%d,%d)",olID.region(),olID.nX(),olID.nY());
    sprintf(bufnm,"(Q%d,R%d,%d,%d)",
            olID.quarter(),olID.region(),olID.nX(),olID.nY());           
    char buf[4096];
    char* format = "OL_%d_%s";
    sprintf(buf,format,sta,bufnm);
    
    if (m_debug) std::cout << "fillOLsfromDigits:     "
                           <<" mkey: "<<mkey.toString()
                           <<" ; buf: "<<buf<<"\n";

    L0Muon::TileRegister* pReg = rfactory->createTileRegister(buf,0);
    pReg->setTile(mkey);
  }
    
  if (m_debug) rfactory->dump();  
    
  return StatusCode::SUCCESS;
}


int L0MuonAlg::fillRawEvent()
{
  int totSize=0; // Size of the RawEvent in units of 32 bits words

  char buf[4096];
  char* format ;
  L0Muon::Register * rawbufreg;
  boost::dynamic_bitset<> rawbufbitset;
  unsigned int size;

  // Pointer to the register factory
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();

  // Register containing the output of the TELL1 linked to the Controler board
  format = "RAWBUFFER_CTRL";
  sprintf(buf,format);
  rawbufreg = rfactory->searchRegister(buf);
  if (rawbufreg==0) {
    error() <<"fillRawEvent: "<<buf<<" register not found" << endmsg;
    return -1;
  }
  
  //debug() << "*** RAWBUFFER CTRL(register) ***"<<endreq;
  rawbufbitset=rawbufreg->getBitset();
  size = 0;
  std::vector<unsigned int> rawCtrlOutput; 
  while(size<rawbufbitset.size()) {
    boost::dynamic_bitset<> word = (rawbufbitset>>size);
    word.resize(32);
    //debug() << word <<endreq;
    size+=32;
    rawCtrlOutput.push_back(word.to_ulong());
  }
  totSize += rawCtrlOutput.size();
  LHCb::RawEvent* raw = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
  raw->addBank(0, LHCb::RawBank::L0Muon,0,rawCtrlOutput);

  // Registers containing the output of the TELL1 linked to the Processing boards 
  for (int iq=0;iq<4;iq++) {
    format = "RAWBUFFER_PROCQ%d";
    sprintf(buf,format, iq+1);
    L0Muon::Register * rawbufprocreg = rfactory->searchRegister(buf);
    std::vector<unsigned int> rawProcOutput; 
    if (rawbufprocreg>0) {  
      //debug() << "* RAWBUFFER PROC Q"<<iq+1<<" (register) *"<<endreq;
      boost::dynamic_bitset<> rawbufbitset=rawbufprocreg->getBitset();
      size = 0;
      while(size<rawbufbitset.size()) {
        boost::dynamic_bitset<> word = (rawbufbitset>>size);
        word.resize(32);
        //debug() << word <<endreq;
        size+=32;
        rawProcOutput.push_back(word.to_ulong());
      }
    }
    totSize += rawProcOutput.size();
    LHCb::RawEvent* raw = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
    raw->addBank(iq+1, LHCb::RawBank::L0Muon,0,rawProcOutput);
  }
    
  return totSize;

}

int L0MuonAlg::fillL0ProcessorData()
{

  int ncand=0;
  // Initialize the words to be sent to L0DU
  unsigned long cu[4],su[4];
  unsigned long  index[4];
  for (int iq=0;iq<4;iq++) {
    cu[iq] = L0MuonBase::emptyMuonData ;
    su[iq] = L0MuonBase::emptyMuonData ;
    index[iq] = 0;
  }

  // Pointer to the register factory
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();

  // Register containing the output of the TELL1 linked to the Controler board
  char buf[4096];
  char* format ;
  format = "RAWBUFFER_CTRL";
  sprintf(buf,format);
  L0Muon::Register * rawbufreg= rfactory->searchRegister(buf);
  if (rawbufreg==0) {
    error() <<"fillL0ProcessorData: "<<buf<<" register not found" << endmsg;
    return -1;
  }

  boost::dynamic_bitset<> rawbufbitset=rawbufreg->getBitset();

  // Decode the output candidate field of the register
  L0Muon::DecodeRawBufferCtrl decodedRawBufCtrl ;
  decodedRawBufCtrl.setBitset(rawbufbitset);
  std::vector<L0Muon::MuonCandidate> candidatesCtrl = decodedRawBufCtrl.candidatesCTRL();
  ncand=candidatesCtrl.size();


  // Fill the words to be sent to L0DU
  std::vector<L0Muon::MuonCandidate>::iterator icand;
  for (icand=candidatesCtrl.begin(); icand<candidatesCtrl.end(); icand++) {
    // Prepare words to be sent to the L0DU
    int quarter = (*icand).quarter();
    debug() << "==> fillL0ProcessorData candidate quarter= "<< quarter <<"\n" << endreq;
    debug() << "==> fillL0ProcessorData candidate pT= "<< (*icand).pT() <<"\n" << endreq;
    debug() << "==> fillL0ProcessorData candidate PU= "<< (*icand).pu() <<"\n" << endreq;
    debug() << "==> fillL0ProcessorData candidate board= "<< (*icand).board() <<"\n" << endreq;
    debug() << "==> fillL0ProcessorData candidate addM3= "<< (*icand).addM3() <<"\n" << endreq;
    debug() << "==> fillL0ProcessorData candidate charge= "<< (*icand).charge() <<"\n" << endreq;
    if (index[quarter]==0) {
      cu[quarter] += ( (*icand).pT()     << L0DUBase::Muon::Pt1::Shift) & L0DUBase::Muon::Pt1::Mask;
      cu[quarter] += ( (*icand).addM3()  << L0DUBase::Muon::Address1::Shift) & L0DUBase::Muon::Address1::Mask;
      su[quarter] += ( (*icand).pu()     << L0DUBase::Muon::Pu1::Shift) & L0DUBase::Muon::Pu1::Mask;
      su[quarter] += ( (*icand).board()  << L0DUBase::Muon::Pb1::Shift) & L0DUBase::Muon::Pb1::Mask;
      su[quarter] += ( (*icand).charge() << L0DUBase::Muon::Sign1::Shift) & L0DUBase::Muon::Sign1::Mask;  
    } else {
      cu[quarter] += ( (*icand).pT()     << L0DUBase::Muon::Pt2::Shift) & L0DUBase::Muon::Pt2::Mask;
      cu[quarter] += ( (*icand).addM3()  << L0DUBase::Muon::Address2::Shift) & L0DUBase::Muon::Address2::Mask;
      su[quarter] += ( (*icand).pu()     << L0DUBase::Muon::Pu2::Shift) & L0DUBase::Muon::Pu2::Mask;
      su[quarter] += ( (*icand).board()  << L0DUBase::Muon::Pb2::Shift) & L0DUBase::Muon::Pb2::Mask;
      su[quarter] += ( (*icand).charge() << L0DUBase::Muon::Sign2::Shift) & L0DUBase::Muon::Sign2::Mask;  
    }
    index[quarter]++;
  }
  for (int iq=0;iq<4;iq++) {
    debug() << "==> fillL0ProcessorData cu["<<iq<<"] = " << cu[iq] <<"\n" << endreq;;
    debug() << "==> fillL0ProcessorData su["<<iq<<"] = " << su[iq] <<"\n" << endreq;;
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

  return ncand;
  
}



std::string L0MuonAlg::dumpCandidates()
{

  std::string strDump="\n";

  // Pointer to the register factory
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();

  // Register containing the output of the TELL1 linked to the Controler board
  char buf[4096];
  char* format ;
  format = "RAWBUFFER_CTRL";
  sprintf(buf,format);
  L0Muon::Register * rawbufreg= rfactory->searchRegister(buf);
  boost::dynamic_bitset<> rawbufbitset=rawbufreg->getBitset();

  // Decode the output candidate field of the register
  L0Muon::DecodeRawBufferCtrl decodedRawBufCtrl ;
  decodedRawBufCtrl.setBitset(rawbufbitset);
  std::vector<L0Muon::MuonCandidate> candidatesCtrl = decodedRawBufCtrl.candidatesCTRL();

  // Fill the words to be sent to L0DU
  std::vector<L0Muon::MuonCandidate>::iterator icand;
  for (icand=candidatesCtrl.begin(); icand<candidatesCtrl.end(); icand++){
    strDump+=(*icand).dump();
  }
  
  return strDump;

}

std::string L0MuonAlg::dumpRawRegisters()
{
  std::stringstream sstrDump;
  sstrDump << "\n";
  
  char buf[4096];
  char* format ;

  // Pointer to the register factory
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();
  
  // Parse the RawBuffers
  L0Muon::Register * rawbufreg;
  std::vector<L0Muon::MuonCandidate>::iterator icand;
  boost::dynamic_bitset<> rawbufbitset;
    
  // Point to the RawBuffer of the controller boards
  format = "RAWBUFFER_CTRL";
  sprintf(buf,format);
  rawbufreg = rfactory->searchRegister(buf);
  rawbufbitset=rawbufreg->getBitset();

  sstrDump << "=======================\n";
  sstrDump << "=RawBufferCtrl bit set:\n";
  sstrDump << "=======================\n";
  sstrDump << " size = "<< rawbufbitset.size()<<"\n";
  //sstrDump << rawbufbitset <<"\n";

  sstrDump << " Decode RawBufferCtrl:\n";
  sstrDump << " ---------------------\n";
  L0Muon::DecodeRawBufferCtrl decodedRawBufCtrl ;
  decodedRawBufCtrl.setBitset(rawbufbitset);
    
  std::vector<L0Muon::MuonCandidate> candidatesCtrl = decodedRawBufCtrl.candidatesCTRL();
  sstrDump << "  * Number of candidatesCtrl: " << candidatesCtrl.size() <<"\n";
  for (icand=candidatesCtrl.begin(); icand<candidatesCtrl.end(); icand++) {
    sstrDump << (*icand).dump();
  }
  std::vector<L0Muon::MuonCandidate> candidatesBcsu = decodedRawBufCtrl.candidatesBCSU();
  sstrDump << "  * Number of candidatesBcsu: " << candidatesBcsu.size() <<"\n";
  for (icand=candidatesBcsu.begin(); icand<candidatesBcsu.end(); icand++) {
    sstrDump << (*icand).dump();
  }

  // Loop over the Rawbuffer of the processing boards
  for (int iq=0; iq<4; iq++){
    format = "RAWBUFFER_PROCQ%d";
    sprintf(buf,format,iq+1);
    rawbufreg = rfactory->searchRegister(buf);
    if (rawbufreg==0) continue;
    rawbufbitset=rawbufreg->getBitset();

    sstrDump << "========================\n";
    sstrDump << "= RawBufferProc bit set:   Q"<<iq+1<<"\n";
    sstrDump << "========================\n";
    sstrDump << " size = "<< rawbufbitset.size()<<" " << rawbufbitset.count()<<"\n";


    sstrDump << " Decode RawBufferProc:\n";
    sstrDump << " ---------------------\n";
    L0Muon::DecodeRawBufferProc decodedRawBufProc ;
    decodedRawBufProc.setBitset(rawbufbitset);

    std::vector<L0Muon::MuonCandidate> candidatesBcsu = decodedRawBufProc.candidatesBCSU();
    sstrDump << "  * Number of candidatesBcsu: " << candidatesBcsu.size() <<"\n";
    for (icand=candidatesBcsu.begin(); icand<candidatesBcsu.end(); icand++) {
      sstrDump << (*icand).dump();
    }
    std::vector<L0Muon::MuonCandidate> candidatesPu = decodedRawBufProc.candidatesPU();
    sstrDump << "  * Number of candidatesPu: " << candidatesPu.size() <<"\n";
    for (icand=candidatesPu.begin(); icand<candidatesPu.end(); icand++) {
      sstrDump << (*icand).dump();
    }
      
    boost::dynamic_bitset<> code = decodedRawBufProc.nonEmptyPU();
    sstrDump << "  * Non empty PU:\n"<< code <<"\n";

    boost::dynamic_bitset<> data = decodedRawBufProc.inputDATA(m_zip);
    sstrDump << "  * Input data: size="<< data.size() <<" set:"<<data.count()<<"\n";

    if (m_debug) {
      boost::dynamic_bitset<>::size_type cursor=data.find_next(0);
      int icursor=0;
      while(cursor<data.size()){
        sstrDump << " "<< ++icursor<<":"<<cursor << " ";
        cursor = data.find_next(cursor);
      }
      sstrDump << "\n";
    }
  }

  return sstrDump.str();

}

