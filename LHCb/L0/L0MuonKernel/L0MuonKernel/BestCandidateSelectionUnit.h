#ifndef L0MUONKERNEL_BESTCANDIDATESELECTIONUNIT_H
#define L0MUONKERNEL_BESTCANDIDATESELECTIONUNIT_H     1

/* class BestCandidateSelectionUnit L0MuonKernel/BestCandidateSelectionUnit
 
   Class representing a BCSU of the hardware processor.
   It receive two candidates from each PU in the board 
   and choose the two ones with the highest pT

   author  Luisanna Tocco 
   date    24 September 2003
*/

#include <boost/dynamic_bitset.hpp>
#include <vector>
#include <cmath>
#include <utility>
#include "ProcessorKernel/Unit.h"
#include "L0MuonKernel/Candidate.h"

namespace L0Muon {

  class BestCandidateSelectionUnit : public Unit {

  public:

    /** Constructor 
        
        @param writeL0Buffer : flag for writing L0Buffer on file

    */
    BestCandidateSelectionUnit(MuonTileID puid);

    /// Destructor
    ~BestCandidateSelectionUnit();

    /// Get candidates from PUs
    void loadCandidates(Candidate * cand);
  
    /// Get status from PU
    void loadStatus(int st){ m_status = st; }                    
  
    /// Debug : print pT of the candidates 
    void dumpCandidates();

    /// Debug : print addresses of candidates 
    void dumpAddresses();


    /// Open the output file
    void setOutputFile(std::string suffixe); 

    /// Write the output file
    void writeL0Buffer();  


    void sortCandidatesbcsu();
    void loadOffsets(std::pair<Candidate*, std::vector<int> >);

   

    void initialize();
    void execute();
    void finalize();
    
    /// Give a static type name to the unit
    std::string type() {
      return "BestCandidateSelectionUnit";
    }

  private:

    void fillAddresses();
    void fillInp();

    void setOutBCSU(); // set the output field
    void setInpBCSU(); // set the input field
    void setIdBCSU();  // set the Identification field
    
    boost::dynamic_bitset<> codedPt(double pt); // code the value of pt on 8 bits

    MuonTileID m_id;

    boost::dynamic_bitset<> m_BcsuInput; // bitset for input field
    boost::dynamic_bitset<> m_BcsuOut ;  // bitset for output field
    boost::dynamic_bitset<> m_BcsuId ;   // bitset for Identification field
    
    unsigned long m_bcsueventnumber ;   // counter for event number


    std::vector<int> m_punumber;
    
    std::vector<Candidate*> m_candidates;
    unsigned int m_status;

    std::vector<std::pair<Candidate*, 
      boost::dynamic_bitset<> > > m_inp;
    
    std::vector<boost::dynamic_bitset<> > m_addresses ;
    std::vector<boost::dynamic_bitset<> > m_Bcsu ;


    FILE *m_bcsul0bufferFile;

    std::vector<std::pair<Candidate*, std::vector<int> > > m_offsets;    

  };


};  // namespace L0Muon

#endif      // L0MUONKERNEL_BOARD_H  
