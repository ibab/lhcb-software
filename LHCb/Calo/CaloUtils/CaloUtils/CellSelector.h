#ifndef CALOUTILS_CELLSELECTOR_H 
#define CALOUTILS_CELLSELECTOR_H 1
#include "CaloUtils/CellMatrix.h"
#include "CaloUtils/CellMatrix3x3.h"
#include "CaloUtils/CellMatrix2x2.h"
#include "CaloUtils/CellNeighbour.h"
#include "CaloUtils/CellSwissCross.h"
#include "CaloDet/DeCalorimeter.h"

/** @class CellSelector CellSelector.h CaloUtils/CellSelector.h
 *  
 */

class CellSelector: public CellMatrix{
public:
  
  CellSelector( const DeCalorimeter* det = 0 , std::string selector = "" );  
  
  virtual ~CellSelector( ); ///< Destructor

  void setDet(  const DeCalorimeter* det);
  inline void setSelector(std::string selector){ m_selector = selector;  };
  inline double operator()    ( const LHCb::CaloCellID& seed , 
                                const LHCb::CaloCellID& cell ) const{
    if( "3x3" == m_selector)return m_cell3x3(seed,cell);
    else if("2x2" == m_selector)return m_cell2x2(seed,cell);
    else if("SwissCross" == m_selector)return m_cellSwissCross(seed,cell);
    else if("Neighbour" == m_selector)return m_cellNeighbour(seed,cell);
    else return 1.;
  };
  
protected:
  
  std::string m_selector;
  CellMatrix3x3 m_cell3x3 ;
  CellMatrix2x2 m_cell2x2 ;
  CellSwissCross m_cellSwissCross ;
  CellNeighbour m_cellNeighbour ;
  
};

#endif // CALOUTILS_CELLSELECTOR_H
