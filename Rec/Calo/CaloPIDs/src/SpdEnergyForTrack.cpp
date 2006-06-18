// $Id: SpdEnergyForTrack.cpp,v 1.1 2006-06-18 18:35:29 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// local
// ============================================================================
#include "CaloEnergyForTrack.h"
// ============================================================================
/** @class SpdEnergyForTrack
 *  The concrete preconfigured insatnce for CaloEnergyForTrack tool
 *  along the track line 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
class SpdEnergyForTrack : public CaloEnergyForTrack 
{
  // friend factory for istantiation
  friend class ToolFactory<SpdEnergyForTrack>;
protected:
  SpdEnergyForTrack
  ( const std::string& type   , // ? 
    const std::string& name   , 
    const IInterface*  parent ) 
    : CaloEnergyForTrack ( type , name , parent ) 
  {
    setProperty ( "DataAddress" , LHCb::CaloDigitLocation::Spd ) ;
    setProperty ( "Tolerance"   , "2"    ) ; /// 2 * Gaudi::Units::mm 
    setProperty ( "Calorimeter" , DeCalorimeterLocation::Spd   ) ;
  } ;
  virtual ~SpdEnergyForTrack() {}
private:
  // default constructor is disabled 
  SpdEnergyForTrack() ;
  // copy constructor is disabled 
  SpdEnergyForTrack           ( const  SpdEnergyForTrack& ) ;
  // assignement is disabled 
  SpdEnergyForTrack& operator=( const SpdEnergyForTrack& ) ;
} ;
// ============================================================================
/// Factory
// ============================================================================
DECLARE_TOOL_FACTORY(SpdEnergyForTrack) ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
