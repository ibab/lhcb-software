// $Id: CaloPi0Monitor.cpp,v 1.1.1.1 2004-10-25 09:00:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2004/02/17 12:00:58  ibelyaev
//  add new algorithm and update for new base
//
// Revision 1.5  2002/12/01 14:26:29  ibelyaev
//  version update
//
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IHistogramSvc.h"
// AIDA 
#include "AIDA/IHistogram1D.h" 
// Event 
#include "Event/CaloHypo.h"
// CaloUtils 
#include "CaloUtils/dirHbookName.h"
// local
#include "CaloPi0Monitor.h"

// ============================================================================
/** @file CaloPi0Monitor.cpp
 * 
 *  Template implementation file for class : CaloPi0Monitor
 *  @see CaloPi0Monitor
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 02/11/2001
 */
// ============================================================================

// ============================================================================
/** @var CaloPi0MonitorFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloPi0Monitor>         s_Factory ;
const        IAlgFactory&CaloPi0MonitorFactory = s_Factory ;

// ============================================================================
/** Standard constructor
 *  @param   name   algorithm name 
 *  @param   svcloc pointer to service locator 
 */
// ============================================================================
CaloPi0Monitor::CaloPi0Monitor
( const std::string& name   ,
  ISvcLocator*       svcloc )
  : CaloAlgorithm ( name , svcloc ) 
  , m_ptPhoton        ( 0.2 * GeV  ) 
  , m_ptPi0           ( 0.3 * GeV  ) 
  , m_2gamma          ( 0          ) 
  , m_2gammaPtPhoton  ( 0          ) 
  , m_2gammaPtPhoton1 ( 0          ) 
  , m_2gammaPtPhoton2 ( 0          ) 
  , m_2gammaPtPi0     ( 0          ) 
  , m_2gammaPtPi01    ( 0          ) 
  , m_2gammaPtPi02    ( 0          ) 
{
  declareProperty ( "PhotonPt" , m_ptPhoton    ) ;
  declareProperty ( "Pi0Pt"    , m_ptPi0       ) ;  
  // set the appropriate default for input data 
  setInputData    ( CaloHypoLocation:: Photons ) ;
};

// ============================================================================
/** destructor
 */
// ============================================================================
CaloPi0Monitor::~CaloPi0Monitor() {};

