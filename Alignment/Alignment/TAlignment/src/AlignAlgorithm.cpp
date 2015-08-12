// $Id: AlignAlgorithm.cpp,v 1.65 2010-10-08 07:23:00 wouter Exp $
// Include files
// from std
// #include <utility>
#include <algorithm>
#include <iterator>
#include <fstream>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiUtils/Aida2ROOT.h"

// from root
#include "TH1D.h"

// from Event
#include "Event/TwoProngVertex.h"
#include "Event/ODIN.h"
#include "Event/Particle.h"

// from DetDesc
#include "DetDesc/DetectorElement.h"

// from Kernel
#include "Kernel/LHCbID.h"

// from BOOST
#include "boost/assign/std/vector.hpp"
#include "boost/assign/list_of.hpp"
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

// local
#include "AlignAlgorithm.h"
#include "AlParameters.h"
#include "AlignKernel/AlEquations.h"
#include "Event/AlignSummaryData.h"

#include <boost/foreach.hpp>
#include "DetDesc/RunChangeIncident.h"

#include <vector>
#include "GaudiKernel/IMonitorSvc.h"
#include "GaudiKernel/IUpdateable.h"
#include "GaudiKernel/SmartIF.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AlignAlgorithm
//
// 2007-03-05 : Jan Amoraal
//-----------------------------------------------------------------------------

namespace {

 Gaudi::Vector6 convert(const Gaudi::Matrix1x6& m) {
   Gaudi::Vector6 v;
   for (int i = 0; i < 6; ++i) v(i) = m(0,i);
   return v;
 }

}

using namespace LHCb;
using namespace boost;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( AlignAlgorithm )

class HistoUpdater
{
  public:
    std::string m_PartitionName;
    std::string m_RefFileName;
    unsigned long m_Reference;
    IMonitorSvc *m_MonSvc;
    void setMonitorService(IMonitorSvc *ms)
    {
      m_MonSvc = ms;
    }
    HistoUpdater(): m_Reference(0),m_MonSvc(0)
    {
    }
    unsigned long readReference()
    {
      FILE *f = fopen(m_RefFileName.c_str(),"r");
      fscanf(f,"%lu",&m_Reference);
      fclose(f);
      return m_Reference;
    }
    unsigned long getReference()
    {
      return m_Reference;
    }
    void Update(int runnr)
    {
      unsigned long ref=readReference();
      if (m_MonSvc != 0)
      {
        SmartIF<IUpdateableIF> aaa(m_MonSvc);
        if (aaa.isValid())aaa->update(runnr*100+ref);
      }
    }
};

AlignAlgorithm::AlignAlgorithm( const std::string& name,
        ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator ),
    m_iteration(0u),
    m_nIterations(0u),
    m_nTracks(0u),
    m_covFailure(0u),
    m_align(0),
    m_projSelector(0),
    m_trackresidualtool("Al::TrackResidualTool"),
    m_vertexresidualtool("Al::VertexResidualTool"),
    m_updatetool("Al::AlignUpdateTool"),
    m_vertextrackselector("TrackSelector",this),
    m_xmlWritersTool("WriteMultiAlignmentConditionsTool"),
    m_fireRunChange(false),
    m_equations(0),
    m_resetHistos(false)
{
  m_HistoUpdater = new HistoUpdater();
  declareProperty("NumberOfIterations"          , m_nIterations        = 10u );
  declareProperty("TracksLocation"              , m_tracksLocation     = TrackLocation::Default  );
  declareProperty("VertexLocation"              , m_vertexLocation     = "" );
  declareProperty("DimuonLocation"              , m_dimuonLocation     = "" );
  declareProperty("ParticleLocation"            , m_particleLocation   = "" );
  declareProperty("ProjectorSelector"           , m_projSelectorName   = "TrackProjectorSelector");
  declareProperty("UseCorrelations"             , m_correlation        = true                    );
  declareProperty("UpdateInFinalize"            , m_updateInFinalize   = false                   );
  declareProperty("OutputDataFile"              , m_outputDataFileName = "" ) ;
  declareProperty("InputDataFiles"              , m_inputDataFileNames ) ;
  declareProperty("Chi2Outlier"                 , m_chi2Outlier        = 10000 ) ;
  declareProperty("UpdateTool",m_updatetool) ;
  declareProperty("TrackResidualTool",m_trackresidualtool) ;
  declareProperty("VertexResidualTool",m_vertexresidualtool) ;
  declareProperty("XmlWritersTool",m_xmlWritersTool) ;
  declareProperty("MaxTracksPerVertex",m_maxTracksPerVertex = 8 ) ;
  declareProperty("MinTracksPerVertex",m_minTracksPerVertex = 2 ) ;

  declareProperty("AlignSummaryDataSvc", m_alignSummaryDataSvc = "DetectorDataSvc" ) ;
  //"HistogramDataSvc" ) ;
  declareProperty("AlignSummaryLocation", m_alignSummaryLocation = "AlignDerivativeData") ;
  declareProperty("FillHistos", m_fillHistos = false ) ;
  declareProperty("ForcedInitialTime", m_forcedInitTime = 0 ) ;
  declareProperty("OnlineMode", m_Online=false);
  //  declareProperty("XmlWriters",m_xmlWriterNames) ;
//  For Online Running
  declareProperty("PartitionName",m_HistoUpdater->m_PartitionName="LHCbA");
  declareProperty("ReferenceFile",m_HistoUpdater->m_RefFileName="");
  declareProperty("RunList",m_RunList);

  m_runnr = 0;
}

