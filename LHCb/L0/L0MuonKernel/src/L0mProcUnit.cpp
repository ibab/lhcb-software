//#include <time.h>
#include "L0MuonKernel/L0mProcUnit.h"
#include "L0MuonKernel/CrateUnit.h"
#include "L0mConf/L0MFoi.h"
#include "L0MuonKernel/Utilities.h"

#define UNKNOWN "Unknown"

/**
   Empty Constructor
*/
L0Muon::L0mProcUnit::L0mProcUnit() {  
}

/**
   Constructor
*/
L0Muon::L0mProcUnit::L0mProcUnit(L0MPuNodeBase & puNode) {


  //=============================================
  //=                                           =
  //=   Set configuration dependant variables   =
  //=                                           =
  //=============================================

  // // std::cout << "L0mProcUnit::L0mProcUnit" <<" Set configuration dependant variables ..."<< "\n";
  m_maxFoi = puNode.fieldOfInterest();
  m_pu = puNode.id();
  m_boardID =puNode.board();
  setName(puNode.name());

  // // std::cout << "L0mProcUnit::L0mProcUnit" <<" ... done"<< "\n";
  
  //=============================================
  //=                                           =
  //=         Build the Units structure         =
  //=                                           =
  //=============================================

  if ( ! m_units.empty() ) {
    m_units.clear();
  }

  // // std::cout << "L0mProcUnit::L0mProcUnit" <<" CablingUnit ..."<< "\n";
  // Cabling Unit
  CablingUnit    * m_cu = new CablingUnit();
  addUnit(m_cu);
  m_cu->setParent(this);
  m_cu->setPU(puNode.id());
  // // std::cout << "L0mProcUnit::L0mProcUnit" <<" ... OK"<< "\n";

  // // std::cout << "L0mProcUnit::L0mProcUnit" <<" FormattingUnit ..."<< "\n";
  // Formatting Unit
  FormattingUnit * m_formatting = new FormattingUnit();
  addUnit(m_formatting);
  m_formatting->setParent(this);
  // // std::cout << "L0mProcUnit::L0mProcUnit" <<" ... OK"<< "\n";

  // L0Buffer Unit
  // // std::cout << "L0mProcUnit::L0mProcUnit" <<" L0BufferUnit ..."<< "\n";
  int type=0;
  L0BufferUnit   * m_l0b = new L0BufferUnit(type); 
  addUnit(m_l0b,"l0buf");
  m_l0b->setParent(this); 
  m_l0b->setPU(puNode.id());
  // // std::cout << "L0mProcUnit::L0mProcUnit" <<" ... OK"<< "\n";

  //=============================================
  //=                                           =
  //=         Build the TileRegisters           =
  //=                                           =
  //=============================================
  // // std::cout << "L0mProcUnit::L0mProcUnit" <<" Build the TileRegisters ..."<< "\n";

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
      // Transform 'L0MTileList' into 'vector<MuonTileID>'
      std::vector<MuonTileID> muontilelist ;
      for (std::vector<L0MTile>::iterator it = l0mtilelist.begin(); it != l0mtilelist.end(); it++){
	if ((*it).isMuonTile()){
	  muontilelist.push_back(*it);
	}         
      }
       
      // If tiles are transfered
      int nbits = muontilelist.size();
      if (nbits >0 ){

	// TileRegister name
	char buf_name[4096];
	char* format_name = "OL_%d_Q%d_%s_%d";
	sprintf(buf_name,format_name,sta,(*il).quarter(),puNode.name().c_str(),olcounter);
	if (m_debug) std::cout << "L0mProcUnit: name for Register"  <<" " << buf_name <<  std::endl;

	// TileRegister alias
	char buf_alias[4096];
	char* format_alias = "OL_%d_(Q%d,R%d,%d,%d)";               
	sprintf(buf_alias,format_alias,sta,(*il).quarter(),(*il).region(), (*il).nX(), (*il).nY());
	if (m_debug) std::cout << "L0mProcUnit: alias for Register" <<" " << buf_alias <<  std::endl;

	// Create the TileRegister
	TileRegister* reg = rfactory->createTileRegister(buf_name,nbits);
	rfactory->createAlias(buf_name,buf_alias);
	reg->setType("OpticalLink");
	reg->setTileVector(muontilelist);     
	reg->setTag(l0mtilelist);      

        // Store the OL in station 3
	//std::cout << "PU: " << puNode.name() << " " << reg->name() << std::endl;
	if ( sta == 2 ) m_ol3[olcounter] = reg;

	// Register the tileregister
	addInputRegister(reg);

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

	// Transform 'L0MTileList' into 'vector<MuonTileID>'
        std::vector<MuonTileID> muontilelist;
        for (std::vector<L0MTile>::iterator it = l0mtilelist.begin(); it != l0mtilelist.end(); it++){
	  muontilelist.push_back(*it); 
        }
       
	// If tiles are transfered
        int nbits = muontilelist.size();
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
          reg->setTileVector(muontilelist);        
          reg->setTag(l0mtilelist);
          reg->setType("Neighbours");

	  // Register the tileregister
          addInputRegister(reg);	
          
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
	// Transform 'L0MTileList' into 'vector<MuonTileID>'
        std::vector<MuonTileID> muontilelist;
        for (std::vector<L0MTile>::iterator it = l0mtilelist.begin(); it != l0mtilelist.end(); it++){           
	  muontilelist.push_back(*it);
        } 
 	
	// If tiles are transfered
        int nbits = muontilelist.size();
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
          reg->setTileVector(muontilelist);
          reg->setTag(l0mtilelist);
          reg->setType("Neighbours");
          
	  // Register the tileregister
          addOutputRegister(reg);	  
      
        } // End if tiles transfered        
      } // End of loop over stations
    } // End of loop over pus

    // ---  End Outputs  ---------------
    
  } // End of Loop over functions

  //======= End neighbours ========================

  //======= L0Buffers register ====================
  
  // Get list of tiles
  std::vector<L0MTile> l0mtilelist = l0bufferTileList(puNode);
  // Transform 'L0MTileList' into 'vector<MuonTileID>'
  std::vector<MuonTileID> muontilelist;
  for (std::vector<L0MTile>::iterator it = l0mtilelist.begin(); it != l0mtilelist.end(); it++){      
    muontilelist.push_back(*it);
  }

  // If tiles are transfered
  int nbits = muontilelist.size();
  if (nbits >0 ){
    // Create the Tile register for the L0Buffer
    char buf[4096];
    char* format ;
    format = "BUF_Q%d_%s";
    sprintf(buf,format,puNode.id().quarter(),puNode.name().c_str());
    if (m_debug) std::cout << "name for Register" <<" " << buf <<  std::endl;

    // Create the TileRegister
    TileRegister* reg = rfactory->createTileRegister(buf,nbits);
    reg->setTileVector(muontilelist);     
    reg->setTag(l0mtilelist);
    reg->setType("L0Buffer");
    
    // Register the tileregister
    addInputRegister(reg);
  }
  // // std::cout << "L0mProcUnit::L0mProcUnit" <<" ... done"<< "\n";

  //=============================================
  //=                                           =
  //=   Register the registers in the subunits  =
  //=                                           =
  //=============================================

  // // std::cout << "L0mProcUnit::L0mProcUnit" <<" Register the registers in the subunits ..."<< "\n";
  std::map<std::string,Register*>::iterator ir ;
  for (ir = m_inputs.begin(); ir!= m_inputs.end(); ir++){
    TileRegister * itr = dynamic_cast<TileRegister*>(ir->second);
    if (itr->Type()=="OpticalLink") {
      m_cu->addInputRegister(itr); 
      m_formatting->addInputRegister(itr);
      m_l0b->addInputRegister(itr);
    }
    if (itr->Type()=="Neighbours") {
      m_cu->addInputRegister(itr);
      m_l0b->addInputRegister(itr);
    }
    if (itr->Type()=="L0Buffer") {
      m_l0b->addOutputRegister(itr,"L0bOut");
    }
  }
  for (ir = m_outputs.begin(); ir!= m_outputs.end(); ir++){
    TileRegister * itr = dynamic_cast<TileRegister*>(ir->second);
    if (itr->Type()=="Neighbours") {
      m_formatting->addOutputRegister(itr);
    }
  }

  // // std::cout << "L0mProcUnit::L0mProcUnit" <<" ... done"<< "\n";
}

