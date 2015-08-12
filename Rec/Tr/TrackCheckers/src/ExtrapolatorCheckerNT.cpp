// $Id: ExtrapolatorChecker.cpp,v 1.5 2008-02-08 08:13:01 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiAlg/GaudiTupleAlg.h"

#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/IMaterialLocator.h"
#include "TrackKernel/CubicStateInterpolationTraj.h"
#include "Kernel/LineTraj.h"

#include "DetDesc/Material.h"

// from Event/LinkerEvent
#include "Linker/LinkedTo.h"
#include "Linker/LinkedFrom.h"

// from Event/TrackEvent
#include "Event/Track.h"

// from Event/MCEvent
#include "Event/MCParticle.h"
#include "Event/MCHit.h"

// boost
#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>

//using namespace Gaudi;
//using namespace Gaudi::Units;
//using namespace LHCb;

class ExtrapolatorCheckerNT : public GaudiTupleAlg 
{
                                                                             
 public:
                                                                             
  /** Standard construtor */
  ExtrapolatorCheckerNT( const std::string& name, ISvcLocator* pSvcLocator );
                                                                             
  /** Destructor */
  virtual ~ExtrapolatorCheckerNT();
  
  /** Algorithm execute */
  virtual StatusCode execute();
  
  /** Algorithm initialize */
  virtual StatusCode initialize();

 /** Algorithm initialize */
  virtual StatusCode finalize();

private:
  LHCb::StateVector stateVector(const LHCb::MCHit& aHit) const ;
  void fill(Tuple& tuple,
	    const std::string& prefix, const LHCb::StateVector& s1,
	    const LHCb::StateVector& s2) ;
  void fill(Tuple& tuple,
	    const LHCb::StateVector& vertexstate,
	    const LHCb::MCHit& firsthit ) ;
private:
  std::vector<std::string> m_dets ;
  ToolHandle<ITrackExtrapolator> m_extrapolator ;
  ToolHandle<IMaterialLocator> m_materiallocator ;
} ;

DECLARE_ALGORITHM_FACTORY( ExtrapolatorCheckerNT )



//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ExtrapolatorCheckerNT::ExtrapolatorCheckerNT( const std::string& name,
                            ISvcLocator* pSvcLocator ) :
  GaudiTupleAlg( name , pSvcLocator ),
  m_extrapolator("TrackMasterExtrapolator"),
  m_materiallocator("DetailedMaterialLocator")
{

  std::vector<std::string> tmp = boost::assign::list_of("IT")("OT")("TT")("Velo");
  declareProperty("Detectors" , m_dets = tmp);
}

//=============================================================================
// Destructor
//=============================================================================
ExtrapolatorCheckerNT::~ExtrapolatorCheckerNT() {} 

StatusCode ExtrapolatorCheckerNT::initialize()
{
  StatusCode sc = GaudiTupleAlg::initialize() ;
  if(sc.isSuccess()) {
    sc = m_extrapolator.retrieve() ;
  }
  if(sc.isSuccess()) {
    sc = m_materiallocator.retrieve() ;
  }
  return sc ;
}

StatusCode ExtrapolatorCheckerNT::finalize()
{
  m_extrapolator.release().ignore() ;
  m_materiallocator.release().ignore() ;
  return GaudiTupleAlg::finalize() ;
}

//=============================================================================
// Main execution
//=============================================================================

namespace {
  class SortMCHits
  {
  public:
    bool operator()(const LHCb::MCHit* lhs, const LHCb::MCHit* rhs) const {
      return lhs->entry().z() < rhs->entry().z() ;
    }
  } ;
}

