// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2001/06/29 10:26:17  ibelyaev
// update to use new features of DetDesc v7 package
// 
// ============================================================================
#ifndef       CALODET_DECALORIMETER_H
#define       CALODET_DECALORIMETER_H 1
// ============================================================================
/// from STL
#include <iostream>
#include <vector>

/// from CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

/// GaudiKernel
#include "GaudiKernel/MsgStream.h"

/// from Det/DetDesc
#include "DetDesc/DetectorElement.h"

/// from Event/CaloGen
#include "CaloGen/CaloCellID.h"
#include "CaloGen/CaloVector.h"
#include "CaloGen/CaloException.h"

/// from Det/CaloDet
#include "CaloDet/CaloCardParams.h"
#include "CaloDet/CLIDDeCalorimeter.h"
#include "CaloDet/CellParam.h"
#include "CaloDet/CardParam.h"


/// forwad declarations
class MsgStream;

//-----------------------------------------------------------------------------
//
// ClassName:   DeCalorimeter 
//  
// Description: Calorimeter detector element class.
//
//-----------------------------------------------------------------------------


class DeCalorimeter: public DetectorElement {

public:
  
  /// Constructors
  DeCalorimeter( const std::string& name    = "" );
  
  /// (dirtual) Destructor
  virtual ~DeCalorimeter() ;
  
  /// object identification
  static  const CLID& classID ()       { return CLID_DeCalorimeter ; } 
  /// object identification
  virtual const CLID& clID    () const ;
  
  /// printout to std::ostream 
  virtual std::ostream& printOut( std::ostream& s = std::cerr ) const;
  /// printout to MsgStream 
  virtual MsgStream&    printOut( MsgStream&                  ) const;

  /// initialization method 
  virtual StatusCode initialize(); 
 
  /// if initialized in a proper way?
  inline  bool  isInitialized() const  { return m_initialized ; }  
  
  /// accessor to number of builded cells 
  unsigned int   numberOfCells () { buildCells() ; return cells.size(); };

  /// set function for coding 
  void setCoding     ( unsigned int nb );
  /// set function for maxEt 
  void setEtInCenter ( double maxEt         ) { m_maxEtInCenter = maxEt; }
  /// set function for maxEtSlope  
  void setEtSlope    ( double maxEtSlope    ) { m_maxEtSlope    = maxEtSlope; }
  /// set function for AdcMax 
  void setAdcMax     ( unsigned int adcMax  ) { m_adcMax        = adcMax; }
  /// set function for active/total ratio 
  void setActiveToTotal (double actToTotal  ) { m_activeToTotal = actToTotal; }
  /// set function for ZshowerMax 
  void setZShowerMax ( double ZShowerMax    ) { m_zShowerMax    = ZShowerMax; }

  /// retrieve max et in center  
  double        maxEtInCenter () const { return m_maxEtInCenter ; }; 
  /// retrieve max et slope 
  double        maxEtSlope    () const { return m_maxEtSlope    ; }; 
  /// retrieve adcMax 
  unsigned int  adcMax        () const { return m_adcMax        ; }; 
  /// retrieve active/total energy ratio 
  double        activeToTotal () const { return m_activeToTotal ; };
  /// retrieve position of shower max 
  double        zShowerMax    () const { return m_zShowerMax    ; };
  
  /// validity flag for the cell 
  inline bool   valid    ( const CaloCellID& ) const ;
  /// x-position of center of the cell 
  inline double cellX    ( const CaloCellID& ) const ;
  /// y-position of center of the cell 
  inline double cellY    ( const CaloCellID& ) const ;
  /// z-position of center of the cell 
  inline double cellZ    ( const CaloCellID& ) const ;
  /// cell size  
  inline double cellSize ( const CaloCellID& ) const ;
  /// sine function for given cell 
  inline double cellSine ( const CaloCellID& ) const ;
  /// PM gain for given cell  
  inline double cellGain ( const CaloCellID& ) const ;
  /// Timing for for given cell  
  inline double cellTime ( const CaloCellID& ) const ;
  /// cell center 
  inline const HepPoint3D&    cellCenter       ( const CaloCellID& ) const ;
  /// list of neighbour cells 
  inline const CaloNeighbors& neighborCells    ( const CaloCellID& ) const ;
  /// list of neighbour cells 
  inline const CaloNeighbors& zsupNeighborCells( const CaloCellID& ) const ;
  
