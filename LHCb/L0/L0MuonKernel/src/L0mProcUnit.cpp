//#include <time.h>
#include "L0MuonKernel/L0mProcUnit.h"
#include "L0mConf/L0MFoi.h"


L0Muon::L0mProcUnit::L0mProcUnit() {
  
  
}
L0Muon::L0mProcUnit::L0mProcUnit(L0MPuNodeBase& puNode,
                                 std::vector<double> & ptpara,
                                 std::vector<int> & foix,
                                 std::vector<int> & foiy,
                                 double & precision,
                                 int & bits,
                                 MsgStream & log) {


  
  if ( ! m_units.empty() ) {
    m_units.clear();
  }
  m_ptparameters =ptpara;
  m_xfoi= foix;
  m_yfoi= foiy; 
  m_precision = precision;
  m_bits = bits;

  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();
  
  m_maxFoi = puNode.fieldOfInterest();
  
  m_puID = puNode.id();
  m_boardID =puNode.board();

  m_cu = new CablingUnit();
  
  addUnit(m_cu);

  m_cu->setParent(this);
  m_cu->setPU(puNode.id());
  

  // m_l0b = new BuildL0BufferUnit();
  //m_l0b->setParent(this);
  //addUnit(l0b, "l0buf");
  //m_l0b->setPU(puNode.id());

  int sta;

  m_formatting = new FormattingUnit();
  addUnit(m_formatting);

  log << MSG::DEBUG << "Setting OL's" << endreq;
  
  for ( sta = 0; sta < 5; sta++) {
   
    std::vector<L0MTile> ols = puNode.opticalLinks(sta);
    
    int olcounter = 0;
    std::vector<L0MTile>::iterator ilm;
    for ( ilm = ols.begin(); ilm != ols.end(); ilm++ ) {
      std::vector<MuonTileID> tlist ;
      std::vector<MuonTileID>::iterator itlist;
       std::vector<L0MTile> tmpv = ilm->tiles();
       std::vector<L0MTile>::iterator itmpv ;

       for (itmpv = tmpv.begin(); itmpv != tmpv.end(); itmpv++){
         if ((*itmpv).isMuonTile()){
           tlist.push_back(*itmpv);
           
           
         }
         
         
         
         
       }

      int bits = tlist.size();
      for (itlist =tlist.begin(); itlist < tlist.end(); itlist++){
        //std::cout<< "\n TEST"  << *itlist ;
      }
      
      char buf[4096];
      char* format = "OL_%d_Q%d_%s_%d";
      sprintf(buf,format,sta,(*ilm).quarter(),puNode.name().c_str(),olcounter);
      
      TileRegister* reg = rfactory->createTileRegister(buf,bits);
          
      reg->setType("InputfieldOL");
      itlist = tlist.begin();
      reg->setType(sta, (*itlist).region());      
      
      reg->setTileVector(tlist);     
      
      char bufalias[4096];
      char* alias = "OL_%d_(Q%d,R%d,%d,%d)";     
          
        sprintf(bufalias,alias,sta,(*ilm).quarter(),
                (*ilm).region(), (*ilm).nX(), (*ilm).nY());
      
      int testalias=0;
      testalias = rfactory->createAlias(buf,bufalias);

      addInputRegister(reg);
      m_cu->addInputRegister(reg); 
      m_formatting->addInputRegister(reg);
     
      //m_l0b->addInputRegister(reg);
      
      olcounter++;
      
      
    }    
    
    
  }
  
  
  
  //======== Setting neighbours  =================
  //
  enum Function {Horizontal, Vertical, Crossing, Backplane};
  //enum Ways {From, To};
  Function func;
  //Ways way;
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
        std::vector<L0MTile> tiles= puNode.tiles(bufnamefunc, "From",
                                            *ilist, sta);

        std::vector<L0MTile>::iterator itiles;
        std::vector<MuonTileID> listoftiles;
        for (itiles = tiles.begin(); itiles != tiles.end(); itiles++){
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
          itiles = tiles.begin();
          if ( ! (*itiles).isMuonTile()){
            regIn->setTypeMT(sta);
          }
          else {
            regIn->setType(sta, (*itiles).region());
          }
          
          regIn->setType("InputfieldNeigh");
          addInputRegister(regIn);	
          m_cu->addInputRegister(regIn);
          //m_l0b->addInputRegister(regIn);
        }
        
        }
       
    }
    
    
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
          
            
        }

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

          itiles = tiles.begin();
          if ( ! (*itiles).isMuonTile()){
            nreg->setTypeMT(sta);
          } else {
            nreg->setType(sta, (*itiles).region());
          }
          
          addOutputRegister(nreg);	  
          m_formatting->addOutputRegister(nreg);
        }
        
        
        
      }
      
      
      
    }
    
    
    
  }
  
  //======= end neighbours ========================
  
}



  






L0Muon::L0mProcUnit::~L0mProcUnit() {
  
}

void L0Muon::L0mProcUnit::execute() {

  if ( ! m_units.empty() ) {
    std::map<std::string,L0Muon::Unit*>::iterator iu;
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      (*iu).second->execute();
    }
  }

}


void L0Muon::L0mProcUnit::initialize() {

   if ( ! m_units.empty() ) {
     std::map<std::string,L0Muon::Unit*>::iterator iu;
     for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {  
       (*iu).second->initialize();
     }
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

