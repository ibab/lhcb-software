#define __CALO_DETECTORELEMENT_DECALORIMETER_CPP__ 1 

///
/// form STL
#include <cmath> 

///
/// from CLHEP 
#include "CLHEP/Units/SystemOfUnits.h"


///
/// from Gaudi 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IGeometryInfo.h"
#include "GaudiKernel/ILVolume.h"
#include "GaudiKernel/TransientStore.h"

///
/// from Calo 

#include "CaloDet/DeCalorimeter.h"
#include "CaloDet/DeSubCalorimeter.h"

//----------------------------------------------------------------------------
//
// Implementation of class :  DeCalorimeter
//
//----------------------------------------------------------------------------


//   **  Standard Constructors

DeCalorimeter::DeCalorimeter( const std::string& name ) 
  :  DetectorElement     ( name )
  ,  m_initialized       ( false      )
  ,  m_cardsInitialized  ( false      )
  ,  m_maxEtInCenter     ( 10.0 * GeV )
  ,  m_maxEtSlope        ( 0.0  * GeV )
  ,  m_adcMax            ( 4095       )           
  ,  m_activeToTotal     ( 6.         )
{ };

//
// Standard Destructor
DeCalorimeter::~DeCalorimeter() 
{ };
  
//----------------------------------------------------------------------------
//   *** Defines the parameters for coding the cell ID. 
//   *** Row and col have the same number of bits, the area is in the top part.
//----------------------------------------------------------------------------

void DeCalorimeter::setCoding( unsigned int nb ) {
  nBitCol      = nb;
  maskCol      = (1<< nBitCol) - 1;
  maskRow      = maskCol << nBitCol;
  nBitRowCol   = 2*nBitCol;
  maxRowCol    = 1<< nBitCol ;
  centerRowCol = maskCol;
  centerRowCol = .5 * centerRowCol ;
}

//----------------------------------------------------------------------------
//   *** Builds the cells from the geometry of the Detector Element
//----------------------------------------------------------------------------

