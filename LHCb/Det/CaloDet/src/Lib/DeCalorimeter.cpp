// $Id: DeCalorimeter.cpp,v 1.32 2006-06-27 16:58:42 odescham Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.31  2006/06/15 14:39:46  ibelyaev
//  remove own Assert and add subCalos
//
// Revision 1.30  2006/06/15 09:27:28  ibelyaev
//  add DeCalorimeter::Cell_ function
// 
// ============================================================================
#define  CALODET_DECALORIMETER_CPP 1
// ============================================================================
// from STL
// ============================================================================
#include <cmath>
#include <algorithm>
// ============================================================================
// from Kernel/LHCbDefintions
// ============================================================================
#include "Kernel/SystemOfUnits.h"
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/SmartDataPtr.h"
// ============================================================================
// DetDesc
// ============================================================================
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ILVolume.h"
// ============================================================================
// CaloKernel
// ============================================================================
#include "Kernel/OSiterator.h"
// ============================================================================
// from Det/CaloDet
// ============================================================================
#include "CaloDet/DeCalorimeter.h"
#include "CaloDet/DeSubCalorimeter.h"
// ============================================================================

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
  ,  m_pedShift          ( 0.6        )
  ,  m_activeToTotal     ( 6.         )
  //
  ,  m_subCalos          () 
  ,  m_subCalos_         ()
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

  m_maxRowCol     = (1<< nb ) - 1;               // 63   (31   for HCAL)
  m_firstRowUp    = m_maxRowCol/2 + 1 ;            // 32   (16   for HCAL)
  m_centerRowCol  = .5 * (double) m_maxRowCol ;    // 31.5 (15.5 for HCAL)
};

