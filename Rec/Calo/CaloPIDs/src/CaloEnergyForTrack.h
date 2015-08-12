// $Id: CaloEnergyForTrack.h,v 1.3 2009-10-25 17:22:36 ibelyaev Exp $
// ============================================================================
#ifndef CALOENERGYFORTRACK_H 
#define CALOENERGYFORTRACK_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <set>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
// ============================================================================
// CaloInterfaces 
// ============================================================================
#include "CaloInterfaces/ICaloTrackIdEval.h"
#include "CaloInterfaces/ICaloDigits4Track.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
#include "Event/CaloDigit.h"
// ============================================================================
// local
// ============================================================================
#include "CaloTrackTool.h"
// ============================================================================
/** @class CaloEnergyForTrack
 *  tool which accumulates the energy for the given track 
 *  along the track line 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
class CaloEnergyForTrack 
  : public virtual ICaloTrackIdEval  
  , public virtual ICaloDigits4Track 
  , public virtual IIncidentListener 
  , public       Calo::CaloTrackTool
{
  // ==========================================================================
  // friend factory for istantiation
  friend class ToolFactory<CaloEnergyForTrack>;
  // ==========================================================================
public:
  // ==========================================================================
  typedef LHCb::CaloDigit::Container   Digits    ;
  typedef ICaloDigits4Track::Line      Line      ;
  // ==========================================================================
public :
  // ==========================================================================
  /// Inform that a new incident has occured @see IIncidentListener
  virtual void handle(const Incident& ) { m_digits = 0  ; }
  // ==========================================================================
public :
  // ==========================================================================
  /// initialize the tool 
  virtual StatusCode initialize() ;
  // ==========================================================================
public :
  // ==========================================================================
  /** The main processing method 
   *  It evaluated the Track ID estimators using the calorimeter information  
   *  @param  track  pointer to the object to be processed
   *  @param  value  (return) the value of the estimator
   *  @return status code 
   */  
  virtual StatusCode process   
  ( const LHCb::Track* track , 
    double&            value ) const ;
  /** The main processing method (functor interface)
   *  It evaluated the Track ID estimators using the calorimeter information  
   *  @param  track  pointer to the object to be processed
   */  
  virtual double     operator() 
    ( const LHCb::Track* track ) const ;
  // ==========================================================================
public: // ICaloDigits4Track
  // ==========================================================================
  /** collect the cellID-s along the line 
   *  @param line   (INPUT)  the line  
   *  @param cells  (OUTPUT) the container of cells 
   *  @return status code 
   */
  virtual StatusCode collect 
  ( const Line&               line   , 
    LHCb::CaloCellID::Vector& cells  ) const ;
  // ==========================================================================
  /** collect the cellID-s along the line 
   *  @param line   (INPUT)  the line  
   *  @param cells  (OUTPUT) the container of cells 
   *  @return status code 
   */
  virtual StatusCode collect 
  ( const Line& line   , 
    LHCb::CaloCellID::Set&  cells  ) const ;
  // ==========================================================================
public:
  // ==========================================================================
  /** collect the cellID-s along the path of the tracks 
   *  @param track  (INPUT)  the track 
   *  @param cells  (OUTPUT) the container of cells 
   *  @return status code 
   */
  virtual StatusCode collect 
  ( const LHCb::Track*        track  , 
    LHCb::CaloCellID::Vector& cells  ) const ;
  // ==========================================================================
  /** collect the cellID-s along the path of the tracks 
   *  @param track  (INPUT)  the track 
   *  @param cells  (OUTPUT) the container of cells 
   *  @return status code 
   */
  virtual StatusCode collect 
  ( const LHCb::Track*        track  , 
    LHCb::CaloCellID::Set&    cells  ) const ;
  // ==========================================================================  
public:
  // ==========================================================================
  /** collect the fired digits along the line 
   *  @param line   (INPUT)  the line  
   *  @param digits (OUTPUT) the container of digits 
   *  @return status code 
   */
  virtual StatusCode collect 
  ( const Line&               line   , 
    LHCb::CaloDigit::Vector&  digits ) const ;
  // ==========================================================================
  /** collect the fired digits along the path of the tracks 
   *  @param line   (INPUT)  the line  
   *  @param digits (OUTPUT) the container of digits 
   *  @return status code 
   */
  virtual StatusCode collect 
  ( const Line&               line   , 
    LHCb::CaloDigit::Set&     digits ) const ;
  // ==========================================================================  
public:
  // ==========================================================================  
  /** collect the fired digits along the path of the tracks 
   *  @param track  (INPUT)  the track 
   *  @param digits (OUTPUT) the container of digits 
   *  @return status code 
   */
  virtual StatusCode collect 
  ( const LHCb::Track*       track  , 
    LHCb::CaloDigit::Vector& digits ) const ;
  // ==========================================================================
  /** collect the fired digits along the path of the tracks 
   *  @param track  (INPUT)  the track 
   *  @param digits (OUTPUT) the container of digits 
   *  @return status code 
   */
  virtual StatusCode collect 
  ( const LHCb::Track*        track  , 
    LHCb::CaloDigit::Set&     digits ) const  ;
  // ==========================================================================  
protected:
  // ==========================================================================
  CaloEnergyForTrack
  ( const std::string& type   , // ? 
    const std::string& name   , 
    const IInterface*  parent ) ;
  virtual ~CaloEnergyForTrack() {}
  // ==========================================================================
protected:
  // ==========================================================================
  // get the data form TES 
  inline const Digits* digits() const ;
  // ==========================================================================
private:
  // ==========================================================================
  // default constructor is disabled 
  CaloEnergyForTrack() ;
  // copy constructor is disabled 
  CaloEnergyForTrack( const  CaloEnergyForTrack& ) ;
  // assignement is disabled 
  CaloEnergyForTrack& operator=( const CaloEnergyForTrack& ) ;
  // ==========================================================================
protected:
  // ==========================================================================
  typedef std::vector<Gaudi::Plane3D>   Planes ;
  typedef std::vector<Gaudi::XYZPoint>  Points ;
  typedef std::vector<LHCb::State*>     States ; 
  typedef LHCb::CaloCellID::Set         Cells  ;
  // ==========================================================================
private:
  // ==========================================================================
  std::string                 m_address      ; // calorimeter data 
  double                      m_bad          ; // "bad" value 
  mutable const Digits*       m_digits       ;
  // 
  LHCb::State::Location       m_location     ;
  Planes                      m_planes       ;
  unsigned short              m_morePlanes   ;
  unsigned short              m_addNeighbors ;
  //
  mutable LHCb::State         m_state        ;
  // ==========================================================================
} ;
// ============================================================================
// get the data form TES 
// ============================================================================
inline 
const CaloEnergyForTrack::Digits* 
CaloEnergyForTrack::digits() const 
{ 
  if ( 0 == m_digits ) { m_digits = get<Digits> ( m_address ) ; } 
  return m_digits ; 
} 
// ============================================================================
// The END 
// ============================================================================
#endif // CALOENERGYFORTRACK_H
// ============================================================================
