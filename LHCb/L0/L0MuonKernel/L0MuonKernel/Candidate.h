// $Id: Candidate.h,v 1.1 2005-02-02 23:09:54 atsareg Exp $

#ifndef PROCESSORKERNEL_CANDIDATE_H
#define PROCESSORKERNEL_CANDIDATE_H     1

/** @class Candidate Candidate.h L0MuonKernel/Candidate.h

    Class representing an internal representation
    of a Muon trigger candidate

*/

#include "MuonKernel/MuonTileID.h"

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
     int                     m_status;
};

inline double Candidate::theta() const 
{
  return m_theta;
}

inline double Candidate::phi() const 
{
  return m_phi;
}

}; // namespace L0Muon

#endif      // PROCESSORKERNEL_CANDIDATE_H
