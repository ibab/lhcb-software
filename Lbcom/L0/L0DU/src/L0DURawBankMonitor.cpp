// $Id: L0DURawBankMonitor.cpp,v 1.1 2008-03-27 16:32:13 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
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
  declareProperty( "DataReBinFactor"   , m_bin = 1);
  declareProperty( "LocationsForEmulatorCheck"   , m_locs);
    

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

  // get the tools
  m_fromRaw  = tool<IL0DUFromRawTool>( m_fromRawTool , m_fromRawTool , this );
  m_condDB   = tool<IL0CondDBProvider>("L0CondDBProvider");
  m_odin     = tool<IEventTimeDecoder>("OdinTimeDecoder","OdinDecoder",this);
  m_emuTool  = tool<IL0DUEmulatorTool>(m_emulatorTool, m_emulatorTool,this);
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
    Warning( "Emtpy location for ODIN '"+ LHCb::ODINLocation::Default +"'" ).ignore();
  }
  


  if(!m_fromRaw->decodeBank()){
    Error("Unable to decode L0DU rawBank", StatusCode::SUCCESS).ignore();
    return StatusCode::SUCCESS;
  } 

  m_report = m_fromRaw->report();
  
  double Sgn[2];
  Sgn[0]=+1.;
  Sgn[1]=-1.;
  
  // Input Data monitoring
  if(m_data){
    // Calo
    fill( histo1D(HistoID("Data/Calo/1")),m_fromRaw->data("Electron(Et)") ,1  );
    fill( histo1D(HistoID("Data/Calo/2")),m_fromRaw->data("Photon(Et)")   ,1  );
    fill( histo1D(HistoID("Data/Calo/3")),m_fromRaw->data("LocalPi0(Et)") ,1  );
    fill( histo1D(HistoID("Data/Calo/4")),m_fromRaw->data("GlobalPi0(Et)"),1  );
    fill( histo1D(HistoID("Data/Calo/5")),m_fromRaw->data("Hadron(Et)")   ,1  );
    fill( histo1D(HistoID("Data/Calo/6")),m_fromRaw->data("Spd(Mult)")    ,1  );
    fill( histo1D(HistoID("Data/SumEt/1")),m_fromRaw->data("Sum(Et)")      ,1  );
    fill( histo1D(HistoID("Data/SumEt/2")),m_fromRaw->data("Sum(Et,Prev2)")      ,1  );
    fill( histo1D(HistoID("Data/SumEt/3")),m_fromRaw->data("Sum(Et,Prev1)")      ,1  );
    fill( histo1D(HistoID("Data/SumEt/4")),m_fromRaw->data("Sum(Et,Next1)")      ,1  );
    fill( histo1D(HistoID("Data/SumEt/5")),m_fromRaw->data("Sum(Et,Next2)")      ,1  );
    // Pile-Up
    fill( histo1D(HistoID("Data/Pus/1")),m_fromRaw->data("PUPeak1(Cont)") ,1);
    fill( histo1D(HistoID("Data/Pus/2")),m_fromRaw->data("PUPeak2(Cont)") ,1);
    // Muon
    fill( histo1D(HistoID("Data/Muon/1")),m_fromRaw->data("M0(Pt)")  * Sgn[m_fromRaw->data("M0(Sgn)")] ,1);
    fill( histo1D(HistoID("Data/Muon/2")),m_fromRaw->data("M1(Pt)")  * Sgn[m_fromRaw->data("M1(Sgn)")] ,1);
    fill( histo1D(HistoID("Data/Muon/3")),m_fromRaw->data("M2(Pt)")  * Sgn[m_fromRaw->data("M2(Sgn)")] ,1);
    fill( histo1D(HistoID("Data/Muon/4")),m_fromRaw->data("M3(Pt)")  * Sgn[m_fromRaw->data("M3(Sgn)")] ,1);
    fill( histo1D(HistoID("Data/Muon/5")),m_fromRaw->data("M4(Pt)")  * Sgn[m_fromRaw->data("M4(Sgn)")] ,1);
    fill( histo1D(HistoID("Data/Muon/6")),m_fromRaw->data("M5(Pt)")  * Sgn[m_fromRaw->data("M5(Sgn)")] ,1);
    fill( histo1D(HistoID("Data/Muon/7")),m_fromRaw->data("M6(Pt)")  * Sgn[m_fromRaw->data("M6(Sgn)")] ,1);
    fill( histo1D(HistoID("Data/Muon/8")),m_fromRaw->data("M7(Pt)")  * Sgn[m_fromRaw->data("M7(Sgn)")] ,1);
  }


  // data address
  if( m_addr ){
    // Calo
    fillCalo2D("Address/Calo/1" , LHCb::CaloCellID(m_fromRaw->data("Electron(Add)")) , 1.);
    fillCalo2D("Address/Calo/2" , LHCb::CaloCellID(m_fromRaw->data("Photon(Add)"))   , 1.);
    fillCalo2D("Address/Calo/3" , LHCb::CaloCellID(m_fromRaw->data("LocalPi0(Add)")) , 1.);
    fillCalo2D("Address/Calo/4" , LHCb::CaloCellID(m_fromRaw->data("GlobalPi0(Add)")), 1.);
    fillCalo2D("Address/Calo/5" , LHCb::CaloCellID(m_fromRaw->data("Hadron(Add)"))   , 1.);
    // PUs
    fill(histo1D(HistoID("Address/Pus/1"))  , m_fromRaw->data("PUPeak1(Add)"), 1.);
    fill(histo1D(HistoID("Address/Pus/2"))  , m_fromRaw->data("PUPeak2(Add)"), 1.);
    // Muon @ToDo
  }  


  // L0DU Processed Data
  if( m_proc ){
    fill( histo1D(HistoID("Process/Muon/1")), m_fromRaw->data("Muon1(Pt)")* Sgn[m_fromRaw->data("Muon1(Sgn)")] ,1 );
    fill( histo1D(HistoID("Process/Muon/2")), m_fromRaw->data("Muon2(Pt)")* Sgn[m_fromRaw->data("Muon2(Sgn)")] ,1 );
    fill( histo1D(HistoID("Process/Muon/3")), m_fromRaw->data("Muon3(Pt)")* Sgn[m_fromRaw->data("Muon3(Sgn)")] ,1 );
    fill( histo1D(HistoID("Process/Muon/4")), m_fromRaw->data("DiMuon(Pt)"),1);
    fill( histo1D(HistoID("Process/Muon/5")), m_fromRaw->muonCleaningPattern(),1);

  }

  // Status
  if(m_status){     
    // from L0Processor
    fill( histo1D(HistoID("Status/Muon/1")), (m_fromRaw->data("MuonCU0(Status)") & 0xC)>>2 , 1 );
    fill( histo1D(HistoID("Status/Muon/2")), (m_fromRaw->data("MuonCU1(Status)") & 0xC)>>2 , 1 );
    fill( histo1D(HistoID("Status/Muon/3")), (m_fromRaw->data("MuonCU2(Status)") & 0xC)>>2 , 1 );
    fill( histo1D(HistoID("Status/Muon/4")), (m_fromRaw->data("MuonCU3(Status)") & 0xC)>>2 , 1 );
    fill( histo1D(HistoID("Status/Muon/5")), (m_fromRaw->data("MuonCU0(Status)") & 0x3) , 1 );
    fill( histo1D(HistoID("Status/Muon/6")), (m_fromRaw->data("MuonCU1(Status)") & 0x3) , 1 );
    fill( histo1D(HistoID("Status/Muon/7")), (m_fromRaw->data("MuonCU2(Status)") & 0x3) , 1 );
    fill( histo1D(HistoID("Status/Muon/8")), (m_fromRaw->data("MuonCU3(Status)") & 0x3) , 1 );
    fill( histo1D(HistoID("Status/Calo/1")), m_fromRaw->data("Electron(Status)") , 1 );
    fill( histo1D(HistoID("Status/Calo/2")), m_fromRaw->data("Photon(Status)") , 1 );
    fill( histo1D(HistoID("Status/Calo/3")), m_fromRaw->data("LocalPi0(Status)") , 1 );
    fill( histo1D(HistoID("Status/Calo/4")), m_fromRaw->data("GlobalPi0(Status)") , 1 );
    fill( histo1D(HistoID("Status/Calo/5")), m_fromRaw->data("Hadron(Status)") , 1 );
    fill( histo1D(HistoID("Status/Calo/6")), m_fromRaw->data("Sum(Status)") , 1 );
    fill( histo1D(HistoID("Status/Calo/7")), m_fromRaw->data("Spd(Status)") , 1 );
    fill( histo1D(HistoID("Status/Pus/1")), m_fromRaw->data("PU1(Status)") , 1 );
    fill( histo1D(HistoID("Status/Pus/2")), m_fromRaw->data("PU2(Status)") , 1 );
    
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
    fill( histo1D(HistoID("Status/L0DU/BCID/1")), odBX - m_fromRaw->bcid().first, 1 );
    fill( histo1D(HistoID("Status/L0DU/BCID/2")), (0x7F & m_fromRaw->bcid().first) - m_fromRaw->bcid().second , 1 );



    // L0DU Emulator check
    bool check = true;
    LHCb::L0DUConfig* config = m_report.configuration();
    if( NULL == config){
      std::stringstream ttck("");
      ttck << format("0x%04X", m_report.tck() ) ;
      Warning("L0DUConfig for tck = " + ttck.str() + " has not been registered -> cannot perform the emulator check").ignore();
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
          Warning("Cannot run the emulator for L0ProcessorData location(s) : " + loc.str() ).ignore();
        }        
      }
      // reset processor data from L0DU location when changed
      if(m_locs.size() != 0){
        config->decrementCounters(); // replay the same event with different input data
        m_emuTool->process(config , m_fromRaw->L0ProcessorDatas());
      }
    }    
    
    // L0DU Error summary
    fill( histo1D(HistoID("Status/Summary/1")), -1. , 1 ); // counter
    if(0 < inputStatus){
      fill( histo1D(HistoID("Status/Summary/1")), L0DUBase::L0DUError::InputData  , 1 );
      Warning("L0DU bank monitor summary : -- input data error bit -- ",StatusCode::SUCCESS).ignore();
    }
    if( (0x7F & m_fromRaw->bcid().first) != m_fromRaw->bcid().second){
      fill( histo1D(HistoID("Status/Summary/1")), L0DUBase::L0DUError::BxPGAShift , 1 );
      Warning("L0DU bank monitor summary : -- PGA2/3 BXID misaligned -- ",StatusCode::SUCCESS).ignore();
    }

    if( odBX != m_fromRaw->bcid().first){
      fill( histo1D(HistoID("Status/Summary/1")), L0DUBase::L0DUError::BxOdinShift , 1 );
      Warning("L0DU bank monitor summary : -- ODIN/L0DU BXID misaligned -- ",StatusCode::SUCCESS).ignore();
    }
    if( (m_fromRaw->status() & 0x1) ){
      fill( histo1D(HistoID("Status/Summary/1")), L0DUBase::L0DUError::Tell1 , 1 );
      Warning("L0DU bank monitor summary : -- TELL1 error bit -- ",StatusCode::SUCCESS).ignore();
    } 
    if( (m_fromRaw->status() & 0x2) ){
      fill( histo1D(HistoID("Status/Summary/1")), L0DUBase::L0DUError::DeMux , 1 );
      Warning("L0DU bank monitor summary : -- DeMultiplexer error bit -- ",StatusCode::SUCCESS).ignore();
    }
    if( (m_fromRaw->status() & 0x4) ){
      fill( histo1D(HistoID("Status/Summary/1")), L0DUBase::L0DUError::TLK   , 1 );
      Warning("L0DU bank monitor summary : -- TLK error bit -- ",StatusCode::SUCCESS).ignore();
    }
    
    if( (m_fromRaw->status() & 0x8) ){
      fill( histo1D(HistoID("Status/Summary/1")), L0DUBase::L0DUError::IdleLink , 1 );
      Warning("L0DU bank monitor summary : -- Idle link error bit -- ",StatusCode::SUCCESS).ignore();
    }
    if( !check ){
      fill( histo1D(HistoID("Status/Summary/1")), L0DUBase::L0DUError::EmulatorCheck , 1 );
      Warning("L0DU bank monitor summary : -- Emulator check error -- ",StatusCode::SUCCESS).ignore();

    }    
  }


  



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

    book1D( "Data/Calo/1"  ,  "Electron(Et)"   ,  0. , 256. * caloEt   ,  256/m_bin);
    book1D( "Data/Calo/2"  ,  "Photon(Et)"     ,  0. , 256. * caloEt    , 256/m_bin);
    book1D( "Data/Calo/3"  ,  "LocalPi0(Et)"   ,  0. , 256. * caloEt    , 256/m_bin);
    book1D( "Data/Calo/4"  ,  "GlobalPi0(Et)"  ,  0. , 256. * caloEt    , 256/m_bin);
    book1D( "Data/Calo/5"  ,  "Hadron(Et)"     ,  0. , 256. * caloEt    , 256/m_bin);
    book1D( "Data/Calo/6"  ,  "Spd(Mult)"      ,  0. , 16384.           , 16384/m_bin);

    book1D( "Data/SumEt/1"  ,  "Sum(Et,T0)"           ,  0. , 16384. * caloEt  , 16384/m_bin);
    book1D( "Data/SumEt/2"  ,  "Sum(Et,Prev2)"        ,  0. , 16384. * caloEt  , 16384/m_bin);
    book1D( "Data/SumEt/3"  ,  "Sum(Et,Prev1)"        ,  0. , 16384. * caloEt  , 16384/m_bin);
    book1D( "Data/SumEt/4"  ,  "Sum(Et,Next1)"        ,  0. , 16384. * caloEt  , 16384/m_bin);
    book1D( "Data/SumEt/5"  ,  "Sum(Et,Next2)"        ,  0. , 16384. * caloEt  , 16384/m_bin);
    //
    book1D( "Data/Pus/1"  ,  "PUPeak1(Cont)"   ,  0. , 256.  , 256/m_bin);
    book1D( "Data/Pus/2"  ,  "PUPeak2(Cont)"   ,  0. , 256.  , 256/m_bin);
    //
    book1D( "Data/Muon/1" ,  "Muon Q00  (Pt)  "   ,  -128.*muonPt, 128*muonPt , 256/m_bin);
    book1D( "Data/Muon/2" ,  "Muon Q01  (Pt)     "   ,  -128.*muonPt, 128.*muonPt , 256/m_bin);
    book1D( "Data/Muon/3" ,  "Muon Q10  (Pt)     "   ,  -128.*muonPt, 128.*muonPt , 256/m_bin);
    book1D( "Data/Muon/4" ,  "Muon Q11  (Pt)     "   ,  -128.*muonPt, 128.*muonPt , 256/m_bin);
    book1D( "Data/Muon/5" ,  "Muon Q20  (Pt)     "   ,  -128.*muonPt, 128.*muonPt , 256/m_bin);
    book1D( "Data/Muon/6" ,  "Muon Q21  (Pt)     "   ,  -128.*muonPt, 128.*muonPt , 256/m_bin);
    book1D( "Data/Muon/7" ,  "Muon Q30  (Pt)     "   ,  -128.*muonPt, 128.*muonPt , 256/m_bin);
    book1D( "Data/Muon/8" ,  "Muon Q31  (Pt)     "   ,  -128.*muonPt, 128.*muonPt , 256/m_bin);
  }



   if( m_proc ){
     book1D( "Process/Muon/1", "1st highest Muon(Pt) "   ,  -128.*muonPt, 128.*muonPt , 256/m_bin);
     book1D( "Process/Muon/2", "2nd highest Muon(Pt) "   ,  -128.*muonPt, 128.*muonPt , 256/m_bin);
     book1D( "Process/Muon/3", "3rd highest Muon3(Pt) "  ,  -128.*muonPt, 128.*muonPt , 256/m_bin);
     book1D( "Process/Muon/4", "DiMuon(Pt) "  ,  0.      , 256.*muonPt , 256/m_bin);
     book1D( "Process/Muon/5", "Muon Cleaning Pattern "  ,  0.      , 256.   , 256);
   }
  



   if(m_status){     
     book1D( "Status/Muon/1" ,  "Muon CU0 status (status & 0xC)" , 0., 4. , 4 );
     book1D( "Status/Muon/2" ,  "Muon CU1 status (status & 0xC)" , 0., 4. , 4 );
     book1D( "Status/Muon/3" ,  "Muon CU2 status (status & 0xC)" , 0., 4. , 4 ); 
     book1D( "Status/Muon/4" ,  "Muon CU3 status (status & 0xC)" , 0., 4. , 4 );
     book1D( "Status/Muon/5" ,  "#muons CU0 (status & 0x3)" , 0., 4. , 4 );
     book1D( "Status/Muon/6" ,  "#muons CU1 (status & 0x3)" , 0., 4. , 4 );
     book1D( "Status/Muon/7" ,  "#muons CU2 (status & 0x3)" , 0., 4. , 4 ); 
     book1D( "Status/Muon/8" ,  "#muons CU3 (status & 0x3)" , 0., 4. , 4 );

     book1D( "Status/Calo/1" ,  "Calo Electron status" , 0., 2. , 2 );
     book1D( "Status/Calo/2" ,  "Calo Photon status" , 0., 2. , 2 );
     book1D( "Status/Calo/3" ,  "Calo LocalPi0 status" , 0., 2. , 2 );
     book1D( "Status/Calo/4" ,  "Calo GlobalPi0  status" , 0., 2. , 2 );
     book1D( "Status/Calo/5" ,  "Calo Hadron status" , 0., 2. , 2 );
     book1D( "Status/Calo/6" ,  "Calo SumEt status" , 0., 2. , 2 );
     book1D( "Status/Calo/7" ,  "Calo SpdMult status" , 0., 2. , 2 );
     book1D( "Status/Pus/1"  ,  "Pus  word1 status" , 0., 2. , 2 );
     book1D( "Status/Pus/2"  ,  "Pus  word2 status" , 0., 2. , 2 );
     book1D( "Status/L0DU/BCID/1" ,  "BCID(ODIN)-BCID(L0DU)" , -15. , 15., 31);
     book1D( "Status/L0DU/BCID/2" ,  "BCID(PGA3)-BCID(PGA2)" , -15. , 15., 31);
     // Global summary
     book1D( "Status/Summary/1"     ,  "L0DU error status summary"       , -1. , 10., 11);
   }

   if( m_addr ){
     bookCalo2D("Address/Calo/1" , "Electron candidate address" , "Ecal");
     bookCalo2D("Address/Calo/2" , "Photon   candidate address" , "Ecal");
     bookCalo2D("Address/Calo/3" , "LocalPi0 candidate address" , "Ecal");
     bookCalo2D("Address/Calo/4" , "GlobalPi0 candidate address", "Ecal");
     bookCalo2D("Address/Calo/5" , "Hadron candidate address"   , "Hcal");
     //
     book1D( "Address/Pus/1" ,  "Pile-up peak1 position" , 0. , 256., 256);
     book1D( "Address/Pus/2" ,  "Pile-up peak2 position" , 0. , 256., 256);
     // @to do 2D view for Muon
     

   }  
}



