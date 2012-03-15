// ============================================================================
// local
// ============================================================================
#include "CaloEnergyForTrack.h"
// ============================================================================
/** @class PrsEnergyForTrack
 *  The concrete preconfigured insatnce for CaloEnergyForTrack tool
 *  along the track line 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
class PrsEnergyForTrack : public CaloEnergyForTrack 
{
  // friend factory for istantiation
  friend class ToolFactory<PrsEnergyForTrack>;
protected:
  PrsEnergyForTrack
  ( const std::string& type   , // ? 
    const std::string& name   , 
    const IInterface*  parent ) 
    : CaloEnergyForTrack ( type , name , parent ) 
  {
    _setProperty ( "DataAddress" , LHCb::CaloDigitLocation::Prs ) ;
    _setProperty ( "Tolerance"   , "2"    ) ; /// 2 * Gaudi::Units::mm  
    _setProperty ( "Calorimeter" , DeCalorimeterLocation::Prs   ) ;
  } ;
  virtual ~PrsEnergyForTrack() {}
private:
  // default constructor is disabled 
  PrsEnergyForTrack() ;
  // copy constructor is disabled 
  PrsEnergyForTrack           ( const  PrsEnergyForTrack& ) ;
  // assignement is disabled 
  PrsEnergyForTrack& operator=( const PrsEnergyForTrack& ) ;
} ;
// ============================================================================
/// Factory
// ============================================================================
DECLARE_TOOL_FACTORY(PrsEnergyForTrack)
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
