// $Id: EHCalSensDet.cpp,v 1.6 2008-07-11 10:47:44 robbep Exp $ 
// Include files 

// SRD & STD 
#include <algorithm>
#include <vector>

// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/PhysicalConstants.h"

// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/AlgFactory.h"

// GiGa 
//#include "GiGa/GiGaMACROs.h"
#include "GiGa/GiGaHashMap.h"

// GaussTools 
#include "GaussTools/GaussTrackInformation.h"

// Geant4 
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

// local
#include "CaloHit.h"
#include "CaloSimHash.h"
#include "EHCalSensDet.h"

//
#include "AIDA/IHistogram1D.h"

//-----------------------------------------------------------------------------
//
//  Implementation of class EHCalSensDet
//
//  2001-01-23 : Vanya Belyaev, Patrick Robbe
//
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EHCalSensDet::EHCalSensDet
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : G4VSensitiveDetector( name  )
  , CaloSensDet        ( type , name , parent ) 
  , m_slotWidth( 25 * ns ) 
{ 
  declareProperty( "SlotWidth" , m_slotWidth ) ; 
}

//=============================================================================
// The fractions of energy deposited in consequitive time-slots
// in the given Ecal/Hcal cell
//=============================================================================
StatusCode EHCalSensDet::timing 
( const double             time      , 
  const LHCb::CaloCellID&        cell      ,
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
}
