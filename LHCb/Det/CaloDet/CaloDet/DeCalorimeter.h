// $Log: not supported by cvs2svn $ 
#ifndef       CALODET_DECALORIMETER_H
#define       CALODET_DECALORIMETER_H 1
/// from STL
#include <iostream>
#include <vector>
/// from CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"

/// from Det/DetDesc
#include "DetDesc/DetectorElement.h"

/// from Event/CaloGen
#include "CaloGen/CaloCellID.h"
#include "CaloGen/CaloVector.h"
#include "CaloGen/CaloException.h"

/// from Det/CaloDet
#include "CaloDet/CaloCardParams.h"
#include "CaloDet/CLIDDeCalorimeter.h"

/// forwad declarations
class MsgStream;

//-----------------------------------------------------------------------------
//
// ClassName:   DeCalorimeter 
//  
// Description: Calorimeter detector element class.
//
//-----------------------------------------------------------------------------

typedef std::vector<CaloCellID> CaloNeighbors ;

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
  bool   valid    ( const CaloCellID& ID ) const { return cells[ID].size() >0;}
  /// x-position of center of the cell 
  double cellX    ( const CaloCellID& ID ) const { return cells[ID].x (); }
  /// y-position of center of the cell 
  double cellY    ( const CaloCellID& ID ) const { return cells[ID].y (); }
  /// z-position of center of the cell 
  double cellZ    ( const CaloCellID& ID ) const { return cells[ID].z (); }
  /// cell size  
  double cellSize ( const CaloCellID& ID ) const { return cells[ID].size (); }
  /// sine function for given cell 
  double cellSine ( const CaloCellID& ID ) const { return cells[ID].sine (); }
  /// PM gain for given cell  
  double cellGain ( const CaloCellID& ID ) const { return cells[ID].gain (); }
  /// Timing for for given cell  
  double cellTime ( const CaloCellID& ID ) const { return cells[ID].time (); }
  /// cell center 
  const HepPoint3D&    cellCenter       ( const CaloCellID& ID ) const { 
    return cells[ID].center       (); }
  /// list of neighbour cells 
  const CaloNeighbors& neighborCells    ( const CaloCellID& ID ) const { 
    return cells[ID].neighbors    (); }
  /// list of neighbour cells 
  const CaloNeighbors& zsupNeighborCells( const CaloCellID& ID ) const { 
    return cells[ID].zsupNeighbors(); }
  
  /// From ID to cell serial number and vice-versa
  int    cellIndex( const CaloCellID& ID ) const { return cells.index(ID); }
  CaloCellID cellIdByIndex( const unsigned int num )    const {
    return ( (num < cells.size() ) ? 
	     (cells.begin()+num)->cellID() : CaloCellID() );
  }
  
  /// More complex functions
  CaloCellID  Cell    ( const HepPoint3D& point )             ;
  
  /// Front-end card information
  /// card number 
  int cardNumber( const CaloCellID& ID ) const {return cells[ID].cardNumber(); }
  /// card row  
  int cardRow   ( const CaloCellID& ID ) const {return cells[ID].cardRow();    }
  /// card column  
  int cardColumn( const CaloCellID& ID ) const {return cells[ID].cardColumn(); }
  /// card address 
  void cardAddress ( const CaloCellID& ID, 
		     int& card, int& row, int& column ) const {
    card   = cells[ID].cardNumber ();
    row    = cells[ID].cardRow() ;
    column = cells[ID].cardColumn() ;
  }
  /// number of cards 
  int nCards          ( )                 const { return m_cards; }
  /// down card number 
  int downCardNumber  ( const int card )  const { 
                                          return feCards[card].downNumber(); }
  /// left card number 
  int leftCardNumber  ( const int card )  const { 
                                          return feCards[card].leftNumber(); }
  /// corder card number 
  int cornerCardNumber( const int card )  const { 
                                          return feCards[card].cornerNumber();}
  /// previous card number 
  int previousCardNumber(const int card ) const{ 
                                        return feCards[card].previousNumber();}
  /// card neighbours
  void cardNeighbors ( const int card, int& down, int& left, int& corner )const{
    down   = feCards[card].downNumber();
    left   = feCards[card].leftNumber();
    corner = feCards[card].cornerNumber();
  }
  /// card area 
  int cardArea       ( const int card ) const {return feCards[card].area();   }
  /// card first row 
  int cardFirstRow   ( const int card ) const {return feCards[card].firstRow();}
  /// card first column 
  int cardFirstColumn( const int card ) const {
    return feCards[card].firstColumn(); }
  /// ID of the bottom left cell, of the top right cell, of the specified cell
  CaloCellID firstCellID ( const int card ) const {  
    return CaloCellID( m_caloIndex,
		       feCards[card].area(), 
		       feCards[card].firstRow(),
		       feCards[card].firstColumn() ); }

  CaloCellID lastCellID ( const int card ) const {
    return CaloCellID( m_caloIndex,
		       feCards[card].area(), 
		       feCards[card].firstRow()    + nRowCaloCard -1 ,
		       feCards[card].firstColumn() + nColCaloCard -1  ); }

  CaloCellID cardCellID ( const int card, const int row, const int col ) const {
    return CaloCellID( m_caloIndex,
		       feCards[card].area(), 
		       feCards[card].firstRow()    + row ,
		       feCards[card].firstColumn() + col  ); }
  
