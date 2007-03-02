// $Id: InEcalAcceptance.cpp,v 1.2 2007-03-02 15:22:08 cattanem Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
// ============================================================================
// Local
// ============================================================================
#include "InCaloAcceptance.h"
// ============================================================================
/** @class InEcalAcceptance 
 *  The precofigured instance of InCaloAcceptance Tool
 *  @author Victor EGORYCHEV Victor.Egorychev@cern.ch
 *  @author Vanya  BELYAEV    ibelyaev@phsycis.syr.edu
 *  @date   2006-05-28
 */
// ============================================================================
class InEcalAcceptance : public InCaloAcceptance 
{  
  /// friend factory for instantiation 
  friend class ToolFactory<InEcalAcceptance> ; 
protected:
  /// standard protected constructor 
  InEcalAcceptance
  ( const std::string& type   ,
    const std::string& name   , 
    const IInterface*  parent )
    : InCaloAcceptance( type, name , parent )
  {
    setProperty ( "Calorimeter" , DeCalorimeterLocation::Ecal ) ;
    setProperty ( "UseFiducial" , "true"  ) ;
    setProperty ( "Tolerance"   , "5" ) ;  /// 5 * Gaudi::Units::mm 
  } ;
  /// virtual and protected destructor 
  virtual ~InEcalAcceptance(){} ;
private:
  InEcalAcceptance() ;
  InEcalAcceptance            ( const InEcalAcceptance& ) ;
  InEcalAcceptance& operator= ( const InEcalAcceptance& ) ;  
} ;
// ============================================================================
/// Declaration of the Tool Factory
// ============================================================================
DECLARE_TOOL_FACTORY( InEcalAcceptance ) ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
