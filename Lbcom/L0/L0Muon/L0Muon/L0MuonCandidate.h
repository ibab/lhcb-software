// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/L0Muon/L0MuonCandidate.h,v 1.3 2001-06-07 16:46:13 atsareg Exp $

#ifndef L0MUON_L0MUONCANDIDATE_H
#define L0MUON_L0MUONCANDIDATE_H 1

// Include files
#include <iostream>
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRefVector.h"

#include "L0Muon/L0mPad.h"

extern const CLID& CLID_L0MuonCandidate;

// const CLID& CLID_L0MuonCandidate = 6002;

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
  L0MuonCandidate() : 
    m_pt(0), m_xM1(0), m_yM1(0), m_theta(0), m_phi(0) { 
    m_status = L0Muon::EMPTY; 
  }  
  L0MuonCandidate(int status ) : 
    m_pt(0), m_xM1(0), m_yM1(0), m_theta(0), m_phi(0) { 
    m_status = status;
  }

  /// Constructor setting all the candidate parameters
  L0MuonCandidate( double pt, 
                   double x, 
		   double y, 
		   double theta, 
		   double phi,
		   SmartRefVector<L0mPad> mpads,
		   int status ) :
                   m_pt(pt), 
		   m_xM1(x), 
		   m_yM1(y), 
		   m_theta(theta), 
		   m_phi(phi),
		   m_pads(mpads),
		   m_status(status)          { }

  ~L0MuonCandidate() { }

  /// Retrieve pointer to class definition structure

  virtual const CLID& clID()    const  { return L0MuonCandidate::classID();  }
  static  const CLID& classID()        { return CLID_L0MuonCandidate;  }

  /// Pt accessor
  double pt() const { return m_pt; } 
  /// set Pt value
  void setPt(double pt) { m_pt = pt; }
  /// X-coordinate of the muon pad in M1 accessor
  double x() const { return m_xM1; } 
  /// Y-coordinate of the muon pad in M1 accessor
  double y() const { return m_yM1; } 
  /// Theta at IP of the candidate accessor
  double theta() const { return m_theta; } 
  /// Phi at IP of the candidate accessor
  double phi() const { return m_phi; } 
  /// accessor to L0mPad in station M1
  L0mPad* pad(unsigned int station);
  /// accessor to L0Muon status code
  int status() const { return m_status; }
  /// set status
  void setStatus(int status) { m_status = status; }
  /// Comparison < operator for sorting the candidates in Pt
  bool operator<(const L0MuonCandidate& lmc) { 
    return fabs(pt()) < fabs(lmc.pt()); 
  }

/// Serialize the pbject, for read/write
  virtual StreamBuffer& serialize( StreamBuffer& s ) const ;
/// Serialize the pbject, for read/write  
  virtual StreamBuffer& serialize( StreamBuffer& s );

private:

  double		 m_pt;    // Pt of the candidate
  double		 m_xM1;   // X-coordinate of the pad in M1
  double		 m_yM1;   // Y-coordinate of the pad in M1
  double		 m_theta;	  // Theta of the muon track at the IP
  double		 m_phi;   // Phi of the muon track at the IP
  SmartRefVector<L0mPad> m_pads;         // references to muon pads
  int                    m_status;	// L0m processor status code
  
};


/// Serialize the object for reading
inline StreamBuffer& L0MuonCandidate::serialize(StreamBuffer& s) {
  ContainedObject::serialize(s) >> m_pt >> m_xM1 >> m_yM1 
                                >> m_theta >> m_phi >> m_status
				>> m_pads;
  return s;
}

/// Serialize the object for writing
inline StreamBuffer& L0MuonCandidate::serialize(StreamBuffer& s) const {
  ContainedObject::serialize(s) << m_pt << m_xM1 << m_yM1 
                                << m_theta << m_phi << m_status
				<< m_pads;
  return s;
}

/// Auxilliary class for sorting candidates in the Pt decreasing order
class ComparePt {
public:
  int operator() (L0MuonCandidate* lmc1,
	          L0MuonCandidate* lmc2) {
    return fabs(lmc1->pt()) > fabs(lmc2->pt());
  }		
};

#endif // L0MUON_L0MUONCANDIDATE_H
