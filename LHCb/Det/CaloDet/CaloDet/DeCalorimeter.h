// $Id: DeCalorimeter.h,v 1.14 2005-12-02 14:52:46 ocallot Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.13  2005/07/06 15:30:24  ibelyaev
//  minor fixes to please  LCGdict
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
// LHCbKernel
#include "Kernel/CaloCellID.h"
/// from CaloKernel
#include "CaloKernel/CaloVector.h"
#include "CaloKernel/CaloException.h"
/// from Det/CaloDet
#include "CaloDet/CaloCardParams.h"
#include "CaloDet/CellParam.h"
#include "CaloDet/CardParam.h"
#include "CaloDet/CLIDDeCalorimeter.h"

/// forwad declarations
class MsgStream;

namespace DeCalorimeterLocation {
  static const std::string Spd  = "/dd/Structure/LHCb/Spd"  ;
  static const std::string Prs  = "/dd/Structure/LHCb/Prs"  ;
  static const std::string Ecal = "/dd/Structure/LHCb/Ecal" ;
  static const std::string Hcal = "/dd/Structure/LHCb/Hcal" ;
}

/** @class DeCalorimeter DeCalorimeter.h CaloDet/DeCalorimeter.h 
 *
 *  Calorimeter detector element class.
 *
 *  @author Olivier Callot Olivier.Callot@cern.ch
 *  @author Vanya Belyaev  Ivan.Belyaev@itep.ru  
 */
class DeCalorimeter: public DetectorElement 
{
private:
  
  DeCalorimeter( const DeCalorimeter& ) ;
  
public:
  
  ///  Constructors
  DeCalorimeter( const std::string& name    = "" );
  
  ///  (dirtual) Destructor
  virtual ~DeCalorimeter() ;
  
  ///  object identification
  static  const CLID& classID ()       { return CLID_DeCalorimeter ; } 
  ///  object identification
  virtual const CLID& clID    () const ;
  
  ///  printout to std::ostream 
  virtual std::ostream& printOut( std::ostream& s = std::cerr ) const;
  ///  printout to MsgStream 
  virtual MsgStream&    printOut( MsgStream&                  ) const;

  ///  initialization method 
  virtual StatusCode initialize(); 
 
  ///  if initialized in a proper way?
  inline  bool  isInitialized() const  { return m_initialized ; }  
  
  ///  accessor to number of builded cells 
  unsigned int   numberOfCells () { buildCells() ; return m_cells.size(); };

  ///  set function for coding 
  void setCoding        ( const unsigned int nb     );
  ///  set function for maxEt 
  void setEtInCenter    ( const double maxEt        ) 
  { m_maxEtInCenter = maxEt; }
  ///  set function for maxEtSlope  
  void setEtSlope       ( const double maxEtSlope   ) 
  { m_maxEtSlope    = maxEtSlope; }
  ///  set function for AdcMax 
  void setAdcMax        ( const unsigned int adcMax ) 
  { m_adcMax        = adcMax; }
  ///  set function for active/total ratio 
  void setActiveToTotal ( const double actToTotal   ) 
  { m_activeToTotal = actToTotal; }
  ///  set function for ZshowerMax 
  void setZShowerMax    ( const double ZShowerMax   ) 
  { m_zShowerMax    = ZShowerMax; }
  
  ///  retrieve max et in center  
  double        maxEtInCenter () const { return m_maxEtInCenter ; }; 
  ///  retrieve max et slope 
  double        maxEtSlope    () const { return m_maxEtSlope    ; }; 
  ///  retrieve adcMax 
  unsigned int  adcMax        () const { return m_adcMax        ; }; 
  ///  retrieve active/total energy ratio 
  double        activeToTotal () const { return m_activeToTotal ; };
  ///  retrieve position of shower max 
  double        zShowerMax    () const { return m_zShowerMax    ; };
  
  ///  validity flag for the cell 
  inline bool   valid    ( const CaloCellID& ) const ;
  ///  x-position of center of the cell 
  inline double cellX    ( const CaloCellID& ) const ;
  ///  y-position of center of the cell 
  inline double cellY    ( const CaloCellID& ) const ;
  ///  z-position of center of the cell 
  inline double cellZ    ( const CaloCellID& ) const ;
  ///  cell size  
  inline double cellSize ( const CaloCellID& ) const ;
  ///  sine function for given cell 
  inline double cellSine ( const CaloCellID& ) const ;
  ///  PM gain for given cell  
  inline double cellGain ( const CaloCellID& ) const ;
  ///  Timing for for given cell  
  inline double cellTime ( const CaloCellID& ) const ;
  ///  cell center 
  inline const HepPoint3D&    cellCenter       ( const CaloCellID& ) const ;
  ///  list of neighbour cells 
  inline const CaloNeighbors& neighborCells    ( const CaloCellID& ) const ;
  ///  list of neighbour cells 
  inline const CaloNeighbors& zsupNeighborCells( const CaloCellID& ) const ;
  
