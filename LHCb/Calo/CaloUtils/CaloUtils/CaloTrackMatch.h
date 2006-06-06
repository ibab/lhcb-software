// $Id: CaloTrackMatch.h,v 1.1 2006-06-06 11:59:52 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOUTILS_CALOTRACKMATCH_H 
#define CALOUTILS_CALOTRACKMATCH_H 1
// ============================================================================
// Include files
// ============================================================================
// LHCbDefinition/ROOT
// ============================================================================
#include "Kernel/GenericVectorTypes.h"
#include "Kernel/SymmetricMatrixTypes.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/CaloPosition.h"
#include "Event/State.h"
// ============================================================================
// Local
// ============================================================================
#include "CaloUtils/CaloTrackTool.h"
// ============================================================================

// ============================================================================
/** @class CaloTrackMatch CaloTrackMatch.h
 *  
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-05-28
 */
// ============================================================================
class CaloTrackMatch : public Calo::CaloTrackTool 
{
protected:
  /// Standard constructor
  CaloTrackMatch
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface* parent ) ;
  /// virtual and protected destcrutor 
  virtual ~CaloTrackMatch();
protected:
  /// helper internal structure to simplify matrix calculations 
  template <unsigned D> 
  class Match_
  {
  public:
    typedef ROOT::Math::SVector<double,D>          Vector ;
    typedef ROOT::Math::SMatrix<
      double,D,D,ROOT::Math::MatRepSym<double,D> > Matrix ;
  public:
    /// constructor 
    Match_ ( const Vector&  params , 
             const Matrix&  matrix ) 
      : m_params   ( params ) 
      , m_matrix   ( matrix ) 
      , m_error    ( false  ) 
      , m_inverted ( false  ) 
    {} ;
    Match_ () 
      : m_params   (       ) 
      , m_matrix   (       ) 
      , m_error    ( false ) 
      , m_inverted ( false ) 
    {} ;
  public:
    inline const Vector& params   () const { return m_params   ; }
    inline const Matrix& matrix   () const { return m_matrix   ; }
    inline       bool    error    () const { return m_error    ; }
    inline       bool    inverted () const { return m_inverted ; }
  public:
    double& operator()
      ( const unsigned int ind   ) { return m_params ( ind         ) ; }
    double& operator() 
      ( const unsigned int ind1  ,
        const unsigned int ind2  ) { return m_matrix ( ind1 , ind2 ) ; }
  public:
    Match_& set         ( const Matrix& m ) { m_matrix   = m ; return *this ; }
    Match_& set         ( const Vector& v ) { m_vector   = v ; return *this ; }
    Match_& setError    ( const bool    e ) { m_error    = e ; return *this ; }
    Match_& setInverted ( const bool    i ) { m_inverted = i ; return *this ; }
  public:
    // invert the matrix 
    int invert() 
    {
      if ( m_inverted   ) { return m_error    ; }    // RETURN 
      m_error = m_matrix.Sinvert () ;
      if ( m_error ) { m_inverted = true ; }
      return error() ;
    } ;
  private: 
    Match_           ( const Match_& ) ;
    Match_& operator=( const Match_& ) ;
  private:
    // the vector of parameters x,y(,e)
    Vector m_params   ;
    // the (inverse) covariance matrix of parameters 
    Matrix m_matrix   ;
    // flag for errors 
    int    m_error    ;
    // flag to indicate that matrix is already inverted
    bool   m_inverted ;
  } ;  
