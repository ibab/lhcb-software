//#include <time.h>
#include "L0MuonKernel/L0mProcUnit.h"
#include "L0mConf/L0MFoi.h"
#include "L0MuonKernel/Utilities.h"

L0Muon::L0mProcUnit::L0mProcUnit() {
  
  
}
L0Muon::L0mProcUnit::L0mProcUnit(L0MPuNodeBase & puNode,
                                 std::vector<double> & ptpara,
                                 bool & ignoreM1,
                                 std::vector<int> & foix,
                                 std::vector<int> & foiy,
                                 double & precision,
                                 int & bits,
                                 std::string writeL0buffer) {

  if ( ! m_units.empty() ) {
    m_units.clear();
  }

  m_ptparameters =ptpara;
  
  if (m_debug) std::cout << "L0mProcUnit: ptpara[0]=" << ptpara[0] << std::endl; 
  
  m_ignoreM1 = ignoreM1;
  m_xfoi= foix;
  m_yfoi= foiy; 
  m_precision = precision;
  m_bits = bits;
  m_writeL0buffer = writeL0buffer;
  
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();
  
  m_maxFoi = puNode.fieldOfInterest();
  
  m_puID = puNode.id();
  m_boardID =puNode.board();
  
  setName(puNode.name());

  m_cu = new CablingUnit();
  
  addUnit(m_cu);

  m_cu->setParent(this);
  m_cu->setPU(puNode.id());

  m_formatting = new FormattingUnit();
  addUnit(m_formatting);
  m_formatting->setParent(this);

 
  if ( ! m_writeL0buffer.empty() ) {
    int type ;
    // Instanciate the L0Buffer described in the configuration
    type = 0;
    m_l0b = new L0BufferUnit(m_puID,m_xfoi,m_yfoi,type); 
    addUnit(m_l0b, "l0buf");

    // Instanciate the PLL L0Buffer 
    type = 1;
    m_l0bpll = new L0BufferUnit(m_puID,m_xfoi,m_yfoi,type); 
    m_l0b->addUnit(m_l0bpll, "l0bufpll");

  } else  {
    m_l0b = 0;
    m_l0bpll = 0;
  }
 

  // Prepare the vector of tiles to be put in the L0Buffer
  std::vector<L0MTile> l0bufferTileList(80);
  std::vector<L0MTile>::iterator itl0bufferTileList;

  for ( int sta = 0; sta < 5; sta++) {
   
    std::vector<L0MTile> ols = puNode.opticalLinks(sta);
    
    int olcounter = 0;
    std::vector<L0MTile>::iterator ilm;
    for ( ilm = ols.begin(); ilm != ols.end(); ilm++ ) {
      std::vector<L0MTile> tmpv = ilm->tiles();
      std::vector<L0MTile>::iterator itmpv ;
      std::vector<MuonTileID> tlist ;
      std::vector<MuonTileID>::iterator itlist;
      

       for (itmpv = tmpv.begin(); itmpv != tmpv.end(); itmpv++){
         if ((*itmpv).isMuonTile()){
           tlist.push_back(*itmpv);
           
           
         } // end if ((*itmpv).isMuonTile()
         
       } // end   for (itmpv = tmpv.begin(
       
       // Fill the L0Buffer vector of tiles with OLs
       l0bufferTileList.insert(l0bufferTileList.end(), tmpv.begin(), tmpv.end());
       std::vector<L0MTile> emptyList(32-tmpv.size());
       l0bufferTileList.insert(l0bufferTileList.end(), emptyList.begin(), emptyList.end());

      int bits = tlist.size();

      
      char buf[4096];
      char* format = "OL_%d_Q%d_%s_%d";
      sprintf(buf,format,sta,(*ilm).quarter(),puNode.name().c_str(),olcounter);
      
      TileRegister* reg = rfactory->createTileRegister(buf,bits);
          
      reg->setType("InputfieldOL");
      itlist = tlist.begin();
      
      reg->setTileVector(tlist);     
      
      reg->setTag(tmpv);
            
      char bufalias[4096];
      char* alias = "OL_%d_(Q%d,R%d,%d,%d)";     
          
      sprintf(bufalias,alias,sta,(*ilm).quarter(),
              (*ilm).region(), (*ilm).nX(), (*ilm).nY());
      
      int testalias=0;


    
      
      testalias = rfactory->createAlias(buf,bufalias);
      if (m_debug) std::cout << "L0mProcUnit: name for Register"  <<" " << buf <<  std::endl;
      if (m_debug) std::cout << "L0mProcUnit: alias for Register" <<" " << bufalias <<  std::endl;
      //*m_if (m_debug) std::cout << "tiles in reg" <<" " <<  std::endl;
      //for (itlist =tlist.begin(); itlist < tlist.end(); itlist++){
      //*m_log<< MSG::DEBUG << " "  << (*itlist).quarter() << " "
      //      << (*itlist).region()  << " " 
      //      << (*itlist).nX() << " "
      //      << (*itlist).nY() << std::endl;
      //}

      addInputRegister(reg);
      m_cu->addInputRegister(reg); 
      m_formatting->addInputRegister(reg);
     

      if ( ! m_writeL0buffer.empty() ){
	char  id[32];
	sprintf(id,"OL_S%d_%d",sta,olcounter);

        m_l0b->addInputRegister(reg,id);
        m_l0bpll->addInputRegister(reg,id);
      }


      olcounter++;
      
      
    }    
    
    
  }
  


  //======== Setting neighbours  =================

  enum Function {Horizontal, Vertical, Crossing, Backplane};

  Function func;
  
  std::vector<MuonTileID>::iterator ilist;

  // Initialize the position in the L0Buffer vector of tiles
  // for neighbour exchanges
  itl0bufferTileList = l0bufferTileList.begin();  
  itl0bufferTileList+=272;
  
  char buf[4096];
  char bufnamefunc[4096];
  char * namefunc=0;

  for (int ifunc = 0; ifunc<4; ifunc++){
    func = Function(ifunc);
    if (func == Horizontal){
      namefunc = "Horizontal";
    } 
    if (func == Vertical){
      namefunc = "Vertical";
    } 
    if (func == Crossing){
      namefunc = "Crossing";
    }
    if (func == Backplane){
      namefunc = "Backplane";
    }

    char* formatfunc = "%s";
    sprintf(bufnamefunc,formatfunc,namefunc);

    // ==============  Inputs  ============================
    std::vector<MuonTileID> listIn = puNode.pus(bufnamefunc,"From");
      
    for (ilist= listIn.begin(); ilist!= listIn.end(); ilist++){
      for (int sta =0; sta<5; sta++){
        std::vector<L0MTile> tilesIn= puNode.tiles(bufnamefunc, "From",
                                            *ilist, sta);

	// Fill the L0Buffer vector of tiles with neigbours
	l0bufferTileList.insert(itl0bufferTileList, tilesIn.begin(), tilesIn.end());
	itl0bufferTileList=l0bufferTileList.end();
        std::vector<L0MTile>::iterator itiles;
        std::vector<MuonTileID> listoftiles;

        for (itiles = tilesIn.begin(); itiles != tilesIn.end(); itiles++){
            listoftiles.push_back(*itiles); 
        }
        
        
        

        int bits = listoftiles.size();
        if (bits >0 ){
            char* formatIn = "%s_sta%d_Q%d_(R%d,%d,%d)_%s";
            sprintf(buf,formatIn,
                    namefunc, 
                    sta,
                    m_puID.quarter(),
                    (*ilist).region(),(*ilist).nX(),(*ilist).nY(), 
                          puNode.name().c_str());
           
          
          TileRegister* regIn= rfactory->createTileRegister(buf,bits);
          regIn->setTileVector(listoftiles);
         
          regIn->setTag(tilesIn);

          regIn->setType("InputfieldNeigh");

          if (m_debug) std::cout << "name for Register" <<" " << buf <<  std::endl;
          //*m_if (m_debug) std::cout << "tiles in input neigh reg" <<" " <<  std::endl;
         
          //std::vector<MuonTileID>::iterator imtl;
          // for (imtl =listoftiles.begin(); imtl < listoftiles.end(); imtl++){
          //*m_log<< MSG::DEBUG << " "  << (*imtl).quarter() << " "
          // << (*imtl).region()  << " " 
          // << (*imtl).nX() << " "
          // << (*imtl).nY() << std::endl;
          //}

          addInputRegister(regIn);	
          m_cu->addInputRegister(regIn);


          if (! m_writeL0buffer.empty()){
            m_l0b->addInputRegister(regIn);
          }
          

     
        } // if (bits >
        
        
        
      } // for (int sta 
      
      
      
      
    } // for (ilist
    
  
    
    // ==============  Outputs  ============================
    std::vector<MuonTileID> listOut= puNode.pus(bufnamefunc,"To");
        
    for (ilist= listOut.begin(); ilist!= listOut.end(); ilist++){
      for (int sta =0; sta<5; sta++){
        std::vector<L0MTile> tiles= 
          puNode.tiles(bufnamefunc, "To", *ilist, sta);
        std::vector<L0MTile>::iterator itiles;
        std::vector<MuonTileID> listoftilesout;
        for (itiles = tiles.begin(); itiles != tiles.end(); itiles++){
           
            listoftilesout.push_back(*itiles);
            //*m_if (m_debug) std::cout << " list of tags"<< " " 
            //<< (*itiles).tag() <<  std::endl;                      
            
        } // for (itiles = tiles.begin( 
        

        int bitsout = listoftilesout.size();
     
        if (bitsout >0 ){
          char* format = "%s_sta%d_Q%d_%s_(R%d,%d,%d)";
          sprintf(buf,format, 
                  namefunc, 
                  sta, 
                  m_puID.quarter(),
                  puNode.name().c_str(),
                  (*ilist).region(),(*ilist).nX(),(*ilist).nY());             
         
        
          TileRegister* nreg= rfactory->createTileRegister(buf,bitsout);
          nreg->setTileVector(listoftilesout);


          nreg->setTag(tiles);
          
          
          addOutputRegister(nreg);	  

          if (m_debug) std::cout << "name for Register" <<" " << buf <<  std::endl;
          //*m_if (m_debug) std::cout << "tiles in output neigh reg" <<" " <<  std::endl;
          //std::vector<MuonTileID>::iterator imtlout;
          //for (imtlout =listoftilesout.begin(); 
          // imtlout < listoftilesout.end(); imtlout++){
          //*m_log<< MSG::DEBUG << " "  << (*imtlout).quarter() << " "
          // << (*imtlout).region()  << " " 
          // << (*imtlout).nX() << " "
          // << (*imtlout).nY() << std::endl;
          //} //for (imtlout 
          
          

          m_formatting->addOutputRegister(nreg);
      
        } // if (bitsout
         
        
        
        
        
      } //  for (int sta
      
      
      
      
    } // for (ilist
    
    
    
    
  } //for (int ifunc

  // Complete the L0Buffer vector of tiles
  if (l0bufferTileList.size()<512) {
    std::vector<L0MTile> emptyList(512-l0bufferTileList.size());
    l0bufferTileList.insert(l0bufferTileList.end(), emptyList.begin(), emptyList.end());
  }
  
  //======= end neighbours ========================
  //======= L0Buffers output register ==========

  if ( ! m_writeL0buffer.empty()) {

    //log << MSG::INFO << "<L0mProcUnit::L0mProcUnit> l0bufferTileList: " <<l0bufferTileList.size()  <<endreq;
    //     int ind=0;
    //     for (std::vector<L0MTile>::iterator it= l0bufferTileList.begin();
    // 	 it!=l0bufferTileList.end();it++){
    
    //       log << MSG::INFO <<" " << ind 
    // 	               <<" " << (*it).tag() 
    // 	               <<"," <<(*it).layout()
    // 	               <<",Q"<<(*it).quarter()+1 
    //                        <<",M"<<(*it).station()+1 
    //                        <<",R"<<(*it).region()+1 
    // 	               <<"," <<(*it).nX()
    // 	               <<"," <<(*it).nY()
    // 	  <<  endreq;
    //       ind++;
    //     }
    
    std::vector<L0MTile> tiles;
    std::vector<L0MTile>::iterator itiles;
    std::vector<MuonTileID> mids;
    char buf[4096];
    char* format ;

    int flag_PLL_l0buffer=0;
    int flag_PLL_OL=0;

    //----- "Standard" L0Buffer -----
    // List of tiles in the L0Buffer
    tiles=l0bufferTileList;

    // Create the Tile register for the L0Buffer
    format = "BUF_Q%d_%s";
    sprintf(buf,format,puNode.id().quarter(),puNode.name().c_str());
    TileRegister* regl0buffer = rfactory->createTileRegister(buf,tiles.size());
        
    // Set the type
    regl0buffer->setType("L0Buffer");


    // Fill the MuonTileID vector list
    mids.clear();
    for (itiles = tiles.begin(); itiles != tiles.end(); itiles++){      
      mids.push_back(*itiles);
    }
    regl0buffer->setTileVector(mids);     
    
    // Fill the tag vector lists
    regl0buffer->setTag(tiles);
        
    // Add the the register to the L0buffer unit output registers
    m_l0b->addOutputRegister(regl0buffer,"L0bOut");
    // Add the the register to the PLL L0buffer unit input registers
    m_l0bpll->addInputRegister(regl0buffer,"L0bIn");	  

    //----- PLL L0Buffer ----- 
    // Read the map describing the content of the PLL L0Buffer
    // If no map is found, the L0Buffer for this PU is not activated.
    tiles=tileListFromMap(puNode.id(),0);
    if (tiles.size()>0) {

      flag_PLL_l0buffer = 1;

      // Create the Tile register for the PLL L0Buffer
      format = "PLLBUF_Q%d_%s";
      sprintf(buf,format,puNode.id().quarter(),puNode.name().c_str());
      TileRegister* regl0bufferPLL = rfactory->createTileRegister(buf,tiles.size());
      
      // Set the type
      regl0bufferPLL->setType("L0Buffer");
      
      // Fill the MuonTileID vector list
      mids.clear();
      for (itiles = tiles.begin(); itiles != tiles.end(); itiles++){
	mids.push_back(*itiles);
      }
      regl0bufferPLL->setTileVector(mids);     
      
      // Fill the tag vector lists
      regl0bufferPLL->setTag(tiles);
      
      // Add the the register to the PLL L0buffer unit output registers
      m_l0bpll->addOutputRegister(regl0bufferPLL,"L0bOut");	  
    }

    //----- PLL OL ----- 
    // Read the map describing the content of the PLL OL
    // If no map is found, the L0Buffer for this PU is not activated.
    tiles=tileListFromMap(puNode.id(),1);
    if (tiles.size()>0) {

      flag_PLL_OL = 1;

      // Create the Tile register for the PLL OL
      format = "PLLOL_Q%d_%s";
      sprintf(buf,format,puNode.id().quarter(),puNode.name().c_str());
      TileRegister* regOLPLL = rfactory->createTileRegister(buf,tiles.size());
      
      // Set the type
      regOLPLL->setType("OpticalLink");
      mids.clear();
      for (itiles = tiles.begin(); itiles != tiles.end(); itiles++){
	mids.push_back(*itiles);
      }
      regOLPLL->setTileVector(mids);     
      
      // Fill the tag vector lists
      regOLPLL->setTag(tiles);
      
      // Add the the register to the PLL L0buffer unit output registers
      m_l0bpll->addOutputRegister(regOLPLL,"OLOut");	  
      
    } 
    //---- Output Settings for the L0Buffers
    // If the PLL L0Buffer for this PU is active, 
    if (flag_PLL_l0buffer & flag_PLL_OL) {
      //m_l0b->activate();
      // Set the output files
      //m_l0b->setOutputFile("std");
      std::string suffixe = m_writeL0buffer;
      m_l0bpll->setOutputFile(suffixe);
    }
      
  }

  
  
}


L0Muon::L0mProcUnit::~L0mProcUnit() {
  
  delete m_cu;
  delete m_formatting;

  if (! m_writeL0buffer.empty() ){
    delete m_l0b;
  }
  

}

// void L0Muon::L0mProcUnit::execute() {
//   L0Muon::Unit::execute();  
// }

// void L0Muon::L0mProcUnit::initialize() {
//   L0Muon::Unit::initialize();
// }

// void L0Muon::L0mProcUnit::finalize() {
//   L0Muon::Unit::finalize();
//   releaseRegisters();
// }

 

int L0Muon::L0mProcUnit::xFoi(int sta)
{
  int xfoi= m_xfoi[sta];
  return xfoi;
  
}

int L0Muon::L0mProcUnit::yFoi(int sta)
{
  int yfoi= m_yfoi[sta];
  return yfoi;
  
}

