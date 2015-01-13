// $Id: OTMonoLayerAlignment.cpp,v 1.65 2010-10-08 07:23:00 wouter Exp $
// Include files

// from std
#include <vector>

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDetDataSvc.h"
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/ToolHandle.h"

// from TrackInterfaces
#include "Kernel/ITrajPoca.h"

// from DetDesc
#include "DetDesc/DetectorElement.h"
#include "OTDet/DeOTModule.h"
#include "OTDet/DeOTDetector.h"
#include "OTDet/OTWireTraj.h"

#include "Event/Track.h"
#include "Event/KalmanFitResult.h"
#include "Event/FitNode.h"
#include "Event/OTMeasurement.h"

// from TrackKernel
#include "TrackKernel/StateZTraj.h"

//#include "ITrackResidualTool.h"
#include "AlignKernel/OTMonoLayerAlignData.h"

// from Kernel
#include "Kernel/LHCbID.h"

#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"

namespace
{
  struct Residual
  {
    double r ;
    double V ;
    double HCH ;
    //int monolayer ;
    Al::OTMonoLayerAlignModuleData::Parameters derivative ;
  } ;

  struct Counters
  {
    size_t nconverged, nnonconverged, nhits ;
    double totalchi2 ;
    Counters() :
      nconverged(0),nnonconverged(0),nhits(0),totalchi2(0.0) {}
  } ;
}

class OTMonoLayerAlignment : public GaudiHistoAlg, virtual public IIncidentListener
{
public:
  OTMonoLayerAlignment(const std::string& name,ISvcLocator* pSvcLocator) ;
  virtual ~OTMonoLayerAlignment() ;
  StatusCode execute() ;
  StatusCode initialize() ;
  StatusCode finalize() ;
  void handle(const Incident& incident) ;
private:
  typedef Al::OTMonoLayerAlignModuleData ModuleData ;
  void update() ;
  void reset() ;
  template<unsigned int N>
  void updateModule( const DeOTModule& module,
		     const ModuleData& mod, 
		     Counters& stats,
		     const std::string& prefix,
		     const ROOT::Math::SMatrix<double,N,6>& projection ) const ;

private:
  enum { NumUniqueModule = LHCb::OTChannelID::NumSequentialModule } ;
  const DeOTModule* module( unsigned int uniquemoduleid ) const {
    return m_modules[uniquemoduleid] ;
  }

  // Interfaces
  std::string m_trackContainer ; // Input Tracks container location
  ToolHandle<ITrajPoca> m_pocatool ;
  std::array<const DeOTModule*,LHCb::OTChannelID::NumSequentialModule> m_modules ;
  Al::OTMonoLayerAlignData* m_moduledata ;

  const DeOTDetector* m_otdet ;
  bool m_updateInFinalize ;
  bool m_useweighting ;
  double m_surveysigma ;
  size_t m_iteration ;
  enum { Translations=1, TranslationsAndRotations=2, Segments=3, DiscontinuousSegments=4 } Mode ;
  int m_mode ;
} ;

//-----------------------------------------------------------------------------
// Implementation file for class : OTMonoLayerAlignment
//
// 2014-11-03 : Wouter Hulsbergen
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OTMonoLayerAlignment )

OTMonoLayerAlignment::OTMonoLayerAlignment( const std::string& name,
					    ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator ),
  m_pocatool("TrajPoca"),
  m_iteration(0)
{
  declareProperty("TrackLocation"   , m_trackContainer = LHCb::TrackLocation::Default  );
  declareProperty("UpdateInFinalize", m_updateInFinalize = true ) ;
  declareProperty("AlignMode", m_mode = DiscontinuousSegments ) ;
  declareProperty("UseWeighting", m_useweighting = false ) ;
  declareProperty("SurveySigma", m_surveysigma = 0.2 ) ;
}

OTMonoLayerAlignment::~OTMonoLayerAlignment() {}


