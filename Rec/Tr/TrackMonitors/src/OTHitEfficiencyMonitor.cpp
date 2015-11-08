#include "GaudiKernel/ToolHandle.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "Event/Track.h"
#include "Event/FitNode.h"
#include "Event/OTMeasurement.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/ITrackInterpolator.h"
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTModule.h"
#include "DetDesc/StaticArray.h"
#include "OTDAQ/IOTRawBankDecoder.h"
#include "AIDA/IProfile1D.h"
#include "AIDA/IProfile2D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram1D.h"


/*
  Author: Wouter Hulsbergen (wouterh@nikhef.nl)
  Modified by Francesco Dettori (Francesco.Dettori@cern.ch)

*/


namespace {

  struct LayerEfficiencyHistograms
  {
    AIDA::IProfile2D* m_2DHitEfficiency ;
    AIDA::IProfile2D* m_2DHotEfficiency ;
    //AIDA::IHistogram2D* m_2DHitOccupancy;
    AIDA::IHistogram2D* m_2DExpOccupancy;
    //AIDA::IHistogram2D* m_2DHotOccupancy;

    LayerEfficiencyHistograms(GaudiHistoAlg& alg,  size_t LayerID , bool isOnline) {
      // construct a directory name
      std::string Layername = std::string("Layer") + std::to_string( LayerID ) + "/" ;
      if(!isOnline){
        m_2DHitEfficiency = alg.bookProfile2D( Layername + "2DHitEfficiency", "2DHitEfficiency",
                                               -3157.0, 3157.0, 74, -2800,2800,100);

        m_2DHotEfficiency = alg.bookProfile2D( Layername + "2DHotEfficiency", "2DHotEfficiency",
                                               -3157.0, 3157.0, 74, -2800,2800,100);

        //m_2DHitOccupancy = alg.book2D( Layername + "2DHitOccupancy", "2D Hit Occupancy",
        //-3157.0, 3157.0, 74, -2800,2800,100);
        m_2DExpOccupancy = alg.book2D( Layername + "2DExpOccupancy", "2D Exp Occupancy",
                                       -3157.0, 3157.0, 74, -2800,2800,100);
        //m_2DHotOccupancy = alg.book2D( Layername + "2DHotOccupancy", "2D Hot Occupancy",
        // -3157.0, 3157.0, 74, -2800,2800,100);

      } else {
        m_2DHitEfficiency = 0;
        m_2DHotEfficiency =0;
        m_2DExpOccupancy = 0;
      }
    }

  };

  struct ModuleEfficiencyHistograms
  {
    AIDA::IProfile1D* m_effvsmonocoord ;
    AIDA::IProfile1D* m_effvsdist ;
    AIDA::IProfile1D* m_receffvsmonocoord ;
    AIDA::IProfile1D* m_effvsyfrac ;
    AIDA::IProfile1D* m_receffvsyfrac ;
    AIDA::IHistogram2D* m_clustersize ;
    AIDA::IHistogram1D* m_deltatdc ;

    ModuleEfficiencyHistograms(GaudiHistoAlg& alg,  size_t moduleID, bool isOnline ) {
      // construct a directory name
      std::string modulename = std::string("Module") + std::to_string( moduleID ) + "/" ;
      if(!isOnline){
        m_effvsdist     = alg.bookProfile1D( modulename + "effvsdist",
                                             "hit efficiency versus distance", -7.5, 7.5, 200) ;
        m_effvsmonocoord = alg.bookProfile1D( modulename + "effvsmonocoord",
                                              "hit efficiency versus coordinate in mono layer plane",
                                              -3, 3, 200) ;
        m_effvsyfrac    = alg.bookProfile1D( modulename + "effvsy",
                                             "hit efficiency versus y-coordinate in mono layer plane",
                                             -0.1, 1.1, 120) ;
        m_receffvsmonocoord = alg.bookProfile1D( modulename + "hoteffvsmonocoord",
                                                 "hot efficiency versus coordinate in mono layer plane",
                                                 -3, 3, 200) ;
        m_receffvsyfrac    = alg.bookProfile1D( modulename + "hoteffvsy",
                                                "hot efficiency versus y-coordinate in mono layer plane",
                                                -0.1, 1.1, 120) ;

        m_clustersize = alg.book2D( modulename + "clustersize",
                                    "clustersize versus slope", -1,1,20,-0.5,10.5,11) ;
        m_deltatdc = alg.book1D( modulename + "deltatdc",
                                 "tdc_neighbour - tdc_central for 2-cell clusters",-128,128,256) ;
      } else {
        m_effvsdist     = alg.bookProfile1D( modulename + "effvsdist",
                                             "hit efficiency versus distance", -7.5, 7.5, 200) ;
        m_effvsmonocoord = 0;
        m_receffvsmonocoord =0;
        m_effvsyfrac=0;
        m_receffvsyfrac=0;
        m_clustersize=0;
        m_deltatdc=0;
      }
    }
  } ;