  /// From ID to cell serial number and vice-versa
  inline int        cellIndex    ( const CaloCellID&  ) const ;
  inline CaloCellID cellIdByIndex( const unsigned int ) const ;
  
  /// More complex functions
  CaloCellID  Cell    ( const HepPoint3D& point )             ;
  
  /// Front-end card information
  /// card number 
  inline int cardNumber( const CaloCellID& ) const ;
  /// card row  
  inline int cardRow   ( const CaloCellID& ) const ;
  /// card column  
  inline int cardColumn( const CaloCellID& ) const ;
  /// card address 
  inline void cardAddress ( const CaloCellID& ID   , 
                            int &             card , 
                            int &             row  , 
                            int &             col  ) const ;
  
  /// number of cards 
  int nCards          ( )                 const { return m_cards; }
  /// down card number 
  inline int downCardNumber     ( const int card   )  const ;
  /// left card number 
  inline int leftCardNumber     ( const int card   )  const ;
  /// corder card number 
  inline int cornerCardNumber   ( const int card   )  const ;
  /// previous card number 
  inline int previousCardNumber ( const int card   )  const ;
  /// card neighbours
  inline void cardNeighbors     ( const int card   , 
                                  int&      down   , 
                                  int&      left   ,  
                                  int&      corner )  const ;
  /// card area 
  inline int cardArea           ( const int card   )  const ;
  /// card first row 
  inline int cardFirstRow       ( const int card   )  const ;
  /// card first column 
  inline int cardFirstColumn    ( const int card   )  const ;
  /// ID of the bottom left cell
  inline CaloCellID firstCellID ( const int card   )  const ;
  /// ID of of the top right cell
  inline CaloCellID lastCellID  ( const int card   )  const ;
  /// ID of the specified cell
  inline CaloCellID cardCellID  ( const int card   , 
                                  const int row    , 
                                  const int col    )  const ;
  ///
protected: 
  
  /// Initialization method for building the cells 
  StatusCode     buildCells    ();
  /// Initialization method for building the cards 
  StatusCode     buildCards    ();
  
protected: 
  
  /// assertion
  inline void Assert( bool , const char*         ) const ; 
  /// assertion
  inline void Assert( bool , const std::string&  ) const ; 
  
private:
  
  /// Flag, to compute the geometry only once
  bool     m_initialized;     
  /// Flag, to compute the card layout only once  
  bool     m_cardsInitialized;
  /// Calorimeter index, to code CellIDs
  int      m_caloIndex;   
  /// Maximum value for the Row / Column =31 or 63
  int      maxRowCol;  
  /// First row or column over center,    16 or 64
  int      firstRowUp;       
  /// Central value = maxRowCol/2,      15.5 or 31.5
  double   centerRowCol;    
  
  /// Et value for the maximum ADC value at teta = 0 
 double   m_maxEtInCenter;    
  
  /**  Increase in Et per radian. 
       This is also the maximum E if m_maxEtInCenter = 0
  */
  double   m_maxEtSlope; 
  /// maximum codage in the ADC, 4095 or 1023
  int      m_adcMax;  
  /// Convertion from activeE() to energy seen 
  double   m_activeToTotal;
  /// Z of the shower maximum in the local frame.
  double   m_zShowerMax;
  /// Collection of cells
  CaloVector<CellParam> cells;
  /// Parameters for the cards
  int m_cards;
  /// Parameters for the cards
  std::vector<CardParam> feCards ;
  
};


/// ===========================================================================
/// ouput operator 
/// ===========================================================================
inline std::ostream&  operator<<( std::ostream& os , const DeCalorimeter& de )
{ return de.printOut( os ); } 

