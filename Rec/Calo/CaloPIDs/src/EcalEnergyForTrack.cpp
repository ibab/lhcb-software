// $Id: EcalEnergyForTrack.cpp,v 1.2 2007-08-24 21:25:18 odescham Exp $
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
/** @class EcalEnergyForTrack
 *  The concrete preconfigured insatnce for CaloEnergyForTrack tool
 *  along the track line 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
class EcalEnergyForTrack : public CaloEnergyForTrack 
{
  // friend factory for istantiation
  friend class ToolFactory<EcalEnergyForTrack>;
protected:
  EcalEnergyForTrack
  ( const std::string& type   , // ? 
    const std::string& name   , 
    const IInterface*  parent ) 
    : CaloEnergyForTrack ( type , name , parent ) 
  {
    _setProperty ( "DataAddress"   , LHCb::CaloDigitLocation::Ecal ) ;
    _setProperty ( "MorePlanes"    , "3"    ) ; /// 3 additional planes 
    _setProperty ( "AddNeigbours"  , "true" ) ; 
    _setProperty ( "Tolerance"     , "5"    ) ; /// 5 * Gaudi::Units::mm 
    _setProperty ( "Calorimeter"   , DeCalorimeterLocation::Ecal   ) ;
  } ;
  virtual ~EcalEnergyForTrack() {}
private:
  // default constructor is disabled 
  EcalEnergyForTrack() ;
  // copy constructor is disabled 
  EcalEnergyForTrack           ( const  EcalEnergyForTrack& ) ;
  // assignement is disabled 
  EcalEnergyForTrack& operator=( const EcalEnergyForTrack& ) ;
} ;
// ============================================================================
/// Factory
// ============================================================================
DECLARE_TOOL_FACTORY(EcalEnergyForTrack) ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