StatusCode ExtrapolatorCheckerNT::execute()
{  
  // Get the MCParticles
  LHCb::MCParticles* particles = get<LHCb::MCParticles>( LHCb::MCParticleLocation::Default );
  
  // create a map from all MCParticles to MCHits
  typedef std::map<const LHCb::MCParticle *, std::vector<const LHCb::MCHit*> > MCHitMap ;
  MCHitMap mchitmap ;
  std::vector<std::string>::const_iterator itDets;
  for( itDets = m_dets.begin(); itDets < m_dets.end(); ++itDets ) {
    std::string location = std::string("/Event/MC/")+ *itDets + "/Hits" ;
    const LHCb::MCHits* hits = get<LHCb::MCHits>( location ) ;
    BOOST_FOREACH( const LHCb::MCHit* mchit, *hits ) {
      if( mchit->mcParticle() )
	mchitmap[ mchit->mcParticle() ].push_back( mchit ) ;
    }
  }
  
  Tuple theTuple = nTuple( "scattertuple" , "" , CLID_ColumnWiseTuple );
  
  // Loop over the MCParticles
  size_t nsel(0) ;
  for( MCHitMap::const_iterator ip = mchitmap.begin() ;
       ip != mchitmap.end(); ++ip) {
    const LHCb::MCParticle* particle = ip->first ;
    // Decide whether the MCParticle will be checked
    const std::vector<const LHCb::MCHit*>& mchits = ip->second ;

    if ( particle->originVertex() && 
	 std::abs(particle->originVertex()->position().z()) < 100 &&
	 std::abs(particle->originVertex()->position().x()) < 0.1 &&
	 std::abs(particle->originVertex()->position().y()) < 0.1 &&
	 mchits.size()>4 /*true || selected( mcPart ) */ ) {
      //std::sort( mchits.begin(), mchits.end(), SortMCHits()) ;
      ++nsel ;

      // find the first and last MCHit after the magnet
      // I want 4 mc hits:
      // - firstvelo
      // - last velo
      // - last TT
      // - first T

      double zmagnet = 5000 ;
      const LHCb::MCHit *mchitfirstvelo(0), *mchitlastvelo(0),
	*mchitlastTT(0), *mchitfirstT(0), *mchitlastT(0) ;
      BOOST_FOREACH( const LHCb::MCHit* mchit, mchits ) {
	if( mchit->entry().z() < 1000 ) {
	  if( mchitfirstvelo==0 || mchitfirstvelo->entry().z() > mchit->entry().z() )
	    mchitfirstvelo = mchit ;
	  if( mchitlastvelo==0 || mchitlastvelo->entry().z() < mchit->entry().z() )
	    mchitlastvelo = mchit ;
	} else if( mchit->entry().z() < zmagnet ) {
	  if( mchitlastTT==0 || mchitlastTT->entry().z() < mchit->entry().z() )
	    mchitlastTT = mchit ;
	} else if( mchit->entry().z() < 9500 ) {
	  if( mchitfirstT==0 || mchitfirstT->entry().z() > mchit->entry().z() )
	    mchitfirstT = mchit ;
	  if( mchitlastT==0 || mchitlastT->entry().z() < mchit->entry().z() )
	    mchitlastT = mchit ;
	}
      }
	
      if( mchitfirstvelo && mchitlastvelo && 
	  mchitfirstvelo != mchitlastvelo &&
	  mchitlastTT && mchitfirstT && mchitlastT &&
	  mchitlastT != mchitfirstT ) {

	theTuple->column("nhits", mchits.size()) ;
	const Gaudi::XYZPoint origin = particle->originVertex()->position() ; 
	const Gaudi::LorentzVector p4 = particle->momentum() ;
	theTuple->column("x",origin.x()) ;  
	theTuple->column("y",origin.y()) ;
	theTuple->column("z",origin.z()) ;
	theTuple->column("p",p4.R()) ;
	theTuple->column("px",p4.Px()) ;
	theTuple->column("py",p4.Py()) ;
	theTuple->column("pz",p4.Pz()) ;

	// Create a statevector for the MC particle at the vertex
	double charge = ( particle->particleID().threeCharge() ) / 3.;
	double qOverP = charge / particle->p() ;
	LHCb::StateVector stateAtOrigin( origin, p4.Vect(), qOverP ) ;
	theTuple->column("qOverP",qOverP) ;
	theTuple->column("charge",charge) ;
	
	// 
	LHCb::StateVector statefirstvelo = stateVector( *mchitfirstvelo ) ;
	LHCb::StateVector statelastvelo = stateVector( *mchitlastvelo ) ;
	LHCb::StateVector statelastTT   = stateVector( *mchitlastTT ) ;
	LHCb::StateVector statefirstT   = stateVector( *mchitfirstT ) ;
	LHCb::StateVector statelastT   = stateVector( *mchitlastT ) ;

	fill( theTuple, "a", stateAtOrigin, statefirstvelo ) ;
	fill( theTuple, "aa", statefirstvelo, stateAtOrigin ) ;
	fill( theTuple, "b", statefirstvelo,statelastvelo ) ;
	fill( theTuple, "c", statelastvelo,statelastTT ) ;
	fill( theTuple, "d", statelastTT,statefirstT ) ;
	fill( theTuple, "dd", statefirstT,statelastTT ) ;
	fill( theTuple, "e", statefirstT,statelastT ) ;

	theTuple->column("fv_x",mchitfirstvelo->entry().x()) ;
	theTuple->column("fv_y",mchitfirstvelo->entry().y()) ;
	theTuple->column("fv_z",mchitfirstvelo->entry().z()) ;
	theTuple->column("fv_dz",mchitfirstvelo->displacement().z()) ;
	
	fill(theTuple, stateAtOrigin, *mchitfirstvelo) ;

	theTuple->write() ;
      }
    }
  } // End loop over MCParticles


  info() << "Number of MCParticles: " << particles->size() << " " << nsel <<  endreq ;

  return StatusCode::SUCCESS;
}