StatusCode DeCalorimeter::buildCells( ) {

//   *** do not initialize, if already initialized 

  if( isInitialized() ) { return StatusCode::SUCCESS; } 
  int nbCells = 0;

  MsgStream log( msgSvc(), "buildCells"+localPath() );
  
  std::vector<double> cellSize; 

//   *** loop over all subparts of calorimeter

  for( IDetectorElement::IDEContainer::iterator child = childBegin() ; 
       childEnd() != child ; ++child ) {

//   *** get subpart of type DeSubCalorimeter

    DeSubCalorimeter* subCalorimeter = 0 ; 
    try       { subCalorimeter = dynamic_cast<DeSubCalorimeter*>(*child); } 
    catch(...){ continue ; } 
    Assert( 0 != subCalorimeter , 
            " Unable to extract DeSubCalorimeter* object! ");

    unsigned int Area     = child - childBegin()           ; 
    while( cellSize.size() <= Area ) { cellSize.push_back(0) ; } ;
    cellSize[ Area ] = subCalorimeter->size() ;

    IGeometryInfo* geoData = subCalorimeter->geometry() ; 
    Assert( 0 != geoData        , " Unable to extract IGeometryInfo* !"  );
    ILVolume* lv = geoData->lvolume();   
    Assert( 0 != lv             , " Unable to extract ILVolume* !"       );
      
    HepPoint3D pointLocal(0,0,0), pointGlobal(0,0,0);


//   *** The center of each cell is specified by step of one cell
//   *** in the local frame. One has to convert to the global frame
//   *** Note: Loop in decrement to extend less frequently the 'cells' buffer

    for(     int Row    = maxRowCol - 1  ;  0 <= Row    ; --Row    ) {  
      pointLocal.setY( cellSize[ Area ] * ( Row - centerRowCol ) ) ;    
      for( int Column = maxRowCol - 1  ;  0 <= Column ; --Column )  { 
	pointLocal.setX( cellSize[ Area ] * ( Column - centerRowCol ) ) ;    
	
	if( !lv->isInside( pointLocal ) ) {  continue ; }

	pointGlobal = geoData->toGlobal( pointLocal );

	CaloCellID id = cellID( Area , Row , Column ) ;
	
	if( cells.size() <= id.index() ) { 
	  cells.addCell( CellParam() , id );   /// extend cells if needed.
	}

	cells[id].setCenterSize( pointGlobal , cellSize[ Area ] ) ; 
	double gain = ( maxEtInCenter() / cells[id].sine() ) + maxEtSlope();
	gain        = gain / (double) adcMax() ;
	cells[id].setGain( gain ) ; 
	++nbCells;

      } /// end of loop over rows 
    } /// end of loop over columns
  } /// end of loop over sub calorimeters 
  
//   *** Compute neighboring cells 

  for( CaloCells::iterator pCell = cells.begin() ; 
       cells.end() != pCell ; ++pCell ) {
    if( pCell->valid() ) { 
      
      //      CaloCellID id( pCell - cells.begin() ) ; 
      CaloCellID id;
      id.setIndex(  pCell - cells.begin() );
      
      int Column          = column ( id ) ; 
      int Row             = row    ( id ) ;
      unsigned int Area   = area   ( id ) ;
      
//   *** Defines the standard 8 neighbouring cells: +-1 X and Y in the same area
//   *** One may also take care (in the future) of the central vertical separation
//   *** between the two calorimeter halves

      for( int iColumn = Column-1 ;  iColumn <= Column+1 ; ++iColumn )	{
	if ( iColumn >= 0  && iColumn <= maskCol ) {      // inside calorimeter
	  
	  for( int iRow = Row - 1 ;  iRow <= Row + 1 ; ++iRow ) {
	    if( ( iRow >= 0 )       &&
		( iRow <= maskCol ) &&                    // inside calorimeter
		( iRow != Row || iColumn != Column  ) ) { // not itself...
	      
	      CaloCellID id2 = cellID( Area , iRow , iColumn ); 
	      if( cells[id2].valid() ) {
		pCell->addZsupNeighbor( id2 ) ; 
		pCell->addNeighbor( id2 ) ; 
	      } 
	    } 
	  } // Loop on rows
	} 
      } // Loop on columns
      
//   *** Find neighbours from different areas.
//   *** For cells in different area to be neighbors, the distance in X and Y of
//   *** the two cells center should be less than half the sum of the cell sizes
//   *** To avoid rounding problems, one uses .55 instead of 1/2
 
      double x    = cells[id].x    (); 
      double y    = cells[id].y    (); 
      double size = cells[id].size (); 

      for( vector<double>::iterator pArea = cellSize.begin() ; 
           cellSize.end() != pArea ; ++pArea ) {
	unsigned int iArea = pArea - cellSize.begin() ; 

	if( iArea == Area ) continue; 

	double sizeArea = *pArea;               
	double margin = 0.55 * ( sizeArea + size ) ; 

	int cc = (int) ( x / sizeArea  + centerRowCol ); 
	int rc = (int) ( y / sizeArea  + centerRowCol ); 

//   *** To be fast, one checks only cells near the expected position (cc and rc)
//   *** We limit to +-4, i.e. the size ratio should be less than 4

	for( int  iRow = rc - 4 ; iRow <= rc + 4 ; ++iRow    ) {

	  if( (iRow >= 0)       && 
	      (iRow <= maskCol)    ) { 

	    for( int  iColumn = cc - 4 ; iColumn <= cc + 4 ; ++iColumn ) {

	      if( ( iColumn >= 0 )      && 
		  ( iColumn <= maskCol)   ) { 

		CaloCellID id2 = cellID( iArea , iRow , iColumn ) ;
		if( cells[id2].valid() ) { 
                  if( ( abs( cells[id2].x() - x ) <= margin ) && 
		      ( abs( cells[id2].y() - y ) <= margin )    ) { 
		    pCell->addNeighbor( id2 ) ; 
		  }
		} 
	      } 
	    } // Loop on columns

	  } 
	} // Loop on Rows

      } /// end of loop over all areas 
    }
  } // end of loop ovel all cells 

  m_initialized = true ; 

  log << MSG::INFO << "Initialized, " << nbCells << " cells." << endreq;

  return StatusCode::SUCCESS; 
};
  
//----------------------------------------------------------------------------
//   ** Return the cell at the specified position
//----------------------------------------------------------------------------

CaloCellID DeCalorimeter::Cell( const HepPoint3D& globalPoint ) {
  
//   ** if point is outside calorimeter 

  Assert( 0 != geometry() , " Unable to extract IGeometryInfo* " ); 
  if( !geometry()->isInside( globalPoint ) ) { return CaloCellID() ; } 
  
//   ** find subcalorimeter   

  for( IDetectorElement::IDEContainer::iterator child = childBegin() ; 
       childEnd() != child ; ++child ) {
    DeSubCalorimeter* subCalorimeter = 0 ; 

    try       { subCalorimeter = dynamic_cast<DeSubCalorimeter*>(*child); } 
    catch(...){ continue ; } 
    Assert( 0 != subCalorimeter , " Unable to extract SubCalorimeter");

    IGeometryInfo* subCalGeo = subCalorimeter->geometry() ; 
    Assert( 0 != subCalGeo , " Unable to extract Geometry Info ");

    if( subCalGeo->isInside( globalPoint ) ) { 

      unsigned int Area     = child - childBegin()   ;
      double       CellSize = subCalorimeter->size() ;  

      HepPoint3D localPoint( subCalGeo->toLocal( globalPoint ) ); 

      int Column = (int) ( localPoint.x() / CellSize + 0.5 + centerRowCol ) ;
      int Row    = (int) ( localPoint.y() / CellSize + 0.5 + centerRowCol ) ;
      
      return cellID( Area , Row , Column ) ; 
    }
  } 
  return CaloCellID() ;
}

