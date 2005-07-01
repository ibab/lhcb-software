//#include <time.h>
#include "L0MuonKernel/ProcUnit.h"
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
  //=   Set configuration dependant variables   =
  //=                                           =
  //=============================================

  m_mid = puNode.id();
  
  //=============================================
  //=                                           =
  //=         Build the Units structure         =
  //=                                           =
  //=============================================

  if ( ! m_units.empty() ) {
    m_units.clear();
  }

  // Core Unit
  CoreUnit    * m_cu = new CoreUnit(m_mid);
  addUnit(m_cu);

  // Formatting Unit
  FormattingUnit * m_formatting = new FormattingUnit(m_mid);
  addUnit(m_formatting);

  // L0Buffer Unit
  L0BufferUnit   * m_l0b = new L0BufferUnit(m_mid); 
  addUnit(m_l0b);

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
	// 	addInputRegister(reg);
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
		  m_mid.quarter(),
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
                  m_mid.quarter(),
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
  // // //     sprintf(buf,name,m_mid.region(), m_mid.nX(), m_mid.nY(),icand+1);
  // // //     TileRegister* pReg= rfactory->createTileRegister(buf,nbits);
  // // //     pReg->setType("Outputfield");     
  // // //     char * format = "cand%d";      
  // // //     sprintf(buf,name,icand);      
  // // //     m_l0b->addInputRegister(reg,buf);
  // // //   }

}
/**
   Constructor
*/
L0Muon::ProcUnit::ProcUnit(DOMNode* pNode):L0MUnit(pNode) {
  
  //   for (std::vector<L0Muon::Unit*>::iterator iu=m_units.begin();iu!=m_units.end();iu++){
  //     if ((*iu)->type()=="CoreUnit") m_cu = (*iu);
  //     if ((*iu)->type()=="FomattingUnit") m_formatting = (*iu);
  //     if ((*iu)->type()=="L0BufferUnit") m_l0b = (*iu);
  //   }
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
   Initialize
*/
void L0Muon::ProcUnit::initialize() {
  if (m_debug) std::cout << "*!* ProcUnit::initialize " <<  std::endl;
  Unit * parent = parentByType("MuonTriggerUnit");

  std::string buildL0Buffer = parent->getProperty("BuildL0Buffer");
  m_buildL0Buffer = false;
  if (buildL0Buffer== "True") m_buildL0Buffer = true;

  if (m_debug) std::cout << "*!* ProcUnit::initialize " 
			 << "m_buildL0Buffer " << m_buildL0Buffer <<  std::endl;
  
  if ( m_buildL0Buffer ) {
    //---- Output Settings for the L0Buffers
    std::string suffixe= parent->getProperty("L0BufferPath");
    if (suffixe!=UNKNOWN) {
      for (std::vector<L0Muon::Unit*>::iterator iu=m_units.begin();iu!=m_units.end();iu++){
	if ((*iu)->type()=="L0BufferUnit"){
	  L0BufferUnit* l0buffer= dynamic_cast<L0BufferUnit*>(*iu);
	  l0buffer->setOutputFile(m_mid, suffixe);
	}
      }
    }
  
  } // End if m_buildL0Buffer
  L0Muon::Unit::initialize();
}

void L0Muon::ProcUnit::execute() {

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
  
  L0Muon::Unit::execute();
}
