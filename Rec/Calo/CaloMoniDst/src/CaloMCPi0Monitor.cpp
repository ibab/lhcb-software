// $Id: CaloMCPi0Monitor.cpp,v 1.1.1.1 2004-10-25 09:00:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.8  2004/03/17 16:35:19  ibelyaev
//  regular update: MCCaloMonitor and CaloPIDsMonitor
//
// ============================================================================
// Include files
#include "Relations/IAssociatorWeighted.h"
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
// AIDA 
#include "AIDA/IHistogram1D.h" 
// Event 
#include "Event/CaloHypo.h"
#include "Event/MCParticle.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h"
// CaloUtils 
#include "CaloUtils/dirHbookName.h"
#include "CaloUtils/ClusterFunctors.h"
// local
#include "CaloMCPi0Monitor.h"

// ============================================================================
/** @file CaloMCPi0Monitor.cpp
 * 
 *  Template implementation file for class : CaloMCPi0Monitor
 *  @see CaloMCPi0Monitor
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 02/11/2001
 */
// ============================================================================

// ============================================================================
/** @var CaloMCPi0MonitorFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloMCPi0Monitor>         s_Factory ;
const        IAlgFactory&CaloMCPi0MonitorFactory = s_Factory ;

// ============================================================================
/** Standard constructor
 *  @param   name   algorithm name 
 *  @param   svcloc pointer to service locator 
 */
// ============================================================================
CaloMCPi0Monitor::CaloMCPi0Monitor
( const std::string& name   ,
  ISvcLocator*       svcloc )
  : CaloMoniAlg      ( name , svcloc ) 
  , m_associatorType ("AssociatorWeighted<CaloCluster,MCParticle,float>")
  , m_associatorName ("CCs2MCPs"  )
  , m_associator     ( 0          ) 
  , m_cut            ( 0.50       )
  , m_calo           ( DeCalorimeterLocation::Ecal )
  , m_ppSvc          ( 0          )
  , m_pi0Name        ( "pi0"      ) 
  , m_pi0ID          ( 0          )
  , m_2g             ( 0          ) 
  , m_2gPi0          ( 0          ) 
{
  declareProperty("AssociatorType" , m_associatorType ) ;
  declareProperty("AssociatorName" , m_associatorName ) ;
  declareProperty("Ancestor"       , m_pi0Name        ) ;
  declareProperty("Cut"            , m_cut            ) ;
  // set teh appropriate defualt value for input data 
  setInputData ( CaloHypoLocation::      Photons ) ;
  // set the appropriate defualt value for detector data 
  setDetData   ( DeCalorimeterLocation:: Ecal    ) ;  
};
// ============================================================================

// ============================================================================
/** destructor
 */
// ============================================================================
CaloMCPi0Monitor::~CaloMCPi0Monitor() {};
// ============================================================================

