#include "L0MuonKernel/BuildL0BufferUnit.h"
#include "L0MuonKernel/L0mProcUnit.h"

L0Muon::BuildL0BufferUnit::BuildL0BufferUnit(){
  
  boost::dynamic_bitset<> tmpl0(512);
  m_l0buf = tmpl0;
  boost::dynamic_bitset<> tmpoutfield(48);
  m_outfield = tmpoutfield ;
  
  boost::dynamic_bitset<> tmpinpfield(432);
  m_inpfield = tmpinpfield ;
 
  boost::dynamic_bitset<> tmpidfield(32);
  m_idfield = tmpidfield ; 

};

L0Muon::BuildL0BufferUnit::~BuildL0BufferUnit(){}

void L0Muon::BuildL0BufferUnit::setIdField(){
}

void L0Muon::BuildL0BufferUnit::setInputField(){
  std::map<std::string,Register*>::iterator ir ;

  boost::dynamic_bitset<>::size_type j = 0;
  
  for (ir = m_inputs.begin(); ir!= m_inputs.end(); ir++){
    
    TileRegister * itr = dynamic_cast<TileRegister*>(ir->second);
  
    if (itr->Type()== "InputfieldNeigh"){
      boost::dynamic_bitset<> ifield = itr->getBitset();
      for (boost::dynamic_bitset<>::size_type i=0; i<ifield.size();i++){
        if (j < 224){
          m_inpfield[j]=ifield[i];
          j++;        
        }
        
      }      
    }
    
  }
  
  j = 224;
  for (ir = m_inputs.begin(); ir!= m_inputs.end(); ir++){
    TileRegister * itr = dynamic_cast<TileRegister*>(ir->second);
    if (itr->Type()== "InputfieldOL"){
          
      if (j> 271 && j< 303)  j=304;
      if (j> 304 && j< 335)  j=336;
      if (j> 336 && j< 359)  j=360;
      
        boost::dynamic_bitset<> ifield = itr->getBitset();
        for (boost::dynamic_bitset<>::size_type i=0; i<ifield.size();i++){
          if (j < 392){           
            m_inpfield[j]=ifield[i];
            j++;
          }          
        }
    }
    
  }  
}


void L0Muon::BuildL0BufferUnit::setOutputField()
{
  
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();
  std::map<std::string,L0Muon::Register*>::iterator ind;
  std::map<std::string,L0Muon::Register*> reg = rfactory->getRegisters();
  char buf[4096] , buf1[4096], buf2[4096];
  char * nameaddr1 ="(R%d,%d,%d)_addr_candidate1";
  sprintf(buf, nameaddr1 ,m_pu.region(),m_pu.nX(),m_pu.nY());
  char * nameaddr2 ="(R%d,%d,%d)_addr_candidate2";
  sprintf(buf1, nameaddr2 ,m_pu.region(),m_pu.nX(),m_pu.nY());
  char * namestatus ="(R%d,%d,%d)_status";
  sprintf(buf2, namestatus ,m_pu.region(),m_pu.nX(),m_pu.nY());

  boost::dynamic_bitset<>::size_type j =0;
  boost::dynamic_bitset<> ifield ;
  int count =0;
  for (ind = reg.begin(); ind != reg.end(); ind ++){
    if ((*ind).first == buf ||(*ind).first == buf1 || (*ind).first == buf2 ){
           
      Register * reg = dynamic_cast<Register*>((*ind).second);
      boost::dynamic_bitset<> ifield = reg->getBitset();
      for (boost::dynamic_bitset<>::size_type i=0; i<ifield.size();i++){
        m_outfield[j+ count]= ifield[i];
        j++;
        addInputRegister(reg);      
      }

      count ++;
    }
    }
  
}

boost::dynamic_bitset<> L0Muon::BuildL0BufferUnit::getIdField(){

  for (boost::dynamic_bitset<>::size_type i=0; i< m_idfield.size();i++){
    //std::cout << m_idfield[i];
  }
  return m_idfield;
}

boost::dynamic_bitset<> L0Muon::BuildL0BufferUnit::getInputField(){
  return m_inpfield;
}


boost::dynamic_bitset<> L0Muon::BuildL0BufferUnit::getOutputField(){
  return m_outfield;
}

void L0Muon::BuildL0BufferUnit::setL0buf(){
  boost::dynamic_bitset<>::size_type j =0;
  
  for (boost::dynamic_bitset<>::size_type i =0; i < m_idfield.size();i++){
    m_l0buf[j] = m_idfield[i];
    j++;
  }
  j=32;
  for (boost::dynamic_bitset<>::size_type i =0; i < m_outfield.size();i++){
    m_l0buf[j]= m_outfield[i];
    j++;
    
  }

  j=80;  
  for (boost::dynamic_bitset<>::size_type i =0; i < m_inpfield.size();i++){
    m_l0buf[j] = m_inpfield[i];
    j++;
    
 }    


  char buf[4096];
  char * name = "L0Buffer";
  sprintf(buf,name);
  int nbits = m_l0buf.size();
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();
  TileRegister* pL0buf= rfactory->createTileRegister(buf,nbits);
  pL0buf->set(m_l0buf);  
  addOutputRegister(pL0buf);
}




void L0Muon::BuildL0BufferUnit::initialize(){
}

void L0Muon::BuildL0BufferUnit::execute(){
  setIdField();
  setInputField();
  setOutputField();
  getIdField();
  getInputField();
  getOutputField();
  setL0buf();
}

void L0Muon::BuildL0BufferUnit::finalize(){
}