// ============================================================================
// intialization method
// ============================================================================
StatusCode DeCalorimeter::initialize() 
{
  /// initialze the base 
  StatusCode sc = DetectorElement::initialize();
  if ( sc.isFailure() ) { return sc ; }
  
  {
    // collect the sub-calorimeters
    typedef IDetectorElement::IDEContainer::iterator _IT ;
    _IT begin = childBegin () ;
    _IT end   = childEnd   () ;
    for ( _IT ichild = begin ; end != ichild ; ++ichild ) 
    {
      IDetectorElement* child = *ichild ;
      if ( 0 == child ) { continue ; }
      DeSubCalorimeter* sub = dynamic_cast<DeSubCalorimeter*> ( child ) ;
      Assert ( 0 != sub , "no DeSubCalorimeter!" ) ;
      m_subCalos  .push_back ( sub ) ;
      m_subCalos_ .push_back ( sub ) ;
    }
    Assert ( !m_subCalos  .empty() , "Empty subcalorimeters!" ) ;
    Assert ( !m_subCalos_ .empty() , "Empty subcalorimeters!" ) ;
  }
  

  //== A few geometrical parameters from UserParameters in structure.xml
  typedef std::vector<std::string> Parameters;
  typedef Parameters::iterator     Iterator;
  Iterator it;
  Parameters pars( paramNames() );

  /// Y to X size ratio
  it = std::find( pars.begin() , pars.end () , std::string("YToXSizeRatio") );
  if( pars.end() != it ) {
    m_YToXSizeRatio = param<double>( *it ) ;
    pars.erase( it );
  }
  
  //ZOffset
  it = std::find( pars.begin() , pars.end () , std::string("ZOffset") );
  if( pars.end() != it ) {
    m_zOffset = param<double>( *it ) ;
    pars.erase( it );
  }

 //Zsize
  it = std::find( pars.begin() , pars.end () , std::string("ZSize") );
  if( pars.end() != it ) {
    m_zSize = param<double>( *it ) ;
    pars.erase( it );
  }
  
  //== Get other information from the condition database
  Condition* gain = condition( "Gain" );
  if ( 0 == gain ) {
    int index =  CaloCellCode::CaloNumFromName( name() );
    std::string myName = CaloCellCode::CaloNameFromNum( index ) + "Det";
    MsgStream msg( msgSvc(), myName );
    msg << MSG::ERROR << "'Gain' condition not found" << endreq;
    return StatusCode::FAILURE;
  }

  setEtInCenter   ( gain->paramAsDouble( "EtInCenter"    ) ) ;
  setEtSlope      ( gain->paramAsDouble( "EtSlope"       ) ) ;
  setActiveToTotal( gain->paramAsDouble( "ActiveToTotal" ) ) ;
  setPedestalShift( gain->paramAsDouble( "PedShift"      ) ) ;


  Condition* reco = condition( "Reco" );
  if ( 0 == reco ) {
    int index =  CaloCellCode::CaloNumFromName( name() );
    std::string myName = CaloCellCode::CaloNameFromNum( index ) + "Det";
    MsgStream msg( msgSvc(), myName );
    msg << MSG::ERROR << "'Reco' condition not found" << endreq;
    return StatusCode::FAILURE;
  }
  setZShowerMax(reco->paramAsDouble("ZShowerMax" ) );


  Condition* hardware = condition( "Hardware" );
  if ( 0 == hardware ) {
    int index =  CaloCellCode::CaloNumFromName( name() );
    std::string myName = CaloCellCode::CaloNameFromNum( index ) + "Det";
    MsgStream msg( msgSvc(), myName );
    msg << MSG::ERROR << "'Hardware' condition not found" << endreq;
    return StatusCode::FAILURE;
  }

  setAdcMax ( hardware->paramAsInt   ( "AdcMax"        ) ) ;
  setCoding ( hardware->paramAsInt   ( "CodingBit"     ) );
  m_centralHoleX = hardware->paramAsInt("centralHoleX");
  m_centralHoleY = hardware->paramAsInt("centralHoleY");
  
  if ( sc.isSuccess() ) { sc = buildCells (); }

  if ( sc.isSuccess() ) { sc = buildCards (); }
  
  Assert ( 0 != geometry() , "Invalid IGeometryInfo!" ) ;
  
  return sc;
};
// ============================================================================
/// Return a reference (tilted) plane
// ============================================================================
Gaudi::Plane3D DeCalorimeter::plane( const double dz) const 
{
  const IGeometryInfo* geometry = this->geometry() ;
  Gaudi::XYZPoint local(0. , 0. , dz);
  if ( fabs(dz-m_zOffset) > m_zSize/2. ) {  
    MsgStream msg( msgSvc(), "DeCalorimeter Plane "+ name () );
    msg << MSG::WARNING 
        << " THE REQUESTED PLANE IS OUTSIDE THE ACTIVE VOLUME of : " << name()
        << " dz = " << dz
        << " z-enveloppe of the active area = ["<< m_zOffset-m_zSize/2. << " ; " 
        << m_zOffset+m_zSize/2. << " ] "
        << endreq ; 
  }
  Gaudi::XYZPoint loff(0. , 0. , dz-1.); //arbitrary but non-0 z-offset
  Gaudi::XYZPoint  point = geometry->toGlobal(local);
  Gaudi::XYZPoint  goff = geometry->toGlobal(loff);
  return Gaudi::Plane3D ( point - goff , point );
};
//----------------------------------------------------------------------------
// ** Builds the cells from the geometry of the Detector Element
//----------------------------------------------------------------------------