  struct ModuleHitMap
  {
    enum { MaxChan = 128} ;
    LHCb::OTChannelID hashit[MaxChan] ;
  } ;

}


class OTHitEfficiencyMonitor : public GaudiHistoAlg
{
public:

  /** Standard construtor */
  OTHitEfficiencyMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  /** Destructor */
  virtual ~OTHitEfficiencyMonitor();

  /** Algorithm initialize */
  virtual StatusCode initialize();

  /** Algorithm finalize */
  virtual StatusCode finalize();

  /** Algorithm execute */
  virtual StatusCode execute();
private:
  void setNormalization(const std::string& hisname)  ;
  // this should be moved to OTChannelID
  enum {NumUniqueStation=3, NumUniqueLayer=12, NumUniqueModule=432, NumUniqueOtis=432*4  } ;

  inline int uniqueLayer( const LHCb::OTChannelID& channelID) {
    return  (channelID.station()-1) * 4 + channelID.layer() ;
  }
  inline int uniqueQuarter( const LHCb::OTChannelID& channelID) {
    return uniqueLayer( channelID) * 4 + channelID.quarter() ;
  }
  inline int uniqueModule( const LHCb::OTChannelID& channelID) {
    return uniqueQuarter(channelID)*9 + channelID.module() - 1 ;
  }
  inline int uniqueOtis( const LHCb::OTChannelID& channelID) {
    return uniqueModule(channelID)*4 + (channelID.straw()-1)/32 ;
  }

  void fillEfficiency(const LHCb::Track& track, const DeOTModule& module,
                      double strawpos[2], double yFrac[2], const LHCb::State& refstate ) ;
  void fillEfficiency(const LHCb::Track& track, const DeOTModule& module, const LHCb::State& state) ;
  void fillEfficiency(const LHCb::Track& track, const DeOTModule& module,
                      const LHCb::FitNode* nodeA,const LHCb::FitNode* nodeB) ;
  void fillEfficiency(const LHCb::Track& track) ;
  void plotHist1D(AIDA::IHistogram1D* hist, double value, double weight);
  void plotHist2D(AIDA::IHistogram2D* hist, double x, double y, double weight);
  void plotProf1D(AIDA::IProfile1D* hist, double x, double y, double weight);
  void plotProf2D(AIDA::IProfile2D* hist, double x, double y, double z, double weight);
private:
  ToolHandle<IOTRawBankDecoder> m_decoder ;
  ToolHandle<ITrackInterpolator> m_interpolator ;
  ToolHandle<ITrackExtrapolator> m_linearextrapolator ;
  const DeOTDetector* m_otdet ;
  std::string m_trackLocation;
  size_t m_minOTHitsPerTrack ;
  double m_maxChi2PerDoF ;
  double m_maxDistError ;
  bool m_isOnline;
  size_t m_maxTracksPerEvent;
  // event data. we'll keep it such that we don't have to move it around.
  ModuleHitMap m_moduleHitMap[NumUniqueModule] ;

  size_t m_nTrackOTHits; // number of OT hits in a given track, useful to keep in mind

  // booked histograms
  AIDA::IProfile1D* m_efficiencyPerModulePr ;
  AIDA::IProfile1D* m_efficiencyPerOtisPr ;
  std::vector<ModuleEfficiencyHistograms*> m_moduleHistograms ;
  std::vector<LayerEfficiencyHistograms*> m_layerHistograms ;


} ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OTHitEfficiencyMonitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTHitEfficiencyMonitor::OTHitEfficiencyMonitor( const std::string& name,
                                                ISvcLocator* pSvcLocator)
  : GaudiHistoAlg( name , pSvcLocator ),
    m_decoder("OTRawBankDecoder"),
    m_interpolator("TrackInterpolator"),
    m_linearextrapolator("TrackLinearExtrapolator")
{
  declareProperty( "TrackLocation", m_trackLocation = LHCb::TrackLocation::Default  );
  declareProperty( "MinOTHits",m_minOTHitsPerTrack = 21 ) ;
  declareProperty( "MaxChi2PerDoF",m_maxChi2PerDoF = 2  ) ;
  declareProperty( "MaxDistError", m_maxDistError = 0.2 ) ;
  declareProperty( "RawBankDecoder",m_decoder ) ;
  declareProperty( "Online", m_isOnline = false);
  declareProperty( "MaxTracks", m_maxTracksPerEvent = 1E6 /* ALL */);

}

