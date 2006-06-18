// $Id: CaloEnergyForTrack.h,v 1.1 2006-06-18 18:35:27 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
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
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
// ============================================================================
// CaloInterfaces 
// ============================================================================
#include "CaloInterfaces/ICaloTrackIdEval.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
#include "Event/CaloDigit.h"
// ============================================================================
// local
// ============================================================================
#include "CaloUtils/CaloTrackTool.h"
// ============================================================================


/** @class CaloEnergyForTrack
 *  tool which accumulates the energy for the given track 
 *  along the track line 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
class CaloEnergyForTrack 
  : public virtual ICaloTrackIdEval  
  , public virtual IIncidentListener 
  , public       Calo::CaloTrackTool
{
  // friend factory for istantiation
  friend class ToolFactory<CaloEnergyForTrack>;
public:
  typedef LHCb::CaloDigit::Container Digits ;
public :
  /// Inform that a new incident has occured @see IIncidentListener
  virtual void handle(const Incident& ) { m_digits = 0  ; }
public :
  /// initialize the tool 
  virtual StatusCode initialize() ;
public :
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
protected:
  CaloEnergyForTrack
  ( const std::string& type   , // ? 
    const std::string& name   , 
    const IInterface*  parent ) ;
  virtual ~CaloEnergyForTrack() {}
protected:
  // get the data form TES 
  inline const Digits* digits() const ;
private:
  // default constructor is disabled 
  CaloEnergyForTrack() ;
  // copy constructor is disabled 
  CaloEnergyForTrack( const  CaloEnergyForTrack& ) ;
  // assignement is disabled 
  CaloEnergyForTrack& operator=( const CaloEnergyForTrack& ) ;
protected:
  typedef std::vector<Gaudi::Plane3D>   Planes ;
  typedef std::vector<Gaudi::XYZPoint>  Points ;
  typedef std::set<LHCb::CaloCellID>    Cells  ;
  typedef std::vector<LHCb::State*>     States ; 
private:
  std::string                 m_address      ; ///< calorimeter data 
  double                      m_bad          ; ///< "bad" value 
  mutable const Digits*       m_digits       ;
  // 
  LHCb::State::Location       m_location     ;
  Planes                      m_planes       ;
  unsigned short              m_morePlanes   ;
  double                      m_nSigmas      ;
  bool                        m_addNeighbors ;
  //
  mutable LHCb::State         m_state        ;
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
} ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // CALOENERGYFORTRACK_H
// ============================================================================
