// $Id: CaloTrackPrsEval.h,v 1.5 2005-05-08 09:34:06 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.5 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOTRACKPrsEval_H 
#define CALOTRACKPrsEval_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
// ============================================================================
// CaloInterfaces 
// ============================================================================
#include "CaloInterfaces/ICaloTrackIdEval.h"
// ============================================================================
// CaloKernel 
// ============================================================================
#include "CaloKernel/CaloTool.h"
// ============================================================================
// TrKernel
// ============================================================================
#include "TrKernel/ITrExtrapolator.h" 
// ============================================================================
// Event
// ============================================================================
#include "Event/CaloDigit.h"
#include "Event/TrStoredTrack.h"
#include "Event/CaloDataFunctor.h"
// ============================================================================

/** @class CaloTrackPrsEval CaloTrackPrsEval.h
 *  
 *  The Tool which evaluates the *ACTUAL* energy 
 *  deposition in Prs/or Spd for given track.
 *
 *  @author Vanya   Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-11-10
 */
class CaloTrackPrsEval :
  public virtual ICaloTrackIdEval  , 
  public virtual IIncidentListener ,
  public          CaloTool        
{
  // friend factory for instantiation 
  friend class ToolFactory<CaloTrackPrsEval>;
public:
  
  /** The main processing method 
   *  @see ICaloTrackIdEval
   *  It evaluates the Prs energy deposition for the given track 
   *  @param  track  pointer to thobject to be processed
   *  @param  value  (return) the value of the estimator
   *  @return status code 
   */  
  virtual StatusCode process    
  ( const TrStoredTrack* track , 
    double&              value ) const ;
  
  /** The main processing method (functor interface)
   *  @see ICaloTrackIdEval
   *  It evaluates the Prs energy deposition for the given track 
   *  @param  track  pointer to thobject to be processed
   */  
  virtual double     operator() 
    ( const TrStoredTrack* track ) const ;

  /** The main processing method 
   *  @see ICaloTrackIdEval 
   *  It evaluated the Track ID estimators using the calorimeter information  
   *  @param  track  pointer to the object to be processed
   *  @param  value  (return) the value of the estimator
   *  @return status code 
   */  
  virtual StatusCode process    
  ( const TrgTrack*      track , 
    double&              value ) const ;
  
  /** The main processing method (functor interface)
   *  @see ICaloTrackIdEval 
   *  It evaluated the Track ID estimators using the calorimeter information  
   *  @param  track  pointer to the object to be processed
   *  @return the value of the estimator
   */  
  virtual double     operator() 
    ( const TrgTrack*      track ) const ;

  /** standard initialization method 
   *  @see CaloTool 
   *  @see  AlgTool 
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode    initialize ();

  /** standard finalization method 
   *  @see CaloTool 
   *  @see  AlgTool 
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode    finalize   ();

  /** handle the incident 
   *  (reset the cached values  at the end of the event)
   *  @see Incident 
   *  @see IIncidentListener 
   *  @see IIncidentSvc 
   *  @param incident incident to be handled 
   */
  virtual void handle( const Incident& incident ) ;
  
protected:
  
  /** Standard constructor
   *  @see   CaloTool
   *  @see    AlgTool 
   *  @param type    type of the tool  (?)
   *  @param name    name of the concrete instance 
   *  @param parent  pointer to parent object (algorithm, service xor tool)
   */
  CaloTrackPrsEval
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent );
  
  virtual ~CaloTrackPrsEval(){}; ///< Destructor

private:

  /** find a track projection to a certain Z 
   *  @param  track track to be projected 
   *  @param  z     z-position of projection
   *  @return status code 
   */
  StatusCode findTrackProjection( const TrStoredTrack* track , 
                                  const double         z     ) const ;
  
private:
  
  /// the default constructor is private
  CaloTrackPrsEval();
  /// the copy constructor is private 
  CaloTrackPrsEval           ( const CaloTrackPrsEval& );
  /// the assignement is private 
  CaloTrackPrsEval& operator=( const CaloTrackPrsEval& );

private:
  
  // bad value 
  double                           m_bad              ;
  
  // input digits 
  std::string                      m_input            ;  
  
  // extrapolator 
  std::string                      m_extrapolatorType ;
  std::string                      m_extrapolatorName ;
  ITrExtrapolator*                 m_extrapolator     ;
  // particle ID (needed for some extrapolators) 
  // it is mutable due to *strange* extrapolator interface 
  mutable ParticleID               m_pid              ;
  long                             m_pidPDG           ;
  // tolerance parameter 
  double                           m_tol              ;
  // shower size 
  double                           m_shower           ;
  // minimal track error 
  double                           m_safe             ;

  // incident service
  IIncidentSvc*                    m_incSvc           ;
  // event service 
  IDataProviderSvc*                m_eventSvc         ;
  
  mutable CaloDigits*              m_digits           ;
  
  // z of calorimeter 
  double                           m_z                ;
  
  // technical 
  mutable bool                     m_tr               ;
  mutable double                   m_trX              ;
  mutable double                   m_trY              ;
  mutable double                   m_trXe             ;
  mutable double                   m_trYe             ;
  
};
// ============================================================================

// ============================================================================
// The END
// ============================================================================
#endif // CALOTRACKPrsEval_H
// ============================================================================
