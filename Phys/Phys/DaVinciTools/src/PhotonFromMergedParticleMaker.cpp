// $Id: PhotonFromMergedParticleMaker.cpp,v 1.3 2004-04-22 02:55:13 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h" 
// LHCbKErnel 
#include "Kernel/CLHEPStreams.h"
// Event 
#include "Event/Particle.h" 
#include "Event/PrimVertex.h"
#include "Event/Vertex.h"
#include "Event/ProtoParticle.h" 
// DaVinciTools 
#include "DaVinciTools/IPhotonFromMergedParams.h"
// local
#include "PhotonFromMergedParticleMaker.h"

/** @file 
 *  
 *  Implementation file for class : PhotonFromMergedParticleMaker
 * 
 *  @date 2003-03-11
 *  @author Olivier Deschamps from Vanya's PhotonParticleMaker
 */

// ============================================================================
/** @var PhotonFromMergedParticleMakerFactory
 *  Declaration of mandatory  Tool Factory
 */
// ============================================================================
static const  ToolFactory<PhotonFromMergedParticleMaker>         s_Factory ;
const        IToolFactory&PhotonFromMergedParticleMakerFactory = s_Factory ; 
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param type   tool type
 *  @param name   tool name 
 *  @param parent tool parent
 */
// ============================================================================
PhotonFromMergedParticleMaker::PhotonFromMergedParticleMaker
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : CaloTool           ( type, name , parent ) 
  //
  , m_input            ( ProtoParticleLocation::Neutrals )
  //
  , m_photParsName     ( "PhotonFromMergedParameters/PhotPars" )
  , m_photPars         ( 0     ) 
  // 
  , m_pointVector      ( 3 , 0 ) 
  , m_point            () 
  // 
  , m_pointErr         ( 3 , 0 )  
  , m_diagonal         ()         // square roots from diagonal elements 
  , m_offDiagonal      ( 3 , 0 )  // correlation coefficients 
  ///
  , m_useCaloTrMatch   ( true ) 
  , m_useCaloDepositID ( false )
  , m_useShowerShape   ( false )
  , m_useClusterMass   ( false )
  // cut 
  , m_cut            ( -1.0  ) // no cut at all 
{
  declareProperty ( "Input"                      , m_input            ) ;
  declareProperty ( "PhotonParametersEvaluator"  , m_photParsName     ) ;
  declareProperty ( "ProductionPoint"            , m_pointVector      ) ;
  m_diagonal.push_back( 50 * micrometer ) ;
  m_diagonal.push_back( 50 * micrometer ) ;
  m_diagonal.push_back(  1 * millimeter ) ;
  declareProperty ( "DiagonalUncertanties"       , m_diagonal         ) ;
  declareProperty ( "OffDiagonalCorrelations"    , m_offDiagonal      ) ;
  
  declareProperty ( "UseCaloTrMatch"             , m_useCaloTrMatch   ) ;
  declareProperty ( "UseCaloDepositID"           , m_useCaloDepositID ) ;
  declareProperty ( "UseShowerShape"             , m_useShowerShape   ) ;
  declareProperty ( "UseClusterMass"             , m_useClusterMass   ) ;

  declareProperty ( "ConfLEvelCut"               , m_cut              ) ;
  
  // declare new interface 
  declareInterface<IParticleMaker> (this);
};
// ============================================================================


// ============================================================================
/// destructor 
// ============================================================================
PhotonFromMergedParticleMaker::~PhotonFromMergedParticleMaker() {};
// ============================================================================

