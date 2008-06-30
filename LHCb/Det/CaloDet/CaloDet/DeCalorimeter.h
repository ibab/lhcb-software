// $Id: DeCalorimeter.h,v 1.33 2008-06-30 16:46:04 odescham Exp $ 
// ============================================================================
#ifndef       CALODET_DECALORIMETER_H
#define       CALODET_DECALORIMETER_H 1
// ============================================================================
/// from STL
#include <iostream>
#include <vector>
//From Kernel/LHCbDefintions
#include "GaudiKernel/Transform3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
/// from Det/DetDesc
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
// LHCbKernel
#include "Kernel/CaloCellID.h"
/// from CaloKernel
#include "CaloKernel/CaloVector.h"
#include "CaloKernel/CaloException.h"
/// from Det/CaloDet
#include "CaloDet/CaloCardParams.h"
#include "CaloDet/CellParam.h"
#include "CaloDet/CardParam.h"
#include "CaloDet/Tell1Param.h"
#include "CaloDet/CaloPin.h"
#include "CaloDet/CaloLed.h"
#include "CaloDet/CLIDDeCalorimeter.h"

/// forward declarations
class MsgStream        ;
class DeSubCalorimeter ;
class DeSubSubCalorimeter ;


namespace CaloPlane
{
  enum Plane {
    Front  = 0 , 
    Middle     ,
    ShowerMax  ,
    Back       
  } ;
}

/// Locations of DeCalorimeter detector elements in the TDS
namespace DeCalorimeterLocation {
  static const std::string Spd  = "/dd/Structure/LHCb/DownstreamRegion/Spd"  ;
  static const std::string Prs  = "/dd/Structure/LHCb/DownstreamRegion/Prs"  ;
  static const std::string Ecal = "/dd/Structure/LHCb/DownstreamRegion/Ecal" ;
  static const std::string Hcal = "/dd/Structure/LHCb/DownstreamRegion/Hcal" ;
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
public:
  typedef std::vector<const DeSubCalorimeter*>  SubCalos  ;
  typedef std::vector<const DeSubSubCalorimeter*>  SubSubCalos  ;
protected:
  typedef std::vector<DeSubCalorimeter*>        SubCalos_ ;
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
  ///  accessor to number of builded cells  ('virtual' cells associated to PIN are removed)
  unsigned int   numberOfCells () { return buildCells().isSuccess() ? m_cells.size() - numberOfPins() : 0 ;  };
  ///  accessor to number of builded FEcards 
  unsigned int   numberOfCards () { return buildCards().isSuccess() ? m_feCards.size() : 0 ; };
 ///  accessor to number of builded tell1Boards
  unsigned int   numberOfTell1s () { return buildTell1s().isSuccess() ?  m_tell1Boards.size() : 0 ; };
  ///  accessor to number of builded PIN-diodes
  unsigned int   numberOfPins () { return buildMonitoringSystem().isSuccess() ? m_pins.size() : 0; };
  ///  accessor to number of builded LEDs
  unsigned int   numberOfLeds () { return buildMonitoringSystem().isSuccess() ? m_leds.size() : 0; };
  ///  accessor to pinArea
  unsigned int   pinArea () { return m_pinArea; };
  ///  set function for coding
  void setCoding        ( const unsigned int nb     );
  // reference plane in the global frame 
  Gaudi::Plane3D plane ( const double           zLocal ) const ;
  inline 
  Gaudi::Plane3D plane ( const Gaudi::XYZPoint& point  ) const ;
  inline 
  Gaudi::Plane3D plane ( const CaloPlane::Plane pos    ) const ;
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
  void setZShowerMax    ( const double zShowerMax   ) 
  { m_zShowerMax    = zShowerMax; }
  void setZSize ( const double zSize ) 
  { m_zSize = zSize; }
  void setZOffset ( const double zOffset ) 
  { m_zOffset = zOffset; }
  void setPedestalShift( const double pedShift ) 
  { m_pedShift = pedShift; }
  void setPinPedestalShift( const double pedShift ) 
  { m_pinPedShift = pedShift; }

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
  double        zSize         () const { return m_zSize         ; };
  double        zOffset       () const { return m_zOffset       ; };  
  double        pedestalShift () const { return m_pedShift      ; };  
  double        pinPedestalShift () const { return m_pinPedShift  ; };  


