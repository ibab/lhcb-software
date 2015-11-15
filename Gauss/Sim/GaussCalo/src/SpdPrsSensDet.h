// $Id: SpdPrsSensDet.h,v 1.7 2007-01-12 15:24:50 ranjard Exp $
#ifndef       GAUSSCALO_SpdPrsSensDet_H
#define       GAUSSCALO_SpdPrsSensDet_H 1

// GaudiKernel
/// Ntupel Svc
#include "GaudiKernel/INTuple.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
/// GiGa
#include "GiGa/GiGaSensDetBase.h"
#include "GiGa/GiGaHashMap.h"
/// local
#include "CaloSim.h"
#include "CaloHit.h"
#include "CaloSensDet.h"

class DeCalorimeter ;  // CaloDet

/** @class SpdPrsSensDet SpdPrsSensDet.h
 *
 *
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author  Grigori Rybkine Grigori.Rybkine@cern.ch
 *
 *  @date    23/01/2001
 */

class SpdPrsSensDet: public CaloSensDet
{

public :

  /** standard initialization (Gaudi)
   *  @see GiGaSensDetBase
   *  @see GiGaBase
   *  @see   AlgTool
   *  @see  IAlgTool
   *  @return status code
   */
  virtual StatusCode initialize   ();

  /** standard finalization (Gaudi)
   *  @see GiGaSensDetBase
   *  @see GiGaBase
   *  @see   AlgTool
   *  @see  IAlgTool
   *  @return status code
   */
  virtual StatusCode finalize    ();

protected:

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
  virtual StatusCode    fillHitInfo
  ( CaloSubHit*                 hit         ,
    const HepGeom::Point3D<double>& prePoint    ,
    const double                globalTime  ,
    const double                deposit     ,
    const G4Track*              track       ,
    const G4ParticleDefinition* pdef        ,
    const G4MaterialCutsCouple* material    ,
    const G4Step*               step        ) const ;

  /** The fractions of energy deposited in consequitive time-bins
   *  for the given calorimeter cell
   *  @param time global time of energy deposition
   *  @param cell cellID of the cell
   *  @param slot (out) the first time slot
   *  @param fracs the vector of fractions for subsequent time-slots;
   *  @return StatusCode
   */
  virtual StatusCode timing
  ( const double            time      ,
    const LHCb::CaloCellID&       cell      ,
    CaloSubHit::Time&       slot      ,
    CaloSensDet::Fractions& fractions ) const ;

public:

  /** standard constructor
   *  @see CaloSensDet
   *  @see GiGaSensDetBase
   *  @see GiGaBase
   *  @see AlgTool
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  SpdPrsSensDet
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;

  /// destructor (virtual and protected)
  virtual ~SpdPrsSensDet() {};

private:

  // no default constructor
  SpdPrsSensDet() ;
  // no copy constructor
  SpdPrsSensDet           ( const SpdPrsSensDet& );
  // no assignement
  SpdPrsSensDet& operator=( const SpdPrsSensDet& ) ;

private:
  double                                      m_BX                ;
  unsigned int                                m_numBXs                ;
  std::vector<double>                         m_sDelays ;
  double                                      m_fracMin                ;

// flag controlling the correction C1'= 7.2/12.6 * C1
// for multiply charged particles: == true  correction is applied
//                                 == false  correction is not applied
  bool                                      m_multiChargedBirks ;
};

#endif  ///< GAUSSCALO_SpdPrsSensDet_H

