// $Id: CaloTrackMatchBase.h,v 1.2 2004-02-17 12:08:10 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/11/13 20:46:43  ibelyaev
// new package 
//
// ============================================================================
#ifndef CALOTRACKTOOLS_CALOTRACKMATCHBASE_H 
#define CALOTRACKTOOLS_CALOTRACKMATCHBASE_H 1
// Include files
#include <functional>
// CLHEP 
#include "CLHEP/Units/SystemOfUnits.h"
// GaudiKernel 
#include "GaudiKernel/IIncidentListener.h"
// CaloInterfaces 
#include "CaloInterfaces/ICaloTrackMatch.h"
// CaloKernel
#include "CaloKernel/CaloTool.h"

template <class TOOL>
class ToolFactory     ; ///< from GaudiKernel
class IIncidentSvc    ; ///< from GaudiKernel  
class ITrExtrapolator ; ///< from TrKernel

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"

class CaloPosition;
class TrStoredTrack;
class TrStateP;

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
  
  /** internal type
   *  type of parameters for mathematical functions
   */
  struct MatchStruct
  { 
    // parametrs x,y, (e) 
    HepVector    params;
    // covarinace matrix x,y,(e)
    HepSymMatrix cov;    
    // constructor 
    MatchStruct( const HepVector    & p , 
                 const HepSymMatrix & m ) : params( p ) , cov( m ) {};
  };
  
  /// internal type for mathematical functions 
  typedef MatchStruct MatchType;
  
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
    int ifail  = 0 ;
    int ifail1 = 0 ;
    int ifail2 = 0 ;
    
    Assert( mt1.cov    .num_row() == mt2.cov    .num_row() && 
            mt1.params .num_row() == mt2.params .num_row() , 
            "Matrix/Vector mismatch!");
    
    HepSymMatrix icov1( mt1.cov      .inverse( ifail1 ) );
    Assert( 0 == ifail1 , "Can not invert mt1 matrix !" );
    
    HepSymMatrix icov2( mt2.cov      .inverse( ifail2 ) );
    Assert( 0 == ifail2 , "Can not invert mt2 matrix !" );
    
    HepSymMatrix cov  ( (icov1+icov2).inverse( ifail  ) );
    Assert( 0 == ifail  , "Can not invert the matrix !" );
    
    const HepVector vmean( cov * ( icov1 * mt1.params + icov2 * mt2.params ) );
    
    const HepVector dv1( vmean - mt1.params );
    const HepVector dv2( vmean - mt2.params );
    
    return icov1.similarity( dv1 ) + icov2.similarity( dv2 );    
  }
  
  
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
  
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
#endif // CALOTRACKTOOLS_CALOTRACKMATCHBASE_H
// ============================================================================