  ///  validity flag for the cell 
  inline bool   valid    ( const LHCb::CaloCellID& ) const ;
  ///  x-position of center of the cell 
  inline double cellX    ( const LHCb::CaloCellID& ) const ;
  ///  y-position of center of the cell 
  inline double cellY    ( const LHCb::CaloCellID& ) const ;
  ///  z-position of center of the cell 
  inline double cellZ    ( const LHCb::CaloCellID& ) const ;
  ///  cell size  
  inline double cellSize ( const LHCb::CaloCellID& ) const ;
  ///  sine function for given cell 
  inline double cellSine ( const LHCb::CaloCellID& ) const ;
  ///  PM gain for given cell  
  inline double cellGain ( const LHCb::CaloCellID& ) const ;
  ///  Timing for for given cell  
  inline double cellTime ( const LHCb::CaloCellID& ) const ;
  ///  cell center 
  inline const Gaudi::XYZPoint& cellCenter       ( const LHCb::CaloCellID& ) const ;
  ///  list of neighbour cells 
  inline const CaloNeighbors& neighborCells    ( const LHCb::CaloCellID& ) const ;
  ///  list of neighbour cells 
  inline const CaloNeighbors& zsupNeighborCells( const LHCb::CaloCellID& ) const ;
  
  ///  From ID to cell serial number and vice-versa
  inline int        cellIndex    ( const LHCb::CaloCellID&  ) const ;
  inline LHCb::CaloCellID cellIdByIndex( const unsigned int ) const ;
  
  ///  More complex functions
  LHCb::CaloCellID  Cell    ( const Gaudi::XYZPoint& point ) const ;  

  // Get Param from a card number/Tell1 number or CellId 
  CardParam cardParam( int card ){return m_feCards[card];  }
  Tell1Param tell1Param(int tell1){return m_tell1Boards[tell1];  }
  CellParam cellParam(LHCb::CaloCellID id){ return m_cells[id]; }
  // Return collections
  CaloVector<CellParam>& cellParams(){return m_cells;}
  CaloVector<CaloPin>&  caloPins(){return m_pins;}
  std::vector<CaloLed>&  caloLeds(){return m_leds;}
  std::vector<CardParam>& cardParams(){return m_feCards;}
  std::vector<Tell1Param>& tell1Params(){return m_tell1Boards;}
    


  ///  accessor to pinFE flag for cards/Tell1/Id
  bool isPinCard (const int card) { return m_feCards[card].isPinCard(); };
  bool isPinTell1(const int tell1) { return m_tell1Boards[tell1].readPin(); };
  bool isPinId(LHCb::CaloCellID id){ return ((unsigned)m_pinArea == id.area()) ? true : false; }
  
  
    

