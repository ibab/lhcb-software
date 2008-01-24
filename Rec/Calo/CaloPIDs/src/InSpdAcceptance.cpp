// $Id: InSpdAcceptance.cpp,v 1.4 2008-01-24 10:22:42 vegorych Exp $
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
/** @class InSpdAcceptance 
 *  The precofigured instance of InCaloAcceptance Tool
 *  @author Victor EGORYCHEV Victor.Egorychev@cern.ch
 *  @author Vanya  BELYAEV    ibelyaev@phsycis.syr.edu
 *  @date   2006-05-28
 */
// ============================================================================
class InSpdAcceptance : public InCaloAcceptance 
{  
  /// friend factory for instantiation 
  friend class ToolFactory<InSpdAcceptance> ; 
protected:
  /// standard protected constructor 
  InSpdAcceptance
  ( const std::string& type   ,
    const std::string& name   , 
    const IInterface*  parent )
    : InCaloAcceptance( type, name , parent )
  {
    setProperty ( "Calorimeter" , DeCalorimeterLocation::Spd ) ;
    setProperty ( "UseFiducial" , false  ) ;
    setProperty ( "Tolerance"   , 1 ) ;  /// 1 * Gaudi::Units::mm 
  } ;
  /// virtual and protected destructor 
  virtual ~InSpdAcceptance(){} ;
private:
  InSpdAcceptance() ;
  InSpdAcceptance            ( const InSpdAcceptance& ) ;
  InSpdAcceptance& operator= ( const InSpdAcceptance& ) ;  
} ;
// ============================================================================
/// Declaration of the Tool Factory
// ============================================================================
DECLARE_TOOL_FACTORY( InSpdAcceptance ) ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