namespace std {
  template<class T >
  T sqr(const T& x ) { return x*x ; }
}
	
void ExtrapolatorCheckerNT::fill(Tuple& tuple,
				 const std::string& prefix, const LHCb::StateVector& s1,
				 const LHCb::StateVector& s2)
{
  // create a state
  LHCb::State state(s1) ;
  // propagate it
  m_extrapolator->propagate( state, s2.z() );

  tuple->column(prefix + "_z1", s1.z()) ;
  tuple->column(prefix + "_z2", s2.z()) ;
  tuple->column(prefix + "_dx", s2.x() - state.x()) ;
  tuple->column(prefix + "_dy", s2.y() - state.y()) ;
  tuple->column(prefix + "_dtx", s2.tx() - state.tx()) ;
  tuple->column(prefix + "_dty", s2.ty() - state.ty()) ;
  tuple->column(prefix + "_dqop",  s2.qOverP() - state.qOverP()) ;

  tuple->column(prefix + "_sdx", std::sqrt(state.covariance()(0,0)) ) ;
  tuple->column(prefix + "_sdy", std::sqrt(state.covariance()(1,1)) ) ;
  tuple->column(prefix + "_sdtx", std::sqrt(state.covariance()(2,2)) ) ;
  tuple->column(prefix + "_sdty", std::sqrt(state.covariance()(3,3)) ) ;
  tuple->column(prefix + "_sdqop", std::sqrt(state.covariance()(4,4)) ) ;

  double E1 = 1/std::abs(s1.qOverP()) ;
  double E2 = 1/std::abs(s2.qOverP()) ;
  double E2prime = 1/std::abs(state.qOverP()) ;
  tuple->column(prefix + "_eloss", E2 - E1 ) ;
  tuple->column(prefix + "_estimatedeloss", E2prime - E1 ) ;

  if( std::sqrt( std::sqr(s1.x()) + std::sqr(s1.y())) < 5 ) {
    // I'd like to know a bit more about material between vertex and first hit
    LHCb::CubicStateInterpolationTraj traj( s1, state ) ;
    IMaterialLocator::Intersections intersections ;
    m_materiallocator->intersect( traj, intersections)  ;
    tuple->column(prefix + "_nintersect", intersections.size() ) ;
    int numAlu(0) ;
    double lenAlu(0) ;
    double elossAlu(0) ;
    if( intersections.size()>0 ) {
      IMaterialLocator::Intersections intersectionsAlu ; 
      for( IMaterialLocator::Intersections:: const_iterator 
	     it = intersections.begin(); it != intersections.end() ; ++it ) 
	if( std::abs(it->material->Z() - 13) < 0.1 ) { // Al
	  lenAlu += std::abs( (it->z1 - it->z2) * std::sqrt( 1 + std::sqr(it->tx) + std::sqr(it->ty)) ) ;
	  ++numAlu ;
	  intersectionsAlu.push_back( *it) ;
	  //std::cout << numAlu << " " << it->material->Z() << " " << *(it->material) << std::endl ;
	} else {
	  //std::cout << it->material->name() << " " << it->z1 << " " << it->z2 << std::endl ;
	}
      
      LHCb::State tmpstate(s2) ;
      m_materiallocator->applyMaterialCorrections(tmpstate,
						  intersectionsAlu,
						  s1.z(),
						  LHCb::ParticleID(211),
						  true,true) ;
      double E = 1/std::abs(tmpstate.qOverP()) ;
      elossAlu = E - E2 ;
    }
    tuple->column(prefix + "_numAlu", numAlu) ;
    tuple->column(prefix + "_lenAlu", lenAlu) ;
    tuple->column(prefix + "_estimatedelossAlu", elossAlu ) ;
  }
}


