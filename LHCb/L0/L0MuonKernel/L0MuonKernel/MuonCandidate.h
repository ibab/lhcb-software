// $Id: MuonCandidate.h,v 1.7 2007-08-27 09:32:24 jucogan Exp $

#ifndef L0MUONKERNEL_MUONCANDIDATE_H
#define L0MUONKERNEL_MUONCANDIDATE_H     1

/** @class MuonCandidate MuonCandidate.h L0MuonKernel/MuonCandidate.h

Class defining an internal representation of a Muon trigger candidate

*/

#include "boost/shared_ptr.hpp"
#include "Kernel/MuonTileID.h"
#include <math.h>
#include <string>

namespace L0Muon {


  class MuonCandidate  {
  
  public:
    
// //     static const int extrapolationM1(int i,int procVersion=-1){
// //       if (version ==0) {
// //         const int ExtrapolationM1[6]={0,+4,+7,+11,+14,+18};
// //         return ExtrapolationM1[i];
// //       } else {
// //         const int ExtrapolationM1[6]={0,+4,+7,+11,+15,+18};
// //         return ExtrapolationM1[i];
// //       }
// //     }

    /// Default Constructor 
    MuonCandidate();

    /// Copy Constructor 
    MuonCandidate(const MuonCandidate& cand);

    /// Get the address in M3
    const int colM3() const
    {
      return m_colM3 ;
    }
    const int rowM3() const
    {
      return m_rowM3 ;
    }
    
    /// Get the offset in M2
    const int offM2() const
    {
      return m_offM2 ;
    }
    
    /// Get the offset in M1
    const int offM1() const
    {
      return m_offM1 ;
    }
    
    /// Get the quarter
    const int quarter() const
    {
      return m_quarter ;
    }
    
    /// Get the pu number
    const int pu() const
    {
      return m_pu ;
    }
    
    /// Get the board number
    const int board() const
    {
      return m_board ;
    }
    
    /// Get the PT
    const int pT() const
    {
      return m_pT ;
    }

    /// Get the charge
    const int charge() const
    {
      return m_charge ;
    }
    
    /// Set the address in M3
    void setColM3(int colM3) 
    {
      m_colM3 = colM3 ;
    }
    void setRowM3(int rowM3) 
    {
      m_rowM3 = rowM3 ;
    }
    
    /// Set the offset in M2
    void setOffM2(int offM2)
    {
      m_offM2 = offM2;
    }
    
    /// Set the offset in M1
    void setOffM1(int offM1)
    {
      m_offM1 = offM1;
    }
    
    /// Set the pu number
    void setQuarter(int quarter)
    {
      m_quarter = quarter;
    }   

    /// Set the pu number
    void setPu(int pu)
    {
      m_pu = pu;
    }   

    /// Set the board number
    void setBoard(int board)
    {
      m_board = board;
    }
    
    /// Set the PT
    void setPT(int pT)
    {
      m_pT = pT;
    }
    
    /// Set the PT
    void setPT(double pT, double dpT, double pTmax)
    {
      pT = fabs(pT)>pTmax ? pTmax:fabs(pT);
      m_pT = int(0.5+pT/dpT);
    }
    
    /// Set the PT
    void setPT(double pT, double dpT, int nbits)
    { 
      m_pT = int(0.5+fabs(pT)/dpT);
      int ptMax = (1<<nbits)-1;
      m_pT = m_pT>ptMax ? ptMax:m_pT ;
    }
    
    /// Set the charge
    void setCharge(int charge)
    {
      m_charge = charge>0 ? +1:0;
    }
    
    /// Set the charge
    void setCharge( double pT)
    {
      m_charge =  pT>0. ? +1:0;      
    }

    std::string dump(std::string tab="");
 
    /// comparison operator using key.
    bool operator<(const MuonCandidate& cand) const;

    /// equality operator using key.
    bool operator==(const MuonCandidate& cand) const;

    /// non-equality operator using key.
    bool operator!=(const MuonCandidate& cand) const;
    
//     /// Return the MuonTileId of the board 
//     LHCb::MuonTileID boardID(bool debug) const;
//     /// Return the MuonTileId of the PU 
//     LHCb::MuonTileID puID(bool debug) const;
    
//     /// Pads in M1, M2 and M3 used by the candidate 
//     std::vector<LHCb::MuonTileID> pads(int procVersion=-1, bool debug=false) const;
    
  private:
    int m_colM3 ;
    int m_rowM3 ;
    int m_offM2 ;
    int m_offM1 ;
    int m_pT ;
    int m_charge ;
    int m_pu;
    int m_board;
    int m_quarter;
    
  };

//   /// Candidate kinematics (PT, theta and Phi)
//   std::vector<double> kine(LHCb::MuonTileID p1, LHCb::MuonTileID p2,
//                            std::vector<double> ptparam, bool debug=false);
//   void xyFromPad(LHCb::MuonTileID pad, double& x, double& y,
//                  std::vector<double> ptparam);
// //   /// Candidate kinematics (PT, theta and Phi)
// //   std::vector<double> kine(LHCb::MuonTileID p1, LHCb::MuonTileID p2,int procVersion=-1, bool debug=false);
// //   std::vector<double> kineV0(LHCb::MuonTileID p1, LHCb::MuonTileID p2, bool debug=false);
// //   void xyFromPad(LHCb::MuonTileID pad, double& x, double& y);

  typedef boost::shared_ptr<MuonCandidate> PMuonCandidate;

}; // namespace L0Muon

inline bool L0Muon::MuonCandidate::operator<(const L0Muon::MuonCandidate& cand) const
{
  return pT()<  cand.pT() ;
}

inline bool L0Muon::MuonCandidate::operator==(const L0Muon::MuonCandidate& cand) const
{
  bool eq =   (pT()      == cand.pT()   )  && (charge()  == cand.charge())                         
           && (offM2()   == cand.offM2())  && (offM1()   == cand.offM1() )  
           && (colM3()   == cand.colM3())  && (rowM3()   == cand.rowM3() )  
           && (pu()      == cand.pu()   )  && (board()   == cand.board() ) 
           && (quarter() == cand.quarter());
  return  eq ;
}

inline bool L0Muon::MuonCandidate::operator!=(const L0Muon::MuonCandidate& cand) const
{
 return !(cand == *this) ;
}

#endif      // L0MUONKERNEL_MUONCANDIDATE_H
