#include <utility>  // For std::pair
#include "L0MuonKernel/CrateUnit.h"

L0Muon::CrateUnit::CrateUnit(PL0MProNet & pProNet, 
                             std::vector<double> & ptpara,
                             std::vector<int> & foix, 
                             std::vector<int> & foiy,
                             double & precision,
                             int & bits,
                             MsgStream & log)

{

   if ( ! m_units.empty() ) {
    m_units.clear();
   
  }

  std::vector<MuonTileID> boards = pProNet->boards();
  std::vector<MuonTileID>::iterator ib;
  std::vector<MuonTileID>::iterator ipusb;
  

  for (ib= boards.begin(); ib != boards.end(); ib++){
  }

  for (ib= boards.begin(); ib != boards.end(); ib++){
    
    std::vector<MuonTileID> pusb = pProNet->pusInBoard(*ib);
    //std::vector<MuonTileID>::iterator it;

    BoardUnit * board = new BoardUnit();
    
    
    for (ipusb = pusb.begin(); ipusb != pusb.end(); ipusb++){
      
      }

    
    for( ipusb = pusb.begin(); ipusb != pusb.end(); ipusb++ ){
      L0MPuNodeBase m_pPuNode = pProNet->puNode(*ipusb);
      Unit * pProcUnit = new L0mProcUnit(m_pPuNode,ptpara,foix,foiy,
                                         precision, bits,log);
      pProcUnit->setParent(board);
      board->addUnit(pProcUnit);
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
  if ( ! m_units.empty() ) {
    std::map<std::string,L0Muon::Unit*>::iterator iu;
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      (*iu).second->initialize();
    }
    
  } 
  
  
}





void L0Muon::CrateUnit::execute()
{

  if ( ! m_units.empty() ) {
    std::map<std::string,L0Muon::Unit*>::iterator iu;
    *m_log << MSG::DEBUG <<"execute" << " " << m_units.size() <<" " 
          << "subunits of crate" << endreq;
    for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
      (*iu).second->execute();
    }
  }
  
  sortCandidates(m_log);
  
  
  
}


void L0Muon::CrateUnit::finalize()
{ 
  if ( ! m_units.empty() ) {
  std::map<std::string,L0Muon::Unit*>::iterator iu;
  for ( iu = m_units.begin(); iu != m_units.end(); iu++ ) {
    //*m_log << MSG::INFO << " finalize " << (*iu).second << endreq;
      (*iu).second->finalize();
  }
  
  m_candidates.clear();
  
  

  
  }
  else {
    return ;
    
  }
  
  
  
}


void L0Muon::CrateUnit::fillCandidates(L0MuonCandidate * cand)
{
  m_candidates.push_back(cand);
}


void L0Muon::CrateUnit::sortCandidates(MsgStream * log)
{
   std::vector<L0MuonCandidate*>::iterator ilmc;
 // Sort candidates if the status is OK

   if(m_status == L0Muon::OK) {
    if (m_candidates.size() > 2) {
       for (ilmc = m_candidates.end()-1;ilmc != m_candidates.begin()+1; 
            ilmc--) {
         delete *ilmc;
         m_candidates.erase(ilmc);
       }
    } else if(m_candidates.size() == 1) { 
       m_candidates.push_back(new L0MuonCandidate(L0Muon::PU_EMPTY));
        } else if(m_candidates.size() == 0){
           m_candidates.push_back(new L0MuonCandidate(PU_EMPTY));
           m_candidates.push_back(new L0MuonCandidate(PU_EMPTY));
        }
   }
   
   std::sort(m_candidates.begin(),m_candidates.end(),ComparePt());  
   *log << MSG::DEBUG << "Candidates sorting from Crate" << endreq ;
     for (ilmc = m_candidates.begin();ilmc != m_candidates.end();ilmc++){
       *log << MSG::DEBUG << "Pt of the candidate = " << (*ilmc)->pt() 
           << endreq;   
       *log << MSG::DEBUG << "Theta of the candidate = " << (*ilmc)->theta() 
           << endreq;  
       *log << MSG::DEBUG	<< "Phi of the candidate = " << (*ilmc)->phi() 
           << endreq;  
       *log << MSG::DEBUG << "Status of the candidate = " << (*ilmc)->status()
           << endreq;
       
       
     }
     
     
}