StatusCode OTMonoLayerAlignment::initialize() {

  info() << "intialize()" << endreq ;

  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_moduledata = new Al::OTMonoLayerAlignData() ;
  IDataProviderSvc* datasvc = svc<IDataProviderSvc>("DetectorDataSvc",true) ;
  sc = datasvc->registerObject("OTMonoLayerAlignData",m_moduledata) ;
  
  // Loading OT Geometry from XML
  m_otdet = getDet<DeOTDetector>(DeOTDetectorLocation::Default );
  const size_t numpars[] = {1,2,4,6} ; // number of parameters in an alignment mode
  size_t newsize = numpars[m_mode-1] ;
  for( auto& otmod: m_otdet->modules() ) {
    m_modules[ otmod->elementID().sequentialUniqueModule() ] = otmod ;
   
    std::vector<double> monopars ;
    otmod->getMonoAlignment( monopars ) ;
    // FIXME: it would actually be nice to just set the parameters properly
    if( monopars.size()!=newsize ) {
      monopars.clear() ;
      monopars.resize(newsize,0.0) ;
      otmod->setMonoAlignment(monopars) ;
    }
  }
  
  /// Get pointer to incident service and add listener
  IIncidentSvc* incSvc = svc<IIncidentSvc>("IncidentSvc");
  if (!incSvc) return Error("==> Failed to retrieve IncidentSvc!", StatusCode::FAILURE);
  incSvc->addListener(this, "UpdateConstants");
  
  sc = m_pocatool.retrieve() ;

  return sc ;
}

