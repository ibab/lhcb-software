#include "IVertexResidualTool.h"
#include "TrackInterfaces/ITrackSelector.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "LHCbMath/MatrixInversion.h"
#include "Event/Node.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

class ITrackExtrapolator ;
namespace LHCb {
  class RecVertex ; 
}

namespace 
{
  // helper structure to hold nformation needed to apply a mass constraint
  struct MassConstraint
  {
    std::vector<double> daughtermasses ;
    double mothermass ;
    double motherwidth ;
  } ;
}

namespace Al
{
  class ITrackResidualTool ;
  struct TrackContribution ;
  
  class VertexResidualTool : public GaudiTool,  
			     virtual public IVertexResidualTool,
			     virtual public IIncidentListener
  {
  public:
    //
    typedef std::vector<const Al::TrackResiduals*> TrackResidualContainer ;
    // constructor
    VertexResidualTool(const std::string& type,const std::string& name,const IInterface* parent);
    // destructor
    virtual ~VertexResidualTool() {} ;
    // initialize
    virtual StatusCode initialize();
    // finalize
    virtual StatusCode finalize(); 
    // incident service handle
    void handle( const Incident& incident ) ;
    // process a vertex (primary vertex of twoprongvertex)
    const Al::MultiTrackResiduals* get(const LHCb::RecVertex& vertex) const ;
    // process a particle. this applies a mass constraint.
    const Al::MultiTrackResiduals* get(const LHCb::Particle& particle) const ;
  private:
    // create a new MultiTrackResiduals
    const Al::MultiTrackResiduals* compute(const TrackResidualContainer& tracks,
					   const Gaudi::XYZPoint& vertexestimate,
					   const MassConstraint* massconstraint ) const ;
    // extrapolate the state in trackresiduals to position z
    StatusCode extrapolate( const Al::TrackResiduals& trackin,
			    double z, TrackContribution& trackout ) const ;
  private:
    ToolHandle<ITrackResidualTool> m_trackresidualtool ;
    ToolHandle<ITrackExtrapolator> m_extrapolator ;
    LHCb::IParticlePropertySvc* m_propertysvc ;
    double m_chiSquarePerDofCut ;
    double m_maxMassConstraintChi2 ;
    bool   m_computeCorrelations ;
    size_t m_maxHitsPerTrackForCorrelations ;
    typedef std::vector<const Al::MultiTrackResiduals*> ResidualContainer ;
    mutable ResidualContainer m_residuals ;
    bool m_debugLevel ;
  } ;
  
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
  
#include <iostream>
#include <algorithm>
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"

#include "Event/TwoProngVertex.h"
#include "Event/Particle.h"
#include "TrackKernel/TrackStateVertex.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "ITrackResidualTool.h"
#include <boost/assign/list_of.hpp> 
#include <boost/foreach.hpp> 
#include "GaudiKernel/ParticleProperty.h"
#include "Kernel/ParticleID.h"


namespace Al
{
  
  DECLARE_TOOL_FACTORY( VertexResidualTool )

  VertexResidualTool::VertexResidualTool(const std::string& type,
				       const std::string& name,
				       const IInterface* parent)
    : GaudiTool(type,name,parent),
      m_trackresidualtool("Al::TrackResidualTool"), // important: use the toolsvc, because of caching!
      m_extrapolator("TrackMasterExtrapolator"),
      m_propertysvc(0),
      m_maxHitsPerTrackForCorrelations(9999)
  {
    // interfaces
    declareInterface<IVertexResidualTool>(this);
    declareProperty("TrackResidualTool",m_trackresidualtool) ;
    declareProperty("Extrapolator",m_extrapolator) ;
    declareProperty("MaxHitsPerTrackForCorrelations", m_maxHitsPerTrackForCorrelations) ;
    declareProperty("MaxVertexChi2PerDoF",m_chiSquarePerDofCut = 10 ) ;
    declareProperty("MaxMassConstraintChi2", m_maxMassConstraintChi2 = 16 ) ;
  }
  
  StatusCode VertexResidualTool::initialize()
  {
    StatusCode sc = GaudiTool::initialize();
    if (sc.isFailure()){
      return Error("Failed to initialize",sc);
    }
    m_trackresidualtool.retrieve().ignore() ;
    m_extrapolator.retrieve().ignore() ;
    incSvc()->addListener(this, IncidentType::EndEvent);    
    m_propertysvc = svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc",true) ;
    m_debugLevel = msgLevel( MSG::DEBUG ) ;
    return sc ;
  }

