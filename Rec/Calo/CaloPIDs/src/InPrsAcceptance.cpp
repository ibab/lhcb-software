// ============================================================================
// Include files 
// ============================================================================
// Local
// ============================================================================
#include "InCaloAcceptance.h"
// ============================================================================
/** @class InPrsAcceptance 
 *  The precofigured instance of InCaloAcceptance Tool
 *  @author Victor EGORYCHEV Victor.Egorychev@cern.ch
 *  @author Vanya  BELYAEV    ibelyaev@phsycis.syr.edu
 *  @date   2006-05-28
 */
// ============================================================================
class InPrsAcceptance : public InCaloAcceptance 
{  
  /// friend factory for instantiation 
  friend class ToolFactory<InPrsAcceptance> ; 
protected:
  /// standard protected constructor 
  InPrsAcceptance
  ( const std::string& type   ,
    const std::string& name   , 
    const IInterface*  parent )
    : InCaloAcceptance( type, name , parent )
  {
    _setProperty ( "Calorimeter" , DeCalorimeterLocation::Prs ) ;
    _setProperty ( "UseFiducial" , "false"  ) ;
    _setProperty ( "Tolerance"   , "10" ) ;  /// 10 * Gaudi::Units::mm 
  } ;
  /// virtual and protected destructor 
  virtual ~InPrsAcceptance(){} ;
private:
  InPrsAcceptance() ;
  InPrsAcceptance            ( const InPrsAcceptance& ) ;
  InPrsAcceptance& operator= ( const InPrsAcceptance& ) ;  
} ;
// ============================================================================
/// Declaration of the Tool Factory
// ============================================================================
DECLARE_TOOL_FACTORY( InPrsAcceptance )
// ============================================================================

