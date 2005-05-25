#ifndef L0MUONKERNEL_BCSUNIT_H
#define L0MUONKERNEL_BCSUNIT_H     1

/** @class BCSUnit BCSUnit.h L0MuonKernel/BCSUnit.h
 
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

  class BCSUnit : public Unit {

  public:

    /// Constructor 
    BCSUnit(MuonTileID puid);

    /// Destructor
    ~BCSUnit();

    /// Get candidates from PUs
    void loadCandidates(PCandidate cand);
  
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
    void loadOffsets(std::pair<PCandidate , std::vector<int> >);

   

    void initialize();
    void preexecute();
    void execute();
    void postexecute();
    
    /// Give a static type name to the unit
    std::string type() {
      return "BCSUnit";
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
    
    std::vector<PCandidate > m_candidates;
    unsigned int m_status;

    std::vector<std::pair<PCandidate, 
      boost::dynamic_bitset<> > > m_inp;
    
    std::vector<boost::dynamic_bitset<> > m_addresses ;
    std::vector<boost::dynamic_bitset<> > m_Bcsu ;


    FILE *m_bcsul0bufferFile;

    std::vector<std::pair<PCandidate, std::vector<int> > > m_offsets;  
    
    bool m_writeL0Buffer;
    bool m_buildL0Buffer;  

  };


};  // namespace L0Muon

#endif      // L0MUONKERNEL_BCSUNIT_H 