/// ===========================================================================
/// ouput operator 
/// ===========================================================================
inline std::ostream&  operator<<( std::ostream& os , const DeCalorimeter* de )
{ return de ? (os<<*de) : (os<<" DeCalorimeter* points to NULL!"<<std::endl); }

/// ===========================================================================
/// ouput operator 
/// ===========================================================================
inline MsgStream&     operator<<( MsgStream&    os , const DeCalorimeter& de )
{ return de.printOut( os ); } 

/// ===========================================================================
/// ouput operator 
/// ===========================================================================
inline MsgStream&     operator<<( MsgStream&    os , const DeCalorimeter* de )
{ return de ? (os<<*de) : (os<<" DeCalorimeter* points to NULL!"<<endreq   ); }

/// ===========================================================================
/// validity flag for the cell
/// ===========================================================================
inline bool DeCalorimeter::valid    ( const CaloCellID& ID ) const 
{ return cells[ID].size() >0;}

/// ===========================================================================
/// x-position of center of the cell
/// ===========================================================================
inline double DeCalorimeter::cellX  ( const CaloCellID& ID ) const 
{ return cells[ID].x (); }
/// ===========================================================================
/// y-position of center of the cell
/// ===========================================================================
inline double DeCalorimeter::cellY  ( const CaloCellID& ID ) const 
{ return cells[ID].y (); }

/// ===========================================================================
/// z-position of center of the cell
/// ===========================================================================
inline double DeCalorimeter::cellZ  ( const CaloCellID& ID ) const 
{ return cells[ID].z (); }

/// ===========================================================================
/// cell size
/// ===========================================================================
inline double DeCalorimeter::cellSize ( const CaloCellID& ID ) const 
{ return cells[ID].size (); }

/// ===========================================================================
/// sine function for given cell
/// ===========================================================================
inline double DeCalorimeter::cellSine ( const CaloCellID& ID ) const 
{ return cells[ID].sine (); }

/// ===========================================================================
/// PM gain for given cell
/// ===========================================================================
inline double DeCalorimeter::cellGain ( const CaloCellID& ID ) const 
{ return cells[ID].gain (); }

/// ===========================================================================
/// Timing for for given cell
/// ===========================================================================
inline double DeCalorimeter::cellTime ( const CaloCellID& ID ) const 
{ return cells[ID].time (); }

/// ===========================================================================
/// cell center
/// ===========================================================================
inline const HepPoint3D& 
DeCalorimeter::cellCenter ( const CaloCellID& ID ) const 
{ return cells[ID].center       (); }

/// ===========================================================================
/// list of neighbour cells
/// ===========================================================================
inline const CaloNeighbors& 
DeCalorimeter::neighborCells    ( const CaloCellID& ID ) const 
{ return cells[ID].neighbors    (); }

/// ===========================================================================
/// list of neighbour cells
/// ===========================================================================
inline const CaloNeighbors& 
DeCalorimeter::zsupNeighborCells( const CaloCellID& ID ) const 
{ return cells[ID].zsupNeighbors(); }

/// ===========================================================================
/// From ID to cell serial number and vice-versa
/// ===========================================================================
inline int    
DeCalorimeter::cellIndex( const CaloCellID& ID ) const 
{ return cells.index(ID); }

/// ===========================================================================
/// From ID to cell serial number and vice-versa
/// ===========================================================================
inline CaloCellID 
DeCalorimeter::cellIdByIndex( const unsigned int num )    const 
{
  return 
    ( (num < cells.size() ) ? (cells.begin()+num)->cellID() : CaloCellID() );
};

/// ===========================================================================
/// card number 
/// ===========================================================================
inline int 
DeCalorimeter::cardNumber( const CaloCellID& ID ) const
{return cells[ID].cardNumber(); }
/// ===========================================================================
/// card row  
/// ===========================================================================
inline int 
DeCalorimeter::cardRow   ( const CaloCellID& ID ) const 
{return cells[ID].cardRow();    }
/// ===========================================================================
/// card column  
/// ===========================================================================
inline int 
DeCalorimeter::cardColumn( const CaloCellID& ID ) const 
{return cells[ID].cardColumn(); }
/// ===========================================================================
/// card address 
/// ===========================================================================
inline void 
DeCalorimeter::cardAddress ( const CaloCellID& ID     ,
                             int &             card   , 
                             int &             row    , 
                             int &             column ) const 
{
  card   = cells[ID].cardNumber ();
  row    = cells[ID].cardRow() ;
  column = cells[ID].cardColumn() ;
};