// ============================================================================
/** standard algorithm initialization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloPi0Monitor::initialize() 
{  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;
  
  StatusCode sc = CaloAlgorithm::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class CaloAlgorithm",sc);}
  
  // book the histograms
  // PAWC/RZ directory
  const std::string directory( dirHbookName( "Calo/" + name() ) );  
  //  simple 2 gamma distribution 
  m_2gamma         =
    histoSvc () -> book ( directory                         ,  ///< directory
                          1                                 ,  ///< ID
                          " Gamma-Gamma Mass "              ,  ///< title
                          500                               ,  ///< #bins
                          0                                 ,  ///< low edge
                          600                              ); ///< upper edge
  if( 0 == m_2gamma )
    { return Error("Could not book 'Gamma-Gamma Mass' histogram" ) ; }
  
  m_2gammaPtPhoton         =
    histoSvc () -> book ( directory                         ,  ///< directory
                          2                                 ,  ///< ID
                          " Gamma-Gamma Mass (PtGamma cut)" ,  ///< title
                          500                               ,  ///< #bins
                          0                                 ,  ///< low edge
                          600                              ); ///< upper edge
  if( 0 == m_2gammaPtPhoton )
    { return Error("Could not book 'Gamma-Gamma Mass (PtGamma cut)' hist" ) ; }

  m_2gammaPtPhoton1        =
    histoSvc () -> book ( directory                          ,  ///< directory
                          3                                  ,  ///< ID
                          " Gamma-Gamma Mass (0.5*PtGamma )" ,  ///< title
                          500                                ,  ///< #bins
                          0                                  ,  ///< low edge
                          600                                ); ///< upper edge
  if( 0 == m_2gammaPtPhoton1 )
    { return Error("Could not book 'Gamma-Gamma Mass (0.5*PtGamma)' hist" ) ; }

  m_2gammaPtPhoton2        =
    histoSvc () -> book ( directory                          ,  ///< directory
                          4                                  ,  ///< ID
                          " Gamma-Gamma Mass (2.0*PtGamma )" ,  ///< title
                          500                                ,  ///< #bins
                          0                                  ,  ///< low edge
                          600                                ); ///< upper edge
  if( 0 == m_2gammaPtPhoton2 )
    { return Error("Could not book 'Gamma-Gamma Mass (2.0*PtGamma)' hist" ) ; }
  
  m_2gammaPtPi0         =
    histoSvc () -> book ( directory                         ,  ///< directory
                          5                                 ,  ///< ID
                          " Gamma-Gamma Mass (PtPi0 cut)"   ,  ///< title
                          500                               ,  ///< #bins
                          0                                 ,  ///< low edge
                          600                              ); ///< upper edge
  if( 0 == m_2gammaPtPi0 )
    { return Error("Could not book 'Gamma-Gamma Mass (PtPi0 cut)' hist" ) ; }

  m_2gammaPtPi01        =
    histoSvc () -> book ( directory                         ,  ///< directory
                          6                                 ,  ///< ID
                          " Gamma-Gamma Mass (0.5*PtPi0)"   ,  ///< title
                          500                               ,  ///< #bins
                          0                                 ,  ///< low edge
                          600                              ); ///< upper edge
  if( 0 == m_2gammaPtPi01 )
    { return Error("Could not book 'Gamma-Gamma Mass (0.5*PtPi0)' hist" ) ; }

  m_2gammaPtPi02        =
    histoSvc () -> book ( directory                         ,  ///< directory
                          7                                 ,  ///< ID
                          " Gamma-Gamma Mass (2.0*PtPi0)"   ,  ///< title
                          500                               ,  ///< #bins
                          0                                 ,  ///< low edge
                          600                              ); ///< upper edge
  if( 0 == m_2gammaPtPi02 )
    { return Error("Could not book 'Gamma-Gamma Mass (2.0*PtPi0)' hist" ) ; }
  
  return StatusCode::SUCCESS;
};
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
  typedef const CaloHypo          Photon  ;
  typedef const CaloHypos         Photons ;
  typedef Photons::const_iterator photon  ;
  
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  // get input data 
  Photons* photons = get<Photons>( inputData() );
  if( 0 == photons ) { return StatusCode::FAILURE ; }
  
  if( 0 == photons -> size() ) 
  { Warning ( "Empty container of photons '" + inputData() + "'" ) ; }
  
  // loop over the first photon 
  for( photon g1 = photons -> begin() ; photons -> end() != g1 ; ++g1 ) 
  {
    // skip nulls 
    if(     0 == *g1 || 0 == (*g1)->momentum() ) { continue ; }
    // loop over the second photon 
    for( photon g2 = g1 + 1 ; photons -> end() != g2 ; ++g2 )
    {
      // skip nulls 
      if( 0 == *g2 || 0 == (*g2)->momentum() ) { continue ; }
      
      const HepLorentzVector& v1 = (*g1)->momentum()->momentum() ;
      const HepLorentzVector& v2 = (*g2)->momentum()->momentum() ;
      
      // 4-vector for pi0 
      const HepLorentzVector pi0( v1 + v2 );
      
      // pi0 mass 
      const double mass = pi0.m() ;
      
      m_2gamma -> fill ( mass ) ;
          
      const double pt  = pi0 .perp () ;
      if( pt  >       m_ptPi0 ) { m_2gammaPtPi0  -> fill( mass ) ; }
      if( pt  > 0.5 * m_ptPi0 ) { m_2gammaPtPi01 -> fill( mass ) ; }
      if( pt  > 2.0 * m_ptPi0 ) { m_2gammaPtPi02 -> fill( mass ) ; }
      
      const double pt1 = v1  .perp () ;
      const double pt2 = v2  .perp () ;
      
      if( pt1 >       m_ptPhoton && 
          pt2 >       m_ptPhoton  ) { m_2gammaPtPhoton  -> fill( mass ) ; }
      if( pt1 > 0.5 * m_ptPhoton && 
          pt2 > 0.5 * m_ptPhoton  ) { m_2gammaPtPhoton1 -> fill( mass ) ; }
      if( pt1 > 2.0 * m_ptPhoton && 
          pt2 > 2.0 * m_ptPhoton  ) { m_2gammaPtPhoton1 -> fill( mass ) ; }
      
    }
  }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
