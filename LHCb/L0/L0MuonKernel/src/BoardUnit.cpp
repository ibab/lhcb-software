#include "L0MuonKernel/BoardUnit.h"
#include "L0MuonKernel/CrateUnit.h"

L0Muon::BoardUnit::BoardUnit(){

  
}


L0Muon::BoardUnit::~BoardUnit() {}

void L0Muon::BoardUnit::initialize(){
  L0Muon::Unit::initialize();
}


void L0Muon::BoardUnit::initialize(MsgStream & log){

 if ( ! m_units.empty() ) {
     std::map<std::string,L0Muon::Unit*>::iterator iu;
     for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
       (*iu).second->initialize(log);

     }

 } else {
   return ;
   
 }
 

}

void L0Muon::BoardUnit::execute(){
  L0Muon::Unit::execute();
}

void L0Muon::BoardUnit::execute(MsgStream & log){
 if ( ! m_units.empty() ) {

   std::map<std::string,L0Muon::Unit*>::iterator iu;
     for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
       (*iu).second->execute(log);
     }
 }
}


void L0Muon::BoardUnit::finalize(){
 if ( ! m_units.empty() ) {
     std::map<std::string,L0Muon::Unit*>::iterator iu;
     for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
       (*iu).second->finalize();
     }
 } else {
   return ;
   
 }
}

