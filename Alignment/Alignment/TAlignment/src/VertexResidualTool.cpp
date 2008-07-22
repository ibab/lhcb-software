#include "IVertexResidualTool.h"
#include "TrackInterfaces/ITrackSelector.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/ToolHandle.h"
#include "LHCbMath/MatrixInversion.h"

class ITrackExtrapolator ;
namespace LHCb {
  class RecVertex ; 
}

namespace Al
{
  class ITrackResidualTool ;
  
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
    // used tracks are removed from the list
    const Al::MultiTrackResiduals* get(const LHCb::RecVertex& vertex,
				       TrackContainer& tracks) const ;
  private:
    // create a new MultiTrackResiduals
    const Al::MultiTrackResiduals* compute(const TrackResidualContainer& tracks,
					   const Gaudi::XYZPoint& vertexestimate) const ;
    // extrapolate the state in trackresiduals to position z
    StatusCode extrapolate( const Al::TrackResiduals& track,
			    double z, LHCb::State& state, 
			    Al::TrackResiduals::ProjectionMatrix& dResidualdState) const ;
  private:
    ToolHandle<ITrackResidualTool> m_trackresidualtool ;
    ToolHandle<ITrackExtrapolator> m_extrapolator ;
    //ToolHandle<ITrackSelector> m_trackselectorhandle ;
    ITrackSelector* m_trackselector ;
    std::string m_trackselectorname ;
    double m_chiSquarePerDofCut ;
    bool   m_computeCorrelations ;
    typedef std::vector<const Al::MultiTrackResiduals*> ResidualContainer ;
    mutable ResidualContainer m_residuals ;
  } ;
  
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
  
#include <iostream>
#include <algorithm>
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "Event/RecVertex.h"
#include "Event/TrackStateVertex.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "ITrackResidualTool.h"

namespace Al
{
  
  DECLARE_TOOL_FACTORY( VertexResidualTool );
  
  VertexResidualTool::VertexResidualTool(const std::string& type,
				       const std::string& name,
				       const IInterface* parent)
    : GaudiTool(type,name,parent),
      m_trackresidualtool("Al::TrackResidualTool"), // important: use the toolsvc, because of caching!
      m_extrapolator("TrackMasterExtrapolator"),
      //m_trackselectorhandle("TrackSelector",this),
      m_chiSquarePerDofCut(10),
      m_computeCorrelations(true)
  {
    // interfaces
    declareInterface<IVertexResidualTool>(this);
    declareProperty("TrackResidualTool",m_trackresidualtool) ;
    declareProperty("Extrapolator",m_extrapolator) ;
    // declareProperty("TrackSelectorHandle",m_trackselectorhandle) ;
    declareProperty("MyTrackSelector",m_trackselectorname="TrackSelector") ;
    declareProperty("ComputeCorrelations",m_computeCorrelations) ;
  }
  