  ///  From ID to cell serial number and vice-versa
  inline int        cellIndex    ( const CaloCellID&  ) const ;
  inline CaloCellID cellIdByIndex( const unsigned int ) const ;
  
  ///  More complex functions
  CaloCellID  Cell    ( const HepPoint3D& point ) const ;
  
  ///  Front-end card information
  ///  card number 
  inline int cardNumber( const CaloCellID& ) const ;
  ///  card row  
  inline int cardRow   ( const CaloCellID& ) const ;
  ///  card column  
  inline int cardColumn( const CaloCellID& ) const ;
  ///  card address 
  inline void cardAddress ( const CaloCellID& ID   , 
                            int &             card , 
                            int &             row  , 
                            int &             col  ) const ;
  
  ///  number of cards 
  int nCards ( )          const { return m_feCards.size(); }
  ///  down card number (to which data is sent)
  inline int downCardNumber     ( const int card   )  const ;
  ///  left card number (to which data is sent)
  inline int leftCardNumber     ( const int card   )  const ;
  ///  corder card number (to which data is sent) 
  inline int cornerCardNumber   ( const int card   )  const ;
  ///  previous card number (for global pi0 computation)
  inline int previousCardNumber ( const int card   )  const ;
  ///  card neighbours
  inline void cardNeighbors     ( const int card   , 
                                  int&      down   , 
                                  int&      left   ,  
                                  int&      corner )  const ;
  ///  Validation card number
  inline int validationNumber   ( const int card   )  const ;
  ///  card area 
  inline int cardArea           ( const int card   )  const ;
  ///  card first row 
  inline int cardFirstRow       ( const int card   )  const ;
  ///  card first column 
  inline int cardFirstColumn    ( const int card   )  const ;
  ///  ID of the bottom left cell
  inline CaloCellID firstCellID ( const int card   )  const ;
  ///  ID of of the top right cell
  inline CaloCellID lastCellID  ( const int card   )  const ;
  ///  ID of the specified cell
  inline CaloCellID cardCellID  ( const int card   , 
                                  const int row    , 
                                  const int col    )  const ;
  ///
protected: 
  
  ///  Initialization method for building the cells 
  StatusCode     buildCells    ();
  ///  Initialization method for building the cards 
  StatusCode     buildCards    ();
  
protected: 
  
  ///  assertion
  inline void Assert( bool , const char*         ) const ; 
  ///  assertion
  inline void Assert( bool , const std::string&  ) const ; 
  
private:
  
  ///  Flag, to compute the geometry only once
  bool     m_initialized;     
  ///  Flag, to compute the card layout only once  
  bool     m_cardsInitialized;
  ///  Calorimeter index, to code CellIDs
  int      m_caloIndex;   
  ///  Maximum value for the Row / Column =31 or 63
  int      m_maxRowCol;  
  ///  First row or column over center,    16 or 64
  int      m_firstRowUp;       
  ///  Central value = m_maxRowCol/2,      15.5 or 31.5
  double   m_centerRowCol;    
  
  ///  Et value for the maximum ADC value at teta = 0 
 double   m_maxEtInCenter;    
  
  /**  Increase in Et per radian. 
   *   This is also the maximum E if m_maxEtInCenter = 0
   */
  double   m_maxEtSlope; 
  ///  maximum codage in the ADC, 4095 or 1023
  int      m_adcMax;  
  ///  Convertion from activeE() to energy seen 
  double   m_activeToTotal;
  ///  Z of the shower maximum in the local frame.
  double   m_zShowerMax;
  ///  Collection of cells
  CaloVector<CellParam> m_cells;
  ///  Parameters for the cards
  std::vector<CardParam> m_feCards ;
  
  /// Y to X ration, close to 1.
  double m_YToXSizeRatio;
  /// number of non-connected cells on both sides of the beam, horizontal
  int    m_centralHoleX;
  /// number of non-connected cells on both sides of the beam, vertical
  int    m_centralHoleY;
  
};