// ============================================================================
/** standard initialization of tool 
 *  @see CaloTool
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode PhotonFromMergedParticleMaker::initialize    () 
{
  // initialize the base class
  StatusCode sc = CaloTool::initialize();
  if( sc.isFailure() ) { return Error(" Unable to initialize CaloTool",sc);}
  
  // locate event data provider
  if( 0 == evtSvc()   ) { return Error(" Unable to locate EventDataSvc",sc);}
  
  // locate photon parameters evaluator 
  if( 0 != m_photPars ) { m_photPars = 0 ; }
  m_photPars = tool<IPhotonFromMergedParams>( m_photParsName , this );
  if( 0 == photPars() ) { return StatusCode::FAILURE ; }           // RETURN
  
  if( 3 != m_pointVector.size() ) 
    { return Error(" Invalid dimention of point (must be 3)");}
  
  m_point.setX( m_pointVector[0] ) ;
  m_point.setY( m_pointVector[1] ) ;
  m_point.setZ( m_pointVector[2] ) ;
  
  if( 3 != m_diagonal.size()    )
    { return Error(" Invalid dimention of Diagonal (must be 3)");}

  m_pointErr ( 1 , 1 ) = m_diagonal[0] * m_diagonal[0] ;
  m_pointErr ( 2 , 2 ) = m_diagonal[1] * m_diagonal[1] ;
  m_pointErr ( 3 , 3 ) = m_diagonal[2] * m_diagonal[2] ;
  
  if( 3 != m_offDiagonal.size() ) 
    { return Error(" Invalid dimention of OffDiagonal (must be 3)");}
  
  {
    const double mx = *std::max_element( m_offDiagonal.begin () , 
                                         m_offDiagonal.end   () ) ;
    const double mn = *std::min_element( m_offDiagonal.begin () , 
                                         m_offDiagonal.end   () ) ; 
    if( fabs( mx ) > 1 || fabs( mn ) > 1 ) 
      { return Error("Wrong value for correlation coefficient!" ) ; }
  }
  
  m_pointErr ( 2 , 1 ) = m_diagonal[0] * 
    sqrt( m_pointErr ( 2 , 2 ) * m_pointErr ( 1 , 1 ) ) ;
  
  m_pointErr ( 3 , 1 ) = m_diagonal[1] * 
    sqrt( m_pointErr ( 3 , 3 ) * m_pointErr ( 1 , 1 ) ) ;
  
  m_pointErr ( 3 , 2 ) = m_diagonal[2] * 
    sqrt( m_pointErr ( 3 , 3 ) * m_pointErr ( 2 , 2 ) ) ;

  MsgStream log( msgSvc() , name() ) ;
  log << MSG::INFO 
      << " Nominal Photon production vertex and its covariance matrix: " 
      << endreq ;
  log << " Point             : " << m_point     << endreq ;
  log << " Covariance Matrix : " << m_pointErr  << endreq ;
  
  log << MSG::INFO 
      << " Following techniques will be used for CL evaluation : " << endreq;
  if( m_useCaloTrMatch    ) 
    { log << "  CaloTrMatch : matching with reconsttucted tracks " << endreq ; }
  if( m_useCaloDepositID  ) 
    { log << " CaloDepositID: Spd/Prs combined analysis          " << endreq ; }
  if( m_useShowerShape  ) 
    { log << " ShowerShape  : Ecal Cluster shape/size            " << endreq ; }
  if( m_useClusterMass  ) 
    { log << " ClusterMass  : Ecal Cluster pi0 mass              " << endreq ; }
  
  if( !m_useCaloTrMatch   && 
      !m_useCaloDepositID && 
      !m_useShowerShape   && 
      !m_useClusterMass     ) 
    { Warning(" No PID techniques are selected for CL evaluation" ) ; }

  if( m_useCaloTrMatch    ) 
    { Warning( "  For CaloTrMatch assume Gauss distribution (wrong?)"); }
  if( m_useCaloDepositID  )
    { Warning( "      CaloDepositID is not implemented yet " ) ; }
  if( m_useShowerShape    ) 
    { Warning( "      ShowerShape   is not implemented yet " ) ; }
  if( m_useClusterMass    ) 
    { Warning( "  For ClusterMass assume exponential distribution (wrong?)"); }
  
  return StatusCode::SUCCESS ;  
};
// ============================================================================

// ============================================================================
/** standard finalization of tool 
 *  @see CaloTool
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode PhotonFromMergedParticleMaker::finalize      () 
{
  // release services and tools 
  m_photPars = 0 ; 
  
  // finalize the base class 
  return CaloTool::finalize ();
};
// ============================================================================

// ============================================================================
namespace PhotonFromMergedParticleMakerLocal
{
  // ==========================================================================
  class IsHypo : public std::unary_function<const CaloHypo*,bool>
  {
  public: 
    /// constructor 
    IsHypo( CaloHypotheses::Hypothesis hypo ): m_hypo ( hypo ) {};
    /// functor interface 
    bool operator() ( const CaloHypo* hypo ) const 
    { return 0 != hypo && m_hypo == hypo->hypothesis() ? true : false ; }
  private:
    IsHypo();
  private:
    CaloHypotheses::Hypothesis m_hypo ;
  };
  // ==========================================================================

  // ==========================================================================
  class IsPID : public std::unary_function<ProtoParticle::PIDDetPair,bool>
  {
  public:
    /// constructor 
    IsPID( const ProtoParticle::detectorPID pid ) : m_pid ( pid ) {};
    // functor interafce 
    bool operator() ( const ProtoParticle::PIDDetPair& p ) const 
    {  return p.first == m_pid ? true : false ; }
  private:
    ProtoParticle::detectorPID m_pid ;
  };
  // ==========================================================================
  
};
// ============================================================================

// ============================================================================
/** Make the particles 
 *  @see IParticleMaker 
 *  @param particles  vector of particles  
 */
