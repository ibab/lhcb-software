// $Id: CaloExtraDigits.cpp,v 1.12 2008-09-22 01:41:23 odescham Exp $
// ============================================================================
// Include files
// STL 
#include <cmath>
#include <algorithm>
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartRef.h"
// local
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

DECLARE_TOOL_FACTORY( CaloExtraDigits );

// ============================================================================
CaloExtraDigits::CaloExtraDigits( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool            ( type, name , parent ) 
{
  declareInterface<ICaloHypoTool>     (this);
  declareProperty ( "ExtraDigitFrom"      , m_toDet ) ;
  m_toDet.push_back("Prs");
  m_toDet.push_back("Spd");
};

// ============================================================================
/// Destructor (virtual and protected)
// ============================================================================
CaloExtraDigits::~CaloExtraDigits(){};
// ============================================================================


// ============================================================================
StatusCode CaloExtraDigits::initialize () 
{
  // initilaize the base class 
  StatusCode sc = GaudiTool::initialize ();
  if( sc.isFailure() ) 
  { return Error( "Could not initialize the base class GaudiTool!", sc ) ; }  

  for(std::vector<std::string>::iterator idet = m_toDet.begin(); idet!=m_toDet.end();idet++){
    m_toCalo[*idet]=tool<ICaloHypo2Calo>("CaloHypo2Calo", "CaloHypo2" + *idet );
  }
  return StatusCode::SUCCESS ;
};


// ============================================================================
StatusCode CaloExtraDigits::process    ( LHCb::CaloHypo* hypo  ) const{ 
return (*this) ( hypo ); 
};

// ============================================================================
StatusCode CaloExtraDigits::operator() ( LHCb::CaloHypo* hypo  ) const{  
  if( 0 == hypo        ) { return Error("CaloHypo* points to NULL!" , 200 ) ; }

  for(std::vector<std::string>::const_iterator idet = m_toDet.begin(); idet!=m_toDet.end();idet++){
    int count = 0;
    std::map<std::string,ICaloHypo2Calo*>::const_iterator itoCalo = m_toCalo.find(*idet);
    if(itoCalo == m_toCalo.end())continue;
    std::vector<LHCb::CaloDigit*> digits = ((*itoCalo).second)->digits( *hypo, *idet);
    for(std::vector<LHCb::CaloDigit*>::iterator id = digits.begin() ; id != digits.end(); id++){
      hypo->addToDigits( *id );
      count++;
    }  
    if ( msgLevel( MSG::DEBUG) )debug() << "Adding " << count << " digits from "<< *idet << endreq;
  }
  return StatusCode::SUCCESS ;  
};
// ============================================================================