// ============================================================================
/** standard algorithm initialization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloMCPi0Monitor::initialize() 
{  
  
  StatusCode sc = CaloMoniAlg::initialize();
  if( sc.isFailure() ) 
  { return Error("Could not initialize the base class CaloMoniAlg",sc);}
  
  // re-initialize the Ecal cluster selector 
  m_calo.setCalo( detData() );
  
  /// locate associator 
  m_associator = tool<Asct>( m_associatorType , m_associatorName ) ;
  if( 0 == m_associator ) { return StatusCode::FAILURE ; }
  
  /// locate particle eproperty service 
  m_ppSvc = svc<IParticlePropertySvc> ( "ParticlePropertySvc" , true );
  if( 0 == m_ppSvc ) 
  { return Error ( "Could not locate ParticlePropertySvc!"      ) ; }
  
  const ParticleProperty* pp = m_ppSvc->find( m_pi0Name );
  if( 0 == pp ) 
  { return Error ( "Could not locate particle '" + m_pi0Name + "'" ) ; }
  m_pi0ID = ParticleID( pp->jetsetID() ) ;
  
  // book the histograms
  // PAWC/RZ directory
  const std::string directory( dirHbookName( "Calo/" + name() ) );  
  //  simple 2 gamma distribution 
  m_2g    = book ( 1  , " Gamma-Gamma mass       " , 0.0 , 1000.0 , 500 ) ;
  m_2gPi0 = book ( 2  , " Gamma-Gamma mass (Pi0) " , 0.0 , 1000.0 , 500 ) ;
  
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
StatusCode CaloMCPi0Monitor::execute() 
{
  // avoid long names 
  typedef const CaloHypo                 Photon      ;
  typedef const CaloHypos                Photons     ;
  typedef CaloHypo::Clusters             Clusters    ;
  
  // iterators 
  typedef Photons::const_iterator        photon      ;
  typedef Clusters::const_iterator       cluster     ;
  
  // relations 
  typedef const Asct::DirectType         DirectType  ;
  // 
  using namespace ClusterFunctors ;
  
  // get input data 
  Photons*   photons   = get<Photons>( inputData() ) ;
  if( 0 ==   photons   ) { return StatusCode::FAILURE ; }
  
  if( 0 ==   photons -> size() ) 
  { Warning ( "Empty container of photons '" + inputData() + "'" ) ; }
  
  const DirectType* table = m_associator->direct() ;
  if( 0 == table ) 
  { return Error("'Direct Relation Table' points to NULL!");}
  
  // loop over the first photon 
  for( photon g1 = photons -> begin() ; photons -> end() != g1 ; ++g1 ) 
  {
    const Photon* photon1 = *g1 ;
    // skip nulls 
    if(     0 == photon1 || 0 == photon1 -> momentum()   ) { continue ; }
    
    // get Ecal cluster
    const Clusters& clusters1 = photon1 -> clusters() ;
    if( clusters1.empty()                         ) { continue ; }
    cluster cluster1 = 
      ( 1 == clusters1.size() ) ? clusters1.begin() : 
      std::find_if( clusters1.begin () , 
                    clusters1.end   () , m_calo );
    if( clusters1.end() == cluster1    ) { continue ; }
    // get all MCtruth information for this cluster 
    const float cut1 = (*cluster1)->e() * m_cut  ;
    const DirectType::Range range1 =  
      table -> relations( *cluster1 , cut1 , true );
    
    // loop over the second photon 
    for( photon g2 = g1 + 1 ; photons->end() != g2 ; ++g2 ) 
    {
      const Photon* photon2 = *g2 ;
      // skip nulls 
      if( 0 == photon2 || 0 == photon2 -> momentum() ) { continue ; }
      
      // get Ecal cluster
      const Clusters& clusters2 = photon2 -> clusters() ;
      if( clusters2.empty()                         ) { continue ; }
      cluster cluster2 = 
        ( 1 == clusters2.size() ) ? clusters2.begin() :
        std::find_if( clusters2.begin () , 
                      clusters2.end   () , m_calo );
      if( clusters2.end() == cluster2    ) { continue ; }
      // get all MCtruth information for this cluster 
      const float cut2 = (*cluster2)->e() * m_cut  ;
      const DirectType::Range range2 =  
        table -> relations( *cluster2 , cut2 , true );
      
      // double loop for search the common ancestor 
      MCParticle* pi0 = 0;
      for( DirectType::iterator mc1 = range1.begin() ; 
           0 == pi0 && range1.end() != mc1 ; ++mc1 )
      {
			  MCParticle* mcp1 = mc1->to();
        if( 0 == mcp1 ) { continue ; }
        for( DirectType::iterator mc2 = range2.begin() ; 
             0 == pi0 && range2.end() != mc2 ; ++mc2 )
        {
          // common ancestor?
          if( mc1->to() != mc2->to() ) { continue ; }
          if( m_pi0ID == mc1->to()->particleID() ) { pi0 = mc1->to() ; }
        } // end of second MC loop 
      } // end of first MC loop
      
      const HepLorentzVector p( photon1 -> momentum() -> momentum() + 
                                photon2 -> momentum() -> momentum() );
      
      const double mass  = p.m();
      
      m_2g     -> fill ( mass ) ;
      
      if( 0 == pi0 ) { continue ; }
      
      m_2gPi0  -> fill ( mass ) ;
      
    } // end of loop over second photon 
  } // end of loop over first photon 
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
