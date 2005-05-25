#include <utility>  // For std::pair
#include <algorithm> 
#include "L0MuonKernel/CrateUnit.h"
#include "L0MuonKernel/BCSUnit.h"

L0Muon::CrateUnit::CrateUnit(PL0MProNet & pProNet,std::vector<MuonTileID> config_pus )
{
  

  if ( ! m_units.empty() ) {
    m_units.clear();
  }

  
   
   std::vector<MuonTileID> boards = pProNet->boards();
   std::vector<MuonTileID>::iterator ib;
   std::vector<MuonTileID>::iterator ipusb;

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
     } else {
       ok = true;
     } 
     
     // We do not need this board  
     if ( ! ok ) continue; 
        
     std::vector<MuonTileID> pusb = pProNet->pusInBoard(*ib);

     BoardUnit * board = new BoardUnit();
     board->setParent(this);
     addUnit(board);

     BCSUnit * bcsu = new BCSUnit(*ib);
     bcsu->setParent(board);     
     board->addUnit(bcsu, "bcsu");
     
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
       } else {
         ok = true;
       }	 
	        
       if ( ok ) {
	 L0MPuNodeBase m_pPuNode = pProNet->puNode(*ipusb);
	 Unit * pProcUnit = new ProcUnit(m_pPuNode);

	 pProcUnit->setParent(board);
	 board->addUnit(pProcUnit);
       }	 
     }
   }  
}

L0Muon::CrateUnit::~CrateUnit(){
}

void L0Muon::CrateUnit::initialize() {
  L0Muon::Unit::initialize();
}

void L0Muon::CrateUnit::preexecute() {
  L0Muon::Unit::preexecute();
}

void L0Muon::CrateUnit::execute()
{
  m_status = L0MuonStatus::OK ;
  L0Muon::Unit::execute();
  sortCandidates();  
}

void L0Muon::CrateUnit::postexecute(){ 
  L0Muon::Unit::postexecute();
  m_candidates.clear();
  m_offsets.clear(); 
}


void L0Muon::CrateUnit::fillCandidates(PCandidate cand){
  m_candidates.push_back(cand);
}

void L0Muon::CrateUnit::fillOffset(std::pair<PCandidate, 
                                   std::vector<int> > off){
  m_offsets.push_back(off);
}

void L0Muon::CrateUnit::sortCandidates(){

   //m_debug = true;

   if (m_debug) std::cout << "\nCrate: " << "# of candidates : " 
                          << m_candidates.size() << std::endl;

   std::sort(m_candidates.begin(),m_candidates.end(),ComparePt());  
   //std::sort(m_offsets.begin(),m_offsets.end(),ComparePt());

   std::vector<PCandidate >::iterator ilmc ;
   std::vector< std::pair<PCandidate, std::vector<int> > >::iterator ioff; 
 
 
   // Sort Candidates if the status is OK
   
   if( m_status == L0MuonStatus::OK) {
   
     if (m_debug) {
       int sss = m_offsets.size();
       std::cout << "Crate: Candidates and offsets entering into Crate: -----" 
                 << sss << "----" << std::endl ;
       for (ioff = m_offsets.begin(); ioff != m_offsets.end(); ioff++){
	 std::vector<int> tmp =(*ioff).second;

	 std::cout << "Crate: Pt of the candidate = " << (*ioff).first->pt();
	 std::cout << " Crate: Offsets = " << tmp[0] << std::endl;
       }
     }
   
     //std::cout << "CrateUnit: Sorting candidates: " << m_candidates.size() << std::endl;
     int count=0;
     if (m_candidates.size() > 2) {   
       for (ilmc = m_candidates.end()-1;ilmc != m_candidates.begin()+1; 
            ilmc--) {

         //std::cout << "Erasing candidate" << std::endl;
	 m_candidates.erase(ilmc);
	 count++;
       }
     } 
     //std::cout << count << " candidates erased " << std::endl;
//      else if ( m_candidates.size() == 1) { 
//        PCandidate pcand(new Candidate(L0MuonStatus::PU_EMPTY));
//        m_candidates.push_back(pcand);
// 
//      } else if ( m_candidates.size() == 0){
//        PCandidate pcand(new Candidate(L0MuonStatus::PU_EMPTY));      
//        m_candidates.push_back(pcand);
//        m_candidates.push_back(pcand);
//      }

     //std::cout << "CrateUnit: Done checking candidates" << std::endl;

     // Sort offsets

//      if (m_offsets.size() > 2) {
//        for (ioff = m_offsets.end()-1;ioff != m_offsets.begin()+1; 
//             ioff--) {
// 
// 	 m_offsets.erase(ioff);
// 
//        }
// 
//      } 
//      else if(m_offsets.size() == 1) { 
//        std::vector<int> tmp;
//        for (int iv =0; iv<10; iv++){       
// 	 tmp.push_back(0);
//        }
// 
//        PCandidate pcand(new Candidate(L0MuonStatus::PU_EMPTY));              
//        std::pair<PCandidate , std::vector<int> > empty = 
// 	 std::make_pair(pcand, tmp);
//        m_offsets.push_back(empty);
// 
//      } 
//      else if(m_offsets.size() == 0){
//        std::vector<int> tmp;
//        for (int iv =0; iv<10; iv++){
// 
// 	 tmp.push_back(0);
//        }
// 
//        PCandidate pcand(new Candidate(L0MuonStatus::PU_EMPTY));
//        std::pair<PCandidate , std::vector<int> > empty = 
// 	 std::make_pair(pcand, tmp);
//        m_offsets.push_back(empty);
//        m_offsets.push_back(empty);
//      }
 
     //std::cout << "CrateUnit: Done Sort offsets" << std::endl; 
   
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

int L0Muon::CrateUnit::xFoi(int sta)
{
  int xfoi= m_xfoi[sta];
  return xfoi;
  
}

int L0Muon::CrateUnit::yFoi(int sta)
{
  int yfoi= m_yfoi[sta];
  return yfoi;
  
}