//=============================================================================
// Destructor
//=============================================================================
OTHitEfficiencyMonitor::~OTHitEfficiencyMonitor()
{
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode OTHitEfficiencyMonitor::initialize()
{
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if(sc.isFailure()) return sc ;
  sc = m_decoder.retrieve() ;
  if(sc.isFailure()) return sc ;
  sc = m_interpolator.retrieve() ;
  if(sc.isFailure()) return sc ;
  sc = m_linearextrapolator.retrieve() ;
  if(sc.isFailure()) return sc ;

  m_otdet = getDet<DeOTDetector>(DeOTDetectorLocation::Default );

  // book all histograms
  setHistoTopDir("OT/") ;
  m_efficiencyPerModulePr = bookProfile1D( "efficiencyVsModule","efficiency per module",
                                           -0.5,NumUniqueModule-0.5,NumUniqueModule) ;
  if(!m_isOnline)
    m_efficiencyPerOtisPr = bookProfile1D( "efficiencyVsOtis","efficiency per otis",
                                           -0.5,NumUniqueOtis-0.5,NumUniqueOtis) ;
  else
    m_efficiencyPerOtisPr = NULL;
  m_moduleHistograms.reserve(9) ;
  m_layerHistograms.reserve(12) ;

  for(int i=0; i<9; ++i)
    m_moduleHistograms.push_back( new ModuleEfficiencyHistograms(*this,i+1, m_isOnline) ) ;

  for(int i=0; i<12; ++i)
    m_layerHistograms.push_back( new LayerEfficiencyHistograms(*this, i, m_isOnline) );

  m_nTrackOTHits = 0;


  if(msgLevel(MSG::DEBUG)) debug() << " Minimum number of OT Hits per track " <<  m_minOTHitsPerTrack << endmsg;
  return sc;
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode OTHitEfficiencyMonitor::finalize()
{
  // here we should create some 'summary' histograms

  // the delete the structure that holds the histograms
  for ( ModuleEfficiencyHistograms* hist: m_moduleHistograms ) delete hist ;
  for ( LayerEfficiencyHistograms* hist: m_layerHistograms ) delete hist ;


  // rescale the occupancy histogram
  m_decoder.release().ignore() ;
  m_interpolator.release().ignore() ;
  m_linearextrapolator.release().ignore() ;
  return GaudiHistoAlg::finalize() ;
}


void OTHitEfficiencyMonitor::plotHist1D(AIDA::IHistogram1D* hist, double value, double weight)
{
  if(hist)
    fill(hist, value, weight);
}
void OTHitEfficiencyMonitor::plotHist2D(AIDA::IHistogram2D* hist, double x, double y, double weight)
{
  if(hist)
    fill(hist, x, y, weight);
}


void OTHitEfficiencyMonitor::plotProf1D(AIDA::IProfile1D* hist, double x, double y, double weight)
{
  if(hist)
    fill(hist, x, y, weight);
}
void OTHitEfficiencyMonitor::plotProf2D(AIDA::IProfile2D* hist, double x,
                                        double y, double z, double weight)
{
  if(hist)
    fill(hist, x, y, z, weight);
}

//=========================================================================
//
//=========================================================================
StatusCode OTHitEfficiencyMonitor::execute()
{

  // first clear the hit map
  for(size_t imod=0; imod<NumUniqueModule; ++imod)
    for(size_t istraw=0; istraw<ModuleHitMap::MaxChan; ++istraw)
      m_moduleHitMap[imod].hashit[istraw] = 0 ;

  // decode all OT hits
  LHCb::OTLiteTimeContainer ottimes;
  StatusCode sc = m_decoder->decode( ottimes ) ;

  if( !sc ) return sc;
  // fill all the hits in the hit map
  for( const auto& hit : ottimes )
    m_moduleHitMap[ uniqueModule( hit.channel() ) ].hashit[  hit.channel().straw()-1 ] = hit.channel() ;

  // now loop over the tracks
  auto tracks = get<LHCb::Track::Range>(m_trackLocation) ;
  if(tracks.size()<m_maxTracksPerEvent){
    for(const LHCb::Track* track: tracks) {
      if( track->fitStatus() == LHCb::Track::Fitted && track->nDoF()>1 &&
          track->chi2PerDoF() < m_maxChi2PerDoF ) {
        // count the number
        //size_t sumn(0) ;
        m_nTrackOTHits = 0;
        for( const auto& node :track->nodes() ) {
          if( node->type() == LHCb::Node::HitOnTrack
              && node->measurement().type() == LHCb::Measurement::OT ) ++m_nTrackOTHits ;
          if( m_nTrackOTHits >= m_minOTHitsPerTrack ) fillEfficiency(*track) ;
        }
      }
    }
  }
  return sc ;
}

void OTHitEfficiencyMonitor::fillEfficiency( const LHCb::Track& track,
                                             const DeOTModule& module,
                                             double strawpos[2], double yfrac[2],
                                             const LHCb::State& refstate)
{
  // make a temporary structure that tells which hits are in this
  // module. if this takes time, then we should do it once per event
  LHCb::OTChannelID modid = module.elementID() ;

  double pitch = module.xPitch() ;
  int uniquemodule  = uniqueModule(modid) ;

  size_t nstraws = module.nChannels() ;

  ModuleEfficiencyHistograms* modulehist = m_moduleHistograms[module.elementID().module()-1] ;
  LayerEfficiencyHistograms* layerhist = m_layerHistograms[module.elementID().uniqueLayer()-4] ;


  // compute the direction in the local frame. this can be done a lot more efficient, but okay:
  Gaudi::XYZVector localslopes = module.geometry()->toLocal( refstate.slopes() ) ;
  double localTx  = localslopes.x()/localslopes.z() ;
  double cosalpha = 1/std::sqrt( 1+localTx*localTx) ;

  for(size_t imono = 0; imono<2 ; ++imono ) {
    // find the hits in a +- 3 straw window
    int monooffset = imono * nstraws/2 - 1 ;

    int lower_straw_dist = 2;
    int upper_straw_dist = 3;

    int minstraw = std::max(int(strawpos[imono]) - lower_straw_dist,1) ;
    int maxstraw = std::min(int(strawpos[imono]) + upper_straw_dist,int(nstraws/2)) ;

    // State precision (put here not to bias only layers not already on track)
    if( std::sqrt( refstate.covariance()(0,0) ) > m_maxDistError ) return;

    for( int istraw = minstraw; istraw<=maxstraw; ++istraw) {
      LHCb::OTChannelID channel = m_moduleHitMap[ uniquemodule ].hashit[ istraw + monooffset ] ;
      bool foundhit = channel != 0 ;
      bool foundhot = foundhit && track.isOnTrack( LHCb::LHCbID( channel ) ) ;
      double monocoord = strawpos[imono] - istraw ;
      double dstraw    = monocoord*pitch*cosalpha  ;

      // Efficiency unbiasing procedure
      // Ask one more hit if a hit is found to be independent from the original request
      if(foundhit){
        if( m_nTrackOTHits < m_minOTHitsPerTrack + 1 ) continue;
      }

      //
      plotProf1D(modulehist->m_effvsdist, dstraw,foundhit, 1.0) ;
      plotProf1D(modulehist->m_effvsmonocoord, monocoord,foundhit, 1.0) ;
      plotProf1D(modulehist->m_receffvsmonocoord, monocoord,foundhot, 1.0) ;
      if( std::abs(dstraw) < 1.25 ) {
        plotProf1D(modulehist->m_effvsyfrac, yfrac[imono],foundhit, 1.0) ;
        plotProf1D(modulehist->m_receffvsyfrac, yfrac[imono],foundhot, 1.0) ;
        plotProf1D(m_efficiencyPerModulePr, uniquemodule, foundhit, 1.0 ) ;	
        plotProf1D(m_efficiencyPerOtisPr, uniquemodule*4 + (istraw+monooffset-1)/32, foundhit, 1.0 ) ;
	
        //Filling the 2D expected occupancy, hiteff and hoteff
        plotHist2D(layerhist->m_2DExpOccupancy, refstate.x(),refstate.y(), 1.0);
        plotProf2D(layerhist->m_2DHitEfficiency, refstate.x(),refstate.y(),foundhit, 1.0);
        plotProf2D(layerhist->m_2DHotEfficiency, refstate.x(),refstate.y(),foundhot, 1.0);
	
        //Filling a 2D histo with the multiplicity of the hits
        //if (foundhit){
        //layerhist->m_2DHitOccupancy->fill(refstate.x(),refstate.y());
        //}
	
        //if(foundhot){
        //layerhist->m_2DHotOccupancy->fill(refstate.x(),refstate.y());
        //}
	
      }
    }
    // monitor 'cluster size'. cluster size is zero if there is no hit in 'closest straw'.
    int closeststraw = int( strawpos[imono] + 0.5 ) ;
    if( 1<=closeststraw && closeststraw <=int(nstraws/2) ) {
      StaticArray<LHCb::OTChannelID,128> clusterhits ;
      LHCb::OTChannelID tmp ;
      for( int istraw = closeststraw;
           istraw<=int(nstraws/2) && (tmp = m_moduleHitMap[ uniquemodule ].hashit[ istraw + monooffset ]) ;
           ++istraw) clusterhits.push_back( tmp ) ;
      for( int istraw = closeststraw-1;
           istraw>=1 && (tmp = m_moduleHitMap[ uniquemodule ].hashit[ istraw + monooffset ]) ;
           --istraw) clusterhits.push_back( tmp ) ;
      plotHist2D(modulehist->m_clustersize, localTx, clusterhits.size(), 1.0 ) ;
      if( clusterhits.size() ==2 )
        plotHist1D(modulehist->m_deltatdc, double(clusterhits[1].tdcTime()) - double(clusterhits[0].tdcTime()), 1.0 ) ;
    }
  }
}

void OTHitEfficiencyMonitor::fillEfficiency(const LHCb::Track& track,
                                            const DeOTModule& module,
                                            const LHCb::State& state )
{
  double strawpos[2], yfrac[2] ;
  for(size_t imono = 0 ; imono<2; ++imono)
    module.monoLayerIntersection( imono,
                                  state.position(),
                                  state.tx(),
                                  state.ty(),
                                  strawpos[imono], yfrac[imono] ) ;
  fillEfficiency(track,module,strawpos,yfrac,state) ;
}

void OTHitEfficiencyMonitor::fillEfficiency(const LHCb::Track& track,
                                            const DeOTModule& module,
                                            const LHCb::FitNode* nodeMonoA,
                                            const LHCb::FitNode* nodeMonoB)
{
  double strawpos[2], yfrac[2] ;
  const LHCb::State* refstate(0) ;
#define SAFEBUTSLOW
#ifdef SAFEBUTSLOW
  // this is the expensive method of getting two unbiased straw positions:
  LHCb::State states[2] ;
  states[0] = nodeMonoA ? nodeMonoA->unbiasedState() : nodeMonoB->state() ;
  states[1] = nodeMonoB ? nodeMonoB->unbiasedState() : nodeMonoA->state() ;
  for(size_t imono = 0 ; imono<2; ++imono)
    module.monoLayerIntersection( imono,
                                  states[imono].position(),
                                  states[imono].tx(),
                                  states[imono].ty(),
                                  strawpos[imono], yfrac[imono] ) ;
  refstate = &(states[1]) ;
#else
  // but there is a second method that is much faster. it is just slightly harder toget things right:
  const LHCb::FitNode* nodes[2]={ 0,0};
  nodes[0] = nodeMonoA ;
  nodes[1] = nodeMonoB ;
  for(size_t imono=0; imono<2; ++imono) {
    const LHCb::FitNode* node = nodes[imono] ;
    const LHCb::State& state = node ? node->state() : nodes[(imono+1)%2]->state() ;
    refstate = &state ;
    module.monoLayerIntersection( imono,
                                  state.position(),
                                  state.tx(),
                                  state.ty(),
                                  strawpos[imono], yfrac[imono] ) ;
    if( node ) { // okay, need to unbias
      const LHCb::OTMeasurement* meas =
        static_cast<const LHCb::OTMeasurement*>(&(node->measurement())) ;
      int thisstraw = meas->channel().straw() % (module.nChannels()/2) ;
      double biaseddoca     = node->doca() ;
      double measurement    = node->residual() + biaseddoca ;
      double unbiaseddoca   = measurement - node->unbiasedResidual() ;
      double biaseddstraw   = strawpos[imono] - thisstraw ;
      double unbiaseddstraw = biaseddstraw * unbiaseddoca / biaseddoca ;
      double unbiasedstraw  = thisstraw + unbiaseddstraw ;
      strawpos[imono] = unbiasedstraw ;
    }
  }
#endif
  fillEfficiency(track,module,strawpos,yfrac,*refstate) ;
}


void OTHitEfficiencyMonitor::fillEfficiency(const LHCb::Track& track)
{
  // first collect all modules that are already on the track. for these we need to unbias.
  typedef std::pair<const LHCb::Node*, const LHCb::Node*> MonoLayerNodes ;
  typedef std::map< const DeOTModule*, MonoLayerNodes > ModulesOnTrack ;
  ModulesOnTrack modulesOnTrack ;

  for( const auto& node : track.nodes() ) {
    if( node->type() == LHCb::Node::HitOnTrack &&
        node->measurement().type() == LHCb::Measurement::OT ) {
      const LHCb::OTMeasurement* meas =
        static_cast<const LHCb::OTMeasurement*>(&node->measurement()) ;
      const DeOTModule* module = &(meas->module()) ;

      // only take the closest node in a mono-layer. we can anyway only unbias 1.
      int mono = (meas->channel().straw()-1) / (module->nChannels()/2) ;
      // Get address of needed node on layer from modules to layers map
      const LHCb::Node*& nodeInModule = mono == 0 ?
        modulesOnTrack[ module ].first :  modulesOnTrack[ module ].second ;
      if( nodeInModule == 0 ||
          std::abs(nodeInModule->residual()) >
          std::abs(node->residual()) ) nodeInModule = node ;

      //modulesOnTrack[ module ].push_back( *inode ) ;
    }
  }
  // process the efficiency for these module. make sure to unbias the
  // state when necessary. keep track of the layers that we have seen,
  // such that we can skip them below
  std::set< const DeOTLayer* > layersOnTrack ;
  for( const auto& mod : modulesOnTrack ) {
    layersOnTrack.insert( m_otdet->findLayer( mod.first->elementID() ) ) ;

    const LHCb::FitNode* nodeMonoA = static_cast<const LHCb::FitNode*>( mod.second.first ) ;
    const LHCb::FitNode* nodeMonoB = static_cast<const LHCb::FitNode*>( mod.second.second ) ;
    fillEfficiency(track, *mod.first, nodeMonoA, nodeMonoB ) ;
  }

  /*This should be moved to initialize and be common to all events... */
  // cache the z-positions of the layers
  static std::vector<double> layerzpos ;
  if( layerzpos.empty() ) {
    for( const auto& lay : m_otdet->layers() )
      layerzpos.push_back(lay->geometry()->toGlobal(Gaudi::XYZPoint(0.0, 0.0, 0.0)).z()) ;
  }

  // now find the modules in the missing planes
  LHCb::State state ;
  int layindex(-1) ;
  std::vector< std::pair< const DeOTModule*, LHCb::State > > modulesNotOnTrack ;
  for( const auto& lay : m_otdet->layers() ) {
    ++layindex;
    if( layersOnTrack.find( lay ) != layersOnTrack.end() ) continue;

    // propagate to the z-coordinate of this layer
    StatusCode sc  = m_interpolator->interpolate( track, layerzpos[layindex] , state );

    // only consider if state has reasonably small error. we should
    // still fine-tune this a little bit, make a propor projecion
    // etc. the best is to cut only when you have unbiased the
    // state. cutting to tight will introduce a significant bias in
    // the efficiency measurement.
    if(sc.isSuccess() && state.covariance()(0,0) < m_maxDistError*m_maxDistError) {
	
      // to get a slightly more precise answer, intersect with the plane
      double tolerance = 0.1 ;
      StatusCode sc_two = m_linearextrapolator->propagate( state, lay->plane(), tolerance);//.ignore() ;
	
      // get a single module. this goes wrong if there are large misalignments!
      const DeOTModule* module = m_otdet->findModule(state.position());
      if(sc_two.isSuccess()){
        if(module ) {
          // fill the efficiency
          fillEfficiency(track, *module, state ) ;	

          // keep track of what we have seen
          modulesNotOnTrack.emplace_back( module, state ) ;
        }
      }else{
        Warning(" Track interpolation or propagation failed ").ignore();

      }
    }
  }
}