protected:
  template <unsigned int D>
  inline double chi2 
  ( const Match_<D>& m1 ,
    const Match_<D>& m2 ) const 
  {
    typedef typename Match_<D>::Vector Vector ;
    typedef typename Match_<D>::Matrix Matrix ;
    // check input data 
    Assert ( m1.inverted() && 0 == m1.error() && 
             m2.inverted() && 0 == m2.error()     , 
             "chi2(): invalid data are detected " ) ;
    // local storage to avoid the dynamic allocation 
    static Matrix s_cov ;
    // evaluate the overall covariance matrix 
    s_cov = m1.matrix() +  m2.matrix() ;
    Assert ( s_cov.Sinvert() , 
             "chi2(): Can not invert the matrix !" );
    // get the weighted and mean parameters 
    Vector pw = m1.matrix()*m1.params() + m2.matrix()*m2.params() ;
    Vector pm = s_cov * pw ;
    // evaluate chi2 
    return 
      ROOT::Math::Similarity ( pm - m1.params() , m1.matrix() ) +
      ROOT::Math::Similarity ( pm - m2.params() , m2.matrix() ) ;
  };
  /// get 2D-infomration form CaloPosition 
  inline StatusCode fill 
  ( const LHCb::CaloPosition& c , Match_<2>& match ) const 
  {   
    const LHCb::CaloPosition::Center& par = c.center() ;
    const LHCb::CaloPosition::Spread& cov = c.spread() ;
    match ( 0     ) = par ( 0     ) ;
    match ( 1     ) = par ( 1     ) ; 
    match ( 0 , 0 ) = cov ( 0 , 0 ) ;
    match ( 0 , 1 ) = cov ( 0 , 1 ) ;
    match ( 1 , 1 ) = cov ( 1 , 1 ) ;
    match.setInverted ( false ) ;
    match.setError    ( 0     ) ;    
    if ( match.invert() ) 
    { return Error ( "match(): Could not invert '2D-calo' matrix") ; }
    return StatusCode::SUCCESS ;
  } ;
  // get 2D-information from State 
  inline StatusCode fill 
  ( const LHCb::State& s , Match_<2>& match ) const 
  {
    const Gaudi::TrackVector&    par = s.stateVector() ;
    match ( 0     ) = par ( 0 ) ;
    match ( 1     ) = par ( 1 ) ;
    const Gaudi::TrackSymMatrix& cov = s.covariance () ;
    match ( 0 , 0 ) = cov ( 0 , 0 ) ;
    match ( 0 , 1 ) = cov ( 0 , 1 ) ;
    match ( 1 , 1 ) = cov ( 1 , 1 ) ;
    match.setInverted ( false ) ;
    match.setError    ( 0     ) ;    
    if ( 0 != match.invert() ) 
    { return Error ( "match(): Could not invert 'track' matrix") ; }
    return StatusCode::SUCCESS ;
  } ;
  /// get 3D-infomration form CaloPosition 
  inline StatusCode fill 
  ( const LHCb::CaloPosition& c , Match_<3>& match ) const 
  {   
    const LHCb::CaloPosition::Parameters& par = c.parameters() ;
    const LHCb::CaloPosition::Covariance& cov = c.covariance() ;
    match ( 0     ) = par ( LHCb::CaloPosition::X ) ;
    match ( 1     ) = par ( LHCb::CaloPosition::Y ) ; 
    match ( 2     ) = par ( LHCb::CaloPosition::E ) ; 
    match ( 0 , 0 ) = cov ( LHCb::CaloPosition::X , LHCb::CaloPosition::X ) ;
    match ( 0 , 1 ) = cov ( LHCb::CaloPosition::X , LHCb::CaloPosition::Y ) ;
    match ( 0 , 2 ) = cov ( LHCb::CaloPosition::X , LHCb::CaloPosition::E ) ;
    match ( 1 , 1 ) = cov ( LHCb::CaloPosition::Y , LHCb::CaloPosition::Y ) ;
    match ( 1 , 2 ) = cov ( LHCb::CaloPosition::Y , LHCb::CaloPosition::E ) ;
    match ( 2 , 2 ) = cov ( LHCb::CaloPosition::E , LHCb::CaloPosition::E ) ;
    match.setInverted ( false ) ;
    match.setError    ( 0     ) ;    
    if ( match.invert() ) 
    { return Error ( "match(): Could not invert '3D-calo' matrix") ; }
    return StatusCode::SUCCESS ;
  } ;
  // get 3D-information from State 
  inline StatusCode fill 
  ( const LHCb::State& s , Match_<3>& match ) const 
  {
    const Gaudi::TrackVector&    par = s.stateVector() ;
    match ( 0     ) =                par ( 0 ) ;
    match ( 1     ) =                par ( 1 ) ;
    match ( 2     ) = ::fabs ( 1.0 / par ( 4 ) ) ; /// @todo check it!
    const double q  = 0 < par(4) ? 1. : -1.    ;        // charge 
    const double f  = -1.0 * q / par ( 4 ) / par( 4 ) ; // d(p)/d(Q/p)
    const Gaudi::TrackSymMatrix& cov = s.covariance () ;
    match ( 0 , 0 ) =     cov ( 0 , 0 )     ; // (x,x) 
    match ( 0 , 1 ) =     cov ( 0 , 1 )     ; // (x,y) 
    match ( 1 , 1 ) =     cov ( 1 , 1 )     ; // (y,y) 
    match ( 0 , 2 ) = f * cov ( 0 , 4 )     ; // (x,p)
    match ( 1 , 2 ) = f * cov ( 1 , 4 )     ; // (y,p)
    match ( 2 , 2 ) = f * cov ( 4 , 4 ) * f ; // (p,p) 
    match.setInverted ( false ) ;
    match.setError    ( 0     ) ;    
    if ( 0 != match.invert() ) 
    { return Error ( "match(): Could not invert 'track' matrix") ; }
    return StatusCode::SUCCESS ;
  } ;
  
protected:
  inline       double        bad   () const { return m_bad   ; }
  inline       LHCb::State& _state ()       { return m_state ; }
  inline const LHCb::State& _state () const { return m_state ; }
private:
  /// bad value for chi2 
  double      m_bad   ;
  LHCb::State m_state ;
} ;

// ============================================================================
// The END 
// ============================================================================
#endif // CALOUTILS_CALOTRACKMATCH_H
// ============================================================================
