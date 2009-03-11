// $Id: EcalEnergyForTrack.cpp,v 1.5 2009-03-11 19:05:40 vegorych Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $
// ============================================================================
// $Log: not supported by cvs2svn $
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
    _setProperty ( "AddNeigbours"  , "false" ) ; 
    _setProperty ( "Tolerance"     , "5" ) ; /// 5 * Gaudi::Units::mm 
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