void ExtrapolatorCheckerNT::fill(Tuple& tuple,
				 const LHCb::StateVector& vertexstate,
				 const LHCb::MCHit& firsthit )
{
  
  //std::cout << "First velo hit: " 
  //<< firsthit.entry().z() << " "
  //<< firsthit.exit().z() << " "
  //<< firsthit.entry() << std::endl ;
  
  LHCb::StateVector firststate = stateVector(firsthit) ;
  // create a stright line from vertexstate to firststate and collect
  // material. but I'd like to go 1mm beyond the mchit in order to
  // really see the sensor.
  Gaudi::XYZPoint endpoint = 
    firsthit.exit() + 1 * firsthit.displacement().Unit() ;
  Gaudi::XYZPoint beginpoint = vertexstate.position() ;
  Gaudi::XYZVector vector = Gaudi::XYZVector( endpoint - beginpoint ) ;
  //LHCb::LineTraj linetraj(vertextfirststate.position(), endpoint) ;
  IMaterialLocator::Intersections intersections ;
  m_materiallocator->intersect(beginpoint,vector,
			       intersections)  ;
  double lastz(-9999) ;
  Gaudi::XYZPoint intersectionFirstPoint ;

  for( IMaterialLocator::Intersections:: const_iterator 
	 it = intersections.begin(); it != intersections.end() ; ++it ) 
    if( std::abs(it->material->Z() - 13) < 0.1 ) { // Al
      //std::cout << numAlu << " " << it->material->Z() << " " << *(it->material) << std::endl ;
    } else {
      //std::cout << it->material->name() << " " << it->z1 << " " << it->z2 << std::endl ;
      if( it->z2 > lastz ) {
	lastz = it->z2 ;
	intersectionFirstPoint = beginpoint + vector * ( it->z1 - beginpoint.z())/vector.z() ;
      }
    }
  double dz = lastz>-1000 ? firsthit.exit().z() - lastz : -999 ;
  tuple->column("firsthitz",firsthit.entry().z()) ;
  tuple->column("firsthitx",firsthit.entry().x()) ;
  tuple->column("firsthity",firsthit.entry().y()) ;
  tuple->column("firstintz",intersectionFirstPoint.z()) ;
  tuple->column("firstintx",intersectionFirstPoint.x()) ;
  tuple->column("firstinty",intersectionFirstPoint.y()) ;
  tuple->column( "moddeltaz", dz) ;
}


