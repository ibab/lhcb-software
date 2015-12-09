// ============================================================================
#define  CALODET_DECALORIMETER_CPP 1
// ============================================================================
// STL
// ============================================================================
#include <cmath>
#include <algorithm>
// ============================================================================
// Gaudi
// ============================================================================
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/IUpdateManagerSvc.h"
// ============================================================================
// DetDesc
// ============================================================================
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ILVolume.h"
// ============================================================================
// Kernel
// ============================================================================
#include "Kernel/OSiterator.h"
#include "GaudiKernel/ToStream.h"
// ============================================================================
// CaloDet
// ============================================================================
#include "CaloDet/DeCalorimeter.h"
#include "CaloDet/DeSubCalorimeter.h"
#include "CaloDet/DeSubSubCalorimeter.h"
// ============================================================================
/** @file DeCalorimeter.cpp
 *
 *  Implementation of class :  DeCalorimeter
 *
 *  @author Olivier Callot Olivier.Callot@cern.ch
 *  @author Vanya Belyaev  Ivan.Belyaev@itep.ru
 *  @maintainer Olivier Deschamps odescham@in2p3.fr
 */
// ============================================================================
/*  constructor
 *  @name object name (useless)
 */
// ============================================================================
DeCalorimeter::DeCalorimeter( const std::string& name )
  :  DetectorElement     ( name       )
{
}
// ============================================================================
// object identification
// ============================================================================
const CLID& DeCalorimeter::clID () const { return DeCalorimeter::classID() ; }
// ============================================================================
// intialization method
// ============================================================================
StatusCode DeCalorimeter::initialize()
{
  //======================
  /// initialize the base
  //======================
  StatusCode sc = DetectorElement::initialize();
  if ( sc.isFailure() ) { return sc ; }

  // naming
  m_caloIndex = CaloCellCode::CaloNumFromName( name() );
  m_caloDet   = CaloCellCode::CaloNameFromNum( m_caloIndex ) + "Det";
  MsgStream msg( msgSvc(), m_caloDet );
  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
    msg << MSG::DEBUG << "'INITIALIZE DeCalorimeter "<< name() <<endmsg;

  //=============================
  // collect the sub-calorimeters
  //=============================
  IDetectorElement::IDEContainer& subdets = childIDetectorElements();
  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
    msg << MSG::DEBUG << " Number of subCalorimeter " << subdets.size() << endmsg;
  for(auto child : subdets){
    if ( !child ) { continue ; }
    DeSubCalorimeter* sub = dynamic_cast<DeSubCalorimeter*> ( child ) ;
    Assert ( sub , "no DeSubCalorimeter!" ) ;
    m_subCalos.push_back ( sub ) ;
  }
  Assert ( !m_subCalos  .empty() , "Empty subcalorimeters!" ) ;
  Assert ( geometry() , "Invalid IGeometryInfo!" ) ;


  //====================================================================
  //== A few geometrical parameters from UserParameters in structure.xml
  //====================================================================
  auto pars = paramNames() ;

  /// Y to X size ratio
  auto it = std::find( pars.begin() , pars.end () , "YToXSizeRatio" );
  m_YToXSizeRatio = ( it != pars.end() ? param<double>( *it ) : 0);
  //ZOffset
  it = std::find( pars.begin() , pars.end () , "ZOffset" );
  m_zOffset = ( it !=  pars.end() ? param<double>( *it ) : 0 );
 //Zsize
  it = std::find( pars.begin() , pars.end () , "ZSize" );
  m_zSize = ( it != pars.end() ? param<double>( *it ) : 0 );


  //=============================================
  //== Get parameters from the condition database
  //=============================================

  // WARNING : the order is important !!

  bool mandatory = true;

  // condition : 'Hardware' (MUST EXIST)
  if( !loadCondition( m_hardware, "Hardware" , mandatory) )return StatusCode::FAILURE;
  updMgrSvc()->registerCondition(this, m_hardware.path(), &DeCalorimeter::updHardware);

  // IGeometry condition (MUST EXIST)
  if( !geometry() )return StatusCode::FAILURE;
  updMgrSvc()->registerCondition(this, geometry(),&DeCalorimeter::updGeometry );

  // condition : 'Reco' (MUST exist)
  if( !loadCondition( m_reco, "Reco" , mandatory) )return StatusCode::FAILURE;
  updMgrSvc()->registerCondition(this, m_reco.path(), &DeCalorimeter::updReco);

  // condition : 'Readout' (MUST EXIST : SPD points onto PRS readout in recent DDDB)
  if( !loadCondition( m_readout, "Readout", mandatory ) )return StatusCode::FAILURE;
  updMgrSvc()->registerCondition(this, m_readout.path(), &DeCalorimeter::updReadout);

  // condition : 'Gain' (MUST EXIST)
  if( !loadCondition( m_gain, "Gain",mandatory  ) )return StatusCode::FAILURE;
  updMgrSvc()->registerCondition(this, m_gain.path(), &DeCalorimeter::updGain);


  // Optional conditions
  for (auto& fc : { std::make_tuple( std::ref(m_monitor), "Monitoring",&DeCalorimeter::updMonitor),
                    std::make_tuple( std::ref(m_calib), "Calibration", &DeCalorimeter::updCalib),
                    std::make_tuple( std::ref(m_pileUp), "PileUpOffset" , &DeCalorimeter::updPileUp),
                    std::make_tuple( std::ref(m_numericGains), "NumericGains" ,&DeCalorimeter::updNumGains),
                    std::make_tuple( std::ref(m_LEDReference), "LEDReference" ,&DeCalorimeter::updLEDReference),
                    std::make_tuple( std::ref(m_quality), "Quality" ,&DeCalorimeter::updQuality),
                    std::make_tuple( std::ref(m_l0calib), "L0Calibration" ,&DeCalorimeter::updL0Calib)
                  } ) {
      if (loadCondition( std::get<0>(fc), std::get<1>(fc) )) {
            updMgrSvc()->registerCondition(this, std::get<0>(fc).path(), std::get<2>(fc) );
      }
  }

  // ================= INITIALIZE ALL CONDITIONS

  sc = updMgrSvc()->update(this);

  // ===========================================

  if ( sc.isSuccess() ) {
    m_initialized = true;

    // DEBUG
    if( UNLIKELY( msg.level() <= MSG::DEBUG ) ) {
      msg << MSG::DEBUG << "'DeCalorimeter " << name() << " correctly initalized with : " <<endmsg;
      msg << MSG::DEBUG << " - " << numberOfCells()  << " channels "    << endmsg;
      msg << MSG::DEBUG << " - " << numberOfCards()  << " FE-boards "   << endmsg;
      msg << MSG::DEBUG << " - " << numberOfTell1s() << " Tell1s "      << endmsg;
      msg << MSG::DEBUG << " - " << numberOfPins()   << " PIN-diodes "  << endmsg;
      msg << MSG::DEBUG << " - " << numberOfLeds()   << " LEDs "        << endmsg;
    }

    // VERBOSE
    if( UNLIKELY( msg.level() <= MSG::VERBOSE ) )
      msg << MSG::VERBOSE << "DDDB parameters : "
          << " Y/X ratio = " <<  m_YToXSizeRatio
          << " zOffset   = " <<  m_zOffset
          << " zSize     = " <<  m_zSize
          << endmsg;

    // LONG VERBOSE
    MsgStream msg1( msgSvc(), m_caloDet + ".Gain" );
    MsgStream msg2( msgSvc(), m_caloDet + ".L0Gain" );
    MsgStream msg3( msgSvc(), m_caloDet + ".Cells" );
    unsigned int nGain=0;
    for( const auto& ic : m_cells) {
      LHCb::CaloCellID id = ic.cellID() ;

      unsigned int Area   = id.area ( ) ;
      double thGain = 0.;
      if( ! id.isPin() ){
        thGain=( maxEtInCenter(Area) / m_cells[id].sine() ) + maxEtSlope(Area);
        thGain        /= (double) adcMax() ;
      }

      if( UNLIKELY( msg1.level() <= MSG::VERBOSE ) )
        msg1 << MSG::VERBOSE << "Expected gain   : " << id << " => "
             <<  cellGain(id) <<  " MeV/ADC "
             << " = (nominal * calib * shift) = "
             << m_cells[id].nominalGain() << " x " << m_cells[id].calibration()
             << " / " << m_cells[id].gainShift() << "   | Theoretical nominal gain [" << thGain << "]"
             << endmsg;
      if( cellGain( id ) <= 0 && !id.isPin() )nGain++;
      if( m_caloDet == "EcalDet" || m_caloDet == "HcalDet"){
        double ect = cellGain(id) *  m_cells[id].sine() / m_l0Et *1024;
        double act = (double) m_cells[id].l0Constant();
        double ag  = 0;
        if(act > 0 )ag  = cellGain(id) *  m_cells[id].sine() / act * 1024;
        if( UNLIKELY( msg2.level() <= MSG::VERBOSE ) )
          msg2 << MSG::VERBOSE << "Chan : " << id << " : nominal L0gain => "
               << m_l0Et<< " MeV/ADC => expected L0Cte is : " << (int) ect
               << "  | actual L0Cte : " << act << " => "  << ag << " MeV/ADC"
               << endmsg;
      }
    }
    // Check gain
    if(nGain > 0 && msg.level() <= MSG::DEBUG)
      msg << MSG::DEBUG << "Found " << nGain << " channel(s) with null gain " << endmsg;
    // Verbosity
    if( UNLIKELY( msg3.level() <= MSG::VERBOSE ) ) {
      msg3 << MSG::VERBOSE << " ----------- List of " << CaloCellCode::CaloNameFromNum( m_caloIndex )
           << " channels " << " ----------- " << endmsg;
      long k = 1;
      for( const auto& ic : m_cells ) {
          msg3 << MSG::VERBOSE << k << " : " << ic.cellID()
               << " pos : (" << ic.x() << "," << ic.y() << "," << ic.z()
               << ") "<< " #neigh " << ic.neighbors().size() << " | ";
          if( k%2 == 0) msg3 << MSG::VERBOSE <<endmsg;
        k++;
      }
      msg3 << MSG::VERBOSE << endmsg;
    }
  }
  else{
    msg << MSG::ERROR << "'DeCalorimeter initalization failed "<< name() <<endmsg;
  }


  return sc;
}
// ============================================================================
/// Return a reference (tilted) plane
// ============================================================================
Gaudi::Plane3D DeCalorimeter::plane( const double dz) const{
  if ( fabs(dz-m_zOffset) > m_zSize/2. ) {
    MsgStream msg( msgSvc(), name() + "Plane " );
    msg << MSG::WARNING
        << " THE REQUESTED PLANE IS OUTSIDE THE ACTIVE VOLUME of : " << name()
        << " dz = " << dz
        << " z-enveloppe of the active area = ["<< m_zOffset-m_zSize/2. << " ; "
        << m_zOffset+m_zSize/2. << " ] "
        << endmsg ;
  }
  const IGeometryInfo* geometry = this->geometry() ;
  Gaudi::XYZPoint  point = geometry->toGlobal(Gaudi::XYZPoint{0. , 0. , dz});
  Gaudi::XYZPoint  goff = geometry->toGlobal(Gaudi::XYZPoint{0. , 0. , dz-1.}); //arbitrary but non-0 z-offset
  return Gaudi::Plane3D ( point - goff , point );
}
//----------------------------------------------------------------------------
// ** Builds the cells from the geometry of the Detector Element
//----------------------------------------------------------------------------