StatusCode OTMonoLayerAlignment::finalize()
{
  if ( m_updateInFinalize ) update() ;
  return GaudiHistoAlg::finalize();
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode OTMonoLayerAlignment::execute()
{
  const std::string iterprefix = "Iter" + std::to_string(m_iteration) + "/" ;

  // Get tracks. 
  LHCb::Track::Range tracks = get<LHCb::Track::Range>(m_trackContainer);

  size_t nOTHits(0), nOTHitsWithTime(0), nUsedOTHits(0) ;
  for( const auto& track: tracks ) {
    // just a sanity check
    const LHCb::KalmanFitResult* fitresult = dynamic_cast<const LHCb::KalmanFitResult*>(track->fitResult() ) ;
    if( track->fitStatus()==LHCb::Track::Fitted &&
	!(track->checkFlag( LHCb::Track::Invalid )) &&
	track->nDoF() > 0 &&
	!track->nodes().empty() &&
	fitresult ) {

      // collect OT residuals by half module
      std::map<const DeOTModule*, std::vector<const LHCb::FitNode*> > othitsinmodules ;
      for( const auto& node: fitresult->fitNodes() ) {
	if( node->type()==LHCb::Node::HitOnTrack && // skip outliers
	    node->hasMeasurement() &&
	    node->measurement().type() == LHCb::Measurement::OT ) {
	  ++nOTHits ;
	  const DeOTModule* otmod =
	    dynamic_cast<const DeOTModule*>(node->measurement().detectorElement()) ;
	  const LHCb::OTMeasurement* otmeas = dynamic_cast<const LHCb::OTMeasurement*>(&(node->measurement()));
	  if( otmeas && otmeas->driftTimeStrategy() == LHCb::OTMeasurement::FitDistance ) {
	    othitsinmodules[ otmod ].push_back( node ) ;
	    ++nOTHitsWithTime ;
	  }
	}
      }
      m_moduledata->addTrack( track->chi2(), track->nDoF(),!othitsinmodules.empty()  ) ; 

      // now we only deal with modules that have two hits in two different monolayers
      for( const auto& otmod: othitsinmodules ) {

	if( otmod.second.size() >=2 ) {
	  
	  // I was using trackresidualtool but that needs a rewrite
	  // before I can use it. So, let's just assume hits are
	  // 100% correlated, which is certainly close to true for
	  // hits in same monolayer.
	  unsigned int hitsinmono[2] = {0,0} ;
	  for( const auto& node: otmod.second ) {
	    LHCb::OTChannelID id = node->measurement().lhcbID().otID() ;
	    int mono = otmod.first->mono(id.straw()) ;
	    ++(hitsinmono[mono]) ;
	  }
	  
	  // for now take only combinations with exactly two hits. can change this in future
	  if( hitsinmono[0]==1 && hitsinmono[1]==1) {

	    unsigned int modindex = otmod.first->elementID().sequentialUniqueModule()  ;
	    ModuleData& md = m_moduledata->module( modindex ) ;
	    std::vector<Residual> residuals ;
	    bool first = true ;
	    double weight = 1 ;
	    int theambiguity = 0 ;

	    for( const auto& node: otmod.second ) {
	      // Unbiasing is really not needed (see my TRT paper): the
	      // V^-1 R V^-2 is exactly the unbiasing.
	      Residual res ;
	      res.r   = node->residual() ;
	      res.V   = node->errMeasure2() ;
	      res.HCH = node->errMeasure2()-node->errResidual2() ;
	    
	      // now compute the derivative
	      const LHCb::OTWireTraj* traj = dynamic_cast<const LHCb::OTWireTraj*>(&(node->measurement().trajectory())) ;
	      
	      const LHCb::State& state = node->state() ;
	      // FIXME: we should actually use the coordinate found by poca. unfortunately, that is not cached in the node!
	      //double beta = (state.y() - traj->beginPoint().y() ) /
	      //(traj->endPoint().y() - traj->beginPoint().y() ) ;

	      // checked this: makes a very tiny difference for stereolayers
	      //{
	      // 	Gaudi::XYZVector bfield(0,0,0) ;
	      // 	LHCb::StateVector statevector(state.stateVector(),state.z()) ;
	      // 	const LHCb::StateZTraj refTraj( statevector, Gaudi::XYZVector() );
	      // 	double muState = statevector.z() ;
	      // 	double muMeas = traj->muEstimate( statevector.position() ) ;
	      // 	// Determine the actual minimum with the Poca tool
	      // 	Gaudi::XYZVector dist ;
	      // 	StatusCode sc = m_pocatool->minimize( refTraj, muState,*traj, muMeas, dist, 0.0005 );
	      // 	std::cout << "mu: " << state.y() << " " << muMeas << std::endl ;
	      // }

	      // assume three segments!
	      unsigned int trajindex ;
	      double betaprime ;
	      traj->indexAndMu(state.y(),trajindex,betaprime) ;
	      if( traj->numSegments()==1 ) {
		// need to do as if this is a three-segment piece!
		const int N = 3 ;
		double frac = N*betaprime ;
		const int tmpindex = int(frac) ;
		trajindex = tmpindex<=0 ? 0 : (tmpindex>=N ? N-1 : tmpindex ) ;
		betaprime = frac - trajindex ;
	      }

	      // we only need this for plotting:
	      double beta = (trajindex + betaprime)/traj->numSegments() ;
	      
	      // {
	      // 	const int N = 3 ;
	      // 	LHCb::Trajectory::Range r = traj->range() ;
	      // 	const double frac = N*(state.y() - r.first) / (r.second - r.first) ;
	      // 	const int tmpindex = int(frac) ;
	      // 	unsigned int tmptrajindex = tmpindex<=0 ? 0 : (tmpindex>=N ? N-1 : tmpindex ) ;
	      // 	double tmpbetaprime = frac - tmptrajindex ;
	      // 	std::cout << "Beta etc: " << traj->numSegments() << " "
	      //  		  << trajindex << " " << betaprime << " "
	      //  		  << tmptrajindex << " " << tmpbetaprime << std::endl ;
	      // }

	      // I hope I got this right :-)
	      int monosign = otmod.first->mono( node->measurement().lhcbID().otID().straw() ) ==0 ? +1 : -1 ;
	      // we can actually get this right by rotating the
	      // pocavector to the module frame. difference is
	      // extremely small.
	      Gaudi::XYZVector pocaVectorLocal = otmod.first->geometry()->toLocal( node->pocaVector() ) ;
	      double drdx  = pocaVectorLocal.x() ;

	      res.derivative(2*trajindex,0)   = 0.5 * monosign * drdx * (1-betaprime) ;
	      res.derivative(2*trajindex+1,0) = 0.5 * monosign * drdx * betaprime ;
	      residuals.push_back( res ) ;

	      const LHCb::OTMeasurement* otmeas = dynamic_cast<const LHCb::OTMeasurement*>(&(node->measurement()));
	      const int ambiguitysign = otmeas->ambiguity() ;	      
	      
	      if( first ) {
		first = false ;
		theambiguity = ambiguitysign * monosign > 0 ? 0 : 1 ;
		weight = 1 ;
		if( m_useweighting ) {
		  double lambda = 0.19 ; // average slope of beta distribution
		  if( md.numhits()>10 ) {
		    // estimate lambda from the average.
		    double av = std::min(0.49,md.avbeta()) ;
		    lambda = av * (1-1.59*av)/(1-2*av) ;
		  }
		  weight = std::exp( - (1-beta) / lambda ) ;
		}
		md.add( node->state().y(), beta, theambiguity, weight) ;
		// temporary: plot the distribution of the tangent of the polar angle of the position
		plot( std::sqrt(state.x()*state.x()+state.y()*state.y())/state.z(), 
		      iterprefix + "tantheta","tantheta",0.0,0.3 ) ;
		plot( beta, iterprefix + "beta","beta",0.0,1.0) ;
		plot1D( beta, iterprefix + "wbeta",0.0,1.0,100,weight ) ;
	      }
	      
	      // fill residual histograms for every quarter. maybe we should cache the histogram somewhere.
	      char modulename[245] ;
	      sprintf(modulename,"OT%dL%dQ%dM%d",
		      otmod.first->stationID(), otmod.first->layerID(), otmod.first->quarterID(), otmod.first->moduleID()) ;
	      char quartername[256] ;
	      sprintf(quartername,"OT%dL%dQ%d",
		      otmod.first->stationID(), otmod.first->layerID(), otmod.first->quarterID()) ;
	      plot( res.r,iterprefix + quartername + "_res",quartername,-1.0,1.0) ;
	      plot( res.r/std::sqrt(node->errResidual2()),iterprefix + quartername + "_respull",quartername,-4.0,4.0) ;
	      plot( res.r*ambiguitysign,iterprefix + quartername + "_signedres",quartername,-1.0,1.0) ;
	      profile1D( beta, std::abs(res.r), iterprefix + quartername + "_absresvsbeta",quartername,0.0,1.0,100) ;
	      profile1D( beta,res.r*monosign,iterprefix +modulename+"_res",modulename,0.0,1.0,100) ;
	      plot2D( beta,res.r,iterprefix +"resvsbeta","resvsbeta",0.0,1.0,-1.0,1.0) ;
	      profile1D( modindex, monosign*ambiguitysign, iterprefix + "ambiguity","ambiguity",-0.5,NumUniqueModule-0.5) ;
	    }
	    
	    // now we need for every hit:
	    // - residual
	    // - covariance between residuals
	    // - derivative of resdual to monolayer alignments
	    // correlations is easy if we have two hits. but what if we have more? 
	    // can we do something about the trackresidualtool such that we can use it?
	    // for now, let's just take the 'best' hit in each monolayer and do the proper thing later.
	    
	    // now update the chi2 derivatives.  for a reason that
	    // I now cannot really grasp anymore I store the
	    // elements of HCH as a list, not really a matrix. (It
	    // seems just totally inefficient to me now.) It makes
	    // the following a bit cumbersome. The AlResiduals
	    // object should just be rewritten at some point, but
	    // not today.

	    ModuleData::Parameters halfdchi2dalpha ;
	    ModuleData::Covariance halfd2chi2dalpha2 ;
	      
	    // first diagonal elements
	    for (const auto& ires: residuals ) {
	      ++nUsedOTHits ;
	      // add to first derivative. if we ignore corelations between module, the following is incorrect
	      //halfdchi2dalpha += 1/ires.V * ires.r * ires.derivative ;
	      // while this one does not yet take care of offdiagonal elements of R. we fix that below:
	      double c = 1/ires.V * (ires.V - ires.HCH) * 1/ires.V  ;
	      halfdchi2dalpha += c * ires.r * ires.derivative ;
	      
	      // add V^{-1} ( V - HCH ) V^{-1} to the 2nd derivative
	      ModuleData::Covariance  tmpsym ;
	      ROOT::Math::AssignSym::Evaluate(tmpsym, ires.derivative * ROOT::Math::Transpose(ires.derivative) ) ;
	      halfd2chi2dalpha2 += c * tmpsym ;
	    }

	    // now the off-diagonal elements
	    
	    for( size_t ihit = 0; ihit != residuals.size(); ++ihit) 
	      for( size_t jhit = 0; jhit < ihit; ++jhit ) {
		const ModuleData::Parameters& ideriv = residuals[ihit].derivative ;
		const ModuleData::Parameters& jderiv = residuals[jhit].derivative ;
		// assume 100% correlation to compute HCH
		double HCH_ij = std::sqrt( residuals[ihit].HCH * residuals[jhit].HCH ) ;
		double c = HCH_ij / (residuals[ihit].V *residuals[jhit].V) ;
	      
		// make sure to symmetrize: add off-diagonal elements in both orders.
		Gaudi::Matrix6x6 tmpasym = ideriv * ROOT::Math::Transpose(jderiv) ;
		ModuleData::Covariance tmpsym ;
		ROOT::Math::AssignSym::Evaluate(tmpsym, tmpasym + ROOT::Math::Transpose(tmpasym)) ;
		halfd2chi2dalpha2 -= c * tmpsym ;

		// the contribution to first derivative! this is entirely missing in a global alignment
		halfdchi2dalpha -= ideriv * c * residuals[jhit].r ;
		halfdchi2dalpha -= jderiv * c * residuals[ihit].r ;
	      }

	    md.halfdchi2dalpha(theambiguity)   += weight * halfdchi2dalpha ;
	    md.halfd2chi2dalpha2(theambiguity) += weight * halfd2chi2dalpha2 ;
	    md.halfdchi2dalphaunweighted() += halfdchi2dalpha ;
	  }
	}
      }
    }
  }

  m_moduledata->addEvent( nOTHits, nOTHitsWithTime, nUsedOTHits ) ;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Update
//=============================================================================

void OTMonoLayerAlignment::update()
{
  info() << "Calling OTMonoLayerAlignment::update()" << endreq ;
  if( m_moduledata->nEvents()==0 ) {
    info() << "No events in module data. Skipping update." << endreq ;
    return ;
  }
  std::cout << "Number of events, tracks, tracks with ot hits, othits, othitswithtime, usedothits: "
	    << m_moduledata->nEvents() << ", "
	    << m_moduledata->nTracks() << ", " 
	    << m_moduledata->nTracksWithOTHits() << ", " 
	    << m_moduledata->nOTHits() << ", "
	    << m_moduledata->nOTHitsWithTime() << ", "
	    << m_moduledata->nUsedOTHits() << std::endl ;
  std::string prefix = "Iter" + std::to_string(m_iteration) + "/" ;
 
  Counters stats ;
  const int nmod = m_modules.size() ;
  for(size_t imod = 0; imod < m_modules.size() ; ++imod ) {

    // compute it
    const ModuleData& moddata = m_moduledata->module(imod) ;
    const DeOTModule* module  = m_modules[imod] ;

    std::cout << "*****************************************************" << std::endl
	      << module->name() << std::endl
	      << "numhitsL, numhitsR, sumw, avbeta, avwbeta, avy: " 
	      << moddata.numhits(0) << " " 
	      << moddata.numhits(1) << " " 
	      << moddata.sumw() << " " 
	      << moddata.avbeta() << " " 
	      << moddata.avwbeta() << " " 
	      << moddata.avy() << std::endl ;

    stats.nhits += moddata.numhits() ;
    if(  moddata.numhits() > 100 ) {

      // depending on the mode, reduce the number of degrees of freedom 
      // SHIT! will need to template everything below on size of matrix!
      if( m_mode == Translations ) {
	ROOT::Math::SMatrix<double,1,6> projection ;
	for( auto i=0; i<6; ++i) projection(0,i) = 1 ;
	updateModule( *module, moddata, stats, prefix, projection ) ;
      } else if(m_mode == TranslationsAndRotations ) {
	ROOT::Math::SMatrix<double,2,6> projection ;
	const double f[] = { 0, 1/3., 1/3., 2/3., 2/3., 1} ;
	for(int i=0; i<6; ++i) {
	  projection(0,i) = 1 - f[i] ;
	  projection(1,i) = f[i] ;
	}
	updateModule( *module, moddata, stats, prefix, projection ) ;
      } else if(m_mode == Segments ) {
	ROOT::Math::SMatrix<double,4,6> projection ;
	projection(0,0) = 1 ;
	projection(1,1) = 1 ;
	projection(1,2) = 1 ;
	projection(2,3) = 1 ;
	projection(2,4) = 1 ;
	projection(3,5) = 1 ;
	updateModule( *module, moddata, stats, prefix, projection ) ;
      } else if(m_mode == DiscontinuousSegments ) {
	ROOT::Math::SMatrix<double,6,6> projection ;
	for(int i=0; i<6; ++i) {
	  projection(i,i) = 1 ;
	}
	updateModule( *module, moddata, stats, prefix, projection ) ;
      } else {
	error() << "Don't recognize mode: " << m_mode << endreq ;
      }
    } else {
      std::cout << "Not enough hits." << std::endl ;
      // make sure to still set the condition: otherwise it will not write anything in conddb
      std::vector<double> parameters(1,0) ;
      module->getMonoAlignment( parameters ) ;
      const_cast<DeOTModule*>(module)->setMonoAlignment( parameters ) ;
    }
    plot(moddata.avbeta(),prefix+"avbeta","avbeta",0.,1.,100) ;
    plot(moddata.avy(),prefix+"avy","avy",-2000.,2000.,100) ;
    profile1D(imod,moddata.avbeta(),prefix+"avbetavsmod","beta vs module",-0.5,-0.5+nmod,nmod) ;
    profile1D(imod,moddata.avy(),prefix+"avyvsmod","y vs module",-0.5,-0.5+nmod,nmod) ;	
  }

  std::cout << "Total chi2 of update: " << stats.totalchi2 << std::endl ;
  std::cout << "Num converged/nonconverged: "
	    << stats.nconverged << " " << stats.nnonconverged << std::endl ;
  std::cout << "Num hits in modules: " << stats.nhits << std::endl ;
}

template<unsigned int N>
void OTMonoLayerAlignment::updateModule( const DeOTModule& module,
					 const ModuleData& moddata, 
					 Counters& stats, const std::string& prefix,
					 const ROOT::Math::SMatrix<double,N,6>& projection) const
{
  typedef ROOT::Math::SMatrix<double, N, N,
                              ROOT::Math::MatRepSym<double,N> > Covariance ;
  typedef ROOT::Math::SMatrix<double,N,1> Parameters ;

  // A bias in the t0 should not affect the mono layer alignment,
  // provided that we have as many tracks going to the left as to the
  // right of the wire. Unfortunately, this is not entirely the
  // case. To fix this, we split the derivatives in 'Left' and 'Right'
  // (counted form the ambiguity in the first monolayer), then
  // recombine such that we end up more or less with a straight
  // average. The reason we don't compute separate solutions is that
  // we don't want to add the survey twice ... Admittedly, the
  // following is not yet entirely satisfactory.
  
  double f0 = moddata.numhits(1) < moddata.numhits(0) ? moddata.numhits(1) / double(moddata.numhits(0)) : 1.0 ;
  double f1 = moddata.numhits(1) > moddata.numhits(0) ? moddata.numhits(0) / double(moddata.numhits(1)) : 1.0 ;
  Al::OTMonoLayerAlignModuleData::Parameters halfdchi2dalphafull = 
    f0 * moddata.halfdchi2dalpha(0) + f1 * moddata.halfdchi2dalpha(1)  ;
  Al::OTMonoLayerAlignModuleData::Covariance halfd2chi2dalpha2full = 
    f0 * moddata.halfd2chi2dalpha2(0) + f1 * moddata.halfd2chi2dalpha2(1)  ;
  
  Parameters halfdchi2dalpha   = projection * halfdchi2dalphafull ;
  Covariance halfd2chi2dalpha2 = ROOT::Math::Similarity(projection,halfd2chi2dalpha2full) ;
  
  // add 'survey' information: constrain to 'no alignment'
  std::vector<double> parameters ;
  module.getMonoAlignment( parameters ) ;
  if( parameters.size() != N ) {
    error() << "Size mismatch: fix in initialization! " 
	    << parameters.size() << " " << N << endreq ;
    return ;
  }
  // 
  for( unsigned int i=0; i<N; ++i) {
    // need to choose an error for the survey. RMS of delta's is about
    // 240 micron.
    double surveyerror2 = m_surveysigma*m_surveysigma ;
    // due to the reweighting, a small survey pulls really hard. we
    // could correct for the average weight ...
    if(m_useweighting) surveyerror2 *= moddata.avw() ;
    halfdchi2dalpha(i,0) += 1/surveyerror2 * parameters.at(i) ;
    halfd2chi2dalpha2(i,i) += 1/surveyerror2 ;    
  }

  Covariance cov = halfd2chi2dalpha2 ;
  int OK = cov.InvertChol() ;
  if(!OK) {
    std::cout << "WARNING: inversion failed!" << halfd2chi2dalpha2 << std::endl ;
  }
  Parameters delta = - cov *  halfdchi2dalpha ;

  double chi2 = -1*(ROOT::Math::Transpose(delta) * projection * moddata.halfdchi2dalphaunweighted())(0,0) ;
  if(chi2<9) {
    ++stats.nconverged ;
  } else {
    ++stats.nnonconverged ; 
  }
  stats.totalchi2 += chi2;
  
  const int nmod = LHCb::OTChannelID::NumSequentialModule ;
  const int imod = module.elementID().sequentialUniqueModule() ;
  
  for(size_t i = 0; i < N ; ++i) {
    std::cout << "current, delta:" << i << ": "
	      << parameters.at(i) << " " << delta(i,0) << " +/- " << std::sqrt( cov(i,i) ) << std::endl ;
    double current = parameters[i] ;
    double total = current + delta(i,0) ;
    plot(total,prefix+"dx"+std::to_string(i),"dx"+std::to_string(i),-1.,1.,100) ;
    plot(total/std::sqrt( cov(i,i) ),prefix+"dxpull"+std::to_string(i),"dxpull"+std::to_string(i),-10.,10.,100) ;
    plot(std::sqrt( cov(i,i) ),prefix+"dxerr"+std::to_string(i),"dxerr"+std::to_string(i),-10.,10.,100) ;
    profile1D(imod,total,prefix+"deltadxvsmod"+std::to_string(i),"delta dx per module"+std::to_string(i),-0.5,-0.5+nmod,nmod) ;
    profile1D(imod,std::sqrt( cov(i,i) ),prefix+"deltaerrvsmod"+std::to_string(i),"delta err per module"+std::to_string(i),-0.5,-0.5+nmod,nmod) ;
  }
      
  // now set the parameters
  for( unsigned int i=0; i<N; ++i)
    parameters[i] += delta(i,0) ;
  const_cast<DeOTModule&>(module).setMonoAlignment( parameters ) ;
}

void OTMonoLayerAlignment::reset() {
  info() << "increasing iteration counter and resetting accumulated data..." << endmsg;
  
  /// increment iteration counter
  ++m_iteration;
  
  // clear derivatives and counters
  m_moduledata->reset() ;
}

void OTMonoLayerAlignment::handle(const Incident& incident)
{
  if ("UpdateConstants" == incident.type()) {
    update();
    reset();
  }
}