LHCb::StateVector ExtrapolatorCheckerNT::stateVector(const LHCb::MCHit& mchit) const
{
  // Correct tx and ty from the MCHit for the magnetic field
  double tx = mchit.dxdz();
  double ty = mchit.dydz();
  //if ( m_correctSlopes ) correctSlopes( mchit, tx, ty );
  Gaudi::XYZVector direction(tx,ty,1.0);
  
  // determine Q/P
  const double charge = ( mchit.mcParticle()->particleID().threeCharge() ) / 3.;
  const double qOverP = charge/mchit.p() ;
   
  // construct true State. maybe we need the midpoint here to make the slope fit better
  // nw, better not: momentum is measured at entry point.
  return LHCb::StateVector(mchit.entry(),direction,qOverP);
}

      /*



      info() << "Before ISC:" << endreq ;
      // Get the state at the vertex
      State stateVtx;
      idealStateCreator() -> createStateVertex( mcPart, stateVtx );

      info() << "After ISC:" << stateVtx << endreq ;

      // Find first MCHit
      std::string detName;
      MCHit* mcHit;
      int detID = findNextHit( mcPart, stateVtx.z(), mcHit, detName );
      
      if( !mcHit ) {
	info() << "Cannot find MCHits." << endreq ;
	return StatusCode::FAILURE ;
      }

      // Get the entry point of the MCHit
      XYZPoint entryP = mcHit -> entry();

      // Extrapolate through RF foil
      extrapolator() -> propagate( stateVtx, entryP.z() );
      TrackVector vec    = stateVtx.stateVector();
      TrackSymMatrix cov = stateVtx.covariance();

      // Correct tx and ty from the MCHit for the magnetic field
      double tx,ty;
      correctSlopes( mcPart, mcHit, tx, ty );
      
      // Determine the resolutions
      double dx  = vec(0) - entryP.x();
      double dy  = vec(1) - entryP.y();
      double dtx = vec(2) - tx;
      double dty = vec(3) - ty;
      double dqp = vec(4) - qOverP(mcPart, mcHit);

      // fill the histograms
      plot1D( dx, 1, "X resolution at 1st meas", -0.5, 0.5, 100 );
      plot1D( dy, 2, "Y resolution at 1st meas", -0.5, 0.5, 100 );
      plot1D( dtx, 3, "Tx resolution at 1st meas", -0.01, 0.01, 100 );
      plot1D( dty, 4, "Ty resolution at 1st meas", -0.01, 0.01, 100 );
      plot1D( stateVtx.p() - mcHit->p(), 5,"dp at 1st meas", 
              -0.3, 0.3, 100 );
      if ( cov(0,0) != 0 && cov(1,1) != 0 && cov(2,2) != 0 && cov(3,3) != 0){
        plot1D( dx / sqrt(cov(0,0)), 11,"X pull at 1st meas", -5., 5., 100 );
        plot1D( dy / sqrt(cov(1,1)), 12,"Y pull at 1st meas", -5., 5., 100 );
        plot1D( dtx / sqrt(cov(2,2)), 13,"Tx pull at 1st meas", -5., 5., 100);
        plot1D( dty / sqrt(cov(3,3)), 14,"Ty pull at 1st meas", -5., 5., 100);
      }
      if ( cov(4,4) != 0 )
        plot1D( dqp / sqrt(cov(4,4)), 15,"q/p pull at 1st meas", -5., 5., 100);

     
      const Gaudi::TrackSymMatrix zeroCov;
      State state;
      state.setState( entryP.x(),  entryP.y(), entryP.z(),
                      tx, ty, qOverP(mcPart, mcHit));
      state.setCovariance( zeroCov );

      detID = findNextHit( mcPart, state.z(), mcHit, detName );

      while( 0 != mcHit ) {

        entryP = mcHit -> entry(); 
        double dz = entryP.z() - state.z();
	
        // Extrapolate to next MCHit
        extrapolator()->propagate( state, entryP.z() );
        TrackVector vec    = state.stateVector();
        TrackSymMatrix cov = state.covariance();

        // Correct tx and ty from the MCHit for the magnetic field
        correctSlopes( mcPart, mcHit, tx, ty );

        // Determine the resolutions
        double dx  = vec(0) - entryP.x();
        double dy  = vec(1) - entryP.y();
        double dtx = vec(2) - tx;
        double dty = vec(3) - ty;
        double dqp = vec(4) - qOverP(mcPart, mcHit);

        // Define the ranges for the resolution plots
        double xr = 0.04;
        double tr = 0.001;
        double pr = 0.3;

        // Determine the histogram title
        int ID = 100 * detID; 
        std::string title = " at " + detName + " hits" ;
        if ( dz > 4.*m && (detName == "OT" || detName == "IT" )) {
          ID = 1100;
          title = " after magnet extrapolation";
          xr = 5.;
          tr = 0.005;
          pr = 5.;
        } else if ( dz > 1.*m && detName == "TT" ) {          
          ID = 1000;
          title = " after RICH1 extrapolation";
          xr = 5.;
          tr = 0.005;
          pr = 10.;
        }

        // fill the histograms
        plot1D( dx, ID+1, "X resolution"+title, -xr, xr, 100 );
        plot1D( dy, ID+2, "Y resolution"+title, -xr, xr, 100 );
        plot1D( dtx, ID+3, "Tx resolution"+title, -tr, tr, 100 );
        plot1D( dty, ID+4, "Ty resolution"+title, -tr, tr, 100 );
        plot1D( state.p() - mcHit->p(), ID+5,"dp"+title, -pr, pr, 100 );
        if ( cov(0,0) != 0 && cov(1,1) != 0 && cov(2,2) != 0 && cov(3,3) != 0){
          plot1D( dx/sqrt(cov(0,0)), ID+11, "X pull"+title, -5., 5.,100);
          plot1D( dy/sqrt(cov(1,1)), ID+12, "Y pull"+title, -5., 5.,100);
          plot1D( dtx/sqrt(cov(2,2)), ID+13, "Tx pull"+title, -5., 5.,100);
          plot1D( dty/sqrt(cov(3,3)), ID+14, "Ty pull"+title, -5., 5.,100);
        }
	      if ( cov(4,4) != 0 )
          plot1D( dqp / sqrt(cov(4,4)), ID+15,"q/p pull"+title, -5., 5., 100);

        state.setState( entryP.x(),  entryP.y(), entryP.z(),
                        tx, ty, qOverP(mcPart,mcHit));
        state.setCovariance( zeroCov );
        
        detID = findNextHit( mcPart, entryP.z(), mcHit, detName );
      }
      */