StatusCode DeCalorimeter::buildCells( ) {


  // init
  MsgStream msg( msgSvc(), m_caloDet + ".BuildCells" );
  int nbCells = 0;
  std::vector<double> cellSize;
  //  loop over sub-calo (calo halves)
  for(auto & elem : m_subCalos){
    m_nArea = (elem)->subSubCalos().size();
    if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
      msg << MSG::DEBUG << " SubCalos :  Side " << (elem)->sideName()
          << " | #areas " << (elem)->subSubCalos().size() << endmsg;
    int side = (elem)->side();
    // loop over sub-sub-calo (calo areas)
    SubSubCalos subSubCalos = (elem)->subSubCalos();
    for(auto & subSubCalo : subSubCalos){
      unsigned int Area = subSubCalo->area();
      double xOffset = -side * subSubCalo->xSize()/2.;
      if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
        msg << MSG::DEBUG<< "SubSubCalo : Area " << Area
            << " | cellSize : " << (subSubCalo)->cellSize()
            << " | local center X-offset: " << xOffset
            << endmsg;
      if( cellSize.size() <= Area ) cellSize.resize(Area+1 ,0);
      cellSize[Area]   = (subSubCalo)->cellSize();

      // get geo data
      const IGeometryInfo* geoData = subSubCalo->geometry() ;
      Assert( geoData        , " Unable to extract IGeometryInfo* !"  );
      const ILVolume* lv = geoData->lvolume();
      Assert( lv             , " Unable to extract ILVolume* !"       );
      //
      Gaudi::XYZPoint pointLocal(0,0,0), pointGlobal(0,0,0);

      // ** The center of each cell is specified by step of one cell
      // ** in the local frame. One has to convert to the global frame
      //
      for( int Row = 0 ; m_maxRowCol >= Row    ; ++Row    ) {
        pointLocal.SetY( m_YToXSizeRatio * cellSize[Area] * (Row-m_centerRowCol));
        for( int Column = 0; m_maxRowCol >= Column ; ++Column )  {
          pointLocal.SetX( cellSize[Area] * ( Column - m_centerRowCol ) + xOffset) ;
          if( !lv->isInside( pointLocal ) ) {  continue ; }
          // Mask the non connected calorimeter cells.
          // Should be only for central part, but is OK also for middle and
          // outer as the hole is quite small...
          if ( ( m_centralHoleX > fabs(Column - m_centerRowCol) ) &&
               ( m_centralHoleY > fabs(Row    - m_centerRowCol) ) ) {
            continue;
          }

          LHCb::CaloCellID id( m_caloIndex, Area , Row , Column ) ;
          if( 0 >  m_cells.index(id)) m_cells.addEntry( CellParam(id) , id );  // store the new cell if it does not already exist
          pointGlobal = geoData->toGlobal( pointLocal );
          m_cells[id].setCenterSize( pointGlobal , cellSize[Area] ) ;
          m_cells[id].setValid( true );
          ++nbCells;

        }  // loop over columns
      }  // loop over rows
    } // loop over half-areas
  } // loop over half-calos




  // ** Compute neighboring cells
  for( auto& cell : m_cells) {

    LHCb::CaloCellID id       = cell.cellID();
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
              cell.addZsupNeighbor( id2 ) ;
              cell.addNeighbor( id2 ) ;
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

      for( auto pArea = cellSize.begin() ;
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
                    cell.addNeighbor( id2 ) ;
                  }
                }
              }
            } // Loop on columns
          }
        } // Loop on Rows
      } /// end of loop over all areas
    }
  } // end of loop ovel all cells


  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
    msg << MSG::DEBUG << "Initialized  " << m_caloDet << ", "
        << nbCells    << " channels" << endmsg;

  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------
// ** Return the cell at the specified position
//----------------------------------------------------------------------------
LHCb::CaloCellID
DeCalorimeter::Cell ( const Gaudi::XYZPoint& globalPoint ) const
{
  const  CellParam* pars = Cell_( globalPoint ) ;
  if ( nullptr == pars ) { return LHCb::CaloCellID() ; }
  return pars->cellID() ;
}

