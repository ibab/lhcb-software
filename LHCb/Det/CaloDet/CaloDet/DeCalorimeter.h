#ifndef       __CALO_DETECTORELEMENT_DECALORIMETER_H__
#define       __CALO_DETECTORELEMENT_DECALORIMETER_H__ 1 

/// from STL 
///
#include <vector>

/// from Gaudi 
///
#include "DetDesc/DetectorElement.h"

/// from CLHEP 
///
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/SystemOfUnits.h"

///
#include "CaloGen/CaloCellID.h"
#include "CaloGen/CaloCollection.h" 

///
#include "CaloDet/CaloCardParams.h"
#include "CaloDet/CLIDDeCalorimeter.h"


//------------------------------------------------------------------------------
//
// ClassName:   DeCalorimeter 
//  
// Description: Calorimeter detector element class.
//
//------------------------------------------------------------------------------

typedef std::vector<CaloCellID> CaloNeighbors ;

class DeCalorimeter: public DetectorElement {

public:
  
  /// Constructors

  DeCalorimeter( const std::string& name    = ""  );
  
  /// Destructor

  virtual ~DeCalorimeter() ;
  
  /// Retrieve refrence to class identifier

  virtual const CLID& clID    () const { return classID()          ; } 
  static  const CLID& classID ()       { return CLID_DeCalorimeter ; } 
  
  /// if initialized in a proper way? 

  inline  bool  isInitialized() const      { return  m_initialized ; }  
  
  ///** Initialization methods
  
  StatusCode     buildCells    ();
  StatusCode     buildCards    ();
  unsigned int   numberOfCells () { buildCells() ; return cells.size(); }; 
  
  //** setters:

  void setCoding        ( unsigned int nb             );
  void setEtInCenter    ( double       maxEtInCenter  )  { m_maxEtInCenter = maxEtInCenter ; }
  void setEtSlope       ( double       maxEtSlope     )  { m_maxEtSlope    = maxEtSlope    ; }
  void setAdcMax        ( unsigned int adcMax         )  { m_adcMax        = adcMax        ; }
  void setActiveToTotal ( double       activeToTotal  )  { m_activeToTotal = activeToTotal ; }

  //** getters:

  double        maxEtInCenter () const { return m_maxEtInCenter ; }; 
  double        maxEtSlope    () const { return m_maxEtSlope    ; }; 
  unsigned int  adcMax        () const { return m_adcMax        ; }; 
  double        activeToTotal () const { return m_activeToTotal ; };

  bool          valid         ( const CaloCellID& ID ) const { return cells[ID].size() > 0 ; }   
  double        cellX         ( const CaloCellID& ID ) const { return cells[ID].x        (); }
  double        cellY         ( const CaloCellID& ID ) const { return cells[ID].y        (); }
  double        cellZ         ( const CaloCellID& ID ) const { return cells[ID].z        (); }
  double        cellSize      ( const CaloCellID& ID ) const { return cells[ID].size     (); }
  double        cellSine      ( const CaloCellID& ID ) const { return cells[ID].sine     (); }
  double        cellGain      ( const CaloCellID& ID ) const { return cells[ID].gain     (); }

  const HepPoint3D&    cellCenter       ( const CaloCellID& ID ) const { return cells[ID].center       (); }
  const CaloNeighbors& neighborCells    ( const CaloCellID& ID ) const { return cells[ID].neighbors    (); }
  const CaloNeighbors& zsupNeighborCells( const CaloCellID& ID ) const { return cells[ID].zsupNeighbors(); }
  
/*    unsigned int column ( const CaloCellID& ID ) { return (   ( unsigned int ) ID ) & maskCol              ; }  */
/*    unsigned int row    ( const CaloCellID& ID ) { return ( ( ( unsigned int ) ID ) & maskRow ) >> nBitCol ; } */
/*    unsigned int area   ( const CaloCellID& ID ) { return (   ( unsigned int ) ID ) >> nBitRowCol          ; }    */

  unsigned int column ( const CaloCellID& ID ) { return ID.col  () ; } 
  unsigned int row    ( const CaloCellID& ID ) { return ID.row  () ; }
  unsigned int area   ( const CaloCellID& ID ) { return ID.area () ; }   

