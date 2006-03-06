// $Id: MuonCandidate.h,v 1.4 2006-03-06 09:56:00 jucogan Exp $

#ifndef L0MUONKERNEL_MUONCANDIDATE_H
#define L0MUONKERNEL_MUONCANDIDATE_H     1

/** @class MuonCandidate MuonCandidate.h L0MuonKernel/MuonCandidate.h

Class defining an internal representation
of a Muon trigger candidate

*/

#include "boost/shared_ptr.hpp"
#include <math.h>
#include <string>

namespace L0Muon {


  class MuonCandidate  {
  
  public:
    

    /// Default Constructor 
    MuonCandidate();

    /// Get the address in M3
    const int addM3() const
    {
      return m_addM3 ;
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
    void setAddM3(int addM3) 
    {
      m_addM3 = addM3 ;
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

    std::string dump();
    
    
  private:
    int m_addM3 ;
    int m_offM2 ;
    int m_offM1 ;
    int m_pT ;
    int m_charge ;
    int m_pu;
    int m_board;
    int m_quarter;
    
  };

  typedef boost::shared_ptr<MuonCandidate> PMuonCandidate;

}; // namespace L0Muon

#endif      // L0MUONKERNEL_MUONCANDIDATE_H
