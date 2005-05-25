//#include <time.h>
#include "L0MuonKernel/ProcUnit.h"
#include "L0MuonKernel/CrateUnit.h"
#include "L0mConf/L0MFoi.h"
#include "L0MuonKernel/Utilities.h"

#define UNKNOWN "Unknown"

/**
   Empty Constructor
*/
L0Muon::ProcUnit::ProcUnit() {  
}

/**
   Constructor
*/
L0Muon::ProcUnit::ProcUnit(L0MPuNodeBase & puNode) {

  //=============================================
  //=                                           =
  //=   Default values for variables            =
  //=                                           =
  //=============================================

  m_writeL0Buffer = false;
  m_buildL0Buffer = true;


  //=============================================
  //=                                           =
  //=   Set configuration dependant variables   =
  //=                                           =
  //=============================================

  m_maxFoi = puNode.fieldOfInterest();
  m_pu = puNode.id();
  m_boardID =puNode.board();
  setName(puNode.name());
  
  //=============================================
  //=                                           =
  //=         Build the Units structure         =
  //=                                           =
  //=============================================

  if ( ! m_units.empty() ) {
    m_units.clear();
  }

  // Core Unit
  CoreUnit    * m_cu = new CoreUnit();
  addUnit(m_cu);
  m_cu->setParent(this);
  m_cu->setPU(puNode.id());

  // Formatting Unit
  FormattingUnit * m_formatting = new FormattingUnit();
  addUnit(m_formatting);
  m_formatting->setParent(this);

  // L0Buffer Unit
  int type=0;
  L0BufferUnit   * m_l0b = new L0BufferUnit(type); 
  addUnit(m_l0b,"l0buf");
  m_l0b->setParent(this); 
  m_l0b->setPU(puNode.id());

  //=============================================
  //=                                           =
  //=         Build the TileRegisters           =
  //=                                           =
  //=============================================

  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();

  //
  //======== Optical links  ======================
  //
  // Loop over stations
  m_ol3[0] = NULL;
  m_ol3[1] = NULL;
  for ( int sta = 0; sta < 5; sta++) {  
    // Loop over the optical links of this station
    int olcounter = 0;
    std::vector<L0MTile> ols = puNode.opticalLinks(sta);
    for (std::vector<L0MTile>::iterator  il = ols.begin(); il != ols.end(); il++ ) {

      // Get list of tiles
      std::vector<L0MTile> l0mtilelist = il->tiles();

      // If tiles are transfered
      int nbits = l0mtilelist.size();
      if (nbits >0 ){

	// TileRegister name
	char buf_name[4096];
	char* format_name = "OL_%d_Q%d_%s_%d";
	sprintf(buf_name,format_name,sta,(*il).quarter(),puNode.name().c_str(),olcounter);
	if (m_debug) std::cout << "ProcUnit: name for Register"  <<" " << buf_name <<  std::endl;

	// TileRegister alias
	char buf_alias[4096];
	char* format_alias = "OL_%d_(Q%d,R%d,%d,%d)";               
	sprintf(buf_alias,format_alias,sta,(*il).quarter(),(*il).region(), (*il).nX(), (*il).nY());
	if (m_debug) std::cout << "ProcUnit: alias for Register" <<" " << buf_alias <<  std::endl;

	// Create the TileRegister
	TileRegister* reg = rfactory->createTileRegister(buf_name,nbits);
	rfactory->createAlias(buf_name,buf_alias);
	reg->setType("OpticalLink");
	L0Muon::splitTileListInfo(l0mtilelist,reg);

        // Store the OL in station 3
	if ( sta == 2 ) m_ol3[olcounter] = reg;

	// Register the tileregister
	addInputRegister(reg);
	m_cu->addInputRegister(reg); 
	m_formatting->addInputRegister(reg);
	m_l0b->addInputRegister(reg);

      }// End if tiles transfered
      olcounter++;
    }  // End of loop over optical links
  } // End of loop over stations
  
  //======== End Optical links  ==================

  //
  //======== Neighbours  =========================
  //
  
  // Loop over functions
  for (int ifunc = 0; ifunc<4; ifunc++){

    // Get the function name
    enum FunctionList {Horizontal, Vertical, Crossing, Backplane};
    FunctionList func = FunctionList(ifunc);
    char * namefunc=0;
    if (func == Horizontal) namefunc = "Horizontal";
    if (func == Vertical)   namefunc = "Vertical";
    if (func == Crossing)   namefunc = "Crossing";
    if (func == Backplane)  namefunc = "Backplane";
    char function[4096];
    sprintf(function,"%s",namefunc);

    std::vector<MuonTileID> lpus;
  
    // ---  Inputs  -------------------
    lpus = puNode.pus(function,"From");
      
    // Loop over pus connected with the current function
    for (std::vector<MuonTileID>::iterator ip= lpus.begin(); ip!= lpus.end(); ip++){
      // Loop over stations
      for (int sta =0; sta<5; sta++){
	// Get list of tiles
        std::vector<L0MTile> l0mtilelist= puNode.tiles(function, "From",*ip, sta);

	// If tiles are transfered
        int nbits = l0mtilelist.size();
        if (nbits >0 ){

	  // TileRegister name
	  char* formatIn = "%s_sta%d_Q%d_(R%d,%d,%d)_%s";
	  char buf[4096];
	  sprintf(buf,formatIn,
		  namefunc, 
		  sta,
		  m_pu.quarter(),
		  (*ip).region(),(*ip).nX(),(*ip).nY(), 
		  puNode.name().c_str());
          if (m_debug) std::cout << "name for Register" <<" " << buf <<  std::endl;
           
          
	  // Create the TileRegister
          TileRegister* reg= rfactory->createTileRegister(buf,nbits);
          reg->setType("Neighbours");
	  L0Muon::splitTileListInfo(l0mtilelist,reg);
	
	  // Register the tileregister
          addInputRegister(reg);	
	  m_cu->addInputRegister(reg);
	  m_l0b->addInputRegister(reg);
          
        } // End if tiles transfered        
      } // End of loop over stations
    } //  End of loop over pus
    
    // ---  End Inputs  ----------------
  
    
    // ---  Outputs  -------------------
    lpus= puNode.pus(function,"To");
       
    // Loop over pus connected with the current function
    for (std::vector<MuonTileID>::iterator ip= lpus.begin(); ip!= lpus.end(); ip++){
      // Loop over stations
      for (int sta =0; sta<5; sta++){
	// Get list of tiles
        std::vector<L0MTile> l0mtilelist= puNode.tiles(function, "To", *ip, sta);
 	
	// If tiles are transfered
        int nbits = l0mtilelist.size();
        if (nbits >0 ){
	  
	  // TileRegister name
          char* format = "%s_sta%d_Q%d_%s_(R%d,%d,%d)";
	  char buf[4096];
          sprintf(buf,format, 
                  namefunc, 
                  sta, 
                  m_pu.quarter(),
                  puNode.name().c_str(),
                  (*ip).region(),(*ip).nX(),(*ip).nY());             
          if (m_debug) std::cout << "name for Register" <<" " << buf <<  std::endl;
         
	  // Create the TileRegister
          TileRegister* reg= rfactory->createTileRegister(buf,nbits);
          reg->setType("Neighbours");
	  L0Muon::splitTileListInfo(l0mtilelist,reg);

	  // Register the tileregister
          addOutputRegister(reg);	  
	  m_formatting->addOutputRegister(reg);
      
        } // End if tiles transfered        
      } // End of loop over stations
    } // End of loop over pus

    // ---  End Outputs  ---------------
    
  } // End of Loop over functions

  //======= End neighbours ========================

  //======= L0Buffers registers ====================
  
  // --- Input fields  ---------------

  // Get list of tiles
  std::vector<L0MTile> l0mtilelist = l0bufferTileList(puNode);

  // If tiles are transfered
  int nbits = l0mtilelist.size();
  if (nbits >0 ){
    // Create the Tile register for the L0Buffer
    char buf[4096];
    char* format ;
    format = "BUF_Q%d_%s";
    sprintf(buf,format,puNode.id().quarter(),puNode.name().c_str());
    if (m_debug) std::cout << "name for Register" <<" " << buf <<  std::endl;

    // Create the TileRegister
    TileRegister* reg = rfactory->createTileRegister(buf,nbits);
    reg->setType("L0Buffer");
    L0Muon::splitTileListInfo(l0mtilelist,reg);

    // Register the tileregister
    addInputRegister(reg);
    m_l0b->addOutputRegister(reg,"L0bOut");
  }

  // // //   // --- Output fields  ---------------

  // // //   for (int icand= 0; icand<2; icand++){
  // // //     int nbits = 0;
  // // //     // Prepare registers with addresser for candidates
  // // //     char * name = "(R%d,%d,%d)_addr_candidate%d";
  // // //     char buf[4096];     
  // // //     sprintf(buf,name,m_pu.region(), m_pu.nX(), m_pu.nY(),icand+1);
  // // //     TileRegister* pReg= rfactory->createTileRegister(buf,nbits);
  // // //     pReg->setType("Outputfield");     
  // // //     char * format = "cand%d";      
  // // //     sprintf(buf,name,icand);      
  // // //     m_l0b->addInputRegister(reg,buf);
  // // //   }

  

}

