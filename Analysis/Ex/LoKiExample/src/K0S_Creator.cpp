// $Id: K0S_Creator.cpp,v 1.1 2004-08-05 09:54:15 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/ParticleProperty.h"
// ============================================================================
// local
// ============================================================================
#include "K0S_Creator.h"
// ============================================================================

/** @file 
 *  
 * Implementation file for class : K0S_Creator
 * 
 *  @date 2004-07-24 
 *  @author Vanya BELYAEV Ivan.Belyaev@tep.ru
 */

// ============================================================================
// Declaration of the Algorithm Factorys
// ============================================================================
static const  AlgFactory<K0S_Creator>         F_factory ;
const        IAlgFactory&K0S_CreatorFactory = F_factory ; 
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @see LoKi::Algo
 *  @param name algorithm instance name 
 *  @param pSvc pointer to Service Locator 
 */
// ============================================================================
K0S_Creator::K0S_Creator 
( const std::string& name ,
  ISvcLocator*       pSvc )
  : LoKi::Algo ( name , pSvc )
  //
  // default for DD Kshorts, could be retuned for each analysis
  // different cuts for DD, LL and LV Kshorts exist in option files
  //
    , m_KsTypeName         ( "DDKs"      ) 
    , m_ksmasswinti        ( 0.1 * GeV   )
    , m_ksmasswinloo       ( 0.3 * GeV   )
    , m_ksZmin             ( 0.0 * cm    ) 
    , m_ksZmax             ( 3000.0 * cm ) 
    , m_chi2UncVtxCut      ( 50          ) 
    , m_ptCut              ( 0.25 * GeV  ) 
    , m_impParamPiCutL     ( 6.0         )
    , m_impParamPiCutV     ( 6.0         )
    , m_impParamPiCutD     ( 3.0         )
    , m_chi2Forward        ( 3.0         ) 
    , m_chi2Match          ( 3.0         ) 
    , m_deForward          ( -2.0        ) 
    , m_pForward           ( 3000.0      ) 
  ///
{
  setProperty     ( "AvoidSelResult"  , "true"             ) ;
  setProperty     ( "DecayDescriptor" , "KS0 -> pi+ pi-"   ) ;
  
  //
  declareProperty ( "KsTypeName"        , m_KsTypeName     ) ; 
  declareProperty ( "KsMassWindowLoose" , m_ksmasswinloo   ) ;
  declareProperty ( "KsMassWindowTight" , m_ksmasswinti    ) ;
  declareProperty ( "KsZMin"            , m_ksZmin         ) ;
  declareProperty ( "KsZMax"            , m_ksZmax         ) ;
  declareProperty ( "Chi2UncFitCut"     , m_chi2UncVtxCut  ) ;
  declareProperty ( "Pt"                , m_ptCut          ) ;

  declareProperty ( "MinImpParamPiL"    , m_impParamPiCutL ) ;
  declareProperty ( "MinImpParamPiV"    , m_impParamPiCutV ) ;
  declareProperty ( "MinImpParamPiD"    , m_impParamPiCutD ) ;

  declareProperty ( "chi2MaxForward"    , m_chi2Forward    ) ;
  declareProperty ( "chi2MaxMatch"      , m_chi2Match      ) ;
  declareProperty ( "deMaxForward"      , m_deForward      ) ;
  declareProperty ( "pMinForward"       , m_pForward       ) ;
  
};
// ============================================================================


// ============================================================================
/** standard initializationfo algorihtm 
 *  @return status code 
 */
