#include <utility>  // For std::pair
#include "L0MuonKernel/CrateUnit.h"
#include "L0MuonKernel/BestCandidateSelectionUnit.h"

L0Muon::CrateUnit::CrateUnit(PL0MProNet & pProNet, 
                             std::vector<double> & ptpara,
                             bool & ignoreM1,
                             std::vector<int> & foix, 
                             std::vector<int> & foiy,
                             double & precision,
                             int & bits,
                             IMuonTileXYZTool * iTileXYZTool,
                             bool & writeL0buffer,
                             MsgStream & log )
{



  

  if ( ! m_units.empty() ) {
    m_units.clear();
  }

   m_iTileXYZTool = iTileXYZTool;
   
   std::vector<MuonTileID> boards = pProNet->boards();
   std::vector<MuonTileID>::iterator ib;
   std::vector<MuonTileID>::iterator ipusb;


   for (ib= boards.begin(); ib != boards.end(); ib++){
    
     std::vector<MuonTileID> pusb = pProNet->pusInBoard(*ib);

     BoardUnit * board = new BoardUnit();
     BestCandidateSelectionUnit * bcsu = new BestCandidateSelectionUnit(writeL0buffer);
     
     
     for ( ipusb = pusb.begin(); ipusb != pusb.end(); ipusb++ ){
       L0MPuNodeBase m_pPuNode = pProNet->puNode(*ipusb);
     
       Unit * pProcUnit = new L0mProcUnit(m_pPuNode,ptpara,ignoreM1,foix,foiy,
                                         precision, bits, writeL0buffer, log);

       pProcUnit->setParent(board);
       board->addUnit(pProcUnit);
     }


     
     bcsu->setParent(board);     

     board->addUnit(bcsu, "bcsu");
     if ( writeL0buffer) {
       bcsu->setOutputFile(*ib);
     }
     

     board->setParent(this);

     addUnit(board);


     


   }
   
}





L0Muon::CrateUnit::~CrateUnit()
{
}

void L0Muon::CrateUnit::initialize()
{
  L0Muon::Unit::initialize();
}

void L0Muon::CrateUnit::initialize(MsgStream & log)
{
 
  if ( ! m_units.empty() ) {
    std::map<std::string,L0Muon::Unit*>::iterator iu;
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      (*iu).second->initialize(log);

   }
    
  } else {
    return ;
  }


}



void L0Muon::CrateUnit::execute()
{


  L0Muon::Unit::execute();
  //sortCandidates();

  
}


void L0Muon::CrateUnit::execute(MsgStream & log)
{


  if ( ! m_units.empty() ){
 
    
    std::map<std::string,L0Muon::Unit*>::iterator iu;
        
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      (*iu).second->execute(log);
    }
  }
  sortCandidates(log);

  
}


void L0Muon::CrateUnit::finalize()
{ 
  if ( ! m_units.empty() ) {
  std::map<std::string,L0Muon::Unit*>::iterator iu;
  for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {    
      (*iu).second->finalize();
  }

  m_candidates.clear();
  m_offsets.clear();
  
  }
  else {
    return ;
    
  }
  
  
  
}


void L0Muon::CrateUnit::fillCandidates(L0MuonCandidate * cand)
{
  m_candidates.push_back(cand);
}

void L0Muon::CrateUnit::fillOffset(std::pair<L0MuonCandidate*, 
                                   std::vector<int> > off)
{
  m_offsets.push_back(off);
}




