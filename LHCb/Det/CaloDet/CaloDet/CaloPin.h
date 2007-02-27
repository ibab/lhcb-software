#ifndef CALODET_CALOPIN_H 
#define CALODET_CALOPIN_H 1

// Include files
class DeCalorimeter;
#include "Kernel/CaloCellCode.h"
#include "Kernel/CaloCellID.h"
/** @class  CaloPin CaloPin.h CaloDet/CaloPin.h
 *  
 *
 */

class CaloPin 
{
  
  friend class DeCalorimeter;
  
public:
  
  /** constructors 
  */
  CaloPin():m_id(LHCb::CaloCellID())  {}  
  CaloPin(LHCb::CaloCellID id );
  
  /// destructor 
  ~CaloPin();


  // getters
  LHCb::CaloCellID   id()      const {return m_id ;}
  std::vector<int>& leds() {return m_leds; }
  std::vector<LHCb::CaloCellID>& cells() {return m_cells; }


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


  int  box()        const {return m_box; }
  int  index()      const {return m_index; }
  int  side()      const {return m_side; }


  friend std::ostream& operator<< (std::ostream& str, CaloPin obj){ return obj.fillStream(str);}
  std::ostream&  fillStream(std::ostream& str)  {    return str <<" cellId " << m_id << " : "
                                                                << m_code <<" : " << m_region; }

  // setters
  void setId(LHCb::CaloCellID id){m_id = id;}  
  void addLed(int id){ m_leds.push_back(id);}  
  void addCell(LHCb::CaloCellID cell){ m_cells.push_back(cell);}  
  void addCaloRegion(int area , int fCol, int fRow, int lCol, int lRow)  {
    m_area.push_back( area );
    m_fRow.push_back( fRow );
    m_fCol.push_back( fCol );
    m_lCol.push_back( lCol ); 
    m_lRow.push_back( lRow );
    std::stringstream s(" ") ;
    s << m_region << " += [" << area << ", "  << fCol <<","<<fRow <<","<<lCol <<","<<lRow << "] ";
    m_region = s.str();
  }

  void setPinLocation(int box, int index , int side)  {
    m_box = box;
    m_index=index;
    m_side = side;
    std::string sideName = (m_side==0) ? "C-side" : "A-side";
    std::stringstream s(" ") ;
    s << m_code << "(" << sideName << " : " << box << "." << index << ")";
    m_code = s.str();
  }
  void setFELocation(int crate, int slot, int adc)  {
    m_crate = crate;
    m_slot = slot;
    m_adc = adc;
  }
  

private:
  LHCb::CaloCellID m_id;
  std::string m_region;
  std::string m_code;
  int m_side;
  int m_index;
  int m_box;
  int m_crate;
  int m_slot;
  int m_adc;
  std::vector<int> m_area;
  std::vector<int> m_fCol;
  std::vector<int> m_fRow;
  std::vector<int> m_lCol;
  std::vector<int> m_lRow;
  std::vector<int> m_leds;
  std::vector<LHCb::CaloCellID> m_cells;
};


#endif // CALODET_CALOPIN_H