/**
   Return the list of tiles in the l0buffer
   SHOULD BE CHANGED TO BE SET ACCORDING TO THE DATABASE
*/
std::vector<L0MTile> L0Muon::ProcUnit::l0bufferTileList(L0MPuNodeBase & puNode){

  // Prepare the vector of tiles to be put in the L0Buffer
  MuonTileID empty_tile = MuonTileID();
  std::vector<L0MTile> l0mtilelist(80,empty_tile);
  std::vector<L0MTile>::iterator itl0mtilelist;
  //- Optical links  
  // Loop over stations
  for ( int sta = 0; sta < 5; sta++) {
    std::vector<L0MTile> ols = puNode.opticalLinks(sta);
    // Loop over optical links of this station
    for (std::vector<L0MTile>::iterator  il = ols.begin(); il != ols.end(); il++ ) {
      std::vector<L0MTile> ltiles = il->tiles();
      l0mtilelist.insert(l0mtilelist.end(), ltiles.begin(), ltiles.end());
      std::vector<L0MTile> emptyList(32-ltiles.size(),empty_tile);
      l0mtilelist.insert(l0mtilelist.end(), emptyList.begin(), emptyList.end());
    } // optical links of this station
  } // stations

    //- Inputs Neighbours 
    // Initialize the position in the L0Buffer vector of tiles for neighbour exchanges
  itl0mtilelist = l0mtilelist.begin();  
  itl0mtilelist+=272;
  // Loop over functions
  for (int ifunc = 0; ifunc<4; ifunc++){

    // Get the function name
    enum FunctionList {Horizontal, Vertical, Crossing, Backplane};
    FunctionList func = FunctionList(ifunc);
    char * namefunc=0;
    if (func == Horizontal) namefunc = "Horizontal";
    if (func == Vertical)   namefunc = "Vertical";
    if (func == Crossing)   namefunc = "Crossing";
    if (func == Backplane)  namefunc = "Backplane";
    char function[4096];
    sprintf(function,"%s",namefunc);

    std::vector<MuonTileID> lpus = puNode.pus(function,"From");      
    // Loop over pus connected with the current function
    for (std::vector<MuonTileID>::iterator ip= lpus.begin(); ip!= lpus.end(); ip++){
      // Loop over stations
      for (int sta =0; sta<5; sta++){
	// Get list of tiles
	std::vector<L0MTile> ltiles= puNode.tiles(function, "From",*ip, sta);
	// Fill the L0Buffer vector of tiles with neigbours
	l0mtilelist.insert(itl0mtilelist, ltiles.begin(), ltiles.end());
	itl0mtilelist=l0mtilelist.end();
      } // End of loop over stations
    } // End of Loop over pus
  } // End of Loop over functions
    // Complete the L0Buffer vector of tiles
  if (l0mtilelist.size()<512) {
    std::vector<L0MTile> emptyList(512-l0mtilelist.size(),empty_tile);
    l0mtilelist.insert(l0mtilelist.end(), emptyList.begin(), emptyList.end());
  }
  return l0mtilelist;
}

