// $Id: HcalEnergyForTrack.cpp,v 1.1 2006-06-18 18:35:28 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// local
// ============================================================================
#include "CaloEnergyForTrack.h"
// ============================================================================
/** @class HcalEnergyForTrack
 *  The concrete preconfigured insatnce for CaloEnergyForTrack tool
 *  along the track line 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
class HcalEnergyForTrack : public CaloEnergyForTrack 
{
  // friend factory for istantiation
  friend class ToolFactory<HcalEnergyForTrack>;
protected:
  HcalEnergyForTrack
  ( const std::string& type   , // ? 
    const std::string& name   , 
    const IInterface*  parent ) 
    : CaloEnergyForTrack ( type , name , parent ) 
  {
    setProperty ( "DataAddress"   , LHCb::CaloDigitLocation::Hcal ) ;
    setProperty ( "MorePlanes"    , "5"    ) ; ///  3 additional planes 
    setProperty ( "AddNeigbours"  , "true" ) ; 
    setProperty ( "Tolerance"     , "15"   ) ; /// 15 * Gaudi::Units::mm 
    setProperty ( "Calorimeter"   , DeCalorimeterLocation::Hcal   ) ;
  } ;
  virtual ~HcalEnergyForTrack() {}
private:
  // default constructor is disabled 
  HcalEnergyForTrack() ;
  // copy constructor is disabled 
  HcalEnergyForTrack           ( const  HcalEnergyForTrack& ) ;
  // assignement is disabled 
  HcalEnergyForTrack& operator=( const HcalEnergyForTrack& ) ;
} ;
// ============================================================================
/// Factory
// ============================================================================
DECLARE_TOOL_FACTORY(HcalEnergyForTrack) ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
