// $Id: HcalEnergyForTrack.cpp,v 1.2 2007-08-24 21:25:18 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2006/06/18 18:35:28  ibelyaev
//  the firstcommmit for DC06 branch
//
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
    _setProperty ( "DataAddress"   , LHCb::CaloDigitLocation::Hcal ) ;
    _setProperty ( "MorePlanes"    , "5"    ) ; ///  3 additional planes 
    _setProperty ( "AddNeigbours"  , "true" ) ; 
    _setProperty ( "Tolerance"     , "15"   ) ; /// 15 * Gaudi::Units::mm 
    _setProperty ( "Calorimeter"   , DeCalorimeterLocation::Hcal   ) ;
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
