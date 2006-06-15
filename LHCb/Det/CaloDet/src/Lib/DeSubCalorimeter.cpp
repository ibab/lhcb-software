// $Id: DeSubCalorimeter.cpp,v 1.12 2006-06-15 14:39:46 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#define CALODET_DESUBCALORIMETER_CPP 1
// ============================================================================

// CaloDet
#include "CaloDet/DeSubCalorimeter.h"
#include "CaloKernel/CaloException.h"

/** @file DeSubCalorimeter.cpp
 *
 *  Implementation of class :  DeSubCalorimeter
 *  @author Olivier Callot Olivier.Callot@cern.ch
 *  @author Vanya Belyaev  Ivan.Belyaev@itep.ru
 */

// ============================================================================
// Standard Constructors
// ============================================================================
DeSubCalorimeter::DeSubCalorimeter( const std::string& name )
  : DetectorElement     ( name )
  , m_size( 0.0 )
{};

// ============================================================================
// Destructor
// ============================================================================
DeSubCalorimeter::~DeSubCalorimeter(){};


// ============================================================================
/// object identification
// ============================================================================
const CLID& DeSubCalorimeter::clID() const
{ return DeSubCalorimeter::classID(); }

// ============================================================================
// standard initialization
// ============================================================================
StatusCode DeSubCalorimeter::initialize()
{
  /// initialize the base class
  StatusCode sc = DetectorElement::initialize();
  if ( sc.isFailure() ) { return sc ; }
  ///
  typedef std::vector<std::string> Parameters;
  typedef Parameters::iterator     Iterator;
  ///
  Parameters pars( paramNames() );
  ///
  { /// cell size
    Iterator it =
      std::find( pars.begin() , pars.end () , std::string("CellSize") );
    if( pars.end() != it )
      {
		const double value = param<double>( *it );
        setSize( value ) ;
        pars.erase( it );
      }
    else
      { return StatusCode::FAILURE ; }
  }
  if( !pars.empty() ) {
    // some "extra" parameters.
    // should be an error??
  }
  //
  Assert ( 0 != geometry() , "DeSubcalorimeter: Invalid GeometryInfo" ) ;
  ///
  return StatusCode::SUCCESS;
  ///
};

// ============================================================================
// standard printout to STL standard stream
// ============================================================================
std::ostream& DeSubCalorimeter::printOut( std::ostream& os ) const
{
  os << "\tDeSubCalorimeter"
     << " fullname ='"    << name ()
     << std::endl;
  os << "\t\t\tCellSize=" << std::setw(12) << m_size
     << std::endl;
  return os;
};

// ============================================================================
// standard printout to Gaudi standard stream
// ============================================================================
MsgStream&    DeSubCalorimeter::printOut( MsgStream&    os ) const
{
  os << "\tDeSubCalorimeter"
     << " fullname ='"    << name ()
     << endreq   ;
  os << "\t\t\tCellSize=" << std::setw(12) << m_size
     << endreq   ;
  return os;
};

// ============================================================================
