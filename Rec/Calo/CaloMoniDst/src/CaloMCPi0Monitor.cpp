// $Id: CaloMCPi0Monitor.cpp,v 1.2 2005-05-08 09:58:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// ============================================================================
#include "Relations/IRelationWeighted.h"
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
// ============================================================================
// AIDA 
// ============================================================================
#include "AIDA/IHistogram1D.h" 
// ============================================================================
// Event 
// ============================================================================
#include "Event/CaloHypo.h"
#include "Event/MCParticle.h"
// ============================================================================
// CaloDet
// ============================================================================
#include "CaloDet/DeCalorimeter.h"
// ============================================================================
// CaloUtils 
// ============================================================================
#include "CaloUtils/dirHbookName.h"
#include "CaloUtils/ClusterFunctors.h"
// ============================================================================
// local 
// ============================================================================
#include "CaloMoniAlg.h"
// ============================================================================

/** @class CaloMCPi0Monitor CaloMCPi0Monitor.cpp
 *  
 *  Simple MC pi0 monitoring algorithm
 *  It produces 3 histograms 
 *  <ol>
 *  <li> "Raw" mass distribution of 2 photons </li>
 *  <li> Mass distribution of 2 photons after Pt cut for each photon </li>
 *  <li> Mass distribution of 2 photons after Pt cut for combination </li>
 *  </ol>
 *
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   02/11/2001
 */

class CaloMCPi0Monitor : public CaloMoniAlg 
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloMCPi0Monitor>;
  
public:
  
  /** standard algorithm initialization 
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode initialize();   
  
  /** standard algorithm execution 
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode execute   ();   
  
protected:
  
  /** Standard constructor
   *  @param   name   algorithm name 
   *  @param   svcloc pointer to service locator 
   */
  CaloMCPi0Monitor
  ( const std::string& name   , 
    ISvcLocator*       svcloc )
    : CaloMoniAlg      ( name , svcloc ) 
    , m_cut            ( 50 * perCent  )
    , m_calo           ( DeCalorimeterLocation::Ecal )
    , m_pi0Name        ( "pi0"      ) 
    , m_pi0ID          ( 0          )
  {
    declareProperty("Ancestor"       , m_pi0Name        ) ;
    declareProperty("Cut"            , m_cut            ) ;
    // set the appropriate default value for input data 
    setInputData   ( "Rec/Relations/CaloClusters2MCParticles");
    // set the appropriate defalts value for input data 
    addToInputs    ( CaloHypoLocation::      Photons    ) ;
    addToInputs    ( CaloHypoLocation:: SplitPhotons    ) ;
    // set the appropriate defualt value for detector data 
    setDetData     ( DeCalorimeterLocation:: Ecal       ) ;  
  };
  
  /// destructor (virtual and protected)
  virtual ~CaloMCPi0Monitor() {} ;
  
private:

  /// default  construstor  is  private 
  CaloMCPi0Monitor(); 
  /// copy     construstor  is  private 
  CaloMCPi0Monitor
  ( const CaloMCPi0Monitor& );
  /// assignement operator  is  private 
  CaloMCPi0Monitor& operator=
  ( const CaloMCPi0Monitor& );
  
