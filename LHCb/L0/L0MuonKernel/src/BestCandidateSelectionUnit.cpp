#include "L0MuonKernel/L0MuonStatus.h"
#include "L0MuonKernel/BestCandidateSelectionUnit.h"
#include "L0MuonKernel/CrateUnit.h"
//#include "Event/L0Muon.h"
#include <stdio.h>
#include <cmath>

/* 3 fields:

  1. Identification field: number of bits 32 (2 words) 
     - bits  0-11: L0Event number
     - bits 12-15: free
     - bits 16-27: L0_B_Id to fill 
     - bits 28-31: free 

  2. Output field: number of bits 48 (3 words)
     - bits  0-15: for pt of candidate 1 & 2.
     - bits 16-31: for addresses of candidate 1 & 2 in M3 
     - bits 32-33: and 40-41 for pu identifier for candidates 1 and 2
     - bits 44-47: for status (0<= number of candidates <=8) 

  3. Input field: number of bits 240 (15 words) 
     - bits  0- 3: BcId of Pu1
     - bits  4- 7: status word of Pu1
     - bits  8-15: first pt of Pu1   
     - bits 16-23: second Pt of Pu1 
     - bits 23-37: first addr (M3+M2+M1) of Pu1-- erratum: bits 23-38
     - bits 38-38: free -- erratum: bit 39
     - bits 39-54: second addr (M3+M2+M1) of Pu1-- erratum: bits 40-54
     - bits 55-55: free
     - ... and so on for Pu2, Pu3, Pu4

  To do :
     - In Identification field I have to add BcId
     - In output field I have to put the absolue address in M1 (to define)

*/


L0Muon::BestCandidateSelectionUnit::BestCandidateSelectionUnit(MuonTileID boardid){
  
  m_id = boardid;
  m_bcsueventnumber = 0;
  m_bcsul0bufferFile =NULL;
  
}

L0Muon::BestCandidateSelectionUnit::~BestCandidateSelectionUnit(){
  if (m_bcsul0bufferFile!=NULL) fclose(m_bcsul0bufferFile);  
}


void L0Muon::BestCandidateSelectionUnit::initialize()
{
  //if (m_debug) std::cout << "Initialize BCSU" << std::endl;
  m_offsets.clear();
  m_candidates.clear();
}

void L0Muon::BestCandidateSelectionUnit::execute()
{

  if (m_debug) std::cout << "Execute BestCandidateSelectionUnit" << std::endl;

  dumpCandidates();
  //dumpAddresses(log);

  // Sort ordered candidates and offsets
  sortCandidatesbcsu();  

  Unit * myCrate = m_parent->parent();

  CrateUnit * cr = dynamic_cast<CrateUnit*>(myCrate);


  // Load ordered candidates and offsets in Crate 
  for (std::vector<std::pair<Candidate*, std::vector<int> > >::
         iterator ioff =
         m_offsets.begin(); ioff < m_offsets.begin()+2; ioff++){

    cr->fillOffset(*ioff);

    std::vector<int> tmp = (*ioff).second;

    
  }


  for (std::vector<Candidate* >::iterator icand = m_candidates.begin();
       icand < m_candidates.begin()+2; icand++){
    
    cr->fillCandidates(*icand);
  }
  

  // Build L0Buffer

  fillAddresses();
  fillInp();
  
  setInpBCSU();
  setIdBCSU();
  setOutBCSU();
  
  // Write Buffer
  writeL0Buffer();

  m_bcsueventnumber++;
  
}


void L0Muon::BestCandidateSelectionUnit::finalize()
{
  //m_offsets.clear();
  //m_candidates.clear();
  releaseRegisters();
  
}



void L0Muon::BestCandidateSelectionUnit::setOutputFile(std::string suffixe)

{
  
  char * name = "/marmuon3/NewInputFiles/l0buf_BCSUQ%dR%d%d%d_%s.txt";
  
  char buf[4096];
  
  sprintf(buf,name,m_id.quarter()+1,m_id.region()+1,m_id.nX(),m_id.nY(),suffixe.c_str());
  
  m_bcsul0bufferFile = fopen(buf,"w");
  
}


void L0Muon::BestCandidateSelectionUnit::loadCandidates(Candidate * cand)
{
  m_candidates.push_back(cand);
}

// function for debug
void L0Muon::BestCandidateSelectionUnit::dumpCandidates()
{
  if (m_candidates.size() > 0){
    if (m_debug) std::cout << "BCSU: # of candidates " 
                           << m_candidates.size() << std::endl;
    for (std::vector<Candidate*>::iterator icand =
           m_candidates.begin(); icand != m_candidates.end();icand++){
      if ( (*icand)->status() == L0MuonStatus::OK ) {	   	   
        if (m_debug) std::cout << "BCSU: " << (*icand)->pt() << std::endl;
      } 	
    }
  } 
}


