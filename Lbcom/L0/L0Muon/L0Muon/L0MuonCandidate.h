// $Header: 

#ifndef L0MUON_L0MUONCANDIDATE_H
#define L0MUON_L0MUONCANDIDATE_H 1

// Include files
#include <iostream>
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRefVector.h"

#include "L0Muon/L0mPad.h"

extern const CLID& CLID_L0MuonCandidate;

/** @class L0MuonCandidate L0MuonCandidate.h L0Muon/L0MuonCandidate.h

   This class represents the input from the muon trigger to the L0
   decision unit.

   @author      Andrei Tsaregorodtsev
   @date        06 April 2001
*/

namespace L0Muon {
  enum StatusCode {
    OK=0,
    EMPTY,
    ERROR,
    CRATE_EMPTY,
    CRATE_ERROR,
    PU_EMPTY,
    PU_ERROR,
    PU_OVERFLOW
  };    
};

class L0MuonCandidate : virtual public ContainedObject {

public:

  /// Default constructor
  L0MuonCandidate( ) : m_pt(0), m_xM1(0), m_yM1(0), m_theta(0), m_phi(0) { }

  /// Constructor setting all the candidate parameters
  L0MuonCandidate( double pt, 
                   double x, 
		   double y, 
		   double theta, 
		   double phi,
		   L0mPad* mp1,
		   L0mPad* mp2,
		   L0mPad* mp3 ) :
                   m_pt(pt), 
		   m_xM1(x), 
		   m_yM1(y), 
		   m_theta(theta), 
		   m_phi(phi),
		   m_pM1(mp1),
		   m_pM2(mp2),
		   m_pM3(mp3)          { }

  ~L0MuonCandidate() { }

  /// Retrieve pointer to class definition structure

  virtual const CLID& clID()    const  { return L0MuonCandidate::classID();  }
  static  const CLID& classID()        { return CLID_L0MuonCandidate;  }

  /// Pt accessor
  double pt() const { return m_pt; } 
  /// X-coordinate of the muon pad in M1 accessor
  double x() const { return m_xM1; } 
  /// Y-coordinate of the muon pad in M1 accessor
  double y() const { return m_yM1; } 
  /// Theta at IP of the candidate accessor
  double theta() const { return m_theta; } 
  /// Phi at IP of the candidate accessor
  double phi() const { return m_phi; } 
  /// accessor to L0mPad in station M1
  L0mPad* padM1() { return m_pM1; } 
  /// accessor to L0mPad in station M2
  L0mPad* padM2() { return m_pM2; }   
  /// accessor to L0mPad in station M3
  L0mPad* padM3() { return m_pM3; } 
  /// accessor to L0Muon status code
  int status() const { return m_status; }
  /// Comparison < operator for sorting the candidates in Pt
  bool operator<(const L0MuonCandidate& lmc) { return pt() < lmc.pt(); }

/// Serialize the pbject, for read/write
  virtual StreamBuffer& serialize( StreamBuffer& s ) const ;
/// Serialize the pbject, for read/write  
  virtual StreamBuffer& serialize( StreamBuffer& s );

private:

  double           m_pt;	  // Pt of the candidate
  double           m_xM1;	  // X-coordinate of the pad in M1
  double           m_yM1;	  // Y-coordinate of the pad in M1
  double           m_theta;	  // Theta of the muon track at the IP
  double           m_phi;	  // Phi of the muon track at the IP
  int              m_status;	  // L0m processor status code
  SmartRef<L0mPad> m_pM1;         // reference to a pad in M1
  SmartRef<L0mPad> m_pM2;         // reference to a pad in M2
  SmartRef<L0mPad> m_pM3;         // reference to a pad in M3

};


/// Serialize the object for reading
inline StreamBuffer& L0MuonCandidate::serialize(StreamBuffer& s) {
  ContainedObject::serialize(s) >> m_pt >> m_xM1 >> m_yM1 
                                >> m_theta >> m_phi >> m_status
				>> m_pM1 >> m_pM2 >> m_pM3;
  return s;
}

/// Serialize the object for writing
inline StreamBuffer& L0MuonCandidate::serialize(StreamBuffer& s) const {
  ContainedObject::serialize(s) << m_pt << m_xM1 << m_yM1 
                                << m_theta << m_phi << m_status
				<< m_pM1 << m_pM2 << m_pM3;
  return s;
}

#endif // L0MUON_L0MUONCANDIDATE_H