// ============================================================================
StatusCode K0S_Creator::initialize()
{
  StatusCode sc = LoKi::Algo::initialize(); 
  if ( sc.isFailure() ) { return sc ; }   

  // check the type os Ks 
  m_KsType = Unknown ;
  
  if       ( "LLKs" ==  m_KsTypeName ) { m_KsType = LLKs ; }
  else if  ( "LVKs" ==  m_KsTypeName ) { m_KsType = LVKs ; }
  else if  ( "LUKs" ==  m_KsTypeName ) { m_KsType = LVKs ; }
  else if  ( "ULKs" ==  m_KsTypeName ) { m_KsType = LVKs ; }
  else if  ( "LDKs" ==  m_KsTypeName ) { m_KsType = LDKs ; }
  else if  ( "DLKs" ==  m_KsTypeName ) { m_KsType = LDKs ; }
  else if  ( "DDKs" ==  m_KsTypeName ) { m_KsType = DDKs ; }
  else 
  { return Error ( "Invalid Ks type '" + m_KsTypeName + "'") ; }
  
  if ( Unknown == m_KsType ) 
  { return Error ( "Invalid Ks type '" + m_KsTypeName + "'") ; } 
  
  const ParticleProperty* pp = LoKi::Particles::ppFromName ( "KS0" ) ;
  if ( 0 == pp ) 
  { return Error ( "Invalid ParticleProperty* for 'KS0' ") ; }
  
  m_KsMass  = pp->mass() ;
  
  return StatusCode::SUCCESS;
};
// ============================================================================


// ============================================================================
/** analyse one event 
 *  @return status code 
 */
// ============================================================================
StatusCode K0S_Creator::analyse    () 
{
  using namespace LoKi        ;
  using namespace LoKi::Cuts  ;
  using namespace LoKi::Fits  ;
  
  const StatusCode OK = StatusCode::SUCCESS ;
  
  // select all primary vertices 
  VRange primaries = vselect ( "PVs" , Vertex::Primary == VTYPE ) ;
  if ( primaries.empty() ) 
  { return Warning ( "No primary vertex is found !" , OK ) ; }
  
  // minimal value of impact parameter chi2 with respect 
  // to all primary vertices 
  Fun mip = MIPCHI2 ( primaries, geo() ) ;
  
  // impact parameter cut for       long pions 
  Cut ipL =  TRISLONG && mip > ( m_impParamPiCutL * m_impParamPiCutL ) ;
  // impact parameter cut for   upstream pions 
  Cut ipU =  TRISUP   && mip > ( m_impParamPiCutV * m_impParamPiCutV ) ;
  // impact parameter cut for downstream pions 
  Cut ipD =  TRISDOWN && mip > ( m_impParamPiCutD * m_impParamPiCutD ) ;
  
  // all track categories togather 
  Cut tracks = ipL || ipU || ipD ;
  
  Range piplus  = select ( "pi+" , "pi+" == ID && tracks ) ;
  Range piminus = select ( "pi-" , "pi-" == ID && tracks ) ;
  
  for ( Loop ks = loop ( "pi+ pi-" , "KS0" , FitVertex ) ; ks ; ++ks ) 
  {
    const Particle* pi1 = ks(1) ;
    if ( 0 == pi1 || goodTrack ( pi1 ) ) { continue ; }     // CONTINUE 
    
    const Particle* pi2 = ks(2) ;
    if ( 0 == pi2 || goodTrack ( pi2 ) ) { continue ; }     // CONTINUE

    if ( !rightPair( pi1 , pi2 ) )           { continue ; }     // CONTINUE
    
    // make preliminary mass estimate 
    if ( fabs ( ks -> mass (1,2) - m_KsMass ) > m_ksmasswinloo ){ continue ; }
      
    // make unconstraied vertex fit 
    StatusCode sc = ks -> fit ( FitVertex ) ;
    if ( sc.isFailure() || !ks )             { continue ; }    // CONTINUE 
    
    // cut on chi2 of vertex fit 
    const double vChi2 = VCHI2 ( ks ) ;
    plot ( vChi2 , 21 , "Chi2 of unconstrained Ks vertex fit" , 0 , 20 ) ;
    if ( vChi2 > m_chi2UncVtxCut )           { continue ; }    // CONTINUE
    
    // cut on minimal z-position of Ks vertex 
    const double z = VZ( ks ) ;
    if ( z < m_ksZmin )                      { continue ; }    // CONTINUE
    
    // cut on minimal Ks transverse momentum 
    const double pt = PT( ks ) ;
    plot ( pt / GeV , 23 , "pt of Ks" , 0 , 10 );
    if ( pt < m_ptCut )                      { continue ; }    // CONTINUE
    
    // cut on maximal z-position of Ks vertex 
    plot ( z , 24 , "z of Ks vertex" , 
           m_ksZmin , m_ksZmax , 
           (int) ( fabs ( m_ksZmax - m_ksZmin ) / 5 ) ) ;
    if ( z > m_ksZmax )                      { continue ; }    // CONTINUE
    
    // get the 'measured mass' 
    const double mass = MM ( ks ) ;
    plot ( mass / GeV , 25 , "Ks Mass", 0.4 , 0.6 ) ;
    
    if ( fabs ( mass - m_KsMass ) > m_ksmasswinti  )      { continue ; }
    
    plot ( mass / GeV , 27 , "Mass of 2 Pions Combination" , 0.4 , 0.6 ) ;
    
    // save Ks 
    ks -> save( "K0S" ) ;
    
  };


  return StatusCode::SUCCESS ;
} ;
// ============================================================================

