// $Id: EHCalSensDet.cpp,v 1.2 2003-12-10 13:49:30 ranjard Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2003/07/07 08:21:07  ibelyaev
//  split the general CaloSensDet class
//
// ============================================================================
/// SRD & STD 
#include <algorithm>
#include <vector>
/// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/PhysicalConstants.h"
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/AlgFactory.h"
/// GiGa 
#include "GiGa/GiGaMACROs.h"
#include "GiGa/GiGaHashMap.h"
/// GaussTools 
#include "GaussTools/GaussTrackInformation.h"
/// Geant4 
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4SDManager.hh"
#include "G4EnergyLossTables.hh"
// GiGaCnv 
#include "GiGaCnv/GiGaVolumeUtils.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h"
/// local
#include "CaloHit.h"
#include "CaloSimHash.h"
#include "EHCalSensDet.h"
///
#include "AIDA/IHistogram1D.h"

// ============================================================================
/** @file 
 * 
 *  Implementation of class EHCalSensDet
 *  
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author  Patrick Robbe robbe@lal.in2p3.fr 
 *
 *  @date   23/01/2001 
 */
// ============================================================================

// ============================================================================
/** standard constructor 
 *  @see GiGaSensDetBase 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
EHCalSensDet::EHCalSensDet
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : CaloSensDet        ( type , name , parent ) 
  , G4VSensitiveDetector( name  )
  , m_slotWidth( 25 * ns ) 
{ 
  declareProperty( "SlotWidth" , m_slotWidth ) ; 
};

// ============================================================================
/** The fractions of energy deposited in consequitive time-slots 
 *  in the give Ecal/Hcal cell
 *  @param time global time of energy deposition
 *  @param cell cellID of the cell 
 *  @param slot (out) the first time slot 
 *  @param fracs the vector of frractions for subsequent time-bins;
 *  @return StatuscCode 
 */
// ============================================================================
StatusCode EHCalSensDet::timing 
( const double             time      , 
  const CaloCellID&        cell      ,
  CaloSubHit::Time&        slot    ,
  CaloSensDet::Fractions&  fractions ) const 
{
  // clear input data 
  slot = 0 ; fractions.clear();
  
  // evaluate the real delta time  
  const double deltaT = time - t0 ( cell ) ;
  
  // find the absolute time slot 
  slot  = (CaloSubHit::Time) floor( deltaT / slotWidth() ) ;
  
  // the time into the slot 
  const double dt  = deltaT - slot * slotWidth() ;
  
  for ( Histos::const_iterator ihist = histos().begin() ; 
        histos().end() != ihist ; ++ihist ) 
    {
      const AIDA::IHistogram1D* histo = *ihist ;
      const int           bin   = histo -> coordToIndex ( dt / ns ) ;
      const double        frac  = histo -> binHeight    ( bin     ) ;
      fractions.push_back( frac ) ;
    };
  
  if( fractions.empty() ) 
    { Warning("timing()::no time information is available!"); }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
