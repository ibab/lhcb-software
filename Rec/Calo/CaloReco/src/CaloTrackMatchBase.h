// $Id: CaloTrackMatchBase.h,v 1.11 2006-05-30 09:42:06 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
#ifndef CALORECO_CALOTRACKMATCHBASE_H 
#define CALORECO_CALOTRACKMATCHBASE_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & SLT 
// ============================================================================
#include <functional>
// ============================================================================
// Kernel
#include "GaudiKernel/SystemOfUnits.h"
#include "Kernel/TrackTypes.h"
#include "Kernel/Vector3DTypes.h"
#include "Kernel/SymmetricMatrixTypes.h"
// GaudiKernel 
#include "GaudiKernel/IIncidentListener.h"
// CaloInterfaces 
#include "CaloInterfaces/ICaloTrackMatch.h"
// GaudiAlg
#include "GaudiAlg/GaudiTool.h"
// Event
#include "Event/State.h"
#include "Event/CaloPosition.h"
// ============================================================================
template <class TOOL>
class ToolFactory     ; ///< from GaudiKernel
class IIncidentSvc    ; ///< from GaudiKernel  
class ITrackExtrapolator ; ///< from TrackInterfaces
// ============================================================================
namespace LHCb{
  class CaloPosition ;
  class Track ;
  class State ;
}
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
  public         GaudiTool      
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
    ( const LHCb::CaloPosition*  caloObj  ,
      const LHCb::Track* trObj    )
  {
    double     c2 = m_bad ;
    StatusCode sc = match ( caloObj , trObj , c2 ) ;
    return MatchingPair( sc , c2 ) ;
  };
  
  /** access to the last used state 
   *  @see ICaloTrackMatch 
   *  @return the last used state 
   */
  virtual const LHCb::State* state() const { return m_state; }
  
  /** handle the incident
   *  @see IIncidentListener 
   *  @param incident incident to be handled 
   */
  virtual void handle ( const Incident& incident ) ;
  
protected: 
  
  /** Find State on specified Z.
   *  @param trObj Object with Track data
   *  @param Z     Z of the State
   *  @param zExtr Z for extrapolation 
   *  @return standard status code
   */
  StatusCode findState
  ( const LHCb::Track* trObj , 
    const double         Z     , 
    const double         zExtr ) const ;

  /** Find State on specified Z.
   *  @param trObj Object with Track data
   *  @param Z     Z of the State
   *  @param zExtr Z for extrapolation 
   *  @param  covX allowed X-precision  (sigma**2)
   *  @param  covY allowed Y-precision  (sigma**2)
   *  @return standard status code
   */
  StatusCode findState
  ( const LHCb::Track* trObj , 
    const double         Z     , 
    const double         zExtr , 
    const double         covX  ,
    const double         covY  ) const ;