  //*** More complex functions

  CaloCellID  Cell    ( const HepPoint3D& point )             ;
  void        printID ( MsgStream& os, const CaloCellID& ID ) ;


  //** Front-end card information

  int cardNumber      ( const CaloCellID& ID )  const { return cells[ID].cardNumber       (); }
  int cardRow         ( const CaloCellID& ID )  const { return cells[ID].cardRow          (); }
  int cardColumn      ( const CaloCellID& ID )  const { return cells[ID].cardColumn       (); }

  void cardAddress    ( const CaloCellID& ID, int& card, int& row, int& column ) const {
    card   = cells[ID].cardNumber ();
    row    = cells[ID].cardRow() ;
    column = cells[ID].cardColumn() ;
  }

  int nCards              ( )                const { return m_cards    ; }
  int downCardNumber      ( const int card ) const { return feCards[card].downNumber   (); }
  int leftCardNumber      ( const int card ) const { return feCards[card].leftNumber   (); }
  int cornerCardNumber    ( const int card ) const { return feCards[card].cornerNumber (); }
  int previousCardNumber  ( const int card ) const { return feCards[card].previousNumber (); }

  void cardNeighbors      ( const int card, int& down, int& left, int& corner ) const {
    down   = feCards[card].downNumber();
    left   = feCards[card].leftNumber();
    corner = feCards[card].cornerNumber();
  }

  int cardArea            ( const int card ) const { return feCards[card].area         (); }
  int cardFirstRow        ( const int card ) const { return feCards[card].firstRow     (); }
  int cardFirstColumn     ( const int card ) const { return feCards[card].firstColumn  (); }

  CaloCellID firstCellID  ( const int card ) const {   //*** ID of the bottom left cell
    return cellID( feCards[card].area(), 
		   feCards[card].firstRow(),
		   feCards[card].firstColumn() ); }

  CaloCellID lastCellID   ( const int card ) const {   //*** ID of the top right cell
    return cellID( feCards[card].area(), 
		   feCards[card].firstRow()    + nRowCaloCard -1 ,
		   feCards[card].firstColumn() + nColCaloCard -1  ); }

  CaloCellID cardCellID   ( const int card, const int row, const int column ) const {  
    return cellID( feCards[card].area(), 
		   feCards[card].firstRow()    + row ,
		   feCards[card].firstColumn() + column  ); } //*** ID of the specified cell


protected:

//----------------------------------------------------------------------------
//** Auxilliary class to store the cell information inside the detector element
//----------------------------------------------------------------------------

  class CellParam {
    
  public:
    
    CellParam() : m_Size           (     0.0*meter ) 
                , m_Center         (-99999.0*meter , -99999.0*meter , 0.0*meter )
                , m_Sine           (     0.0 ) 
                , m_Gain           (     0.0 ) 
                , m_CardNumber     (     -1  )
                , m_Neighbors      (         ) 
                , m_ZsupNeighbors  (         )
                  { };
      
    ~CellParam() {}
      
    bool                 valid         () const { return size() > 0      ; }   

    double               x             () const { return m_Center.x()    ; }
    double               y             () const { return m_Center.y()    ; }
    double               z             () const { return m_Center.z()    ; }
    const HepPoint3D&    center        () const { return m_Center        ; }
    double               size          () const { return m_Size          ; }
    double               sine          () const { return m_Sine          ; }
    double               gain          () const { return m_Gain          ; }
    int                  cardNumber    () const { return m_CardNumber    ; }
    int                  cardRow       () const { return m_CardRow       ; }
    int                  cardColumn    () const { return m_CardColumn    ; }
      
    const CaloNeighbors& neighbors     () const { return m_Neighbors     ; }
    const CaloNeighbors& zsupNeighbors () const { return m_ZsupNeighbors ; }
      
    ///
    /// To initialize the cell: Center and size, neighbours, gain
    ///

    void  setCenterSize( const HepPoint3D& point, double S) {
      m_Center = point; 
      m_Size   = S; 
      m_Sine   = sqrt( (point.x()*point.x() + point.y()*point.y()) / 
		       point.mag2() ); 
    }