// ============================================================================
StatusCode PhotonFromMergedParticleMaker::makeParticles 
           ( ParticleVector & particles ) 
{
  MsgStream log( msgSvc() , name() ) ;
  // avoid some long names
  using   namespace PhotonFromMergedParticleMakerLocal               ;
  using   namespace CaloHypotheses                         ;
  typedef const SmartRefVector<CaloHypo>             Hypos ;
  
  if( !particles.empty() ) 
    { Warning( "makeParticles(): extend non-empty vector of Particles" ) ; }
  
  // locate input data 
  const ProtoParticles* pps = get< ProtoParticles > ( inputData() );
  if( 0 == pps ) { return StatusCode::FAILURE ; }

  unsigned long nAccepted = 0 ;
  
  for( ProtoParticles::const_iterator ipp = pps->begin() ; 
       pps->end() != ipp ; ++ipp ) 
    {
      ProtoParticle* pp = *ipp ;
      // skip invalid and charged 
      if( 0 == pp || 0 != pp->track() )   { continue ; }        // CONTINUE
      
      // evaluate the Confidence Level
      const double CL = confLevel( pp );
      
      // Loop over hypo and find the TWO photon_from_merged_pi0 (OD)
      const Hypos& hypos = pp -> calo () ;


      //Hypos::const_iterator ihypo=
      //std::find_if(hypos.begin(),hypos.end(),IsHypo(PhotonFromMergedPi0));
      //if( hypos.end() == ihypo        )   {continue ; }       // CONTINUE
      //
      // Need to LOOP OVER HYPOS (2 SplitPhotons per ProtoParticle !!)
      for(Hypos::const_iterator ihypo =hypos.begin();hypos.end()!=ihypo;++ihypo)
        {
          if((*ihypo)->hypothesis() != 
             CaloHypotheses::PhotonFromMergedPi0){continue;}

          const CaloPosition* pos = (*ihypo)->position() ;
          log << MSG::DEBUG << " Ipp PhotonFromMergedPi0 "<< pos->e()<< " " 
              << CL << endreq;
          // confidence level 
          if( CL < m_cut                  )   { continue ; }     // CONTINUE 
          // counter 
          ++nAccepted ;
          // create new particle and start to fill it 
          Particle* particle = new Particle();
          // it is a photon! 
          particle -> setParticleID  ( 22    ) ; // photon 
          particle -> setIsResonance ( false ) ;
          // origin is the protoparticle!
          particle -> setOrigin      ( pp    ) ;
          // mass and mass uncertainties 
          particle -> setMass        ( 0.0   ) ; // photon is mass-less 
          particle -> setMassErr     ( 0.0   ) ; // the mass is EXACT zero!
          // confidence level 
          particle  -> setConfLevel  ( CL    ) ; 
          // fill photon parameters :
          //   the 4-momentum, production vertex and their correlations 

          //Default: Apply 1s found t primary vertex for production point
          double isfill = m_pointVector[0]*m_pointVector[1]*m_pointVector[2];
          SmartDataPtr<Vertices> primvtx( evtSvc(), VertexLocation::Primary);
          if ( 0 != primvtx->size() && 0 == isfill ){
            Vertices::iterator ivtx = primvtx->begin() ;
            const Vertex*  pv     = *ivtx ;
            log << MSG::DEBUG << "Nvertex " << primvtx->size() << endreq;
            log << MSG::DEBUG 
                <<  pv->position().x()  << " "
                <<  pv->position().y()  << " "
                <<  pv->position().z()  << " "
                << endreq;
            m_point    = pv->position();
            m_pointErr = pv->positionErr();
          }
          log << MSG::DEBUG << "m_point" <<
            m_point[0]<< " "<<
            m_point[1]<< " "<<
            m_point[2]<< endreq;

          const CaloHypo* thehypo=*ihypo;
          StatusCode sc = photPars()->process( particle   , 
                                               thehypo    ,
                                               m_point    , 
                                               m_pointErr ) ;
          if( sc.isFailure() ) 
        {
          Error("Unable to fill photon_from_merged parameters, skip particle " 
                , sc ) ;
          delete particle ;
          continue ;                                               // CONTINUE
        }
      
          // add the particle to the container 
          particles.push_back( particle );
        }// End of loop over hypo (OD)
    }

  log << MSG::DEBUG
      << " Create " << nAccepted   << " PhotonsFromMerged "  
      << " from   " << pps->size() << " ProtoParticles " << endreq ;
  
  return StatusCode::SUCCESS ;
};
// ============================================================================


