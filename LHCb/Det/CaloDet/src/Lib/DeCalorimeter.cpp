// $Id: DeCalorimeter.cpp,v 1.20 2002-12-17 15:39:53 ocallot Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.19  2002/10/03 10:21:42  cattanem
// fix buggy usage of abs()
//
// Revision 1.18  2002/06/15 06:25:25  ocallot
// new user parameters for x/y cell size and central hole
//
// Revision 1.17  2002/04/02 14:55:16  ibelyaev
//  add 'const' qualifier to DeCalorimeter::Cell method
//
// Revision 1.16  2002/03/28 13:47:14  ibelyaev
// new version of Kernel packages, move out all XMl-stuff
//
// Revision 1.15  2001/12/09 14:16:17  ibelyaev
//  update for newer version of Gaudi
//
// Revision 1.14  2001/11/25 15:08:46  ibelyaev
//  update for newer CaloKernel Package
//
// ============================================================================
#define  CALODET_DECALORIMETER_CPP 1
// ============================================================================
// from STL
#include <cmath>
#include <algorithm>
// from CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
// from Gaudi
#include "GaudiKernel/SmartDataPtr.h"
// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ILVolume.h"
// CaloKernel
#include "CaloKernel/OSiterator.h"
#include "CaloKernel/CaloPrint.h"
//
// from Det/CaloDet
#include "CaloDet/DeCalorimeter.h"
#include "CaloDet/DeSubCalorimeter.h"

// ============================================================================
/** @file DeCalorimeter.cpp
 *
 *  Implementation of class :  DeCalorimeter
 *
 *  @author Olivier Callot Olivier.Callot@cern.ch
 *  @author Vanya Belyaev  Ivan.Belyaev@itep.ru 
 */
// ============================================================================

// ============================================================================
/** constructor 
 *  @name object name (useless)
 */
DeCalorimeter::DeCalorimeter( const std::string& name )
  :  DetectorElement     ( name       )
  ,  m_initialized       ( false      )
  ,  m_cardsInitialized  ( false      )
  ,  m_caloIndex         ( -1         )
  ,  m_maxEtInCenter     ( 10.0 * GeV )
  ,  m_maxEtSlope        ( 0.0  * GeV )
  ,  m_adcMax            ( 4095       )
  ,  m_activeToTotal     ( 6.         )
{ };
// ============================================================================

// ============================================================================
/// Destructor
DeCalorimeter::~DeCalorimeter() {};
// ============================================================================

// ============================================================================
// object identification
// ============================================================================
const CLID& DeCalorimeter::clID () const { return DeCalorimeter::classID() ; }

//----------------------------------------------------------------------------
// ** Defines the maximum and center Row and Column in the cell number
//----------------------------------------------------------------------------
void DeCalorimeter::setCoding( const unsigned int nb ) {
  
  maxRowCol     = (1<< nb ) - 1;               // 63   (31   for HCAL)
  firstRowUp    = maxRowCol/2 + 1 ;            // 32   (16   for HCAL)
  centerRowCol  = .5 * (double) maxRowCol ;    // 31.5 (15.5 for HCAL)
};