/**
   Return the list of tiles in the l0buffer
   SHOULD BE CHANGED TO BE SET ACCORDING TO THE DATABASE
*/
std::vector<L0MTile> L0Muon::L0mProcUnit::l0bufferTileList(L0MPuNodeBase & puNode){

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
std::map<std::string, std::string>  L0Muon::L0mProcUnit::setPLLRegisters(MuonTileID puid, std::string l0bufferMapPath){
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
  std::vector<L0MTile> l0mtilelist;
      
  // PLL L0Buffer:
  // Read the map describing the content of the PLL L0Buffer
  // If no map is found, the L0Buffer for this PU is not activated.
  l0mtilelist=tileListFromMap(puid,0,l0bufferMapPath);
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
      
    // Fill the MuonTileID vector list
    std::vector<MuonTileID> muontilelist;
    for (std::vector<L0MTile>::iterator it = l0mtilelist.begin(); it != l0mtilelist.end(); it++){
      muontilelist.push_back(*it);
    }
    reg->setTileVector(muontilelist);     
      
    // Fill the tag vector lists
    reg->setTag(l0mtilelist);
      
  } // End PLL L0Buffer

  // PLL OL:
  // Read the map describing the content of the PLL OL
  // If no map is found, the L0Buffer for this PU is not activated.
  l0mtilelist=tileListFromMap(puid,1,l0bufferMapPath);
  

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

    // Fill the MuonTileID vector list
    std::vector<MuonTileID> muontilelist;
    for (std::vector<L0MTile>::iterator it = l0mtilelist.begin(); it != l0mtilelist.end(); it++){
      muontilelist.push_back(*it);
    }
    reg->setTileVector(muontilelist);           

    // Fill the tag vector lists
    reg->setTag(l0mtilelist);
            
  } // End PLL OL

  return registerNames;

}

