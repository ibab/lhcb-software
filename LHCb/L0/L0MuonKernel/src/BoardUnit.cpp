#include "L0MuonKernel/BoardUnit.h"
#include "L0MuonKernel/CrateUnit.h"

L0Muon::BoardUnit::BoardUnit(){
}


L0Muon::BoardUnit::~BoardUnit() {}

void L0Muon::BoardUnit::initialize(){

 if ( ! m_units.empty() ) {
     std::map<std::string,L0Muon::Unit*>::iterator iu;
     for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
       (*iu).second->initialize();
     }
 }

}

void L0Muon::BoardUnit::execute(){
 if ( ! m_units.empty() ) {
     std::map<std::string,L0Muon::Unit*>::iterator iu;
     
     CrateUnit * myc = dynamic_cast<CrateUnit*>(m_parent);
     MsgStream * log = myc->getMsgStream();
     *log << MSG::DEBUG <<"execute " << " " << m_units.size() << " " 
            <<"subunits of board" << endreq;
     for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
       (*iu).second->execute();
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

