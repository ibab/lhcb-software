#include <utility>  // For std::pair
#include <algorithm> 
#include "L0MuonKernel/CrateUnit.h"
#include "L0MuonKernel/BestCandidateSelectionUnit.h"

L0Muon::CrateUnit::CrateUnit(PL0MProNet & pProNet, 
                             std::vector<double> & ptpara,
                             bool & ignoreM1,
                             std::vector<int> & foix, 
                             std::vector<int> & foiy,
                             double & precision,
                             int & bits,
                             std::string writeL0buffer,
			     std::vector<MuonTileID> config_pus )
{



  

  if ( ! m_units.empty() ) {
    m_units.clear();
  }
   
   std::vector<MuonTileID> boards = pProNet->boards();
   std::vector<MuonTileID>::iterator ib;
   std::vector<MuonTileID>::iterator ipusb;

   // std::cout << "Crate constructor " <<  std::endl;
//    for ( int i = 0; i<5 ; i++ ) {
//      std::cout << "Foi X in station " << i << " " << foix[i] <<  std::endl;
//    }

   bool ok;

   for (ib= boards.begin(); ib != boards.end(); ib++){
    
     ok = true;
     // Check that the board contains at least one of the requested PU's if
     // the list of PU's is given explicitely
     if (config_pus.size() > 0) {
       ok = false;
       std::vector<MuonTileID>::iterator ipu;
       for ( ipu = config_pus.begin(); ipu != config_pus.end(); ipu++ ) {
         MuonTileID good_board = ipu->containerID(MuonLayout(1,1));
         if ( (*ib) == good_board ) {
	   ok = true;
	   break;
	 }
       }
     }  
     
     // We do not need this board  
     if ( ! ok ) continue; 
        
     std::vector<MuonTileID> pusb = pProNet->pusInBoard(*ib);

     BoardUnit * board = new BoardUnit();
     BestCandidateSelectionUnit * bcsu = new BestCandidateSelectionUnit(*ib);
     
     
     for ( ipusb = pusb.begin(); ipusb != pusb.end(); ipusb++ ){
            
       ok = true;
       // Check that this PU was requested
       if (config_pus.size() > 0) {
         ok = false;
	 std::vector<MuonTileID>::iterator ipu = std::find(config_pus.begin(),
	                                                   config_pus.end(),
							   (*ipusb));
	 if ( ipu != config_pus.end() ) {
	   ok = true;
	 }
       }	 
	        
       if ( ok ) {
	 L0MPuNodeBase m_pPuNode = pProNet->puNode(*ipusb);
	 Unit * pProcUnit = new L0mProcUnit(m_pPuNode,ptpara,ignoreM1,foix,foiy,
                                           precision, bits, writeL0buffer);

	 pProcUnit->setParent(board);
	 board->addUnit(pProcUnit);
       }	 
     }
     
     bcsu->setParent(board);     

     board->addUnit(bcsu, "bcsu");
     if ( ! writeL0buffer.empty() ) {
       // Open output file for the L0Buffer
       // bcsu->setOutputFile(writeL0buffer);
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



void L0Muon::CrateUnit::execute()
{

  L0Muon::Unit::execute();
  sortCandidates();

  
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


void L0Muon::CrateUnit::fillCandidates(Candidate * cand)
{
  m_candidates.push_back(cand);
}

void L0Muon::CrateUnit::fillOffset(std::pair<Candidate*, 
                                   std::vector<int> > off)
{
  m_offsets.push_back(off);
}




void L0Muon::CrateUnit::sortCandidates()
{

   if (m_debug) std::cout << "\nCrate: " << "# of candidates : " 
                          << m_candidates.size() << std::endl;

   std::sort(m_candidates.begin(),m_candidates.end(),ComparePt());  
   std::sort(m_offsets.begin(),m_offsets.end(),ComparePt());

   std::vector<Candidate*>::iterator ilmc ;
   std::vector< std::pair<Candidate*, std::vector<int> > >::iterator ioff; 
 
 
   // Sort Candidates if the status is OK
   
   if( m_status == L0MuonStatus::OK) {
   
     if (m_debug) {
       std::cout << "Crate: Candidates and offsets entering into Crate: " 
                 << m_offsets.size() << std::endl ;
       for (ioff = m_offsets.begin(); ioff != m_offsets.end(); ioff++){
	 std::vector<int> tmp =(*ioff).second;

	 std::cout << "Crate: Pt of the candidate = " << (*ioff).first->pt();
	 std::cout << " Crate: Offsets = " << tmp[0] << std::endl;
       }
     }
   
     

     if (m_candidates.size() > 2) {   
       for (ilmc = m_candidates.end()-1;ilmc != m_candidates.begin()+1; 
            ilmc--) {

         delete *ilmc;
         m_candidates.erase(ilmc);


       }

     } else if ( m_candidates.size() == 1) { 
       m_candidates.push_back(new Candidate(L0MuonStatus::PU_EMPTY));

     } else if ( m_candidates.size() == 0){
       m_candidates.push_back(new Candidate(L0MuonStatus::PU_EMPTY));
       m_candidates.push_back(new Candidate(L0MuonStatus::PU_EMPTY));
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
      
     std::pair<Candidate* , std::vector<int> > empty = 
       std::make_pair(new Candidate(L0MuonStatus::PU_EMPTY), tmp);
     m_offsets.push_back(empty);
 
   } else if(m_offsets.size() == 0){
     std::vector<int> tmp;
     for (int iv =0; iv<10; iv++){
       
       tmp.push_back(0);
     }
          
     std::pair<Candidate* , std::vector<int> > empty = 
       std::make_pair(new Candidate(L0MuonStatus::PU_EMPTY), tmp);
     m_offsets.push_back(empty);
     m_offsets.push_back(empty);
   }
 
    
   
   }
   

   // Printout for debug   

   if (m_debug) {
     std::cout << "Crate: " << "Candidates and offsets sorting from Crate:" 
	       << std::endl ;
     for (ioff = m_offsets.begin();ioff != m_offsets.end();ioff++){
       std::vector<int> tmp =(*ioff).second;
       std::cout << "Crate: " << "Pt of the candidate = " << (*ioff).first->pt(); 
       std::cout << " Crate: " << "Offsets = " << tmp[0] << std::endl;
       /*if (m_debug) std::cout << "Offsets = " << tmp[0] << std::endl;
       if (m_debug) std::cout << "Offsets = " << tmp[1] << std::endl;
       if (m_debug) std::cout << "Offsets = " << tmp[2] << std::endl;
       if (m_debug) std::cout << "Offsets = " << tmp[3] << std::endl;
       if (m_debug) std::cout << "Offsets = " << tmp[4] << std::endl;
       if (m_debug) std::cout << "Offsets = " << tmp[5] << std::endl;
       if (m_debug) std::cout << "Offsets = " << tmp[6] << std::endl;
       if (m_debug) std::cout << "Offsets = " << tmp[7] << std::endl;
       if (m_debug) std::cout << "Offsets = " << tmp[8] << std::endl;
       if (m_debug) std::cout << "Offsets = " << tmp[9] << std::endl;*/
     } 
   }  
}









 

