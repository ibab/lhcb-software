// $Id: L0mProcUnit.h,v 1.5 2005-02-03 19:49:51 atsareg Exp $

#ifndef L0MUONKERNEL_L0MPROCUNIT_H
#define L0MUONKERNEL_L0MPROCUNIT_H     1

/* @class L0mProcUnit L0mProcUnit.h L0MuonKernel/L0mProcUnit.h

   Class representing a processing unit 
   of the level-0 muon trigger 
   
*/

#include <vector>
#include "ProcessorKernel/Unit.h"
#include "L0mConf/L0MPuNodeBase.h"
#include "L0mConf/L0MTile.h"
#include "L0mConf/L0MFoi.h"
#include "ProcessorKernel/RegisterFactory.h"
#include "L0MuonKernel/Tower.h"
#include "L0MuonKernel/CablingUnit.h"
#include "L0MuonKernel/BuildL0BufferUnit.h"
#include "L0MuonKernel/FormattingUnit.h"

namespace L0Muon {

class L0mProcUnit : public Unit {

 public:

  /// Default constructor
  L0mProcUnit();

  /** Constructor

        @param pProNet   :  data flow from L0mConf
        @param ptpara    :  geometrical parameters for calculating pT
        @param ignoreM1  :  flag for searching candidates without M1
        @param foix      :  field of interest in the x direction
        @param foiy      :  field of interest in the y direction
        @param precision :  precision for calculating pT
        @param bits      :  number of bits for codifying pT
        @param writeL0Buffer         :  flag for writing L0Buffers on files
  */
  L0mProcUnit(L0MPuNodeBase& puNode,
              std::vector<double> & ptpara,
              bool & ignoreM1,
              std::vector<int> & foix,
              std::vector<int> & foiy,
              double & precision,
              int & bits,
              bool & writeL0buffer);

  /// Destructor
  ~L0mProcUnit();
 
  /// Return the MuonTileID of the PU
  MuonTileID puId(){return m_puID;} 

  /// Return the MuonTileID of the board
  MuonTileID boardId(){return m_boardID;}

  /// Return the field of interest from L0mConf
  L0MFoi foi() {return m_maxFoi; }
  
  /// Return parameters for calculating pT
  std::vector<double> ptParameters(){ return m_ptparameters;}
 
  /** Return the flag for searching 
      candidates without M1
  */ 
  bool ignoreM1(){ return m_ignoreM1;}
 
  /// Return the flag for writing L0Buffer
  bool writeL0Buffer(){ return m_writeL0buffer;}

  /// Return the precision for pT
  double precision(){ return m_precision;}

  /// Return the number of bits for codifying pT
  int bits(){ return m_bits;}

  /// Return x foi in station sta
  int xFoi(int sta);
  
  /// Return y foi in station sta
  int yFoi(int sta);

  /// Initialize subunits
  //void initialize();

 
  /// Execute subunits
  //void execute();

  /// Finalize subunits
  //void finalize();
  
  /// Give a static type name to the unit
  std::string type() {
    return "L0mProcUnit";
  }
  

 private:

    
  CablingUnit * m_cu;
  BuildL0BufferUnit * m_l0b;
  FormattingUnit * m_formatting;
  MuonTileID m_puID;
  MuonTileID m_boardID;
  L0MFoi m_maxFoi;
  
  std::vector<double> m_ptparameters;
  std::vector<int> m_xfoi;
  std::vector<int> m_yfoi;
  double m_precision;
  int m_bits;
  bool m_ignoreM1;

  bool m_writeL0buffer;
    
};

};  // namespace L0Muon

#endif      // L0MUONKERNEL_L0MPROCUNIT_H  
