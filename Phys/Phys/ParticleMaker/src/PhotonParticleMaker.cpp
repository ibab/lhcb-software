// $Id: PhotonParticleMaker.cpp,v 1.2 2005-01-06 10:46:54 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2004/08/24 06:47:48  pkoppenb
// ParticleMaker extracted from DaVinciTools
//
// Revision 1.8  2004/07/28 14:15:24  pkoppenb
// untag
//
// Revision 1.7  2004/05/11 16:01:25  pkoppenb
// DVAlgorithm.cpp
//
// Revision 1.6  2004/04/21 02:29:30  ibelyaev
//  PhotonParticleMaker is modified
//
// Revision 1.5  2004/03/11 13:02:14  pkoppenb
// Split DaVinciTools into DaVinciTools and DaVinciKernel
//
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h" 
// LHCbKErnel 
#include "Kernel/CLHEPStreams.h"
// Event 
#include "Event/Particle.h" 
#include "Event/ProtoParticle.h" 
#include "Event/CaloHypo.h" 
// Det
#include "CaloDet/DeCalorimeter.h"
// DaVinciTools 
#include "Kernel/IPhotonParams.h"
// local
#include "PhotonParticleMaker.h"

/** @file 
 *  
 *  Implementation file for class : PhotonParticleMaker
 * 
 *  @date 2003-01-19 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */

// ============================================================================
namespace PhotonParticleMakerLocal 
{
  // ==========================================================================
  /** @class DigitFromCalo 
   *  simple utility to count digits from certain calorimeter 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date 31/03/2002 
   */
  // ==========================================================================
  class DigitFromCalo 
    : public std::unary_function<const CaloDigit*,bool>
  { 
  public:   
    /** constructor
     *  @parameter calo  calorimeter name 
     */
    explicit DigitFromCalo( const std::string& calo )
      : m_calo( CaloCellCode::CaloNumFromName( calo ) ) {} ;
    /** constructor
     *  @parameter calo  calorimeter index 
     */
    explicit DigitFromCalo( const int  calo )
      : m_calo(                                calo   ) {} ;
    /** the only essential method
     *  @param digit pointer to CaloDigit object 
     *  @return true if digit belongs to the predefined calorimeter 
     */
    inline bool operator() ( const CaloDigit* digit ) const 
    {
      if( 0 == digit ) { return false ; }
      return (int) digit->cellID().calo() == m_calo ;
    };
  private:
    /// default constructor is private 
    DigitFromCalo();
  private:
    int m_calo ;
  };  
  // ==========================================================================
  /** @class PhotonCnv 
   *  simple evaluator if the photon is converted or not 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2003-04-08
   */
  // ==========================================================================
  class PhotonCnv
  {
  public:
    /** constructor 
     *  @param det detectro name 
     */
    PhotonCnv( const std::string& det = DeCalorimeterLocation::Spd )
      : m_det ( det ) {}
    /** the only one essential method 
     *  @return status code 
     */
    inline StatusCode converted 
    ( const CaloHypo* photon , 
      bool&           cnv    ) const 
    {
      if ( 0 == photon ) 
        { return StatusCode ( 300 ) ; }                        // RETURN 
      if ( CaloHypotheses::Photon != photon->hypothesis() )   
        { return StatusCode ( 301 ) ; }                        // RETURN  }
      
      // loop over all digits 
      typedef CaloHypo::Digits Digits;
      const Digits& digits = photon->digits();
      for( Digits::const_iterator d = digits.begin() ; digits.end() != d ; ++d )
        {
          if( m_det( *d ) ) { cnv = true ; return StatusCode::SUCCESS ; } 
        }
      // no digits from spd 
      cnv = false ;
      return StatusCode::SUCCESS ;
    };
  private: 
    DigitFromCalo m_det ;
  };
  // ==========================================================================
}; // end of namespace PhotonParticleMaker
// ============================================================================

// ============================================================================
/** @var PhotonParticleMakerFactory
 *  Declaration of mandatory  Tool Factory
 */
// ============================================================================
static const  ToolFactory<PhotonParticleMaker>         s_Factory ;
const        IToolFactory&PhotonParticleMakerFactory = s_Factory ; 
// ============================================================================
// ============================================================================
/** Standard constructor
 *  @param type   tool type
 *  @param name   tool name 
 *  @param parent tool parent
 */
// ============================================================================
PhotonParticleMaker::PhotonParticleMaker
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : CaloTool           ( type, name , parent ) 
  //
  , m_input            ( ProtoParticleLocation::Neutrals )
  //
  , m_photParsName     ( "PhotonParameters/PhotPars" )
  , m_photPars         ( 0     ) 
  // 
  , m_pointVector      ( 3 , 0 ) 
  , m_point            () 
  // 
  , m_pointErr         ( 3 , 0 )  
  , m_diagonal         ()         // square roots from diagonal elements 
  , m_offDiagonal      ( 3 , 0 )  // correlation coefficients 
  ///
  , m_useCaloTrMatch   ( false )
  , m_useCaloDepositID ( false )
  , m_useShowerShape   ( false )
  , m_useClusterMass   ( false )
  , m_usePhotonID      ( true  )
  // cut
  , m_cut              ( -1.0  ) // no cut at all
  // converted
  , m_converted        ( false )
  , m_useAll           ( true  )
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
  declareProperty ( "UsePhotonID"                , m_usePhotonID      ) ;

  declareProperty ( "ConfLevelCut"               , m_cut              ) ;

  declareProperty ( "UseAllPhotons"              , m_useAll           ) ;

  // declare new interface
  declareInterface<IParticleMaker> (this);
};
// ============================================================================


