// $Id: L0mProcUnit.h,v 1.2 2004-03-11 15:57:55 ltocco Exp $

#ifndef L0MUONKERNEL_L0MPROCUNIT_H
#define L0MUONKERNEL_L0MPROCUNIT_H     1

/* class L0mProcUnit L0mProcUnit L0MuonKernel/L0mProcUnit.h

   Class representing a unit of data processing logic 
   of the L0Muon Trigger for hardware simulations
   
   author  Andrei Tsaregorodtsev
   date  12 January 2002
*/

#include <vector>
#include "L0MuonKernel/Unit.h"
#include "L0mConf/L0MPuNodeBase.h"
#include "L0mConf/L0MTile.h"
#include "L0mConf/L0MFoi.h"
#include "L0MuonKernel/RegisterFactory.h"
#include "L0MuonKernel/Tower.h"
#include "L0MuonKernel/CablingUnit.h"
//#include "L0MuonKernel/BuildL0BufferUnit.h"
//#include "L0MuonKernel/LUT.h"
#include "L0MuonKernel/FormattingUnit.h"
#include "GaudiKernel/MsgStream.h"

namespace L0Muon {

class L0mProcUnit : public L0Muon::Unit {

 public:
  L0mProcUnit();
  L0mProcUnit(L0MPuNodeBase& puNode,
              std::vector<double> & ptpara,
              std::vector<int> & foix,
              std::vector<int> & foiy,
              double & precision,
              int & bits,
              MsgStream & log);
  ~L0mProcUnit();

  MuonTileID puId(){return m_puID;}
  MuonTileID boardId(){return m_boardID;}

  L0MFoi foi() {return m_maxFoi; }
  
  std::vector<double> ptParameters(){ return m_ptparameters;}
  double precision(){ return m_precision;}
  int bits(){ return m_bits;}

  int xFoi(int sta);
  
  int yFoi(int sta);


  void execute();
  void initialize();
  void finalize();
  

 private:
  
  CablingUnit * m_cu;
  //BuildL0BufferUnit * m_l0b;
  FormattingUnit * m_formatting;
  MuonTileID m_puID;
  MuonTileID m_boardID;
  L0MFoi m_maxFoi;
  
  std::vector<double> m_ptparameters;
  std::vector<int> m_xfoi;
  std::vector<int> m_yfoi;
  double m_precision;
  int m_bits;
  
  
};

};  // namespace L0Muon

#endif      // L0MUONKERNEL_L0MPROCUNIT_H  