protected: 
  
  /// Initialization method for building the cells 
  StatusCode     buildCells    ();
  /// Initialization method for building the cards 
  StatusCode     buildCards    ();
  
protected:

//----------------------------------------------------------------------------
// ** Auxilliary class to store cell's information inside the detector element
// ** The constructor creates a null cellID if no argument, as a default
// ** creator is needed for CaloVector...
//----------------------------------------------------------------------------

  class CellParam {
    
  public:
    
    CellParam( ) 
      : m_cellID     (         )
      , m_Size       (     0.0*meter )
      , m_Center     (-99999.0*meter , -99999.0*meter , 0.0*meter )
      , m_Sine       (     0.0 )
      , m_Gain       (     0.0 )
      , m_CardNumber (     -1  )
      , m_Neighbors  (         )
      , m_ZsupNeighbors  (     )
      { };
      
    CellParam( CaloCellID& id ) 
      : m_cellID     (     id  )
      , m_Size       (     0.0*meter )
      , m_Center     (-99999.0*meter , -99999.0*meter , 0.0*meter )
      , m_Sine       (     0.0 )
      , m_Gain       (     0.0 )
      , m_CardNumber (     -1  )
      , m_Neighbors  (         )
      , m_ZsupNeighbors  (     )
      { m_cellID = id; };
      
    ~CellParam() {}
      
    bool                 valid         () const { return size() > 0      ; }   
    CaloCellID           cellID        () const { return m_cellID        ; }
    double               x             () const { return m_Center.x()    ; }
    double               y             () const { return m_Center.y()    ; }
    double               z             () const { return m_Center.z()    ; }
    const HepPoint3D&    center        () const { return m_Center        ; }
    double               size          () const { return m_Size          ; }
    double               sine          () const { return m_Sine          ; }
    double               gain          () const { return m_Gain          ; }
    double               time          () const { return m_Time          ; }
    int                  cardNumber    () const { return m_CardNumber    ; }
    int                  cardRow       () const { return m_CardRow       ; }
    int                  cardColumn    () const { return m_CardColumn    ; }
      
    const CaloNeighbors& neighbors     () const { return m_Neighbors     ; }
    const CaloNeighbors& zsupNeighbors () const { return m_ZsupNeighbors ; }
      
// ** To initialize the cell: Geometry, neighbours, gain

    void  setCenterSize( const HepPoint3D& point, double S) {
      m_Center = point; 
      m_Size   = S; 
      m_Sine   = sqrt( (point.x()*point.x() + point.y()*point.y()) / 
		       point.mag2() ); 
      m_Time   = point.mag() /c_light *ns;
    }

    void addZsupNeighbor( const CaloCellID& ID) { 
      m_ZsupNeighbors.push_back(ID);
    }
    void addNeighbor    ( const CaloCellID& ID) { m_Neighbors.push_back(ID); }
    void setGain        ( const double gain   ) { m_Gain = gain            ; }
    void setFeCard      ( const int num, const int relCol, const int relRow ) {
      m_CardNumber  = num;
      m_CardColumn  = relCol;
      m_CardRow     = relRow;
    }
      
    bool operator==( const CellParam& c2 ) const { 
      return center() == c2.center() && size() == c2.size(); }
      
  private:
    CaloCellID    m_cellID         ; // ID of the cell
    double        m_Size           ; // Cell size
    HepPoint3D    m_Center         ; // Cell centre
    double        m_Sine           ; // To transform E to Et
    double        m_Gain           ; // MeV per ADC count
    double        m_Time           ; // Nominal time of flight from Vertex (ns)
    int           m_CardNumber     ; // Front-end card number 
    int           m_CardRow        ; // card row and column
    int           m_CardColumn     ;
    CaloNeighbors m_Neighbors      ; // List of neighbors
    CaloNeighbors m_ZsupNeighbors  ; // List of neighbors in same area

  };   // ** End Class CellParam