/**
   Set the registers for the l0buffer used in the testbench (temporarly)
*/
std::map<std::string, std::string>  L0Muon::ProcUnit::setPLLRegisters(MuonTileID puid, std::string l0bufferMapPath){
  //
  // Set the tile register used for the output of the L0buffer specific to V2-TestBench.
  //

  std::map<std::string, std::string> registerNames;
  registerNames["l0buffer"] = UNKNOWN;
  registerNames["OL"]       = UNKNOWN;
  if (l0bufferMapPath==UNKNOWN) return registerNames;

  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();
  // 
  // --- PLL stuff: l0buffer specific to V2-TestBench
  //
  // The tile content of these registers are described in external files.
  //
  // // //   std::cout << "ProcUnit::setPLLRegisters name():" << name() << std::endl;
  std::vector<L0MTile> l0mtilelist;
      
  // PLL L0Buffer:
  // Read the map describing the content of the PLL L0Buffer
  // If no map is found, the L0Buffer for this PU will not be activated.
  l0mtilelist=tileListFromMap(puid,0,l0bufferMapPath);
  // // //   std::cout << "ProcUnit::setPLLRegisters [buf] l0mtilelist=" << l0mtilelist.size() << std::endl;
  if (l0mtilelist.size()>0) {

    // Create the Tile register for the PLL L0Buffer
    char buf[4096];
    char* format ;
    format = "PLLBUF_Q%d_%s";
    sprintf(buf,format,puid.quarter(),name().c_str());
    TileRegister* reg = rfactory->createTileRegister(buf,l0mtilelist.size());
    registerNames["l0buffer"] = buf;

    // Set the type
    reg->setType("L0Buffer");
    L0Muon::splitTileListInfo(l0mtilelist,reg);
      
    std::vector<MuonTileID> mids = reg->getTileVector();
    // // //     std::cout << "ProcUnit::setPLLRegisters [buf] mids.size()=" << mids.size() << std::endl;

  } // End PLL L0Buffer

  // PLL OL:
  // Read the map describing the content of the PLL OL
  // If no map is found, the L0Buffer for this PU will not be activated.
  l0mtilelist=tileListFromMap(puid,1,l0bufferMapPath);
  // // //   std::cout << "ProcUnit::setPLLRegisters [OL] l0mtilelist=" << l0mtilelist.size() << std::endl;
  if (l0mtilelist.size()>0) {

    // Create the Tile register for the PLL OL
    char buf[4096];
    char* format ;
    format = "PLLOL_Q%d_%s";
    sprintf(buf,format,puid.quarter(),name().c_str());
    TileRegister* reg = rfactory->createTileRegister(buf,l0mtilelist.size());
    registerNames["OL"] = buf;
      
    // Set the type
    reg->setType("OLBuffer");
    L0Muon::splitTileListInfo(l0mtilelist,reg);

    std::vector<MuonTileID> mids = reg->getTileVector();
    // // //     std::cout << "ProcUnit::setPLLRegisters [OL] mids.size()=" << mids.size() << std::endl;
            
  } // End PLL OL

  return registerNames;

}