  ///  Front-end card information
  inline bool isReadout(const LHCb::CaloCellID&  ) const;
  ///  card number 
  inline int cardNumber( const LHCb::CaloCellID& ) const ;
  ///  card row  
  inline int cardRow   ( const LHCb::CaloCellID& ) const ;
  ///  card column  
  inline int cardColumn( const LHCb::CaloCellID& ) const ;
  ///  card address 
  inline void cardAddress ( const LHCb::CaloCellID& ID   , 
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
  // Selection Board Type
  inline int selectionType      ( const int card   )  const ;
  ///  card area 
  inline int cardArea           ( const int card   )  const ;
  ///  card first row 
  inline int cardFirstRow       ( const int card   )  const ;
  ///  card last column 
  inline int cardLastColumn    ( const int card   )  const ;
  ///  card last row 
  inline int cardLastRow       ( const int card   )  const ;
  ///  card first column 
  inline int cardFirstColumn    ( const int card   )  const ;
  // Valid boundaries
  inline int cardFirstValidRow    ( const int card   )  const ;
  inline int cardLastValidRow     ( const int card   )  const ;
  inline int cardFirstValidColumn ( const int card   )  const ;
  inline int cardLastValidColumn  ( const int card   )  const ;


  ///  ID of the bottom left cell
  inline LHCb::CaloCellID firstCellID ( const int card   )  const ;
  ///  ID of of the top right cell
  inline LHCb::CaloCellID lastCellID  ( const int card   )  const ;
  ///  ID of the specified cell
  inline LHCb::CaloCellID cardCellID  ( const int card   , 
                                  const int row    , 
                                  const int col    )  const ;


  // number of Tell1s
  int nTell1s ( )  const { return m_tell1Boards.size(); }
  // Card -> Tell1
  inline int cardToTell1( const int card ) const ;
  // Card ID & content
  inline int cardCrate( const int card ) const  ;
  inline int cardSlot( const int card ) const ;
  inline int cardCode( const int card ) const ;
  inline std::vector<LHCb::CaloCellID>&  cardChannels( const int card )  ;  
  inline int cardIndexByCode( const int crate , const int slot );
  // Tell1 -> FECards
  inline std::vector<int> tell1ToCards(const int tell1 ) const;

  ///  More complex functions
  inline const CellParam* Cell_( const Gaudi::XYZPoint& point ) const ;

public:
  /// get constant access to subcalorimeters 
  const SubCalos&  subCalos() const { return m_subCalos ; }
protected:
  // get non-constant access to subcalorimeters 
  const SubCalos_& subCalos()       { return m_subCalos_ ; }
protected:
  ///  Initialization method for building the cells/cards/tell1/PIN layout
  StatusCode     buildCells    ();
  StatusCode     buildCards    ();
  StatusCode     buildTell1s    ();
  StatusCode buildMonitoringSystem();

private:
  DeCalorimeter( const DeCalorimeter& ) ;
private:
  
  ///  Flag, to compute the geometry only once
  bool     m_initialized;     
  ///  Flag, to compute the card layout only once  
  bool     m_cardsInitialized;
  ///  Flag, to compute the tell1 layout only once  
  bool     m_tell1sInitialized;
  ///  Flag, to build the Monitoring layout only once  
  bool     m_monitoringInitialized;
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
  ///  Pedestal shift
  double   m_pedShift;
  double   m_pinPedShift;
  ///  Convertion from activeE() to energy seen 
  double   m_activeToTotal;
  ///  Z of the shower maximum in the local frame.
  double   m_zShowerMax;

  ///  Collection of cells
  CaloVector<CellParam> m_cells;
  ///  Parameters for the FEcards
  std::vector<CardParam> m_feCards ;
  ///  Parameters for the tell1s
  std::vector<Tell1Param> m_tell1Boards ;

  // Collection of PIN-diodes/LEDs
  CaloVector<CaloPin> m_pins;
  std::vector<CaloLed> m_leds;
  int m_pinArea;
  std::vector<int> m_pinTell1s;

  /// Y to X ration, close to 1.
  double m_YToXSizeRatio;
  /// number of non-connected cells on both sides of the beam, horizontal
  int    m_centralHoleX;
  /// number of non-connected cells on both sides of the beam, vertical
  int    m_centralHoleY;

  double   m_zSize;
  double   m_zOffset;
  
  SubCalos   m_subCalos  ;
  SubCalos_  m_subCalos_ ;
  int m_codingBit;
  
} ;

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
inline bool DeCalorimeter::valid    ( const LHCb::CaloCellID& ID ) const 
{ return m_cells[ID].valid();}

// ===========================================================================
//  x-position of center of the cell
// ===========================================================================
inline double DeCalorimeter::cellX  ( const LHCb::CaloCellID& ID ) const 
{ return m_cells[ID].x (); }
//  ===========================================================================
//  y-position of center of the cell
//  ===========================================================================
inline double DeCalorimeter::cellY  ( const LHCb::CaloCellID& ID ) const 
{ return m_cells[ID].y (); }

// ===========================================================================
//  z-position of center of the cell
// ===========================================================================
inline double DeCalorimeter::cellZ  ( const LHCb::CaloCellID& ID ) const 
{ return m_cells[ID].z (); }

// ===========================================================================
//  cell size
// ===========================================================================
inline double DeCalorimeter::cellSize ( const LHCb::CaloCellID& ID ) const 
{ return m_cells[ID].size (); }

// ===========================================================================
//  sine function for given cell
// ===========================================================================
inline double DeCalorimeter::cellSine ( const LHCb::CaloCellID& ID ) const{ return m_cells[ID].sine (); }

// ===========================================================================
//  PM gain for given cell
// ===========================================================================
inline double DeCalorimeter::cellGain ( const LHCb::CaloCellID& ID ) const 
{ return m_cells[ID].gain (); }

// ===========================================================================
//  Timing for for given cell
// ===========================================================================
inline double DeCalorimeter::cellTime ( const LHCb::CaloCellID& ID ) const 
{ return m_cells[ID].time (); }

// ===========================================================================
//  cell center
// ===========================================================================
inline const Gaudi::XYZPoint& 
DeCalorimeter::cellCenter ( const LHCb::CaloCellID& ID ) const 
{ return m_cells[ID].center       (); }

// ============================================================================
//  list of neighbour cells
// ============================================================================
inline const CaloNeighbors& 
DeCalorimeter::neighborCells    ( const LHCb::CaloCellID& ID ) const 
{ return m_cells[ID].neighbors    (); }

// ============================================================================
//  list of neighbour cells
// ============================================================================
inline const CaloNeighbors& 
DeCalorimeter::zsupNeighborCells( const LHCb::CaloCellID& ID ) const 
{ return m_cells[ID].zsupNeighbors(); }

// ============================================================================
//  From ID to cell serial number and vice-versa
// ============================================================================
inline int    
DeCalorimeter::cellIndex( const LHCb::CaloCellID& ID ) const 
{ return m_cells.index(ID); }

// ============================================================================
//  From ID to cell serial number and vice-versa
// ============================================================================
inline LHCb::CaloCellID 
DeCalorimeter::cellIdByIndex( const unsigned int num )    const 
{
  return 
    ( (num < m_cells.size() ) ? (m_cells.begin()+num)->cellID() : LHCb::CaloCellID() );
};



inline bool 
DeCalorimeter::isReadout( const LHCb::CaloCellID& ID )const{
  return ( m_cells[ID].cardNumber() >=0 )? true : false ; 
}
// ============================================================================
//  card number 
// ============================================================================
inline int 
DeCalorimeter::cardNumber( const LHCb::CaloCellID& ID ) const
{return m_cells[ID].cardNumber(); }
// ============================================================================
//  card row  
// ============================================================================
inline int 
DeCalorimeter::cardRow   ( const LHCb::CaloCellID& ID ) const 
{return m_cells[ID].cardRow();    }
// ============================================================================
//  card column  
// ============================================================================
inline int 
DeCalorimeter::cardColumn( const LHCb::CaloCellID& ID ) const 
{return m_cells[ID].cardColumn(); }
// ============================================================================
//  card address 
// ============================================================================
inline void 
DeCalorimeter::cardAddress ( const LHCb::CaloCellID& ID     ,
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
//  selection board type
// ============================================================================
inline int DeCalorimeter::selectionType(const int card ) const
{  return m_feCards[card].selectionType();}

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

// ============================================================================
//  card last row 
// ============================================================================
inline int DeCalorimeter::cardLastRow   ( const int card ) const 
{ return m_feCards[card].lastRow();};

// ============================================================================
//  card last column 
// ============================================================================
inline int DeCalorimeter::cardLastColumn( const int card ) const 
{ return m_feCards[card].lastColumn(); };

// ============================================================================
//  valid boundaries :  first/last col/row with a valid cell
// ============================================================================
inline int DeCalorimeter::cardFirstValidRow( const int card ) const{ 
  LHCb::CaloCellID cell = firstCellID(card);
  if( valid( cell ) )return cell.row();

  for(int irow = cardFirstRow(card) ; irow <= cardLastRow(card) ; ++irow){
    for(int icol = cardFirstColumn(card) ; icol <= cardLastColumn(card) ; ++icol){
      LHCb::CaloCellID test(cell.calo(), cell.area(), irow, icol );
      if( valid( test ) )return irow;
    }
  }    
  return -1;
}

inline int DeCalorimeter::cardFirstValidColumn( const int card ) const{ 
  LHCb::CaloCellID cell = firstCellID(card);
  if( valid( cell ) )return cell.col();
  for(int irow = cardFirstRow(card) ; irow <= cardLastRow(card) ; ++irow){
    for(int icol = cardFirstColumn(card) ; icol <= cardLastColumn(card) ; ++icol){
      LHCb::CaloCellID test(cell.calo(), cell.area(), irow, icol );
      if( valid( test ) )return icol;
    }
  }
  return -1;
}

inline int DeCalorimeter::cardLastValidRow( const int card ) const{ 
  LHCb::CaloCellID cell = lastCellID(card);
  if( valid( cell ) )return cell.row();

  for(int irow = cardLastRow(card) ; irow >= cardFirstRow(card) ; --irow){
    for(int icol = cardLastColumn(card) ; icol >= cardFirstColumn(card) ; --icol){
      LHCb::CaloCellID test(cell.calo(), cell.area(), irow, icol);
      if( valid( test ) )return irow;
    }
  }
  return -1;
}
inline int DeCalorimeter::cardLastValidColumn( const int card ) const{ 
  LHCb::CaloCellID cell = lastCellID(card);
  if( valid( cell ) )return cell.col();

  for(int irow = cardLastRow(card) ; irow >= cardFirstRow(card) ; --irow){
    for(int icol = cardLastColumn(card) ; icol >= cardFirstColumn(card) ; --icol){
      LHCb::CaloCellID test(cell.calo(), cell.area(), irow, icol );
      if( valid( test ) )return icol;
    }
  }
  return -1;
}


// ===========================================================================
//  ID of the bottom left cell, of the top right cell, of the specified cell
// ===========================================================================
inline LHCb::CaloCellID DeCalorimeter::firstCellID ( const int card ) const 
{  
  return LHCb::CaloCellID( m_caloIndex                 ,
                     m_feCards[card].area       () , 
                     m_feCards[card].firstRow   () ,
                     m_feCards[card].firstColumn() ); 
};

// ===========================================================================
//  ID of the bottom left cell, of the top right cell, of the specified cell
// ===========================================================================
inline LHCb::CaloCellID DeCalorimeter::lastCellID ( const int card ) const 
{
  return LHCb::CaloCellID( m_caloIndex                                   ,
                     m_feCards[card].area       ()                   , 
                     m_feCards[card].firstRow   () + nRowCaloCard -1 ,
                     m_feCards[card].firstColumn() + nColCaloCard -1 ); 
};

// ===========================================================================
//  ID of the bottom left cell, of the top right cell, of the specified cell
// ===========================================================================
inline LHCb::CaloCellID DeCalorimeter::cardCellID ( const int card , 
                                              const int row  , 
                                              const int col ) const 
{
  return LHCb::CaloCellID( m_caloIndex                       ,
                     m_feCards[card].area       ()       , 
                     m_feCards[card].firstRow   () + row ,
                     m_feCards[card].firstColumn() + col ); 
};

// ============================================================================
//  FEcard -> Tell1 
// ============================================================================
inline int DeCalorimeter::cardToTell1( const int card ) const 
{
  return m_feCards[card].tell1();
}
// ============================================================================
//  Tell1 -> FECards 
// ============================================================================
inline std::vector<int> DeCalorimeter::tell1ToCards( const int tell1 ) const 
{
  return m_tell1Boards[tell1].feCards();
}

// ============================================================================
//  card Id & content
// ============================================================================
inline int DeCalorimeter::cardCrate( const int card ) const 
{ return m_feCards[card].crate(); };
inline int DeCalorimeter::cardSlot( const int card ) const 
{ return m_feCards[card].slot(); };
inline int DeCalorimeter::cardCode( const int card ) const 
{ return m_feCards[card].code(); };
inline std::vector<LHCb::CaloCellID>&  DeCalorimeter::cardChannels( const int card ) 
{ return m_feCards[card].ids(); };

inline int DeCalorimeter::cardIndexByCode( const int crate, const int slot ) 
{ 
  for (unsigned int icard = 0 ; icard < m_feCards.size() ;  ++icard){
    if(m_feCards[icard].crate() == crate && 
       m_feCards[icard].slot() == slot )return icard;
  }
  return -1;
};



// ============================================================================
#endif  //    CALODET_DECALORIMETER_H
// ============================================================================
// CaloDet 
// ============================================================================
#include "CaloDet/DeCalorimeter.icpp"
// ============================================================================