private:
  // address of relation table 
  std::string   m_table ;
  // photon "purity" cut 
  float                            m_cut                         ;  
  ClusterFunctors::ClusterFromCalo m_calo                        ;
  std::string                      m_pi0Name                     ;
  ParticleID                       m_pi0ID                       ;
};

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
  
  /// locate particle eproperty service 
  IParticlePropertySvc* ppS = 
    svc<IParticlePropertySvc> ( "ParticlePropertySvc" , true );
  if( 0 == ppS ) { return StatusCode::FAILURE ; }
  
  const ParticleProperty* pp = ppS->find( m_pi0Name );
  if( 0 == pp ) 
  { return Error ( "Could not locate particle '" + m_pi0Name + "'" ) ; }
  m_pi0ID = ParticleID( pp->jetsetID() ) ; 
  
  book ( 1 , " Gamma-Gamma mass        " , 0.0 , 1.0 , 500 ) ;
  book ( 2 , " Gamma-Gamma mass (pi0?) " , 0.0 , 1.0 , 500 ) ;
  
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
  typedef const CaloHypos                Hypos       ;
  typedef std::vector<const Photon*>     Photons     ;
  typedef CaloHypo::Clusters             Clusters    ;
  
  // iterators 
  typedef Photons::const_iterator        photon      ;
  typedef Clusters::const_iterator       cluster     ;
  
  // relation table 
  typedef const IRelationWeighted<CaloCluster,MCParticle,float> Table ;
  typedef Table::Range                                          Range ;
  
  using namespace ClusterFunctors ;
  
  if ( inputs().empty() ) { return Error("No input data are specified") ; }
  
  Photons photons ;
  for ( Inputs::const_iterator input = inputs().begin() ; 
        inputs().end() != input ; ++input ) 
  {
    const Hypos* hypos = get<Hypos> ( *input ) ;
    if ( 0 == hypos ) { return StatusCode::FAILURE ; }
    photons.insert ( photons.end  () , 
                     hypos->begin () , 
                     hypos->end   () ) ;
  }
  
  const Table* table = get<Table>( inputData() ) ;
  if ( 0 == table ) 
  { return Error ( "Relation Table points to NULL! " ) ; }
  
  // loop over the first photon 
  for ( photon g1 = photons.begin() ; photons.end() != g1 ; ++g1 ) 
  {
    const Photon* photon1 = *g1 ;
    // skip nulls 
    if (     0 == photon1 || 0 == photon1 -> momentum()   ) { continue ; }
    
    // get Ecal cluster
    const Clusters& clusters1 = photon1 -> clusters() ;
    if ( clusters1.empty()                         ) { continue ; }
    cluster cluster1 = 
      ( 1 == clusters1.size() ) ? clusters1.begin() : 
      std::find_if( clusters1.begin () , 
                    clusters1.end   () , m_calo );
    if ( clusters1.end() == cluster1    ) { continue ; }
    // get all MCtruth information for this cluster 
    const float cut1 = (*cluster1)->e() * m_cut  ;
    const Range range1 =  
      table -> relations( *cluster1 , cut1 , true );
    
    // loop over the second photon 
    for ( photon g2 = g1 + 1 ; photons.end() != g2 ; ++g2 ) 
    {
      const Photon* photon2 = *g2 ;
      // skip nulls 
      if ( 0 == photon2 || 0 == photon2 -> momentum() ) { continue ; }
      
      // get Ecal cluster
      const Clusters& clusters2 = photon2 -> clusters() ;
      if ( clusters2.empty()                         ) { continue ; }
      cluster cluster2 = 
        ( 1 == clusters2.size() ) ? clusters2.begin() :
        std::find_if( clusters2.begin () , 
                      clusters2.end   () , m_calo );
      if ( clusters2.end() == cluster2    ) { continue ; }
      // get all MCtruth information for this cluster 
      const float cut2 = (*cluster2)->e() * m_cut  ;
      const Range range2 =  
        table -> relations( *cluster2 , cut2 , true );
      
      // double loop for search the common ancestor 
      MCParticle* pi0 = 0;
      for ( Range::iterator mc1 = range1.begin() ; 
            0 == pi0 && range1.end() != mc1 ; ++mc1 )
      {
			  MCParticle* mcp1 = mc1->to();
        if ( 0 == mcp1 ) { continue ; }
        for ( Range::iterator mc2 = range2.begin() ; 
              0 == pi0 && range2.end() != mc2 ; ++mc2 )
        {
          // common ancestor?
          if ( mc1->to() != mc2->to() ) { continue ; }
          if ( m_pi0ID == mc1->to()->particleID() ) { pi0 = mc1->to() ; }
        } // end of second MC loop 
      } // end of first MC loop
      
      const CLHEP::HepLorentzVector 
        p( photon1 -> momentum() -> momentum() + 
           photon2 -> momentum() -> momentum() );
      
      const double mass  = p.m();
      
      histo ( 1 ) -> fill ( mass / GeV ) ;
      
      if ( 0 == pi0 ) { continue ; }
      
      histo ( 2 ) -> fill ( mass / GeV ) ;
      
    } // end of loop over second photon 
  } // end of loop over first photon 
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