void L0Muon::CrateUnit::sortCandidates(MsgStream & log)
{


   std::sort(m_candidates.begin(),m_candidates.end(),ComparePt());  
   std::sort(m_offsets.begin(),m_offsets.end(),ComparePt());

   std::vector<L0MuonCandidate*>::iterator ilmc ;
   std::vector< std::pair<L0MuonCandidate*, std::vector<int> > >::iterator ioff; 
 
 
   // Sort Candidates if the status is OK
   
   if( m_status == L0MuonStatus::OK) {
     log << MSG::DEBUG << "candidates into Crate:"<< endreq ;
      
      for (ilmc = m_candidates.begin();ilmc != m_candidates.end(); ilmc++){
        log << MSG::DEBUG << "Pt of the candidate = " << (*ilmc)->pt() 
            << endreq;
      }
   
   
     log << MSG::DEBUG << "Candidates and offsets entering into  Crate" 
         << " " << m_offsets.size() << endreq ;
     for (ioff = m_offsets.begin(); ioff != m_offsets.end(); ioff++){
       std::vector<int> tmp =(*ioff).second;
       
       log << MSG::DEBUG << "Pt of the candidate = " << (*ioff).first->pt() 
          << endreq;
       log << MSG::DEBUG << "Offsets = " << tmp[0] << endreq;
     
     }
   
     

     if (m_candidates.size() > 2) {   
       for (ilmc = m_candidates.end()-1;ilmc != m_candidates.begin()+1; 
            ilmc--) {

         delete *ilmc;
         m_candidates.erase(ilmc);


       }

     } else if ( m_candidates.size() == 1) { 
       m_candidates.push_back(new L0MuonCandidate(L0MuonStatus::PU_EMPTY));

     } else if ( m_candidates.size() == 0){
       m_candidates.push_back(new L0MuonCandidate(L0MuonStatus::PU_EMPTY));
       m_candidates.push_back(new L0MuonCandidate(L0MuonStatus::PU_EMPTY));
     }


     
     
   
   
   
   


   // Sort offsets

   if (m_offsets.size() > 2) {
     for (ioff = m_offsets.end()-1;ioff != m_offsets.begin()+1; 
          ioff--) {

       m_offsets.erase(ioff);
       
     }
     
   } else if(m_offsets.size() == 1) { 
     std::vector<int> tmp;
     for (int iv =0; iv<10; iv++){       
       tmp.push_back(0);
     }
      
     std::pair<L0MuonCandidate* , std::vector<int> > empty = 
       std::make_pair(new L0MuonCandidate(L0MuonStatus::PU_EMPTY), tmp);
     m_offsets.push_back(empty);
 
   } else if(m_offsets.size() == 0){
     std::vector<int> tmp;
     for (int iv =0; iv<10; iv++){
       
       tmp.push_back(0);
     }
          
     std::pair<L0MuonCandidate* , std::vector<int> > empty = 
       std::make_pair(new L0MuonCandidate(L0MuonStatus::PU_EMPTY), tmp);
     m_offsets.push_back(empty);
     m_offsets.push_back(empty);
   }
 
    
   
   }
   

   // Printout for debug   

   log << MSG::DEBUG << "Candidates sorting from Crate" << endreq ;
   for (ilmc = m_candidates.begin();ilmc != m_candidates.end();ilmc++){
     log << MSG::DEBUG << "Pt of the candidate = " << (*ilmc)->pt() 
         << endreq;
   }
   
   log << MSG::DEBUG << "Candidates and offsets sorting from Crate" 
       << endreq ;
   for (ioff = m_offsets.begin();ioff != m_offsets.end();ioff++){
     std::vector<int> tmp =(*ioff).second;
     log << MSG::DEBUG << "Pt of the candidate = " << (*ioff).first->pt() 
         << endreq;   
     log << MSG::DEBUG << "Offsets = " << tmp[0] << endreq;
     /*std::cout << "Offsets = " << tmp[0] << std::endl;
     std::cout << "Offsets = " << tmp[1] << std::endl;
     std::cout << "Offsets = " << tmp[2] << std::endl;
     std::cout << "Offsets = " << tmp[3] << std::endl;
     std::cout << "Offsets = " << tmp[4] << std::endl;
     std::cout << "Offsets = " << tmp[5] << std::endl;
     std::cout << "Offsets = " << tmp[6] << std::endl;
     std::cout << "Offsets = " << tmp[7] << std::endl;
     std::cout << "Offsets = " << tmp[8] << std::endl;
     std::cout << "Offsets = " << tmp[9] << std::endl;*/

     
   }
   
    
     
     
   
}









 

