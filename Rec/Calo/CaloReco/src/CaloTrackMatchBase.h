// $Id: CaloTrackMatchBase.h,v 1.5 2004-10-24 12:17:18 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOTRACKTOOLS_CALOTRACKMATCHBASE_H 
#define CALOTRACKTOOLS_CALOTRACKMATCHBASE_H 1
// ============================================================================
// Include files
// ============================================================================
// ============================================================================
#include <functional>
// ============================================================================
// CLHEP 
// ============================================================================
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/DiagMatrix.h"
// ============================================================================
// GaudiKernel 
// ============================================================================
#include "GaudiKernel/IIncidentListener.h"
// ============================================================================
// CaloInterfaces 
// ============================================================================
#include "CaloInterfaces/ICaloTrackMatch.h"
// ============================================================================
// CaloKernel
// ============================================================================
#include "CaloKernel/CaloTool.h"
// ============================================================================
template <class TOOL>
class ToolFactory     ; ///< from GaudiKernel
class IIncidentSvc    ; ///< from GaudiKernel  
class ITrExtrapolator ; ///< from TrKernel
// ============================================================================
class CaloPosition  ;
class TrStoredTrack ;
class TrStateP      ;
class TrgTrack      ;
// ============================================================================
 
/** @class CaloTrackMatchBase CaloTrackMatchBase.h
 *  
 *  The base class for concrete implementtaions of matching utilities. \n
 *  Contains functions common for all mathcing tools (mathematical evaluations),
 *  prototypes for private functions in every tool
 *  and a prototype for the main matching method.
 *  @author Dima  Rusinov Dmitri.Roussinov@cern.ch
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   01/11/2001
 */