//----------------------------------------------------------------------------
//   ** Construct the Front End card information for each cell
//----------------------------------------------------------------------------

StatusCode DeCalorimeter::buildCards( )  {

//   *** do not initialize, if already initialized 

  if( m_cardsInitialized ) { return StatusCode::SUCCESS; } 

  MsgStream log( msgSvc(), "buildCards"+localPath() );
  
  m_cards   = 0;

//   *** loop over all subparts of calorimeter

  for( IDetectorElement::IDEContainer::iterator child = childBegin() ; 
       childEnd() != child ; ++child ) {

//   *** get subpart of type DeSubCalorimeter

    DeSubCalorimeter* subCalorimeter = 0 ; 
    try       { subCalorimeter = dynamic_cast<DeSubCalorimeter*>(*child); } 
    catch(...){ continue ; } 
    Assert( 0 != subCalorimeter , " Unable to extract DeSubCalorimeter* object! ");
    
    unsigned int iArea     = child - childBegin()           ; 

//   ** Find the first row for this area: A cards starts at the first row/column
//   ** For the columns, this is always starting at 0 (or 8).

    int firstRow = -1;
    for ( int iRow = 0; (maxRowCol > iRow ) && (firstRow < 0); ++iRow ) {
      CaloCellID anID = cellID( iArea , iRow , maxRowCol/2 ); 
      if( cells[anID].valid() )  { firstRow = iRow; }
    }

    if ( firstRow >=0 ) {
      for ( int iCol = 0; maxRowCol > iCol; iCol += nColCaloCard ) {
	for ( int iRow = firstRow; maxRowCol-firstRow > iRow; iRow+=nRowCaloCard ) {
	  
//   ** for each possible card, check if any of the 32 channels of the card exist.
	  
	  int nchan        =  0;
	  int downCard     = -1;
	  int leftCard     = -1;
	  int cornerCard   = -1;
	  int previousCard = -1;
	  for ( int cc = 0; nColCaloCard > cc ; ++cc ) {
	    for ( int rr = 0; nRowCaloCard > rr ; ++rr ) {
	      CaloCellID anID = cellID( iArea , iRow+rr , iCol+cc ); 
	      if ( cells[anID].valid() ) {
		nchan = nchan+1;
		cells[anID].setFeCard( m_cards, cc, rr );
		
//   ** One can find the Down/Left (in the Calorimeter frame) cards, 
//   **      they are already built, as we do in ID order...
		
		if ( (0 == rr) && (0 < iRow) ) {
		  CaloCellID testID = cellID( iArea , iRow-1 , iCol+cc ); 
		  if ( cardNumber(testID) >= 0 ) { 
		    downCard = cardNumber(testID); 

//   *** Define the 'previous' card (ECAL only), the down card in the same validation card.
//   *** This is the down card, except if the card is just over the horizontal line

		    if ( (iRow < centerRowCol) || (iRow > centerRowCol+nRowCaloCard) ) {
		      previousCard = downCard;
		    }
		  } 
		}
		
//   ** As the two halves are independend, no 'Left' neighbors for the central column.

		if ( (0 == cc) && (0 < iCol) && (maxRowCol/2 != iCol) ) {
		  CaloCellID testID = cellID( iArea , iRow+rr , iCol-1 ); 
		  if ( cardNumber(testID) >= 0 ) { leftCard = cardNumber(testID); } 
		  if ( (0 == rr) && (0 < iRow) ) {
		    CaloCellID testID = cellID( iArea , iRow-1 , iCol-1 ); 
		    if ( cardNumber(testID) >= 0 ) { cornerCard = cardNumber(testID); } 
		  }
		}
	      }  //   ** valid ID
	    }  //   ** Row in card
	  }  //   ** Column in card
	
	  if ( nchan > 0 ) { 
	    feCards.push_back( CardParam( iArea, iRow, iCol ) );        /// extend card buffer.
	    feCards[m_cards].setNeighboringCards( downCard,    leftCard, 
						  cornerCard , previousCard);
	    m_cards = m_cards + 1;  
	  }        //   ** Finished creating a new card with its cells.
	} //   ** Row
      } //   ** Col
    } //   ** If valid area, i.e. has a first row
  } //   ** Area
  m_cardsInitialized = true;

  log << MSG::INFO << "Initialized, " << m_cards << " front-end cards." << endreq;

  return StatusCode::SUCCESS; 
};

//----------------------------------------------------------------------------
//   *** Print the cell ID coordinates in a readable format
//----------------------------------------------------------------------------

void DeCalorimeter::printID( MsgStream& os, const CaloCellID& ID ) {
  
  os << ID ;
  
//    int pArea = area(ID);
//    int pRow  = row(ID);
//    int pCol  = column(ID);
//    os.width(5); os << (int)ID << "=[";
//    os.width(1); os << pArea << "," ;
//    os.width(2); os << pRow << "," ;
//    os.width(2); os << pCol << "] ";
}