  StatusCode VertexResidualTool::finalize()
  {
    return m_trackresidualtool.release() && m_extrapolator.release() && 
      /*m_trackselectorhandle.release() && */ GaudiTool::finalize() ;
  }
  
  void VertexResidualTool::handle( const Incident& incident )
  {
    if ( IncidentType::EndEvent == incident.type() ) {
      for( ResidualContainer::iterator it = m_residuals.begin() ;
	   it != m_residuals.end(); ++it)
	delete *it ;
      m_residuals.clear() ;
    }
  }
  
  const Al::MultiTrackResiduals* VertexResidualTool::get(const LHCb::RecVertex& vertex) const
  {
    // loop over the list of vertices, collect tracks in the vertex
    const Al::MultiTrackResiduals* rc(0) ;
    TrackResidualContainer trackresiduals ;
    for( SmartRefVector<LHCb::Track>::const_iterator itrack = vertex.tracks().begin() ;
	 itrack !=  vertex.tracks().end(); ++itrack) {
      const Al::TrackResiduals* trackres = m_trackresidualtool->get( **itrack ) ;
      if(trackres) {
	trackresiduals.push_back( trackres ) ;
      } else {
	warning() << "No residuals returned by trackresidualtool!" << endreq ;
	assert(0) ;
      }
    }

    const MassConstraint* mconstraint(0) ;
    rc = compute( trackresiduals, vertex.position(), mconstraint ) ;
    if(rc) m_residuals.push_back(rc) ;
    
    delete mconstraint ;
    return rc ;
  }

  const Al::MultiTrackResiduals* VertexResidualTool::get(const LHCb::Particle& p) const
  {
    // loop over the list of vertices, collect tracks in the vertex
    const Al::MultiTrackResiduals* rc(0) ;
    TrackResidualContainer trackresiduals ;
    MassConstraint mconstraint ;
    mconstraint.mothermass = m_propertysvc->find( p.particleID() )->mass() ;
    mconstraint.motherwidth = m_propertysvc->find( p.particleID() )->width() ;
    //    info() << "Mothermass: " << mconstraint.mothermass << "\t width: " << mconstraint.motherwidth << endreq ;

    bool success(true) ;
    BOOST_FOREACH( const LHCb::Particle* daughter, p.daughters()) {
      if( daughter->proto() && daughter->proto()->track()) {
	const LHCb::Track* track = daughter->proto()->track() ;
	const Al::TrackResiduals* trackres = m_trackresidualtool->get( *track ) ;
	if( trackres ) {
	  trackresiduals.push_back( trackres ) ;
	  const LHCb::ParticleProperty* pp = m_propertysvc->find( daughter->particleID() ) ;
	  if( !pp ) {
	    warning() << "Cannot find property for particle: " << daughter->particleID() << " "
		      << daughter->particleID().pid() << endreq ;
	    mconstraint.daughtermasses.push_back( daughter->momentum().M() ) ;
	  } else {
	    mconstraint.daughtermasses.push_back( pp->mass() ) ;
	  }
	  //info() << "daughtermass: " << mconstraint.daughtermasses.back() << endreq ;
	} else {
	  warning() << "No residuals returned by trackresidualtool!" << endreq ;
	  success = false ;
	}
      } else {
	warning() << "Composite contains daughter that is not a track. Need to adapt code for that." << endreq ;
	success = false ;
	assert(0) ;
      }
    }
    
    if(success) {
      rc = compute( trackresiduals, p.referencePoint(), &mconstraint ) ;
      if(rc) m_residuals.push_back(rc) ;
    }
    return rc ;
  }

  struct TrackContribution
  { 
    typedef std::vector< Gaudi::TrackProjectionMatrix > ProjectionMatrix ;
    const Al::TrackResiduals* trackresiduals ;
    LHCb::State               inputstate ;  // state at vertex before vertex fit
    Gaudi::TrackSymMatrix     inputCovInv ; // inverse of cov matrix of that state
    ProjectionMatrix residualStateCov ; // same as in the residuals, but now extrapolated to vertex state
    ProjectionMatrix dResidualdState ;  // residualStateCov * C(vertex-state)^{-1}, just cached for speed
    size_t offset ;
  } ;