/**
   Initialize
*/
void L0Muon::ProcUnit::initialize() {
  CrateUnit * pcrate = dynamic_cast<CrateUnit *>( parentByType("CrateUnit"));

  if ( pcrate->getProperty("WriteL0Buffer") == "True")  m_writeL0Buffer = true;
  if ( pcrate->getProperty("BuildL0Buffer") == "False") m_buildL0Buffer = false;


  // Construct the PLL L0Buffer (V2-TestBench)
  bool l0bufferPLLFlag    = true;

  if (m_debug) std::cout << "ProcUnit::initialize " 
			 << "m_writeL0Buffer " << m_writeL0Buffer << " " 
			 << "m_buildL0Buffer " << m_buildL0Buffer << " " 
			 << "l0bufferPLLFlag " << l0bufferPLLFlag <<  std::endl;
  
  if ( m_buildL0Buffer ) {

    // Create a L0BufferUnit for the PLL buffer
    int type=1;
    L0BufferUnit   * m_l0bpll = new L0BufferUnit(type); 
    addUnit(m_l0bpll, "l0bufpll");
    m_l0bpll->setParent(this);
    m_l0bpll->setName("PLLBuf");
    m_l0bpll->setPU(m_pu);

    // Register the input registers
    std::map<std::string,Register*>::iterator ir ;
    for (ir = m_inputs.begin(); ir!= m_inputs.end(); ir++){   
      TileRegister * itr = dynamic_cast<TileRegister*>(ir->second);
      if (itr->Type()=="OpticalLink") {
	m_l0bpll->addInputRegister(itr);
      }
      if (itr->Type()=="L0Buffer") {
	m_l0bpll->addInputRegister(itr,"L0bIn");	  
      }
    }

    // Set the output registers
    std::string l0bufferMapPath    = pcrate->getProperty("L0BufferMapPath");
    if (m_debug) std::cout << "ProcUnit::initialize " 
			   << "l0bufferMapPath " << l0bufferMapPath <<  std::endl;
    std::map<std::string, std::string> registerNames = setPLLRegisters(m_pu, l0bufferMapPath);

    // Register the output register
    L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();

    // L0Buffer part
    std::string l0bufferRegisterName=registerNames["l0buffer"];
    if (m_debug) std::cout << "ProcUnit::initialize " 
			   << "l0bufferRegisterName " << l0bufferRegisterName <<  std::endl;
    if (l0bufferRegisterName!=UNKNOWN){
      TileRegister* reg = rfactory->searchTileRegister(l0bufferRegisterName.c_str());
      // Add the the register to the PLL L0buffer unit output registers
      m_l0bpll->addOutputRegister(reg,"L0bOut");	  
    } else {
      l0bufferPLLFlag = false;
    }

    // OL part
    std::string olRegisterName=registerNames["OL"];
    if (m_debug) std::cout << "ProcUnit::initialize " 
			   << "olRegisterName " << olRegisterName <<  std::endl;
    if (olRegisterName!=UNKNOWN){
      TileRegister* reg = rfactory->searchTileRegister(olRegisterName.c_str());
      // Add the the register to the PLL L0buffer unit output registers
      m_l0bpll->addOutputRegister(reg,"OLOut");	  
    } else {
      l0bufferPLLFlag = false;
    }

    //---- Output Settings for the L0Buffers
    std::string l0bufferStdOutSuffixe= pcrate->getProperty("L0BufferStdSuffixe");
    if (l0bufferStdOutSuffixe!=UNKNOWN) {
      L0BufferUnit* m_l0b = dynamic_cast<L0BufferUnit*>(subUnit("l0buf"));
      m_l0b->setOutputFile(m_pu, l0bufferStdOutSuffixe);
    }
    std::string l0bufferPLLOutSuffixe= pcrate->getProperty("L0BufferPLLSuffixe");
    if ( l0bufferPLLFlag  && l0bufferPLLOutSuffixe!=UNKNOWN) {
      L0BufferUnit* m_l0bpll = dynamic_cast<L0BufferUnit*>(subUnit("l0bufpll"));
      m_l0bpll->setOutputFile(m_pu, l0bufferPLLOutSuffixe);
    }
    if (m_debug) std::cout << "ProcUnit::initialize " 
			   << "l0bufferPLLOutSuffixe " << l0bufferPLLOutSuffixe << " " 
			   << "l0bufferPLLFlag " << l0bufferPLLFlag <<  std::endl;
  
  } // End if m_buildL0Buffer
  L0Muon::Unit::initialize();
}

void L0Muon::ProcUnit::execute() {

  if ( ! m_buildL0Buffer ) {

    //    // Skip this PU processing if no seeds are noticed
    //  
    //      bool skip = true;
    //      if ( m_ol3[0] ) {
    //        if ( ! m_ol3[0]->empty() ) skip = false;
    //      }  
    //      if ( m_ol3[1] ) {
    //        if ( ! m_ol3[1]->empty() ) skip = false;  
    //      }  
    //      if ( skip ) return;  

  }
  L0Muon::Unit::execute();
}
