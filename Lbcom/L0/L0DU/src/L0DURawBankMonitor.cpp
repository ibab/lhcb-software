// $Id: L0DURawBankMonitor.cpp,v 1.15 2009-09-16 16:09:27 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiUtils/Aida2ROOT.h"
// from lhcb
#include "Event/ODIN.h" 
// local
#include "L0DURawBankMonitor.h"


//-----------------------------------------------------------------------------
// Implementation file for class : L0DURawBankMonitor
//
// 2008-03-03 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( L0DURawBankMonitor );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0DURawBankMonitor::L0DURawBankMonitor( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : Calo2Dview ( name , pSvcLocator )
{

  declareProperty( "L0DUFromRawTool"   , m_fromRawTool = "L0DUFromRawTool" );  
  declareProperty( "EmulatorTool"      , m_emulatorTool= "L0DUEmulatorTool");
  declareProperty( "Convert"           , m_conv = false);
  declareProperty( "DataMonitor"       , m_data = true);
  declareProperty( "AddressMonitor"    , m_addr = true);
  declareProperty( "ProcessingMonitor" , m_proc = true);
  declareProperty( "StatusMonitor"     , m_status = true);
  declareProperty( "FullMonitoring"    , m_full = false);
  declareProperty( "DataReBinFactor"   , m_bin = 1);
  declareProperty( "DecodeBank"        , m_decode = true);
  declareProperty( "LocationsForEmulatorCheck"   , m_locs);
  declareProperty( "FullWarning"       , m_warn = false);
  declareProperty( "xLabelOptions"     , m_lab ="" );
  declareProperty( "genericPath"       , m_generic = true );
  declareProperty( "CaloReadoutTool"   ,  m_caloTool  = "CaloDataProvider" );
    

  setHistoDir( name );
}
//=============================================================================
// Destructor
//=============================================================================
L0DURawBankMonitor::~L0DURawBankMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode L0DURawBankMonitor::initialize() {
  StatusCode sc = Calo2Dview::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_first = true;
  if(m_full){
    m_data=true;
    m_addr=true;
    m_proc=true;
    m_status=true;
  }



  // get the L0tools
  m_fromRaw  = tool<IL0DUFromRawTool>( m_fromRawTool , m_fromRawTool );
  m_fromRaw->fillDataMap();
  m_condDB   = tool<IL0CondDBProvider>("L0CondDBProvider");
  m_odin     = tool<IEventTimeDecoder>("OdinTimeDecoder","OdinDecoder",this);
  m_emuTool  = tool<IL0DUEmulatorTool>(m_emulatorTool, m_emulatorTool,this);
  // Spd DAQ tool
  m_spd = tool<ICaloDataProvider>( m_caloTool , "SpdReadoutTool" , this );

  //
  bookHistos();
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0DURawBankMonitor::execute() {

  debug() << "==> Execute" << endmsg;

  // get odin info
  m_odin->getTime();
  unsigned int odBX = 0;
  if( exist<LHCb::ODIN>( LHCb::ODINLocation::Default) ){
    LHCb::ODIN* odin = get<LHCb::ODIN> ( LHCb::ODINLocation::Default );
    odBX = odin->bunchId();
  }else{
    Warning( "Emtpy location for ODIN '"+ LHCb::ODINLocation::Default +"'" ,StatusCode::SUCCESS).ignore();
  }


  
  bool ok ;
  if(m_decode){
    ok = m_fromRaw->decodeBank();
  }else{
    ok = m_fromRaw->report().valid();
  }  
  if(!ok){
    Error("Readout error : unable to monitor the L0DU rawBank", StatusCode::SUCCESS,StatusCode::SUCCESS).ignore();
    return StatusCode::SUCCESS;
  }
  

  LHCb::L0DUReport report = m_fromRaw->report();
  
  double Sgn[2];
  Sgn[0]=+1.;
  Sgn[1]=-1.;
  
  // Input Data monitoring
  if(m_data){
    // Calo
    fill( histo1D(toHistoID("Data/Calo/1")),m_fromRaw->data("Electron(Et)") ,1  );
    fill( histo1D(toHistoID("Data/Calo/2")),m_fromRaw->data("Photon(Et)")   ,1  );
    fill( histo1D(toHistoID("Data/Calo/3")),m_fromRaw->data("LocalPi0(Et)") ,1  );
    fill( histo1D(toHistoID("Data/Calo/4")),m_fromRaw->data("GlobalPi0(Et)"),1  );
    fill( histo1D(toHistoID("Data/Calo/5")),m_fromRaw->data("Hadron(Et)")   ,1  );
    fill( histo1D(toHistoID("Data/Calo/6")),m_fromRaw->data("Spd(Mult)")    ,1  );
    fill( histo1D(toHistoID("Data/SumEt/1")),m_fromRaw->data("Sum(Et)")      ,1  );
    fill( histo1D(toHistoID("Data/SumEt/2")),m_fromRaw->data("Sum(Et,Prev2)")      ,1  );
    fill( histo1D(toHistoID("Data/SumEt/3")),m_fromRaw->data("Sum(Et,Prev1)")      ,1  );
    fill( histo1D(toHistoID("Data/SumEt/4")),m_fromRaw->data("Sum(Et,Next1)")      ,1  );
    fill( histo1D(toHistoID("Data/SumEt/5")),m_fromRaw->data("Sum(Et,Next2)")      ,1  );
    // Pile-Up
    fill( histo1D(toHistoID("Data/Pus/1")),m_fromRaw->data("PUPeak1(Cont)") ,1);
    fill( histo1D(toHistoID("Data/Pus/2")),m_fromRaw->data("PUPeak2(Cont)") ,1);
    fill( histo1D(toHistoID("Data/Pus/3")),m_fromRaw->data("PUHits(Mult)") ,1);
    // Muon
    fill( histo1D(toHistoID("Data/Muon/1")),m_fromRaw->data("M0(Pt)")  * Sgn[m_fromRaw->data("M0(Sgn)")] ,1);
    fill( histo1D(toHistoID("Data/Muon/2")),m_fromRaw->data("M1(Pt)")  * Sgn[m_fromRaw->data("M1(Sgn)")] ,1);
    fill( histo1D(toHistoID("Data/Muon/3")),m_fromRaw->data("M2(Pt)")  * Sgn[m_fromRaw->data("M2(Sgn)")] ,1);
    fill( histo1D(toHistoID("Data/Muon/4")),m_fromRaw->data("M3(Pt)")  * Sgn[m_fromRaw->data("M3(Sgn)")] ,1);
    fill( histo1D(toHistoID("Data/Muon/5")),m_fromRaw->data("M4(Pt)")  * Sgn[m_fromRaw->data("M4(Sgn)")] ,1);
    fill( histo1D(toHistoID("Data/Muon/6")),m_fromRaw->data("M5(Pt)")  * Sgn[m_fromRaw->data("M5(Sgn)")] ,1);
    fill( histo1D(toHistoID("Data/Muon/7")),m_fromRaw->data("M6(Pt)")  * Sgn[m_fromRaw->data("M6(Sgn)")] ,1);
    fill( histo1D(toHistoID("Data/Muon/8")),m_fromRaw->data("M7(Pt)")  * Sgn[m_fromRaw->data("M7(Sgn)")] ,1);
  }
  // data address
  if( m_addr ){
    // Calo
    fillCalo2D(toHistoID("Address/Calo/1") , LHCb::CaloCellID(m_fromRaw->data("Electron(Add)")) , 1.);
    fillCalo2D(toHistoID("Address/Calo/2") , LHCb::CaloCellID(m_fromRaw->data("Photon(Add)"))   , 1.);
    fillCalo2D(toHistoID("Address/Calo/3") , LHCb::CaloCellID(m_fromRaw->data("LocalPi0(Add)")) , 1.);
    fillCalo2D(toHistoID("Address/Calo/4") , LHCb::CaloCellID(m_fromRaw->data("GlobalPi0(Add)")), 1.);
    fillCalo2D(toHistoID("Address/Calo/5") , LHCb::CaloCellID(m_fromRaw->data("Hadron(Add)"))   , 1.);
    // PUs
    fill(histo1D(toHistoID("Address/Pus/1"))  , m_fromRaw->data("PUPeak1(Add)"), 1.);
    fill(histo1D(toHistoID("Address/Pus/2"))  , m_fromRaw->data("PUPeak2(Add)"), 1.);
    // Muon @ToDo
    for(int i = 0 ; i < 8 ; i++){
      std::stringstream n("");
      std::stringstream np("");
      n  << i;
      np << i+1;
      int add =  m_fromRaw->data("M"+n.str()+"(Add)");
      int ad1 = (add & 0x07F);
      int ad2 = (add & 0xF80) >> 7 ;
      fill(histo2D(toHistoID("Address/Muon/"+np.str())) ,  ad2 , ad1 , 1.);
    }
  }
  
  // L0DU Processed Data
  if( m_proc ){
    fill( histo1D(toHistoID("Process/Muon/1")), m_fromRaw->data("Muon1(Pt)")* Sgn[m_fromRaw->data("Muon1(Sgn)")] ,1 );
    fill( histo1D(toHistoID("Process/Muon/2")), m_fromRaw->data("Muon2(Pt)")* Sgn[m_fromRaw->data("Muon2(Sgn)")] ,1 );
    fill( histo1D(toHistoID("Process/Muon/3")), m_fromRaw->data("Muon3(Pt)")* Sgn[m_fromRaw->data("Muon3(Sgn)")] ,1 );
    fill( histo1D(toHistoID("Process/Muon/4")), m_fromRaw->data("DiMuon(Pt)"),1);
    fill( histo1D(toHistoID("Process/Muon/5")), m_fromRaw->muonCleaningPattern(),1);
  }

  // Status
  if(m_status){     
    if(report.decision() )fill( histo1D(toHistoID("Status/BCID/1")), m_fromRaw->bcid().first, 1 );
    // from L0Processor
    fill( histo1D(toHistoID("Status/Muon/1")), (m_fromRaw->data("MuonCU0(Status)") & 0xC)>>2 , 1 );
    fill( histo1D(toHistoID("Status/Muon/2")), (m_fromRaw->data("MuonCU1(Status)") & 0xC)>>2 , 1 );
    fill( histo1D(toHistoID("Status/Muon/3")), (m_fromRaw->data("MuonCU2(Status)") & 0xC)>>2 , 1 );
    fill( histo1D(toHistoID("Status/Muon/4")), (m_fromRaw->data("MuonCU3(Status)") & 0xC)>>2 , 1 );
    fill( histo1D(toHistoID("Status/Muon/5")), (m_fromRaw->data("MuonCU0(Status)") & 0x3) , 1 );
    fill( histo1D(toHistoID("Status/Muon/6")), (m_fromRaw->data("MuonCU1(Status)") & 0x3) , 1 );
    fill( histo1D(toHistoID("Status/Muon/7")), (m_fromRaw->data("MuonCU2(Status)") & 0x3) , 1 );
    fill( histo1D(toHistoID("Status/Muon/8")), (m_fromRaw->data("MuonCU3(Status)") & 0x3) , 1 );
    fill( histo1D(toHistoID("Status/Calo/1")), m_fromRaw->data("Electron(Status)") , 1 );
    fill( histo1D(toHistoID("Status/Calo/2")), m_fromRaw->data("Photon(Status)") , 1 );
    fill( histo1D(toHistoID("Status/Calo/3")), m_fromRaw->data("LocalPi0(Status)") , 1 );
    fill( histo1D(toHistoID("Status/Calo/4")), m_fromRaw->data("GlobalPi0(Status)") , 1 );
    fill( histo1D(toHistoID("Status/Calo/5")), m_fromRaw->data("Hadron(Status)") , 1 );
    fill( histo1D(toHistoID("Status/Calo/6")), m_fromRaw->data("Spd(Status)") , 1 );
    fill( histo1D(toHistoID("Status/Calo/7")), m_fromRaw->data("Sum(Status)") , 1 );
    fill( histo1D(toHistoID("Status/Pus/1")), m_fromRaw->data("PU1(Status)") , 1 );
    fill( histo1D(toHistoID("Status/Pus/2")), m_fromRaw->data("PU2(Status)") , 1 );
    
    int inputStatus = 0;
    if( 0 < (m_fromRaw->data("MuonCU0(Status)") & 0xC) )inputStatus++;
    if( 0 < (m_fromRaw->data("MuonCU1(Status)") & 0xC) )inputStatus++;
    if( 0 < (m_fromRaw->data("MuonCU2(Status)") & 0xC) )inputStatus++;
    if( 0 < (m_fromRaw->data("MuonCU3(Status)") & 0xC) )inputStatus++;
    if( 0 < m_fromRaw->data("Electron(Status)"))inputStatus++;
    if( 0 < m_fromRaw->data("Photon(Status)"))inputStatus++;
    if( 0 < m_fromRaw->data("LocalPi0(Status)"))inputStatus++;
    if( 0 < m_fromRaw->data("GlobalPi0(Status)"))inputStatus++;
    if( 0 < m_fromRaw->data("Hadron(Status)"))inputStatus++;
    if( 0 < m_fromRaw->data("Sum(Status)"))inputStatus++;
    if( 0 < m_fromRaw->data("Spd(Status)"))inputStatus++;
    if( 0 < m_fromRaw->data("PU1(Status)"))inputStatus++;
    if( 0 < m_fromRaw->data("PU2(Status)"))inputStatus++;
    
    // BXID shift
    fill( histo1D(toHistoID("Status/L0DU/BCID/1")), odBX - m_fromRaw->bcid().first, 1 );
    fill( histo1D(toHistoID("Status/L0DU/BCID/2")), (0x7F & m_fromRaw->bcid().first) - m_fromRaw->bcid().second , 1 );

    // L0DU Emulator check
    bool check = true;
    LHCb::L0DUConfig* config = report.configuration();
    if( NULL == config){
      std::stringstream ttck("");
      ttck << format("0x%04X", report.tck() ) ;
      Warning("L0DUConfig for tck = " + ttck.str() + " has not been registered -> cannot perform the emulator check"
              ,StatusCode::SUCCESS).ignore();
    }else{
      std::stringstream local("");
      local << LHCb::L0ProcessorDataLocation::L0DU;
      check = emulatorCheck( config, 1 ,"from " + local.str() ) && check;
      // additional locations 
      int k = 1;
      for(std::vector<std::vector<std::string> >::iterator ilocs = m_locs.begin() ; ilocs != m_locs.end() ; ilocs++){
        k++;
        std::vector<std::string> dataLocs = *ilocs;
        std::stringstream loc("");
        loc << "{";
        for(std::vector<std::string>::iterator iloc = (*ilocs).begin() ; iloc != (*ilocs).end() ; iloc++){
          loc << *iloc << " , ";
        }
        loc << "}";
        config->decrementCounters(); // replay the same event with different input data
        if(m_emuTool->process(config , dataLocs ).isSuccess()){
          check = emulatorCheck( config, k , " from  " + loc.str() ) && check;
        }else{
          Warning("Cannot run the emulator for L0ProcessorData location(s) : " + loc.str() ,StatusCode::SUCCESS).ignore();
        }        
      }
      // reset processor data from L0DU location when changed
      if(m_locs.size() != 0){
        config->decrementCounters(); // replay the same event with different input data
        m_emuTool->process(config , m_fromRaw->L0ProcessorDatas());
      }
    }

    // L0DU Error summary
    fill( histo1D(toHistoID("Status/Summary/1")), -1. , 1 ); // counter
    if(0 < inputStatus){
      fill( histo1D(toHistoID("Status/Summary/1")), L0DUBase::L0DUError::InputData  , 1 );
      if(m_warn)Warning(" Status::Warning : L0DU bank monitor summary : -- input data error bit -- "
                        ,StatusCode::SUCCESS).ignore();
    }
    if( (0x7F & m_fromRaw->bcid().first) != m_fromRaw->bcid().second){
      fill( histo1D(toHistoID("Status/Summary/1")), L0DUBase::L0DUError::BxPGAShift , 1 );
      if(m_warn)Warning("Status::Warning : L0DU bank monitor summary : -- PGA2/3 BXID misaligned -- "
                        ,StatusCode::SUCCESS).ignore();
      debug() << "BCID L0DU/PGA3 : " <<  (0x7F &m_fromRaw->bcid().first) << " / " << m_fromRaw->bcid().second<< endreq;
    }
    if( odBX != m_fromRaw->bcid().first){
      fill( histo1D(toHistoID("Status/Summary/1")), L0DUBase::L0DUError::BxOdinShift , 1 );
      if(m_warn)Warning("Status::Warning : L0DU bank monitor summary : -- ODIN/L0DU BXID misaligned -- "
                        ,StatusCode::SUCCESS).ignore();
      debug() << "BCID L0DU/ODIN : " <<  m_fromRaw->bcid().first << " / " << odBX << endreq;
    }
    if( (m_fromRaw->status() & 0x1) ){
      fill( histo1D(toHistoID("Status/Summary/1")), L0DUBase::L0DUError::Tell1 , 1 );
      if(m_warn)Warning("Status::Warning  : L0DU bank monitor summary : -- TELL1 error bit -- ",StatusCode::SUCCESS).ignore();
    }
    if( (m_fromRaw->status() & 0x2) ){
      fill( histo1D(toHistoID("Status/Summary/1")), L0DUBase::L0DUError::DeMux , 1 );
      if(m_warn)Warning("Status::Warning : L0DU bank monitor summary : -- DeMultiplexer error bit -- "
                        ,StatusCode::SUCCESS).ignore();
    }
    if( (m_fromRaw->status() & 0x4) ){
      fill( histo1D(toHistoID("Status/Summary/1")), L0DUBase::L0DUError::TLK   , 1 );
      if(m_warn)Warning("Status::Warning : L0DU bank monitor summary : -- TLK error bit -- ",StatusCode::SUCCESS).ignore();
    }
    if( (m_fromRaw->status() & 0x8) ){
      fill( histo1D(toHistoID("Status/Summary/1")), L0DUBase::L0DUError::IdleLink , 1 );
      if(m_warn)Warning("Status::Warning : L0DU bank monitor summary : -- Idle link error bit -- ",StatusCode::SUCCESS).ignore();
    }
    if( !check ){
      fill( histo1D(toHistoID("Status/Summary/1")), L0DUBase::L0DUError::EmulatorCheck , 1 );
      if(m_warn)Warning("Status::Warning : L0DU bank monitor summary : -- Emulator check error -- ",StatusCode::SUCCESS).ignore();
    }    
    if( NULL == config ){
      fill( histo1D(toHistoID("Status/Summary/1")), L0DUBase::L0DUError::UnknownTCK , 1 );
      if(m_warn)Warning("Status::Warning  : L0DU bank monitor summary : -- unknown TCK -- ",StatusCode::SUCCESS).ignore();
    }
    unsigned int nSpdDAQ = 0;
    if( m_spd->ok() )nSpdDAQ = m_spd->adcs().size();
    if( m_fromRaw->data("Spd(Mult)") != nSpdDAQ ){
      fill( histo1D(HistoID("Status/Summary/1")), L0DUBase::L0DUError::WrongSpdMult , 1 );
      if(m_warn)Warning("Status::Warning  : L0DU bank monitor summary : -- Wrong Spd Mult -- ",StatusCode::SUCCESS).ignore();
    }
  }
  if(m_first)m_first=false;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0DURawBankMonitor::finalize() {

  debug() << "==> Finalize" << endmsg;

  return Calo2Dview::finalize();  // must be called after all other actions
}

//=============================================================================


void L0DURawBankMonitor::bookHistos() {
  double caloEt =  m_conv ? m_condDB->caloEtScale() : 1.;
  double muonPt =  m_conv ? m_condDB->muonPtScale() : 1.;
  
  if(m_data){
    
    book1D( toHistoID("Data/Calo/1")  ,  "Electron(Et)"   ,  0. , 256. * caloEt   ,  256/m_bin);
    book1D( toHistoID("Data/Calo/2")  ,  "Photon(Et)"     ,  0. , 256. * caloEt    , 256/m_bin);
    book1D( toHistoID("Data/Calo/3")  ,  "LocalPi0(Et)"   ,  0. , 256. * caloEt    , 256/m_bin);
    book1D( toHistoID("Data/Calo/4")  ,  "GlobalPi0(Et)"  ,  0. , 256. * caloEt    , 256/m_bin);
    book1D( toHistoID("Data/Calo/5")  ,  "Hadron(Et)"     ,  0. , 256. * caloEt    , 256/m_bin);
    book1D( toHistoID("Data/Calo/6")  ,  "Spd(Mult)"      ,  0. , 16384.           , 16384/m_bin);
    //
    book1D( toHistoID("Data/SumEt/1")  ,  "Sum(Et,T0)"           ,  0. , 16384. * caloEt  , 16384/m_bin);
    book1D( toHistoID("Data/SumEt/2")  ,  "Sum(Et,Prev2)"        ,  0. , 16384. * caloEt  , 16384/m_bin);
    book1D( toHistoID("Data/SumEt/3") ,  "Sum(Et,Prev1)"        ,  0. , 16384. * caloEt  , 16384/m_bin);
    book1D( toHistoID("Data/SumEt/4")  ,  "Sum(Et,Next1)"        ,  0. , 16384. * caloEt  , 16384/m_bin);
    book1D( toHistoID("Data/SumEt/5")  ,  "Sum(Et,Next2)"        ,  0. , 16384. * caloEt  , 16384/m_bin);
    //
    book1D( toHistoID("Data/Pus/1")  ,  "PUPeak1(Cont)"   ,  0. , 256.  , 256/m_bin);
    book1D( toHistoID("Data/Pus/2")  ,  "PUPeak2(Cont)"   ,  0. , 256.  , 256/m_bin);
    book1D( toHistoID("Data/Pus/3")  ,  "PUHits(Mult)"    ,  0. , 256.  , 256/m_bin);
    //
    book1D( toHistoID("Data/Muon/1") ,  "Muon Q00  (Pt*sign) "   ,  -128.*muonPt, 128*muonPt , 256/m_bin);
    book1D( toHistoID("Data/Muon/2") ,  "Muon Q01  (Pt*sign) "   ,  -128.*muonPt, 128.*muonPt , 256/m_bin);
    book1D( toHistoID("Data/Muon/3") ,  "Muon Q10  (Pt*sign) "   ,  -128.*muonPt, 128.*muonPt , 256/m_bin);
    book1D( toHistoID("Data/Muon/4") ,  "Muon Q11  (Pt*sign) "   ,  -128.*muonPt, 128.*muonPt , 256/m_bin);
    book1D( toHistoID("Data/Muon/5") ,  "Muon Q20  (Pt*sign) "   ,  -128.*muonPt, 128.*muonPt , 256/m_bin);
    book1D( toHistoID("Data/Muon/6") ,  "Muon Q21  (Pt*sign) "   ,  -128.*muonPt, 128.*muonPt , 256/m_bin);
    book1D( toHistoID("Data/Muon/7") ,  "Muon Q30  (Pt*sign) "   ,  -128.*muonPt, 128.*muonPt , 256/m_bin);
    book1D( toHistoID("Data/Muon/8") ,  "Muon Q31  (Pt*sign) "   ,  -128.*muonPt, 128.*muonPt , 256/m_bin);
  }
  
  if( m_proc ){
    book1D( toHistoID("Process/Muon/1"), "1st highest Muon (Pt*sign) "   ,  -128.*muonPt, 128.*muonPt , 256/m_bin);
    book1D( toHistoID("Process/Muon/2"), "2nd highest Muon (Pt*sign) "   ,  -128.*muonPt, 128.*muonPt , 256/m_bin);
    book1D( toHistoID("Process/Muon/3"), "3rd highest Muon (Pt*sign) "   ,  -128.*muonPt, 128.*muonPt , 256/m_bin);
    book1D( toHistoID("Process/Muon/4"), "DiMuon (Pt) "                  ,  0.      , 256.*muonPt , 256/m_bin);
    book1D( toHistoID("Process/Muon/5"), "Muon Cleaning Pattern "        ,  0.      , 256.   , 256);
  }
  
  if(m_status){     
    book1D( toHistoID("Status/Muon/1") ,  "Muon CU0 status (status & 0xC)" , 0., 4. , 4 );
    book1D( toHistoID("Status/Muon/2") ,  "Muon CU1 status (status & 0xC)" , 0., 4. , 4 );
    book1D( toHistoID("Status/Muon/3") ,  "Muon CU2 status (status & 0xC)" , 0., 4. , 4 ); 
    book1D( toHistoID("Status/Muon/4") ,  "Muon CU3 status (status & 0xC)" , 0., 4. , 4 );
    book1D( toHistoID("Status/Muon/5") ,  "#muons CU0 (status & 0x3)" , 0., 4. , 4 );
    book1D( toHistoID("Status/Muon/6") ,  "#muons CU1 (status & 0x3)" , 0., 4. , 4 );
    book1D( toHistoID("Status/Muon/7") ,  "#muons CU2 (status & 0x3)" , 0., 4. , 4 ); 
    book1D( toHistoID("Status/Muon/8") ,  "#muons CU3 (status & 0x3)" , 0., 4. , 4 );
    
    book1D( toHistoID("Status/Calo/1") ,  "Calo Electron status" , 0., 2. , 2 );
    book1D( toHistoID("Status/Calo/2") ,  "Calo Photon status" , 0., 2. , 2 );
    book1D( toHistoID("Status/Calo/3") ,  "Calo LocalPi0 status" , 0., 2. , 2 );
    book1D( toHistoID("Status/Calo/4") ,  "Calo GlobalPi0  status" , 0., 2. , 2 );
    book1D( toHistoID("Status/Calo/5") ,  "Calo Hadron status" , 0., 2. , 2 );
    book1D( toHistoID("Status/Calo/6") ,  "Calo SpdMult status" , 0., 2. , 2 );
    book1D( toHistoID("Status/Calo/7") ,  "Calo SumEt status" , 0., 2. , 2 );
    book1D( toHistoID("Status/Pus/1") ,  "Pus  word1 status" , 0., 2. , 2 );
    book1D( toHistoID("Status/Pus/2")  ,  "Pus  word2 status" , 0., 2. , 2 );
    book1D( toHistoID("Status/L0DU/BCID/1") ,  "BCID(ODIN)-BCID(L0DU)" , -15. , 15., 31);
    book1D( toHistoID("Status/L0DU/BCID/2") ,  "BCID(PGA3)-BCID(PGA2)" , -15. , 15., 31);
    // Global summary
    AIDA::IHistogram1D* histo = book1D( toHistoID("Status/Summary/1")     ,  "L0DU error status summary"       , -1. , 10., 11);
      TH1D* th1 = Gaudi::Utils::Aida2ROOT::aida2root( histo );        
      TAxis* xAxis = th1->GetXaxis();
      xAxis->SetBinLabel( 1 , "Counter" );
      xAxis->SetBinLabel( 2 , "Tell1 Error" );
      xAxis->SetBinLabel( 3 , "DeMultiplexer Error" );
      xAxis->SetBinLabel( 4 , "TLK Error" );
      xAxis->SetBinLabel( 5 , "Idle Link" );
      xAxis->SetBinLabel( 6 , "PGA  BC MisAlign" );
      xAxis->SetBinLabel( 7 , "ODIN BC MisAlign" );
      xAxis->SetBinLabel( 8 , "Input Data Error" );
      xAxis->SetBinLabel( 9 , "Emulator Check" );
      xAxis->SetBinLabel( 10 , "Unknow TCK" );
      xAxis->SetBinLabel( 11 , "SpdMult Error" );
      xAxis->LabelsOption( m_lab.c_str() );
    // BXID decision
    book1D( toHistoID("Status/BCID/1")        ,  "BCID for positive decisions"     , 0 , 3565, 3565);
  }
  
  if( m_addr ){
    bookCalo2D(toHistoID("Address/Calo/1") , "Electron candidate address" , "Ecal");
    bookCalo2D(toHistoID("Address/Calo/2") , "Photon   candidate address" , "Ecal");
    bookCalo2D(toHistoID("Address/Calo/3") , "LocalPi0 candidate address" , "Ecal");
    bookCalo2D(toHistoID("Address/Calo/4") , "GlobalPi0 candidate address", "Ecal");
    bookCalo2D(toHistoID("Address/Calo/5") , "Hadron candidate address"   , "Hcal");
    //
     //
    book2D("Address/Muon/1" , " Muon Q00 address (PU|PB)vs(M3-add) " , 0. , 32. ,  32 , 0., 128. , 128 );
    book2D("Address/Muon/2" , " Muon Q01 address (PU|PB)vs(M3-add) " , 0. , 32. ,  32 , 0., 128. , 128 );
    book2D("Address/Muon/3" , " Muon Q10 address (PU|PB)vs(M3-add) " , 0. , 32. ,  32 , 0., 128. , 128 );
    book2D("Address/Muon/4" , " Muon Q11 address (PU|PB)vs(M3-add) " , 0. , 32. ,  32 , 0., 128. , 128 );
    book2D("Address/Muon/5" , " Muon Q20 address (PU|PB)vs(M3-add) " , 0. , 32. ,  32 , 0., 128. , 128 );
    book2D("Address/Muon/6" , " Muon Q21 address (PU|PB)vs(M3-add) " , 0. , 32. ,  32 , 0., 128. , 128 );
    book2D("Address/Muon/7" , " Muon Q30 address (PU|PB)vs(M3-add) " , 0. , 32. ,  32 , 0., 128. , 128 );
    book2D("Address/Muon/8" , " Muon Q31 address (PU|PB)vs(M3-add) " , 0. , 32. ,  32 , 0., 128. , 128 );
    //
    book1D( toHistoID("Address/Pus/1") ,  "Pile-up peak1 position" , 0. , 256., 256);
    book1D( toHistoID("Address/Pus/2") ,  "Pile-up peak2 position" , 0. , 256., 256);
    // @to do 2D view for Muon
  }  
                                         }

bool L0DURawBankMonitor::emulatorCheck(LHCb::L0DUConfig* config, int unit, std::string txt) {
  if( NULL == config){
    Warning("L0DUConfig config point to NULL",StatusCode::SUCCESS).ignore();
    return false;
  }
  bool check = true;
  std::stringstream one("");
  std::stringstream two("");
  //one << unit*2-1;
  //two << unit*2;
  one << unit;
  two << unit;
  

  LHCb::L0DUChannel::Map channels = config->channels();
  int cBin = channels.size();
  LHCb::L0DUElementaryCondition::Map conditions = config->conditions();
  int ecBin = conditions.size();
  // Counters
  AIDA::IHistogram1D* h1 = plot1D( -1. ,"Status/L0DU/EmulatorCheck/Channels/" + one.str(),
          "L0DU channels preDecision emulator check (" + txt + ")" ,-1. ,(double) cBin  , cBin+1);
  AIDA::IHistogram1D* h2 = plot1D( -1. ,"Status/L0DU/EmulatorCheck/Conditions/" + two.str(),
          "L0DU conditions value emulator check (" + txt + ")",-1. ,(double) ecBin  , ecBin+1);


  TAxis* ax1=0;
  TAxis* ax2=0;
  if(m_first){
    TH1D* th1 = Gaudi::Utils::Aida2ROOT::aida2root( h1 );        
    TH1D* th2 = Gaudi::Utils::Aida2ROOT::aida2root( h2 );        
    ax1 = th1->GetXaxis();
    ax2 = th2->GetXaxis();
    ax1->SetBinLabel( 1 , "Counter" );
    ax2->SetBinLabel( 1 , "Counter" );
  }
  
  LHCb::L0DUReport report = m_fromRaw->report();


  for(LHCb::L0DUChannel::Map::iterator it = channels.begin();it!=channels.end();it++){
    int id = ((*it).second)->id() ;
    if( report.channelPreDecision( id ) != ((*it).second)->emulate()->emulatedPreDecision() ){
      debug() << "Emulator check error for channel " << (*it).first << endreq;
      plot1D( (double) id ,"Status/L0DU/EmulatorCheck/Channels/" + one.str(),
              "L0DU channels preDecision emulator check (" + txt + ")" ,-1. ,(double) cBin  , cBin+1);
      check = false;
    }
    std::string name = (*it).first;
    if(m_first)ax1->SetBinLabel( id+2 , name.c_str() );
  }
  
  for(LHCb::L0DUElementaryCondition::Map::iterator it = conditions.begin();it!=conditions.end();it++){
    int id = ((*it).second)->id() ;
    if( report.conditionValue( id ) != ((*it).second)->emulatedValue() ){
      debug() << "Emulator check error for condition " << (*it).first << endreq;
      plot1D( (double) id ,"Status/L0DU/EmulatorCheck/Conditions/" + two.str(), 
              "L0DU conditions value emulator check (" + txt + ")",-1. ,(double) ecBin  , ecBin+1);
      check = false;
    }
    std::string name = (*it).first;
    if(m_first)ax2->SetBinLabel( id+2 , name.c_str() );
  }
  if(m_first){
    ax1->LabelsOption( m_lab.c_str() );
    ax2->LabelsOption( m_lab.c_str() );
  }
  return check;
}

     
GaudiAlg::HistoID L0DURawBankMonitor::toHistoID(std::string unit){
  if( m_generic  ) return HistoID( unit );
  int index = unit.find_first_of("/");
  std::string pref = unit.substr(0,index);
  if( unit.find("Calo/1") != std::string::npos )return HistoID( pref + "/Calo/Electron" );
  else if( unit.find("Calo/2")!= std::string::npos)return HistoID( pref + "/Calo/Photon" );
  else if( unit.find("Calo/3")!= std::string::npos)return HistoID( pref + "/Calo/LocalPi0" );
  else if( unit.find("Calo/4")!= std::string::npos)return HistoID( pref + "/Calo/GlobalPi0" );
  else if( unit.find("Calo/5")!= std::string::npos)return HistoID( pref + "/Calo/Hadron" );
  else if( unit.find("Calo/6")!= std::string::npos)return HistoID( pref + "/Calo/SpdMult" );
  else if( unit.find("Calo/7")!= std::string::npos)return HistoID( pref + "/Calo/SumEt" );
  else if( unit.find("Process/Muon/1")!= std::string::npos)return HistoID( pref + "/Muon/Muon1" );
  else if( unit.find("Process/Muon/2")!= std::string::npos)return HistoID( pref + "/Muon/Muon2" );
  else if( unit.find("Process/Muon/3")!= std::string::npos)return HistoID( pref + "/Muon/Muon3" );
  else if( unit.find("Process/Muon/4")!= std::string::npos)return HistoID( pref + "/Muon/DiMuon" );
  else if( unit.find("Process/Muon/5")!= std::string::npos)return HistoID( pref + "/Muon/MuonCleaning" );
  else if( unit.find("Data/Muon/1")!= std::string::npos)return HistoID( pref + "/Muon/Muon00" );
  else if( unit.find("Data/Muon/2")!= std::string::npos)return HistoID( pref + "/Muon/Muon01" );
  else if( unit.find("Data/Muon/3")!= std::string::npos)return HistoID( pref + "/Muon/Muon10" );
  else if( unit.find("Data/Muon/4")!= std::string::npos)return HistoID( pref + "/Muon/Muon11" );
  else if( unit.find("Data/Muon/5")!= std::string::npos)return HistoID( pref + "/Muon/Muon20" );
  else if( unit.find("Data/Muon/6")!= std::string::npos)return HistoID( pref + "/Muon/Muon21" );
  else if( unit.find("Data/Muon/7")!= std::string::npos)return HistoID( pref + "/Muon/Muon30" );
  else if( unit.find("Data/Muon/8")!= std::string::npos)return HistoID( pref + "/Muon/Muon31" );
  else if( unit.find("Status/Muon/1")!= std::string::npos)return HistoID( pref + "/Muon/CU0Status" );
  else if( unit.find("Status/Muon/2")!= std::string::npos)return HistoID( pref + "/Muon/CU1Status" );
  else if( unit.find("Status/Muon/3")!= std::string::npos)return HistoID( pref + "/Muon/CU2Status" );
  else if( unit.find("Status/Muon/4")!= std::string::npos)return HistoID( pref + "/Muon/CU3Status" );
  else if( unit.find("Status/Muon/5")!= std::string::npos)return HistoID( pref + "/Muon/CU0Count" );
  else if( unit.find("Status/Muon/6")!= std::string::npos)return HistoID( pref + "/Muon/CU1Count" );
  else if( unit.find("Status/Muon/7")!= std::string::npos)return HistoID( pref + "/Muon/CU2Count" );
  else if( unit.find("Status/Muon/8")!= std::string::npos)return HistoID( pref + "/Muon/CU3Count" );
  else if( unit.find("Status/Pus/1")!= std::string::npos)return HistoID( pref + "/Pus/Word1" );
  else if( unit.find("Status/Pus/2")!= std::string::npos)return HistoID( pref + "/Pus/Word2" );
  else if( unit.find("Pus/1")!= std::string::npos)return HistoID( pref + "/Pus/Peak1" );
  else if( unit.find("Pus/2")!= std::string::npos)return HistoID( pref + "/Pus/Peak2" );
  else if( unit.find("Pus/3")!= std::string::npos)return HistoID( pref + "/Pus/Hits" );
  return HistoID( unit );
  
} 
     