  const Al::MultiTrackResiduals* VertexResidualTool::compute(const TrackResidualContainer& tracks,
							     const Gaudi::XYZPoint& vertexestimate,
							     const MassConstraint* mconstraint) const
  {
    Al::MultiTrackResiduals* rc(0) ;
    bool success = true ;

    // first we need to propagate all tracks to the vertex and keep
    // track of the correlation between the state at the vertex and
    // the reference state inside the TrackResiduals object
    
    typedef std::vector<TrackContribution> Contributions ;
    Contributions states ;
    double totalchisq(0) ;
    size_t totalndof(0) ;
    size_t nacceptedtracks(0) ;
    size_t numresiduals(0) ;
    size_t numexternalhits(0) ;

    for( TrackResidualContainer::const_iterator itrack = tracks.begin() ;
	 itrack != tracks.end() && success; ++itrack) {
      TrackContribution tc ;
      tc.offset = numresiduals ;
      tc.trackresiduals = *itrack ;
      StatusCode sc = extrapolate( **itrack, vertexestimate.z(), tc ) ;
      if( sc.isSuccess() ) { 
	states.push_back( tc ) ;
	numresiduals += (*itrack)->size() ;
	numexternalhits += (*itrack)->nExternalHits() ;
	totalchisq += (*itrack)->chi2() ;
	totalndof  += (*itrack)->nDoF() ;
	++nacceptedtracks ;
      }
      else {
	warning() << "Extrapolation failed. Rejecting vertex." << endreq ;
	success = false ;
      }
    }
    

    if(success) {
      // now vertex the states
      LHCb::TrackStateVertex vertex ;
      for( Contributions::const_iterator i = states.begin() ; i!= states.end(); ++i)
	vertex.addTrack( i->inputstate ) ;

      LHCb::TrackStateVertex::FitStatus fitstatus = vertex.fit() ;
      double vchi2orig = vertex.chi2() ; // cache it, because I know it is slow
      int ndof = vertex.nDoF() ;
            
      if(fitstatus == LHCb::TrackStateVertex::FitSuccess && mconstraint ) {
	//	static std::vector<double> masshypos = boost::assign::list_of(m_muonmass)(m_muonmass) ;
	if( m_debugLevel ) 
	  debug() << "mass before constraint: "
		  << vertex.mass(mconstraint->daughtermasses) 
		  << " +/- " << vertex.massErr(mconstraint->daughtermasses) << endreq ;
	double qopbefore = std::sqrt(vertex.stateCovariance(0)(4,4)) ;
	fitstatus = vertex.constrainMass( mconstraint->daughtermasses, mconstraint->mothermass, mconstraint->motherwidth) ;
	// vertex fit updates chi2, but not ndof
	ndof += 1 ;
	if( m_debugLevel ) 
	  debug() << "mass afterconstraint: "
		  << vertex.mass(mconstraint->daughtermasses)
		  << " +/- " << vertex.massErr(mconstraint->daughtermasses) << std::endl
		  << "error on qop of first track, original, after vertex fit, after mass fit: "
		  << std::sqrt(vertex.inputState(0).covariance()(4,4)) << " "
		  << qopbefore << " "
		  << std::sqrt(vertex.stateCovariance(0)(4,4)) << endreq ;
      }
      // this is the chisquare including an eventual mass constraint
      double vchi2 = vertex.chi2() ; // cache it, because I know it is slow

      counter("vchi2") += vchi2 ;
      counter("vchi2orig") += vchi2orig ;
      //counter("niter") += vertex.nIter() ;
       
      if( m_debugLevel ) {
	debug() << "Fitted vertex, chi2/dof=" << vchi2 << "/" << ndof << endreq ;
	debug() << "Vertex position orig/new="
		<< vertexestimate << "/" << vertex.position() << endreq ;
      }

      // note that when we cut on the vertex chisqur2 we exclude the mass constraint
      if(fitstatus == LHCb::TrackStateVertex::FitSuccess && vchi2orig / vertex.nDoF() < m_chiSquarePerDofCut &&
	 (!mconstraint || (vchi2-vchi2orig) < m_maxMassConstraintChi2 ) ) {
	// create a vertexresiduals object
	totalchisq += vchi2 ;
	totalndof  += ndof ;
	std::vector<const Al::TrackResiduals*> tracks ;
	BOOST_FOREACH( const TrackContribution& atrack, states) 
	  tracks.push_back( atrack.trackresiduals ) ;
	rc = new Al::MultiTrackResiduals( totalchisq, totalndof, numexternalhits, tracks, vchi2, ndof ) ;
	rc->m_residuals.reserve(numresiduals) ;
	if( m_debugLevel )
	  debug() << "created the vertex: " << numresiduals << endreq ;
	
	// calculate all new residuals and all correlations
	bool computationerror(false) ;
	for(size_t i = 0; i<states.size() && !computationerror; ++i) {
	  Gaudi::TrackVector    deltaState = vertex.state(i).stateVector() - vertex.inputState(i).stateVector() ;
	  Gaudi::TrackSymMatrix deltaCov   = vertex.state(i).covariance()  - vertex.inputState(i).covariance() ;

	  // These are only needed to compute correlations with the reference state/vertex that we later use for the constraints.
	  ROOT::Math::SMatrix<double,5,5> dStateOutDStateIn = states[i].inputCovInv * vertex.state(i).covariance() ;
	  ROOT::Math::SMatrix<double,5,3> stateVertexCov = vertex.matrixA(i) * vertex.covMatrix() ;
	  stateVertexCov += vertex.matrixB(i) * vertex.momPosCovMatrix(i) ;
	  ROOT::Math::SMatrix<double,5,3> dVertexDStateIn = states[i].inputCovInv * stateVertexCov ;

	  size_t ioffset = states[i].offset ;
	  for(size_t irow = 0; irow<states[i].trackresiduals->size(); ++irow) {
	    // copy the original residual
	    assert( rc->m_residuals.size() == irow+ioffset ) ;
	    rc->m_residuals.push_back( states[i].trackresiduals->residuals()[irow] ) ;
	    Al::Residual1D& res = rc->m_residuals.back() ;
	    
	    // update with vertex info
	    double deltar = (states[i].dResidualdState[irow] * deltaState)(0) ;
	    double deltaHCH = ROOT::Math::Similarity(states[i].dResidualdState[irow], deltaCov)(0,0) ;
	    res.setR(   res.r() + deltar ) ;
	    res.setHCH( res.HCH() + deltaHCH ) ;
	    if( res.HCH() < 0 ) {
	      warning() << "problem computing update of track errors"
			<< states[i].trackresiduals->residuals()[irow].HCH()
			<< deltaHCH << std::endl 
			<< deltaCov << endreq ;
	      computationerror = true ;
	    }
	    // update also the covariance with reference state and
	    // vertex. the math behind this is actually not entirely
	    // trivial.
	    res.setResidualStateCov( states[i].residualStateCov[irow] * dStateOutDStateIn ) ;
	    res.setResidualVertexCov( states[i].residualStateCov[irow] * dVertexDStateIn ) ;
	    /*
	    for( size_t k=0; k<5; ++k) {
	      double s = std::sqrt( res.V() * vertex.state(i).covariance()(k,k) ) ;
	      if( std::abs( res.residualStateCov()(0,k) / s) > 1) {
	      std::cout << "problem computing residual track covariance: "
			  << k << " "
			  << res.node().z() << " "
			  << res.residualStateCov()(0,k) << " " << s << " "
			  << "track cov: " << vertex.state(i).covariance()(k,k) << std::endl ;
		//assert(0) ;
	      }
	    }

	    bool anerror(false) ;
	    for( size_t k=0; k<3; ++k)
	      if( std::abs( res.residualVertexCov()(0,k) / std::sqrt( res.V() * vertex.covMatrix()(k,k) ) ) > 1 ) {
		std::cout << "problem computing residual vertex covariance: "
			  << res.node().z() << " "
			  << k << " "
			  << res.residualVertexCov()(0,k) << " "
			  << std::sqrt( res.V() * vertex.covMatrix()(k,k) ) << std::endl ;
		std::cout << "contributions: " ;
		for( size_t l=0; l<5; ++l) 
		  std::cout << states[i].residualStateCov[irow](0,l) * dVertexDStateIn(l,k) << " " ;
		std::cout << std::endl ;

		//assert(0) ;
		anerror = true ;
	      }
	    // let;s chack the track-vertex correlation matrix actually make sense
	    if(anerror) {
	      std::cout << "delta-z: "
			<< vertex.position().z() - vertexestimate.z() << std::endl ;
	      ROOT::Math::SMatrix<double,5,3> statevertexcov = vertex.matrixA(i) * vertex.covMatrix() ;
	      for( size_t k=0; k<5; ++k)
		for (size_t l=0; l<3; ++l) 
		  statevertexcov(k,l) /= std::sqrt( vertex.covMatrix()(l,l) * vertex.state(i).covariance()(k,k) ) ;
	      std::cout << "statevertexcor: " << statevertexcov << std::endl ;
	    }
	    */
	  }
	  
	  // loop over all existing entries of the per-track HCH and update
	  for( Al::Residuals::CovarianceContainer::const_iterator 
		 ielem = states[i].trackresiduals->m_HCHElements.begin() ;
	       ielem != states[i].trackresiduals->m_HCHElements.end(); ++ielem) {
	    double deltaHCH = 
	      (states[i].dResidualdState[ielem->row] * deltaCov *
	       ROOT::Math::Transpose(states[i].dResidualdState[ielem->col]))(0,0) ;
	    rc->m_HCHElements.push_back
	      ( Al::CovarianceElement( ielem->row + ioffset, 
				       ielem->col + ioffset,
				       ielem->val + deltaHCH ) ) ;
	  }
				       
	  // now the correlations between the tracks. this is the very time-consuming part.
	  if( m_maxHitsPerTrackForCorrelations>0 && !computationerror)
	    for(size_t j =0; j<i && !computationerror; ++j) {
	      size_t joffset = states[j].offset ;
	      size_t maxirow = std::min( states[i].trackresiduals->size(),m_maxHitsPerTrackForCorrelations) ;
	      for(size_t irow = 0; irow<maxirow; ++irow) {
		// store this intermediate matrix too save some time
		Gaudi::TrackProjectionMatrix A = states[i].dResidualdState[irow] * vertex.stateCovariance(i,j) ;
		size_t maxjcol = std::min( states[j].trackresiduals->size(), m_maxHitsPerTrackForCorrelations) ;
		for(size_t jcol = 0; jcol<maxjcol; ++jcol) {
		  double deltaHCH = (A * ROOT::Math::Transpose( states[j].dResidualdState[jcol] ))(0,0) ;
		  rc->m_HCHElements.push_back
		    ( Al::CovarianceElement( irow + ioffset, jcol + joffset, deltaHCH ) ) ;
		}
	      }
	    }
	  
	  
	  // Let's check the result Keep track of the roots.
	  if(!computationerror) {
	    std::string message ;
	    computationerror = rc->testHCH(message) ;
	    if(computationerror) warning() << message << endreq ;
	  }
	}
	
	if(computationerror) {
	  delete rc ;
	  rc = 0 ;
	  warning() << "VertexResidualTool::compute failed" << endreq ;
	} 
      } else {
	warning() << "rejected vertex with chisqu/dof: " << vchi2 / vertex.nDoF() 
		  << " isConstrained = " << bool(mconstraint!=0) << endreq ;
      } 
    } else {
	warning() << "not enough tracks for vertex anymore" << endreq ;
    }
    return rc ;
  }
  
