// $Id: InPrsAcceptance.cpp,v 1.1 2006-06-18 18:35:29 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
// ============================================================================
// Local
// ============================================================================
#include "InCaloAcceptance.h"
// ============================================================================
/** @class In PrsAcceptance 
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
    setProperty ( "Calorimeter" , DeCalorimeterLocation::Prs ) ;
    setProperty ( "UseFiducial" , "false"  ) ;
    setProperty ( "Tolerance"   , "10" ) ;  /// 10 * Gaudi::Units::mm 
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
DECLARE_TOOL_FACTORY( InPrsAcceptance ) ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
