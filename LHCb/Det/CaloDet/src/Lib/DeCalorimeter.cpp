#define __CALODET_DECALORIMETER_CPP__ 1 

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
/// from Det/CaloDet

#include "CaloDet/DeCalorimeter.h"
#include "CaloDet/DeSubCalorimeter.h"

//----------------------------------------------------------------------------
//
// Implementation of class :  DeCalorimeter
//
//----------------------------------------------------------------------------


// **  Standard Constructors

DeCalorimeter::DeCalorimeter( const std::string& name )
  :  DetectorElement     ( name       )
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
// ** Defines the maximum and center Row and Column in the cell number
//----------------------------------------------------------------------------

void DeCalorimeter::setCoding( unsigned int nb ) {

  maxRowCol     = (1<< nb ) - 1;               // 63   (31   for HCAL)
  firstRowUp    = maxRowCol/2 + 1 ;            // 32   (16   for HCAL)
  centerRowCol  = .5 * (double) maxRowCol ;    // 31.5 (15.5 for HCAL)
}

//----------------------------------------------------------------------------
// ** Builds the cells from the geometry of the Detector Element
//----------------------------------------------------------------------------

StatusCode DeCalorimeter::buildCells( ) {

// ** do not initialize, if already initialized 

  if( isInitialized() ) { return StatusCode::SUCCESS; } 
  int nbCells = 0;

  MsgStream log( msgSvc(), "buildCells"+localPath() );

  m_caloIndex = CaloCellCode::CaloNumFromName( localPath() );
  
  std::vector<double> cellSize; 

// ** loop over all subparts of calorimeter

  for( IDetectorElement::IDEContainer::iterator child = childBegin() ; 
       childEnd() != child ; ++child ) {

// ** get subpart of type DeSubCalorimeter

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
    pointLocal.setZ( zShowerMax() );

// ** The center of each cell is specified by step of one cell
// ** in the local frame. One has to convert to the global frame

    for( int Row = 0 ; maxRowCol >= Row    ; ++Row    ) {  
      pointLocal.setY( cellSize[ Area ] * ( Row - centerRowCol ) ) ;    
      for( int Column = 0; maxRowCol >= Column ; ++Column )  { 
	pointLocal.setX( cellSize[ Area ] * ( Column - centerRowCol ) ) ;    
	
	if( !lv->isInside( pointLocal ) ) {  continue ; }

	CaloCellID id( m_caloIndex, Area , Row , Column ) ;
	cells.addEntry( CellParam(id) , id );  // store the new cell

	pointGlobal = geoData->toGlobal( pointLocal );
	cells[id].setCenterSize( pointGlobal , cellSize[ Area ] ) ; 

	double gain = ( maxEtInCenter() / cells[id].sine() ) + maxEtSlope();
	gain        = gain / (double) adcMax() ;
	cells[id].setGain( gain ) ; 
	++nbCells;

      } /// end of loop over rows 
    } /// end of loop over columns
  } /// end of loop over sub calorimeters 
  
// ** Compute neighboring cells 

  for( CaloVector<CellParam>::iterator pCell = cells.begin() ; 
       cells.end() != pCell ; ++pCell ) {
      
    CaloCellID id       = pCell->cellID();
    int Column          = id.col  ( ) ; 
    int Row             = id.row  ( ) ;
    unsigned int Area   = id.area ( ) ;
    int nNeigh          = 0;
      
// ** Defines the standard 8 neighbouring cells: +-1 X and Y in the same area
// ** One may also take care (in the future) of the central vertical separation
// ** between the two calorimeter halves

    for( int iColumn = Column-1 ;  iColumn <= Column+1 ; ++iColumn )	{
      if ( iColumn >= 0  && iColumn <= maxRowCol ) {      // inside calorimeter
	  
	for( int iRow = Row - 1 ;  iRow <= Row + 1 ; ++iRow ) {
	  if( ( iRow >= 0 )         &&
	      ( iRow <= maxRowCol ) &&                    // inside calorimeter
	      ( iRow != Row || iColumn != Column  ) ) {   // not itself...
	      
	    CaloCellID id2( m_caloIndex, Area , iRow , iColumn ); 
	    if( cells[id2].valid() ) {
	      pCell->addZsupNeighbor( id2 ) ; 
	      pCell->addNeighbor( id2 ) ; 
	      nNeigh++;
	    } 
	  } 
	} // Loop on rows
      } 
    } // Loop on columns

    if ( 8 != nNeigh ) {
      
// ** Find neighbours from different areas, if not 8 same-area neighbors.
// ** For cells in different area to be neighbors, the distance in X and Y of
// ** the two cells center should be less than half the sum of the cell sizes
// ** To avoid rounding problems, one uses .55 instead of 1/2
 
      double x    = cells[id].x    (); 
      double y    = cells[id].y    (); 
      double size = cells[id].size (); 
    
      for( std::vector<double>::iterator pArea = cellSize.begin() ; 
	   cellSize.end() != pArea ; ++pArea ) {
	unsigned int iArea = pArea - cellSize.begin() ; 
	
	if( (iArea == Area)   || 
	    (iArea == Area+2) || 
	    (iArea == Area-2)    ) continue;   // Only in nearby area.

	double sizeArea = *pArea;               
	double margin = 0.55 * ( sizeArea + size ) ; 

	int cc = (int) ( x / sizeArea  + centerRowCol ); 
	int rc = (int) ( y / sizeArea  + centerRowCol ); 

// ** To be fast, one checks only cells near the expected position (cc and rc)
// ** We limit to +-4, i.e. the size ratio should be less than 4

	for( int  iRow = rc - 4 ; iRow <= rc + 4 ; ++iRow    ) {
	  if( (0 <= iRow ) && (maxRowCol >= iRow )    ) { 

	    for( int  iColumn = cc - 4 ; iColumn <= cc + 4 ; ++iColumn ) {
	      if( ( 0 <= iColumn ) && ( maxRowCol >= iColumn )   ) { 

		CaloCellID id2( m_caloIndex, iArea , iRow , iColumn ) ;
	      
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

    log << MSG::VERBOSE << "Cell " << id << " Neighbors ";
    CaloNeighbors::const_iterator neighbor = neighborCells( id ).begin() ; 
    while ( neighbor != neighborCells( id ).end() ) {   
      log << (*neighbor) ;
      ++neighbor;
    }
    log << endreq;

  } // end of loop ovel all cells 

  m_initialized = true ; 

  log << MSG::INFO << "Initialized, index = " << m_caloIndex << ", " 
      << nbCells << " cells." << endreq;

  return StatusCode::SUCCESS; 
};
  
//----------------------------------------------------------------------------
// ** Return the cell at the specified position
//----------------------------------------------------------------------------

CaloCellID DeCalorimeter::Cell( const HepPoint3D& globalPoint ) {
  
// ** if point is outside calorimeter 

  Assert( 0 != geometry() , " Unable to extract IGeometryInfo* " ); 
  if( !geometry()->isInside( globalPoint ) ) { return CaloCellID( ) ; } 
  
// ** find subcalorimeter   

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

      int Column = (int) ( localPoint.x() / CellSize + firstRowUp ) ;
      int Row    = (int) ( localPoint.y() / CellSize + firstRowUp ) ;
      
      return CaloCellID( m_caloIndex, Area , Row , Column ) ; 
    }
  } 
  return CaloCellID( ) ;
}

//----------------------------------------------------------------------------
// ** Construct the Front End card information for each cell
//----------------------------------------------------------------------------

StatusCode DeCalorimeter::buildCards( )  {

// ** do not initialize, if already initialized 

  if( m_cardsInitialized ) { return StatusCode::SUCCESS; } 

  MsgStream log( msgSvc(), "buildCards"+localPath() );
  
  m_cards   = 0;

// ** loop over all subparts of calorimeter

  for( IDetectorElement::IDEContainer::iterator child = childBegin() ; 
       childEnd() != child ; ++child ) {

// ** get subpart of type DeSubCalorimeter

    DeSubCalorimeter* subCalorimeter = 0 ; 
    try       { subCalorimeter = dynamic_cast<DeSubCalorimeter*>(*child); } 
    catch(...){ continue ; } 
    Assert( 0 != subCalorimeter , 
	    " Unable to extract DeSubCalorimeter* object! ");
    
    unsigned int iArea     = child - childBegin()           ; 

// ** Find the first row for this area: A cards starts at the first row/column
// ** For the columns, this is always starting at 0 (or 8).

    int firstRow = -1;
    for ( int iRow = 0; (maxRowCol >= iRow ) && (firstRow < 0); ++iRow ) {
      CaloCellID anID( m_caloIndex, iArea , iRow , firstRowUp ); 
      if( cells[anID].valid() )  { firstRow = iRow; }
    }

    if ( firstRow >=0 ) {
      for ( int iCol = 0; maxRowCol >= iCol; iCol += nColCaloCard ) {
	for ( int iRow = firstRow; 
	      maxRowCol-firstRow >= iRow; iRow+=nRowCaloCard ) {
	  
// ** for each possible card, check if any of the 32 channels of the card exist
	  
	  int nchan        =  0;
	  int downCard     = -1;
	  int leftCard     = -1;
	  int cornerCard   = -1;
	  int previousCard = -1;
	  for ( int cc = 0; nColCaloCard > cc ; ++cc ) {
	    for ( int rr = 0; nRowCaloCard > rr ; ++rr ) {
	      CaloCellID anID( m_caloIndex, iArea , iRow+rr , iCol+cc ); 
	      if ( cells[anID].valid() ) {
		nchan = nchan+1;
		cells[anID].setFeCard( m_cards, cc, rr );
		
// ** One can find the Down/Left (in the Calorimeter frame) cards, 
// ** they are already built, as we do in ID order...
		
		if ( (0 == rr) && (0 < iRow) ) {
		  CaloCellID testID( m_caloIndex, iArea , iRow-1 , iCol+cc ); 
		  if ( cardNumber(testID) >= 0 ) { 
		    downCard = cardNumber(testID); 

// ** Define the 'previous' card (ECAL only), the previous (down) card in the
// ** same validation card.
// ** This is the down card, except if the card is just over the horizontal 
// ** line

		    if ( (iRow < firstRowUp) || 
			 (iRow > firstRowUp + nRowCaloCard) ) {
		      previousCard = downCard;
		    }
		  } 
		}
		
// ** As the two halves are independend, no 'Left' neighbors for the central 
// ** column. Test on 'firstRowUp' which is also the first Column Left

		if ( (0 == cc) && (0 < iCol) && (firstRowUp != iCol) ) {
		  CaloCellID testID( m_caloIndex, iArea , iRow+rr , iCol-1 ); 
		  if ( cardNumber(testID) >= 0 ) { 
		    leftCard = cardNumber(testID); 
		  } 
		  if ( (0 == rr) && (0 < iRow) ) {
		    CaloCellID testID( m_caloIndex, iArea , iRow-1 , iCol-1 ); 
		    if ( cardNumber(testID) >= 0 ) { 
		      cornerCard = cardNumber(testID); 
		    } 
		  }
		}
	      }  // ** valid ID
	    }  // ** Row in card
	  }  // ** Column in card
	
	  if ( nchan > 0 ) { 
	    feCards.push_back( CardParam( iArea, iRow, iCol ) ); // add card
	    feCards[m_cards].setNeighboringCards( downCard,    leftCard, 
						  cornerCard , previousCard);
	    m_cards = m_cards + 1;  
	  } // ** Finished creating a new card with its cells.
	} // ** Row
      } // ** Col
    } // ** If valid area, i.e. has a first row
  } // ** Area
  m_cardsInitialized = true;

  log << MSG::INFO << "Initialized, " << m_cards 
      << " front-end cards." << endreq;

  return StatusCode::SUCCESS; 
};
