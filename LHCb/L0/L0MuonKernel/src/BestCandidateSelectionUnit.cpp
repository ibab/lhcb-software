#include "L0MuonKernel/BestCandidateSelectionUnit.h"

L0Muon::BestCandidateSelectionUnit::BestCandidateSelectionUnit(){
}

L0Muon::BestCandidateSelectionUnit::~BestCandidateSelectionUnit(){}


void L0Muon::BestCandidateSelectionUnit::calculateBestPt()
{
  long unsigned int Ptvalue1=0 ;
  long unsigned int Ptvalue2=0 ;
  std::map<std::string,Register*>::iterator ir;
  std::map<std::string,Register*>::iterator jr;
  for (ir = m_inputs.begin(); ir!= m_inputs.end(); ir++){
    
    Register * itr = dynamic_cast<Register*>(ir->second);
    if (itr->Type()=="OutputfieldPt"){
      boost::dynamic_bitset<> Pt1 = itr->getBitset();
      boost::dynamic_bitset<> uPt1(8);
      for (boost::dynamic_bitset<>::size_type i =1; i< Pt1.size(); i++){
        uPt1[i] = Pt1[i];
      }
      
      Ptvalue1 = uPt1.to_ulong();
      if (jr !=ir){
      for (jr = m_inputs.begin(); jr!= m_inputs.end(); jr++){
      
          Register * itr = dynamic_cast<Register*>(jr->second);
          if (itr->Type()=="OutputfieldPt"){
            boost::dynamic_bitset<> Pt2 = itr->getBitset();
            boost::dynamic_bitset<> uPt2(8);
            for (boost::dynamic_bitset<>::size_type i =1; i< Pt2.size(); i++){
              uPt2[i] = Pt2[i];
            }
            if (Ptvalue1 >= uPt2.to_ulong())
            {
              Ptvalue2 = std:: max (uPt2.to_ulong(),Ptvalue2);
            } else { 

              Ptvalue2 = Ptvalue1;
              Ptvalue1 = uPt2.to_ulong();
            }
            
          }
          
      }
      
      
      }
      
      break;
      
    }
    
    
    
  }
  
  for (ir = m_inputs.begin(); ir!= m_inputs.end(); ir++){
    
    Register * itr = dynamic_cast<Register*>(ir->second);
    if (itr->Type()=="OutputfieldPt"){
      boost::dynamic_bitset<> Pt = itr->getBitset();
      if (Ptvalue1 == Pt.to_ulong() || Ptvalue2 == Pt.to_ulong()){

        addOutputRegister(itr);
        
      }
      
      
    }
    
    
  }
  
}    
    

void L0Muon::BestCandidateSelectionUnit::setOutBCSU()
{
  std::map<std::string,Register*>::iterator ir;
  std::map<std::string,Register*>::iterator jr;
  
  for (ir = m_outputs.begin(); ir!= m_outputs.end(); ir++){
    Register * itr = dynamic_cast<Register*>(ir->second);
    std::string name = itr->name();
      
    char buf[4096];
    char * nm ="(R%c,%c,%c)_addr_candidate%c";
    sprintf(buf, nm ,name[2],name[4],name[6],name[21]);
    
    for (jr = m_inputs.begin(); jr!= m_inputs.end(); jr++){
      Register * jtr = dynamic_cast<Register*>(jr->second);
      if (jtr->name()==buf){
        addOutputRegister(jtr);
        
      }
      
      
    }
    
    
  }
  
}




void L0Muon::BestCandidateSelectionUnit::initialize()
{
}

void L0Muon::BestCandidateSelectionUnit::execute()
{

  calculateBestPt();
  setOutBCSU();
  //dumpRegisters();  
}

void L0Muon::BestCandidateSelectionUnit::finalize()
{
}




