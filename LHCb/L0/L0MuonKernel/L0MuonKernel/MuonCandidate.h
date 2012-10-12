// $Id: MuonCandidate.h,v 1.9 2008-12-09 18:47:28 cattanem Exp $

#ifndef L0MUONKERNEL_MUONCANDIDATE_H
#define L0MUONKERNEL_MUONCANDIDATE_H     1


#include "boost/shared_ptr.hpp"
#include "Kernel/MuonTileID.h"
#include <math.h>
#include <string>

namespace L0Muon {


  /** @class MuonCandidate MuonCandidate.h L0MuonKernel/MuonCandidate.h

      Class defining an internal representation of a Muon trigger candidate.

      @author Julien Cogan
      @date 2005 December the 15th

  */
  class MuonCandidate  {

  public:


    /// Default Constructor
    MuonCandidate();

    /// Copy Constructor
    MuonCandidate(const MuonCandidate& cand);

    /// Get the address in M3 (column)
    int colM3() const
    {
      return m_colM3 ;
    }

    /// Get the address in M3 (row)
    int rowM3() const
    {
      return m_rowM3 ;
    }

    /// Get the offset in M2
    int offM2() const
    {
      return m_offM2 ;
    }

    /// Get the offset in M1
    int offM1() const
    {
      return m_offM1 ;
    }

    /// Get the quarter
    int quarter() const
    {
      return m_quarter ;
    }

    /// Get the pu number
    int pu() const
    {
      return m_pu ;
    }

    /// Get the board number
    int board() const
    {
      return m_board ;
    }

    /// Get the PT
    int pT() const
    {
      return m_pT ;
    }

    /// Get the charge
    int charge() const
    {
      return m_charge ;
    }

    /// Set the address in M3 (column)
    void setColM3(int colM3)
    {
      m_colM3 = colM3 ;
    }

    /// Set the address in M3 (row)
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

    /// Return a string with the candidate informations
    std::string dump(std::string tab="");

    /// comparison operator using key.
    bool operator<(const MuonCandidate& cand) const;

    /// equality operator using key.
    bool operator==(const MuonCandidate& cand) const;

    /// non-equality operator using key.
    bool operator!=(const MuonCandidate& cand) const;

  private:
    int m_colM3 ; ///< column index of the candidate in M3.
    int m_rowM3 ; ///< row index of the candidate in M3.
    int m_offM2 ; ///< offset of the candidate in M2.
    int m_offM1 ; ///< offset of the candidate in M1.
    int m_pT ;    ///< encoded pt of the candidate.
    int m_charge ;///< charge of the candidate (=0: negative, =1: positiove).
    int m_pu;     ///< index of the PU where the candidate was found (0 to 3).
    int m_board;  ///< index of the board where the candidate was found (0 to 11).
    int m_quarter;///< index of the quarter where the candidate was found (0 to 3).

  };


  /// A share pointer to a MuonCandidate
  typedef boost::shared_ptr<MuonCandidate> PMuonCandidate;

} // namespace L0Muon

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
