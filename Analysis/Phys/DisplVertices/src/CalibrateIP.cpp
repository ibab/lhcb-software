// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/PhysicalConstants.h"


#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "Event/TwoProngVertex.h"


#include "LHCbMath/EigenSystem.h"   
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/GenericVectorTypes.h"

#include "AIDA/IProfile1D.h"
#include "AIDA/IAxis.h"
#include "GaudiUtils/Aida2ROOT.h"

#include "VeloDet/DeVelo.h"

#include "TF1.h"

// local
#include "CalibrateIP.h"

using namespace LHCb ;
using namespace std ;
//-----------------------------------------------------------------------------
// Implementation file for class : CalibrateIP
//
// 2009-09-17 : Neal Gauvin
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CalibrateIP );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CalibrateIP::CalibrateIP( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator )
    , m_vertexer("TrackVertexer")
    , m_nbevent(0)
    , m_bin(500)
    , m_beamstab(0.05)
{
  declareProperty( "PVContainer", m_PVContainerName = LHCb::RecVertexLocation::Primary ) ;
  declareProperty( "OutputLocation", m_outloc = "/Event/"+name+"/BeamLine" );
  declareProperty( "RefitPVwithLongOnly", m_refit = false );
  declareProperty( "LocalVeloFrame", m_local = false );
  declareProperty( "IPbyHistFit", m_histo = false );
  declareProperty( "SmearPV", m_smear = false );
  declareProperty( "MinZ", m_minz = -25*Gaudi::Units::cm );
  declareProperty( "MaxZ", m_maxz = 75*Gaudi::Units::cm );
  declareProperty( "MaxY", m_maxy = 5*Gaudi::Units::mm );
  declareProperty( "MaxX", m_maxx = 5*Gaudi::Units::mm );
  declareProperty( "MaxChi2ODoF", m_maxchi = 2 );
  declareProperty( "MinNbLongTrks", m_minlongs = 6 );
  declareProperty( "MinNbTrks", m_mintrks = 10 );
  declareProperty("BeamLineInitPos", m_BLInitPos ); 
  declareProperty("BeamLineInitDir", m_BLInitDir );
  declareProperty("BeamLineCycle", m_cycle = 1000 );
}
//=============================================================================
// Destructor
//=============================================================================
CalibrateIP::~CalibrateIP() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CalibrateIP::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  //Get a Beam Line Info class to store Beam calibration data
  m_Beam = new BeamLineInfo();

  //get track vertexer
  sc = m_vertexer.retrieve() ;

  //get the Velo geometry
  string velo = "/dd/Structure/LHCb/BeforeMagnetRegion/Velo/Velo";
  const IDetectorElement* lefthalv = getDet<IDetectorElement>( velo + "Left" );
  const IDetectorElement* righthalv = 
    getDet<IDetectorElement>( velo + "Right" );
  const IGeometryInfo* halfgeominfo = lefthalv->geometry() ;
  //check that Velo is closed
  Gaudi::XYZPoint localorigin(0,0,0);
  Gaudi::XYZPoint leftcenter = lefthalv->geometry()->toGlobal(localorigin) ;
  Gaudi::XYZPoint rightcenter = righthalv->geometry()->toGlobal(localorigin) ;
  if( abs(leftcenter.x() - rightcenter.x())> 0.1 *Gaudi::Units::mm ) {
    info() << "Velo not closed, work in global frame" << endmsg;
    m_local = false;
  }

  //matrix to transform to local velo frame
  m_toVeloFrame = halfgeominfo->toLocalMatrix() ;
  m_toGlobalFrame = halfgeominfo->toGlobalMatrix();

  //Book histos
  m_x = book1D("PV x position", -m_maxx, m_maxx, m_bin) ;
  m_y = book1D("PV y position", -m_maxy, m_maxy, m_bin) ;
  m_z = book1D("PV z position", m_minz, m_maxz, m_bin) ;

  int bins2 = m_bin*m_bin;
  m_xx = book1D("PVxx", 0., (m_maxx*m_maxx), bins2) ;
  m_yy = book1D("PVyy", 0., (m_maxy*m_maxy), bins2) ;
  m_zz = book1D("PVzz", 0. , (m_maxz*m_maxz), bins2) ;
  m_xy = book1D("PVxy", -(m_maxx*m_maxy), (m_maxx*m_maxy), bins2) ;
  m_xz = book1D("PVxz", -(m_maxx*m_maxz), (m_maxx*m_maxz), bins2) ;
  m_yz = book1D("PVyz", -(m_maxy*m_maxz), (m_maxy*m_maxz), bins2) ;

  if( !m_BLInitPos.empty() ){
    //sanity check
    if( m_BLInitPos.size()<3 ){
      info()<<"BeamLineInitPos not set properly. Need 3 doubles. "
            << endmsg;
      return StatusCode::FAILURE;
    }
    m_Beam->SetIPPosition( m_BLInitPos[0], m_BLInitPos[1], m_BLInitPos[2] );
    if(msgLevel(MSG::DEBUG))
      debug()<<"Beam line initial position : "<< m_BLInitPos << endmsg;
  } else if( m_local ){
    m_Beam->SetIPPosition( 0., 0., 0. );
    m_Beam->MoveIPToFrame( m_toVeloFrame );
  } else {
    m_Beam->SetIPPosition( 0., 0., -10000. );
  } 
    
  if( !m_BLInitDir.empty() ){
    //sanity check
    if( m_BLInitDir.size()<3 ){
      info()<<"BeamLineInitDir not set properly. Need 3 doubles. "
            << endmsg;
      return StatusCode::FAILURE;
    }
    m_Beam->SetZEigenVector( m_BLInitDir[0], m_BLInitDir[1], m_BLInitDir[2] );
    if(msgLevel(MSG::DEBUG))
      debug()<<"Beam line initial direction : "<< m_BLInitDir << endmsg;
  } else if( m_local ){
    m_Beam->SetZEigenVector( 0., 0., 1. );
    m_Beam->MoveZEVToFrame( m_toVeloFrame );
  } else {
    m_Beam->SetZEigenVector( 0., 0., 0. ); 
  }
  

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CalibrateIP::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  ++m_nbevent;

  //Get the reconstructed primary vertices
  if (!exist<RecVertices>(m_PVContainerName)) 
    return Warning( m_PVContainerName+" not found", StatusCode::SUCCESS, 0);
  const RecVertices* pvcontainer = get<RecVertices>( m_PVContainerName ) ;
  if( msgLevel(MSG::DEBUG) )
    debug()<< "Number of Reconstructed Vertices " 
           << pvcontainer->size() << endmsg;

  
  //Loop on the reconstructed primary vertices
  for( RecVertices::const_iterator ipv = pvcontainer->begin() ;
       ipv != pvcontainer->end(); ++ipv ) {
    const RecVertex* pv = *ipv ;
    int nbtrks = pv->tracks().size();


    //refit the PV with long tracks only
    vector<const Track*> longtracks;
    GetLongs( pv, longtracks );
    int nblongs = longtracks.size();
    const RecVertex* refitpv = pv;

    if( msgLevel(MSG::DEBUG) || m_refit ){
      if( nblongs < 2 ) continue;
      refitpv = m_vertexer->fit( longtracks );
    }

    //transform the PV position to the local Velo frame
    Gaudi::XYZPoint localPV;
    if( msgLevel(MSG::DEBUG) || m_local ){
      localPV = m_toVeloFrame * pv->position();
    }

    if( msgLevel( MSG::DEBUG ) ){
      debug()<<"Primary Vertex position "<< pv->position() 
             <<" R "<< pv->position().rho()
             <<" Number of associated tracks "<< nbtrks 
             <<" Number of long tracks " << nblongs 
             <<" Chi2/NDoF " << pv->chi2PerDoF() << endmsg;
      debug()<<" PV in Velo local frame "<< localPV << endmsg;
      debug()<<" Refitted PV with long tracks only "<< refitpv->position() 
             <<" R "<< refitpv->position().rho() 
             <<" Chi2/NDoF " << refitpv->chi2PerDoF() << endmsg;
    }

    if( m_refit ) pv = refitpv;      
    Gaudi::XYZPoint PV = pv->position();
    if( m_local ) PV = localPV;

    //Eventually perform an artificial smearing of the vertex...
    if(msgLevel(MSG::DEBUG)) debug()<<"Pos before smearing "<< PV << endmsg;
    if( m_smear ) SmearPV( PV );
    if(msgLevel(MSG::DEBUG)) debug()<<"Pos after smearing "<< PV << endmsg;

    double x = PV.x();
    double y = PV.y();
    double z = PV.z();
    double r = PV.rho();
    double chi = pv->chi2PerDoF();

    //eventually cut on the PV !
    if( chi > m_maxchi || nblongs < m_minlongs || nbtrks < m_mintrks ) 
      continue;
    if( m_histo == false && 
        ( abs(x) > m_maxx || abs(y) > m_maxy || z > m_maxz || z < m_minz ) ) 
      continue;
    ++m_nbpv;

    //plot variables
    m_x->fill( x ); m_y->fill( y ); m_z->fill( z );
    m_xx->fill( x*x ); m_yy->fill( y*y ); m_zz->fill( z*z );
    m_xy->fill( x*y ); m_yz->fill( y*z ); m_xz->fill( x*z );

    plot( r, "PV radial distance to z axis", 0., m_maxx, m_bin ); 
    plot( chi, "PV chisquare per dof",0,5) ;
    plot( nbtrks, "PV number of tracks", -0.5, 120.5, 121 );
    plot( nblongs, "NumLongTracksPerPV", -0.5,99.5,50);
    profile1D( x, chi,"PV x versus chi2/Dof",0.,m_maxx,m_bin );
    profile1D( y, chi,"PV y versus chi2/Dof",0.,m_maxy,m_bin );
    profile1D( z, chi,"PV z versus chi2/Dof", m_minz, m_maxz, m_bin );
    profile1D( nbtrks, chi,"PV nb trks v. chi2/Dof", -0.5, 121.5, 121 );
    profile1D( nblongs, chi,"PV nb longs v. chi2/Dof", -0.5, 101.5, 101 );

  }

  //Update the beam line 
  if( m_nbevent%m_cycle == 0 ){

    m_Beam->SetNbEntries( m_nbpv );

    //Translate AIDA into ROOT
    TH1D* rx = Gaudi::Utils::Aida2ROOT::aida2root( m_x );
    TH1D* ry = Gaudi::Utils::Aida2ROOT::aida2root( m_y );
    TH1D* rz = Gaudi::Utils::Aida2ROOT::aida2root( m_z );
    TH1D* rxx = Gaudi::Utils::Aida2ROOT::aida2root( m_xx );
    TH1D* ryy = Gaudi::Utils::Aida2ROOT::aida2root( m_yy );
    TH1D* rzz = Gaudi::Utils::Aida2ROOT::aida2root( m_zz );
    TH1D* rxy = Gaudi::Utils::Aida2ROOT::aida2root( m_xy );
    TH1D* ryz = Gaudi::Utils::Aida2ROOT::aida2root( m_yz );
    TH1D* rxz = Gaudi::Utils::Aida2ROOT::aida2root( m_xz );
    
    //Get the means
    double mx, my, mz, mxx, myy, mzz, mxy, myz, mxz;

    if(m_histo){

      rx->Fit("gaus","QO","goff"); ry->Fit("gaus","QO","goff");
      rz->Fit("gaus","QO","goff"); 
      //rxx->Fit("gaus","QO","goff"); rxz->Fit("gaus","QO","goff");
      //ryy->Fit("gaus","QO","goff"); rzz->Fit("gaus","QO","goff");
      //rxy->Fit("gaus","QO","goff"); ryz->Fit("gaus","QO","goff");
      
      mx = rx->GetFunction("gaus")->GetParameter(1);
      my = ry->GetFunction("gaus")->GetParameter(1);
      mz = rz->GetFunction("gaus")->GetParameter(1);
      //     mxx = rxx->GetFunction("gaus")->GetParameter(1);
      //     myy = ryy->GetFunction("gaus")->GetParameter(1);
      //     mzz = rzz->GetFunction("gaus")->GetParameter(1);
      //     mxy = rxy->GetFunction("gaus")->GetParameter(1);
      //     myz = ryz->GetFunction("gaus")->GetParameter(1);
      //     mxz = rxz->GetFunction("gaus")->GetParameter(1);
      
      //########### temporary
      mxx = rxx->GetMean();
      myy = ryy->GetMean();
      mzz = rzz->GetMean();
      mxy = rxy->GetMean();
      myz = ryz->GetMean();
      mxz = rxz->GetMean();
      
    } else {
      
      mx = rx->GetMean();
      my = ry->GetMean();
      mz = rz->GetMean();
      mxx = rxx->GetMean();
      myy = ryy->GetMean();
      mzz = rzz->GetMean();
      mxy = rxy->GetMean();
      myz = ryz->GetMean();
      mxz = rxz->GetMean();
      
    }
    
    //Compute the covariance matrix
    Gaudi::SymMatrix3x3 Cov;
    Cov(0,0) = mxx - mx*mx;
    Cov(0,1) = mxy - mx*my;
    Cov(0,2) = mxz - mx*mz;
    Cov(1,2) = myz - my*mz;
    Cov(1,1) = myy - my*my;
    Cov(2,2) = mzz - mz*mz;
    
    //let's diagonalize the matrix
    // create the evaluator 
    Gaudi::Math::GSL::EigenSystem eval ;
    
    // get the sorted vector of eigenvalues and eigenvectors
    Gaudi::Vector3  eigval;
    Gaudi::Matrix3x3 eigvects;
    StatusCode sc = 
      eval.eigenVectors( Cov, eigval, eigvects );
    //fill eigenvalues and eigenvectors in beam calibration class
    if( sc.isSuccess() ){
      m_Beam->SetEigenValues( eigval );
      m_Beam->SetEigenVectors( eigvects );
    } else {
      warning()<<"Failed to get the eigenvalues and eigenvectors of " 
               <<"the IP covariance matrix, saving the estimate of "
               <<"the mean interaction point position."<< endmsg;
    }    
    
    //keep old position
    Gaudi::XYZPoint & IPold = m_Beam->GetIPPosition();
    //Fill new IP in beam calibration class
    m_Beam->SetIPPosition( mx, my, mz );

    //transform back into LHCb frame
    if( m_local && context() == "Info" ){
      info()<<"Average interaction point position in the Velo frame : "
            << m_Beam->GetIPPosition() << endmsg;
      m_Beam->MoveToFrame( m_toGlobalFrame );
    }
    
    //  Set X,Y,Z eigenvectors as being the vectors closest to the X,Y,Z axis, 
    //     respectively, with positive eigenvalues.
    m_Beam->SetOrder();
 
    //Eventually reset if the beams have moved
    if( m_Beam->HasMoved( IPold, m_beamstab ) ){
      m_x->reset();
      m_y->reset();
      m_z->reset();
      m_xx->reset();
      m_yy->reset();
      m_zz->reset();
      m_xy->reset();
      m_xz->reset();
      m_yz->reset();
    }   
  
    //Print out !
    if( context() == "Info" ){
      info()<<"Average interaction point position : "<< m_Beam->PrintIP() 
            <<" done on "<< m_nbpv <<" primary vertices."<< endmsg;
      info()<<"Covariance matrix of the interaction point "
            <<"position distribution : "<< endmsg;
      if ( msgLevel(MSG::DEBUG) ) cout<< Cov <<endl;
      info()<<"X eigenvector "<< m_Beam->GetXEigenVector() 
            <<" Y eigenvector "<< m_Beam->GetYEigenVector() 
            <<" Z eigenvector "<< m_Beam->GetZEigenVector() <<endmsg;
      info()<<"Eigenvalues "<< eigval << endmsg;
    }
    
  }

  //anyway...
  if( m_Beam->GetIPPosition().z() < -1000. ) return StatusCode::SUCCESS;
  
  //Save the beam line on the TES.
  SaveBeamLine();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CalibrateIP::finalize() {

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  delete m_Beam;

  return m_vertexer.release() && GaudiHistoAlg::finalize();
}