bool L0DURawBankMonitor::emulatorCheck(LHCb::L0DUConfig* config, int unit, std::string txt) {
  if( NULL == config){
    Warning("L0DUConfig config point to NULL").ignore();
    return false;
  }
  bool check = true;
  std::stringstream one("");
  one << unit*2-1;
  std::stringstream two("");
  two << unit*2;
  

  LHCb::L0DUChannel::Map channels = config->channels();
  int cBin = channels.size();
  LHCb::L0DUElementaryCondition::Map conditions = config->conditions();
  int ecBin = conditions.size();
  // Counters
  plot1D( -1. ,"Status/L0DU/EmulatorCheck/" + one.str(),
          "L0DU triggered channels emulator check (" + txt + ")" ,-1. ,(double) cBin  , cBin+1);
  plot1D( -1. ,"Status/L0DU/EmulatorCheck/" + two.str(),
          "L0DU conditions value emulator check (" + txt + ")",-1. ,(double) ecBin  , ecBin+1);


  for(LHCb::L0DUChannel::Map::iterator it = channels.begin();it!=channels.end();it++){
    int id = ((*it).second)->id() ;
    if( m_report.triggeredChannel( id ) != ((*it).second)->emulate()->emulatedDecision() ){
      plot1D( (double) id ,"Status/L0DU/EmulatorCheck/" + one.str(),
              "L0DU triggered channels emulator check (" + txt + ")" ,-1. ,(double) cBin  , cBin+1);
      check = false;
    }
  }
  
  for(LHCb::L0DUElementaryCondition::Map::iterator it = conditions.begin();it!=conditions.end();it++){
    int id = ((*it).second)->id() ;
    if( m_report.conditionValue( id ) != ((*it).second)->emulatedValue() ){
      plot1D( (double) id ,"Status/L0DU/EmulatorCheck/" + two.str(), 
              "L0DU conditions value emulator check (" + txt + ")",-1. ,(double) ecBin  , ecBin+1);
      check = false;
    }
  }
  return check;
}