    void addZsupNeighbor( const CaloCellID& ID ) { m_ZsupNeighbors.push_back( ID ) ; }
    void addNeighbor    ( const CaloCellID& ID ) { m_Neighbors.push_back( ID )     ; }
    void setGain        ( const double gain    ) { m_Gain = gain                   ; }
    void setFeCard      ( const int num, const int relCol, const int relRow ) {
      m_CardNumber  = num;
      m_CardColumn  = relCol;
      m_CardRow     = relRow;
    }
      
    bool operator==( const CellParam& c2 ) const { 
      return center() == c2.center() && size() == c2.size(); 
    }
      
  private:
      
    double        m_Size           ; // Cell size
    HepPoint3D    m_Center         ; // Cell centre
    double        m_Sine           ; // To transform E to Et
    double        m_Gain           ; // MeV per ADC count
    int           m_CardNumber     ; // Front-end card number and internal address
    int           m_CardRow        ;
    int           m_CardColumn     ;
    CaloNeighbors m_Neighbors      ; // List of neighbors
    CaloNeighbors m_ZsupNeighbors  ; // List of neighbors in same area

  };   //*** End Class CellParam
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//** Auxilliary class : A collection of cells
//----------------------------------------------------------------------------

  class CaloCells: public CaloCollection<CellParam> {

  public:
      
    CaloCells():CaloCollection<CellParam>(){};

    ~CaloCells (){};

    inline StatusCode addCell( const CellParam& cell , const CaloCellID& ID )	{ 
      return addEntry( cell , ID ) ; 
    }; 
  };   //*** End Class CaloCells 
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//**  Auxilliary class to store the Front End card information
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

  }; //*** End class CardParam
//----------------------------------------------------------------------------


protected: 
  
/*    CaloCellID cellID( unsigned int Area ,  */
/*  		     unsigned int Row ,  */
/*  		     unsigned int Column ) const { */
/*      return CaloCellID( ( Area << nBitRowCol ) + ( Row << nBitCol ) + Column ) ;  */
/*    } */

  CaloCellID cellID( unsigned int Area , 
		     unsigned int Row , 
		     unsigned int Column ) const {
    CaloCellID cell; 
    return cell.setArea( Area ).setRow( Row ).setCol( Column );
  }
  
  inline void Assert( bool , const char*         ) const ; 
  inline void Assert( bool , const std::string&  ) const ; 
  
private:
  
  bool     m_initialized;      // Flag, to compute the geometry only once
  bool     m_cardsInitialized; // Flag, to compute the card layout only once
  
  int      nBitCol;         // Number of bits to code the Row/Column, from Xml file
  int      maskCol;         // Mask to select the Column = 2**nBitCol - 1
  int      maskRow;         // Mask to select the Row    = same, shifted
  int      nBitRowCol;      // Number of bits for Row + Col = 2* nBitCol
  int      maxRowCol;       // Maximum value for the Row / Column
  double   centerRowCol;    // Central value = 2**(nBitCol-1) - 1

  //** Parameters for the gain computation

  double   m_maxEtInCenter; // Et value for the maximum ADC value at teta = 0
  double   m_maxEtSlope;    // Increase in Et per radian. Is also the maximum E if m_maxEtInCenter = 0
  int      m_adcMax;        // maximum codage in the ADC, 4095 or 1023
  double   m_activeToTotal; // Convertion from activeE() to total energy seen in the ADC

  CaloCells cells;          // Collection of cells

  //** Parameters for the cards

  int m_cards;
  std::vector<CardParam> feCards ;

};

//----------------------------------------------------------------------------
// Explicit inline methods, those needing more than a single statement...
//----------------------------------------------------------------------------

inline void DeCalorimeter::Assert( bool assertion , const char*  message ) const {
  if( !assertion ) {
    std::string msg("DeCalorimeter:: ");  
    throw CaloException( msg += message  );  
  }
}; 
  
inline void DeCalorimeter::Assert( bool assertion , const std::string&  message ) const {
  if( !assertion ) {
    std::string msg("DeCalorimeter:: ");  
    throw CaloException( msg += message  );  
  }
}; 

#endif  //    __CALO_DETECTORELEMENT_DECALORIMETER_H__




