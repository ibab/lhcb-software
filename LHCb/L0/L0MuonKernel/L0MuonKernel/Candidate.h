// $Id: Candidate.h,v 1.2 2005-04-04 07:58:43 atsareg Exp $

#ifndef L0MUONKERNEL_CANDIDATE_H
#define L0MUONKERNEL_CANDIDATE_H     1

/** @class Candidate Candidate.h L0MuonKernel/Candidate.h

    Class representing an internal representation
    of a Muon trigger candidate

*/

#include "MuonKernel/MuonTileID.h"
#include "boost/shared_ptr.hpp"

namespace L0Muon {

  class Candidate  {
  
    public:
    
      /// Constructor with just a status info
      Candidate(int status);

      /// Constructor with full information
      Candidate(double pt, double theta, double phi, const std::vector<MuonTileID>& pads, int status);

      /// Default Constructor 
      Candidate() 
	: m_pt(0.0),m_theta(0.0),m_phi(0.0),m_pads() {}
	
     /// Accessor to the encoded Pt
     double pt() const;
     
     /// Accessor to the full precision Pt
     double ptPrecision() const;
    
     /// Accessor to the pad in M1
     MuonTileID padM1() const;

     /// Accessor to the pad in a given station
     MuonTileID pad(unsigned int station) const;

     /// Accessor to the encoded status
     int status() const;

     /// Comparison operator for sorting candidates in Pt
     bool operator<(const Candidate& lmc) const;
     
     /// Retrieve Theta at IP of the candidate
     double theta() const; 

     /// Retrieve Phi at IP of the candidate
     double phi() const;
     
     /// Setting the candidate offsets
     void setOffsets(const std::vector<int>& offsets);
     
     /// Retrieve offsets
     std::vector<int> offsets() const { return m_offsets; }
     
     /// Dump the candidate paramters
     void dump();
    
    private:
    
     /// Setting the candidate status bits
     void setStatus(int status);

     /// Setting the candidate Pt bits
     void setPt(double pt);

     /// Setting the candidate M1 pad bits
     void setPadM1(const MuonTileID& pad);


     double                  m_pt;      ///< Pt of the candidate
     int                     m_ptround; ///< Rounded Pt of the candidate
     double                  m_theta;   ///< Theta at IP of the candidate
     double                  m_phi;     ///< Phi at IP of the candidate
     std::vector<MuonTileID> m_pads;    ///< Pads used in the candidate 
     int                     m_status;  ///< Candidate status
     std::vector<int>        m_offsets; ///< Offsets as found by the track search
  };

  typedef boost::shared_ptr<Candidate> PCandidate;

  inline double Candidate::theta() const 
  {
    return m_theta;
  }

  inline double Candidate::phi() const 
  {
    return m_phi;
  }

}; // namespace L0Muon

#endif      // L0MUONKERNEL_CANDIDATE_H