// ============================================================================
// intialization method
// ============================================================================
StatusCode DeCalorimeter::initialize()
{
  StatusCode sc = DetectorElement::initialize();
  ///
  if( sc.isFailure() ) { return sc ; }
  ///
  typedef std::vector<std::string> Parameters;
  typedef Parameters::iterator     Iterator;
  ///
  Parameters pars( userParameters() );
  ///
  { /// coding bits
    Iterator it =
      std::find( pars.begin() , pars.end () , std::string("CodingBit") );
    if( pars.end() != it ) {
      setCoding( userParameterAsInt(*it) ) ;
      pars.erase( it );
    }
  }
  { /// Et in Center
    Iterator it =
      std::find( pars.begin() , pars.end () , std::string("EtInCenter") );
    if( pars.end() != it ) {
      const double value = userParameterAsDouble( *it ) ;
      setEtInCenter( value ) ;
      pars.erase( it );
    }
  }
  { /// Et in slope
    Iterator it =
      std::find( pars.begin() , pars.end () , std::string("EtSlope") );
    if( pars.end() != it ) {
      const double value = userParameterAsDouble( *it ) ;
      setEtSlope( value ) ;
      pars.erase( it );
    }
  }
  { /// AdcMax
    Iterator it =
      std::find( pars.begin() , pars.end () , std::string("AdcMax") );
    if( pars.end() != it ) {
      setAdcMax( userParameterAsInt(*it) ) ;
      pars.erase( it );
    }
  }
  { /// Total/Active ratio
    Iterator it =
      std::find( pars.begin() , pars.end () , std::string("ActiveToTotal") );
    if( pars.end() != it ) {
      const double value = userParameterAsDouble( *it ) ;
      setActiveToTotal( value ) ;
      pars.erase( it );
    }
  }
  { /// Z shower max position
    Iterator it =
      std::find( pars.begin() , pars.end () , std::string("ZShowerMax") );
    if( pars.end() != it ) {
      const double value = userParameterAsDouble( *it ) ; 
      setZShowerMax( value ) ;
      pars.erase( it );
    }
  }
  { /// Y to X size ratio 
    Iterator it =
      std::find( pars.begin() , pars.end () , std::string("YToXSizeRatio") );
    if( pars.end() != it ) {
      m_YToXSizeRatio = userParameterAsDouble( *it ) ; 
      pars.erase( it );
    }
  }
  { /// central hole X size 
    Iterator it =
      std::find( pars.begin() , pars.end () , std::string("centralHoleX") );
    if( pars.end() != it ) {
      m_centralHoleX = userParameterAsInt( *it ) ; 
      pars.erase( it );
    }
  }
  { /// central hole Y size 
    Iterator it =
      std::find( pars.begin() , pars.end () , std::string("centralHoleY") );
    if( pars.end() != it ) {
      m_centralHoleY = userParameterAsInt( *it ) ; 
      pars.erase( it );
    }
  }
  ///
  if( !pars.empty() ) {
    // some "extra" parameters.
    // should be an error??
  }
  ///
  if( sc.isSuccess() ) { sc = buildCells(); }
  if( sc.isSuccess() ) { sc = buildCards(); }
  ///
  return sc;
};

//----------------------------------------------------------------------------
// ** Builds the cells from the geometry of the Detector Element
//----------------------------------------------------------------------------

