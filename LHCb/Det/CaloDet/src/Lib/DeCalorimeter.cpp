// $Id: DeCalorimeter.cpp,v 1.46 2008-05-29 21:42:41 odescham Exp $ 
// ============================================================================
#define  CALODET_DECALORIMETER_CPP 1
// ============================================================================
// from STL
// ============================================================================
#include <cmath>
#include <algorithm>
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/SystemOfUnits.h"
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
  ,  m_initialized            ( false  )
  ,  m_cardsInitialized       ( false  )
  ,  m_tell1sInitialized      ( false  )
  ,  m_monitoringInitialized  ( false  )
  ,  m_caloIndex         ( -1         )
  ,  m_maxEtInCenter     ( 10.0 * Gaudi::Units::GeV )
  ,  m_maxEtSlope        ( 0.0  * Gaudi::Units::GeV )
  ,  m_adcMax            ( 4095       )
  ,  m_pedShift          ( 0.6        )
  ,  m_pinPedShift       ( 5.9        )
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


  /// initialize the base 
  StatusCode sc = DetectorElement::initialize();
  if ( sc.isFailure() ) { return sc ; }  
  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "'INITIALIZE DeCalorimeter "<< name() <<endreq;

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

  setEtInCenter   ( gain->paramAsDouble( "EtInCenter"      ) ) ;
  setEtSlope      ( gain->paramAsDouble( "EtSlope"         ) ) ;
  setActiveToTotal( gain->paramAsDouble( "ActiveToTotal"   ) ) ;
  gain->exists("PedShift") ? setPedestalShift( gain->paramAsDouble( "PedShift"        ) ) : setPedestalShift( 0. ) ;
  gain->exists("PinPedShift") ? setPinPedestalShift(gain->paramAsDouble( "PinPedShift"  ) ) : setPinPedestalShift(0.);


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
  
  
  if ( sc.isSuccess() ) {msg << MSG::DEBUG << "INIT cells " << endreq; sc = buildCells (); }
  if ( sc.isSuccess() ) {msg << MSG::DEBUG << "INIT Cards " << endreq; sc = buildCards (); }
  if ( sc.isSuccess() ) {msg << MSG::DEBUG << "INIT Tell1s " << endreq; sc = buildTell1s (); }
  if ( sc.isSuccess() ) {msg << MSG::DEBUG << "INIT Monitoring " << endreq; sc = buildMonitoringSystem (); }
  
  if ( sc.isSuccess() ) {
    msg << MSG::DEBUG << "'DeCalorimeter correctly initalized "<< name() <<endreq;
  }
  else{
    msg << MSG::DEBUG << "'DeCalorimeter initalization failed "<< name() <<endreq;
  }
  

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


  m_caloIndex = CaloCellCode::CaloNumFromName( name() );


  std::string myName = CaloCellCode::CaloNameFromNum( m_caloIndex ) + "Det";
  MsgStream msg( msgSvc(), myName + ".BuildCells" );

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
        m_cells[id].setValid( true );


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
    msg << MSG::ERROR << "No 'cards' parameters in 'CellsToCards' condition" << endreq;
    return StatusCode::FAILURE;
  }

  if ( !cond->exists( "PinArea" ) ) {
    msg << MSG::DEBUG << "No PIN FE-board " << endreq;
    m_pinArea = -1;
  } else{
    m_pinArea = cond->paramAsInt( "PinArea" );
  }


  // Look for maps 
  std::vector<int> vec;
  if( cond->exists( "Mapping"  ) )vec= cond->paramAsIntVect( "Mapping" ) ;
  std::map<int , std::vector<int> > maps;
  unsigned int k = 0;

  while( k < vec.size() ){
    int typ = vec[k];
    msg << MSG::DEBUG << "Building mapping : type =  " << typ << endreq;
    if( typ == CardParam::None ){
      msg << MSG::ERROR << "The map type = " << typ << " is reserved ! Please change in condDB" << endreq;
      return StatusCode::FAILURE;
    }
    k++;
    int siz = 0;
    if(k < vec.size())siz = vec[k];
    for( int id = 0 ; id < siz ; id++ ){
      k++;
      if(k<vec.size())maps[ typ ].push_back( vec[ k] );
    }
    k++;
    if( k == vec.size() ){
      msg << MSG::DEBUG << "Maps from condDB are completed " << endreq;
    }    
  }

  msg << MSG::DEBUG << "Defined " << maps.size() << " channels mapping" << endreq;

  // Cards
  int aSize = cond->paramAsInt( "CardArraySize" );
  std::vector<int> temp = cond->paramAsIntVect( "cards" );
  msg << MSG::DEBUG << "The calorimeter has " << temp.size()/aSize
      << " front end cards." << endreq;
  int firstCrate = temp[6];
  int prevValidation = -1;
  for ( unsigned int kk = 0; temp.size()/aSize > kk  ; ++kk ) {
    int ll = aSize*kk;
    int cardNum = temp[ll];
    int area    = temp[ll+1];
    int fCol    = temp[ll+2];
    int fRow    = temp[ll+3];
    int lCol    = temp[ll+4];
    int lRow    = temp[ll+5];
    int crate   = temp[ll+6];
    int slot    = temp[ll+7];
    int mapType = 0;
    if(aSize == 9)mapType = temp[ll+8];

    // build the FEcard
    CardParam myCard(area, fRow, fCol ,lRow,lCol, cardNum, crate, slot);
    myCard.setMapping( (CardParam::Mapping) mapType );
    if(m_pinArea == area)myCard.setIsPin(true);
    if ( (int) kk != cardNum ) {
      msg << MSG::ERROR << "FE-Card number not in sequence: Found " << cardNum
          << " expect " << kk << endreq;
      return StatusCode::FAILURE;
    }


    std::vector<LHCb::CaloCellID> cellids;
    // Update CellParam
    LHCb::CaloCellID dummy( 0, 0, 0, 0 );
    for ( int row = fRow; lRow >= row; ++row ) {
      for ( int col = fCol; lCol >= col; ++col ) {
        LHCb::CaloCellID id( m_caloIndex, area, row, col );        
        if ( !valid( id ) ) {
          ( mapType == CardParam::None) ? myCard.addID( dummy ) : cellids.push_back( dummy );
        }else{  
          m_cells[id].setFeCard( cardNum, col-fCol, row-fRow );
          ( mapType == CardParam::None) ? myCard.addID( id ) : cellids.push_back( id );
        }
      }
    }
    
    // Re-map if needed
    std::map<int , std::vector<int> >::iterator it = maps.find( mapType );
    if( it == maps.end() && mapType != CardParam::None ){
      msg << MSG::ERROR << "The requested map type = " << mapType << " is not defined in condDB." << endreq;
      return StatusCode::FAILURE;
    }

    if( it != maps.end() ){
      msg << MSG::DEBUG << "The FE-board " << cardNum << " channels are re-mapped using mapping type = " << mapType << endreq;
      std::vector<int> map = (*it).second;
      unsigned int count = 0;
      for( std::vector<int>::iterator i = map.begin() ; i != map.end() ; ++i){
        LHCb::CaloCellID dummy( 0, 0, 0, 0 );
        if( *i < 0 ){
          myCard.addID( dummy );
        }else if( *i < (int) cellids.size() ){
          count++;
          myCard.addID( cellids [ *i ] );
        }else{
          msg << MSG::ERROR << "The FEB "<< cardNum << " has no channel number " << *i 
              << " as expected with mapping " << mapType << endreq;
          return StatusCode::FAILURE;            
        }
      }
      if( cellids.size()  != count ){
        msg << MSG::ERROR << "The mapping with type = " << mapType << " is not consistent with the FEB "<< cardNum << endreq;
        return StatusCode::FAILURE;
      }    
    }
    //
    m_feCards.push_back( myCard ); // add card
  }

  // Selection Board  Type ( e,g.pi0L,pi0G = -1 , hadron master = 0 , had. slave1 = 1, had. slave 2 = 2)
  if ( cond->exists( "HadronSB" ) ) {
    std::vector<int> hadSB = cond->paramAsIntVect( "HadronSB" );
    std::vector<int>::iterator it = hadSB.begin();
    while( hadSB.end() > it ) {
      unsigned int num = (*it++);
      int nb  = (*it++);
      while ( 0 < nb-- ) {
        unsigned int feCard = *it++;
        m_feCards[feCard].setSelectionType( num );
      }
    }
  }
  else{
    msg << MSG::DEBUG << "No 'HadronSB' parameters in 'CellsToCards' condition" << endreq;
  }   



  // 2nd loop on FE-Cards - 
  //== Find the cards TO WHICH this card sends data + Validation board
  for(std::vector<CardParam>::iterator icard = m_feCards.begin() ; icard != m_feCards.end() ; ++icard){
    CardParam& card = *icard;


    int downCard     = -1;
    int leftCard     = -1;
    int cornerCard   = -1;
    int previousCard = -1;


    LHCb::CaloCellID dummy( 0, 0, 0, 0 );
    int area = card.area();
    int fRow = card.firstRow();
    int lRow = card.lastRow();
    int fCol = card.firstColumn();
    int lCol = card.lastColumn();
    int crate= card.crate();
    int slot = card.slot();
    

    for ( int row = fRow; lRow >= row; ++row ) {
      for ( int col = fCol; lCol >= col; ++col ) {
        LHCb::CaloCellID id( m_caloIndex, area, row, col );        
        if( !valid( id ) ) continue;

//== previous card in crate: only non zero row number
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
    if(leftCard == -1)cornerCard = -1 ; // WARNING : corner is transmitted via left !!

    int validationCard = 2 * ( crate-firstCrate);
    if ( 7 < slot ) validationCard += 1;
    if ( prevValidation == validationCard ) {
      previousCard = downCard;
    } else {
      prevValidation = validationCard;
    }

    if( area != m_pinArea ){
      card.setNeighboringCards( downCard, leftCard, cornerCard , previousCard);
      card.setValidationNumber( validationCard );
    }
    
  
      
    msg << MSG::DEBUG
        << format("Card %3d (crate %2d slot %2d) has down %3d left %3d corner %3d previous %3d validation %2d Selection %2d #channels% 5d",
                  card.number() , card.crate(), card.slot(), card.downNumber(), card.leftNumber(),card.cornerNumber(),
                  card.previousNumber(), card.validationNumber(),card.selectionType() ,card.ids().size() ) 
        << endreq;
    msg << MSG::VERBOSE << " >> boundaries : " 
        << cardFirstValidRow(card.number())    << " (" << cardFirstRow( card.number() )     << ") -> "   
        << cardLastValidRow(card.number())     << " (" << cardLastRow( card.number() )      << ")  && "  
        << cardFirstValidColumn(card.number()) << " (" << cardFirstColumn( card.number() )  << ")  -> "  
        << cardLastValidColumn(card.number())  << " (" << cardLastColumn( card.number() )   << ") "     
        << endreq;
  }

  msg << MSG::DEBUG << "Initialized, " << nCards() << " front-end cards." << endreq;

  return StatusCode::SUCCESS;
};


//--------------------------------
// ** Construct the Tell1 Readout 
//--------------------------------

StatusCode DeCalorimeter::buildTell1s( )  {

  if( m_tell1sInitialized ) { return StatusCode::SUCCESS; }
  
  int index =  CaloCellCode::CaloNumFromName( name() );
  std::string myName = CaloCellCode::CaloNameFromNum( index ) + "Det";
  MsgStream msg( msgSvc(), myName + ".BuildTell1s" );
  
  m_tell1sInitialized = true;
  m_tell1Boards.clear();
  
  Condition* cond = condition( "CellsToCards" );
  if ( 0 == cond ) {
    msg << MSG::DEBUG << "No 'CellsToCards' condition" << endreq;  // SPD case
    return StatusCode::SUCCESS;
  }
  if ( !cond->exists( "Tell1" ) ) {
    msg << MSG::ERROR << "No 'Tell1' parameters in 'CellsToCards' condition" << endreq;
    return StatusCode::FAILURE;
  }
  
  std::vector<int> temp = cond->paramAsIntVect( "Tell1" );
  std::vector<int>::iterator it = temp.begin();
  while( temp.end() > it ) {
    unsigned int num = (*it++);
    if ( num != m_tell1Boards.size() ) {
      msg << MSG::ERROR << "TELL1 number not in sequence: Found " << num 
          << " expect " << m_tell1Boards.size() << endreq;
      return StatusCode::FAILURE;
    }
    int nb  = (*it++);
    Tell1Param tell1( num );
    msg << MSG::DEBUG << " create Tell1 number " << num << " handling : " << endreq;
    while ( 0 < nb-- ) {
      unsigned int feCard = *it++;
      msg << MSG::DEBUG << "   ----> FE-Card number " << feCard << endreq;
      if ( m_feCards.size() < feCard ) {
        msg << MSG::ERROR << "**** TELL1 " << num << " reads unknown card " << feCard << endreq;
        return StatusCode::FAILURE;
      }
      if( m_feCards[feCard].tell1() > 0 ){
        msg << MSG::ERROR << "**** FE-Card " << feCard << " already linked to Tell1 board " 
            << m_feCards[feCard].tell1() << " - Cannot link to Tell1 board " << num << endreq;
        return StatusCode::FAILURE;
      }
      m_feCards[feCard].setTell1( num );
      tell1.addFeCard( feCard );
      if( m_feCards[feCard].isPinCard() )tell1.setReadPin( true );
    } 
    if ( tell1.readPin() )m_pinTell1s.push_back( num );
    m_tell1Boards.push_back( tell1 );   
  }  
  msg << MSG::DEBUG << "Initialized, " << nTell1s() << " Tell1 board "<< endreq;  
  return StatusCode::SUCCESS;
}


//----------------------------------------------
// ** Construct the monitoring readout (LED/PIN)
//----------------------------------------------

StatusCode DeCalorimeter::buildMonitoringSystem( )  {

  int index =  CaloCellCode::CaloNumFromName( name() );
  std::string myName = CaloCellCode::CaloNameFromNum( index ) + "Det";
  MsgStream msg( msgSvc(), myName + ".BuildMonitoringSystem" );

  // ** do not initialize, if already initialized
  if( m_monitoringInitialized ) { return StatusCode::SUCCESS; }
  m_monitoringInitialized = true;
  
  // load conditions
  Condition* cond = condition( "MonitoringSystem" );
  if ( 0 == cond ) {
    msg << MSG::DEBUG << "No 'MonitoringSystem' condition" << endreq; // Spd/Prs
    return StatusCode::SUCCESS;
  }
  if ( !cond->exists( "PIN" ) ) {
    msg << MSG::ERROR << "No 'PIN' parameters in 'Monitoring' condition" << endreq;
    return StatusCode::FAILURE;
  }
  
  std::vector<int> temp = cond->paramAsIntVect( "PIN" );
  int ll = -1;
  int kk = -1;
  while ( ll+1 < (int) temp.size() ) {
    // constant size sub-array
    int box       = temp[++ll];
    int pinIndex  = temp[++ll];
    int side      = temp[++ll]; // 0 = Left ; 1 = Right
    int crate     = temp[++ll];
    int slot      = temp[++ll];
    int channel   = temp[++ll]; // == readout channel numbering ( WARNING != ADC numbering)
    int cLed      = temp[++ll];
    int rLed      = temp[++ll];
    int nReg      = temp[++ll];
    // floating size  sub-array      
    int area      = temp[++ll];
    int fCol      = temp[++ll];
    int fRow      = temp[++ll];
    int lCol      = temp[++ll];
    int lRow      = temp[++ll];
    int area2 = -1;
    int fCol2 = -1;
    int fRow2 = -1;
    int lCol2 = -1;
    int lRow2 = -1;
    if( 2 == nReg ){
      area2      = temp[++ll];
      fCol2      = temp[++ll];
      fRow2      = temp[++ll];
      lCol2      = temp[++ll];
      lRow2      = temp[++ll];
    }
    // Define a CaloCellId for PIN-diode : 
    // area = m_pinArea,  
    // relative PinRow/PinCol derives from channel according to standard FEB usage
    // absolute PinRow/PinCol according to Card condDB 

    // Relative position within card (8x4)
    int pinRow = (int) channel/nColCaloCard;
    int pinCol = channel - pinRow * nColCaloCard;
    // Absolute Row & Col wrt to CardParam
    int iCard = cardIndexByCode( crate , slot );
    if( 0 > iCard ){
      msg << MSG::ERROR << " No FE-Card defined in crate/slot "
          << crate << "/" << slot << endreq;
      return StatusCode::FAILURE; 
    }
    LHCb::CaloCellID pinId(m_caloIndex, 
                           m_pinArea, 
                           m_feCards[iCard].firstRow() + pinRow, 
                           m_feCards[iCard].firstColumn() + pinCol );

    CaloPin pin;
    if( 0 <=  m_pins.index(pinId) ) {      
      // the caloPin already exist
      pin = m_pins[pinId];      
      if( box != pin.box() || pinIndex != pin.index() || side != pin.side() ){
        msg << MSG::ERROR << "PIN-diode with same ID but different location already exists " << endreq;
        return StatusCode::FAILURE;
      }
      msg << MSG::DEBUG << "Extended definition for an existing  PIN-diode is found  " << endreq;
    }else{
      // create a new one and store it
      pin = CaloPin(pinId);
      pin.setPinLocation(box,pinIndex,side);
      m_pins.addEntry( pin  , pinId );
      // built a new VALID 'virtual' cell associated with the PIN 
      m_cells.addEntry(CellParam(pinId),pinId);
      m_cells[pinId].addPin(pinId);
      m_cells[pinId].setValid( true ); 
      m_cells[pinId].setFeCard( iCard, pinCol , pinRow);
      // update FE-Card
      std::vector<LHCb::CaloCellID>& ids = m_feCards[iCard].ids();
      ids[channel] = pinId;
      kk++;
      msg << MSG::DEBUG << "PIN diode " << kk << "  ------" << endreq;    
    }
    // update CaloRegion
    m_pins[pinId].addCaloRegion(area, fCol,fRow,lCol,lRow);
    if( area2 >= 0)m_pins[pinId].addCaloRegion(area2,fCol2,fRow2,lCol2,lRow2); 
    // Hcal Leds can be  distributed on 2 areas

    msg << MSG::DEBUG << "-->"  << m_pins[pinId] << endreq;    


    int nleds = m_pins[pinId].leds().size();
    

    // Built the Led system
    int rSize = (lRow-fRow+1)/rLed;
    int cSize = (lCol-fCol+1)/cLed;
    for(int ir = 1; ir != rLed+1 ; ir++){
      for(int ic = 1; ic != cLed+1 ; ic++){

        // Local numbering (ic,ir) -> ic + (ir-1)*cLed
        //   +---+---+  
        //  | 3 | 4 |  
        //  +-------+  
        // | 1 | 2 |  
        // +-------+  

        // LED numbering (jc,jr) -> index - as in Irina document
        //       C-side     A-side
        //   +---+---+  +---+---+
        //  | 2 | 1 |  | 1 | 2 |
        //  +-------+  +-------+
        // | 4 | 3 |  | 3 | 4 |
        // +-------+  +-------+

        int jr = rLed+1-ir;
        int jc = ic;
        if(0 == side)jc = cLed+1-ic;
        int ledIndex = jc + (jr-1)*cLed  + nleds ;//increment !!

        int fColLed = fCol + (ic-1)*cSize ;
        int lColLed = fColLed + cSize -1 ;
        int fRowLed = fRow + (ir-1)*rSize ;
        int lRowLed = fRowLed + rSize -1;        

        // Build the LED
        unsigned int ledNum = m_leds.size();
        CaloLed   led( ledNum );
        led.setIndex(ledIndex);
        led.addCaloRegion(area, fColLed,fRowLed,lColLed,lRowLed);
        led.setPin(pinId);        
        m_pins[pinId].addLed(ledNum); // update CaloPin 
        m_cells[pinId].addLed(ledNum); // add the Led to the virtual pin cell
        // Hcal Leds can be distributed on 2 areas
        if( 0 <= area2 ){
          if( 1 != rLed*cLed ){
            msg << MSG::ERROR 
                << "Don't know how to distributed Leds array on several Calo area" 
                << endreq;
            return StatusCode::FAILURE;
          }
          led.addCaloRegion(area2,fCol2,fRow2,lCol2,lRow2);
        }
        m_leds.push_back(led); // store CaloLed
        msg << MSG::DEBUG << "   --- " << led << endreq;
      }
    }
    


 
    // Link to cell (if not done yet)
    std::vector<int>& leds = m_pins[pinId].leds();
    for(std::vector<int>::iterator iled=leds.begin() + nleds ; 
        iled != leds.end() ; ++iled ) {
      
      int ledNum = (*iled); 
      CaloLed& led = m_leds[ ledNum ];
      msg << MSG::VERBOSE << " LED " << ledNum << endreq;

      for( unsigned int ireg = 0 ; ireg < led.areas().size();  ++ireg ){
        int ar = led.areas()[ireg];
        int fr = led.firstRows()[ireg];
        int lr = led.lastRows()[ireg];
        int fc = led.firstColumns()[ireg];
        int lc = led.lastColumns()[ireg];
       for ( int row = fr ; lr >= row; ++row ) {
         for ( int col = fc; lc >= col; ++col ) {
           LHCb::CaloCellID id( m_caloIndex, ar, row, col );
           msg << MSG::VERBOSE << " Connected cells to LED/Pin " << id << " valid ? " << valid(id) << endreq;
           if ( valid( id ) ) {
             m_cells[id].addPin( pinId );
             m_cells[id].addLed( ledNum );
             m_leds[ledNum].addCell(id);  // update led            
             m_pins[pinId].addCell(id);  // update pin
           }
         }  
       }
      }
    }
  }      
  

  // check all cells (including virtual) are connected to a Led
  for(CaloVector<CellParam>::iterator icel = m_cells.begin(); icel != m_cells.end() ; ++icel){
    msg << MSG::VERBOSE << " Cell " << (*icel).cellID() 
        << " <- Led " << (*icel).leds() << " -> Pin " << (*icel).pins() << endreq;
    if(  0 == (*icel).leds().size() ){
      msg << MSG::ERROR << "Cell id = " << (*icel).cellID() << " is not connect to the monitoring system."<< endreq;
      return StatusCode::FAILURE;
    }    
  }


  msg << MSG::DEBUG << "Initialized, " << m_leds.size() << " monitoring LEDs" << endreq;
  msg << MSG::DEBUG << "Initialized, " << m_pins.size() << " PIN-diodes." << endreq;

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