protected: 
  
  /// internal type type of parameters for mathematical functions
  struct Match3D
  { 
    // parameters x,y, (e)
    Gaudi::Vector3 params   ;
    // covariance matrix x,y,(e)
    Gaudi::SymMatrix3x3 cov      ;    
    // error flag 
    int          error    ;
    // inversion flag 
    bool         inverted ;
    // constructor 
    Match3D 
    ( const Gaudi::Vector3  & p , 
      const Gaudi::SymMatrix3x3 & m ) 
      : params   (   p   )
      , cov      (   m   ) 
      , error    (   0   ) 
      , inverted ( false ) {} ;
    // constructor 
    Match3D () 
      : params   (       ) 
      , cov      (       ) 
      , error    (   0   ) 
      , inverted ( false ) {} ;
    // inversion
    int invert() 
    {
      if ( inverted ) { return error ; }
      error    = 0 ;
      bool ok = cov.Sinvert ( ) ;
      if(!ok)error=1;
      inverted = true ;
      return error    ;
    };
  };
  

  struct Match2D
  { 
    // parameters x,y, (e)
    Gaudi::Vector2 params   ;
    // covariance matrix x,y,(e)
    Gaudi::SymMatrix2x2 cov      ;    
    // error flag 
    int          error    ;
    // inversion flag 
    bool         inverted ;
    // constructor 
    Match2D
    ( const Gaudi::Vector2  & p , 
      const Gaudi::SymMatrix2x2 & m ) 
      : params   (   p   )
      , cov      (   m   ) 
      , error    (   0   ) 
      , inverted ( false ) {} ;
    // constructor 
    Match2D () 
      : params   (       ) 
      , cov      (       ) 
      , error    (   0   ) 
      , inverted ( false ) {} ;
    // inversion
    int invert() 
    {
      if ( inverted ) { return error ; }
      error    = 0 ;
      bool ok = cov.Sinvert ( ) ;
      if(!ok)error=1;
      inverted = true ;
      return error    ;
    };
  };
  
  
  
  // ==========================================================================
  /** chi2 method \n
   * implements a formula:
   * chi2 = dp1(T)*C1(-1)*dp1 + dp2(T)*C2(-1)*dp2 \n
   * where dpi = pi - mean. \n
   * Ci(-1) is the inverted covariance. \n
   * In case of failure throws a CaloException.
   * @param mt1 struct with first vector and its covariance
   * @param mt2 struct with second vector and its covariance
   * @return result chi2
   */

  // 3D
  inline double chi2
  ( const Match3D& mt1 , 
    const Match3D& mt2 ) const 
  {
    Assert ( mt1.inverted    && mt2.inverted                && 
             0 == mt1.error  &&  0 == mt2.error             && 
             "Matrix/Vector mismatch!" ) ;
    
    const Gaudi::SymMatrix3x3& icov1 = mt1.cov   ;
    const Gaudi::SymMatrix3x3& icov2 = mt2.cov   ;
    Gaudi::SymMatrix3x3&       cov   = m_aux_sym3D ;
    
    const int ifail = mtrxOp( cov , icov1 , icov2 ) ;
    Assert ( 0 == ifail  , "Can not invert the matrix !" );
    
    const Gaudi::Vector3 vmean ( cov * ( icov1 * mt1.params + icov2 * mt2.params ) );
    
    return 
      Similarity(icov1, vmean - mt1.params) + Similarity(icov2, vmean - mt2.params);
  };

  // 2D
  inline double chi2
  ( const Match2D& mt1 , 
    const Match2D& mt2 ) const 
  {
    
    Assert ( mt1.inverted    && mt2.inverted                && 
             0 == mt1.error  &&  0 == mt2.error             && 
             "Matrix/Vector mismatch!" ) ;
    
    const Gaudi::SymMatrix2x2& icov1 = mt1.cov   ;
    const Gaudi::SymMatrix2x2& icov2 = mt2.cov   ;
    Gaudi::SymMatrix2x2&       cov   = m_aux_sym2D ;
    
    const int ifail = mtrxOp( cov , icov1 , icov2 ) ;
    Assert ( 0 == ifail  , "Can not invert the matrix !" );
    const Gaudi::Vector2 vmean ( cov * ( icov1 * mt1.params + icov2 * mt2.params ) );
    return 
      Similarity(icov1, vmean - mt1.params) + Similarity(icov2, vmean - mt2.params);
  };
  
  // ==========================================================================
  
  // 3D
  inline int mtrxOp
  ( Gaudi::SymMatrix3x3&       cov ,
    const Gaudi::SymMatrix3x3& mx1 , 
    const Gaudi::SymMatrix3x3& mx2 ) const 
  {
    cov = mx1 + mx2 ;
    return cov.Sinvert ( )  ? 0 : 1  ; 
  };

  // 2D
  inline int mtrxOp
  ( Gaudi::SymMatrix2x2&       cov ,
    const Gaudi::SymMatrix2x2& mx1 , 
    const Gaudi::SymMatrix2x2& mx2 ) const 
  {
    cov = mx1 + mx2 ;
    return cov.Sinvert ( )  ? 0 : 1  ; 
  };

protected:
  
  /** return the decoded track bits pattern
   *  @param track track objects 
   *  @return track bits pattern 
   */
  std::string bits ( const LHCb::Track* track ) const ;
  
protected:
  
  /** accessor to extrapolator 
   *  @return pointer to track extrapolator
   */
  ITrackExtrapolator* extrapolator() const { return m_extrapolator; }
  
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
  mutable LHCb::State*             m_state     ;
  
  // the previous stored track   
  mutable const LHCb::Track* m_prevTrack ;  
  
  // default 'bad' value 
  double                       m_bad           ;
  
  // optmization for extrapolators is ON/OFF
  bool                         m_optimized     ;
  
private: 
  
  // allowed interval for closest Z 
  double                       m_zmin      ;
  double                       m_zmax      ;
  
  // particle ID to be used for extrapolation 
  int                          m_pdgID     ;
  mutable LHCb::ParticleID           m_pid       ;
  
  // interface to track extrapolator
  
  ITrackExtrapolator*             m_extrapolator      ;    
  
  // and the extrapolator name
  std::string                  m_extrapolatorName  ;
  
  // allowed tolerance factor in X/Y (relative!) 
  double                       m_precision         ;
  // extrapolation tolerance in Z 
  double                       m_tolerance         ;

  // local storages 
  mutable Gaudi::SymMatrix2x2  m_aux_sym2D  ;
  mutable Gaudi::SymMatrix3x3  m_aux_sym3D  ;
};
// ============================================================================
#endif // CALORECO_CALOTRACKMATCHBASE_H
