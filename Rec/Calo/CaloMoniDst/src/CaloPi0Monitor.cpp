// $Id: CaloPi0Monitor.cpp,v 1.2 2005-05-08 09:58:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IHistogramSvc.h"
// ============================================================================
// AIDA
// ============================================================================
#include "AIDA/IHistogram1D.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/CaloHypo.h"
// ============================================================================
// CaloUtils
// ============================================================================
#include "CaloUtils/dirHbookName.h"
// ============================================================================
// local
// ============================================================================
#include "CaloMoniAlg.h"
// ============================================================================

/** @class CaloPi0Monitor CaloPi0Monitor.cpp
 *
 *  Simple pi0 monitoring algorithm
 *  It produces 7 histograms:
 *  <ol>
 *  <li> "Raw" mass distribution of 2 photons </li>
 *  <li> Mass distribution of 2 photons after     Pt cut for each photon </li>
 *  <li> Mass distribution of 2 photons after 0.5*Pt cut for each photon </li>
 *  <li> Mass distribution of 2 photons after 2.0*Pt cut for each photon </li>
 *  <li> Mass distribution of 2 photons after     Pt cut for combination </li>
 *  <li> Mass distribution of 2 photons after 0.5*Pt cut for combination </li>
 *  <li> Mass distribution of 2 photons after 2.0*Pt cut for combination </li>
 *  </ol>
 *
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/11/2001
 */
class CaloPi0Monitor : public CaloMoniAlg
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloPi0Monitor>;
public:
  /// standard algorithm initialization
  virtual StatusCode initialize()
  {
    StatusCode sc = CaloMoniAlg::initialize();
    if( sc.isFailure() ) { return sc ; }
    book ( 1 , " Gamma-Gamma Mass "                  ,
           0 ,  600                                  ) ;
    book ( 2 , " Gamma-Gamma Mass (    PtGamma cut)" ,
           0 ,  600                                  ) ;
    book ( 3 , " Gamma-Gamma Mass (0.5xPtGamma cut)" ,
           0 ,  600                                  ) ;
    book ( 4 , " Gamma-Gamma Mass (2.5xPtGamma cut)" ,
           0 ,  600                                  ) ;
    book ( 5 , " Gamma-Gamma Mass (    PtPi0   cut)" ,
           0 ,  600                                  ) ;
    book ( 6 , " Gamma-Gamma Mass (0.5xPtPi0   cut)" ,
           0 ,  600                                  ) ;
    book ( 7 , " Gamma-Gamma Mass (2.5xPtPi0   cut)" ,
           0 ,  600                                  ) ;
    return StatusCode::SUCCESS;
  };
  /// standard algorithm execution
  virtual StatusCode execute   ();
protected:
  /** Standard constructor
   *  @param   name   algorithm name
   *  @param   svcloc pointer to service locator
   */
  CaloPi0Monitor
  ( const std::string& name   ,
    ISvcLocator*       svcloc )
    : CaloMoniAlg ( name , svcloc )
    , m_ptPhoton  ( 250 * MeV  )
    , m_ptPi0     ( 500 * MeV  )
  {
    declareProperty ( "PhotonPt" , m_ptPhoton         ) ;
    declareProperty ( "Pi0Pt"    , m_ptPi0            ) ;
    // set the appropriate default for input data
    addToInputs     ( CaloHypoLocation::      Photons ) ;
    addToInputs     ( CaloHypoLocation:: SplitPhotons ) ;
  };
  /// destructor (virtual and protected)
  virtual ~CaloPi0Monitor(){};
private:
  /// default  construstor  is  private
  CaloPi0Monitor();
  /// copy     construstor  is  private
  CaloPi0Monitor
  ( const CaloPi0Monitor& );
  /// assignement operator  is  private
  CaloPi0Monitor& operator=
  ( const CaloPi0Monitor& );
private:
  // cut on Photon Pt
  double         m_ptPhoton        ;
  // cut on Pi0    Pt
  double         m_ptPi0           ;
};
// ============================================================================

// ============================================================================
/** @var CaloPi0MonitorFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloPi0Monitor>         s_Factory ;
const        IAlgFactory&CaloPi0MonitorFactory = s_Factory ;
// ============================================================================

// ============================================================================
/** standard algorithm execution
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code
 */
// ============================================================================
StatusCode CaloPi0Monitor::execute()
{
  // avoid long names
  typedef const CaloHypo                Photon  ;
  typedef const CaloHypos               Hypos   ;
  typedef std::vector<const CaloHypo*>  Photons ;
  typedef Photons::const_iterator       photon  ;
  
  if ( !produceHistos() ) { return StatusCode::SUCCESS ; }
  
  if ( inputs().empty() ) 
  { return Error ( "No input data are specified" ) ; }
  
  Photons photons ;
  for ( Inputs::const_iterator input = inputs().begin() ; 
        inputs().end() != input ; ++input ) 
  {
    // get input data
    const Hypos* hypos = get<Hypos>( *input );
    if ( 0 == hypos ) { return StatusCode::FAILURE ; }
    photons.insert ( photons .  end   () , 
                     hypos   -> begin () , 
                     hypos   -> end   () ) ; 
  }
  
  if ( photons.empty() ) 
  { return Warning ( "No Photons arte found!", StatusCode::SUCCESS ) ; }
  
  AIDA::IHistogram1D* h1 = histo ( 1 ) ;
  AIDA::IHistogram1D* h2 = histo ( 2 ) ;
  AIDA::IHistogram1D* h3 = histo ( 3 ) ;
  AIDA::IHistogram1D* h4 = histo ( 4 ) ;
  AIDA::IHistogram1D* h5 = histo ( 5 ) ;
  AIDA::IHistogram1D* h6 = histo ( 6 ) ;
  AIDA::IHistogram1D* h7 = histo ( 6 ) ;
  
  // loop over the first photon
  for ( photon g1 = photons.begin() ; photons.end() != g1 ; ++g1 )
  {
    // skip nulls
    if (     0 == *g1 || 0 == (*g1)->momentum() ) { continue ; }
    // loop over the second photon
    for ( photon g2 = g1 + 1 ; photons.end() != g2 ; ++g2 )
    {
      // skip nulls
      if ( 0 == *g2 || 0 == (*g2)->momentum() ) { continue ; }
      
      const HepLorentzVector& v1 = (*g1)->momentum()->momentum() ;
      const HepLorentzVector& v2 = (*g2)->momentum()->momentum() ;
      
      // 4-vector for pi0
      const HepLorentzVector pi0( v1 + v2 );
      
      // pi0 mass
      const double mass = pi0.m() ;
      
      hFill ( h1 ,  mass ) ;
      
      const double pt1 = v1  .perp () ;
      const double pt2 = v2  .perp () ;
      if( pt1 >       m_ptPhoton &&
          pt2 >       m_ptPhoton  ) { hFill ( h2 , mass ) ; }
      if( pt1 > 0.5 * m_ptPhoton &&
          pt2 > 0.5 * m_ptPhoton  ) { hFill ( h3 , mass ) ; }
      if( pt1 > 2.0 * m_ptPhoton &&
          pt2 > 2.0 * m_ptPhoton  ) { hFill ( h4 , mass ) ; }
      
      const double pt  = pi0 .perp () ;
      if ( pt  >       m_ptPi0    ) { hFill ( h5 , mass ) ; }
      if ( pt  > 0.5 * m_ptPi0    ) { hFill ( h6 , mass ) ; }
      if ( pt  > 2.0 * m_ptPi0    ) { hFill ( h7 , mass ) ; }
      
    }  
  }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The End
// ============================================================================