void L0Muon::BestCandidateSelectionUnit::dumpAddresses()
{
 std::map<std::string,L0Muon::Register*>::iterator ir;
 for ( ir = m_inputs.begin(); ir != m_inputs.end(); ir++) {
   // boost::dynamic_bitset<> bits = (*ir).second->getBitset();
   if (m_debug) std::cout << "BCSU: " << (*ir).first << std::endl;
   boost::dynamic_bitset<> bits = (*ir).second->getBitset();
   for (boost::dynamic_bitset<>::size_type i =0; i < bits.size(); i++){
     if (m_debug) std::cout << "BCSU: " << " " << bits[i] ;     
   }
   if (m_debug) std::cout << std::endl;
 }
}


/*
  - Fill m_addresses with the informations contained 
  in the input registers (15 bits).
  - Add the pu number (2 bits at the end)
  Note: o BCSU contains 2 types of registers : 
         - addresses ("...addr_candidate...",15 bits)
         - status ("...status...",4 bits)
	o The condition to select the registers with 
          addresses should be changed
*/
void L0Muon::BestCandidateSelectionUnit::fillAddresses()
{

  m_addresses.clear();
    
  // Loop over the input register
  int iadd=0;
  std::map<std::string,L0Muon::Register*>::iterator ir;   
  for ( ir = m_inputs.begin(); ir != m_inputs.end(); ir++) {
    boost::dynamic_bitset<> bits = (*ir).second->getBitset();
    // Select the registers containing the addresses
    if (bits.size()>4){
      // Each PU has 2 addresses, divide the addresses counter by 2 to get the PU index
      int ipu = iadd/2;
      bits.push_back((ipu>>0)&1);
      bits.push_back((ipu>>1)&1);
      m_addresses.push_back(bits);
      ++iadd;
    }
  }
}


void L0Muon::BestCandidateSelectionUnit::sortCandidatesbcsu()
{
  //std::sort(m_candidates.begin(),m_candidates.end(), 
  //        CalculateBestPt());

  std::sort(m_candidates.begin(),m_candidates.end(), 
            ComparePt());

  //std::sort(m_offsets.begin(),m_offsets.end(), CalculateBestPt());
  std::sort(m_offsets.begin(),m_offsets.end(), ComparePt());

}


/*
  Fill m_inp (pair a candidate with its addresse).
  Note: m_inp is sorted with to decreasing pt
*/
void L0Muon::BestCandidateSelectionUnit::fillInp()
{

  m_inp.clear();

  int imax = m_candidates.size();
  
  for (int i =0 ; i <imax; i++){
    std::pair<Candidate*, boost::dynamic_bitset<> > tmp = 
      std::make_pair(m_candidates[i], m_addresses[i]);
    
    m_inp.push_back(tmp);
    
  }
  //std::sort(m_inp.begin(),m_inp.end(), CalculateBestPt());
  std::sort(m_inp.begin(),m_inp.end(), L0Muon::ComparePt());
} 


/*
  Fill m_BcsuId
*/
void L0Muon::BestCandidateSelectionUnit::setIdBCSU()
{
  //
  // Identification field = 2 words-> 32 bits (16 x 2)
  //
  m_BcsuId.clear();
  
  // Bits 0-11 L0Event number
  boost::dynamic_bitset<> evn(12, m_bcsueventnumber);
  int evnsize = evn.size();
  for (int i=0; i < evnsize ; i++){
    bool val = evn[i];
    m_BcsuId.push_back(val);
  }

  // Bits 12-15 free
  // Bits 16-27 L0_B_Id to fill 
  // Bits 28-31 free 
  
  // for the moment bits from 12 up to 31 are set to 0
  m_BcsuId.resize(32,0ul);

}

/*
  Fill m_BcsuOut
*/
void L0Muon::BestCandidateSelectionUnit::setOutBCSU()
 {
  //
  // Output field = 3 words-> 48 bits (16 x 3)
  //
   m_BcsuOut.clear();

  // Bits 0-15 for pt of candidate 1 & 2.
  for (int i=0; i<2; i++) {
    double p = m_inp[i].first->pt();
    boost::dynamic_bitset<> pt = codedPt(p);
    for ( boost::dynamic_bitset<>::size_type ib =0; ib < 8; ib++){  
      bool val = pt[ib];
      m_BcsuOut.push_back(val);
    } 
  }

  // Bits 16-31 for addresses of candidate 1 & 2 in M3 
  for (int i=0; i<2; i++) {
    boost::dynamic_bitset<> addr = m_inp[i].second ;
    // in M3
    for ( boost::dynamic_bitset<>::size_type it=0; it < 7; it++){
      bool val = addr[it];
      m_BcsuOut.push_back(val);
    }
    m_BcsuOut.push_back(0);
  }

  // Bits 32-33 and 40-41 for pu identifier for candidates 1 and 2
  for (int i=0; i<2; i++) {
    boost::dynamic_bitset<> addr = m_inp[i].second ;
    // PU
    for ( boost::dynamic_bitset<>::size_type it=15; it < 17; it++){
      bool val = addr[it];
      m_BcsuOut.push_back(val);
    } 
    if (i==0) m_BcsuOut.resize(40,0);
  } 
  m_BcsuOut.resize(44,0);

  // Bits 44-47 for status (0<= number of candidates <=8) 
  unsigned long icounter =0;
  for (std::vector<Candidate*>::iterator icand =
         m_candidates.begin(); icand != m_candidates.end();icand++){
    if ( (*icand)->status() != L0MuonStatus::PU_EMPTY){
      icounter++;
    }
  }  
  boost::dynamic_bitset<> st(4,icounter);
  for ( boost::dynamic_bitset<>::size_type ib =0; ib < st.size(); ib++){
    bool val = st[ib];
    m_BcsuOut.push_back(val);
  }
 
}