/**
   Bootstrap
*/
void L0Muon::L0mProcUnit::bootstrap() {
  CrateUnit * pcrate = dynamic_cast<CrateUnit *>( parentByType("CrateUnit"));

  if ( pcrate->getProperty("WriteL0Buffer") == "True") m_writeL0Buffer = true;
  if ( pcrate->getProperty("BuildL0Buffer") == "False") m_buildL0Buffer = false;

  // Construct the PLL L0Buffer (V2-TestBench)
  std::string l0bufferPLLFlag    = pcrate->getProperty("L0BufferPLLFlag");
  
  if ( m_buildL0Buffer ) {
  
    if (l0bufferPLLFlag == "True") {

      // Create a L0BufferUnit for the PLL buffer
      int type=1;
      L0BufferUnit   * m_l0bpll = new L0BufferUnit(type); 
      addUnit(m_l0bpll, "l0bufpll");
      m_l0bpll->setParent(this);
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
      std::map<std::string, std::string> registerNames = setPLLRegisters(m_pu, l0bufferMapPath);

      // Register the output register
      L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();

      // L0Buffer part
      std::string l0bufferRegisterName=registerNames["l0buffer"];
      if (l0bufferRegisterName!=UNKNOWN){
	TileRegister* reg = rfactory->searchTileRegister(l0bufferRegisterName.c_str());
	// Add the the register to the PLL L0buffer unit output registers
	m_l0bpll->addOutputRegister(reg,"L0bOut");	  
      } else {
	l0bufferPLLFlag = "False";
      }

      // OL part
      std::string olRegisterName=registerNames["OL"];
      if (olRegisterName!=UNKNOWN){
	TileRegister* reg = rfactory->searchTileRegister(olRegisterName.c_str());
	// Add the the register to the PLL L0buffer unit output registers
	m_l0bpll->addOutputRegister(reg,"OLOut");	  
      } else {
	l0bufferPLLFlag = "False";
      }
    }// End if (l0bufferPLLFlag == "True")

    //---- Output Settings for the L0Buffers
    std::string l0bufferStdOutSuffixe= pcrate->getProperty("L0BufferStdSuffixe");
    // // std::cout << "L0mProcUnit::bootstrap" <<"  l0bufferStdOutSuffixe= "<< l0bufferStdOutSuffixe << "\n";
    if (l0bufferStdOutSuffixe!=UNKNOWN) {
      L0BufferUnit* m_l0b = dynamic_cast<L0BufferUnit*>(subUnit("l0buf"));
      m_l0b->setOutputFile(m_pu, l0bufferStdOutSuffixe);
    }
    std::string l0bufferPLLOutSuffixe= pcrate->getProperty("L0BufferPLLSuffixe");
    // // std::cout << "L0mProcUnit::bootstrap" <<"  l0bufferPLLOutSuffixe= "<< l0bufferPLLOutSuffixe << "\n";
    if ( l0bufferPLLFlag == "True" && l0bufferPLLOutSuffixe!=UNKNOWN) {
      L0BufferUnit* m_l0bpll = dynamic_cast<L0BufferUnit*>(subUnit("l0bufpll"));
      m_l0bpll->setOutputFile(m_pu, l0bufferPLLOutSuffixe);
    }
  }
  L0Muon::Unit::bootstrap();
  // // std::cout << "L0mProcUnit::bootstrap" <<"  ...done "<< "\n";
}

void L0Muon::L0mProcUnit::execute() {

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