//----------------------------------------------------------------------------
// ** Construct the Front End card information for each cell
//----------------------------------------------------------------------------

StatusCode DeCalorimeter::buildCards( )  {


  // init
  MsgStream msg( msgSvc(), m_caloDet + ".BuildCards" );
  m_feCards.clear();
  m_pinArea = 0;

  // check the condition
  if( !hasCondition("Readout") )return StatusCode::SUCCESS ;
  // condition not mandatory (e.g. Spd)
  // get FEB readout
  if ( !m_readout->exists( "FEB" ) || !m_readout->exists( "size") ) {
    msg << MSG::ERROR << "incomplete 'cards' description in 'Readout' condition" << endmsg;
    return StatusCode::FAILURE;
  }
  // get pin ID.area
  if ( !m_readout->exists( "pinArea" ) ) {
    if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
      msg << MSG::DEBUG << "No PIN FE-board for " << m_caloDet << endmsg;
    m_pinArea = -1;
  } else{
    m_pinArea = m_readout->param<int>( "pinArea" ) ;
  }


  //  channel remapping
  std::map<int , std::vector<int> > maps;
  if( m_readout->exists( "Mapping"  ) ) {
    const auto& vec = m_readout->param<std::vector<int>>( "Mapping" ) ;
    unsigned int k = 0;

    while( k < vec.size() ){
      int typ = vec[k];
      if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
        msg << MSG::DEBUG << "Building mapping : type =  " << typ << endmsg;
      if( typ == CardParam::None ){
        msg << MSG::ERROR << "The map type = " << typ
            << " is reserved ! Please change in condDB" << endmsg;
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
      if( UNLIKELY( (msg.level() <= MSG::DEBUG) && (k == vec.size()) ) ) {
        msg << MSG::DEBUG << "Maps from condDB are completed " << endmsg;
      }
    }
  }

  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
    msg << MSG::DEBUG << "Defined " << maps.size() << " different channel mappings" << endmsg;

  // decode cards array
  // ------------------
  auto  aSize = m_readout->param<int>( "size" );
  const auto& temp = m_readout->param<std::vector<int>>( "FEB" );
  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
    msg << MSG::DEBUG << "The calorimeter has " << temp.size()/aSize << " front end cards." << endmsg;
  int firstCrate = temp[6];
  for ( unsigned int kk = 0; kk*aSize < temp.size() ; ++kk ) {
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
    if(aSize >= 9)mapType = temp[ll+8];

    // build the FEcard
    CardParam myCard(area, fRow, fCol ,lRow,lCol, cardNum, crate, slot);
    myCard.setMapping( (CardParam::Mapping) mapType );
    if(m_pinArea == area)myCard.setIsPin(true);
    if ( (int) kk != cardNum ) {
      msg << MSG::ERROR << "FE-Card number not in sequence: Found "
          << cardNum  << " expect " << kk << endmsg;
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
    auto it = maps.find( mapType );
    if( it == maps.end() && mapType != CardParam::None ){
      msg << MSG::ERROR << "The requested map type = " << mapType
          << " is not defined in condDB." << endmsg;
      return StatusCode::FAILURE;
    }

    if( it != maps.end() ){
      if( UNLIKELY( msg.level() <= MSG::VERBOSE ) )
        msg << MSG::VERBOSE << "The FE-board " << cardNum
            << " channels are re-mapped using mapping type = " << mapType << endmsg;
      std::vector<int> map = (*it).second;
      unsigned int count = 0;
      for(auto & elem : map){
        LHCb::CaloCellID dummy2( 0, 0, 0, 0 );
        if( elem < 0 ){
          myCard.addID( dummy2 );
        }else if( elem < (int) cellids.size() ){
          count++;
          myCard.addID( cellids [ elem ] );
        }else{
          msg << MSG::ERROR << "The FEB "<< cardNum << " has no channel number " << elem
              << " as it is expected with mapping " << mapType << endmsg;
          return StatusCode::FAILURE;
        }
      }
      if( cellids.size()  != count ){
        msg << MSG::ERROR << "The mapping with type = " << mapType
            << " is not consistent with the FEB "<< cardNum << endmsg;
        return StatusCode::FAILURE;
      }
    }
    //
    m_feCards.push_back( myCard ); // add card
  }

  // Selection Board  Type ( e,g.pi0L,pi0G = -1 , hadron master = 0 , had. slave1 = 1, had. slave 2 = 2)

  // ---------- HcalDet only : SelectionBoard -> Hcal FE relation
  if ( m_readout->exists( "SelectionBoard" ) ) {
    const auto& sb = m_readout->param<std::vector<int>>( "SelectionBoard" );
    auto it = sb.begin();
    while( sb.end() > it ) {
      unsigned int num = (*it++);
      int nb  = (*it++);
      while ( 0 < nb-- ) {
        unsigned int feCard = *it++;
        m_feCards[feCard].setSelectionType( num );
      }
    }
  }
  else{
    if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
      msg << MSG::DEBUG << "No 'SelectionBoard' parameters in 'Readout' condition" << endmsg;
  }

  // EcalDet only :  Validation Board -> Hcal FE relation
  if ( m_readout->exists( "ValidationBoard" ) ) {
    const auto& vb = m_readout->param<std::vector<int>>( "ValidationBoard" );
    auto it = vb.begin();
    while( vb.end() > it ) {
      unsigned int num = (*it++);
      int nb  = (*it++);
      std::vector<int> hcalFe;
      while ( 0 < nb-- ) {
        hcalFe.push_back( *it++);
      }
      m_valCards[num]=hcalFe;
    }
  }
  else{
    if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
      msg << MSG::DEBUG << "No 'ValidationBoard' parameters in 'Readout' condition" << endmsg;
  }

  // 2nd loop on FE-Cards -
  //== Find the cards TO WHICH this card sends L0 data + Validation board
  int prevValidation = -1;
  for(auto & card : m_feCards){



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
            LHCb::CaloCellID testID2( m_caloIndex, area , row-1 , col-1 );
            if ( cardNumber(testID2) >= 0 ) {
              cornerCard = cardNumber(testID2);
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

    if( area != m_pinArea && !card.isParasitic() ){
      card.setNeighboringCards( downCard, leftCard, cornerCard , previousCard);
      card.setValidationNumber( validationCard );
    }

    if( UNLIKELY( msg.level() <= MSG::DEBUG ) ) msg << MSG::DEBUG
        << format("Card %3d (crate %2d slot %2d) has down %3d left %3d corner %3d previous %3d ",
                  card.number() , card.crate(), card.slot(),
                  card.downNumber(), card.leftNumber(),card.cornerNumber(),card.previousNumber() )
        << format(" validation %2d Selection %2d #channels% 5d",
                  card.validationNumber(),card.selectionType() ,card.ids().size())
        << endmsg;
    if( UNLIKELY( msg.level() <= MSG::VERBOSE ) ) msg << MSG::VERBOSE << " >> boundaries : "
        << cardFirstValidRow(card.number())    << " (" << cardFirstRow( card.number() )     << ") -> "
        << cardLastValidRow(card.number())     << " (" << cardLastRow( card.number() )      << ")  && "
        << cardFirstValidColumn(card.number()) << " (" << cardFirstColumn( card.number() )  << ")  -> "
        << cardLastValidColumn(card.number())  << " (" << cardLastColumn( card.number() )   << ") "
        << endmsg;
  }

  if( UNLIKELY( msg.level() <= MSG::DEBUG ) ) msg << MSG::DEBUG
      << "Initialized, " << numberOfCards() << " front-end cards." << endmsg;
  return StatusCode::SUCCESS;
}


//--------------------------------
// ** Construct the Tell1 Readout
//--------------------------------

StatusCode DeCalorimeter::buildTell1s( )  {


  // init
  MsgStream msg( msgSvc(), m_caloDet + ".BuildTell1s" );
  m_tell1Boards.clear();

  // check the condition
  if( 0 == m_readout)return StatusCode::SUCCESS;

  // Tell1 description
  if ( !m_readout->exists( "Tell1" ) ) {
    msg << MSG::ERROR << "No 'Tell1' parameters in 'Readout' condition" << endmsg;
    return StatusCode::FAILURE;
  }

  const auto& temp = m_readout->param<std::vector<int>>( "Tell1" );
  auto it = temp.begin();
  while( temp.end() > it ) {
    unsigned int num = (*it++);
    if ( num != m_tell1Boards.size() ) {
      msg << MSG::ERROR << "TELL1 number not in sequence: Found " << num
          << " expect " << m_tell1Boards.size() << endmsg;
      return StatusCode::FAILURE;
    }
    int nb  = (*it++);
    Tell1Param tell1( num );
    if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
      msg << MSG::DEBUG << " create Tell1 number " << num << " handling : " << endmsg;
    while ( 0 < nb-- ) {
      unsigned int feCard = *it++;
      if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
        msg << MSG::DEBUG << "   ----> FE-Card number " << feCard << endmsg;
      if ( m_feCards.size() < feCard ) {
        msg << MSG::ERROR << "**** TELL1 " << num << " reads unknown card " << feCard << endmsg;
        return StatusCode::FAILURE;
      }
      if( m_feCards[feCard].tell1() > 0 ){
        msg << MSG::ERROR << "**** FE-Card " << feCard << " already linked to Tell1 board "
            << m_feCards[feCard].tell1() << " - Cannot link to Tell1 board " << num << endmsg;
        return StatusCode::FAILURE;
      }
      m_feCards[feCard].setTell1( num );
      tell1.addFeCard( feCard );
      if( m_feCards[feCard].isPinCard() )tell1.setReadPin( true );
    }
    if ( tell1.readPin() )m_pinTell1s.push_back( num );
    m_tell1Boards.push_back( tell1 );
  }
  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
    msg << MSG::DEBUG << "Initialized, " << numberOfTell1s() << " Tell1 board "<< endmsg;
  return StatusCode::SUCCESS;
}


//----------------------------------------------
// ** Construct the monitoring readout (LED/PIN)
//----------------------------------------------

StatusCode DeCalorimeter::buildMonitoring( )  {
  // init
  MsgStream msg( msgSvc(), m_caloDet + ".BuildMonitoring" );
  m_pins.clear();
  m_leds.clear();

  // check conditions
  if ( !hasCondition("Monitoring") )return StatusCode::SUCCESS;// monitoring not mandatory (Prs/Spd)

  // check PIN array
  if ( !m_monitor->exists( "PIN" ) ) {
    msg << MSG::ERROR << "No 'PIN' parameters in 'Monitoring' condition" << endmsg;
    return StatusCode::FAILURE;
  }

  const auto& temp = m_monitor->param<std::vector<int>>( "PIN" );
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
          << crate << "/" << slot << endmsg;
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
        msg << MSG::ERROR << "PIN-diode with same ID but different location already exists " << endmsg;
        return StatusCode::FAILURE;
      }
      if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
        msg << MSG::DEBUG << "Extended definition for an existing  PIN-diode is found  " << endmsg;
    }else{
      // create a new one and store it
      pin = CaloPin(pinId);
      pin.setPinLocation(box,pinIndex,side);
      m_pins.addEntry( pin  , pinId );
      // built a new VALID 'virtual' cell associated with the PIN if it does not exists !!
      if( 0 >  m_cells.index(pinId))m_cells.addEntry(CellParam(pinId),pinId);
      m_cells[pinId].addPin(pinId);
      m_cells[pinId].setValid( true );
      m_cells[pinId].setFeCard( iCard, pinCol , pinRow);
      // update FE-Card
      std::vector<LHCb::CaloCellID>& ids = (std::vector<LHCb::CaloCellID>&) m_feCards[iCard].ids(); // no-const
      ids[channel] = pinId;
      kk++;
      if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
        msg << MSG::DEBUG << "PIN diode " << kk << "  ------" << endmsg;
    }
    // update CaloRegion
    m_pins[pinId].addCaloRegion(area, fCol,fRow,lCol,lRow);
    if( area2 >= 0)m_pins[pinId].addCaloRegion(area2,fCol2,fRow2,lCol2,lRow2);
    // Hcal Leds can be  distributed on 2 areas

    if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
      msg << MSG::DEBUG << "-->"  << m_pins[pinId] << endmsg;


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
                << endmsg;
            return StatusCode::FAILURE;
          }
          led.addCaloRegion(area2,fCol2,fRow2,lCol2,lRow2);
        }
        m_leds.push_back(led); // store CaloLed
        if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
          msg << MSG::DEBUG << "   --- " << led << endmsg;
      }
    }




    // Link to cell (if not done yet)
    std::vector<int>& leds = (std::vector<int>&) m_pins[pinId].leds(); // non-const conversion
    for(auto iled=leds.begin() + nleds ;
        iled != leds.end() ; ++iled ) {

      int ledNum = (*iled);
      CaloLed& led = m_leds[ ledNum ];
      if( UNLIKELY( msg.level() <= MSG::VERBOSE ) )
        msg << MSG::VERBOSE << " LED " << ledNum << endmsg;

      for( unsigned int ireg = 0 ; ireg < led.areas().size();  ++ireg ){
        int ar = led.areas()[ireg];
        int fr = led.firstRows()[ireg];
        int lr = led.lastRows()[ireg];
        int fc = led.firstColumns()[ireg];
        int lc = led.lastColumns()[ireg];
       for ( int row = fr ; lr >= row; ++row ) {
         for ( int col = fc; lc >= col; ++col ) {
           LHCb::CaloCellID id( m_caloIndex, ar, row, col );
           if( UNLIKELY( msg.level() <= MSG::VERBOSE ) )
             msg << MSG::VERBOSE << " Connected cells to LED/Pin " << id << " valid ? " << valid(id) << endmsg;
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
  for(auto & elem : m_cells){
    if( UNLIKELY( msg.level() <= MSG::VERBOSE ) )
      msg << MSG::VERBOSE << " Cell " << (elem).cellID()
          << " <- Led " << (elem).leds() << " -> Pin " << (elem).pins() << endmsg;
    if(  0 == (elem).leds().size() ){
      msg << MSG::ERROR << "Cell id = " << (elem).cellID() << " is not connect to the monitoring system."<< endmsg;
      return StatusCode::FAILURE;
    }
  }


  if( UNLIKELY( msg.level() <= MSG::DEBUG ) ) {
    msg << MSG::DEBUG << "Initialized, " << m_leds.size() << " monitoring LEDs" << endmsg;
    msg << MSG::DEBUG << "Initialized, " << m_pins.size() << " PIN-diodes." << endmsg;
  }

  return StatusCode::SUCCESS;
}

// pileUp subtraction -------------- //
StatusCode DeCalorimeter::getPileUpOffset( )  {
  // init
  MsgStream msg( msgSvc(), m_caloDet + ".PileUpOffset" );
  if ( !hasCondition("PileUpOffset") )return StatusCode::SUCCESS; // calibration not mandatory

  if ( !m_pileUp->exists( "data" )
       || !m_pileUp->exists( "size")
       || !m_pileUp->exists( "Method")
       || !m_pileUp->exists( "Bin")
       || !m_pileUp->exists( "Min") ) {

    if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
      msg << MSG::DEBUG << "No 'data' in 'PileUpOffset' condition : will assume offset = 0." << endmsg;
    return StatusCode::SUCCESS;
  }


  m_puMeth = m_pileUp->param<int>("Method");
  m_puBin  = m_pileUp->param<int>("Bin");
  m_puMin  = m_pileUp->param<int>("Min");

  if( UNLIKELY( msg.level() <= MSG::VERBOSE ) )
    msg << MSG::VERBOSE << "Pileup offset substraction : method = " << m_puMeth
        << " ; Min = " << m_puMin
        << " ; Bin = " << m_puBin << endmsg;


  auto size     = m_pileUp->param<int>( "size" );
  const auto& data = m_pileUp->param<std::vector<double>>( "data" );
  int count = 0;
  for ( unsigned int kk = 0; kk*size < data.size() ; ++kk ) {
    int ll = size*kk;
    double cell   = data[ll];
    double offs   = data[ll+1];
    double eoffs  = (size>2) ? data[ll+2] : 0;

    LHCb::CaloCellID id = LHCb::CaloCellID( (int) cell );
    id.setCalo( CaloCellCode::CaloNumFromName( name() ));
    if( m_cells[id].valid() ){
      m_cells[id].setPileUpOffset( offs,eoffs );
      m_cells[id].setPileUpOffsetSPD( offs,eoffs ); // set the same offset for SPD-tag/-untag cluster by default
      if( UNLIKELY( msg.level() <= MSG::VERBOSE ) )
        msg << MSG::VERBOSE << "Added pileup offset for channel " << id
            << " : <offset> = " << offs << " ( RMS = " << eoffs << " )" << endmsg;
    }else
      msg << MSG::WARNING << "Trying to add pileup offset on non-valid channel : " << id << endmsg;
    count++;
  }

  // alternative map for SPD-tagged clusters (FACULTATIVE)
  int ccount = 0;
  if( m_pileUp->exists("dataSPD") ){
    const auto& dataSPD = m_pileUp->param<std::vector<double>>( "dataSPD" );
    for ( unsigned int kk = 0;  kk*size < dataSPD.size() ; ++kk ) {
      int ll = size*kk;
      double cell   = dataSPD[ll];
      double offs   = dataSPD[ll+1];
      double eoffs  = (size>2) ? dataSPD[ll+2] : 0;
      LHCb::CaloCellID id = LHCb::CaloCellID( (int) cell );
      id.setCalo( CaloCellCode::CaloNumFromName( name() ));
      if( m_cells[id].valid() ){
        m_cells[id].setPileUpOffsetSPD( offs,eoffs );
        if( UNLIKELY( msg.level() <= MSG::VERBOSE ) )
          msg << MSG::VERBOSE << "Added pileup (SPD-tag) offset for channel " << id
              << " : <offset> = " << offs << " ( RMS = " << eoffs << " )" << endmsg;
      }else
        msg << MSG::WARNING << "Trying to add pileup offset on non-valid channel : " << id << endmsg;
      ccount++;
    }
  }
  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
  {
    msg << MSG::DEBUG << "Pileup offset added for " << count << " channel(s) " << endmsg;
    msg << MSG::DEBUG << "Alternative Pileup offset added for " << ccount << " channel(s) " << endmsg;
  }
  return StatusCode::SUCCESS;
}


// absolute calibration -------------- //
StatusCode DeCalorimeter::getCalibration( )  {
  // init
  MsgStream msg( msgSvc(), m_caloDet + ".Calibration" );

  // check conditions
  if ( !hasCondition("Calibration") )return StatusCode::SUCCESS; // calibration not mandatory (assume deltaGain = 1.)


  // check array
  if ( !m_calib->exists( "data" ) || !m_calib->exists( "size") ) {
    if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
      msg << MSG::DEBUG << "No 'data' in 'Calibration' condition : will assume deltaG = 1." << endmsg;
    return StatusCode::SUCCESS;
  }
  int size = m_calib->param<int>( "size" );
  const auto& data = m_calib->param<std::vector<double>>( "data" );
  if( !data.empty() && size > 2)msg << MSG::WARNING << "Reference data are not longer supported within 'Calibration' condition "<<
    "- use dedicated 'LEDReference' condition instead" << endmsg;
  int count = 0;
  for ( unsigned int kk = 0; kk*size < data.size(); ++kk ) {
    int ll = size*kk;
    double cell   = data[ll];
    double dg     = data[ll+1];
    //double ledDataRef = (size>2) ? data[ll+2] : 0.;
    //double ledMoniRef = (size>3) ? data[ll+3] : 1.;

    LHCb::CaloCellID id = LHCb::CaloCellID( (int) cell );
    id.setCalo( CaloCellCode::CaloNumFromName( name() ));
    //get cell
    if( m_cells[id].valid() ){
      m_cells[id].setCalibration( dg );
      // m_cells[id].setLedDataRef( ledDataRef, ledMoniRef );
      if( UNLIKELY( msg.level() <= MSG::VERBOSE ) )
        msg << MSG::VERBOSE << "Added calibration for channel " << id
            << " : dG = " << dg
          // << " Reference (<PMT> , <PMT/PIN>) datafrom LED signal = (" << ledDataRef << "," << ledMoniRef << ")"
            <<endmsg;
      count++;
    }else{
      msg << MSG::WARNING << "Trying to add calibration on non-valid channel : " << id << endmsg;
    }
  }
  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
    msg << MSG::DEBUG << "Calibration constant added for " << count << " channel(s) " << endmsg;
  return StatusCode::SUCCESS;
}

// LED reference data for relative calibration ------------------ //
StatusCode DeCalorimeter::getLEDReference( )  {
  // init
  MsgStream msg( msgSvc(), m_caloDet + ".LEDReference" );

  // check conditions
  if ( !hasCondition("LEDReference") )return StatusCode::SUCCESS; // not mandatory


  // check array
  if ( !m_LEDReference->exists( "data" ) ) {
    if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
      msg << MSG::DEBUG << "No 'data' in 'LEDReference' condition" << endmsg;
    return StatusCode::FAILURE;
  }
  auto size = m_LEDReference->param<int>( "size" );
  const auto& data = m_LEDReference->param<std::vector<double>>( "data" );

  int count = 0;
  for ( unsigned int kk = 0; kk*size < data.size(); ++kk ) {
    int ll = size*kk;
    double cell   = data[ll];
    double ledDataRef = data[ll+1];
    double ledMoniRef = ( size>2) ? data[ll+2] : 1.;

    LHCb::CaloCellID id = LHCb::CaloCellID( (int) cell );
    id.setCalo( CaloCellCode::CaloNumFromName( name() ));
    //get cell
    if( m_cells[id].valid() ){
      m_cells[id].setLedDataRef( ledDataRef, ledMoniRef );
      if( UNLIKELY( msg.level() <= MSG::VERBOSE ) )
        msg << MSG::VERBOSE << "Added LED reference for channel " << id
            << " Reference (<PMT> , <PMT/PIN>) datafrom LED signal = (" << ledDataRef << "," << ledMoniRef << ")"<<endmsg;
      count++;
    }else{
      msg << MSG::WARNING << "Trying to add reference on non-valid channel : " << id << endmsg;
    }
  }
  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
    msg << MSG::DEBUG << "LED reference added for " << count << " channel(s) " << endmsg;
  return StatusCode::SUCCESS;
}


// Quality and current LED signal for relative calibration ------------------ //
StatusCode DeCalorimeter::getQuality( )  {
  // init
  MsgStream msg( msgSvc(), m_caloDet + ".Quality" );

  // check conditions
  if ( !hasCondition("Quality") )return StatusCode::SUCCESS; // quality not mandatory (set ::Unknown)


  // check array
  if ( !m_quality->exists( "data" ) || !m_quality->exists( "size") ) {
    if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
      msg << MSG::DEBUG << "No 'data' in 'Quality condition : will assume Flag = 0" << endmsg;
    return StatusCode::SUCCESS;
  }
  auto size = m_quality->param<int>( "size" );
  const auto& data = m_quality->param<std::vector<double>>( "data" );

  int count = 0;
  int bad = 0;
  int badLED=0;
  int masked =0;
  for ( unsigned int kk = 0; kk*size < data.size(); ++kk ) {
    int ll = size*kk;
    double cell       = data[ll];
    double qFlag      = data[ll+1];
    double ledData    = (size>2) ? data[ll+2] : 0.;
    double ledDataRMS = (size>3) ? data[ll+3] : 0.;
    double ledMoni    = (size>4) ? data[ll+4] : 1.;
    double ledMoniRMS = (size>5) ? data[ll+5] : 0.;
    LHCb::CaloCellID id = LHCb::CaloCellID( (int) cell );
    id.setCalo( CaloCellCode::CaloNumFromName( name() ));
    //get cell
    if( m_cells[id].valid() ){
      m_cells[id].addQualityFlag( (int) qFlag );
      m_cells[id].setLedData( ledData , ledDataRMS );
      m_cells[id].setLedMoni( ledMoni , ledMoniRMS );
      if( UNLIKELY( msg.level() <= MSG::VERBOSE ) ) {
        msg << MSG::VERBOSE << "Added quality for channel " << id << " : quality = " << qFlag << endmsg;
        msg << MSG::VERBOSE << "    current <PMT> +- RMS from LED signal : "
            << ledData << " +- " << ledDataRMS <<endmsg;
        msg << MSG::VERBOSE << "    current <PMT/PIN> +- RMS  from LED signal "
            << ledMoni << " +- " << ledMoniRMS <<endmsg;
      }
      if( CaloCellQuality::OK != (int) qFlag ){
        if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
          msg << MSG::DEBUG << "Quality " << id << " : " << m_cells[id].cellStatus() << endmsg;
        int iFlag = (int) qFlag;
        bool bLed = false;
        bool bRO = false;
        bool bMask = false;
        int  d = 1;
        while( iFlag > 0){
          if( (iFlag & 0x1) == 1){
            if(CaloCellQuality::qName(d).find("LED") != std::string::npos )
              bLed = true;
            else if( (CaloCellQuality::Flag) iFlag == CaloCellQuality::OfflineMask)
              bMask = true;
            else
              bRO = true;
          }
          iFlag /= 2;
          d += 1;
        }
        if( bLed )badLED++;
        if( bRO    )bad++;
        if( bMask )masked++;
      }
      count++;
    }else{
      msg << MSG::WARNING << "Trying to add quality on non-valid channel : " << id << endmsg;
    }
  }
  if( UNLIKELY( msg.level() <= MSG::DEBUG ) ){
    msg << MSG::DEBUG << "Quality constant added for " << count << " channel(s) " << endmsg;
    if(bad>0   )msg << MSG::DEBUG << "Found  " << bad<< " problematic readout channel(s)"<<endmsg;
    if(masked>0)msg << MSG::DEBUG << "Found  " << masked << " channel(s) to be masked offline " << endmsg;
    if(badLED>0)msg << MSG::DEBUG << "Found  " << badLED << " channel(s) with 'LED monitoring problem'" << endmsg;
  }
  return StatusCode::SUCCESS;
}


StatusCode DeCalorimeter::getL0Calibration( )  {
  // init
  MsgStream msg( msgSvc(), m_caloDet + ".L0calibration" );

  // check conditions
  if ( !hasCondition("L0Calibration") )return StatusCode::SUCCESS; // l0calibration not mandatory (assume dg = 1.)


  // check array
  if ( !m_l0calib->exists( "data" ) || !m_l0calib->exists( "size") ) {
    if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
      msg << MSG::DEBUG << "No 'data' in 'L0calibration' condition : will assume cte = 0" << endmsg;
    return StatusCode::SUCCESS;
  }
  auto size = m_l0calib->param<int>( "size" );
  const auto& data = m_l0calib->param<std::vector<int>>( "data" );

  int count = 0;
  for ( unsigned int kk = 0; kk*size < data.size(); ++kk ) {
    int ll = size*kk;
    int cell   = data[ll];
    int ct     = data[ll+1];

    LHCb::CaloCellID id = LHCb::CaloCellID( cell );
    //get cell
    if( m_cells[id].valid() ){
      m_cells[id].setL0Constant( ct );
      if( UNLIKELY( msg.level() <= MSG::VERBOSE ) )
        msg << MSG::VERBOSE << "Added l0calibration for channel " << id
            << " : L0 calibration constant  = " << ct << endmsg;
      count++;
    }else{
      msg << MSG::WARNING << "Trying to add l0calibration on non-valid channel : " << id << endmsg;
    }
  }
  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
    msg << MSG::DEBUG << "L0calibration constant added for " << count << " channel " << endmsg;
  return StatusCode::SUCCESS;
}



StatusCode DeCalorimeter::getNumericGains( )  {
  // init
  MsgStream msg( msgSvc(), m_caloDet + ".NumericGains" );

  // check conditions
  if ( !hasCondition("NumericGains") )return StatusCode::SUCCESS; // numericGains not mandatory


  // check array
  if ( !m_numericGains->exists( "data" ) || !m_numericGains->exists( "size") ) {
    if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
      msg << MSG::DEBUG << "No 'data' in 'NumericGains' condition : will assume cte = 0" << endmsg;
    return StatusCode::SUCCESS;
  }
  auto size = m_numericGains->param<int>( "size" );
  const auto& data = m_numericGains->param<std::vector<int>>( "data" );

  int count = 0;
  for ( unsigned int kk = 0; kk*size < data.size() ; ++kk ) {
    int ll   = size*kk;
    int cell = data[ll];
    int ng   = data[ll+1];

    LHCb::CaloCellID id = LHCb::CaloCellID( cell );
    id.setCalo( m_caloIndex );
    //get cell
    if( m_cells[id].valid() ){
      m_cells[id].setNumericGain( ng );
      if( UNLIKELY( msg.level() <= MSG::VERBOSE ) )
        msg << MSG::VERBOSE << "Added NumericGain for channel " << id
            << " :   " << ng << endmsg;
      ++count;
    }else{
      msg << MSG::WARNING << "Trying to NumericGain on non-valid channel : " << id << endmsg;
    }
  }
  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
    msg << MSG::DEBUG << "NumericGain added for " << count << " channel " << endmsg;
  return StatusCode::SUCCESS;
}


// ============================================================================
/// print to std::stream
// ============================================================================
std::ostream& DeCalorimeter::printOut( std::ostream& os ) const {

  os << "\tDeCalorimeter index=" << m_caloIndex
     << ", name from index ='"
     << CaloCellCode::CaloNameFromNum( m_caloIndex ) << "'"
     << ", fullname ='"   << name ()  << "'";
  os << "\t Parameters"
     << std::endl
     << "\t\tEt value for maximum ADC value at theta(0) =  ";
  for(unsigned int i=0 ; i<m_nArea;i++){
    os << "area= "<< i << " :: "<< m_maxEtInCenter[i] << " | " ;
  }
  os << std::endl
     << "\t\tIncrease in Et per radian                  =  ";
  for(unsigned int i=0 ; i<m_nArea;i++){
    os << " area= "<< i << " :: "<< m_maxEtSlope[i] << " | " ;
  }
  os << std::endl
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

    for (const auto& cell : m_cells ) {
      LHCb::CaloCellID id = cell.cellID();
      os << "Cell " << id << " Neighbors ";
      std::copy( neighborCells( id ).begin() ,
                 neighborCells( id ).end()   ,
                 OS_iterator<LHCb::CaloCellID,std::ostream>( os , "," ) );
      os << std::endl;
    }

  return os ;
}

// ============================================================================
/// print to MsgStream
// ============================================================================
MsgStream&    DeCalorimeter::printOut( MsgStream&    os ) const {
  os << "\tDeCalorimeter index="   << std::setw(2) << m_caloIndex
     << ", name from index='"
     << CaloCellCode::CaloNameFromNum( m_caloIndex ) << "'"
     << ", fullname ='"            << name ()  << "'";

  os << "\t Parameters"
     << endmsg
     << "\t\tEt value for maximum ADC value at theta(0) =  "
     << m_maxEtInCenter
     << endmsg
     << "\t\tIncrease in Et per radian                  =  "
     << ( m_maxEtSlope    )
     << endmsg
     << "\t\tMaximum codage in the ADC                  =  "
     << ( m_adcMax        )
     << endmsg
     << "\t\tConversion from activeE() to energy seen   =  "
     << ( m_activeToTotal )
     << endmsg
     << "\t\tZ of the shower maximum in the local frame =  "
     << ( m_zShowerMax    )
     << endmsg
     << "\t\tMaximum value for Row/Column               =  "
     << (  m_maxRowCol      )
     << endmsg
     << "\t\tFirst Row or Column  over center           =  "
     << ( m_firstRowUp      )
     << endmsg
     << "\t\tCentral Value = m_maxRowCol/2                =  "
     << ( m_centerRowCol    )
     << endmsg ;

    const MSG::Level lev = os.currentLevel();
    os.report( lev - 1 );
    ///
    for (const auto& cell : m_cells) {
      LHCb::CaloCellID id = cell.cellID();
      os << " Cell " << id << " Neighbors ";
      std::copy( neighborCells( id ).begin() ,
                 neighborCells( id ).end()   ,
                 OS_iterator<LHCb::CaloCellID,MsgStream>(os,",") );
      os << endmsg;
    }
    ///
    os.report( lev );

  return os ;
}



//
bool DeCalorimeter::loadCondition( SmartRef<Condition>& cond, std::string name , bool mandatory){
  MsgStream msg( msgSvc(), m_caloDet );
  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
    msg << MSG::DEBUG << "Loading condition '"<<name<<"'"<<endmsg;
  if ( hasCondition(name) ){
    cond = condition( name );
    return true;
  }
  if( msg.level() <= (mandatory ? MSG::ERROR : MSG::DEBUG) )
    msg << (mandatory ? MSG::ERROR : MSG::DEBUG) <<"'" << name << "'  condition not found" << endmsg;
  return false;
}




// Update cache
StatusCode DeCalorimeter::updGain(){
  MsgStream msg( msgSvc(), m_caloDet );
  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
    msg << MSG::DEBUG << "Updating condition 'Gain'" << endmsg;

  if( !hasCondition("Gain") )return StatusCode::FAILURE; // the gain condition must exist

  // check
  if( (!m_gain->exists("data") || !m_gain->exists("size") )
      && (!m_gain->exists("EtInCenter") || !m_gain->exists("EtSlope")) ){
    msg << MSG::ERROR << "No valid nominal gain setting is defined within 'Gain' condition" << endmsg;
    return StatusCode::FAILURE;
  }


  bool nominal = false;
  int count = 0;
  // Nominal Gain parameters
  if( m_gain->exists("data") && m_gain->exists("size")){
    if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
      msg << MSG::DEBUG << "Apply DB nominal gains" << endmsg;
    nominal=true;
    auto size = m_gain->param<int>( "size" );
    const auto& data = m_gain->param<std::vector<double>>( "data" );
    count = 0;
    for ( unsigned int kk = 0; kk*size < data.size() ; ++kk ) {
      int ll = size*kk;
      double cell   = data[ll];
      double gain   = data[ll+1];
      LHCb::CaloCellID id = LHCb::CaloCellID( (int) cell );
      id.setCalo( CaloCellCode::CaloNumFromName( name() ));
      if( m_cells[id].valid() ){
        m_cells[id].setNominalGain( gain );
        count++;
      }
    }
  }

  // Theoretical gain parameters
  if( m_gain->exists("EtInCenter") && m_gain->exists("EtSlope")){
    const auto& etSlope    = m_gain->param<std::vector<double>>( "EtSlope"      );
    const auto& etInCenter = m_gain->param<std::vector<double>>( "EtInCenter"      );

    if( etInCenter.size() != etSlope.size() ){
      msg << MSG::ERROR << "The gain parameters per region are not consistent" << endmsg;
      return StatusCode::FAILURE;
    }

    m_maxEtInCenter = etInCenter  ;
    m_maxEtSlope    = etSlope     ;
    if( (m_maxEtInCenter.size() != m_nArea && m_maxEtInCenter.size() !=1 ) ||
        (m_maxEtSlope.size() != m_nArea && m_maxEtSlope.size() != 1) ){
      msg << MSG::ERROR << " Found " << m_maxEtInCenter.size() << " gain parameters " << endmsg;
      msg << MSG::ERROR << " There should be " << m_nArea << " parameters (or a single one for all regions)"  << endmsg;
      return StatusCode::FAILURE;
    }
    // ** update gain/channel with theoretical value (if needed)
    if (!nominal){
      if(  m_caloDet == "EcalDet" ||  m_caloDet == "HcalDet" )
        msg << MSG::DEBUG << "Apply theoretical nominal gain as EtInCenter+sin(Theta)*EtSlope" << endmsg;
      count=0;
      for( auto& cell : m_cells) {
        LHCb::CaloCellID id       = cell.cellID();
        if( id.isPin() )continue;
        unsigned int Area   = id.area ( ) ;
        double gain = ( maxEtInCenter(Area) / cell.sine() ) + maxEtSlope(Area);
        gain        = gain / (double) adcMax() ;
        cell.setNominalGain( gain ) ;
        ++count;
      }
    }
  }

  std::string nom = (nominal) ? "Nominal" : "Theoretical";
  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
    msg << MSG::DEBUG << nom<< " gain constant added for " << count << " / " << numberOfCells() << " channel(s) " <<  endmsg;
  if( UNLIKELY( msg.level() <= MSG::ERROR ) )
    if( count != (int)numberOfCells() )
      msg << MSG::ERROR << nom << " gains are missing for " << (int)numberOfCells() - count << " channels !" << endmsg;

  // Pedestal shift
  m_pedShift      = m_gain->exists( "PedShift"     ) ? m_gain->paramAsDouble( "PedShift"      ) : 0. ;
  m_pinPedShift   = m_gain->exists( "PinPedShift"  ) ? m_gain->paramAsDouble( "PinPedShift"   ) : 0. ;
  m_l0Et          = m_gain->exists( "L0EtBin"      ) ? m_gain->paramAsDouble( "L0EtBin"       ) : 0. ;

  m_gainE    = m_gain->exists( "GainError")       ? m_gain->paramAsDouble("GainError")        : -1. ;
  m_stoch    = m_gain->exists( "StochasticTerm")  ? m_gain->paramAsDouble("StochasticTerm")   : -1. ;
  m_cNoise   = m_gain->exists( "CoherentNoise")   ? m_gain->paramAsDouble("CoherentNoise" )   : -1. ;
  m_iNoise   = m_gain->exists( "IncoherentNoise") ? m_gain->paramAsDouble("IncoherentNoise" ) : -1. ;

  m_zSupMeth = m_gain->exists( "ZeroSupMethod") ? m_gain->paramAsDouble( "ZeroSupMethod" ) : 1. ; // 1D 0-sup per default
  m_zSup     = m_gain->exists( "ZeroSup") ? m_gain->paramAsDouble( "ZeroSup" ) : 0.5 ; // zero suppression
  // Needed in simulation only
  m_mip      = m_gain->exists( "MipDeposit") ? m_gain->paramAsDouble( "MipDeposit" ) : 0. ;
  m_dyn      = m_gain->exists( "DynamicsSaturation") ? m_gain->paramAsDouble( "DynamicsSaturation" ) : 1. ;
  m_prev     = m_gain->exists( "PreviousFraction") ? m_gain->paramAsDouble( "PreviousFraction") : 0. ;
  m_l0Thresh = m_gain->exists( "L0Threshold"  ) ? m_gain->paramAsDouble( "L0Threshold"   ) : 0.5 ;
  if( m_gain->exists( "phePerMip") ) m_phe      =    m_gain->param<std::vector<double>>( "phePerMip" ) ;
  if( m_gain->exists( "L0Correction") ) m_l0Cor =    m_gain->param<std::vector<double>>( "L0Correction" ) ;
  m_activeToTotal = m_gain->exists( "ActiveToTotal") ? m_gain->paramAsDouble( "ActiveToTotal" ) : 1. ;

  // special setting for Spd :
  if( m_caloDet == "SpdDet"   )m_l0Thresh = m_zSup; // Spd is L0 only

  return StatusCode::SUCCESS;
}

StatusCode DeCalorimeter::updReco(){
  MsgStream msg( msgSvc(), m_caloDet );
  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
    msg << MSG::DEBUG << "Updating condition 'Reco'" << endmsg;

  if( !hasCondition("Reco") )return StatusCode::FAILURE; // the reco condition MUST exist

  m_zShowerMax = m_reco->paramAsDouble("ZShowerMax" ) ;
  for( auto& cell : m_cells) cell.setZshower( m_zShowerMax );
  return StatusCode::SUCCESS;
}

StatusCode DeCalorimeter::updHardware(){
  MsgStream msg( msgSvc(), m_caloDet );
  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
    msg << MSG::DEBUG << "Updating condition 'Hardware'" << endmsg;
  if( !hasCondition("Hardware") )return StatusCode::FAILURE;  // the hardware condition MUST exist
  m_adcMax = m_hardware->param<int>( "AdcMax"    );
  m_coding = m_hardware->param<int>( "CodingBit" );
  m_maxRowCol     = (1<< m_coding ) - 1;         // 63   (31   for HCAL)
  m_firstRowUp    = m_maxRowCol / 2 + 1 ;        // 32   (16   for HCAL)
  m_centerRowCol  = .5 * (double) m_maxRowCol ;  // 31.5 (15.5 for HCAL)
  m_centralHoleX = m_hardware->param<int>("centralHoleX");
  m_centralHoleY = m_hardware->param<int>("centralHoleY");
  return StatusCode::SUCCESS;
}

StatusCode DeCalorimeter::updReadout(){
  MsgStream msg( msgSvc(), m_caloDet );
  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
    msg << MSG::DEBUG << "Updating condition 'Readout'" << endmsg;
  if( !hasCondition("Readout") )return StatusCode::SUCCESS;
  // the readout condition is not mandatory (e.g Spd)
  StatusCode sc = buildCards();
  if(sc.isSuccess())sc=buildTell1s();
  return sc;
}

StatusCode DeCalorimeter::updMonitor(){
  MsgStream msg( msgSvc(), m_caloDet );
  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
    msg << MSG::DEBUG << "Updating condition 'Monitoring'" << endmsg;
  if( !hasCondition("Monitoring") )return StatusCode::SUCCESS;
      //  the monitoring condition is not mandatory (e.g Spd/Prs)
  resetCellParam(msg,"Monitoring",[](CellParam& ic) {
    LHCb::CaloCellID id = ic.cellID() ;
    if( !id.isPin())return;
      ic.setValid(false);
      ic.resetPins();
      ic.resetLeds();
  });
  return buildMonitoring();
}

StatusCode DeCalorimeter::updGeometry(){
  MsgStream msg( msgSvc(), m_caloDet );
  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
    msg << MSG::DEBUG << "Updating condition 'IGeometry'" << endmsg;
  if(!geometry() )return StatusCode::FAILURE;
  resetCellParam(msg,"Geometry", [](CellParam& ic) {
      if( !ic.cellID().isPin() ) {
        ic.setValid(false);
        ic.setCenterSize( Gaudi::XYZPoint() , 0.);
      }
  });
  auto sc = buildCells();
  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
    msg << MSG::DEBUG << "Number of invalid cells = " << numberOfInvalidCells() << endmsg;
  return sc;
}

StatusCode DeCalorimeter::updCalib(){
  MsgStream msg( msgSvc(), m_caloDet );
  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
    msg << MSG::DEBUG << "Updating condition 'Calibration'" << endmsg;
  if( !hasCondition("Calibration") )return StatusCode::SUCCESS; // the calibration condition is NOT mandatory
  resetCellParam(msg,"Calibration",[](CellParam& ic) {
      ic.setCalibration( 1.0 ); // same default as in CellParam.cpp
  });
  return getCalibration();
}

StatusCode DeCalorimeter::updPileUp(){
  MsgStream msg( msgSvc(), m_caloDet );
  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
    msg << MSG::DEBUG << "Updating condition 'PileUpOffset'" << endmsg;
  if( !hasCondition("PileUpOffset") )return StatusCode::SUCCESS; // the pileUpOffset condition is NOT mandatory
  resetCellParam(msg,"PileUpOffset",[](CellParam& ic) {
      ic.setPileUpOffset( 0.0 , 0.0); // same default as in CellParam.cpp
  });
  return getPileUpOffset();
}

StatusCode DeCalorimeter::updLEDReference(){
  MsgStream msg( msgSvc(), m_caloDet );
  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
    msg << MSG::DEBUG << "Updating condition 'LEDReference'" << endmsg;
  if( !hasCondition("LEDReference") )return StatusCode::SUCCESS; // the LEDReference condition is NOT mandatory
  resetCellParam(msg,"LEDReference",[](CellParam& ic) {
      ic.setLedDataRef( -1.0, 0.0); // same default as in CellParam.cpp
  });
  return getLEDReference();
}

StatusCode DeCalorimeter::updQuality(){
  MsgStream msg( msgSvc(), m_caloDet );
  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
    msg << MSG::DEBUG << "Updating condition 'Quality'" << endmsg;
  if( !hasCondition("Quality") )return StatusCode::SUCCESS; // the quality condition is NOT mandatory
  resetCellParam(msg,"Quality",[](CellParam& ic) {
      ic.setLedData( -1.0, -1.0);
      ic.setLedMoni( -1.0, 0.0);
      ic.setQualityFlag(CaloCellQuality::OK); // same default as in CellParam.cpp
  });
  return getQuality();
}


StatusCode DeCalorimeter::updL0Calib(){
  MsgStream msg( msgSvc(), m_caloDet );
  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
    msg << MSG::DEBUG << "Updating condition 'L0Calibration'" << endmsg;
  if( !hasCondition("L0Calibration") )return StatusCode::SUCCESS; // the L0Calibration condition is NOT mandatory
  resetCellParam(msg,"L0Calibration",[](CellParam& ic) {
      ic.setL0Constant( 0.0 ); // same default as in CellParam.cpp
  });
  return getL0Calibration();
}

StatusCode DeCalorimeter::updNumGains(){
  MsgStream msg( msgSvc(), m_caloDet );
  if( UNLIKELY( msg.level() <= MSG::DEBUG ) )
    msg << MSG::DEBUG << "Updating condition 'NumericGain'" << endmsg;
  if( !hasCondition("NumericGains") )return StatusCode::SUCCESS; // the L0Calibration condition is NOT mandatory
  resetCellParam(msg,"NumericGains",[](CellParam& ic) {
      ic.setNumericGain( 0.0 ); // same default as in CellParam.cpp
  });
  return getNumericGains();
}