  StatusCode VertexResidualTool::extrapolate( const Al::TrackResiduals& track,
					      double z,
					      TrackContribution& tc ) const 
  {
    // Just to remind you, this is the covariance matrix for a state and its extrapolated state
    //
    //             C1          C1 F^T
    //           F C1       F C1 F^T + Q
    //
    //  where F is the transport matrix and Q is the noise. So, the
    //  covariance in the extrapolated state is the well-known
    //
    //     C2 = F C1 F^T + Q
    //
    // and the correlation is the lesser known
    //
    //     C21 = F C1
    //

    // Now, we have the correlation matrix for C1 and the residuals
    // R. Let's call that 'E' (which is a 5xN matrix). To get the
    // correlation between C' and R, we use the formula
    //
    //  E'  =  C21 * C1^{-1} * E = F E
    //
    // So, we just need to multiply E with the transport matrix,
    // which is kind of obvious.
    //
    // In the end we are not so much interested in E, but in the
    // derivatives of the residuals to the state, which is
    //
    // dRdS = C2^{-1} * E' = C2^{-1} * F * E
    
    // propagate the reference state and get the transport matrix
    Gaudi::TrackMatrix F ;
    tc.inputstate = track.state() ;
    StatusCode sc = m_extrapolator->propagate(tc.inputstate,z,&F) ;
    if( sc.isSuccess() ) {

      // calculate the correlation
      // stateResidualCorrelation = convertToCLHEP(F) * track.m_stateResCov ;
      
      // invert the covariance matrix. cache it for later use
      tc.inputCovInv = tc.inputstate.covariance() ;
      bool OK = tc.inputCovInv.InvertChol() ;
      if(!OK) {
	warning() << "Inversion error in VertexResidualTool::extrapolate" << endreq ;
	sc = StatusCode::FAILURE ;
      } else {
	tc.residualStateCov.resize(track.size()) ;
	tc.dResidualdState.resize(track.size()) ;
	for(size_t inode=0; inode<track.size(); ++inode) {
	  tc.residualStateCov[inode] = track.residuals()[inode].residualStateCov() *  ROOT::Math::Transpose(F) ;
	  tc.dResidualdState[inode]  = tc.residualStateCov[inode] * tc.inputCovInv ;
	}
      }
    } 
    return sc ;
  }
}
