// $Id: CaloTrackEcalEval.h,v 1.4 2004-09-02 18:55:36 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOTRACKEcalEval_H 
#define CALOTRACKEcalEval_H 1
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
// Event
#include "Event/CaloHypo.h"
#include "Event/TrStoredTrack.h"
// 

/** @class CaloTrackEcalEval CaloTrackEcalEval.h
 *  
 *  The Tool which evaluates the energy deposition 
 *  in Ecal for given track
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-11-10
 */
class CaloTrackEcalEval :
  public virtual ICaloTrackIdEval  , 
  public virtual IIncidentListener ,
  public          CaloTool        
{
  // friend factory for instantiation 
  friend class ToolFactory<CaloTrackEcalEval>;
public:
  
  /** The main processing method 
   *  @see ICaloTrackIdEval
   *  It evaluates the Ecal energy deposition for the given track 
   *  @param  track  pointer to thobject to be processed
   *  @param  value  (return) the value of the estimator
   *  @return status code 
   */  
  virtual StatusCode process    
  ( const TrStoredTrack* track , 
    double&              value ) const ;
  
  /** The main processing method (functor interface)
   *  @see ICaloTrackIdEval
   *  It evaluates the Ecal energy deposition for the given track 
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
   *  @param  the value of the estimator
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
  CaloTrackEcalEval
  ( const std::string& type   , 
    const std::string& name   ,
    const IInterface*  parent );
  
  virtual ~CaloTrackEcalEval(){}; ///< Destructor
  
private:
  
  /// the default constructor is private
  CaloTrackEcalEval();
  /// the copy constructor is private 
  CaloTrackEcalEval           ( const CaloTrackEcalEval& );
  /// the assignement is private 
  CaloTrackEcalEval& operator=( const CaloTrackEcalEval& );

private:
  
  // bad value 
  double          m_bad ;
  
  // interface type 
  typedef IAssociatorWeighted<CaloHypo,TrStoredTrack,float> IAsct ;
  // relation table type 
  typedef IAsct::InverseType                                Table ;
  typedef Table::Range                                      Range ;
  // associator to hypo type
  std::string     m_associatorType ;
  std::string     m_associatorName ;
  IAsct*          m_associator     ;
  // the realtion table itself (for efficiency reasons)
  mutable Table*  m_table          ;
  
};
// ============================================================================

// ============================================================================
// The END
// ============================================================================
#endif // CALOTRACKEcalEval_H
// ============================================================================
