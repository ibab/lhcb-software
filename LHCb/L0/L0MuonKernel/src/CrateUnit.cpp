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
  

  //for (ib= boards.begin(); ib != boards.end(); ib++){
  //}

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
  //sortOffsets(m_log);
  
  
  
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




void L0Muon::CrateUnit::sortCandidates(MsgStream * log)
{
   std::vector<L0MuonCandidate*>::iterator ilmc;

 std::vector< std::pair<L0MuonCandidate*, std::vector<int> > >::iterator ioff;



 // Sort candidates if the status is OK

 //===============================================================
 
 //std::sort(m_candidates.begin(),m_candidates.end(),ComparePt());  

 //std::vector< std::pair<L0MuonCandidate*, std::vector<int> > > tmp; 
 //tmp =m_offsets;
  
 //for (int i =0; i< m_candidates.size(); i++){

 //  if ((m_offsets[i].first)->pt() != (m_candidates[i])->pt() ||  
 //   (m_offsets[i].first)->theta() != (m_candidates[i])->theta() ||
 //    (m_offsets[i].first)->phi() != (m_candidates[i])->phi()  )
 //{
       


 //    for (int j =0; j<m_offsets.size();j++){
 //    
 //    if ((m_offsets[j].first)->pt()== (m_candidates[i])->pt() &&  
 //        (m_offsets[j].first)->theta()== (m_candidates[i])->theta() &&
 //        (m_offsets[j].first)->phi()== (m_candidates[i])->phi() 
 //        )
 //    {
 //      tmp[i].first = m_offsets[j].first;
 //      tmp[i].second = m_offsets[j].second;
 //    }
       
 //  }
     
 //  } else {
 //    tmp[i].first = m_offsets[i].first;
 //    tmp[i].second = m_offsets[i].second;
 //  }
   
   
 //}
 
  

 // m_offsets.clear();
 //m_offsets = tmp;
  
 //for (int k=0; k<tmp.size(); k++){
 //m_offsets[k].first=tmp[k].first;
 //m_offsets[k].second=tmp[k].second;
 //}
 
 
 //============================== 

 if(m_status == L0Muon::OK) {
   *log << MSG::DEBUG << "candidates entering into Crate"<< " "  
        << m_candidates.size()<< endreq ;
   for (ilmc = m_candidates.begin();ilmc != m_candidates.end(); ilmc++){
     *log << MSG::DEBUG << "Pt of the candidate = " << (*ilmc)->pt() 
          << endreq;   
     //*log << MSG::DEBUG << "Theta of the candidate = " << (*ilmc)->theta() 
     //   << endreq;  
     //*log << MSG::DEBUG	<< "Phi of the candidate = " << (*ilmc)->phi() 
     //   << endreq;  
   }
   

   *log << MSG::DEBUG << "Candidates and offsets entering into  Crate" 
        << " " << m_offsets.size() << endreq ;
   for (ioff = m_offsets.begin();ioff != m_offsets.end();ioff++){
     std::vector<int> tmp =(*ioff).second;
     *log << MSG::DEBUG << "Pt of the candidate = " << (*ioff).first->pt() 
          << endreq;      
     *log << MSG::DEBUG << "Offsets = " << tmp[0] << endreq;
     
   }
       
       
       
     
     
     

   if (m_candidates.size() > 2) {   
     for (ilmc = m_candidates.end()-1;ilmc != m_candidates.begin()+1; 
          ilmc--) {
         
        ////////////////////////////
 
     ////////////////////////////////////  


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



   ///////////////
 if (m_offsets.size() > 2) {
   for (ioff = m_offsets.end()-1;ioff != m_offsets.begin()+1; 
        ioff--) {
     //delete (*ioff).first;
     m_offsets.erase(ioff);
   }
 } else if(m_offsets.size() == 1) { 
   std::vector<int> tmp;
   for (int iv =0; iv<10; iv++){       
     tmp.push_back(0);
   }
      
   std::pair<L0MuonCandidate* , std::vector<int> > empty = 
     std::make_pair(new L0MuonCandidate(L0Muon::PU_EMPTY), tmp);
   m_offsets.push_back(empty);
 } else if(m_offsets.size() == 0){
   std::vector<int> tmp;
   for (int iv =0; iv<10; iv++){
       
     tmp.push_back(0);
   }
          
   std::pair<L0MuonCandidate* , std::vector<int> > empty = 
     std::make_pair(new L0MuonCandidate(L0Muon::PU_EMPTY), tmp);
   m_offsets.push_back(empty);
   m_offsets.push_back(empty);
 }
 

 
   
   
 std::sort(m_candidates.begin(),m_candidates.end(),ComparePt());  
 //===============================================================
  
 std::sort(m_candidates.begin(),m_candidates.end(),ComparePt());  

 std::vector< std::pair<L0MuonCandidate*, std::vector<int> > > tmp; 
 tmp =m_offsets;
  
 for (unsigned int i =0; i< m_candidates.size(); i++){

   if ((m_offsets[i].first)->pt() != (m_candidates[i])->pt() ||  
       (m_offsets[i].first)->theta() != (m_candidates[i])->theta() ||
       (m_offsets[i].first)->phi() != (m_candidates[i])->phi()  )
   {
       


     for (unsigned int j =0; j<m_offsets.size();j++){
    
       if ((m_offsets[j].first)->pt()== (m_candidates[i])->pt() &&  
           (m_offsets[j].first)->theta()== (m_candidates[i])->theta() &&
           (m_offsets[j].first)->phi()== (m_candidates[i])->phi() 
           )
       {
         tmp[i].first = m_offsets[j].first;
         tmp[i].second = m_offsets[j].second;
       }
       
     }
     
     } else {
       tmp[i].first = m_offsets[i].first;
       tmp[i].second = m_offsets[i].second;
     }
   
   
 }
 
  

 m_offsets.clear();
 m_offsets = tmp;
  
 for (unsigned int k=0; k<tmp.size(); k++){
   m_offsets[k].first=tmp[k].first;
   m_offsets[k].second=tmp[k].second;
 }

 //===================================================


 *log << MSG::DEBUG << "Candidates sorting from Crate" << endreq ;
   for (ilmc = m_candidates.begin();ilmc != m_candidates.end();ilmc++){
       //*log << MSG::DEBUG << "Pt of the candidate = " << (*ilmc)->pt() 
       //  << endreq;   
       //*log << MSG::DEBUG << "Theta of the candidate = " << (*ilmc)->theta() 
       //  << endreq;  
       //*log << MSG::DEBUG	<< "Phi of the candidate = " << (*ilmc)->phi() 
       //  << endreq;  
       //*log << MSG::DEBUG << "Status of the candidate = " << (*ilmc)->status()
       //  << endreq;

     //*log << MSG::INFO << "Pt of the candidate = " << (*ilmc)->pt() 
     //   << endreq;   
     //*log << MSG::INFO << "Theta of the candidate = " << (*ilmc)->theta() 
     //   << endreq;  
     //*log << MSG::INFO	<< "Phi of the candidate = " << (*ilmc)->phi() 
     //   << endreq;  
              
     
   }
    *log << MSG::DEBUG << "Candidates and offsets sorting from Crate" << endreq ;
     for (ioff = m_offsets.begin();ioff != m_offsets.end();ioff++){
        std::vector<int> tmp =(*ioff).second;
       *log << MSG::DEBUG << "Pt of the candidate = " << (*ioff).first->pt() 
           << endreq;   
       *log << MSG::DEBUG << "Offsets = " << tmp[0] << endreq;
       
     }
     
   
}


void L0Muon::CrateUnit::sortOffsets(MsgStream * log)
{
 std::vector< std::pair<L0MuonCandidate*, std::vector<int> > >::iterator ioff;

 if(m_status == L0Muon::OK) {
   *log << MSG::DEBUG << "offset size"<< " "  << m_offsets.size()<< endreq ;
   *log << MSG::DEBUG << "Candidates and offsets entering into  Crate" << endreq ;
     for (ioff = m_offsets.begin();ioff != m_offsets.end();ioff++){
        std::vector<int> tmp =(*ioff).second;
       *log << MSG::DEBUG << "Pt of the candidate = " << (*ioff).first->pt() 
           << endreq;      
       *log << MSG::DEBUG << "Offsets = " << tmp[0] << endreq;
       
     }
     



   if (m_offsets.size() > 2) {
       for (ioff = m_offsets.end()-1;ioff != m_offsets.begin()+1; 
            ioff--) {
         //delete (*ioff).first;
         m_offsets.erase(ioff);
       }
    } else if(m_offsets.size() == 1) { 
      std::vector<int> tmp;
      for (int iv =0; iv<10; iv++){       
        tmp.push_back(0);
      }
      
      std::pair<L0MuonCandidate* , std::vector<int> > empty = 
        std::make_pair(new L0MuonCandidate(L0Muon::PU_EMPTY), tmp);
       m_offsets.push_back(empty);
        } else if(m_offsets.size() == 0){
          std::vector<int> tmp;
          for (int iv =0; iv<10; iv++){
            
            tmp.push_back(0);
          }
          
          std::pair<L0MuonCandidate* , std::vector<int> > empty = 
            std::make_pair(new L0MuonCandidate(L0Muon::PU_EMPTY), tmp);
          m_offsets.push_back(empty);
          m_offsets.push_back(empty);
        }
   
 }
 
 
 
     *log << MSG::DEBUG << "Candidates and offsets sorting from Crate" << endreq ;
     for (ioff = m_offsets.begin();ioff != m_offsets.end();ioff++){
        std::vector<int> tmp =(*ioff).second;
       *log << MSG::DEBUG << "Pt of the candidate = " << (*ioff).first->pt() 
           << endreq;   
       *log << MSG::DEBUG << "Offsets = " << tmp[0] << endreq;
       
     }
     
     
     
 }




std::vector<std::pair<L0MuonCandidate*, std::vector<int> > > 
L0Muon::CrateUnit::offsets() 
{
  

  for (std::vector<std::pair<L0MuonCandidate*, std::vector<int> > >::
         iterator ioff=m_offsets.begin(); ioff != m_offsets.end();
       ioff++){

    
    std::vector<int> offs = (*ioff).second;
    
   
  }
  
  
  return m_offsets;
  
  
}




 

