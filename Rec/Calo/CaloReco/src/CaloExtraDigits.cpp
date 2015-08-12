// ============================================================================
// Include files
// ============================================================================
// STL 
// ============================================================================
#include <cmath>
#include <algorithm>
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/SmartRef.h"
// ============================================================================
// local
// ============================================================================
#include "CaloExtraDigits.h"
// ============================================================================
/** @file CaloExtraDigits.cpp
 * 
 *  Implementation file for class : CaloExtraDigits
 *  @see CaloExtraDigits 
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 01/04/2002 
 */
// ============================================================================
DECLARE_TOOL_FACTORY( CaloExtraDigits )
// ============================================================================
CaloExtraDigits::CaloExtraDigits
( const std::string&  type   ,
  const std::string&  name   ,
  const IInterface*   parent )
  : GaudiTool            ( type, name , parent ) 
{
  //
  declareInterface<ICaloHypoTool>     (this);
  //
  if ( std::string::npos != name.find ( "Prs"  ) )
  { m_toDet.push_back ( "Prs"  ) ; }
  if ( std::string::npos != name.find ( "Spd"  ) ) 
  { m_toDet.push_back ( "Spd"  ) ; }
  if ( std::string::npos != name.find ( "Hcal" ) ) 
  { m_toDet.push_back ( "Hcal" ) ; }
  if ( std::string::npos != name.find ( "Ecal" ) ) 
  { m_toDet.push_back ( "Ecal" ) ; }  
  //
  declareProperty ( "ExtraDigitFrom"    , m_toDet ) ;
  //
  declareProperty ("Detector"           , m_det = "Ecal" );
}
// ============================================================================
// Destructor (virtual and protected)
// ============================================================================
CaloExtraDigits::~CaloExtraDigits(){}
// ============================================================================
StatusCode CaloExtraDigits::initialize () 
{
  // initilaize the base class 
  StatusCode sc = GaudiTool::initialize ();
  if( sc.isFailure() ) 
  { return Error ( "Could not initialize the base class GaudiTool!", sc ) ; }  
  //
  if ( m_toDet.empty() ) 
  { return Error ( "List of 'ExtraDigitsFrom' is empty!" ) ; }  
  //
  for ( std::vector<std::string>::iterator idet = m_toDet.begin(); 
        idet!=m_toDet.end();idet++)
  {
    m_toCalo[*idet]=tool<ICaloHypo2Calo>("CaloHypo2Calo", "CaloHypo2" + *idet , this );
    m_toCalo[*idet]->setCalos(m_det,*idet);
  }
  // 
  return StatusCode::SUCCESS ;
}
// ============================================================================
StatusCode CaloExtraDigits::process    ( LHCb::CaloHypo* hypo  ) const
{ return (*this) ( hypo ); }
// ============================================================================
StatusCode CaloExtraDigits::operator() ( LHCb::CaloHypo* hypo  ) const
{  
  if ( 0 == hypo        ) { return Error("CaloHypo* points to NULL!" , 200 ) ; }
  //
  for ( std::map<std::string,ICaloHypo2Calo*>::const_iterator 
          idet = m_toCalo.begin(); idet!=m_toCalo.end();idet++)
  {
    ICaloHypo2Calo*    tool   = idet->second ;
    const std::string& toCalo = idet->first  ;
    unsigned int count = 0 ;
    const std::vector<LHCb::CaloDigit*>& digits = tool->digits ( *hypo, toCalo );
    for ( std::vector<LHCb::CaloDigit*>::const_iterator id = digits.begin() ; 
          id != digits.end(); id++)
    {
      hypo->addToDigits( *id );
      ++count;
    }
    //
    if (UNLIKELY( msgLevel(MSG::DEBUG) ) )debug() << "Adding " << count << " digits from "<< *idet << endmsg; 
    //
    counter ( toCalo ) += count ;
  }
  return StatusCode::SUCCESS ;  
}