StatusCode DeCalorimeter::buildCells( ) {
	
  // ** do not initialize, if already initialize

  if( isInitialized() ) { return StatusCode::SUCCESS; }
  int nbCells = 0;

  MsgStream log( msgSvc(), "buildCells"+ name () );

  m_caloIndex = CaloCellCode::CaloNumFromName( name() );

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
    const ILVolume* lv = geoData->lvolume();
    Assert( 0 != lv             , " Unable to extract ILVolume* !"       );

    HepPoint3D pointLocal(0,0,0), pointGlobal(0,0,0);
    pointLocal.setZ( zShowerMax() );

	
    // ** The center of each cell is specified by step of one cell
    // ** in the local frame. One has to convert to the global frame

    for( int Row = 0 ; maxRowCol >= Row    ; ++Row    ) {
      pointLocal.setY( m_YToXSizeRatio * cellSize[Area] * (Row-centerRowCol));

      for( int Column = 0; maxRowCol >= Column ; ++Column )  {
        pointLocal.setX( cellSize[ Area ] * ( Column - centerRowCol ) ) ;
 
        if( !lv->isInside( pointLocal ) ) {  continue ; }

        // Mask the non connected calorimeter cells. 
        // Should be only for central part, but is OK also for middle and
        // outer as the hole is quite small...
        
        if ( ( m_centralHoleX > fabs(Column - centerRowCol) ) &&
             ( m_centralHoleY > fabs(Row    - centerRowCol) ) ) {
          continue;
        }

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

    // ** Defines the standard 8 neighbouring cells: 
    // ** +-1 X and Y in the same area
    // ** One may also take care (in the future) of 
    // ** the central vertical separation
    // ** between the two calorimeter halves

    for( int iColumn = Column-1 ;  iColumn <= Column+1 ; ++iColumn ) {
      if ( iColumn >= 0  && iColumn <= maxRowCol ) { // inside calorimeter
 
        for( int iRow = Row - 1 ;  iRow <= Row + 1 ; ++iRow ) {
          if( ( iRow >= 0 )         &&
              ( iRow <= maxRowCol ) &&              // inside calorimeter
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

      // ** Find neighbours from different areas, 
      // ** if not 8 same-area neighbors.
      // ** For cells in different area to be neighbors, 
      // ** the distance in X and Y of
      // ** the two cells center should be less than half 
      // ** the sum of the cell sizes
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

        // ** To be fast, one checks only cells near 
        // ** the expected position (cc and rc)
        // ** We limit to +-4, i.e. the size ratio should be less than 4

        for( int  iRow = rc - 4 ; iRow <= rc + 4 ; ++iRow    ) {
          if( (0 <= iRow ) && (maxRowCol >= iRow )    ) {

            for( int  iColumn = cc - 4 ; iColumn <= cc + 4 ; ++iColumn ) {
              if( ( 0 <= iColumn ) && ( maxRowCol >= iColumn )   ) {

                CaloCellID id2( m_caloIndex, iArea , iRow , iColumn ) ;
 
                if( cells[id2].valid() ) {
                  if( ( fabs( cells[id2].x() - x ) <= margin ) &&
                      ( fabs( cells[id2].y() - y ) <= margin )    ) {
                    pCell->addNeighbor( id2 ) ;
                  }
                }
              }
            } // Loop on columns
 
          }
        } // Loop on Rows
 
      } /// end of loop over all areas
    }

    // Commented by V.B.
    // log << MSG::VERBOSE << "Cell " << id << " Neighbors ";
    // CaloNeighbors::const_iterator neighbor = neighborCells( id ).begin() ;
    // while ( neighbor != neighborCells( id ).end() ) {
    //  log << (*neighbor) ;
    //  ++neighbor;
    //  }
    // log << endreq;

  } // end of loop ovel all cells

  m_initialized = true ;

  log << MSG::DEBUG << "Initialized, index = " << m_caloIndex << ", "
      << nbCells    << " cells." << endreq;

  return StatusCode::SUCCESS;
};

//----------------------------------------------------------------------------
// ** Return the cell at the specified position
//----------------------------------------------------------------------------

CaloCellID DeCalorimeter::Cell( const HepPoint3D& globalPoint ) const
{
  
  // ** if point is outside calorimeter

  Assert( 0 != geometry() , " Unable to extract IGeometryInfo* " );
  if( !geometry()->isInside( globalPoint ) ) { return CaloCellID( ) ; }

  // ** find subcalorimeter
  
  for( IDetectorElement::IDEContainer::const_iterator child = 
         childBegin() ; childEnd() != child ; ++child ) {
    const DeSubCalorimeter* subCalorimeter = 0 ;
    
    try       { subCalorimeter = 
                  dynamic_cast<const DeSubCalorimeter*>(*child); }
    catch(...){ continue ; }
    Assert( 0 != subCalorimeter , " Unable to extract SubCalorimeter");
    
    const IGeometryInfo* subCalGeo = subCalorimeter->geometry() ;
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

  MsgStream log( msgSvc(), "buildCards"+ name () );

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

    // ** Find the first row for this area: 
    // ** A cards starts at the first row/column
    // ** For the columns, this is always starting at 0 (or 8).

    int validationCard;
    int prevValidation = -1;

    int firstRow = -1;
    for ( int iRow = 0; (maxRowCol >= iRow ) && (firstRow < 0); ++iRow ) {
      CaloCellID anID( m_caloIndex, iArea , iRow , firstRowUp );
      if( cells[anID].valid() )  { firstRow = iRow; }
    }

    if ( firstRow >=0 ) {
      for ( int iCol = 0; maxRowCol >= iCol; iCol += nColCaloCard ) {
        for ( int iRow = firstRow;
              maxRowCol-firstRow >= iRow; iRow+=nRowCaloCard ) {
 
          // ** for each possible card, check if any of the 32 channels 
          // ** of the card exist
 
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
                
                // ** One can find the Down/Left 
                // ** (in the Calorimeter frame) cards,
                // ** they are already built, as we do in ID order...
  
                if ( (0 == rr) && (0 < iRow) ) {
                  CaloCellID testID( m_caloIndex, iArea , iRow-1 , iCol+cc );
                  if ( cardNumber(testID) >= 0 ) {
                    downCard = cardNumber(testID);
                  }
                }
  
                // ** As the two halves are independend, 
                // ** no 'Left' neighbors for the central
                // ** column. Test on 'firstRowUp' which is 
                // ** also the first Column Left

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

            //== Hardcoded validation card number...
            
            if (                   6 >= m_cards ) validationCard = 0 ;
            if (  7 <= m_cards && 12 >= m_cards ) validationCard = 1 ;
            if ( 13 <= m_cards && 19 >= m_cards ) validationCard = 2 ;
            if ( 20 <= m_cards && 25 >= m_cards ) validationCard = 3 ;
            if ( 26 <= m_cards && 29 >= m_cards ) validationCard = 4 ;
            if ( 30 <= m_cards && 33 >= m_cards ) validationCard = 5 ;
            if ( 34 <= m_cards && 37 >= m_cards ) validationCard = 4 ;
            if ( 38 <= m_cards && 41 >= m_cards ) validationCard = 5 ;
            if ( 42 <= m_cards && 45 >= m_cards ) validationCard = 6 ;
            if ( 46 <= m_cards && 49 >= m_cards ) validationCard = 7 ;
            if ( 50 <= m_cards && 53 >= m_cards ) validationCard = 6 ;
            if ( 54 <= m_cards && 57 >= m_cards ) validationCard = 7 ;
            if ( 58 <= m_cards && 64 >= m_cards ) validationCard = 8 ;
            if ( 65 <= m_cards && 70 >= m_cards ) validationCard = 9 ;
            if ( 71 <= m_cards && 77 >= m_cards ) validationCard = 10 ;
            if ( 78 <= m_cards && 83 >= m_cards ) validationCard = 11 ;

            if ( 84 <= m_cards &&  89 >= m_cards ) validationCard = 12 ;
            if ( 90 <= m_cards &&  93 >= m_cards ) validationCard = 13 ;
            if ( 94 <= m_cards &&  99 >= m_cards ) validationCard = 14 ;
            if (100 <= m_cards && 103 >= m_cards ) validationCard = 15 ;
            if (104 <= m_cards && 105 >= m_cards ) validationCard = 13 ;
            if (106 <= m_cards && 107 >= m_cards ) validationCard = 15 ;
            if (108 <= m_cards && 109 >= m_cards ) validationCard = 13 ;
            if (110 <= m_cards && 111 >= m_cards ) validationCard = 15 ;
            if (112 <= m_cards && 113 >= m_cards ) validationCard = 16 ;
            if (114 <= m_cards && 115 >= m_cards ) validationCard = 17 ;
            if (116 <= m_cards && 117 >= m_cards ) validationCard = 16 ;
            if (118 <= m_cards && 119 >= m_cards ) validationCard = 17 ;
            if (120 <= m_cards && 123 >= m_cards ) validationCard = 16 ;
            if (124 <= m_cards && 129 >= m_cards ) validationCard = 18 ;
            if (130 <= m_cards && 133 >= m_cards ) validationCard = 17 ;
            if (134 <= m_cards && 139 >= m_cards ) validationCard = 19 ;
    
            if (140 <= m_cards && 142 >= m_cards ) validationCard = 20 ;
            if (143 <= m_cards && 148 >= m_cards ) validationCard = 21 ;
            if (149 <= m_cards && 154 >= m_cards ) validationCard = 22 ;
            if (155 <= m_cards && 157 >= m_cards ) validationCard = 23 ;
            if (158 <= m_cards && 160 >= m_cards ) validationCard = 20 ;
            if (161 <= m_cards && 163 >= m_cards ) validationCard = 23 ;
            if (164 <= m_cards && 166 >= m_cards ) validationCard = 24 ;
            if (167 <= m_cards && 169 >= m_cards ) validationCard = 25 ;
            if (170 <= m_cards && 175 >= m_cards ) validationCard = 26 ;
            if (176 <= m_cards && 178 >= m_cards ) validationCard = 25 ;
            if (179 <= m_cards && 181 >= m_cards ) validationCard = 24 ;
            if (182 <= m_cards && 187 >= m_cards ) validationCard = 27 ;

            if ( prevValidation == validationCard ) {
              previousCard = downCard;
            }
            else {
              prevValidation = validationCard;
            }

            feCards.push_back( CardParam( iArea, iRow, iCol ) ); // add card
            feCards[m_cards].setNeighboringCards( downCard,    leftCard,
                                                  cornerCard , previousCard);
            feCards[m_cards].setValidationNumber( validationCard );
            m_cards = m_cards + 1;
          } // ** Finished creating a new card with its cells.
        } // ** Row
      } // ** Col
    } // ** If valid area, i.e. has a first row
  } // ** Area
  m_cardsInitialized = true;

  log << MSG::DEBUG          << "Initialized, " << m_cards
      << " front-end cards." << endreq;

  return StatusCode::SUCCESS;
};

// ============================================================================
/// print to std::stream
// ============================================================================
std::ostream& DeCalorimeter::printOut( std::ostream& os ) const
{
  CaloPrint print;
  
  os << "\tDeCalorimeter index=" << print( m_caloIndex ) 
     << ", name from index ='"   
     << CaloCellCode::CaloNameFromNum( m_caloIndex ) << "'"
     << ", fullname ='"   << name ()  << "'"
     << "\tCellsInitialized=" ;
  if( m_initialized ) { os <<  "true" ; }
  else                { os << "false" ; }
  os << "\tCardsInitialized=" ;
  if( m_cardsInitialized ) { os <<  "true" ; }
  else                     { os << "false" ; }
  os << std::endl;
  
  os << "\t Parameters"
     << std::endl
     << "\t\tEt value for maximum ADC value at theta(0) =  "
     << print( m_maxEtInCenter ) 
     << std::endl
     << "\t\tIncrease in Et per radian                  =  "
     << print( m_maxEtSlope    ) 
     << std::endl
     << "\t\tMaximum codage in the ADC                  =  "
     << print( m_adcMax        )
     << std::endl
     << "\t\tConversion from activeE() to energy seen   =  "
     << print( m_activeToTotal )
     << std::endl
     << "\t\tZ of the shower maximum in the local frame =  "
     << print( m_zShowerMax    )
     << std::endl
     << "\t\tMaximum value for Row/Column               =  "
     << print(   maxRowCol     )
     << std::endl
     << "\t\tFirst Row or Column  over center           =  "
     << print( firstRowUp      )
     << std::endl
     << "\t\tCentral Value = maxRowCol/2                =  "
     << print( centerRowCol    )
     << std::endl ;

  if( m_initialized )
    {
      CaloVector<CellParam>::const_iterator pCell = cells.begin() ;
      while( cells.end() != pCell )
        {
          CaloCellID id = (pCell++)->cellID();
          os << "Cell " << id << " Neighbors ";
          std::copy( neighborCells( id ).begin() ,
                     neighborCells( id ).end()   ,
                     OS_iterator<CaloCellID,std::ostream>( os , "," ) );
          os << std::endl;
        }
    }

  return os ;
};

// ============================================================================
/// print to MsgStream
// ============================================================================
MsgStream&    DeCalorimeter::printOut( MsgStream&    os ) const
{
  CaloPrint print;
  os << "\tDeCalorimeter index="   << std::setw(2) << m_caloIndex
     << ", name from index='"
     << CaloCellCode::CaloNameFromNum( m_caloIndex ) << "'"
     << ", fullname ='"            << name ()  << "'"
     << "\tCellsInitialized=" ;
  if( m_initialized ) { os <<  "true" ; }
  else                { os << "false" ; }
  os << "\tCardsInitialized " ;
  if( m_cardsInitialized ) { os <<  "true" ; }
  else                     { os << "false" ; }
  os << endreq   ;

  os << "\t Parameters"
     << endreq
     << "\t\tEt value for maximum ADC value at theta(0) =  "
     << print( m_maxEtInCenter )
     << endreq
     << "\t\tIncrease in Et per radian                  =  "
     << print( m_maxEtSlope    )
     << endreq
     << "\t\tMaximum codage in the ADC                  =  "
     << print( m_adcMax        )
     << endreq
     << "\t\tConversion from activeE() to energy seen   =  "
     << print( m_activeToTotal )
     << endreq
     << "\t\tZ of the shower maximum in the local frame =  "
     << print( m_zShowerMax    )
     << endreq
     << "\t\tMaximum value for Row/Column               =  "
     << print(  maxRowCol      )
     << endreq
     << "\t\tFirst Row or Column  over center           =  "
     << print( firstRowUp      )
     << endreq
     << "\t\tCentral Value = maxRowCol/2                =  "
     << print( centerRowCol    )
     << endreq ;

  if( m_initialized )
    {
      const MSG::Level lev = os.currentLevel();
      os.report( lev - 1 );
      ///
      CaloVector<CellParam>::const_iterator pCell = cells.begin() ;
      while( cells.end() != pCell )
        {
          CaloCellID id = (pCell++)->cellID();
          os << " Cell " << id << " Neighbors ";
          std::copy( neighborCells( id ).begin() ,
                     neighborCells( id ).end()   ,
                     OS_iterator<CaloCellID,MsgStream>(os,",") );
          os << endreq;
        }
      ///
      os.report( lev );
    }

  return os ;
};

// ============================================================================
// The End 
// ============================================================================









