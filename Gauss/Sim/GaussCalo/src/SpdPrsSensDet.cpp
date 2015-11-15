// $Id: SpdPrsSensDet.cpp,v 1.12 2008-07-11 10:47:44 robbep Exp $
// Include files

/// SRD & STD
#include <algorithm>
#include <vector>
/// CLHEP
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Units/PhysicalConstants.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IHistogramSvc.h"

// GiGa
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
#include "G4MaterialCutsCouple.hh"

// GiGaCnv
#include "GiGaCnv/GiGaVolumeUtils.h"

// CaloDet
#include "CaloDet/DeCalorimeter.h"

// local
#include "CaloHit.h"
#include "CaloSimHash.h"
#include "SpdPrsSensDet.h"
//
#include "AIDA/IAxis.h"
#include "AIDA/IHistogram1D.h"

// DetDesc
#include "DetDesc/IGeometryInfo.h"

// ============================================================================
/** @file
 *
 *  Implementation of class SpdPrsSensDet
 *
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author  Grigori Rybkine Grigori.Rybkine@cern.ch
 *
 *  @date   23/01/2001
 */
// ============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( SpdPrsSensDet )

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
SpdPrsSensDet::SpdPrsSensDet
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : G4VSensitiveDetector( name  )
  , CaloSensDet        ( type , name , parent )
  , m_BX ( 25. * ns )
  , m_numBXs (6)
  , m_sDelays ( 3, 0. )
  , m_fracMin ( 1.e-5 )
  , m_multiChargedBirks ( true )
{
  declareProperty ( "BunchCrossing"     ,  m_BX      ) ;
  declareProperty ( "NumberBXs"         ,  m_numBXs  ) ;
  declareProperty ( "IntegrationDelays" ,  m_sDelays ) ;
  declareProperty ( "FracMin"           ,  m_fracMin ) ;
}

// ============================================================================
/** standard initialization (Gaudi)
 *  @see GiGaSensDetBase
 *  @see GiGaBase
 *  @see   AlgTool
 *  @see  IAlgTool
 *  @return statsu code
 */
// ============================================================================
StatusCode SpdPrsSensDet::initialize   ()
{
  return CaloSensDet::initialize();
}

// ============================================================================
/** standard finalization (Gaudi)
 *  @see GiGaSensDetBase
 *  @see GiGaBase
 *  @see   AlgTool
 *  @see  IAlgTool
 *  @return statsu code
 */
// ============================================================================
StatusCode SpdPrsSensDet::finalize    ()
{
  m_sDelays.clear () ;
  // finalize the base class
  return CaloSensDet::finalize();
}

// ============================================================================
/** fill the hit with the concrete information about the energy and the time.
 *  The function is to be called from ProcessHits method.
 *
 *  @param hit        hit  to be filled with the correct information,
 *                    the cellID information is accessible from the hit
 *  @param prePoint   PreStepPoint  for the given step
 *  @param globalTime the global time of prestep point
 *  @param deposit    raw energy deposition for the step
 *  @param track      the actual G4Track pbject (decoded from G4Step)
 *  @param pdef       the actual particle type  (decoded from G4Step/G4Track)
 *  @param material   the actual material       (decoded from G4Step)
 *  @param step       the step itself, the most important
 *                    information from the step
 *                    is already decoded into prePoint,globalTime,track,
 *                    particle definition,material etc for efficiency reasons.
 *
 */
// ============================================================================
StatusCode    SpdPrsSensDet::fillHitInfo
( CaloSubHit*                     hit,
  const HepGeom::Point3D<double>& /* prePoint   */,
  const double                    globalTime,
  const double                    deposit,
  const G4Track*                  track,
  const G4ParticleDefinition*     particle,
  const G4MaterialCutsCouple*     material,
  const G4Step*                   /* step       */) const
{
  if ( 0 == hit ) { return StatusCode::FAILURE ; }

  // Birks' Law Correction
  double edep = deposit ;
  edep *= birkCorrection
    ( particle                  ,
      track->GetKineticEnergy() ,
      material                  ) ;

  // add the current energy deposition to the sub-hit
  // smearing the energy deposition over a number of bunch crossings (timing)
  CaloSubHit::Time slot;
  CaloSensDet::Fractions frac;
  frac.reserve( m_numBXs ) ;

  const LHCb::CaloCellID cellID = hit->cellID();

  StatusCode sc = timing( globalTime , cellID , slot , frac );
  if ( sc.isFailure() )
  { return Error ( "Could not smear Edep!" , sc ) ; }

  for( unsigned int i = 0; i < frac.size(); i++, slot += 1 )
  { if ( frac[i] > m_fracMin ) { hit->add( slot, edep*frac[i] ) ; } }

  return StatusCode::SUCCESS ;
}
// ============================================================================
/** The fractions of energy deposited in consequitive time-slots
 *  for the given calorimeter cell
 *  @param time global time of energy deposition
 *  @param cell cellID of the cell
 *  @param slot (out) the first time slot
 *  @param fracs the vector of fractions for subsequent time-slots;
 *  @return StatusCode
 */
// ============================================================================
StatusCode SpdPrsSensDet::timing
( const double             time      ,
  const LHCb::CaloCellID&  cell      ,
  CaloSubHit::Time&        slot      ,
  CaloSensDet::Fractions&  fractions ) const
{
  const double locTime = time - t0( cell );

  // number of the current 25 ns bx w.r.t. local time
  slot = static_cast<CaloSubHit::Time>( floor( locTime/m_BX ) );

  const double refTime = locTime - slot * m_BX;

  // which area the cell is in: 0-Outer, 1-Middle, 2-Inner
  const unsigned int area = cell.area();

  const IAxis & axis = histos()[area]->axis();
  const double lowerEdge = axis.lowerEdge();
  const double upperEdge = axis.upperEdge();

  unsigned int i; double t;
  for( i = 0, t = - m_BX + m_sDelays[area] - refTime;
       i < m_numBXs;
       i++, t += m_BX )
    if( lowerEdge < t && t < upperEdge )
    {
      fractions.push_back( histos()[area]->binHeight( axis.coordToIndex(t) ) );
    }
    else fractions.push_back(0.);

  slot -= 1;

  return StatusCode::SUCCESS ;
}
// ============================================================================

// ============================================================================
// The END
// ============================================================================
