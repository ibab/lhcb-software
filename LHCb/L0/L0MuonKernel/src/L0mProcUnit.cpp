//#include <time.h>
#include "L0MuonKernel/L0mProcUnit.h"
#include "L0mConf/L0MFoi.h"


L0Muon::L0mProcUnit::L0mProcUnit() {
  
  
}
L0Muon::L0mProcUnit::L0mProcUnit(L0MPuNodeBase & puNode,
                                 std::vector<double> & ptpara,
                                 bool & ignoreM1,
                                 std::vector<int> & foix,
                                 std::vector<int> & foiy,
                                 double & precision,
                                 int & bits,
                                 bool & writeL0buffer,
                                 MsgStream & log) {

  if ( ! m_units.empty() ) {
    m_units.clear();
  }

  m_ptparameters =ptpara;
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


  m_cu = new CablingUnit();
  
  addUnit(m_cu);

  m_cu->setParent(this);
  m_cu->setPU(puNode.id());

  m_formatting = new FormattingUnit();
  addUnit(m_formatting);
  m_formatting->setParent(this);

 
  if ( m_writeL0buffer) {
    m_l0b = new BuildL0BufferUnit();
    m_l0b->setOutputFile(m_puID);
    addUnit(m_l0b, "l0buf");
    m_l0b->setParent(this);
    m_l0b->setPU(puNode.id());
  } else if ( ! m_writeL0buffer) {
    m_l0b = 0;
  }
 

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
      log << MSG::DEBUG << "name for Register" <<" " << buf <<  endreq;
      log << MSG::DEBUG << "alias for Register" <<" " << bufalias <<  endreq;
      //*m_log << MSG::DEBUG << "tiles in reg" <<" " <<  endreq;
      //for (itlist =tlist.begin(); itlist < tlist.end(); itlist++){
      //*m_log<< MSG::DEBUG << " "  << (*itlist).quarter() << " "
      //      << (*itlist).region()  << " " 
      //      << (*itlist).nX() << " "
      //      << (*itlist).nY() << endreq;
      //}

      addInputRegister(reg);
      m_cu->addInputRegister(reg); 
      m_formatting->addInputRegister(reg);
     

      if ( m_writeL0buffer ){
        m_l0b->addInputRegister(reg);
      }


      olcounter++;
      
      
    }    
    
    
  }
  


  //======== Setting neighbours  =================

  enum Function {Horizontal, Vertical, Crossing, Backplane};

  Function func;
  
  std::vector<MuonTileID>::iterator ilist;

  
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

          log << MSG::DEBUG << "name for Register" <<" " << buf <<  endreq;
          //*m_log << MSG::DEBUG << "tiles in input neigh reg" <<" " <<  endreq;
         
          //std::vector<MuonTileID>::iterator imtl;
          // for (imtl =listoftiles.begin(); imtl < listoftiles.end(); imtl++){
          //*m_log<< MSG::DEBUG << " "  << (*imtl).quarter() << " "
          // << (*imtl).region()  << " " 
          // << (*imtl).nX() << " "
          // << (*imtl).nY() << endreq;
          //}

          addInputRegister(regIn);	
          m_cu->addInputRegister(regIn);


          if (m_writeL0buffer){
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
            //*m_log << MSG::DEBUG << " list of tags"<< " " 
            //<< (*itiles).tag() <<  endreq;                      
            
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

          log << MSG::DEBUG << "name for Register" <<" " << buf <<  endreq;
          //*m_log << MSG::DEBUG << "tiles in output neigh reg" <<" " <<  endreq;
          //std::vector<MuonTileID>::iterator imtlout;
          //for (imtlout =listoftilesout.begin(); 
          // imtlout < listoftilesout.end(); imtlout++){
          //*m_log<< MSG::DEBUG << " "  << (*imtlout).quarter() << " "
          // << (*imtlout).region()  << " " 
          // << (*imtlout).nX() << " "
          // << (*imtlout).nY() << endreq;
          //} //for (imtlout 
          
          

          m_formatting->addOutputRegister(nreg);
      
        } // if (bitsout
         
        
        
        
        
      } //  for (int sta
      
      
      
      
    } // for (ilist
    
    
    
    
  } //for (int ifunc
  
  
  
  //======= end neighbours ========================


  
  
}







  






L0Muon::L0mProcUnit::~L0mProcUnit() {
  
  delete m_cu;
  delete m_formatting;

  //#if __L0BUFFER__
  if (m_writeL0buffer){
    delete m_l0b;
  }
  
  //#endif

}

void L0Muon::L0mProcUnit::execute() {
  L0Muon::Unit::execute();
  
}

void L0Muon::L0mProcUnit::execute(MsgStream & log) {

 
  if ( ! m_units.empty() ) {
    std::map<std::string,L0Muon::Unit*>::iterator iu;
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      (*iu).second->execute(log);
    }
  }


  
}


void L0Muon::L0mProcUnit::initialize() {
  L0Muon::Unit::initialize();
}

void L0Muon::L0mProcUnit::initialize(MsgStream & log) {

 
   if ( ! m_units.empty() ) {
     std::map<std::string,L0Muon::Unit*>::iterator iu;
     for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {  

       (*iu).second->initialize(log);
     }
          
   } else {
     return ;
   }
   
       

}


void L0Muon::L0mProcUnit::finalize() {
  L0Muon::Unit::finalize();
  releaseRegisters();
  
  
 
}

 

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