// ============================================================================
/**  evaluate Photon's "confidence level" from following quantities 
 * 
 *  <li>  <c> CaloTrMatch     </c> 
 *  <li>  <c> CaloDepositID   </c>
 *  <li>  <c> CaloShowerShape </c>
 *  <li>  <c> ClusterMass     </c> 
 *
 */
// ============================================================================
double PhotonFromMergedParticleMaker::confLevel( const ProtoParticle* pp ) const
{ 
  // avoid long typing 
  using   namespace PhotonFromMergedParticleMakerLocal               ;
  typedef const std::vector<std::pair<int,double> >  PIDs  ;
  
  if( 0 == pp ) 
    { Error("confLevel(): ProtoParticle* points to NULL!"); return -1 ; };
  
  // get all 
  const PIDs& pids = pp -> pIDDetectors() ;
  
  double CL = 1.0 ;
  
  // track matching 
  if( m_useCaloTrMatch  ) 
    { 
      PIDs::const_iterator ipid = 
        std::find_if( pids.begin () , 
                      pids.end   () , 
                      IsPID( ProtoParticle::CaloTrMatch   ) );
      if( pids.end() != ipid ) 
        { 
          // assume gaussian distribution (it is wrong!)
          CL *= ( 1.0 - exp( -0.5 * ipid->second )  ) ; // Update CL
        } 
      else 
        { Warning("confLevel(): CaloTrMatch is not available" ) ; }
    }
  
  // CaloDepositID 
  if( m_useCaloDepositID ) 
    { 
      PIDs::const_iterator ipid = 
        std::find_if( pids.begin () , 
                      pids.end   () , 
                      IsPID( ProtoParticle::CaloDepositID   ) );
      if( pids.end() != ipid ) 
        { 
          Warning("confLevel(): usage of CaloDepositID is not implemented"); 
        }   // Update CL
      else 
        { Warning("confLevel(): CaloDepositID is not available" ) ; }
    }
  
  // ShowerShape  
  if( m_useShowerShape ) 
    {
      PIDs::const_iterator ipid = 
        std::find_if( pids.begin () , 
                      pids.end   () , 
                      IsPID( ProtoParticle::ShowerShape ) );
      if( pids.end() != ipid )
        { 
          Warning("confLevel(): usage of CaloShowerShape is not implemented");
        }   // Update CL
      else 
        { Warning("confLevel(): CaloShowerShape is not available" ) ; }
    }
  
  if( m_useClusterMass ) 
    { 
      PIDs::const_iterator ipid = 
        std::find_if( pids.begin () , 
                      pids.end   () , 
                      IsPID( ProtoParticle::ClusterMass     ) );
      if( pids.end() != ipid ) 
        {
          // assume exponential distribution (it is wrong!)
          // 1 - CL(single photon) for Merged !!!
          CL *= 1.-exp( -0.5 * ipid->second / ( 25 * MeV ) ) ; 
        }  // Update CL
      else 
        { Warning("confLevel(): ClusterMass is not available" ) ; }
    }
  
  return CL ;
};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