AlignAlgorithm::~AlignAlgorithm()
{
  delete m_HistoUpdater;
}


StatusCode AlignAlgorithm::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  /// Set histogram path
    //if ("" == histoTopDir()) setHistoTopDir("Alignment/");

  /// Get pointer to incident service and add listener
  IIncidentSvc* incSvc = svc<IIncidentSvc>("IncidentSvc");
  if (!incSvc) return Error("==> Failed to retrieve IncidentSvc!", StatusCode::FAILURE);
  incSvc->addListener(this, "UpdateConstants");
  incSvc->addListener(this, "BeginEvent");
  incSvc->addListener(this, "DAQ_PAUSE");

  /// Get tool to align detector
  m_align = tool<IGetElementsToBeAligned>("GetElementsToBeAligned");
  if (!m_align) return Error("==> Failed to retrieve detector selector tool!", StatusCode::FAILURE);
 
  sc = m_trackresidualtool.retrieve() ;
  if ( sc.isFailure() ) return sc;

  sc = m_vertexresidualtool.retrieve() ;
  if ( sc.isFailure() ) return sc;

  sc = m_updatetool.retrieve() ;
  if ( sc.isFailure() ) return sc;

  sc = m_vertextrackselector.retrieve() ;
  if ( sc.isFailure() ) return sc;

    /// Get range  detector elements
  const Elements& elements = m_align->elements();

  if (printDebug()) {
    debug() << "==> Got " << elements.size() << " elements to align!" << endmsg;
    for(Elements::const_iterator i = elements.begin(); i!= elements.end(); ++i) {
      const AlParameters::DofMask& ownDoFMask = (*i)->dofMask();
      debug() <<  "        " << (**i) << endmsg;
      const std::vector<std::string> dofs = boost::assign::list_of("Tx")("Ty")("Tz")("Rx")("Ry")("Rz");
      debug() << "DOFs: ";
      for (AlParameters::DofMask::const_iterator j = ownDoFMask.begin(), jEnd = ownDoFMask.end(); j != jEnd; ++j) {
        if ((*j)) info() << dofs.at(std::distance(ownDoFMask.begin(), j));
      }
      debug() << endmsg;
    }
  }
  if (m_HistoUpdater->m_RefFileName.empty()) {
    m_HistoUpdater->m_RefFileName =
        "/group/online/dataflow/options/"+m_HistoUpdater->m_PartitionName+"/Alignement_Reference_File.txt";
  }

  /// create the summary data and register in the TES
  LHCb::AlignSummaryData* m_summaryData = new LHCb::AlignSummaryData(0) ;
  IDataProviderSvc* datasvc = svc<IDataProviderSvc>(m_alignSummaryDataSvc,true) ;
  sc = datasvc->registerObject(m_alignSummaryLocation,m_summaryData) ;
  if( !sc.isSuccess() ) {
    error() << "cannot register object. statuscode = "
            << sc << endreq ;
  }
  m_equations = &(m_summaryData->equations()) ;

  /// Get projector selector tool
  m_projSelector = tool<ITrackProjectorSelector>(m_projSelectorName, "Projector", this);
  if (!m_projSelector) return Error("==> Failed to retrieve projector selector tool", StatusCode::FAILURE);

  /// Monitoring
  /// Book residual histograms
  /// Residuals
  /// @todo: this should go into a monitoring tool
  if( m_fillHistos ) {
    info() << "booking histograms assuming " << m_nIterations << " iterations " << endmsg;
    for(Elements::const_iterator i = elements.begin(); i!= elements.end(); ++i)
      m_elemHistos.push_back( new AlElementHistos(*this,**i,m_nIterations) ) ;
    m_resetHistos = false ;
  }

  info() << "Use correlations = " << m_correlation << endreq ;

  //  for( auto i : m_xmlWriterNames )
  //    m_xmlWriters.push_back( tool<IWriteAlignmentConditionsTool>(i,this) ) ;

  if (m_Online)
  {
    m_HistoUpdater->setMonitorService(monitorSvc());
  }
  return StatusCode::SUCCESS;
}

StatusCode AlignAlgorithm::finalize() {
  if ( m_updateInFinalize && !m_Online ) update() ;

  for( std::vector<AlElementHistos*>::iterator ielem = m_elemHistos.begin() ;
       ielem != m_elemHistos.end(); ++ielem) delete *ielem ;

  m_trackresidualtool.release() ;
  m_vertexresidualtool.release() ;
  m_updatetool.release() ;
  if (m_Online)
  {
    m_HistoUpdater->m_MonSvc=0;
  }
  return GaudiHistoAlg::finalize();
}