// ===========================================================================
/** ouput operator for class DeCalorimeter
 *  @see DeCalorimeter 
 *  @param os reference to standard STL/STD output stream 
 *  @param de reference to DeCalorimeter object 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
inline std::ostream&  operator<<( std::ostream& os , const DeCalorimeter& de )
{ return de.printOut( os ); } 
// ===========================================================================

// ===========================================================================
/** ouput operator for class DeCalorimeter
 *  @see DeCalorimeter 
 *  @param os reference to standard STL/STD output stream 
 *  @param de pointer to DeCalorimeter object 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
inline std::ostream&  operator<<( std::ostream& os , const DeCalorimeter* de )
{ return de ? (os<<*de) : (os<<" DeCalorimeter* points to NULL!"<<std::endl); }
// ===========================================================================

// ===========================================================================
/** ouput operator for class DeCalorimeter
 *  @see DeCalorimeter 
 *  @see MsgStream
 *  @param os reference to Gaudi message output stream 
 *  @param de reference to DeCalorimeter object 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
inline MsgStream&     operator<<( MsgStream&    os , const DeCalorimeter& de )
{ return de.printOut( os ); } 
// ===========================================================================

// ===========================================================================
/** ouput operator for class DeCalorimeter
 *  @see DeCalorimeter 
 *  @see MsgStream
 *  @param os reference to Gaudi message output stream 
 *  @param de pointer to DeCalorimeter object 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
inline MsgStream&     operator<<( MsgStream&    os , const DeCalorimeter* de )
{ return de ? (os<<*de) : (os<<" DeCalorimeter* points to NULL!"<<endreq   ); }
// ===========================================================================

// ===========================================================================
//  validity flag for the cell
// ===========================================================================
inline bool DeCalorimeter::valid    ( const CaloCellID& ID ) const 
{ return m_cells[ID].size() >0;}

// ===========================================================================
//  x-position of center of the cell
// ===========================================================================
inline double DeCalorimeter::cellX  ( const CaloCellID& ID ) const 
{ return m_cells[ID].x (); }
//  ===========================================================================
//  y-position of center of the cell
//  ===========================================================================
inline double DeCalorimeter::cellY  ( const CaloCellID& ID ) const 
{ return m_cells[ID].y (); }

// ===========================================================================
//  z-position of center of the cell
// ===========================================================================
inline double DeCalorimeter::cellZ  ( const CaloCellID& ID ) const 
{ return m_cells[ID].z (); }

// ===========================================================================
//  cell size
// ===========================================================================
inline double DeCalorimeter::cellSize ( const CaloCellID& ID ) const 
{ return m_cells[ID].size (); }

// ===========================================================================
//  sine function for given cell
// ===========================================================================
inline double DeCalorimeter::cellSine ( const CaloCellID& ID ) const 
{ return m_cells[ID].sine (); }

// ===========================================================================
//  PM gain for given cell
// ===========================================================================
inline double DeCalorimeter::cellGain ( const CaloCellID& ID ) const 
{ return m_cells[ID].gain (); }

// ===========================================================================
//  Timing for for given cell
// ===========================================================================
inline double DeCalorimeter::cellTime ( const CaloCellID& ID ) const 
{ return m_cells[ID].time (); }

// ===========================================================================
//  cell center
// ===========================================================================
inline const HepPoint3D& 
DeCalorimeter::cellCenter ( const CaloCellID& ID ) const 
{ return m_cells[ID].center       (); }

// ============================================================================
//  list of neighbour cells
// ============================================================================
inline const CaloNeighbors& 
DeCalorimeter::neighborCells    ( const CaloCellID& ID ) const 
{ return m_cells[ID].neighbors    (); }

// ============================================================================
//  list of neighbour cells
// ============================================================================
inline const CaloNeighbors& 
DeCalorimeter::zsupNeighborCells( const CaloCellID& ID ) const 
{ return m_cells[ID].zsupNeighbors(); }

// ============================================================================
//  From ID to cell serial number and vice-versa
// ============================================================================
inline int    
DeCalorimeter::cellIndex( const CaloCellID& ID ) const 
{ return m_cells.index(ID); }

// ============================================================================
//  From ID to cell serial number and vice-versa
// ============================================================================
inline CaloCellID 
DeCalorimeter::cellIdByIndex( const unsigned int num )    const 
{
  return 
    ( (num < m_cells.size() ) ? (m_cells.begin()+num)->cellID() : CaloCellID() );
};

// ============================================================================
//  card number 
// ============================================================================
inline int 
DeCalorimeter::cardNumber( const CaloCellID& ID ) const
{return m_cells[ID].cardNumber(); }
// ============================================================================
//  card row  
// ============================================================================
inline int 
DeCalorimeter::cardRow   ( const CaloCellID& ID ) const 
{return m_cells[ID].cardRow();    }
// ============================================================================
//  card column  
// ============================================================================
inline int 
DeCalorimeter::cardColumn( const CaloCellID& ID ) const 
{return m_cells[ID].cardColumn(); }
// ============================================================================
//  card address 
// ============================================================================
inline void 
DeCalorimeter::cardAddress ( const CaloCellID& ID     ,
                             int &             card   , 
                             int &             row    , 
                             int &             column ) const 
{
  card   = m_cells[ID].cardNumber ();
  row    = m_cells[ID].cardRow() ;
  column = m_cells[ID].cardColumn() ;
};

// ============================================================================
//  down card number 
// ============================================================================
inline int DeCalorimeter::downCardNumber  ( const int card )  const 
{ return m_feCards[card].downNumber(); }

// ============================================================================
//  left card number 
// ============================================================================
inline int DeCalorimeter::leftCardNumber  ( const int card )  const 
{ return m_feCards[card].leftNumber(); }

// ============================================================================
//  corder card number 
// ============================================================================
inline int DeCalorimeter::cornerCardNumber( const int card )  const 
{ return m_feCards[card].cornerNumber();}

// ============================================================================
//  previous card number 
// ============================================================================
inline int DeCalorimeter::previousCardNumber(const int card ) const
{  return m_feCards[card].previousNumber();}

// ============================================================================
//  validation card number 
// ============================================================================
inline int DeCalorimeter::validationNumber(const int card ) const
{  return m_feCards[card].validationNumber();}

// ============================================================================
//  card neighbours
// ============================================================================
inline void DeCalorimeter::cardNeighbors ( const int card, 
                                           int  & down, 
                                           int  & left, 
                                           int  & corner )const
{
  down   = m_feCards[card].downNumber();
  left   = m_feCards[card].leftNumber();
  corner = m_feCards[card].cornerNumber();
};

// ============================================================================
//  card area 
// ============================================================================
inline int DeCalorimeter::cardArea       ( const int card ) const 
{ return m_feCards[card].area(); };

// ============================================================================
//  card first row 
// ============================================================================
inline int DeCalorimeter::cardFirstRow   ( const int card ) const 
{ return m_feCards[card].firstRow();};

// ============================================================================
//  card first column 
// ============================================================================
inline int DeCalorimeter::cardFirstColumn( const int card ) const 
{ return m_feCards[card].firstColumn(); };

// ===========================================================================
//  ID of the bottom left cell, of the top right cell, of the specified cell
// ===========================================================================
inline CaloCellID DeCalorimeter::firstCellID ( const int card ) const 
{  
  return CaloCellID( m_caloIndex                 ,
                     m_feCards[card].area       () , 
                     m_feCards[card].firstRow   () ,
                     m_feCards[card].firstColumn() ); 
};

// ===========================================================================
//  ID of the bottom left cell, of the top right cell, of the specified cell
// ===========================================================================
inline CaloCellID DeCalorimeter::lastCellID ( const int card ) const 
{
  return CaloCellID( m_caloIndex                                   ,
                     m_feCards[card].area       ()                   , 
                     m_feCards[card].firstRow   () + nRowCaloCard -1 ,
                     m_feCards[card].firstColumn() + nColCaloCard -1 ); 
};

// ===========================================================================
//  ID of the bottom left cell, of the top right cell, of the specified cell
// ===========================================================================
inline CaloCellID DeCalorimeter::cardCellID ( const int card , 
                                              const int row  , 
                                              const int col ) const 
{
  return CaloCellID( m_caloIndex                       ,
                     m_feCards[card].area       ()       , 
                     m_feCards[card].firstRow   () + row ,
                     m_feCards[card].firstColumn() + col ); 
};


// ============================================================================
// Explicit inline methods, those needing more than a single statement...
// ============================================================================
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
// The End 
// ============================================================================
#endif  //    CALODET_DECALORIMETER_H
// ============================================================================



















