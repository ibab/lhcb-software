/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.1  2001/07/02 17:17:58  ibelyaev
/// improvements in readability of DeCalorimeter.h
///
/// ===========================================================================
#ifndef CALODET_CARDPARAM_H 
#define CALODET_CARDPARAM_H 1
/// ===========================================================================

// Include files
class DeCalorimeter;

/** @class  CardParam CardParam.h CaloDet/CardParam.h
 *  
 *
 */

class CardParam 
{
  
  friend class DeCalorimeter;
  
public:
  
  /** constructor 
      @param iArea  calorimeter area 
      @param iRow   row 
      @param icol   column
  */
  CardParam( int iArea, int iRow, int iCol );
  
  /// destructor 
  ~CardParam();
  
  int downNumber      () const { return m_DownNumber     ; }
  int leftNumber      () const { return m_LeftNumber     ; }
  int cornerNumber    () const { return m_CornerNumber   ; }
  int previousNumber  () const { return m_PreviousNumber ; }
  int validationNumber() const { return m_validationNumber;}
  int area            () const { return m_Area           ; }
  int firstRow        () const { return m_Row            ; }
  int firstColumn     () const { return m_Col            ; }
  
  void setValidationNumber( int num ) {
    m_validationNumber = num;
  }
  
  void setNeighboringCards  ( const int down  ,   
                              const int left, 
                              const int corner, 
                              const int previous ) { 
    m_DownNumber   = down ;
    m_LeftNumber   = left ;
    m_CornerNumber = corner ; 
    m_PreviousNumber = previous;
  }
  
private:
  
  int m_Area;
  int m_Row;
  int m_Col;
  int m_DownNumber;
  int m_LeftNumber;
  int m_CornerNumber;
  int m_PreviousNumber;
  int m_validationNumber;
};


/// ===========================================================================
#endif // CALODET_CARDPARAM_H
/// ===========================================================================
