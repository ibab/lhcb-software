// ============================================================================
#ifndef CALOUTILS_CALOTRACKMATCH_H 
#define CALOUTILS_CALOTRACKMATCH_H 1
// ============================================================================
// Include files
// ============================================================================
// Geometry Definitions (Gaudi+ROOT)
// ============================================================================
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/CaloPosition.h"
#include "Event/State.h"
// ============================================================================
// Local
// ============================================================================
#include "CaloTrackTool.h"
// ============================================================================
#include "DetDesc/Condition.h"
#include "Kernel/ILHCbMagnetSvc.h"
// #include "GaudiKernel/Kernel.h"
// ============================================================================
/** @class CaloTrackMatch CaloTrackMatch.h
 *  Description of the use of the track-cluster matching X-correction
 *  parameters for electrons and positrons.
 *
 *  The X-correction part of the code has been designed with the kind help from
 *  Vanya BELYAEV and Dmitry GOLUBKOV.
 *
 *  Common form of the X-correction between the X-position of the state
 *  of track and barycentre of the CaloHypo object is:
 *  dX = a0*p + a1 + a2/p + a3/p^2 + ...
 *  where p - momentum of the track in GeV/$c$,
 *  parameter vectors for each zone of the ECAL
 *  m_alphaN<Area>[3] = {a0, a1, a2, a3, ...}
 *  for e- MagUp and e+ MagDown: (q*polarity) < 0
 *  and
 *  m_alphaP<Area>[3] = {a0, a1, a2, a3, ...}
 *  for e- MagDown and e+ MagUp: (q*polarity) > 0
 *  
 *  By default the X-correction parameters are read from the CondDB
 *  path '/dd/Conditions/ParticleID/Calo/ElectronXCorrection'. 
 *  
 *  In the case when the CondDB is switched off or reading
 *  of the X-correction parameters from the CondDB is disabled
 *  the X-correction is not implemented and all X-correction parameters
 *  are equal to zero.
 *
 *  Usage of the X-correction parameters in Bender scripts:
 *  def configure ( ) :
 *      """
 *      Job configuration 
 *      """
 *      ...
 *      from Configurables import CaloElectronMatch
 *      my_tool = CaloElectronMatch()
 *      ## disable use of the CondDB to apply correction coefficients
 *      ## from the Options
 *      my_tool.ConditionName = ""
 *      ## Set the X-correction parameters for the Outer zone
 *      ## of the ECAL in case (q*polarity) < 0
 *      my_tool.AlphaNOut = [ 0.0, -18.92, 83.46, -292.4 ]
 *
 *  For more info see talk by O. Stenyakin
 *  at 2014/02/20 Moscow student meeting 
 *  https://indico.cern.ch/event/302695/
 *  or
 *  at 2014/01/24 Calo Objects meeting
 *  https://indico.cern.ch/event/296617/
 *
 *  @author Oleg STENYAKIN oleg.stenyakin@cern.ch
 *  @date   2014-03-03
 */
// ============================================================================
class CaloTrackMatch : public Calo::CaloTrackTool   , public virtual IIncidentListener{
public:
  /// initialization
  virtual StatusCode initialize() ;
  StatusCode i_updateAlpha();
  virtual void handle(const Incident& ) { 
    m_position = NULL  ; 
    m_plane=Gaudi::Plane3D();
  }
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
      , m_ok       ( true   ) 
      , m_inverted ( false  ) 
    {} ;
    Match_ () 
      : m_params   (       ) 
      , m_matrix   (       ) 
      , m_ok       ( true  ) 
      , m_inverted ( false ) 
    {} ;
  public:
    inline const Vector& params   () const { return m_params   ; }
    inline const Matrix& matrix   () const { return m_matrix   ; }
    inline       bool    ok       () const { return m_ok       ; }
    inline       bool    inverted () const { return m_inverted ; }
  public:
    double& operator()
      ( const unsigned int ind   ) { return m_params ( ind         ) ; }
    double& operator() 
      ( const unsigned int ind1  ,
        const unsigned int ind2  ) { return m_matrix ( ind1 , ind2 ) ; }
  public:
    Match_& set         ( const Matrix& m ) { m_matrix   = m ; return *this ; }
    Match_& set         ( const Vector& v ) { m_params   = v ; return *this ; }
    Match_& setOK       ( const bool    e ) { m_ok       = e ; return *this ; }
    Match_& setInverted ( const bool    i ) { m_inverted = i ; return *this ; }
  public:
    // invert the matrix 
    bool invert() 
    {
      if ( m_inverted   ) { return m_ok    ; }    // RETURN 
      m_ok = m_matrix.Invert () ;
      if ( m_ok         ) { m_inverted = true ; }
      return ok() ;
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
    bool   m_ok       ;
    // flag to indicate that matrix is already inverted
    bool   m_inverted ;
  } ;  
protected:
  inline bool updateCaloPos(const LHCb::CaloPosition* p1, const LHCb::CaloPosition* p2){ 
    return ( p1!=p2 ); 
  }