/// ===========================================================================
/// down card number 
/// ===========================================================================
inline int DeCalorimeter::downCardNumber  ( const int card )  const 
{ return feCards[card].downNumber(); }

/// ===========================================================================
/// left card number 
/// ===========================================================================
inline int DeCalorimeter::leftCardNumber  ( const int card )  const 
{ return feCards[card].leftNumber(); }

/// ===========================================================================
/// corder card number 
/// ===========================================================================
inline int DeCalorimeter::cornerCardNumber( const int card )  const 
{ return feCards[card].cornerNumber();}

/// ===========================================================================
/// previous card number 
/// ===========================================================================
inline int DeCalorimeter::previousCardNumber(const int card ) const
{  return feCards[card].previousNumber();}

/// ===========================================================================
/// card neighbours
/// ===========================================================================
inline void DeCalorimeter::cardNeighbors ( const int card, 
                                           int  & down, 
                                           int  & left, 
                                           int  & corner )const
{
  down   = feCards[card].downNumber();
  left   = feCards[card].leftNumber();
  corner = feCards[card].cornerNumber();
};

/// ===========================================================================
/// card area 
/// ===========================================================================
inline int DeCalorimeter::cardArea       ( const int card ) const 
{ return feCards[card].area(); };

/// ===========================================================================
/// card first row 
/// ===========================================================================
inline int DeCalorimeter::cardFirstRow   ( const int card ) const 
{ return feCards[card].firstRow();};

/// ===========================================================================
/// card first column 
/// ===========================================================================
inline int DeCalorimeter::cardFirstColumn( const int card ) const 
{ return feCards[card].firstColumn(); };

/// ===========================================================================
/// ID of the bottom left cell, of the top right cell, of the specified cell
/// ===========================================================================
inline CaloCellID DeCalorimeter::firstCellID ( const int card ) const 
{  
  return CaloCellID( m_caloIndex                 ,
                     feCards[card].area       () , 
                     feCards[card].firstRow   () ,
                     feCards[card].firstColumn() ); 
};

/// ===========================================================================
/// ID of the bottom left cell, of the top right cell, of the specified cell
/// ===========================================================================
inline CaloCellID DeCalorimeter::lastCellID ( const int card ) const 
{
  return CaloCellID( m_caloIndex                                   ,
                     feCards[card].area       ()                   , 
                     feCards[card].firstRow   () + nRowCaloCard -1 ,
                     feCards[card].firstColumn() + nColCaloCard -1 ); 
};

/// ===========================================================================
/// ID of the bottom left cell, of the top right cell, of the specified cell
/// ===========================================================================
inline CaloCellID DeCalorimeter::cardCellID ( const int card , 
                                              const int row  , 
                                              const int col ) const 
{
  return CaloCellID( m_caloIndex                       ,
                     feCards[card].area       ()       , 
                     feCards[card].firstRow   () + row ,
                     feCards[card].firstColumn() + col ); 
};


//----------------------------------------------------------------------------
// Explicit inline methods, those needing more than a single statement...
//----------------------------------------------------------------------------
inline void DeCalorimeter::Assert( bool assertion , 
                                   const char* message ) const {
  if( !assertion ) {
    std::string msg("DeCalorimeter:: ");  
    throw CaloException( msg += message  );  
  }
};
inline void DeCalorimeter::Assert( bool assertion , 
                                   const std::string&  message ) const {
  if( !assertion ) {
    std::string msg("DeCalorimeter:: ");  
    throw CaloException( msg += message  );  
  }
};

// ============================================================================
#endif  //    CALODET_DECALORIMETER_H
// ============================================================================



















