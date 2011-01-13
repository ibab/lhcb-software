// ============================================================================
#define CALODET_DESUBCALORIMETER_CPP 1
// ============================================================================

// CaloDet
#include "CaloDet/DeSubCalorimeter.h"
#include "CaloDet/DeSubSubCalorimeter.h"
#include "CaloKernel/CaloException.h"

/** @file DeSubCalorimeter.cpp
 *
 *  Implementation of class :  DeSubCalorimeter
 *  @author Olivier Deschamos odescham@in2p3.fr
 */

// ============================================================================
// Standard Constructors
// ============================================================================
DeSubCalorimeter::DeSubCalorimeter( const std::string& name )
  : DetectorElement     ( name )
  , m_side(0)
  , m_subSubCalos          () 
{}

// ============================================================================
// Destructor
// ============================================================================
DeSubCalorimeter::~DeSubCalorimeter(){}


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


  int index =  CaloCellCode::CaloNumFromName( name() );
  std::string myName = CaloCellCode::CaloNameFromNum( index ) + "SubDet";
  MsgStream msg( msgSvc(), myName );


  typedef std::vector<std::string> Parameters;
  typedef Parameters::iterator     Iterator;
  Parameters pars( paramNames() );
  /// calo half ID 
  Iterator it = std::find( pars.begin() , pars.end () , std::string("CaloSide") );
  if( pars.end() != it ){
    const int value = param<int>( *it );
    setSide( value ) ;
    pars.erase( it );
  }else{ 
    return StatusCode::FAILURE ; 
  } 
  
  {
    // collect the sub-sub-calorimeters (calo areas)
    IDetectorElement::IDEContainer& subdets = childIDetectorElements();
    msg << MSG::DEBUG << name() << " :  number of subSubCalorimeter " << subdets.size() << endmsg;
    for(IDetectorElement::IDEContainer::iterator ichild = subdets.begin() ; ichild != subdets.end() ; ++ichild){
      IDetectorElement* child = *ichild ;
      if ( 0 == child ) { continue ; }
      DeSubSubCalorimeter* subSub = dynamic_cast<DeSubSubCalorimeter*> ( child ) ;
      Assert ( 0 != subSub , "no DeSubSubCalorimeter!" ) ;
      m_subSubCalos.push_back ( subSub ) ;
    }
    Assert ( !m_subSubCalos.empty() , "Empty subcalorimeters!" ) ;
  }

//
  Assert ( 0 != geometry() , "DeSubcalorimeter: Invalid GeometryInfo" ) ;
///
return StatusCode::SUCCESS;
///
}

// ============================================================================
// standard printout to STL standard stream
// ============================================================================
std::ostream& DeSubCalorimeter::printOut( std::ostream& os ) const
{
  os << "\tDeSubCalorimeter" << " fullname ='"    << name () << std::endl;
  os << "\t\t\tSide=" << sideName() << " #areas " << subSubCalos().size() << std::endl   ;
  return os;
}

// ============================================================================
// standard printout to Gaudi standard stream
// ============================================================================
MsgStream&    DeSubCalorimeter::printOut( MsgStream&    os ) const
{
  os << "\tDeSubCalorimeter"<< " fullname ='"    << name () << endmsg;
  os << "\t\t\tSide=" << sideName() << " #areas " << subSubCalos().size() << endmsg   ;
  return os;
}

// ============================================================================
