/// ===========================================================================
/// $Id: CardParam.h,v 1.13 2009-04-06 15:42:33 odescham Exp $
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




  enum Mapping { None=0, 
                 Prs64Top=1,
                 Prs64Bottom=2,
                 Prs32Top=3,
                 Prs32Bottom=4};
  
  
  /** constructor 
      @param iArea  calorimeter area 
      @param iRow   row 
      @param iCol   column
      @param ilRow
      @param ilCol
      @param iNum
      @param iCrate
      @param iSlot
  */
  CardParam( int iArea = 0 , 
             int iRow  = 0 , 
             int iCol  = 0 ,
             int ilRow  = 0 , 
             int ilCol  = 0 ,
             int iNum  = 0 ,
             int iCrate= 0 ,
             int iSlot = 0
             ) ;
  
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
  int lastRow         () const { return m_lRow           ; }
  int lastColumn      () const { return m_lCol           ; }
  bool isPinCard      () const { return m_isPin          ; }
  int tell1           () const { return m_tell1          ; }
  int  number         () const { return m_number         ; }
  int  crate          () const { return m_crate          ; }
  int  slot           () const { return m_slot           ; }
  int  code           () const { return 16*m_crate+m_slot; }
  int  selectionType  () const { return m_selectionType  ; }    
  Mapping mapping     () const { return m_mapping        ; }
  bool isParasitic()     const { return ( (m_Row+m_lRow+m_Col+m_lCol) == 0 );}  // special FEBs (no channelID associated)
  bool isPmtCard      () const { return (!isPinCard() && !isParasitic());}

  const std::vector<LHCb::CaloCellID>& ids() { return m_ids; }  
  void addID( LHCb::CaloCellID id )    { m_ids.push_back( id ); }  
  void setTell1(int tell1)             { m_tell1 = tell1; }
  void setValidationNumber( int num )  {m_validationNumber = num;}  
  void setNeighboringCards  ( const int down  ,   
                              const int left, 
                              const int corner,
                              const int previous ) { 
    m_DownNumber   = down ;
    m_LeftNumber   = left ;
    m_CornerNumber = corner ; 
    m_PreviousNumber = previous;
  }
  void setIsPin(bool pin)  {    m_isPin = pin;  }
  void setSelectionType( int num){ m_selectionType = num;}
  void setMapping(Mapping typ){m_mapping = typ;}
  
    
  
private:
  
  int m_Area;
  int m_Row;
  int m_Col;
  int m_lRow;
  int m_lCol;
  int m_DownNumber;
  int m_LeftNumber;
  int m_CornerNumber;
  int m_PreviousNumber;
  int m_validationNumber;
  bool m_isPin;
  int m_number;
  std::vector<LHCb::CaloCellID> m_ids;
  int m_crate;
  int m_slot;
  int m_tell1;
  int m_selectionType; // [e,g,pi0L,pi0G] = -1 ; [hadronMaster] = 0 ; [hadronSlave1]=1 ; [hadronSlave2]=2
  Mapping m_mapping;  // see CardParam::Mapping  enum
};


/// ===========================================================================
#endif // CALODET_CARDPARAM_H
/// ===========================================================================