/*
  Fill m_BcsuInput
 */
void L0Muon::BestCandidateSelectionUnit::setInpBCSU()
{
  //
  // Input field = 15 words-> 240 bits (16 x 15)
  //
  m_BcsuInput.clear();
  
  // Get status from the registers
  std::vector<boost::dynamic_bitset<> > status;
  std::map<std::string,Register*>::iterator ir;
  for ( ir = m_inputs.begin(); ir != m_inputs.end(); ir++) {
    boost::dynamic_bitset<> bits = (*ir).second->getBitset();
    if (bits.size()==4){
      status.push_back(bits);
    }
  }

  // Get the pt from the Candidates
  std::vector<boost::dynamic_bitset<> > pts ;
  for (std::vector<Candidate*>::iterator icand =
         m_candidates.begin(); icand != m_candidates.end();icand++){
    double pt = (*icand)->pt();
    boost::dynamic_bitset<> ptbits = codedPt(pt);
    pts.push_back(ptbits);
  }

  int jmax = status.size();


  // Loop over PUs
  for (int j =0; j<jmax; j++){
    
    // Bits   0- 3: BcId (empty)
    for (int i=0; i <4; i++){
      m_BcsuInput.push_back(0);
    }

    // Bits   4- 7: status word 
    boost::dynamic_bitset<> st1 = status[0+j]; 
    for (boost::dynamic_bitset<>::size_type i =0; i < st1.size(); i++){
      m_BcsuInput.push_back(st1[i]);    
    }
  
    // Bits  8-15: first pt    
    boost::dynamic_bitset<> pt1 = pts[0+2*j];
    for (boost::dynamic_bitset<>::size_type i =0; i < pt1.size(); i++){
      m_BcsuInput.push_back(pt1[i]);   
    }

    // Bits 16-23: second Pt   
    boost::dynamic_bitset<> pt2 = pts[1+2*j];
    for (boost::dynamic_bitset<>::size_type i =0; i < pt2.size(); i++){
      m_BcsuInput.push_back(pt2[i]);      
    }

    // Bits 23-37: first addr (M3+M2+M1) 
    boost::dynamic_bitset<> add1 = m_addresses[0+2*j];
    add1.resize(15,0ul);
    for (boost::dynamic_bitset<>::size_type i =0; i < add1.size(); i++){
      m_BcsuInput.push_back(add1[i]);  
    }
    // Bits 38-38: free
    m_BcsuInput.push_back(0);

    // Bits 40-54: second addr (M3+M2+M1)
    boost::dynamic_bitset<> add2 = m_addresses[1+2*j];
    add2.resize(15,0ul);
    for (boost::dynamic_bitset<>::size_type i =0; i < add2.size(); i++){
      m_BcsuInput.push_back(add2[i]);          
    }
    // Bits 55-55: free
    m_BcsuInput.push_back(0);
    
  } // End of Loop over PUs

  // Complete the buffer with 0
  // (missing PUs in region R2 and last 16 bits)
  m_BcsuInput.resize(240,0ul);
  
}


void L0Muon::BestCandidateSelectionUnit::writeL0Buffer()
{
  
  if (m_bcsul0bufferFile==NULL) return;

  boost::dynamic_bitset<> bcsu[3];
  bcsu[0]=m_BcsuId;
  bcsu[1]=m_BcsuOut;
  bcsu[2]=m_BcsuInput;

  for (int iset=0;iset<3;iset++){

    for (boost::dynamic_bitset<>::size_type i =0; i <bcsu[iset].size();++i){ 
      int iwd=0;
      for (int ind=0;ind<16;ind++) {
        int val=bcsu[iset][i] ;
  
        iwd+= int(val*pow(2,ind));
        ++i;
      }
      fprintf(m_bcsul0bufferFile,"%04x\n",iwd);
      --i;
    }
  }

  fprintf(m_bcsul0bufferFile,"----\n");

}


/*
  Encode the p value given as a double on 8 bits (including one bit for the sign)
*/

boost::dynamic_bitset<> L0Muon::BestCandidateSelectionUnit::codedPt(double pt){

  unsigned long p= int((fabs(pt)+20.)/40.);

  if (p>0xaf) p=0xaf;
  
  boost::dynamic_bitset<> ptbits(7,p);
  
  if      (pt <=0.) ptbits.push_back(0);
  else if (pt  >0.) ptbits.push_back(1);
  
  return ptbits;
}


void L0Muon::BestCandidateSelectionUnit::loadOffsets(std::pair<Candidate*,std::vector<int> > off)
{   
  m_offsets.push_back(off);
}