//----------------------------------------------------------------------------
// **  Auxilliary class to store the Front End card information
//----------------------------------------------------------------------------

  class CardParam {

  public:
    CardParam( int iArea, int iRow, int iCol ) 
      : m_Area           ( iArea )
      , m_Row            ( iRow )
      , m_Col            ( iCol )
      , m_DownNumber     ( -1 )
      , m_LeftNumber     ( -1 )
      , m_CornerNumber   ( -1 ) 
      , m_PreviousNumber ( -1 )
      { } ;
    
    ~CardParam() {}
    
    int downNumber      () const { return m_DownNumber     ; }
    int leftNumber      () const { return m_LeftNumber     ; }
    int cornerNumber    () const { return m_CornerNumber   ; }
    int previousNumber  () const { return m_PreviousNumber ; }
    int area            () const { return m_Area           ; }
    int firstRow        () const { return m_Row            ; }
    int firstColumn     () const { return m_Col            ; }

    void setNeighboringCards  ( const int down,   const int left, 
				const int corner, const int previous ) { 
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

  }; // ** End class CardParam
//----------------------------------------------------------------------------


protected: 
  
  inline void Assert( bool , const char*         ) const ; 
  inline void Assert( bool , const std::string&  ) const ; 
  
private:
  
  bool     m_initialized;      // Flag, to compute the geometry only once
  bool     m_cardsInitialized; // Flag, to compute the card layout only once
  
  int      m_caloIndex;     // Calorimeter index, to code CellIDs
  int      maxRowCol;       // Maximum value for the Row / Column =31 or 63
  int      firstRowUp;      // First row or column over center,    16 or 64
  double   centerRowCol;    // Central value = maxRowCol/2,      15.5 or 31.5

// ** Parameters for the gain computation

  double   m_maxEtInCenter; // Et value for the maximum ADC value at teta = 0
  double   m_maxEtSlope;    // Increase in Et per radian. This is also
                            // the maximum E if m_maxEtInCenter = 0
  int      m_adcMax;        // maximum codage in the ADC, 4095 or 1023
  double   m_activeToTotal; // Convertion from activeE() to energy seen 
  double   m_zShowerMax;    // Z of the shower maximum in the local frame.

  CaloVector<CellParam> cells;          // Collection of cells

// ** Parameters for the cards

  int m_cards;
  std::vector<CardParam> feCards ;

};
///
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
/// CaloDet 
///
inline std::ostream&  operator<<( std::ostream& os , const DeCalorimeter& de )
{ return de.printOut( os ); } 
///
inline std::ostream&  operator<<( std::ostream& os , const DeCalorimeter* de )
{ return de ? (os<<*de) : (os<<" DeCalorimeter* points to NULL!"<<std::endl); }
///
inline MsgStream&     operator<<( MsgStream&    os , const DeCalorimeter& de )
{ return de.printOut( os ); } 
///
inline MsgStream&     operator<<( MsgStream&    os , const DeCalorimeter* de )
{ return de ? (os<<*de) : (os<<" DeCalorimeter* points to NULL!"<<endreq   ); }
///

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

#endif  //    CALODET_DECALORIMETER_H



















