//#include <time.h>
#include "L0MuonKernel/ProcUnit.h"
#include "L0mConf/L0MFoi.h"
#include "L0MuonKernel/Utilities.h"
#include "ProcessorKernel/RegisterFactory.h"
#include "L0MuonKernel/L0BufferProcUnit.h"
#include "L0MuonKernel/CoreUnit.h"
#include "L0MuonKernel/FormattingUnit.h"

#define UNKNOWN "Unknown"

/**
   Empty Constructor
*/
L0Muon::ProcUnit::ProcUnit() {  
}

/**
   Constructor
*/
L0Muon::ProcUnit::ProcUnit(L0MPuNodeBase & puNode,int rawBufferMode, int l0BufferMode) {

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

  // Formatting Unit
  FormattingUnit * formatting = new FormattingUnit(m_mid);
  addUnit(formatting);

  // Core Unit
  CoreUnit    * core = new CoreUnit(m_mid);
  addUnit(core);

  // L0Buffer Unit
  L0BufferProcUnit   * l0buffer = new L0BufferProcUnit(m_mid); 
  if (l0BufferMode==1) addUnit(l0buffer);
  
  //=============================================
  //=                                           =
  //=         Build the TileRegisters           =
  //=                                           =
  //=============================================

  char buf[4096];
  char buf_alias[4096];
  char* format ;
  
  // Counter for the l0buffer input register
  int l0bufferRegCount=0;
  
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
        format = "OL_%s_M%d_%d";
        sprintf(buf,format,puNode.name().c_str(),sta+1,olcounter);
        if (m_debug) std::cout << "ProcUnit: name for Register"  <<" " << buf <<  std::endl;
 
        // Create the TileRegister
        TileRegister* reg = rfactory->createTileRegister(buf,nbits);

        // TileRegister aliases
        // Alias used at filling time
        format = "OL_%d_(Q%d,R%d,%d,%d)";               
        sprintf(buf_alias,format,sta,(*il).quarter(),(*il).region(), (*il).nX(), (*il).nY());
        rfactory->createAlias(buf,buf_alias);
        if (m_debug) std::cout << "ProcUnit: alias for Register" << buf <<" is " << buf_alias <<  std::endl;
 
        if (rawBufferMode>0) { // Only for Medium (1) and Full (2) raw buffers
          // Alias used for the raw buffers
          format = "RAWDATAIN%s_%d";               
          sprintf(buf_alias,format,puNode.name().c_str(),l0bufferRegCount);
          rfactory->createAlias(buf,buf_alias);
          if (m_debug) std::cout << "ProcUnit: alias for Register" << buf <<" is " << buf_alias <<  std::endl;
        }
        
        // Fill the vector of tiles
        L0Muon::splitTileListInfo(l0mtilelist,reg);

        // Optical link
        reg->setType("OpticalLink");
        
        // Store the OL in station 3
        if ( sta == 2 ) m_ol3[olcounter] = reg;

        // Register the tileregister
        core->addInputRegister(reg); 
        formatting->addInputRegister(reg);
        format = "RAW_%d";
        sprintf(buf,format,l0bufferRegCount++);
        l0buffer->addInputRegister(reg,buf);
    
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

    L0MTileList lpus;
  
    // ---  Inputs  -------------------
    lpus = puNode.pus(L0MBase::PUNIT,function,"From");
      
    // Loop over pus connected with the current function
    for (L0MTileList::iterator ip= lpus.begin(); ip!= lpus.end(); ip++){
      // Loop over stations
      for (int sta =0; sta<5; sta++){
        // Get list of tiles
        std::vector<L0MTile> l0mtilelist= puNode.tiles(L0MBase::PUNIT,function, "From",*ip, sta);

        // If tiles are transfered
        int nbits = l0mtilelist.size();
        if (nbits >0 ){

          // TileRegister name
          format = "%s_M%d_Q%dR%d%d%d_%s";
          sprintf(buf,format,
                  namefunc, 
                  sta+1,
                  (*ip).quarter()+1,(*ip).region()+1,(*ip).nX(),(*ip).nY(), 
                  puNode.name().c_str());
          if (m_debug) std::cout << "name for Register" <<" " << buf <<  std::endl;
           
          // Create the TileRegister
          TileRegister* reg= rfactory->createTileRegister(buf,nbits);
          reg->setType("Neighbours");
          L0Muon::splitTileListInfo(l0mtilelist,reg);
	
          if (rawBufferMode>1) {// Only for Full (2) raw buffers
            // TileRegister alias (used for the raw buffers)
            format = "RAWDATAIN%s_%d";               
            sprintf(buf_alias,format,puNode.name().c_str(),l0bufferRegCount);
            rfactory->createAlias(buf,buf_alias);
            if (m_debug) std::cout << "ProcUnit: alias for Register " << buf<<" is " << buf_alias <<  std::endl;
          }
        
          // Register the tileregister
          core->addInputRegister(reg);
          format = "RAW_%d";
          sprintf(buf,format,l0bufferRegCount++);
          l0buffer->addInputRegister(reg,buf);
          
          
        } // End if tiles transfered        
      } // End of loop over stations
    } //  End of loop over pus
    
    // ---  End Inputs  ----------------
  
    
    // ---  Outputs  -------------------
    lpus= puNode.pus(L0MBase::PUNIT,function,"To");
       
    // Loop over pus connected with the current function
    for (L0MTileList::iterator ip= lpus.begin(); ip!= lpus.end(); ip++){
      // Loop over stations
      for (int sta =0; sta<5; sta++){
        // Get list of tiles
        std::vector<L0MTile> l0mtilelist= puNode.tiles(L0MBase::PUNIT,function, "To", *ip, sta);
 	
        // If tiles are transfered
        int nbits = l0mtilelist.size();
        if (nbits >0 ){
	  
          // TileRegister name
          format = "%s_M%d_%s_Q%dR%d%d%d";
          sprintf(buf,format, 
                  namefunc, 
                  sta+1, 
                  puNode.name().c_str(),
                  (*ip).quarter()+1,(*ip).region()+1,(*ip).nX(),(*ip).nY());             
          if (m_debug) std::cout << "name for Register" <<" " << buf <<  std::endl;
         
          // Create the TileRegister
          TileRegister* reg= rfactory->createTileRegister(buf,nbits);
          reg->setType("Neighbours");
          L0Muon::splitTileListInfo(l0mtilelist,reg);

          // Register the tileregister
          formatting->addOutputRegister(reg);
      
        } // End if tiles transfered        
      } // End of loop over stations
    } // End of loop over pus

    // ---  End Outputs  ---------------
    
  } // End of Loop over functions

  //======= End neighbours ========================

  //======= L0Buffer registers ====================
  
  // --- Input field  ---------------

  // Get list of tiles
  MuonTileID empty_tile = MuonTileID();
  std::vector<L0MTile> l0mtilelist(36*16,empty_tile);
  L0MFunctionList lfunc =  puNode.functions(L0MBase::L0BUFFER,L0MBase::FROM);
  L0MTileList tiles ;
  for (L0MFunctionList::iterator ifun = lfunc.begin();ifun !=lfunc.end();ifun++){
    std::string func = (*ifun);
    //std::cout << "L0Muon::ProcUnit::l0bufferTileList func: "<<func<<  std::endl;
    L0MTileList tiles = puNode.tiles(L0MBase::L0BUFFER,func);;
    for ( L0MTileList::iterator it=tiles.begin();it!=tiles.end();it++) {
      int pos = (*it).position();
      l0mtilelist[pos]=(*it);
    }
  }


  if (l0BufferMode!=0 || rawBufferMode!=0) {
    // If tiles are transfered
    int nbits = l0mtilelist.size();
    if (nbits >0 ){
      // Create the Tile register for the L0Buffer
      format = "FORMATTEDDATAIN_%s";
      sprintf(buf,format,puNode.name().c_str());
      TileRegister* reg = rfactory->createTileRegister(buf,nbits);
      reg->setType("L0BufferInputField");
      L0Muon::splitTileListInfo(l0mtilelist,reg);

      // Register the tileregister
      l0buffer->addInputRegister(reg,"FORMATTED");
    }
  
  }

  //======= Build the Candidate register ====================
  format = "CAND_PU%s";
  sprintf(buf,format,puNode.name().c_str());
  Register* candreg = rfactory->createRegister(buf,L0Muon::BitsCandRegTot);
  candreg->setType("Candidates");
  core->addOutputRegister(candreg);
  l0buffer->addInputRegister(candreg);
}

/**
   Constructor
*/
L0Muon::ProcUnit::ProcUnit(DOMNode* pNode):L0MUnit(pNode) {
  
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
