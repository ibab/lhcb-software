// $Id: EcalEnergyForTrack.cpp,v 1.7 2009-10-31 01:09:46 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.7 $
// ============================================================================
// local
// ============================================================================
#include "CaloEnergyForTrack.h"
// ============================================================================
/** @class EcalEnergyForTrack
 *  The concrete preconfigured insatnce for CaloEnergyForTrack tool
 *  along the track line 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
class EcalEnergyForTrack : public CaloEnergyForTrack 
{
  // ==========================================================================
  // friend factory for istantiation
  friend struct ToolFactory<EcalEnergyForTrack>;
  // ==========================================================================
protected:
  // ==========================================================================
  EcalEnergyForTrack
  ( const std::string& type   , // ? 
    const std::string& name   , 
    const IInterface*  parent ) 
    : CaloEnergyForTrack ( type , name , parent ) 
  {
    setProperty ( "DataAddress"   , LHCb::CaloDigitLocation::Ecal ).ignore() ;
    setProperty ( "MorePlanes"    , 3    ).ignore() ; /// 3 additional planes 
    setProperty ( "AddNeigbours"  , 0    ).ignore() ; 
    setProperty ( "Tolerance"     , 5 * Gaudi::Units::mm  ).ignore() ;
    setProperty ( "Calorimeter"   , DeCalorimeterLocation::Ecal   ).ignore() ;
  } 
  virtual ~EcalEnergyForTrack() {}
  // ==========================================================================
private:
  // ==========================================================================
  // default constructor is disabled 
  EcalEnergyForTrack() ;
  // copy constructor is disabled 
  EcalEnergyForTrack           ( const  EcalEnergyForTrack& ) ;
  // assignement is disabled 
  EcalEnergyForTrack& operator=( const EcalEnergyForTrack& ) ;
  // ==========================================================================
} ;
// ============================================================================
// Factory
// ============================================================================
DECLARE_TOOL_FACTORY(EcalEnergyForTrack) 
// ============================================================================
// The END 
// ============================================================================
