
#include "L0MuonKernel/BuildL0BufferUnit.h"
#include "L0MuonKernel/L0mProcUnit.h"
#include <cmath>

L0Muon::BuildL0BufferUnit::BuildL0BufferUnit(){
  
   
  boost::dynamic_bitset<> tmpl0(512);
  m_l0buf = tmpl0;

  boost::dynamic_bitset<> tmpoutfield(48);
  m_outfield = tmpoutfield ;
  
  boost::dynamic_bitset<> tmpinpfield(432);
  m_inpfield = tmpinpfield ;
 
  boost::dynamic_bitset<> tmpidfield(32);
  m_idfield = tmpidfield ; 

  m_l0bufferFile = NULL;

  m_l0EventNumber=0;

  
};

void L0Muon::BuildL0BufferUnit::setOutputFile(MuonTileID puid){
  char * name = "/afs/cern.ch/user/l/ltocco/scratch0/L0Buffers/l0buf_PUQ%dR%d%d%d.txt";
  char buf[4096];
  sprintf(buf,name,puid.quarter()+1,puid.region()+1,puid.nX(),puid.nY());
  m_l0bufferFile = fopen(buf,"w");
}

L0Muon::BuildL0BufferUnit::~BuildL0BufferUnit(){
  if (m_l0bufferFile!=NULL) fclose(m_l0bufferFile);
}

void L0Muon::BuildL0BufferUnit::setIdField(){

  m_idfield.resize(0);

  boost::dynamic_bitset<> l0EventNumber(12,m_l0EventNumber);
  boost::dynamic_bitset<> l0BId(12,0ul);
  
  for (boost::dynamic_bitset<>::size_type i=0; i<l0EventNumber.size();i++){
    bool val = l0EventNumber[i];
    m_idfield.push_back(val) ;
  }

  m_idfield.push_back(0) ; 
  m_idfield.push_back(0) ; 
  m_idfield.push_back(0) ; 
  m_idfield.push_back(1) ; 

  for (boost::dynamic_bitset<>::size_type i=0; i<l0BId.size();i++){
    //bool val = l0EventNumber[i];
    bool val = l0BId[i];
    m_idfield.push_back(val) ;
  }


  
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
        }else {         
	  
	}
        
      }      
    }
    
  }
 
  

  int countOL=0;

  for (ir = m_inputs.begin(); ir!= m_inputs.end(); ir++){
    TileRegister * itr = dynamic_cast<TileRegister*>(ir->second);

    if (itr->Type()== "InputfieldOL"){

      std::string olname  = ir->first;
      std::string refname = "OL_2_Qq_(R1,x,y)_i";
      if(  olname[3]==refname[3] && olname[10]==refname[10] ) continue;
          
      j=240+countOL*32;

      boost::dynamic_bitset<> ifield = itr->getBitset();

      for (boost::dynamic_bitset<>::size_type i=0; i<ifield.size();i++){ 
        if (j < 432){            
          m_inpfield[j]=ifield[i];
          j++;
        } else {          
          return;
          
        }
      }
     
      countOL++;

    }
    
  }  

}


void L0Muon::BuildL0BufferUnit::setOutputField()
{
  

  std::map<std::string,Register*>::iterator ir ;

  boost::dynamic_bitset<>::size_type j = 0;
  for (ir = m_inputs.begin(); ir!= m_inputs.end(); ir++){
    TileRegister * itr = dynamic_cast<TileRegister*>(ir->second);

    if (itr->Type()== "Outputfield"){  
      boost::dynamic_bitset<> ifield = itr->getBitset();
      for (boost::dynamic_bitset<>::size_type i=0; i<ifield.size();i++){
        m_outfield[j]=ifield[i];
        j++;
      }
      m_outfield[j]=0;
      j++;
    }
  }


}

boost::dynamic_bitset<> L0Muon::BuildL0BufferUnit::getIdField(){

  return m_idfield;
}

boost::dynamic_bitset<> L0Muon::BuildL0BufferUnit::getInputField(){
  return m_inpfield;
}


boost::dynamic_bitset<> L0Muon::BuildL0BufferUnit::getOutputField(){
  return m_outfield;
}

void L0Muon::BuildL0BufferUnit::setL0buf(){

  boost::dynamic_bitset<>::size_type j ;

  j=0;  
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

  if (m_debug) std::cout << "Initialize BuildL0Buffer" << std::endl;  

}


void L0Muon::BuildL0BufferUnit::execute(){
  
  if (m_debug) std::cout << "Write L0Buffer" << std::endl;
  
  setIdField();
  setInputField();
  setOutputField();
  setL0buf();

  

  if (m_l0bufferFile!=NULL) dump(m_l0bufferFile);


  m_l0EventNumber++;

}

void L0Muon::BuildL0BufferUnit::finalize(){
}

void L0Muon::BuildL0BufferUnit::dump(FILE *l0bufferFile){
  int ib=0;


  for (std::map<std::string,L0Muon::Register*>::iterator ir = m_inputs.begin(); ir != m_inputs.end(); ir++) {

    boost::dynamic_bitset<> bits = (*ir).second->getBitset();
  }
  ib=0;
  
  for (std::map<std::string,L0Muon::Register*>::iterator ir = m_outputs.begin(); ir != m_outputs.end(); ir++) {
    boost::dynamic_bitset<> bits = (*ir).second->getBitset();

    ib=0;
    int iword=0;
    for (boost::dynamic_bitset<>::size_type i =0; i < bits.size();i++){

      int val=bits[i] ;
      if (val>0) {
        iword+= int(pow(2,ib));

      }

      ib++;
      if ((ib%16)==0) {
        ib=0;
        fprintf(l0bufferFile,"%04x\n",iword);
        iword=0;
        i++;
        //if (i != bits.size()) {
          //std::cout <<"<BuildL0BufferUnit::dump>\t\t";
        //}
        i--;
      }
    }
    if(ib!=0) {
      fprintf(l0bufferFile,"!!!\n");
    }
  }
  fprintf(l0bufferFile,"----\n");

  
}

