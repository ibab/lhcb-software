// $Id: HcalEnergyForTrack.cpp,v 1.6 2009-10-30 09:52:09 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.6 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2009/03/11 19:05:40  vegorych
// new PIDs PDF for DC09
//
// Revision 1.4  2008/05/13 12:25:51  odescham
// restore _setProperty()
//
// Revision 1.3  2008/01/24 10:22:42  vegorych
// _setProperty was removed, AddNeigbours warning was solved
//
// Revision 1.2  2007/08/24 21:25:18  odescham
// fix uncheck. StatusCodes
//
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
  // ==========================================================================
  /// friend factory for istantiation
  friend struct ToolFactory<HcalEnergyForTrack>;
  // ==========================================================================
protected:
  // ==========================================================================
  HcalEnergyForTrack
  ( const std::string& type   , // ? 
    const std::string& name   , 
    const IInterface*  parent ) 
    : CaloEnergyForTrack ( type , name , parent ) 
  {
    setProperty ( "DataAddress"    , LHCb::CaloDigitLocation::Hcal ).ignore() ;
    setProperty ( "Calorimeter"    , DeCalorimeterLocation::Hcal   ).ignore() ;
    //
    setProperty ( "MorePlanes"     , 5                             ).ignore() ; 
    setProperty ( "AddNeigbours"   , 0                             ).ignore() ;  
    setProperty ( "Tolerance"      , 15.0 * Gaudi::Units::mm       ).ignore() ;
  }
  // ==========================================================================
  virtual ~HcalEnergyForTrack() {}
  // ==========================================================================
private:
  // ==========================================================================
  // default constructor is disabled 
  HcalEnergyForTrack() ;
  // copy constructor is disabled 
  HcalEnergyForTrack           ( const  HcalEnergyForTrack& ) ;
  // assignement is disabled 
  HcalEnergyForTrack& operator=( const HcalEnergyForTrack& ) ;
  // ==========================================================================
} ;
// ============================================================================
// Factory
// ============================================================================
DECLARE_TOOL_FACTORY(HcalEnergyForTrack) 
// ============================================================================
// The END 
// ============================================================================
