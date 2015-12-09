// $Id: DeSubSubCalorimeter.cpp,v 1.3 2009-04-17 13:41:04 cattanem Exp $ 
// ============================================================================
#define CALODET_DESUBSUBCALORIMETER_CPP 1
// ============================================================================

// CaloDet
#include "CaloDet/DeSubSubCalorimeter.h"
#include "CaloKernel/CaloException.h"

#include <algorithm> // for std::find

/** @file DeSubSubCalorimeter.cpp
 *
 *  Implementation of class :  DeSubSubCalorimeter
 *  @author Olivier Callot Olivier.Callot@cern.ch
 *  @author Vanya Belyaev  Ivan.Belyaev@itep.ru
 */

// ============================================================================
// Standard Constructors
// ============================================================================
DeSubSubCalorimeter::DeSubSubCalorimeter( const std::string& name )
  : DetectorElement     ( name )
    , m_cellSize( 0.0 )
    , m_xSize(0.0)
    , m_ySize(0.0)
    , m_area( -1  )
{}

// ============================================================================
// Destructor
// ============================================================================
DeSubSubCalorimeter::~DeSubSubCalorimeter(){}


// ============================================================================
/// object identification
// ============================================================================
const CLID& DeSubSubCalorimeter::clID() const
{ return DeSubSubCalorimeter::classID(); }

// ============================================================================
// standard initialization
// ============================================================================
StatusCode DeSubSubCalorimeter::initialize(){
  /// initialize the base class
  StatusCode sc = DetectorElement::initialize();
  if ( sc.isFailure() ) { return sc ; }

  typedef std::vector<std::string> Parameters;
  typedef Parameters::iterator     Iterator;
  Parameters pars( paramNames() );
  /// cell size
  auto it = std::find( pars.begin() , pars.end () , std::string("CellSize") );
  if( pars.end() != it ){
    const double value = param<double>( *it );
    setCellSize( value ) ;
    pars.erase( it );
  }else{ 
      return StatusCode::FAILURE ; 
  }
  /// subSubCalo X size
  auto itx = std::find( pars.begin() , pars.end () , std::string("XSize") );
  if( pars.end() != itx ){
    const double value = param<double>( *itx );
    setXSize( value ) ;
    pars.erase( itx );
  }else{ 
    return StatusCode::FAILURE ; 
  }
  /// subSubCalo Y size
  auto ity = std::find( pars.begin() , pars.end () , std::string("YSize") );
  if( pars.end() != ity ){
    const double value = param<double>( *ity );
    setYSize( value ) ;
    pars.erase( ity );
  }else{ 
    return StatusCode::FAILURE ; 
  }
  /// area Id 
  auto itt = std::find( pars.begin() , pars.end () , std::string("Area") );
  if( pars.end() != itt ){
    const int value = param<int>( *itt );
    setArea( value ) ;
    pars.erase( itt );
  }else{ 
      return StatusCode::FAILURE ; 
  }
  Assert ( nullptr != geometry() , "DeSubSubcalorimeter: Invalid GeometryInfo" ) ;
  return StatusCode::SUCCESS;
}

// ============================================================================
// standard printout to STL standard stream
// ============================================================================
std::ostream& DeSubSubCalorimeter::printOut( std::ostream& os ) const{
  os << "\tDeSubSubCalorimeter" << " fullname ='"    << name () << std::endl;
  os << "\t\t\tCellSize=" << std::setw(12) << m_cellSize << " Area-ID = " << m_area  
     << " X-center offset : " << xSize()/2. << std::endl;
  return os;
}

// ============================================================================
// standard printout to Gaudi standard stream
// ============================================================================
MsgStream&    DeSubSubCalorimeter::printOut( MsgStream&    os ) const{
  os << "\tDeSubSubCalorimeter"  << " fullname ='"    << name ()     << endmsg   ;
  os << "\t\t\tCellSize=" << std::setw(12) << m_cellSize << " Area-ID = " << m_area  
     << " X-center offset : " << xSize()/2 << endmsg;
  return os;
}

