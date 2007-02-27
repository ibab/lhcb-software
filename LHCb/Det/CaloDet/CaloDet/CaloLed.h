#ifndef CALODET_CALOLED_H 
#define CALODET_CALOLED_H 1

// Include files
class DeCalorimeter;
#include "Kernel/CaloCellCode.h"
#include "Kernel/CaloCellID.h"



/** @class  CaloLed CaloLed.h CaloDet/CaloLed.h
 *  
 *
 */

class CaloLed 
{  
  friend class DeCalorimeter; 
public:
  
  /** constructors
  */
  CaloLed():m_id( -1 )  {}  
  CaloLed(int number);
  
  
  /// destructor 
  ~CaloLed();
  

  //getters
  int number()  const  { return m_id ; }
  std::vector<LHCb::CaloCellID>& cells() {return m_cells; }
  LHCb::CaloCellID   pin() const {return m_pin;   }

  std::vector<int>  firstRows()    const {return m_fRow;  }    
  std::vector<int>  lastRows()     const {return m_lRow; } 
  std::vector<int>  firstColumns() const {return m_fCol; }
  std::vector<int>  lastColumns()  const {return m_lCol; }
  std::vector<int>  areas()        const  { return m_area  ; }
  int   firstRow()    const  {return *(m_fRow.begin() ) ;  }    
  int   lastRow()     const  {return *(m_lRow.begin() ) ;  } 
  int   firstColumn() const  {return *(m_fCol.begin() ) ;  }
  int   lastColumn()  const  {return *(m_lCol.begin() ) ;  }
  int   area()        const  {return *(m_area.begin() )  ; }
  int  index()       const {return m_index; }

  // setters
  void setNumber(int number){m_id = number;}
  void addCell(const LHCb::CaloCellID cell){ m_cells.push_back(cell);}  
  void setPin(const LHCb::CaloCellID pin){m_pin = pin; }
  void setIndex(const int index){ m_index =index;  }
  void addCaloRegion(int area, int fCol, int fRow, int lCol, int lRow)  {
    m_area.push_back( area );
    m_fRow.push_back( fRow );
    m_fCol.push_back( fCol );
    m_lCol.push_back( lCol ); 
    m_lRow.push_back( lRow );
    std::stringstream s(" ") ;
    s << m_region << " += [" << area << ", "  << fCol <<","<<fRow <<","<<lCol <<","<<lRow << "] ";
    m_region = s.str();
  }
  friend std::ostream& operator<< (std::ostream& str,  CaloLed obj){ return obj.fillStream(str);}
  std::ostream&  fillStream(std::ostream& str)  {    return str << " Led : " << number() 
                                                                << " index "  << index() 
                                                                <<" : "      <<  m_region 
                                                                << " readout by  PINId" << pin() ;  }
  
    

private:
  int m_id;
  std::string m_region;
  LHCb::CaloCellID m_pin;
  std::vector<int> m_area;  
  std::vector<int> m_fCol;
  std::vector<int> m_fRow;
  std::vector<int> m_lCol;
  std::vector<int> m_lRow;
  int m_index;
  std::vector<LHCb::CaloCellID> m_cells;
};



#endif // CALODET_CALOLED_H