  StatusCode VertexResidualTool::initialize()
  {
    StatusCode sc = GaudiTool::initialize();
    if (sc.isFailure()){
      return Error("Failed to initialize",sc);
    }
    m_trackresidualtool.retrieve().ignore() ;
    m_extrapolator.retrieve().ignore() ;
    m_trackselector = tool<ITrackSelector>(m_trackselectorname,this) ;
    //m_trackselectorhandle.retrieve().ignore() ;
    incSvc()->addListener(this, IncidentType::EndEvent);
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
  
  struct CompareLHCbIds {
    bool operator()(const LHCb::LHCbID& lhs, const LHCb::LHCbID& rhs) const {
      return lhs.lhcbID() < rhs.lhcbID() ;
    }
  } ;
  
  struct TrackClonePredicate
  {
    std::set<LHCb::LHCbID,CompareLHCbIds> ids ;
    TrackClonePredicate( const LHCb::Track* lhs ) { ids.insert(lhs->lhcbIDs().begin(),lhs->lhcbIDs().end()) ; }
    bool operator()(const LHCb::Track* rhs) const {
      // the requirement is that all LHCbIDs of rhs appear in lhs
      bool foundall(true) ;
      for( std::vector<LHCb::LHCbID>::const_iterator id=rhs->lhcbIDs().begin() ;
	   foundall && id != rhs->lhcbIDs().end(); ++id) 
	foundall = ids.find( *id ) != ids.end() ;
      return foundall ;
    }
  } ;
  
  const Al::MultiTrackResiduals* VertexResidualTool::get(const LHCb::RecVertex& vertex,
							 TrackContainer& tracks) const
  {
//     std::cout << "compcorr, name of track selector: "
// 	      << m_computeCorrelations << " "
// 	      << m_trackselector->name() << " "
// 	      << std::endl ;
    // loop over the list of vertices, collect tracks in the vertex
    const Al::MultiTrackResiduals* rc(0) ;
    TrackContainer usedtracks ;
    TrackResidualContainer trackresiduals ;
    for( SmartRefVector<LHCb::Track>::const_iterator itrack = vertex.tracks().begin() ;
	 itrack !=  vertex.tracks().end(); ++itrack) {
      // we'll use the track in the provided list, not the track in the vertex
      TrackContainer::const_iterator jtrack = std::find_if( tracks.begin(), tracks.end(), TrackClonePredicate(*itrack) ) ;
      if( jtrack != tracks.end() /* && ((**jtrack).hasT()||(**jtrack).hasTT() )*/
	  && m_trackselector->accept( **jtrack ) ) {
	//assert( (**jtrack).hasT()||(**jtrack).hasTT() ) ;
	const Al::TrackResiduals* trackres = m_trackresidualtool->get( **jtrack ) ;
	if(trackres) {
	  usedtracks.push_back( *jtrack ) ; 
	  trackresiduals.push_back( trackres ) ;
	} else {
	  warning() << "No residuals returned by trackresidualtool!" << endreq ;
	}
      }
    }
    
    if(trackresiduals.size()>=2) {
      debug() << "OK, will compute vertex with  " <<  trackresiduals.size()
	     << " out of " << vertex.tracks().size() << " tracks in original vertex." << endreq ;
      rc = compute( trackresiduals, vertex.position() ) ;
      if(rc) {
	// tool owns whatever it returns
	m_residuals.push_back(rc) ;
	// remove tracks from input list. (should use std::remove)
	TrackContainer unusedtracks ;
	for( TrackContainer::const_iterator itrack = tracks.begin(); itrack != tracks.end(); ++itrack)
	  if( std::find(usedtracks.begin(), usedtracks.end(), *itrack) ==  usedtracks.end() )
	    unusedtracks.push_back( *itrack ) ; 
	tracks = unusedtracks ;
      } 
    } else {
      warning() << "Did not find enough tracks in vertex"
		<< vertex.tracks().size() << " " << tracks.size() << endreq ;
    }
    
    return rc ;
  }

  struct TrackContribution
  {
    const Al::TrackResiduals* trackresiduals ;
    LHCb::State               inputstate ;
    Al::TrackResiduals::ProjectionMatrix dResidualdState ;
    size_t offset ;
  } ;

  const Al::MultiTrackResiduals* VertexResidualTool::compute(const TrackResidualContainer& tracks,
							     const Gaudi::XYZPoint& vertexestimate) const
  {
    Al::MultiTrackResiduals* rc(0) ;
    bool success = true ;

    // first we need to propagate all tracks to the vertex and keep
    // track of the correlation between the state at the vertex and
    // the reference state inside the TrackResiduals object
    
    typedef std::vector<TrackContribution> Contributions ;
    Contributions states ;
    Al::Residuals::NodeContainer allnodes ;
    double totalchisq(0) ;
    size_t totalndof(0) ;

    for( TrackResidualContainer::const_iterator itrack = tracks.begin() ;
	 itrack != tracks.end() && success; ++itrack) {
      TrackContribution tc ;
      tc.offset = allnodes.size() ;
      tc.trackresiduals = *itrack ;
      StatusCode sc = extrapolate( **itrack, vertexestimate.z(), tc.inputstate, tc.dResidualdState ) ;
      
      if( sc.isSuccess() ) {
	states.push_back( tc ) ;
	allnodes.insert(allnodes.end(), (*itrack)->nodes().begin(), (*itrack)->nodes().end()) ;
	totalchisq += (*itrack)->chi2() ;
	totalndof  += (*itrack)->nDoF() ;
      }
      else {
	warning() << "Extrapolation failed" << endreq ;
	success = false ;
      }
    }
    
    if(success) {
      // now vertex the states
      LHCb::TrackStateVertex vertex ;
      for( Contributions::const_iterator i = states.begin() ; i!= states.end(); ++i)
	vertex.addTrack( i->inputstate ) ;

      LHCb::TrackStateVertex::FitStatus fitstatus = vertex.fit() ;

      double vchi2 = vertex.chi2() ; // cache it, because I know it is slow
      debug() << "Fitted vertex, chi2/dof=" << vchi2 << "/" << vertex.nDoF() << endreq ;
      debug() << "Vertex position orig/new="
	      << vertexestimate << "/" << vertex.position() << endreq ;
      
      if(fitstatus == LHCb::TrackStateVertex::FitSuccess && vchi2 / vertex.nDoF() < m_chiSquarePerDofCut) {
	// create a vertexresiduals object
	totalchisq += vchi2 ;
	totalndof  += vertex.nDoF() ;
	rc = new Al::MultiTrackResiduals( allnodes, totalchisq, totalndof, states.size(), vchi2, vertex.nDoF()  ) ;
	debug() << "created the vertex: " << allnodes.size() << endreq ;

	// calculate all new residuals and all correlations
	bool computationerror(false) ;
	for(size_t i = 0; i<states.size() && !computationerror; ++i) {
	  Gaudi::TrackVector    deltaState = vertex.state(i).stateVector() - vertex.inputState(i).stateVector() ;
	  Gaudi::TrackSymMatrix deltaCov   = vertex.state(i).covariance()  - vertex.inputState(i).covariance() ;
	  
	  size_t ioffset = states[i].offset ;
	  for(size_t irow = 1; irow<=states[i].trackresiduals->size(); ++irow) {
	    
	    double deltar = (states[i].dResidualdState[irow-1] * deltaState)(0) ;
	    rc->m_r(irow+ioffset) = states[i].trackresiduals->m_r(irow) + deltar ;
	    
	    for(size_t icol = 1; icol<=irow; ++icol) {
	      double deltaHCH = 
		(states[i].dResidualdState[irow-1] * deltaCov *
		 ROOT::Math::Transpose(states[i].dResidualdState[icol-1]))(0,0) ;
	      rc->m_HCH.fast(irow + ioffset, icol + ioffset) = 
		states[i].trackresiduals->m_HCH.fast(irow,icol) + deltaHCH ;
	      
	      if( icol==irow &&  rc->m_HCH.fast(irow + ioffset, irow + ioffset) < 0 ) {
		warning() << "problem computing update of track errors"
			  << states[i].trackresiduals->m_HCH.fast(irow,irow) << " "
			  << deltaHCH << std::endl 
			  << deltaCov << endreq ;
		computationerror = true ;
	      }
	    }
	  }
	  
	  // now the correlations. this is the very time-consuming part
	  if( m_computeCorrelations && !computationerror) {
	    for(size_t j =0; j<i && !computationerror; ++j) {
	      size_t joffset = states[j].offset ;
	      for(size_t irow = 1; irow<=states[i].trackresiduals->size(); ++irow) {
		// store this intermediate matrix too save some time
		Gaudi::TrackProjectionMatrix A = states[i].dResidualdState[irow-1] * vertex.stateCovariance(i,j) ;
		for(size_t jcol = 1; jcol<=states[j].trackresiduals->size(); ++jcol)
		  rc->m_HCH.fast(irow + ioffset, jcol + joffset) = 
		    (A * ROOT::Math::Transpose( states[j].dResidualdState[jcol-1] ))(0,0) ;
	      }
	    }
	  }
       
	  // Let's check the result. Keep track of the roots.
	  if(!computationerror) {
	    bool error(false) ;
	    const double tol = 1e-3 ;
	    std::vector<double> HCHdiagroots ;
	    std::vector<double> Rdiagroots ;
	    for(size_t irow=0; irow<rc->size() && !error; ++irow) {
	      double c = rc->HCH(irow,irow) ;
	      error = ! (0<=c) ;
	      if(error) warning() << "found negative element on diagonal: " << c << endreq ;
	      error = ! ( c<rc->V(irow)) ;
	      if(error) warning() << "found too large element on diagonal: " << c/rc->V(irow) << endreq ;
	      
	      HCHdiagroots.push_back( std::sqrt(c) ) ;
	      Rdiagroots.push_back( std::sqrt(rc->V(irow) - c) ) ;
	    }
	    
	    bool offdiagerror(false) ;
	    for(size_t irow=0; irow<rc->size() && !error; ++irow) {
	      for(size_t icol=0; icol<irow && !error; ++icol) {
		double c = rc->HCH(irow,icol) / (HCHdiagroots[irow]*HCHdiagroots[icol]) ;
		double d = rc->HCH(irow,icol) / (Rdiagroots[irow]*Rdiagroots[icol]) ;
		bool thiserror =  ! (-1-tol < c && c <1+tol) || ! (-1-tol < d && d <1+tol) ;
		if(thiserror) {
		  warning() << "found bad element on offdiagonal: " 
			    << irow << " " << icol << " " 
			    << c << " " << d << endreq ;
		}
		offdiagerror |= thiserror;
	      }
	    }
	    computationerror |=  error || offdiagerror ;
	  }
	}
	
	if(computationerror) {
	  delete rc ;
	  rc = 0 ;
	  warning() << "VertexResidualTool::compute failed" << endreq ;
	} 
      } else {
	warning() << "rejected vertex with chisqu/dof: "
		  << vchi2 / vertex.nDoF() << endreq ;
      }
    }
    return rc ;
  }
  
  StatusCode VertexResidualTool::extrapolate( const Al::TrackResiduals& track,
					      double z,
					      LHCb::State& state,
					      Al::TrackResiduals::ProjectionMatrix& dResidualdState ) const
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
    state = track.state() ;
    StatusCode sc = m_extrapolator->propagate(state,z,&F) ;
    if( sc.isSuccess() ) {

      // calculate the correlation
      // stateResidualCorrelation = convertToCLHEP(F) * track.m_stateResCov ;
      
      // invert the covariance matrix
      Gaudi::TrackSymMatrix Cinv = state.covariance() ;
      bool OK = Gaudi::Math::invertPosDefSymMatrix( Cinv ) ;
      if(!OK) {
	warning() << "Inversion error in VertexResidualTool::extrapolate" << endreq ;
	sc = StatusCode::FAILURE ;
      } else {
	dResidualdState.resize(track.size()) ;
	for(size_t inode=0; inode<track.size(); ++inode)
	  dResidualdState[inode] = ( track.residualStateCov()[inode] * ROOT::Math::Transpose(F) ) * Cinv ;
      }
    } 
    return sc ;
  }
}
