// $Id: CaloTrackEval.h,v 1.1.1.1 2003-03-13 18:52:02 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/12/01 14:22:57  ibelyaev
//  Hcal stuff and updated S-coprrections
//
// ============================================================================
#ifndef CaloTrackEval_H 
#define CaloTrackEval_H 1
// Include files
// LHcbKernel
#include "Relations/IAssociatorWeighted.h"
// GaudiKernel
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
// CaloInterfaces 
#include "CaloInterfaces/ICaloTrackIdEval.h"
// CaloKernel 
#include "CaloKernel/CaloTool.h"
// TrKernel
#include "TrKernel/ITrExtrapolator.h" 
// Event
#include "Event/CaloDigit.h"
#include "Event/TrStoredTrack.h"
#include "Event/CaloDataFunctor.h"
// CaloDet 
#include "CaloDet/DeCalorimeter.h"
// 

/** @class CaloTrackEval CaloTrackEval.h
 *  
 *  The Tool which evaluates the *ACTUAL* energy 
 *  deposition in "thick" Calorimeter (Hcal or Ecal) 
 *  for given track.
 *
 *  @author Vladimir Romanovsky Vladimir.Romanovski@cern.ch
 *  @author Vanya Belyaev       Ivan.Belyaev@itep.ru
 *  @date   2002-11-10
 */
class CaloTrackEval :
  public virtual ICaloTrackIdEval  , 
  public virtual IIncidentListener ,
  public          CaloTool        
{
  // friend factory for instantiation 
  friend class ToolFactory<CaloTrackEval>;
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
   *  @param  the value of the estimator
   */  
  virtual double     operator() 
    ( const TrStoredTrack* track ) const ;

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
  CaloTrackEval
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent );
  
  virtual ~CaloTrackEval(){}; ///< Destructor

private:

  /** find a track projection to a certain Z 
   *  @param  z     z-position of projection
   *  @return status code 
   */
  StatusCode findTrackProjection( const double         z     ) const ;
  
private:
  
  /// the default constructor is private
  CaloTrackEval();
  /// the copy constructor is private 
  CaloTrackEval           ( const CaloTrackEval& );
  /// the assignement is private 
  CaloTrackEval& operator=( const CaloTrackEval& );

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
  
  // z of calorimeter 
  double                           m_z                ;
  double                           m_st               ;
  int 				                     m_numst	          ;

  mutable CaloDigits*              m_digits           ;
  
  // technical 
  mutable TrState* 			           m_state            ;
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
#endif // CaloTrackEval_H
// ============================================================================