/*



//=============================================================================
// Find the next MCHit starting from a given z
// looping over the hits in all the tracking detectors
//=============================================================================
int ExtrapolatorCheckerNT::findNextHit( const MCParticle* mcPart,
                                      const double zRec,
                                      MCHit*& closestHit,
                                      std::string& detectorName) const
{
  detectorName = "Not found!";
  int detID = 0;
  MCHit* tmpClosestHit = 0;
  closestHit = tmpClosestHit;
  double closestZ = 100000.;

  std::vector<std::string>::const_iterator itDets;
  for( itDets = m_dets.begin(); itDets < m_dets.end(); ++itDets ) {
    std::string location = std::string("/Event/MC/")+ *itDets + "/Hits" ;
    findNextXxxHit( mcPart, zRec,
		    location,
		    //MCParticleLocation::Default + "2MC" + *itDets + "Hits",
                    tmpClosestHit );
    if ( tmpClosestHit != 0 &&
         tmpClosestHit->entry().z() > zRec && 
         tmpClosestHit->entry().z() < closestZ ) {
      closestHit = tmpClosestHit;
      closestZ = tmpClosestHit->entry().z();
      detectorName = *itDets;
      detID = 1 + itDets - m_dets.begin();
    }
  }
  return detID;
}

//=============================================================================
// Find the next MCHit of type Xxx starting from a given z
//=============================================================================
void ExtrapolatorCheckerNT::findNextXxxHit( const MCParticle* mcPart,
                                          const double zRec,
                                          std::string linkPath,
                                          MCHit*& closestHit ) const
{
  // Retrieve MCParticle to MCHit linker tables
  //LinkedFrom<MCHit,MCParticle> mcp2mchitLink( evtSvc(), msgSvc(), linkPath );
  LinkedFrom<MCHit> mcp2mchitLink( evtSvc(), msgSvc(), linkPath );
  
  double closestZ = 100000.;

  MCHit* mcHit = mcp2mchitLink.first( mcPart );
  while( 0 != mcHit ) {
    double zOfHit = mcHit -> entry().z();
    // get the closest hit
    if ( zOfHit > zRec + 0.1 && zOfHit < closestZ ) {
      closestHit = mcHit;
      closestZ   = zOfHit ;
    }
    mcHit = mcp2mchitLink.next();
  }
}

//=============================================================================
// Determine q/p given an MCHit and a MCParticle
//=============================================================================
double ExtrapolatorCheckerNT::qOverP( const MCParticle* mcPart,
                                    const MCHit* mcHit ) const
{
  double charge = ( mcPart -> particleID().threeCharge() ) / 3.;
  double momentum = mcPart -> p();
  if ( mcHit != NULL && mcHit -> p() != 0. ) momentum = mcHit -> p();
  return ( momentum > TrackParameters::lowTolerance ) ? charge / momentum : 0.;
}

//=============================================================================
// Correct slopes for magnetic field given an MCHit and a MCParticle
//=============================================================================
void ExtrapolatorCheckerNT::correctSlopes( const MCParticle* mcPart,
                                         const MCHit* mcHit,
                                         double& tx, double& ty ) const
{
  // TODO: I hope this method can be removed as soon as the displacement vector
  // in the MCHit is calculated in Gauss using the momentum direction of the
  // *entry point*. (JvT: 27/10/2006).

  // Get magnetic field vector
  Gaudi::XYZVector B;
  fieldSvc()->fieldVector( mcHit -> midPoint() , B );

  // Calculate new displacement vector and tx,ty slopes
  Gaudi::XYZVector d = mcHit -> displacement();
  Gaudi::XYZVector dNew = d - ( 0.5 * d.R() * qOverP(mcPart, mcHit) * 
                                d.Cross(B) * eplus * c_light);
  tx = dNew.x() / dNew.z();
  ty = dNew.y() / dNew.z();  
}


*/
