// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/GaudiException.h"
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"

// Include files
// CaloEvent
#include "Event/CaloCluster.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h"
// CaloUtils 
#include "CaloUtils/ClusterFunctors.h"
// DetDesc 
#include "DetDesc/IGeometryInfo.h" 

// local
#include "NeutralParticleMaker.h"

// Declaration of the Tool Factory
static const  ToolFactory<NeutralParticleMaker>          s_factory ;
const        IToolFactory& NeutralParticleMakerFactory = s_factory ; 

/** @class NeutralParticleMaker NeutralParticleMaker.cpp
 *  Produces Neutral Particles from Neutral ProtoParticles
 *
 * @author Frederic Machefert
 * @date 24/07/02/2002 
*/

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
NeutralParticleMaker::NeutralParticleMaker( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : AlgTool ( type, name , parent )
  , m_ppSvc(0)
  , m_EDS(0)
  , m_nameEcal("/dd/Structure/LHCb/Ecal")
  , m_nameSpd ("/dd/Structure/LHCb/Spd")
  , m_namePrs ("/dd/Structure/LHCb/Prs")
  , m_calo      ( DeCalorimeterLocation::Ecal )  
{
  // Declaring implemented interfaces
  declareInterface<IParticleMaker>(this);
  
  // Clean data members
  m_particleNames.clear();
  m_ids.clear();
  m_confLevels.clear();
  //  m_vertexPosError.clear();

  // Declare properties
  declareProperty( "InputProtoP", m_input = ProtoParticleLocation::Neutrals);
  declareProperty( "ParticleNames", m_particleNames );
  declareProperty( "ConfLevelCuts", m_confLevels );
  declareProperty("DetEcal",m_nameEcal);
  declareProperty("DetSpd" ,m_nameSpd);
  declareProperty("DetPrs" ,m_namePrs);
  //  declareProperty( "VertexPositionError",m_vertexPosError );
}
//=============================================================================
// Destructor
//=============================================================================
NeutralParticleMaker::~NeutralParticleMaker( ) { };

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode NeutralParticleMaker::initialize() {
  MsgStream logbk(msgSvc(), name());
  logbk << MSG::DEBUG << "==> NeutralParticleMaker:Initialising" << endreq;
  
  StatusCode sc;
  
  // Retrieve the data service
  sc = service("EventDataSvc", m_EDS, true);
  if( sc.isFailure() ) {
    logbk << MSG::FATAL << "    Unable to locate Event Data Service"
        << endreq;
    return sc;
  }

  // Retrieve the data service
  sc = service("DetectorDataSvc", m_DDS, true);
  if( sc.isFailure() ) {
    logbk << MSG::FATAL << "    Unable to locate Event Data Service"
        << endreq;
    return sc;
  }
  
  // Access the ParticlePropertySvc to retrieve pID for wanted particles
  logbk << MSG::DEBUG << "    Looking for Particle Property Service." << endreq;
  
  sc = service("ParticlePropertySvc", m_ppSvc, true);
  if( sc.isFailure() ) {
    logbk << MSG::FATAL << "    Unable to locate Particle Property Service"
        << endreq;
    return sc;
  }  
  
 
  // Particle Names and ID : Neutral and merged Pi0
  ParticleProperty* gammaProp = m_ppSvc->find( "gamma" );

  if ( 0 == gammaProp )   {
    logbk << MSG::ERROR << "Cannot retrieve properties for gamma"<<endreq;
    return StatusCode::FAILURE;
  }
  m_gammaID=gammaProp->jetsetID();

  ParticleProperty* pi0Prop = m_ppSvc->find( "pi0" );
  if ( 0 == pi0Prop )   {
    logbk << MSG::ERROR << "Cannot retrieve properties for pi0"<<endreq;
    return StatusCode::FAILURE;
  }
  m_pi0ID=pi0Prop->jetsetID();

  // Test the ParticleNames Vector 
  if (m_particleNames.size() == 0) {
    logbk << MSG::ERROR << " ParticleNames is empty. "  
        << "Please, initialize it in your job options file" <<  endreq;
    return StatusCode::FAILURE;
  }

  if(m_particleNames[0] != "All" ){
    
    // Test the Confidence Level Cuts Vector 
    if (m_confLevels.size() == 0) {
      logbk << MSG::ERROR << "PhysDesktopConfLevelCuts is empty. " 
          << "Please, initialize it in your job options file " << endreq;
      return StatusCode::FAILURE;
    }
    
    // Test if the the Confidence Level Cuts Vector and ParticleNames 
    // have the same size
    if (m_confLevels.size() != m_particleNames.size() ) {
      logbk << MSG::ERROR << "PhysDesktopConfLevelCuts size is  " 
          << "different from  PhysDesktopParticleNames" << endreq;
      return StatusCode::FAILURE;
    }

    std::vector<std::string>::const_iterator ipartsName;
    std::vector<double>::const_iterator iConfLevel=m_confLevels.begin();
    for ( ipartsName = m_particleNames.begin(); 
          ipartsName != m_particleNames.end(); ++ipartsName ) {
      ParticleProperty* partProp = m_ppSvc->find( *ipartsName );
      if ( 0 == partProp )   {
        logbk << MSG::ERROR << "Cannot retrieve properties for particle \"" 
	      << *ipartsName << "\" " << endreq;
        return StatusCode::FAILURE;
      }
      if( partProp->charge() == 0 ){
        std::pair<int,double> id(partProp->jetsetID(),(*iConfLevel++));
        m_ids.push_back(id);
      }
      logbk << MSG::DEBUG << " Particle Requested: Name = " << (*ipartsName) 
	    << " PID = " << partProp->jetsetID() << endreq;
    }
    
  }  //if(m_particleNames[0] != "All")

  // ECAL
  SmartDataPtr<DeCalorimeter> detecal (detSvc(),m_nameEcal);
  if(!detecal)
    logbk<<MSG::ERROR<<"Unable to retrieve ECAL detector "
       <<m_nameEcal<<endreq;
  if (!detecal){ return StatusCode::FAILURE ;}

  // SPD
  SmartDataPtr<DeCalorimeter> detspd (detSvc(),m_nameSpd);
  if(!detspd)
    logbk<<MSG::ERROR<<"Unable to retrieve SPD detector "
       <<m_nameSpd<<endreq;
  if (!detspd){    return StatusCode::FAILURE ; }
  
  // PRS
  SmartDataPtr<DeCalorimeter> detprs (detSvc(),m_namePrs);
  if(!detprs)
    logbk<<MSG::ERROR<<"Unable to retrieve PRS detector "
	 <<m_namePrs<<endreq;
  if (!detprs){ return StatusCode::FAILURE ; }
  
  // Convert Detectors to DeCalorimeter
  m_detEcal = (DeCalorimeter*) detecal;
  m_detSpd = (DeCalorimeter*) detspd;
  m_detPrs = (DeCalorimeter*) detprs;

  const IGeometryInfo* geoinf = m_detEcal->geometry() ;
  if( 0 == geoinf ) { logbk<<MSG::ERROR<<"IGeotryInfo is not available!"<<endreq; }
  // center of the detector in mother reference frame 
  HepPoint3D center = geoinf->toGlobal( HepPoint3D() );
  m_zEcal = center.z() + m_detEcal->zShowerMax ();
  logbk<<"Ecal z position:"<<m_zEcal<<endreq;

  const IGeometryInfo* geoinfSpd = m_detSpd->geometry() ;
  if( 0 == geoinfSpd ) { 
    logbk<<MSG::ERROR<<"IGeotryInfo is not available!"<<endreq;
  }
  // center of the detector in mother reference frame 
  HepPoint3D centerSpd = geoinfSpd->toGlobal( HepPoint3D() );
  m_zSpd = centerSpd.z();

  m_shiftSpd=30.5;
  logbk<<MSG::INFO<<"Spd scintillateur z position : shift="<<m_shiftSpd<<endreq;

  const IGeometryInfo* geoinfPrs = m_detPrs->geometry() ;
  if( 0 == geoinfPrs ) { 
    logbk<<MSG::ERROR<<"IGeotryInfo is not available!"<<endreq;
  }
  // center of the detector in mother reference frame 
  HepPoint3D centerPrs = geoinfPrs->toGlobal( HepPoint3D() );
  m_zPrs = centerPrs.z();


  // Vertex Position / Error

  m_x0=0.;
  m_y0=0.;
  m_z0=0.;
  m_vertexErr_x=0.07;
  m_vertexErr_y=0.07;
  m_vertexErr_z=50.25;
  logbk << MSG::INFO <<"Vertex position [x,y,z]"
	<<m_x0<<" "<<m_y0<<" "<<m_z0<<endreq;
  logbk << MSG::INFO <<"Vertex position Error [x,y,z]: "
	<<m_vertexErr_x<<" "<<m_vertexErr_y<<" "<<m_vertexErr_z
	<<endreq;

  return StatusCode::SUCCESS;
  
}


//=============================================================================
// Pointer to the Event Data service
//=============================================================================
IDataProviderSvc* NeutralParticleMaker::eventSvc() const
{
  return m_EDS;
}

//=============================================================================
// Pointer to the Event Data service
//=============================================================================
IDataProviderSvc* NeutralParticleMaker::detSvc() const
{
  return m_DDS;
}


//=============================================================================
// Selection
//=============================================================================
double NeutralParticleMaker::applySelection( SmartRef<ProtoParticle> cand ) {

  MsgStream  logbk( msgSvc(), name() );

  // get Ecal CaloHypo
  const SmartRef<CaloHypo> hypo = *(cand->calo().begin());

  // V.B. 
  if( hypo->hypothesis() != CaloHypotheses::Photon ) { return -100; }
  
  // get Ecal cluster
  const CaloHypo::Clusters& clus = hypo->clusters() ;
  
  // iterators 
  typedef CaloHypo::Clusters::const_iterator cluster ;
  
  if( clus.empty() ) { return -1; }  

  /*
    cluster cl = 
    ( 1 == clus.size() ) ? clus.begin() : 
    std::find_if( clus.begin () , 
    clus.end   () , DeCalorimeterLocation::Ecal );
    if( clus.end() == cl    ) { continue ; }
  */
  
  if (clus.size()!=1) {
    logbk<<MSG::ERROR<<" Hypo has more or less than one Cluster ! "<<endreq;
  }
  
  cluster cl=clus.begin();
  
  //get cluster seed
  CaloCluster::Entries::const_iterator iseed = 
    ClusterFunctors::
    locateDigit( (*cl)->entries().begin() ,  
		 (*cl)->entries().end  () ,  
		 CaloDigitStatus::SeedCell  ) ;
  if( (*cl)->entries().end() == iseed ) 
    { 
      logbk<<MSG::ERROR<<" The Seed Cell is not found! "<<endreq;
    }
  ///
  
  const CaloDigit* seed = iseed->digit();
  if( 0 == seed ) 
    {
      logbk<<MSG::ERROR<<" The Seed Digit points to NULL! "<<endreq;
    }
  
  // SPD - PRS information
  double eSpd=0.;
  double ePrs=0.;
  
  const CaloPosition *pos = hypo->position();
  const  HepPoint3D position (pos->x(),pos->y(),pos->z());
  logbk<<MSG::DEBUG<<"hypothesis position: "
       <<pos->x()<<" "
       <<pos->y()<<" "
       <<pos->z()<<endreq;
  
  HepPoint3D ptSpd,ptPrs; 
  
  ptSpd  =  ( position - m_vertex );
  ptSpd *=  ( m_zSpd + m_shiftSpd - m_vertex.z() ) / 
    ( position.z() - m_vertex.z() );
  ptSpd +=  m_vertex ;
  logbk<<MSG::DEBUG<<"Spd point: "
       <<ptSpd.x()<<" "
       <<ptSpd.y()<<" "
       <<ptSpd.z()<<endreq;
  
  const CaloCellID cellSpd = m_detSpd->Cell( ptSpd );
  
  
  ptPrs  = ( position - m_vertex );
  ptPrs *=  ( m_zPrs - m_vertex.z() ) / 
    ( position.z() - m_vertex.z() );
  ptPrs +=  m_vertex ;
  logbk<<MSG::DEBUG<<"Prs point: "
       <<ptPrs.x()<<" "
       <<ptPrs.y()<<" "
       <<ptPrs.z()<<endreq;
  
  const CaloCellID cellPrs = m_detPrs->Cell( ptPrs );
  
  // Look at Spd
  
  if( !(CaloCellID() == cellSpd) )  // valid cell! 
    {          
      for (SmartRefVector<CaloDigit>::const_iterator 
	     digit=hypo->digits().begin() ;
	   digit != hypo->digits().end() ; digit++ ) {
	if ( (*digit)->cellID() == cellSpd ) {
	  eSpd=(*digit)->e();
	  logbk<<MSG::DEBUG<<"SPD digit Position : "
	       <<" ("<<m_detSpd->cellX(cellSpd)<<","
	       <<m_detSpd->cellY(cellSpd)<<")"<<endreq;
	}
      }
    }
  
  // Look at Prs	
  
  if( !(CaloCellID() == cellPrs) )  // valid cell! 
    {    
      for (SmartRefVector<CaloDigit>::const_iterator 
	     digit=hypo->digits().begin() ;
	   digit != hypo->digits().end() ; digit++ ) {
	if ( (*digit)->cellID() == cellPrs ) {
	  ePrs=(*digit)->e();
	  logbk<<MSG::DEBUG<<"Prs digit Position : "
	       <<" ("<<m_detPrs->cellX(cellPrs)<<","
	       <<m_detPrs->cellY(cellPrs)<<") "
	       <<endreq;
	}
      }
    }
  
  logbk << MSG::DEBUG << "E[PRS]=" << ePrs <<
    " - E[SPD]=" << eSpd << endreq;
  

  double confL=0.;      
  
  if (eSpd<1. && ePrs>0.) confL=1.;
  if (eSpd>1. && ePrs>10.) confL=.5;

  logbk << MSG::DEBUG << "Evaluated ConfidenceLevel= " << confL << endreq;

  return confL; 
}



//=============================================================================
// Main execution
//=============================================================================
StatusCode NeutralParticleMaker::makeParticles( ParticleVector & parts ) {
  
  MsgStream  logbk( msgSvc(), name() );
  
  logbk << MSG::DEBUG << "==> NeutralParticleMaker::makeParticles() is running." 
	<< endreq;

  int protoID;
  double protoCL;
  
  // make Neutral particles:
  int nGammaParticles=0;   // Counter of Gamma particles created.
  
  SmartDataPtr<ProtoParticles> candidates ( eventSvc(),m_input);
  if ( !candidates ) {
    logbk << MSG::ERROR << "No ProtoParticle container found in "  
          << m_input << endreq;
    return StatusCode::FAILURE;
  }
  
  if ( 0 == candidates->size() ) { 
    logbk << MSG::INFO << "No ProtoParticles retrieved from "  
          << m_input << endreq;
    return StatusCode::SUCCESS;
  }
  
  // Logbk number of ProtoPartCandidates retrieved
  logbk << MSG::DEBUG << "Number of Neutral ProtoParticles retrieved  = "
        << candidates->size() << endreq;

  m_vertex.setX(m_x0);
  m_vertex.setY(m_y0);
  m_vertex.setY(m_z0);
  
  if( "All" == m_particleNames[0] ){
    
    logbk << MSG::DEBUG << "Making all particles " << endreq;
    
    for( ProtoParticles::const_iterator icand = candidates->begin();
         icand != candidates->end(); icand++){
      
      /*
	std::vector< std::pair<int,double> >::const_iterator infoDet;
	for (infoDet=(*icand)->pIDDetectors().begin();
	infoDet != (*icand)->pIDDetectors().end() ; ++infoDet) {
        logbk<< MSG::DEBUG << "Info Detector=" << (*infoDet).first << " "
	<< (*infoDet).second<<endreq;
        //protoCL = (*icand)->detPIDvalue(ProtoParticle::CaloTrMatch);
	}
      */
      
      protoID=m_gammaID;
      
      logbk << MSG::DEBUG << "\n " << "Processing New Particle Candidate (protoID=" << 
	protoID << ")" << endreq;

      SmartRef<ProtoParticle> phCand=*icand; 

      protoCL=0.;

      if (0!=phCand) { protoCL=applySelection(phCand); }

      if (protoCL<0.) {	continue; }
      
      Particle* particle = new Particle();
      
      StatusCode sc = fillParticle( *icand, protoID, protoCL, particle);

      if( sc.isFailure() ) {
        delete particle;
      }
      else {  
        // Insert particle in particle vector.
	logbk << MSG::DEBUG << "Storing Particle [" 
	      << protoID << "] -> ConfLevel=" << particle->confLevel() << endreq; 
        parts.push_back(particle);
        nGammaParticles++;
      }  
    }
  }
  
  else {
    if( m_ids.size() !=0 ) {
      
      // Loop over all ProtoParticles and fill Particle if the ProtoParticle 
      // satisfies the PID and corresponding CL cut  :

      for(ProtoParticles::iterator icand = candidates->begin();
          icand != candidates->end();icand++){  

        // Iterator on requested PID's

        std::vector<std::pair<int,double> >::iterator iWantedPID; 
        for ( iWantedPID = m_ids.begin();
              iWantedPID != m_ids.end(); 
              ++iWantedPID ) {

          protoID=m_gammaID;
	  logbk << MSG::DEBUG << " Processing New Particle Candidate (protoID=" << 
	    protoID << ")" << endreq;
	  
	  SmartRef<ProtoParticle> phCand=*icand; 
	  
	  protoCL=0.;
	  
	  if (0!=phCand) { protoCL=applySelection(phCand); }

	  if (protoCL<0.) {	continue; }

	  if ( protoID  == (*iWantedPID).first && 
	       protoCL >= (*iWantedPID).second  ){ 
	    // We have a good candidate to this iWantedPID:
	    Particle* particle = new Particle();
	    StatusCode sc = fillParticle(*icand, protoID, protoCL, particle);

	    if( sc.isFailure() ) {
	      delete particle;
	    }
	    else {  
              // Insert particle in particle vector.
	      logbk << MSG::DEBUG << "Storing Particle [" 
		    << protoID << "] -> ConfLevel=" << particle->confLevel() << endreq; 
              parts.push_back(particle);
              nGammaParticles++;
            }
          }
        }
      }
    }
  }

  logbk << MSG::DEBUG << "Number of Particles created: " << nGammaParticles <<endreq;
  
  return StatusCode::SUCCESS;
  
}
//=========================================================================
// fill particles
//========================================================================= 
StatusCode NeutralParticleMaker::fillParticle( const ProtoParticle* protoCand,
                                               int protoID, double protoCL,
                                               Particle* particle ) {
  
  MsgStream  logbk( msgSvc(), name() );

  // Start filling the particle:     
  particle->setParticleID( protoID );
  particle->setConfLevel( protoCL );
  // Get mass from Particle Property Service to calculate the 4-momentum
  ParticleProperty* partProp = m_ppSvc->findByStdHepID( protoID );     
  double mass = (*partProp).mass();
  particle->setMass(mass);
  particle->setMassErr(0.0); // For the moment but already in constructor
  particle->setIsResonance(false);  // Already in constructor

  if( protoCand->calo().size() != 1 ) {
    logbk<<MSG::DEBUG<<"ProtoParticle has "<<protoCand->calo().size()
         <<" calohypo !"<<endreq;
    return StatusCode::FAILURE;
  }

  SmartRef<CaloHypo> hypo=*(protoCand->calo().begin());

  // set position: will need to set position at same place momentum is
  // given
  /*
    // If Position of photon is defined by measurement on the calo front-face
  HepPoint3D position( hypo->position()->x(),
                       hypo->position()->y(),
                       hypo->position()->z() ) ;
  */
  //Photon 'position' is now defined by the vertex position... (0.,0.,0.) up to now!
  HepPoint3D position(0.,0.,0.);

  particle->setPointOnTrack( position );
  

  logbk << MSG::DEBUG << "position = " << position << endreq;

  // set four momentum
  HepLorentzVector quadriMomentum;
  quadriMomentum.setPx( hypo->momentum()->momentum().x() );
  quadriMomentum.setPy( hypo->momentum()->momentum().y() );
  quadriMomentum.setPz( hypo->momentum()->momentum().z() );
  quadriMomentum.setE( hypo->momentum()->momentum().e() );
  particle->setMomentum( quadriMomentum );
  logbk << MSG::DEBUG << "momentum = " << quadriMomentum << endreq;
  
  ////////////////////////////////////////////////////////////////////////
  // Error Matrix Element calculations

  const HepSymMatrix& posCov = hypo->position()->covariance();   
  double e=quadriMomentum.e();

  double x=hypo->position()->x();
  double y=hypo->position()->y();
  double z=hypo->position()->z();
  double x0=m_x0;
  double y0=m_y0;
  double z0=m_z0;

  HepVector3D momentum(x-x0,y-y0,z-z0);
  double r2=momentum.mag2();
  double r=sqrt(r2);
  double r3=r*r2;
  double ir=1./r;

  momentum*=(e/r);

  double dpx_dx  = e * (  ir - (x-x0)*(x-x0)/r3);
  double dpx_dy  = e * (     - (x-x0)*(y-y0)/r3);
  double dpx_dz  = 0.;
  double dpx_de  = (x-x0)  /r;
  double dpx_dx0 = e * ( -ir + (x-x0)*(x-x0)/r3);
  double dpx_dy0 = e * (     + (x-x0)*(y-y0)/r3);
  double dpx_dz0 = e * (     + (x-x0)*(z-z0)/r3);
  
  double dpy_dx  = e * (  ir - (y-y0)*(x-x0)/r3);
  double dpy_dy  = e * (     - (y-y0)*(y-y0)/r3);
  double dpy_dz  = 0.;
  double dpy_de  = (y-y0)  /r;
  double dpy_dx0 = e * ( -ir + (y-y0)*(x-x0)/r3);
  double dpy_dy0 = e * (     + (y-y0)*(y-y0)/r3);
  double dpy_dz0 = e * (     + (y-y0)*(z-z0)/r3);
  
  double dpz_dx  = e * (  ir - (z-z0)*(x-x0)/r3);
  double dpz_dy  = e * (     - (z-z0)*(y-y0)/r3);
  double dpz_dz  = 0.;
  double dpz_de  = (z-z0)  /r;
  double dpz_dx0 = e * ( -ir + (z-z0)*(x-x0)/r3);
  double dpz_dy0 = e * (     + (z-z0)*(y-y0)/r3);
  double dpz_dz0 = e * (     + (z-z0)*(z-z0)/r3);

  double cov_xx=posCov(CaloPosition::X,CaloPosition::X);
  double cov_xy=posCov(CaloPosition::X,CaloPosition::Y);
  double cov_xe=posCov(CaloPosition::X,CaloPosition::E);
  double cov_yy=posCov(CaloPosition::Y,CaloPosition::Y);
  double cov_ye=posCov(CaloPosition::Y,CaloPosition::E);
  double cov_ee=posCov(CaloPosition::E,CaloPosition::E);
  
  double cov_ze=0.; 

  //================================================================================
  
  double cov_ex0= 0.; 
  double cov_ey0= 0.;  
  double cov_ez0= 0.;

  //================================================================================

  double cov_x0x0= m_vertexErr_x*m_vertexErr_x;
  double cov_x0y0= 0.;
  double cov_x0z0= 0.; 
  
  //================================================================================
  
  double cov_y0x0=cov_x0y0;
  double cov_y0y0= m_vertexErr_y*m_vertexErr_y;
  double cov_y0z0= 0.;

  //================================================================================

  double cov_z0x0=cov_x0z0;
  double cov_z0y0=cov_y0z0;
  double cov_z0z0= m_vertexErr_z*m_vertexErr_z;
  
  //================================================================================
  
  double cov_pxpx=
     dpx_dx  * dpx_dx  * cov_xx+
2. * dpx_dx  * dpx_dy  * cov_xy+
2. * dpx_dx  * dpx_de  * cov_xe+
     dpx_dy  * dpx_dy  * cov_yy+
2. * dpx_dy  * dpx_de  * cov_ye+
     dpx_de  * dpx_de  * cov_ee+

     dpx_dx0 * dpx_dx0 * cov_x0x0+
2. * dpx_dx0 * dpx_dy0 * cov_x0y0+
2. * dpx_dx0 * dpx_dz0 * cov_x0z0+
     dpx_dy0 * dpx_dy0 * cov_y0y0+
2. * dpx_dy0 * dpx_dz0 * cov_y0z0+
     dpx_dz0 * dpx_dz0 * cov_z0z0 ;
  
  double cov_pxpy=
     dpx_dx  * dpy_dx  * cov_xx+
     dpx_dx  * dpy_dy  * cov_xy+
     dpx_dy  * dpy_dx  * cov_xy+
     dpx_dx  * dpy_de  * cov_xe+
     dpx_de  * dpy_dx  * cov_xe+
     dpx_dy  * dpy_dy  * cov_yy+
     dpx_dy  * dpy_de  * cov_ye+
     dpx_de  * dpy_dy  * cov_ye+
     dpx_de  * dpy_de  * cov_ee+

     dpx_dx0 * dpy_dx0 * cov_x0x0+
     dpx_dx0 * dpy_dy0 * cov_x0y0+
     dpx_dy0 * dpy_dx0 * cov_x0y0+
     dpx_dx0 * dpy_dz0 * cov_x0z0+
     dpx_dz0 * dpy_dx0 * cov_x0z0+
     dpx_dy0 * dpy_dy0 * cov_y0y0+
     dpx_dy0 * dpy_dz0 * cov_y0z0+
     dpx_dz0 * dpy_dy0 * cov_y0z0+
     dpx_dz0 * dpy_dz0 * cov_z0z0 ;

  double cov_pxpz=
     dpx_dx  * dpz_dx  * cov_xx+
     dpx_dx  * dpz_dy  * cov_xy+
     dpx_dy  * dpz_dx  * cov_xy+
     dpx_dx  * dpz_de  * cov_xe+
     dpx_de  * dpz_dx  * cov_xe+
     dpx_dy  * dpz_dy  * cov_yy+
     dpx_dy  * dpz_de  * cov_ye+
     dpx_de  * dpz_dy  * cov_ye+
     dpx_de  * dpz_de  * cov_ee+

     dpx_dx0 * dpz_dx0 * cov_x0x0+
     dpx_dx0 * dpz_dy0 * cov_x0y0+
     dpx_dy0 * dpz_dx0 * cov_x0y0+
     dpx_dx0 * dpz_dz0 * cov_x0z0+
     dpx_dz0 * dpz_dx0 * cov_x0z0+
     dpx_dy0 * dpz_dy0 * cov_y0y0+
     dpx_dy0 * dpz_dz0 * cov_y0z0+
     dpx_dz0 * dpz_dy0 * cov_y0z0+
     dpx_dz0 * dpz_dz0 * cov_z0z0 ;

  double cov_pxe=
     dpx_dx * cov_xe +
     dpx_dy * cov_ye +
     dpx_dz * cov_ze +
     dpx_de * cov_ee ;

  double cov_pxx0=
     dpx_dx0 * cov_x0x0+
     dpx_dy0 * cov_x0y0+
     dpx_dz0 * cov_x0z0 ;

  double cov_pxy0=
     dpx_dx0 * cov_y0x0+
     dpx_dy0 * cov_y0y0+
     dpx_dz0 * cov_y0z0 ;

  double cov_pxz0=
     dpx_dx0 * cov_z0x0+
     dpx_dy0 * cov_z0y0+
     dpx_dz0 * cov_z0z0 ; 


//================================================================================

  double cov_pypy=
     dpy_dx  * dpy_dx  * cov_xx+
2. * dpy_dx  * dpy_dy  * cov_xy+
2. * dpy_dx  * dpy_de  * cov_xe+
     dpy_dy  * dpy_dy  * cov_yy+
2. * dpy_dy  * dpy_de  * cov_ye+
     dpy_de  * dpy_de  * cov_ee+

     dpy_dx0 * dpy_dx0 * cov_x0x0+
2. * dpy_dx0 * dpy_dy0 * cov_x0y0+
2. * dpy_dx0 * dpy_dz0 * cov_x0z0+
     dpy_dy0 * dpy_dy0 * cov_y0y0+
2. * dpy_dy0 * dpy_dz0 * cov_y0z0+
     dpy_dz0 * dpy_dz0 * cov_z0z0 ;

  double cov_pypz=
     dpy_dx  * dpz_dx  * cov_xx+
     dpy_dx  * dpz_dy  * cov_xy+
     dpy_dy  * dpz_dx  * cov_xy+
     dpy_dx  * dpz_de  * cov_xe+
     dpy_de  * dpz_dx  * cov_xe+
     dpy_dy  * dpz_dy  * cov_yy+
     dpy_dy  * dpz_de  * cov_ye+
     dpy_de  * dpz_dy  * cov_ye+
     dpy_de  * dpz_de  * cov_ee+

     dpy_dx0 * dpz_dx0 * cov_x0x0+
     dpy_dx0 * dpz_dy0 * cov_x0y0+
     dpy_dy0 * dpz_dx0 * cov_x0y0+
     dpy_dx0 * dpz_dz0 * cov_x0z0+
     dpy_dz0 * dpz_dx0 * cov_x0z0+
     dpy_dy0 * dpz_dy0 * cov_y0y0+
     dpy_dy0 * dpz_dz0 * cov_y0z0+
     dpy_dz0 * dpz_dy0 * cov_y0z0+
     dpy_dz0 * dpz_dz0 * cov_z0z0 ;

  double cov_pye=
     dpy_dx * cov_xe +
     dpy_dy * cov_ye +
     dpy_dz * cov_ze +
     dpy_de * cov_ee ;

  double cov_pyx0=
     dpy_dx0 * cov_x0x0+
     dpy_dy0 * cov_x0y0+
     dpy_dz0 * cov_x0z0 ;

  double cov_pyy0=
     dpy_dx0 * cov_y0x0+
     dpy_dy0 * cov_y0y0+
     dpy_dz0 * cov_y0z0 ;

  double cov_pyz0=
     dpy_dx0 * cov_z0x0+
     dpy_dy0 * cov_z0y0+
     dpy_dz0 * cov_z0z0 ;

//================================================================================

  double cov_pzpz=
     dpz_dx  * dpz_dx  * cov_xx+
2. * dpz_dx  * dpz_dy  * cov_xy+
2. * dpz_dx  * dpz_de  * cov_xe+
     dpz_dy  * dpz_dy  * cov_yy+
2. * dpz_dy  * dpz_de  * cov_ye+
     dpz_de  * dpz_de  * cov_ee+

     dpz_dx0 * dpz_dx0 * cov_x0x0+
2. * dpz_dx0 * dpz_dy0 * cov_x0y0+
2. * dpz_dx0 * dpz_dz0 * cov_x0z0+
     dpz_dy0 * dpz_dy0 * cov_y0y0+
2. * dpz_dy0 * dpz_dz0 * cov_y0z0+
     dpz_dz0 * dpz_dz0 * cov_z0z0 ;

  double cov_pze=
     dpz_dx * cov_xe +
     dpz_dy * cov_ye +
     dpz_dz * cov_ze +
     dpz_de * cov_ee ;

  double cov_pzx0=
     dpz_dx0 * cov_x0x0+
     dpz_dy0 * cov_x0y0+
     dpz_dz0 * cov_x0z0 ;

  double cov_pzy0=
     dpz_dx0 * cov_y0x0+
     dpz_dy0 * cov_y0y0+
     dpz_dz0 * cov_y0z0 ;

  double cov_pzz0=
     dpz_dx0 * cov_z0x0+
     dpz_dy0 * cov_z0y0+
     dpz_dz0 * cov_z0z0 ;

//================================================================================

// Photon momentum depends on Vertex position and Measured position (Calo Front face).
// By convention, what is defined as 'Position' is the Vertex Position ie (0,0,0). 
// The Momentum error are defined taking into account the error on the measured 
// position (calo front face). But the correlation are evaluated with respect to the 
// 'Position' which is defined as the vertex position.

  // Set pointOnTrackErr: (Error on x, y and z)
  // Defined as Vertex Position Error:
  HepSymMatrix pointOnTrackErr(3, 0);
  pointOnTrackErr(1,1) = cov_x0x0;
  pointOnTrackErr(2,1) = cov_y0x0;
  pointOnTrackErr(2,2) = cov_y0y0;
  pointOnTrackErr(3,1) = cov_z0x0;
  pointOnTrackErr(3,2) = cov_z0y0;
  pointOnTrackErr(3,3) = cov_z0z0;
  particle->setPointOnTrackErr(pointOnTrackErr);

  logbk << MSG::DEBUG << "pointOnTrackErr"           << endreq;
  logbk << MSG::DEBUG << particle->pointOnTrackErr()  << endreq;
  
  // Set Momentum Error:
  HepSymMatrix MomentumErr(4, 0);
  MomentumErr(1,1) = cov_pxpx;
  MomentumErr(2,1) = cov_pxpy;
  MomentumErr(2,2) = cov_pypy;
  MomentumErr(3,1) = cov_pxpz;
  MomentumErr(3,2) = cov_pypz;
  MomentumErr(3,3) = cov_pzpz;
  MomentumErr(4,1) = cov_pxe;
  MomentumErr(4,2) = cov_pye;
  MomentumErr(4,3) = cov_pze;
  MomentumErr(4,4) = cov_ee; 

  particle->setMomentumErr( MomentumErr );
  logbk << MSG::DEBUG << "part momErr"           << endreq;
  logbk << MSG::DEBUG << particle->momentumErr() << endreq;
  
  // Set position-momentum correlation matrix. 
  HepMatrix posMomCorr(4, 3, 0);
  posMomCorr(1,1) = cov_pxx0; 
  posMomCorr(1,2) = cov_pxy0; 
  posMomCorr(1,3) = cov_pxz0; 
  posMomCorr(2,1) = cov_pyx0; 
  posMomCorr(2,2) = cov_pyy0; 
  posMomCorr(2,3) = cov_pyz0; 
  posMomCorr(3,1) = cov_pzx0; 
  posMomCorr(3,2) = cov_pzy0; 
  posMomCorr(3,3) = cov_pzz0; 
  posMomCorr(4,1) = cov_ex0; 
  posMomCorr(4,2) = cov_ey0; 
  posMomCorr(4,3) = cov_ez0; 
  particle->setPosMomCorr(posMomCorr);

  logbk << MSG::DEBUG << "posMom correlation"   << endreq;
  logbk << MSG::DEBUG << particle->posMomCorr() << endreq;

  ///////////////////////////////////////////////////////////////////////////
  
  particle->setOrigin(protoCand);
  return StatusCode::SUCCESS;
} 
  
//==========================================================================
//  Finalize
//==========================================================================
StatusCode NeutralParticleMaker::finalize() {
    
  MsgStream logbk(msgSvc(), name());
  logbk << MSG::DEBUG << "==> NeutralParticleMaker::finalizing" << endreq;
  
  return StatusCode::SUCCESS;
}
//=========================================================================