// ============================================================================
/// destructor
// ============================================================================
PhotonParticleMaker::~PhotonParticleMaker() {};
// ============================================================================

// ============================================================================
/** standard initialization of tool
 *  @see CaloTool
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code
 */
// ============================================================================
StatusCode PhotonParticleMaker::initialize    ()
{
  // initialize the base class
  StatusCode sc = CaloTool::initialize();
  if( sc.isFailure() ) { return Error(" Unable to initialize CaloTool",sc);}

  // locate photon parameters evaluator
  m_photPars = tool<IPhotonParams>( m_photParsName , this );
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
  if( m_usePhotonID     )
    { log << " PhotonID     : Tracking/CALO combined Photon id   " << endreq ; }

  if( !m_useCaloTrMatch   &&
      !m_useCaloDepositID &&
      !m_useShowerShape   &&
      !m_useClusterMass   &&
      !m_usePhotonID     )
    { Warning(" No PID techniques are selected for CL evaluation" ) ; }

  if( m_useCaloTrMatch    )
    { Warning( "  For CaloTrMatch assume Gauss distribution (wrong?)"); }
  if( m_useCaloDepositID  )
    { Warning( "      CaloDepositID is not implemented yet " ) ; }
  if( m_useShowerShape    )
    { Warning( "      ShowerShape   is not implemented yet " ) ; }
  if( m_useClusterMass    )
    { Warning( "  For ClusterMass assume exponential distribution (wrong?)"); }

  if      ( m_useAll    )
    { Warning( "\t         *ALL*     'ordinary' photons are to be created" ) ; }
  else if ( m_converted )
    { Warning( "\tOnly     converted 'ordinary' photons are to be created" ) ; }
  else
    { Warning( "\tOnly non-converted 'ordinary' photons are to be created" ) ; }

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
StatusCode PhotonParticleMaker::finalize      ()
{
  // release services and tools
  m_photPars = 0 ;

  // finalize the base class
  return CaloTool::finalize ();
};
// ============================================================================

// ============================================================================
namespace PhotonParticleMakerLocal
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
StatusCode PhotonParticleMaker::makeParticles ( ParticleVector & particles )
{
  // avoid some long names
  using   namespace PhotonParticleMakerLocal               ;
  using   namespace CaloHypotheses                         ;
  typedef const SmartRefVector<CaloHypo>             Hypos ;

  if( !particles.empty() )
    { Warning( "makeParticles(): extend non-empty vector of Particles" ) ; }

  // locate input data
  const ProtoParticles* pps = get< ProtoParticles > (inputData());
  if( 0 == pps ) { return StatusCode::FAILURE ; }

  unsigned long nAccepted = 0 ;

  // evaluator of converted photons
  PhotonParticleMakerLocal::PhotonCnv phCnv;

  for( ProtoParticles::const_iterator ipp = pps->begin() ;
       pps->end() != ipp ; ++ipp )
    {
      ProtoParticle* pp = *ipp ;
      // skip invalid and charged
      if ( 0 == pp || 0 != pp->track() )   { continue ; }        // CONTINUE

      // evaluate the Confidence Level
      const double CL = confLevel( pp );

      // find the first photon hypothesis
      const Hypos& hypos = pp -> calo () ;
      Hypos::const_iterator ihypo =
        std::find_if( hypos.begin() , hypos.end() , IsHypo( Photon ) ) ;
      if ( hypos.end() == ihypo        )   { continue ; }       // CONTINUE

      const CaloHypo* hypo = *ihypo ;
      if ( 0           ==  hypo        )   { continue ; }       // CONTINUE

      bool cnv = false ;
      StatusCode sc = phCnv.converted( hypo , cnv ) ;
      if( sc.isFailure() ){ Error("Error from PhotonCnv" , sc ) ; continue ; }

      // skip extra
      if( !m_useAll && m_converted  != cnv ) { continue ; }

      // confidence level
      if ( CL < m_cut                      ) { continue ; }    // CONTINUE

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
      sc = photPars()->process( particle   ,
                                m_point    ,
                                m_pointErr ) ;
      if( sc.isFailure() )
        {
          Error("Unable to fill photon parameters, skip particle " , sc ) ;
          delete particle ;
          continue ;                                               // CONTINUE
        }

      // add the particle to the container
      particles.push_back( particle );

    }

  MsgStream log( msgSvc() , name() ) ;
  log << MSG::INFO
      << " Create " << nAccepted   <<
    ( m_converted ? " Converted Photons " : "           Photons " )
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
double PhotonParticleMaker::confLevel( const ProtoParticle* pp ) const
{
  // avoid long typing
  using   namespace PhotonParticleMakerLocal               ;
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

  // Cluster Mass
  if( m_useClusterMass )
    {
      PIDs::const_iterator ipid =
        std::find_if( pids.begin () ,
                      pids.end   () ,
                      IsPID( ProtoParticle::ClusterMass     ) );
      if( pids.end() != ipid )
        {
          // assume exponential distribution (it is wrong!)
          CL *= exp( -0.5 * ipid->second / ( 25 * MeV ) ) ;
        }  // Update CL
      else
        { Warning("confLevel(): ClusterMass is not available" ) ; }
    }

  // PhotonID estimator
  if( m_usePhotonID )
    {
      PIDs::const_iterator ipid =
        std::find_if( pids.begin () ,
                      pids.end   () ,
                      IsPID( ProtoParticle::PhotonID     ) );
      if( pids.end() != ipid )
        {
          // assume exponential distribution (it is wrong!)
          CL *= ( ipid->second ) ;
        }  // Update CL
      else
        { Warning("confLevel(): PhotonID is not available" ) ; }
    }

  return CL ;
};
// ============================================================================


// ============================================================================
// The END
// ============================================================================