// ============================================================================
/** check the track quality,
 *  indeed it is  LoKi-version of Ks2PiPi::trackQlt 
 */
// ============================================================================
bool K0S_Creator::goodTrack ( const Particle* p ) const
{
  using namespace LoKi::Cuts ;
  
  if ( 0 == p ) { return false ; }
  
  if      ( TRFORWARD ( p ) ) 
  {
    const double chi2    = TCHI2NDF ( p ) ;    
    if ( chi2 < m_chi2Forward )                   { return false ; }  // RETURN
    
    const int    detflag = detFlag  ( p ) ;
    if ( -1 == detflag  && P ( p ) < m_pForward ) { return false ; } // RETURN 
    if ( -2 == detflag  || -4 == detflag ) 
    {
      const double de    = PIDe     ( p ) ;
      if ( de > m_deForward )                     { return false ; } // RETURN
    }
  }
  else if ( TRMATCH   ( p ) ) 
  {
    const double chi2    = TCHI2NDF ( p ) ; 
    if ( chi2 < m_chi2Match )                     { return false ; }  // RETURN
    
    const int    detflag = detFlag  ( p ) ;
    if ( 0 > detflag        )                     { return false ; }  // RETURN 
  }

  return true ;
};
// ============================================================================

// ============================================================================
/// helper function for K0S_Creator::goodTrack implementation
// ============================================================================
int K0S_Creator::detFlag( const Particle* p ) const 
{
  using namespace LoKi::Cuts ;
  using namespace LoKi::Cuts ;
  
  int detflag = -1 ;
  if ( PPBITCALO ( p ) ) { detflag -=         1 ; }
  if ( PPBITMUON ( p ) ) { detflag -=         2 ; }
  if ( PPBITRICH ( p ) ) { detflag  =  -detflag ; }
  return detflag ;
};

// ============================================================================
/// check the consistensy the track types with Ks type 
// ============================================================================
bool K0S_Creator::rightPair 
( const Particle* pi1  , 
  const Particle* pi2  , 
  const KsType    k0s  ) const 
{
  
  using namespace LoKi        ;
  using namespace LoKi::Cuts  ;
  
  if ( 0 == pi1 || 0 == pi2 ) { return false ; }  
  
  switch( k0s ) 
  {
  case LLKs : 
    return     TRISLONG ( pi1 ) && TRISLONG ( pi2 )     ;  break ;
  case LVKs :
    return ( ( TRISLONG ( pi1 ) && TRISUP   ( pi2 ) ) || 
             ( TRISUP   ( pi1 ) && TRISLONG ( pi2 ) ) ) ;  break ;
  case LDKs :
    return ( ( TRISLONG ( pi1 ) && TRISDOWN ( pi2 ) ) || 
             ( TRISDOWN ( pi1 ) && TRISLONG ( pi2 ) ) ) ;  break ;
  case DDKs :
    return     TRISDOWN ( pi1 ) && TRISDOWN ( pi2 )     ;  break ;
  default   :  Error("rigthPair(): invalid case!")      ;  break ;
  }
  //
  return false ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