class CaloTrackMatchBase: 
  virtual public IIncidentListener ,
  virtual public ICaloTrackMatch   , 
  public          CaloTool      
{  
public:
  
  /** enum with return codes
   *  the codes are valid for matching tools only
   */
  enum ERROR 
  {
    TOOL      = 250 ,
    EXTR_INIT = 251 ,
    MATHEM    = 252 ,
    TRACK     = 253 ,
    EXTRAP    = 254 
  };

public:

  /** standard initialization
   *  @see   CaloTool
   *  @see    AlgTool
   *  @see   IAlgTool
   *  @return status code
   */
  virtual StatusCode initialize () ;
  
  /** standard finalization
   *  @see   CaloTool
   *  @see    AlgTool
   *  @see   IAlgTool
   *  @return status code
   */
  virtual StatusCode finalize   () ;
  
  /** The main matching method (Stl interface) 
   *  @param caloObj  pointer to "calorimeter" object (position)
   *  @param trObj    pointer to tracking object (track)
   *  @return pair of status code/chi2  for matching procedure 
   */
  virtual MatchingPair    operator() 
    ( const CaloPosition*  caloObj,
      const TrStoredTrack* trObj    );
  
  /** access to the last used state 
   *  @see ICaloTrackMatch 
   *  @return teh last used state 
   */
  virtual const TrState *state() const { return m_state; }

  /** the main matching method  
   *
   *  @param caloObj  pointer to "calorimeter" object (position)
   *  @param trObj    pointer to tracking object (track)
   *  @param chi2     returned value of chi2 of the matching
   *  @return status code for matching procedure 
   */
  virtual StatusCode match 
  ( const CaloPosition*   caloObj  , 
    const TrgTrack*       trObj    ,
    double&               chi2     ) ;

  /** The main matching method (Stl interface) 
   *  @param caloObj  pointer to "calorimeter" object (position)
   *  @param trObj    pointer to tracking object (track)
   *  @return pair of status code/chi2  for matching procedure 
   */
  virtual MatchingPair    operator() 
    ( const CaloPosition*   caloObj  , 
      const TrgTrack*       trObj    ) ;

  /** handle the incident
   *  @see IIncidentListener 
   *  @param incident incident to be handled 
   */
  virtual void handle ( const Incident& incident ) ;
  
protected: 
  
  /** Find TrState on specified Z.
   *  @param Object with Track data
   *  @param Z    of the TrState
   *  @param Zext z for extrapolation 
   *  @return standard status code
   */
  StatusCode findState
  ( const TrStoredTrack* trObj , 
    const double         Z     , 
    const double         zExtr ) const ;
  
  /// internal type type of parameters for mathematical functions
  struct MatchStruct1
  { 
    // parametrs x,y, (e) 
    HepVector    params  ;
    // covarinace matrix x,y,(e)
    HepSymMatrix cov     ;    
    // error flag 
    int          error   ;
    // constructor 
    MatchStruct1 
    ( const HepVector    & p , 
      const HepSymMatrix & m ) 
      : params ( p )
    { cov = m.inverse ( error ) ; } ;
  };
  
  /// internal type for type of parameters for mathematical functions
  struct MatchStruct2
  { 
    // parametrs x,y, (e) 
    HepVector     params  ;
    // covarinace matrix x,y,(e)
    HepDiagMatrix cov     ;    
    // error flag 
    int           error   ;
    
    // constructor 
    MatchStruct2 
    ( const HepVector     & p , 
      const HepDiagMatrix & m ) 
      : params ( p ) 
    { cov = m.inverse( error ) ; }
  };
  
  /// internal type for mathematical functions 
  typedef MatchStruct1 MatchType1 ;  
  typedef MatchType1   MatchType  ;  
  
  /// internal type for mathematical functions 
  typedef MatchStruct2 MatchType2;
  
  /** chi2 method \n
   * implements a formula:
   * chi2 = dp1(T)*C1(-1)*dp1 + dp2(T)*C2(-1)*dp2 \n
   * where dpi = pi - mean. \n
   * Ci(-1) is the inverted covariance. \n
   * In case of failure throws a CaloException.
   * @param struct with first vector and its covariance
   * @param struct with second vector and its covariance
   * @return result chi2
   */
  inline double chi2 ( const MatchType& mt1 , 
                       const MatchType& mt2 ) const 
  {

    Assert ( 0 == mt1.error  &&  0 == mt2.error && 
             mt1.cov    .num_row() == mt2.cov    .num_row() && 
             mt1.params .num_row() == mt2.params .num_row() , 
             "Matrix/Vector mismatch!" ) ;
    
    const HepSymMatrix& icov1 = mt1.cov   ;
    const HepSymMatrix& icov2 = mt2.cov   ;
    HepSymMatrix&       cov   = m_aux_sym ;
    
    const int ifail = mtrxOp( cov , icov1 , icov2 ) ;
    Assert ( 0 == ifail  , "Can not invert the matrix !" );
    
    const HepVector vmean ( cov * ( icov1 * mt1.params + icov2 * mt2.params ) );
    
    return 
      icov1.similarity ( vmean - mt1.params ) + 
      icov2.similarity ( vmean - mt2.params ) ;
    
  };
  
  /** chi2 method \n
   * implements a formula:
   * chi2 = dp1(T)*C1(-1)*dp1 + dp2(T)*C2(-1)*dp2 \n
   * where dpi = pi - mean. \n
   * Ci(-1) is the inverted covariance. \n
   * In case of failure throws a CaloException.
   * @param struct with first vector and its covariance
   * @param struct with second vector and its covariance
   * @return result chi2
   */
  inline double chi2 ( const MatchType2& mt1 , 
                       const MatchType2& mt2 ) const 
  {
    
    Assert ( 0 == mt1.error  &&  0 == mt2.error && 
             mt1.cov    .num_row() == mt2.cov    .num_row() && 
             mt1.params .num_row() == mt2.params .num_row() , 
             "Matrix/Vector mismatch!" ) ;
    
    const HepDiagMatrix& icov1 = mt1.cov    ;
    const HepDiagMatrix& icov2 = mt2.cov    ;
    HepDiagMatrix&       cov   = m_aux_diag ;
    
    const int ifail = mtrxOp( cov , icov1 , icov2 ) ;
    Assert ( 0 == ifail  , "Can not invert the matrix !" );
    
    const HepVector vmean ( cov * ( icov1 * mt1.params + icov2 * mt2.params ) );
    
    return 
      icov1.similarity ( vmean - mt1.params ) + 
      icov2.similarity ( vmean - mt2.params ) ;
    
  };
  
  /** chi2 method \n
   * implements a formula:
   * chi2 = dp1(T)*C1(-1)*dp1 + dp2(T)*C2(-1)*dp2 \n
   * where dpi = pi - mean. \n
   * Ci(-1) is the inverted covariance. \n
   * In case of failure throws a CaloException.
   * @param struct with first vector and its covariance
   * @param struct with second vector and its covariance
   * @return result chi2
   */
  inline double chi2 ( const MatchType1& mt1 , 
                       const MatchType2& mt2 ) const 
  { 
    
    Assert ( 0 == mt1.error  &&  0 == mt2.error && 
             mt1.cov    .num_row() == mt2.cov    .num_row() && 
             mt1.params .num_row() == mt2.params .num_row() , 
             "Matrix/Vector mismatch!" ) ;
    
    const HepSymMatrix&  icov1 = mt1.cov   ;
    const HepDiagMatrix& icov2 = mt2.cov   ;
    HepSymMatrix&        cov   = m_aux_sym ;

    const int ifail = mtrxOp( cov , icov1 , icov2 ) ;
    Assert ( 0 == ifail  , "Can not invert the matrix !" );
    
    const HepVector vmean ( cov * ( icov1 * mt1.params + icov2 * mt2.params ) );
    
    return 
      icov1.similarity ( vmean - mt1.params ) + 
      icov2.similarity ( vmean - mt2.params ) ;
    
  };
  
  /** chi2 method \n
   * implements a formula:
   * chi2 = dp1(T)*C1(-1)*dp1 + dp2(T)*C2(-1)*dp2 \n
   * where dpi = pi - mean. \n
   * Ci(-1) is the inverted covariance. \n
   * In case of failure throws a CaloException.
   * @param struct with first vector and its covariance
   * @param struct with second vector and its covariance
   * @return result chi2
   */
  inline double chi2 ( const MatchType2& mt1 , 
                       const MatchType1& mt2 ) const 
  { return chi2 ( mt2 , mt1 ) ; }
  
  inline int mtrxOp
  ( HepSymMatrix&       cov ,
    const HepSymMatrix& mx1 , 
    const HepSymMatrix& mx2 ) const 
  {
    cov = mx1 + mx2 ;
    int ifail = 0 ;
    cov.invert ( ifail ) ;
    return ifail ; 
  };
  
  inline int mtrxOp 
  ( HepDiagMatrix&       cov ,
    const HepDiagMatrix& mx1 , 
    const HepDiagMatrix& mx2 ) const 
  {
    cov = mx1 + mx2 ; 
    int ifail = 0 ;
    cov.invert ( ifail ) ;
    return ifail ; 
  };
  
  inline int mtrxOp 
  ( HepSymMatrix&        cov ,
    const HepSymMatrix&  mx1 , 
    const HepDiagMatrix& mx2 ) const 
  {
    cov = mx1 + mx2 ;
    int ifail = 0 ;
    cov.invert ( ifail ) ;
    return ifail ; 
  };

protected:
  
  /** return the decoded track bits pattern
   *  @param track track objects 
   *  @return track bits pattern 
   */
  std::string bits ( const TrStoredTrack* track ) const ;
  
protected:
  
  /** accessor to extrapolator 
   *  @return pointer to track extrapolator
   */
  ITrExtrapolator* extrapolator() const { return m_extrapolator; }
  
  /** standard constructor 
   *  @param type tool type   (useless) 
   *  @param name tool name   
   *  @param parent pointer to parent object 
   *         (algorithm, service or another tool)
   */
  CaloTrackMatchBase
  ( const std::string &type,
    const std::string &name,
    const IInterface  *parent );
  
  /** destructor is protected and virtual
   */
  virtual ~CaloTrackMatchBase();
  
private:
  
  /** default constructor in private!
   */
  CaloTrackMatchBase(); 
  
  /** copy constructor is private!
   *  @param copy object to be copied
   */
  CaloTrackMatchBase( const CaloTrackMatchBase& copy );
  
  /** assignement operator is private!
   *  @param copy object to be copied
   */
  CaloTrackMatchBase& operator=( const CaloTrackMatchBase& copy );
  
protected:
  
  // track state used for matching
  mutable TrState*             m_state     ;
  
  // the previous stored track   
  mutable const TrStoredTrack* m_prevTrack ;  
  
  // default 'bad' value 
  double                       m_bad       ;

private: 
  
  // allowed interval for closest Z 
  double                       m_zmin      ;
  double                       m_zmax      ;
  
  // particle ID to be used for extrapolation 
  int                          m_pdgID     ;
  mutable ParticleID           m_pid       ;

  
  // interface to track extrapolator
  
  ITrExtrapolator*             m_extrapolator      ;    
  
  // and the extrapolator name
  std::string                  m_extrapolatorName  ;

  // extrapolation tolerance in Z 
  double                       m_tolerance         ;

  // local storages 
  mutable HepSymMatrix         m_aux_sym  ;
  mutable HepDiagMatrix        m_aux_diag ;
  
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
#endif // CALOTRACKTOOLS_CALOTRACKMATCHBASE_H
// ============================================================================