StatusCode AlignAlgorithm::stop()
{
  if (m_Online)
  {
    if(!m_outputDataFileName.empty())
      m_equations->writeToFile( m_outputDataFileName.c_str() ) ;
    m_fireRunChange = true ;
  }
  return StatusCode::SUCCESS;
}

StatusCode AlignAlgorithm::start()
{
  StatusCode sc;
  if (m_Online) {
    // reset contents of ASD
    reset() ;
//    std::vector<std::string> workers = toolSvc()->getInstances("IAlignWork");
//    if (workers.size()>0)
//    {
//      sc = toolSvc()->retrieveTool("IAlignWork",workers[0],m_IAlwork,0,false);
//      if (!sc)
//      {
//        Error("Cannot retrieve IAlignWork tool");
//        return sc;
//      }
//    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode AlignAlgorithm::execute() {

  // Reset histograms if required
  if(m_resetHistos) resetHistos() ;

  // Make sure that the alignment frames are properly initialized on the first event
  if( m_equations->initTime() != 0 && m_equations->initTime() != m_align->initTime() ) {
    error() << "Mismatch in init time: " << m_equations->initTime() << " " << m_align->initTime() << ". Aborting."
        << endreq ;
    return StatusCode::FAILURE ;
  }

  Gaudi::Time eventtime ;
  if( exist<LHCb::ODIN>( LHCb::ODINLocation::Default ) ){
    const LHCb::ODIN* odin = get<LHCb::ODIN> ( LHCb::ODINLocation::Default );
    eventtime = odin->eventTime() ;
    m_runnr = odin->runNumber() ;
  }

  if( m_equations->initTime() == 0 ) {
    if( m_forcedInitTime ) 
      m_align->initAlignmentFrame( m_forcedInitTime ) ;
    else
      m_align->initAlignmentFrame(eventtime) ;
    m_align->initEquations( *m_equations ) ;
  }

  // Get tracks. Copy them into a vector, because that's what we need for dealing with vertices.
  LHCb::Track::Range tracks;
  if( !m_tracksLocation.empty() ) {
    tracks = get<LHCb::Track::Range>(m_tracksLocation);
    if (printVerbose()) verbose() << "Number of tracks in container " + m_tracksLocation + ": " << tracks.size() << endmsg;
  }
  m_nTracks += tracks.size() ;

  // First just copy the tracks
  std::vector<const LHCb::Track*> selectedtracks ;
  for(  LHCb::Track::Range::const_iterator iTrack = tracks.begin() ; iTrack != tracks.end() ; ++iTrack)
    // just a sanity check
    if( (*iTrack)->fitStatus()==LHCb::Track::Fitted &&
  !(*iTrack)->checkFlag( LHCb::Track::Invalid ) &&
  (*iTrack)->nDoF() > 0 &&
  !(*iTrack)->nodes().empty() &&
  testNodes( **iTrack ) ) {
      const Al::Residuals* res = m_trackresidualtool->get(**iTrack) ;
      if( res ) {
  selectedtracks.push_back( *iTrack ) ;
      } else {
  warning() << "Error computing residual cov matrix. Skipping track of type "
      << (*iTrack)->type() << " with key: " << (*iTrack)->key()
      << " and chi2 / dof: " << (*iTrack)->chi2() << "/" << (*iTrack)->nDoF() << endmsg ;
  ++m_covFailure;
      }
    } else {
      debug() << "Skipping bad track:"
        << " fitstatus = " << (*iTrack)->fitStatus()
        << " nDoF = " << (*iTrack)->nDoF()
        << " #nodes = " << (*iTrack)->nodes().size() << endreq ;
    }

  // Now I got a bit worried about overlaps. Sort these tracks in the
  // number of LHCbIDs. Then remove overlapping tracks.
  std::sort(selectedtracks.begin(), selectedtracks.end(),
      boost::lambda::bind(&LHCb::Track::nLHCbIDs,*boost::lambda::_1) >
      boost::lambda::bind(&LHCb::Track::nLHCbIDs,*boost::lambda::_2)) ;
  std::vector< const LHCb::Track* > nonoverlappingtracks ;
  std::set< unsigned int > selectedlhcbids ;
  for( std::vector<const LHCb::Track*>::const_iterator itr = selectedtracks.begin() ;
       itr != selectedtracks.end(); ++itr) {
    std::set<unsigned int> ids ;
    for( std::vector<LHCb::LHCbID>::const_iterator iid = (*itr)->lhcbIDs().begin() ;
  iid != (*itr)->lhcbIDs().end(); ++iid) ids.insert( iid->lhcbID() ) ;
    //std::set<LHCb::LHCbID> ids( (*itr)->lhcbIDs().begin(), (*itr)->lhcbIDs().end() ) ;
    if( ids.size() != (*itr)->lhcbIDs().size() ) {
      warning() << "Skipping track with non-unique LHCbIds. Type= "
    << (*itr)->type() << " " << (*itr)->history() << endreq ;
    } else {
      std::set<unsigned int> allids = selectedlhcbids ;
      allids.insert( ids.begin(), ids.end() ) ;
      if( allids.size() != selectedlhcbids.size() + ids.size() ) {
  //warning() << "Skipping track of type " << (*itr)->type()
  // << "because it overlaps with another selected track" << endreq ;
      } else {
  nonoverlappingtracks.push_back(*itr) ;
  selectedlhcbids = allids ;
      }
    }
  }
  if( selectedtracks.size() != nonoverlappingtracks.size() ) {
    static int count(0) ;
    if(++count<20)
      info() << "Rejected " << selectedtracks.size() - nonoverlappingtracks.size() << " out of "
      << selectedtracks.size() << " tracks because of overlaps." << endreq ;
  }
  selectedtracks = nonoverlappingtracks ;

  // Now deal with dimuons, vertices, if there are any.
  size_t numusedtracks(0) ;
  size_t numusedvertices(0) ;
  size_t numuseddimuons(0) ;
  if( !m_particleLocation.empty() ) {
    LHCb::Particle::Range particles = get<LHCb::Particle::Range>(m_particleLocation) ;
    BOOST_FOREACH( const LHCb::Particle* p, particles ) {
      const Al::MultiTrackResiduals* res = m_vertexresidualtool->get(*p) ;
      if( res && accumulate( *res ) ) {
  m_equations->addVertexChi2Summary( res->vertexChi2(), res->vertexNDoF() ) ;
  ++numuseddimuons ;
  numusedtracks += res->numTracks() ;
  removeParticleTracks( *p, selectedtracks ) ;
      }
    }
  }

  if( !m_dimuonLocation.empty() ) {
    const LHCb::TwoProngVertices* vertices = get<LHCb::TwoProngVertices>(m_dimuonLocation);
    if(vertices ) {
      for( LHCb::TwoProngVertices::const_iterator ivertex = vertices->begin() ;
    ivertex != vertices->end(); ++ivertex ) {
  // rebuild this vertex from tracks in the selected track list
  const LHCb::RecVertex* clone = cloneVertex( **ivertex, selectedtracks ) ;
  if( clone ) {
    const Al::MultiTrackResiduals* res = m_vertexresidualtool->get(*clone) ;
    if( res && accumulate( *res ) ) {
      m_equations->addVertexChi2Summary( res->vertexChi2(), res->vertexNDoF() ) ;
      ++numuseddimuons ;
      numusedtracks += res->numTracks() ;
      removeVertexTracks( *clone, selectedtracks ) ;
    }
    delete clone ;
  } else {
    info() << "Could not rebuild psi vertex from input track list." << endreq ;
  }
      }
    }
  }

  if( !m_vertexLocation.empty() ) {
    LHCb::RecVertex::Range vertices = get<LHCb::RecVertex::Range>(m_vertexLocation);

    if( !vertices.empty() ) {
      for( LHCb::RecVertex::Range::const_iterator ivertex = vertices.begin() ;
    ivertex != vertices.end(); ++ivertex ) {
  // split this vertex in vertices of the right size
  VertexContainer splitvertices ;
  splitVertex( **ivertex, selectedtracks, splitvertices ) ;
  for( VertexContainer::iterator isubver = splitvertices.begin() ;
      isubver != splitvertices.end() ; ++isubver ) {
    const Al::MultiTrackResiduals* res = m_vertexresidualtool->get(*isubver) ;
    if (res && accumulate( *res ) ) {
      m_equations->addVertexChi2Summary( res->vertexChi2(), res->vertexNDoF() ) ;
      ++numusedvertices ;
      numusedtracks += res->numTracks() ;
      removeVertexTracks( *isubver, selectedtracks ) ;
      plot( isubver->tracks().size(), "NumTracksPerPV", -0.5,30.5,31) ;
    }
  }
      }
    }
  }

  // Loop over the remaining tracks
  if (printVerbose()) verbose() << "Number of tracks left after processing vertices: " << selectedtracks.size() << endreq ;

  for( std::vector<const LHCb::Track*>::const_iterator iTrack = selectedtracks.begin() ;
       iTrack != selectedtracks.end(); ++iTrack ) {

    // this cannot return a zero pointer since we have already checked before
    const Al::Residuals* res = m_trackresidualtool->get(**iTrack) ;
    assert(res!=0) ;
    if( res && accumulate( *res ) ) ++numusedtracks ;
  }

  m_equations->addEventSummary( numusedtracks, numusedvertices, numuseddimuons, eventtime, m_runnr ) ;

  return StatusCode::SUCCESS;
}


bool AlignAlgorithm::accumulate( const Al::Residuals& residuals )
{
  bool accept = false ;
  if( residuals.size() > 0 &&
      (residuals.nAlignables() > 1 || residuals.nExternalHits()>0 ) ) {
    accept = true ;

    // let's first get the derivatives
    typedef Gaudi::Matrix1x6 Derivative ;
    std::vector< Derivative > derivatives ;
    derivatives.reserve( residuals.size() ) ;
    for (Al::Residuals::ResidualContainer::const_iterator
    ires = residuals.residuals().begin();
  ires != residuals.residuals().end() && accept;++ires) {
      // Project measurement
      const LHCb::Measurement& meas = ires->node().measurement() ;
      const ITrackProjector* proj = m_projSelector->projector(meas);
      if (!proj) {
  error() << "==> Could not get projector for selected measurement!" << endmsg;
  accept = false ;
      } else {
  LHCb::StateVector state(ires->node().state().stateVector(),ires->node().state().z());
  derivatives.push_back( proj->alignmentDerivatives(state,meas,Gaudi::XYZPoint(0,0,0)) * ires->element().jacobian() ) ;
      }
    }

    if( accept ) {
      size_t nodeindex(0) ;
      for (Al::Residuals::ResidualContainer::const_iterator
      ires = residuals.residuals().begin() ;
    ires != residuals.residuals().end();++ires, ++nodeindex) {
  const Derivative& deriv = derivatives[nodeindex] ;
  Al::ElementData& elementdata = m_equations->element(ires->element().index()) ;
  elementdata.addHitSummary(ires->V(), ires->R(), ires->node().state().position()) ;

  // add to the first derivative

  // 'alignment' outliers are not added to the first
  // derivative. However, since they have been used in the track
  // fit, they must be added to the 2nd derivative, otherwise we
  // loose the unconstrained modes. we'll solve the relative
  // normalization when we build the full linear system.
  if( ! (ires->chi2() > m_chi2Outlier) )
    // FIX ME: get rid of minus sign
    elementdata.m_dChi2DAlpha  -= 1/ires->V() * ires->r() * convert(deriv) ;
  else
    elementdata.m_numOutliers++ ;

  // add V^{-1} ( V - HCH ) V^{-1} to the 2nd derivative
  Gaudi::SymMatrix6x6 tmpsym ;
  ROOT::Math::AssignSym::Evaluate(tmpsym,Transpose(deriv)*deriv ) ;
  elementdata.m_d2Chi2DAlpha2 += (1/ ires->V() * ires->R() * 1/ ires->V() ) * tmpsym ;

  // compute the derivative of the track parameters, used for one of the
  // canonical constraints:
  //   dx/dalpha = - dchi^2/dalphadx * ( dchi^2/dx^2)^{-1}
  //             = - 2 * dr/dalpha * V^{-1} * H * C
  // This stil needs some work because I actually want the
  // derivative to the first state.
  //
  // Now we actually want x to be the reference state:
  //  dxref/dxi = Cref,i * Ci^{-1} -->
  //  dxref/dalpha = -2 * dr/dalpha * V^{-1} * cov( r_i, x_ref)
  // The last object on the right is already stored in the residual class
  elementdata.m_dStateDAlpha  += ROOT::Math::Transpose(deriv) * 1/ires->V() * ires->residualStateCov() ;
  // Same for the vertex
  elementdata.m_dVertexDAlpha += ROOT::Math::Transpose(deriv) * 1/ires->V() * ires->residualVertexCov() ;
      }

      // add V^{-1} R V^{-1} to the 2nd derivative for the offdiagonal entries
      if( m_correlation ) {
  for(Al::Residuals::CovarianceContainer::const_iterator ihch = residuals.HCHElements().begin() ;
      ihch != residuals.HCHElements().end(); ++ihch) {
    assert(ihch->col < ihch->row) ;
    size_t rowindex = residuals.residual(ihch->row).element().index() ;
    size_t colindex = residuals.residual(ihch->col).element().index() ;
    const Derivative& rowderiv = derivatives[ihch->row] ;
    const Derivative& colderiv = derivatives[ihch->col] ;
    double c = ihch->val / (residuals.residual(ihch->row).V()*residuals.residual(ihch->col).V()) ;
    if( rowindex < colindex ) {
      Al::ElementData& elementdata = m_equations->element(rowindex) ;
      elementdata.m_d2Chi2DAlphaDBeta[colindex].add( -c * Transpose(rowderiv) * colderiv) ;
    } else if (rowindex > colindex) {
      Al::ElementData& elementdata = m_equations->element(colindex) ;
      elementdata.m_d2Chi2DAlphaDBeta[rowindex].add( -c * Transpose(colderiv) * rowderiv) ;
    } else if(rowindex == colindex ) {
      Al::ElementData& elementdata = m_equations->element(rowindex) ;
      // make sure to symmetrize: add diagonal elements in both orders.
      Gaudi::Matrix6x6 tmpasym = Transpose(rowderiv) * colderiv ;
      Gaudi::SymMatrix6x6 tmpsym ;
      ROOT::Math::AssignSym::Evaluate(tmpsym, tmpasym + Transpose(tmpasym)) ;
      // 	    std::cout << "row,col: "
      // 		      << ihch->row << " " << ihch->col << " " << rowindex << " " << colindex << std::endl ;
      // 	    std::cout << "row deriv: " << rowderiv << std::endl ;
      // 	    std::cout << "col deriv: " << colderiv << std::endl ;
      // 	    std::cout << "tmpasym: " << std::endl
      // 		      << tmpasym << std::endl
      // 		      << "tmpsym: " << std::endl
      // 		      << tmpsym << std::endl ;
      elementdata.m_d2Chi2DAlpha2 -= c * tmpsym ;
    }
  }
      }

      // fill some histograms
      if( m_fillHistos ) {
  nodeindex = 0 ;
  for (Al::Residuals::ResidualContainer::const_iterator ires = residuals.residuals().begin();
      ires != residuals.residuals().end();++ires, ++nodeindex) {
    const Derivative& deriv = derivatives[nodeindex] ;
    double f = std::sqrt(ires->R()/ires->V()) ;
    double pull = ires->r() / std::sqrt(ires->R()) ;
    size_t index = ires->element().index() ;
    double sign = deriv(0,0) > 0 ? 1 : -1 ;
    m_elemHistos[index]->m_nHitsHisto->fill(m_iteration);
    m_elemHistos[index]->m_resHisto->fill(m_iteration, sign*ires->r());
    m_elemHistos[index]->m_unbiasedResHisto->fill(m_iteration, sign*ires->r()/f);
    m_elemHistos[index]->m_pullHisto->fill(m_iteration, sign*pull);
    m_elemHistos[index]->m_autoCorrHisto->fill(m_iteration,f) ;
    for(int ipar=0; ipar<6; ++ipar) {
      double weight = deriv(0,ipar) * f ;
      double thispull = pull ;
      if(weight<0) { weight *= -1 ; thispull *= -1 ; }
      // the last minus sign is because somebody defined our first derivative with the wrong sign
      m_elemHistos[index]->m_residualPullHistos[ipar]->fill( -thispull, weight ) ;
    }
  }
      }

      // keep some information about the tracks that we have seen
      m_equations->addChi2Summary( residuals.chi2(), residuals.nDoF(), residuals.nExternalHits() ) ;

      std::vector<size_t> elementsOnTrack( residuals.residuals().size() ) ;
      for(size_t i=0; i< residuals.residuals().size(); ++i)
  elementsOnTrack[i] = residuals.residuals()[i].element().index() ;
      std::sort( elementsOnTrack.begin(),  elementsOnTrack.end() ) ;
      std::vector<size_t>::const_iterator newend = std::unique(elementsOnTrack.begin(),elementsOnTrack.end()) ;
      for( std::vector<size_t>::const_iterator icol = elementsOnTrack.begin();
    icol != newend; ++icol) {
  m_equations->element(*icol).addTrack() ;
  for( std::vector<size_t>::const_iterator irow= elementsOnTrack.begin();
      irow != icol; ++irow)
    m_equations->element(*irow).m_d2Chi2DAlphaDBeta[*icol].addTrack() ;
      }
    }
  }
  return accept ;
}


//=============================================================================
//  Update
//=============================================================================
void AlignAlgorithm::update()
{
  if(m_equations) {
    info() << "Total number of tracks: " << m_nTracks << endreq ;
    info() << "Number of covariance calculation failures: " << m_covFailure << endreq ;
    if( !m_inputDataFileNames.empty() ) {
      for(std::vector<std::string>::const_iterator ifile = m_inputDataFileNames.begin() ;
    ifile != m_inputDataFileNames.end(); ++ifile) {
  Al::Equations tmp(0) ;
  tmp.readFromFile( (*ifile).c_str() ) ;
  m_equations->add( tmp ) ;
  warning() << "Adding derivatives from input file: " << *ifile << " " << tmp.numHits() << " "
      << tmp.totalChiSquare() << " " << m_equations->totalChiSquare() << endreq ;
      }
      if(m_align->initTime() == 0 )
  m_align->initAlignmentFrame( m_equations->initTime() ) ;
    }
    update(*m_equations) ;
  }
}

void AlignAlgorithm::update(const Al::Equations& equations)
{
  if(!m_outputDataFileName.empty())
    m_equations->writeToFile( m_outputDataFileName.c_str() ) ;
  m_updatetool->process(equations,m_iteration,m_nIterations).ignore() ;
  // write the xml
  m_xmlWritersTool->write();
  //  for( auto i: m_xmlWriters ) i->write() ;
}

void AlignAlgorithm::reset() {
  if (printDebug()) debug() << "increasing iteration counter and resetting accumulated data..." << endmsg;
  /// increment iteration counter
  ++m_iteration;
  // set counters to zero
  m_nTracks = 0u;
  m_covFailure = 0u;
  // clear derivatives. update the parameter vectors used for bookkeeping.
  m_equations->clear() ;
  // clear the histograms on the next execute call
  m_resetHistos = true ;
}

void AlignAlgorithm::resetHistos()
{
  m_resetHistos = false ;
  // moved this seperately such that histograms are not reset on last iteration
  if( m_fillHistos ) {
    std::for_each(m_elemHistos.begin(),m_elemHistos.end(),
      boost::lambda::bind(&AlElementHistos::reset,*boost::lambda::_1)) ;
  }
}

StatusCode AlignAlgorithm::queryInterface(const InterfaceID& id, void** ppI) {
  // check the placeholder:
  if (0 == ppI) return StatusCode::FAILURE;
  // check ID of the interface
  if (IIncidentListener::interfaceID() != id) return Algorithm::queryInterface(id, ppI);
  *ppI = static_cast<IIncidentListener*>(this);
  addRef();
  return StatusCode::SUCCESS;
}

void AlignAlgorithm::handle(const Incident& incident) {
  if ("UpdateConstants" == incident.type()) {
    update();
    reset();
  }
  if( m_Online && m_fireRunChange && "BeginEvent"==incident.type() ) {
    // throw a runchange incident to make sure it reads the xml
    IIncidentSvc* incSvc = svc<IIncidentSvc>("IncidentSvc");
    incSvc->fireIncident(RunChangeIncident(name(),m_runnr)) ;
    m_fireRunChange = false ;
  }
  if (m_Online && incident.type() == "DAQ_PAUSE")
  {
    if (m_RunList.size() >0)
    {
      if (m_RunList[0] != "*")
      {
        m_HistoUpdater->Update(::atoi(m_RunList[0].c_str()));
      }
    }
  }
}


namespace {

  void extractTracks( const LHCb::Particle& p,
          std::vector<const LHCb::Track*>& tracks)
  {
    if( p.proto() && p.proto()->track() )
      tracks.push_back(p.proto()->track() ) ;
    BOOST_FOREACH( const LHCb::Particle* dau,
      p.daughters() )
      extractTracks( *dau, tracks ) ;
  }

  struct CompareLHCbIds {
    bool operator()(const LHCb::LHCbID& lhs, const LHCb::LHCbID& rhs) const {
      return lhs.lhcbID() < rhs.lhcbID() ;
    }
  } ;

  struct TrackClonePredicate
  {
    const LHCb::Track* lhs ;
    TrackClonePredicate( const LHCb::Track* tr ) : lhs(tr) {}
    bool operator()(const LHCb::Track* rhs) const {
      // either it is the same tracks, or all LHCbIDs of rhs appear in lhs or vice versa
      return rhs == lhs ||
  lhs->nCommonLhcbIDs(*rhs) == std::min(lhs->lhcbIDs().size(),rhs->lhcbIDs().size()) ;
    }
  } ;

  struct CompareTypeAndSide
  {
    int direction( const LHCb::Track& track ) const {
      return track.checkFlag(LHCb::Track::Backward) ? -1 : 1 ;
    }
    int side( const LHCb::Track& track ) const {
      int txside    = track.firstState().tx() > 0 ? 1 : -1 ;
      return txside * direction(track) ;
    }
    bool operator() ( const LHCb::Track* lhs, const LHCb::Track* rhs ) const {
      return lhs->type() < rhs->type() ||
  (lhs->type()==rhs->type() &&
  (side(*lhs) < side(*rhs) ||
    (side(*lhs) == side(*rhs) && direction(*lhs) < direction(*rhs) ) ) );
    }
  } ;

  template<class TYPE>
  struct IsEqual
  {
    const TYPE* m_p ;
    IsEqual( const TYPE& ref) : m_p(&ref) {}
    bool operator()( const SmartRef<TYPE>& lhs ) { return &(*lhs) == m_p ;}
  } ;
}


// void AlignAlgorithm::removeUsedTracks( const Al::MultiTrackResidual& vertex,
// 				       TrackContainer& tracks) const
// {
//   TrackContainer unusedtracks ;
//   for( TrackContainer::iterator itrack = tracks.begin(); itrack != tracks.end(); ++itrack)
//     if( std::find_if( vertex.tracks().begin(), vertex.tracks().end(),
// 		      IsEqual<LHCb::Track>(**itrack) )==vertex.tracks().end() )
//       unusedtracks.push_back(*itrack) ;
//   tracks = unusedtracks ;
// }

void AlignAlgorithm::selectVertexTracks( const LHCb::RecVertex& vertex,
          const TrackContainer& tracks,
          TrackContainer& tracksinvertex) const
{
  // loop over the list of vertices, collect tracks in the vertex
  tracksinvertex.reserve( tracks.size() ) ;
  for( SmartRefVector<LHCb::Track>::const_iterator itrack = vertex.tracks().begin() ;
       itrack !=  vertex.tracks().end(); ++itrack)
    if( *itrack) {
      // we'll use the track in the provided list, not the track in the vertex
      TrackContainer::const_iterator jtrack = std::find_if( tracks.begin(), tracks.end(), TrackClonePredicate(*itrack) ) ;
      if( jtrack != tracks.end() && m_vertextrackselector->accept( **jtrack ) )
  tracksinvertex.push_back( *jtrack ) ;
    }
}

void AlignAlgorithm::removeVertexTracks( const LHCb::RecVertex& vertex,
          TrackContainer& tracks) const
{
  TrackContainer unusedtracks ;
  for( TrackContainer::iterator itrack = tracks.begin(); itrack != tracks.end(); ++itrack)
    if( std::find_if( vertex.tracks().begin(), vertex.tracks().end(),
          IsEqual<LHCb::Track>(**itrack) )==vertex.tracks().end() )
      unusedtracks.push_back(*itrack) ;
  tracks = unusedtracks ;
}

LHCb::RecVertex* AlignAlgorithm::cloneVertex( const LHCb::RecVertex& vertex,
                const TrackContainer& selectedtracks ) const
{
  LHCb::RecVertex* rc(0) ;
  TrackContainer tracksinvertex ;
  selectVertexTracks( vertex, selectedtracks, tracksinvertex ) ;
  if( tracksinvertex.size() >= 2 ) {
    rc = vertex.clone() ;
    rc->clearTracks() ;
    for( TrackContainer::iterator itrack = tracksinvertex.begin() ;
  itrack != tracksinvertex.end(); ++itrack)
      rc->addToTracks( *itrack ) ;
  }
  return rc ;
}

void AlignAlgorithm::splitVertex( const LHCb::RecVertex& vertex,
          const TrackContainer& tracks,
          VertexContainer& splitvertices) const
{
  //
  TrackContainer tracksinvertex ;
  selectVertexTracks ( vertex, tracks, tracksinvertex ) ;
  if( printVerbose() )
    verbose() << "Tracks in vertex, selected: "
        << vertex.tracks().size() << ", " << tracksinvertex.size() << endreq ;

  if( tracksinvertex.size() >= m_minTracksPerVertex ) {
    // sort the tracks by track type, then by side in the velo
    std::sort( tracksinvertex.begin(), tracksinvertex.end(), CompareTypeAndSide()) ;
    // the number of vertices after splitting. minimum 2 tracks per vertex.
    size_t numsplit =
      tracksinvertex.size() / m_maxTracksPerVertex +
      ( (tracksinvertex.size()%m_maxTracksPerVertex)>=m_minTracksPerVertex ? 1 : 0) ;
    splitvertices.clear() ;
    splitvertices.resize(numsplit,LHCb::RecVertex( vertex.position() ) ) ;
    for( size_t itrack=0; itrack<tracksinvertex.size(); ++itrack)
      splitvertices[  itrack%numsplit ].addToTracks( tracksinvertex[itrack] ) ;
  }

  if( printVerbose() )
    verbose() << "Divided " << tracksinvertex.size() << " over "
        << splitvertices.size() << " vertices." << endreq ;
  // for( VertexContainer::const_iterator iver = splitvertices.begin() ;
  //        iver != splitvertices.end(); ++iver) {
  //     //std::cout << "    #tracks = " << iver->tracks().size() << std::endl ;
  //     assert( iver->tracks().size() >= m_minTracksPerVertex ) ;
  //   }
}

void AlignAlgorithm::removeParticleTracks( const LHCb::Particle& particle,
            TrackContainer& tracks) const
{
  std::vector<const LHCb::Track*> particletracks ;
  extractTracks(particle,particletracks) ;

  TrackContainer unusedtracks ;
  for( TrackContainer::iterator itrack = tracks.begin(); itrack != tracks.end(); ++itrack)
    if( std::find_if( particletracks.begin(), particletracks.end(),
          TrackClonePredicate(*itrack) )==particletracks.end() )
      unusedtracks.push_back(*itrack) ;
  tracks = unusedtracks ;
}


bool AlignAlgorithm::testNodes( const LHCb::Track& track ) const
{
  bool success = true ;
  BOOST_FOREACH( const LHCb::Node* node, track.nodes() )
    if( node->hasMeasurement() && node->type() == LHCb::Node::HitOnTrack ) {

      if( !(node->errResidual2() > TrackParameters::lowTolerance )) {
  warning() << "Found node with zero error on residual: " << track.type() << " "
      << node->measurement().type() << " " << node->errResidual2() << endreq ;
  success = false ;
      }
      if( !(node->errResidual2() < node->errMeasure2() ) ) {
  warning() << "Found node with R2 > V2: " << track.type() << " "
      << node->measurement().type() << " " << node->errResidual2() << " " << node->errMeasure2() << endreq ;
  success = false ;
      }
      if( !( node->errMeasure2() > 1e-6 ) ) {
  warning() << "Found node with very small error on measurement: " << track.type() << " "
      << node->measurement().type() << " " << node->errMeasure2() << endreq ;
  success = false ;
      }
      if( node->errResidual2() < 1e-3 * node->errMeasure2() ) {
  debug() << "Found node with negligible weight: " << track.type() << " "
    << node->measurement().type() << " " << node->errResidual2() << " " << node->errMeasure2() << endreq ;
  success = false ;
      }
    }

  return success ;
}