  template <unsigned int D>
  inline double chi2 
  ( const Match_<D>& m1 ,
    const Match_<D>& m2 ) const 
  {
    typedef typename Match_<D>::Vector Vector ;
    typedef typename Match_<D>::Matrix Matrix ;
    // check input data 
    //    Assert ( m1.inverted() && m1.ok() && 
    //         m2.inverted() && m2.ok()     , 
    //         "chi2(): invalid data are detected " ) ;

    if (  !m1.inverted() || !m1.ok() || !m2.inverted() || !m2.ok() ){ 
      Warning(  "chi2(): invalid data are detected - return bad chi2" ).ignore();
      return 99999999. ; 
    }


    // local storage to avoid the dynamic allocation 
    static Matrix s_cov ;
    // evaluate the overall covariance matrix 
    s_cov = m1.matrix() +  m2.matrix() ;
    if ( !s_cov.Invert() ){ 
      Warning(  "chi2(): can not invert the matrix - return bad chi2" ).ignore() ; 
      return 99999999. ; 
    }

    // get the weighted and mean parameters 
    Vector pw = m1.matrix()*m1.params() + m2.matrix()*m2.params() ;
    Vector pm = s_cov * pw ;

    // evaluate chi2 
    double temp =
      ROOT::Math::Similarity ( pm - m1.params() , m1.matrix() ) +
      ROOT::Math::Similarity ( pm - m2.params() , m2.matrix() ) ;
    return temp;
  }

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
    match.setOK       ( true  ) ;    
    if ( !match.invert() ){
      if ( msgLevel ( MSG::DEBUG ) ){ debug() << "CaloPosition:" << c << endmsg ; }
      return Warning ( "match(): Could not invert '2D-calo' matrix, see debug",StatusCode::FAILURE,0) ;
    }
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
    match.setOK       ( true  ) ;    
    if ( !match.invert() ) 
    { 
      if ( msgLevel ( MSG::DEBUG ) ) 
      { debug() << "State:" << s << endmsg ; }
      return Warning ( "match(): Could not invert 'track' matrix", StatusCode::FAILURE ) ; 
    }
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
    match.setOK       ( true  ) ;    
    if ( !match.invert() ) 
    { 
      if ( msgLevel ( MSG::DEBUG ) ) 
      { debug() << "CaloPosition:" << c << endmsg ; }
      return Warning ( "match(): Could not invert '3D-calo' matrix", StatusCode::FAILURE ) ; 
    }
    return StatusCode::SUCCESS ;
  } ;
  /// get 2D-infomration from CaloPosition for Bremstrahlung
  inline StatusCode fillBrem 
  ( const LHCb::CaloPosition& c , Match_<2>& match ) const 
  {   
    const LHCb::CaloPosition::Parameters& par = c.parameters() ;
    const LHCb::CaloPosition::Covariance& cov = c.covariance() ;
    match ( 0     ) = par ( LHCb::CaloPosition::X ) ;
    match ( 1     ) = par ( LHCb::CaloPosition::Y ) ; 
    match ( 0 , 0 ) = cov ( LHCb::CaloPosition::X , LHCb::CaloPosition::X ) ;
    match ( 0 , 1 ) = cov ( LHCb::CaloPosition::X , LHCb::CaloPosition::Y ) ;
    match ( 1 , 1 ) = cov ( LHCb::CaloPosition::Y , LHCb::CaloPosition::Y ) ;
    match.setInverted ( false ) ;
    match.setOK       ( true  ) ;    
    if ( !match.invert() ) 
    { 
      if ( msgLevel ( MSG::DEBUG ) ) 
      { debug() << "CaloPosition:" << c << endmsg ; }
      return Warning ( "match(): Could not invert '2D-calobrem' matrix", StatusCode::FAILURE ) ; 
    }
    return StatusCode::SUCCESS ;
  } ;
  // get 3D-information from State 
  inline StatusCode fill 
  ( const LHCb::State& s , Match_<3>& match ) const 
  {
    const Gaudi::TrackVector&    par = s.stateVector() ;
    const double q  = 0 < par(4) ? 1. : -1. ; // charge
    const double mom = ::fabs( 1.0/par(4)/Gaudi::Units::GeV ); // momentum in GeV

    // find calo area corresponding to the input LHCb::State &s;
    const CellParam* cell = calo()->Cell_( s.position() ) ; // cell parameters (null if point is outside the Calorimeter)
    unsigned int area = 4; // initialize with some invalid area number
    if ( cell ) // protection against tracks pointing outside the Calorimeter
      area = cell->cellID().area() ; // 0:Outer, 1:Middle, 2:Inner
    else // roughly assign the area around the beam hole to the Inner, everything outside Calo -- to the Outer
      area = ( fabs(s.position().x()) < 2.*Gaudi::Units::m && fabs(s.position().y()) < 2.*Gaudi::Units::m ) ? 2 : 0; // |x,y| < > 2m

    float polarity = m_magFieldSvc->isDown() ? -1 : +1 ;
    bool qpolarity = ( q * polarity > 0 ) ; // true : (q*polarity) > 0, false : (q*polarity) < 0

    const std::vector<double> *alpha = NULL;
    switch ( area ) // symbolic names only declaread as "the private part" of namespace CaloCellCode in CaloCellCode.cpp
    {
    case 0 : // Outer  ECAL
      alpha = qpolarity ? &m_alphaPOut : &m_alphaNOut;
      break;
    case 1 : // Middle ECAL
      alpha = qpolarity ? &m_alphaPMid : &m_alphaNMid;
      break;
    case 2 : // Inner  ECAL
      alpha = qpolarity ? &m_alphaPInn : &m_alphaNInn;
      break;
    }
    Assert( alpha, "electron track pointing to an impossible Calo area outside 0..2 range");    

    match ( 0 ) = par ( 0 ) ;
    // now add the correction series dX = a0*p + a1 + a2/p + a3/p^2 + ...
    if ( !alpha->empty() )
    {
      double mmm = mom; // p, 1, 1/p, 1/p^2, ...
      double inv = fabs( par(4)*Gaudi::Units::GeV ); // abs(1/p) in GeV

      for ( std::vector<double>::const_iterator it = alpha->begin(); it != alpha->end(); ++ it)
      {
        match ( 0 ) += (*it) * mmm ;
        mmm *= inv ; // [1/p]^k, k=-1, 0, ... size(alpha)-2
      }
    }

    match ( 1     ) =                par ( 1 ) ;
    match ( 2     ) = ::fabs ( 1.0 / par ( 4 ) ) ; /// @todo check it!
    const double f  = -1.0 * q / par ( 4 ) / par( 4 ) ; // d(p)/d(Q/p)
    const Gaudi::TrackSymMatrix& cov = s.covariance () ;
    match ( 0 , 0 ) =     cov ( 0 , 0 )     ; // (x,x) 
    match ( 0 , 1 ) =     cov ( 0 , 1 )     ; // (x,y) 
    match ( 1 , 1 ) =     cov ( 1 , 1 )     ; // (y,y) 
    match ( 0 , 2 ) = f * cov ( 0 , 4 )     ; // (x,p)
    match ( 1 , 2 ) = f * cov ( 1 , 4 )     ; // (y,p)
    match ( 2 , 2 ) = f * cov ( 4 , 4 ) * f ; // (p,p) 
    match.setInverted ( false ) ;
    match.setOK       ( true  ) ;    
    if ( !match.invert() ) 
    {
      if ( msgLevel ( MSG::DEBUG ) ) 
      { debug() << "State:" << s << endmsg ; }
      return Warning ( "match(): Could not invert 'track' matrix", StatusCode::FAILURE ) ; 
    }
    return StatusCode::SUCCESS ;
  } ;  
protected:
  inline       double        bad    () const { return m_bad   ; }
  inline       LHCb::State& _state  ()       { return m_state ; }
  inline const LHCb::State& _state  () const { return m_state ; }

  Condition * m_cond;
  std::string m_conditionName;
  ILHCbMagnetSvc * m_magFieldSvc;
protected:
  const LHCb::CaloPosition* m_position ;
  Gaudi::Plane3D            m_plane    ;
private:
  /// bad value for chi2 
  double       m_bad       ;
  LHCb::State  m_state     ;
  std::vector<double> m_alphaPOut ; // (q*polarity) > 0, Outer  ECAL
  std::vector<double> m_alphaNOut ; // (q*polarity) < 0, Outer  ECAL
  std::vector<double> m_alphaPMid ; // (q*polarity) > 0, Middle ECAL
  std::vector<double> m_alphaNMid ; // (q*polarity) < 0, Middle ECAL
  std::vector<double> m_alphaPInn ; // (q*polarity) > 0, Inner  ECAL
  std::vector<double> m_alphaNInn ; // (q*polarity) < 0, Inner  ECAL  
} ;

// ============================================================================
// The END 
// ============================================================================
#endif // CALOUTILS_CALOTRACKMATCH_H
// ============================================================================