StatusCode DeCalorimeter::buildCells( ) {

  // ** do not initialize, if already initialize

  if( isInitialized() ) { return StatusCode::SUCCESS; }
  int nbCells = 0;

  MsgStream msg( msgSvc(), "buildCells"+ name () );

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

    Gaudi::XYZPoint pointLocal(0,0,0), pointGlobal(0,0,0);
    pointLocal.SetZ( zShowerMax() );


    // ** The center of each cell is specified by step of one cell
    // ** in the local frame. One has to convert to the global frame

    for( int Row = 0 ; m_maxRowCol >= Row    ; ++Row    ) {
      pointLocal.SetY( m_YToXSizeRatio * cellSize[Area] * (Row-m_centerRowCol));

      for( int Column = 0; m_maxRowCol >= Column ; ++Column )  {
        pointLocal.SetX( cellSize[ Area ] * ( Column - m_centerRowCol ) ) ;

        if( !lv->isInside( pointLocal ) ) {  continue ; }

        // Mask the non connected calorimeter cells.
        // Should be only for central part, but is OK also for middle and
        // outer as the hole is quite small...

        if ( ( m_centralHoleX > fabs(Column - m_centerRowCol) ) &&
             ( m_centralHoleY > fabs(Row    - m_centerRowCol) ) ) {
          continue;
        }

        LHCb::CaloCellID id( m_caloIndex, Area , Row , Column ) ;
        m_cells.addEntry( CellParam(id) , id );  // store the new cell

        pointGlobal = geoData->toGlobal( pointLocal );
        m_cells[id].setCenterSize( pointGlobal , cellSize[ Area ] ) ;

        double gain = ( maxEtInCenter() / m_cells[id].sine() ) + maxEtSlope();
        gain        = gain / (double) adcMax() ;
        m_cells[id].setGain( gain ) ;
        ++nbCells;

      } /// end of loop over rows
    } /// end of loop over columns
  } /// end of loop over sub calorimeters

  // ** Compute neighboring cells

  for( CaloVector<CellParam>::iterator pCell = m_cells.begin() ;
       m_cells.end() != pCell ; ++pCell ) {

    LHCb::CaloCellID id       = pCell->cellID();
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
      if ( iColumn >= 0  && iColumn <= m_maxRowCol ) { // inside calorimeter

        for( int iRow = Row - 1 ;  iRow <= Row + 1 ; ++iRow ) {
          if( ( iRow >= 0 )         &&
              ( iRow <= m_maxRowCol ) &&              // inside calorimeter
              ( iRow != Row || iColumn != Column  ) ) {   // not itself...

            LHCb::CaloCellID id2( m_caloIndex, Area , iRow , iColumn );
            if( m_cells[id2].valid() ) {
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

      double x    = m_cells[id].x    ();
      double y    = m_cells[id].y    ();
      double size = m_cells[id].size ();

      for( std::vector<double>::iterator pArea = cellSize.begin() ;
           cellSize.end() != pArea ; ++pArea ) {
        unsigned int iArea = pArea - cellSize.begin() ;

        if( (iArea == Area)   ||
            (iArea == Area+2) ||
            (iArea == Area-2)    ) continue;   // Only in nearby area.

        double sizeArea = *pArea;
        double margin = 0.55 * ( sizeArea + size ) ;

        int cc = (int) ( x / sizeArea  + m_centerRowCol );
        int rc = (int) ( y / sizeArea  + m_centerRowCol );

        // ** To be fast, one checks only cells near
        // ** the expected position (cc and rc)
        // ** We limit to +-4, i.e. the size ratio should be less than 4

        for( int  iRow = rc - 4 ; iRow <= rc + 4 ; ++iRow    ) {
          if( (0 <= iRow ) && (m_maxRowCol >= iRow )    ) {

            for( int  iColumn = cc - 4 ; iColumn <= cc + 4 ; ++iColumn ) {
              if( ( 0 <= iColumn ) && ( m_maxRowCol >= iColumn )   ) {

                LHCb::CaloCellID id2( m_caloIndex, iArea , iRow , iColumn ) ;

                if( m_cells[id2].valid() ) {
                  if( ( fabs( m_cells[id2].x() - x ) <= margin ) &&
                      ( fabs( m_cells[id2].y() - y ) <= margin )    ) {
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

  msg << MSG::DEBUG << "Initialized, index = " << m_caloIndex << ", "
      << nbCells    << " cells." << endreq;

  return StatusCode::SUCCESS;
};

//----------------------------------------------------------------------------
// ** Return the cell at the specified position
//----------------------------------------------------------------------------
LHCb::CaloCellID 
DeCalorimeter::Cell ( const Gaudi::XYZPoint& globalPoint ) const
{
  const  CellParam* pars = Cell_( globalPoint ) ;
  if ( 0 == pars ) { return LHCb::CaloCellID() ; }              
  return pars->cellID() ;
} ;

//----------------------------------------------------------------------------
// ** Construct the Front End card information for each cell
//----------------------------------------------------------------------------

StatusCode DeCalorimeter::buildCards( )  {

  // ** do not initialize, if already initialized

  if( m_cardsInitialized ) { return StatusCode::SUCCESS; }

  int index =  CaloCellCode::CaloNumFromName( name() );
  std::string myName = CaloCellCode::CaloNameFromNum( index ) + "Det";
  MsgStream msg( msgSvc(), myName + ".BuildCards" );

  m_cardsInitialized = true;
  m_feCards.clear();

  Condition* cond = condition( "CellsToCards" );
  if ( 0 == cond ) {
    msg << MSG::DEBUG << "No 'CellsToCards' condition" << endreq;  // SPD case
    return StatusCode::SUCCESS;
  }
  if ( !cond->exists( "cards" ) ) {
    msg << MSG::INFO << "No 'cards' parameters in 'CellsToCards' condition" << endreq;
    return StatusCode::FAILURE;
  }

  std::vector<int> temp = cond->paramAsIntVect( "cards" );
  msg << MSG::DEBUG << "The calorimeter has " << temp.size()/8
      << " front end cards." << endreq;
  int firstCrate = temp[6];
  int prevValidation = -1;
  for ( unsigned int kk = 0; temp.size()/8 > kk  ; ++kk ) {
    int ll = 8*kk;
    int cardNum = temp[ll];
    int area    = temp[ll+1];
    int fCol    = temp[ll+2];
    int fRow    = temp[ll+3];
    int lCol    = temp[ll+4];
    int lRow    = temp[ll+5];
    int crate   = temp[ll+6];
    int slot    = temp[ll+7];

    int downCard     = -1;
    int leftCard     = -1;
    int cornerCard   = -1;
    int previousCard = -1;

    bool first = true;
    for ( int row = fRow; lRow >= row; ++row ) {
      for ( int col = fCol; lCol >= col; ++col ) {
        LHCb::CaloCellID id( m_caloIndex, area, row, col );
        if ( valid( id ) ) {
          m_cells[id].setFeCard( cardNum, col-fCol, row-fRow );

          first = false;

          //== Find the cards TO WHICH this card sends data.
          //== previous card in crate: only non zero row numer
          if ( (row == fRow ) && (0 < fRow) ) {
            LHCb::CaloCellID testID( m_caloIndex, area , row-1 , col );
            if ( cardNumber(testID) >= 0 ) {
              downCard = cardNumber(testID);
            }
          }

          //== Left card:  As the two halves are independend, no 'Left' neighbors for
          //== the central column. Test on 'm_firstRowUp' which is also the first Column Left
          //== Find also the corner card.

          if ( (col == fCol) && (0 < col) && (m_firstRowUp != col) ) {
            LHCb::CaloCellID testID( m_caloIndex, area , row , col-1 );
            if ( cardNumber(testID) >= 0 ) {
              leftCard = cardNumber(testID);
            }
            if ( (row == fRow) && (0 < row) ) {
              LHCb::CaloCellID testID( m_caloIndex, area , row-1 , col-1 );
              if ( cardNumber(testID) >= 0 ) {
                cornerCard = cardNumber(testID);
              }
            }
          }
        }
      }
    }

    int validationCard = 2 * ( crate-firstCrate);
    if ( 7 < slot ) validationCard += 1;
    if ( prevValidation == validationCard ) {
      previousCard = downCard;
    } else {
      prevValidation = validationCard;
    }
    CardParam myCard(area, fRow, fCol );
    myCard.setNeighboringCards( downCard, leftCard, cornerCard , previousCard);
    myCard.setValidationNumber( validationCard );
    m_feCards.push_back( myCard ); // add card

    msg << MSG::DEBUG
        << format ( "Card %3d (crate %2d slot%2d) has down %3d left %3d corner %3d previous %3d validation %2d",
                    cardNum, crate, slot, downCard, leftCard, cornerCard, 
                    previousCard, validationCard )
        << endreq;
  }

  msg << MSG::DEBUG << "Initialized, " << nCards() << " front-end cards." << endreq;

  return StatusCode::SUCCESS;
};

// ============================================================================
/// print to std::stream
// ============================================================================
std::ostream& DeCalorimeter::printOut( std::ostream& os ) const {

  os << "\tDeCalorimeter index=" << m_caloIndex 
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
     << m_maxEtInCenter 
     << std::endl
     << "\t\tIncrease in Et per radian                  =  "
     << m_maxEtSlope    
     << std::endl
     << "\t\tMaximum codage in the ADC                  =  "
     << m_adcMax        
     << std::endl
     << "\t\tConversion from activeE() to energy seen   =  "
     << m_activeToTotal 
     << std::endl
     << "\t\tZ of the shower maximum in the local frame =  "
     << m_zShowerMax    
     << std::endl
     << "\t\tMaximum value for Row/Column               =  "
     << m_maxRowCol     
     << std::endl
     << "\t\tFirst Row or Column  over center           =  "
     << m_firstRowUp      
     << std::endl
     << "\t\tCentral Value = m_maxRowCol/2                =  "
     << m_centerRowCol    
     << std::endl ;

  if( m_initialized ) {
    CaloVector<CellParam>::const_iterator pCell = m_cells.begin() ;
    while( m_cells.end() != pCell ) {
      LHCb::CaloCellID id = (pCell++)->cellID();
      os << "Cell " << id << " Neighbors ";
      std::copy( neighborCells( id ).begin() ,
                 neighborCells( id ).end()   ,
                 OS_iterator<LHCb::CaloCellID,std::ostream>( os , "," ) );
      os << std::endl;
    }
  }

  return os ;
};

// ============================================================================
/// print to MsgStream
// ============================================================================
MsgStream&    DeCalorimeter::printOut( MsgStream&    os ) const {
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
     << m_maxEtInCenter 
     << endreq
     << "\t\tIncrease in Et per radian                  =  "
     << ( m_maxEtSlope    )
     << endreq
     << "\t\tMaximum codage in the ADC                  =  "
     << ( m_adcMax        )
     << endreq
     << "\t\tConversion from activeE() to energy seen   =  "
     << ( m_activeToTotal )
     << endreq
     << "\t\tZ of the shower maximum in the local frame =  "
     << ( m_zShowerMax    )
     << endreq
     << "\t\tMaximum value for Row/Column               =  "
     << (  m_maxRowCol      )
     << endreq
     << "\t\tFirst Row or Column  over center           =  "
     << ( m_firstRowUp      )
     << endreq
     << "\t\tCentral Value = m_maxRowCol/2                =  "
     << ( m_centerRowCol    )
     << endreq ;

  if( m_initialized ) {
    const MSG::Level lev = os.currentLevel();
    os.report( lev - 1 );
    ///
    CaloVector<CellParam>::const_iterator pCell = m_cells.begin() ;
    while( m_cells.end() != pCell ) {
      LHCb::CaloCellID id = (pCell++)->cellID();
      os << " Cell " << id << " Neighbors ";
      std::copy( neighborCells( id ).begin() ,
                 neighborCells( id ).end()   ,
                 OS_iterator<LHCb::CaloCellID,MsgStream>(os,",") );
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