//=============================================================================
//  Return a vector of long tracks only
//=============================================================================
void CalibrateIP::GetLongs( const RecVertex* pv, 
                            vector<const Track*> & vec ){

  
  SmartRefVector< Track >::const_iterator i = pv->tracks().begin();
  SmartRefVector< Track >::const_iterator iend = pv->tracks().end();
  for( ; i != iend; ++i ){
    if ( (*i)->checkType( Track::Long ) ) vec.push_back( i->target() );
  }  
}

//=============================================================================
//  Artificially smear the PV position
//=============================================================================
void CalibrateIP::SmearPV( Gaudi::XYZPoint & PV ){
  
  //Rotate it !
  double pi = 3.1415926535897931;
  double angle = pi/210;
  double y = PV.y()*cos(angle) - PV.z()*sin(angle);
  double z = PV.y()*sin(angle) + PV.z()*cos(angle);
  PV.SetXYZ( PV.x(), y, z );

  //Shift it !
  double xs = 0.1 *Gaudi::Units::mm;
  double ys = 0.1 *Gaudi::Units::mm;
  double zs = 10 *Gaudi::Units::cm;
  PV.SetXYZ( PV.x() + xs, PV.y() + ys, PV.z() + zs ) ;
  
}
//=============================================================================
// Save the beam line on the TES.
//=============================================================================
void CalibrateIP::SaveBeamLine(){
  
  Particle * p = new Particle();

  const Gaudi::LorentzVector mom = Gaudi::LorentzVector(
                m_Beam->GetZEigenVector().x(), 
                m_Beam->GetZEigenVector().y(), 
                m_Beam->GetZEigenVector().z(), 0. );
  p->setMomentum( mom );
  p->setReferencePoint( m_Beam->GetIPPosition() );  

  Particles* vec = new Particles();
  vec->insert( p ); 
  put( vec, m_outloc );

}
