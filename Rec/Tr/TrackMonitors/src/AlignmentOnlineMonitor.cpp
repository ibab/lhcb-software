#include <memory>
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrackInterfaces/ITrackProjector.h"
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/Node.h"
#include "Event/Measurement.h"
#include "AIDA/IProfile1D.h"
#include "AIDA/IHistogram1D.h"
#include "GaudiUtils/Aida2ROOT.h"
#include "DetDesc/3DTransformationFunctions.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "TProfile.h"
// includes from TTTrackMonitor
#include "TrackMonitorBase.h"
#include "Kernel/LHCbID.h"
#include "Kernel/SiChargeFun.h"
#include "Event/FitNode.h"
#include "Event/STMeasurement.h"
#include "Kernel/ITNames.h"
#include "Kernel/TTNames.h"

//tuples in finalise:
#include "GaudiAlg/GaudiTupleAlg.h"
#include <GaudiAlg/Tuple.h>
#include <GaudiAlg/GaudiTuples.h>
using namespace LHCb;
using namespace Gaudi;
using namespace Tuples;
namespace {



  struct ResidualRecorder{
    ResidualRecorder(GaudiHistoAlg& alg, const std::string& dir,
                     size_t numelements, std::string m_names[42])
      : m_dir(dir), m_parent(&alg), m_numelements(numelements) {
      // initialise finalisation histograms
      m_parentMean = alg.book1D("Means","Means",-0.1,0.1,1000);
      m_parentRMS  = alg.book1D("RMS","RMS",0,1,1000);
      m_parentSkewness = alg.book1D("Skewness","Skewness",-5,5,1000);
      m_parentKurtosis = alg.book1D("Kurtosis","Kurtosis",0,10,1000);
      // book histograms
      for( int i=0; i<m_numelements; ++i ){
        //#Book histograms for residuals in the 42 TTLayers: TTaX, ...
        double f = 1.0;
        if(m_dir=="OT"){f=3.0;}
        if(m_dir=="Velo"){f=0.4;}
        m_residual[i] = alg.book1D( m_dir + "/" + m_names[i], "Residual (RMS unbiased) in " + m_names[i] , -0.5*f, 0.5*f, 100) ;
        //if (msgLevel(MSG::DEBUG)) debug() << i <<"th 1D Histo booked: " << dir << "/" << m_names[i] << endmsg ;
      };
    };

    void fill(size_t layer, const LHCb::Node& node) {
      if (node.hasMeasurement()){	
        double residual = node.residual() * std::sqrt(node.errMeasure2()/node.errResidual2()) ;
        m_residual[layer]->fill(residual);
      };
    };

    void fillfinal(){
      //      std::cout<<"========== Moments of Residual Distributions =============="<<std::endl;
      for(int i=0; i<m_numelements; i++){
        if(m_residual[i] != 0 ) {
          TH1D* pr = Gaudi::Utils::Aida2ROOT::aida2root ( m_residual[i] ) ;
          m_parentMean    ->fill( pr && pr->GetEntries() > 0 ? pr->GetMean()     : 0.0 );
          m_parentRMS     ->fill( pr && pr->GetEntries() > 0 ? pr->GetRMS()      : 0.0 );
          m_parentSkewness->fill( pr && pr->GetEntries() > 0 && pr->GetRMS() > 0 ? pr->GetSkewness() : 0.0 );
          m_parentKurtosis->fill( pr && pr->GetEntries() > 0 && pr->GetRMS() > 0 ? pr->GetKurtosis() : 0.0 );
          /*
          std::cout <<pr->GetName()
          << ":: MEAN: "  <<pr->GetMean()    <<" RMS: "     <<pr->GetRMS()
          << " SKEWNESS: "<<pr->GetSkewness()<<" KURTOSIS: "<<pr->GetKurtosis()
          <<std::endl;
          */
        }
      };
      // std::cout<<"==========================================================="<< std::endl;
    };

    ~ResidualRecorder(){
      fillfinal();

    };
    // residual plots
    IHistogram1D* m_residual[42] ;
    // moments for finalise step
    IHistogram1D* m_parentMean;
    IHistogram1D* m_parentRMS;
    IHistogram1D* m_parentSkewness;
    IHistogram1D* m_parentKurtosis;
    // double m_combMean = 0;
    std::string m_dir ;
    GaudiHistoAlg* m_parent ;
    int m_numelements ;
  };
}

class AlignmentOnlineMonitor : public GaudiHistoAlg {

/** @class  AlignmentOnlineMonitor
 *
 *  Produces residual plots to check the alignment in OnlineBrunel
 *
 *  @author Stefan Hallmann
 *  @date   2013-09-04
 */

public:
  /** Standard construtor */
  AlignmentOnlineMonitor( const std::string& name, ISvcLocator* pSvcLocator );
	
  /** Destructor */
  virtual ~AlignmentOnlineMonitor();

  /** Algorithm initialize */
  virtual StatusCode initialize();

  /** Algorithm initialize */
  virtual StatusCode finalize();

  /** Algorithm execute */
  virtual StatusCode execute();

private:
  //enum { NumOT = 432, NumVelo = 42, NumIT=336, NumTT=288 } ;

  //size_t otUniqueID(const LHCb::OTChannelID& id) {

  //	return (((id.station()-1)*4 + id.layer())*4 + id.quarter())*9 + id.module() ;
  //} ;

  size_t otUniqueStation(const LHCb::OTChannelID& id) {
    if (msgLevel(MSG::VERBOSE)) verbose()  <<"OT:" << id.station()<<" "<< id.layer() <<" "<<id.quarter()<<endmsg;
    return (id.station()-1) ;
  } ;

  size_t otUniqueQuarter(const LHCb::OTChannelID& id) {
    return (id.quarter()) ;
  } ;

  size_t otUniqueStationQuarter(const LHCb::OTChannelID& id) {
    return ((id.station()-1)*4 + id.quarter()) ;
  } ;

  //size_t veloUniqueID(const LHCb::VeloChannelID& id) {
  //	unsigned int station = (id.sensor()%64)/2 ;
  //	unsigned int side    = (id.sensor()%2) ;
  //unsigned int type    = (id.sensor()/64) ;
  // if (msgLevel(MSG::VERBOSE)) verbose()   << "velo: " << side << " " << station << endmsg ;
  //	return side * 21 + station ;
  //} ;


  size_t veloUniqueSide(const LHCb::VeloChannelID& id) {
    unsigned int side    = (id.sensor()%2) ;
    if (msgLevel(MSG::VERBOSE)) {
      unsigned int station = (id.sensor()/64) ;
      verbose()   << "velo: " << side << " " << station << endmsg ;
    }
    return side;
  } ;

  size_t veloUniqueSideStation(const LHCb::VeloChannelID& id) {
    return (id.sensor()%64) ;
    //    if (msgLevel(MSG::VERBOSE)) verbose()   << "velo: " << side << " " << station << endmsg ;
  } ;

  size_t itUniqueID(const LHCb::STChannelID& id) {
    if (msgLevel(MSG::VERBOSE)) verbose()   << "it: " << id.station() << " " << id.layer() << " "
                                            << id.detRegion() << " " << id.sector() << endmsg ;
    return (((id.station()-1)*4 + id.layer()-1)*4 + id.detRegion()-1)*7 + id.sector()-1 ;
  } ;

		
  size_t itUniqueBoxStation(const LHCb::STChannelID& id) {
    if (msgLevel(MSG::VERBOSE)) verbose()   << "it: " << id.station() << " " << id.layer() << " "
                                            << id.detRegion() << " " << id.sector() << endmsg ;
    return itUniqueID(id)%4 + 4*(id.station()-1);
  } ;




  size_t itUniqueBox(const LHCb::STChannelID& id) {
    if (msgLevel(MSG::VERBOSE)) verbose()   << "it: " << id.station() << " " << id.layer() << " "
                                            << id.detRegion() << " " << id.sector() << endmsg ;
    return itUniqueID(id)%4;
  } ;
	
  size_t itUniqueStation(const LHCb::STChannelID& id) {
    if (msgLevel(MSG::VERBOSE)) verbose()   << "it: " << id.station() << " " << id.layer() << " "
                                            << id.detRegion() << " " << id.sector() << endmsg ;
    return (id.station()-1);
  } ;

  //size_t ttUniqueID(const LHCb::STChannelID& id) {
  //if (msgLevel(MSG::VERBOSE)) verbose()   << "tt: " << id.station() << " " << id.layer() << " "
  //                                        << id.detRegion() << " " << id.sector() << endmsg ;
  //	return (((id.station()-1)*2 + id.layer()-1)*3 + id.detRegion()-1)*24 + id.sector()-1 ;
  //} ;

  size_t ttUniqueLayer(const LHCb::STChannelID& id) {
    if (msgLevel(MSG::VERBOSE)) verbose()   << "tt: " << id.station() << " " << id.layer() << " "
                                            << id.detRegion() << " " << id.sector() << endmsg ;
    return (id.station()-1)*2 + id.layer()-1 ;
  } ;
	
  size_t ttUniqueStation(const LHCb::STChannelID& id) {
    if (msgLevel(MSG::VERBOSE)) verbose()   << "tt: " << id.station() << " " << id.layer() << " "
                                            << id.detRegion() << " " << id.sector() << endmsg ;
    return (id.station()-1) ;
  } ;



private:
  std::string m_trackLocation;
  ToolHandle<ITrackProjector> m_projector ;
  std::string m_alignmentReference ;
  std::string m_modeVelo;
  std::string m_modeIT;
  std::string m_modeOT;
  std::string m_modeTT;
  std::unique_ptr<ResidualRecorder> m_velo ;
  std::unique_ptr<ResidualRecorder> m_it ;
  std::unique_ptr<ResidualRecorder> m_ot ;
  std::unique_ptr<ResidualRecorder> m_tt ;
} ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( AlignmentOnlineMonitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  AlignmentOnlineMonitor::AlignmentOnlineMonitor( const std::string& name, ISvcLocator* pSvcLocator)
    : GaudiHistoAlg( name , pSvcLocator ), m_projector("TrackProjector")  {
    declareProperty( "TrackLocation", m_trackLocation = LHCb::TrackLocation::Default);
    declareProperty( "VeloMode", m_modeVelo="side");
    declareProperty( "OTMode", m_modeOT="all");
    declareProperty( "ITMode", m_modeIT="all");
    declareProperty( "TTMode", m_modeTT="all");
  }

//=============================================================================
// Destructor
//=============================================================================
AlignmentOnlineMonitor::~AlignmentOnlineMonitor() {
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode AlignmentOnlineMonitor::initialize()
{
	StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
	if(sc.isFailure()) return sc ;
	sc = m_projector.retrieve() ;
    static const std::string histoDir = "Track/" ;
    if ( histoTopDir().empty() ) setHistoTopDir(histoDir);

	
	if(m_modeOT == "all"){ // 3stations * 4quarters (neglected: * 18modules)
		std::string otNames[12] = {"OT1a","OT1b","OT1c","OT1d","OT2a","OT2b","OT2c","OT2d","OT3a","OT3b","OT3c","OT3d"};
		m_ot.reset( new ResidualRecorder(*this, "OT",   12, otNames) );
	} else if((m_modeOT == "quarter")){
    std::string otNames[4] = {"OTa","OTb","OTc","OTd"};
		m_ot.reset( new ResidualRecorder(*this, "OT",   4, otNames) );
	} else if(m_modeOT == "station"){
		std::string otNames[3] = {"OT1","OT2","OT3"};
		m_ot.reset( new ResidualRecorder(*this, "OT",   3, otNames) );
	} else {
    err() << "OT histogram mode not defined. Possible values are <<>>, <<quarter>>, <<station>>" << endmsg ;
	}
	if (msgLevel(MSG::VERBOSE)) verbose()  <<"booked OT histos"<<endmsg;


	if(m_modeIT == "all"){ // 3stations * 4quarters (neglected: * 18modules)
		std::string itNames[12];
		for(int i = 0;i<3;i++){
			for(int j = 0;j<4;j++){
				itNames[j+(4*i)] = ITNames().StationToString(i+1) + " " + ITNames().BoxToString(j+1); 	
			}
		};
		//	 = {"IT1a","IT1b","IT1c","IT1d","IT2a","IT2b","IT2c","IT2d","IT3a","IT3b","IT3c","IT3d"};
		m_it.reset(  new ResidualRecorder(*this, "IT",   12, itNames) );
	} else if((m_modeIT == "box")){
    std::string itNames[4];
		for(int i = 0; i<4; i++){ itNames[i] =  ITNames().BoxToString(i+1);}
		m_it.reset(  new ResidualRecorder(*this, "IT",   4, itNames) );
	} else if(m_modeIT == "station"){
    std::string itNames[3] = {"IT1","IT2","IT3"};
		m_it.reset(  new ResidualRecorder(*this, "IT",   3, itNames) );
	} else {
		err() << "IT histogram mode not defined. Possible values are <<>>, <<box>>, <<station>>" << endmsg;
	}
	if (msgLevel(MSG::VERBOSE)) verbose()  <<"booked IT histos"<<endmsg;



	if(m_modeVelo == "all"){//21*2halves
    std::string veloNames[42];
		for(int i = 0;i<21;i++){
			std::string s;
			std::stringstream out;
			out << i+1;
			s = out.str();
			veloNames[i*2] = "Velo Side 1, Station "+s;
			veloNames[i*2+1] = "Velo Side 2, Station "+s;
		}
		m_velo.reset( new ResidualRecorder(*this, "Velo", 42, veloNames) );
	} else if(m_modeVelo == "side"){
    std::string veloNames[2] = {"Velo Side 1","Velo Side 2"};
		m_velo.reset( new ResidualRecorder(*this, "Velo", 2, veloNames) );
	} else {
		Warning("Velo histogram mode not defined. Possible values are <<>>, <<side>>").ignore();
	}
	if (msgLevel(MSG::VERBOSE)) verbose()  <<"booked Velo histos"<<endmsg;


	if(m_modeTT == "station"){
    std::string ttNames[2] = {"TTa","TTb"};
		m_tt.reset(  new ResidualRecorder(*this, "TT", 2, ttNames) );
	} else if(m_modeTT == "layer" || m_modeTT == "all"){
    std::string ttNames[4] = {"TTaX","TTaU","TTbV","TTbX"};
		m_tt.reset( new ResidualRecorder(*this, "TT", 4, ttNames) );
	} else {
    Warning("TT histogram mode not defined. Possible values are <<>>, <<layer>>, <<station>>").ignore();
	}
	if (msgLevel(MSG::VERBOSE)) verbose()  <<"booked TT histos"<<endmsg;


	return sc ;
}
//=============================================================================
// Finalize
//=============================================================================
StatusCode AlignmentOnlineMonitor::finalize()
{
	StatusCode sc = m_projector.release() ;
	if(sc.isFailure()) return sc ;
	m_velo.reset();
	m_ot.reset() ;
	m_it.reset() ;
	m_tt.reset() ;
	return GaudiHistoAlg::finalize() ;
}

StatusCode AlignmentOnlineMonitor::execute()
{
	auto tracks = get<LHCb::Track::Range>( m_trackLocation ) ;


    auto  ot_fun = ( m_modeOT == "station" ?  &AlignmentOnlineMonitor::otUniqueStation
                   : m_modeOT == "quarter" ?  &AlignmentOnlineMonitor::otUniqueQuarter
                   : m_modeOT == "all"     ?  &AlignmentOnlineMonitor::otUniqueStationQuarter
                   : nullptr );
    auto velo_fun = ( m_modeVelo == "side" ?  &AlignmentOnlineMonitor::veloUniqueSide
                    : m_modeVelo == "all"  ?  &AlignmentOnlineMonitor:: veloUniqueSideStation
                    : nullptr );
    auto it_fun = ( m_modeIT == "station" ? &AlignmentOnlineMonitor::itUniqueStation
                  : m_modeIT == "box"     ? &AlignmentOnlineMonitor::itUniqueBox
                  : m_modeIT == "all"     ? &AlignmentOnlineMonitor::itUniqueBoxStation
                  : nullptr );

    auto tt_fun = (	( m_modeTT == "layer" || m_modeTT == "all" ) ? &AlignmentOnlineMonitor::ttUniqueLayer
                  :  m_modeTT == "station" ? &AlignmentOnlineMonitor::ttUniqueStation
                  : nullptr );

	for( const LHCb::Track* track: tracks) {
		for( const LHCb::Node* node: track->nodes() ) {
			if( node->type() != LHCb::Node::HitOnTrack ) continue;
			const LHCb::Measurement& meas = node->measurement() ;
			LHCb::LHCbID id = meas.lhcbID();
			if( id.isOT() && ot_fun) {
                m_ot->fill( (this->*ot_fun)(id.otID()), *node) ;
			} else if( id.isVelo() && velo_fun) {
                m_velo->fill( (this->*velo_fun)(id.veloID()), *node) ;
			} else if( id.isIT() && it_fun ) {
                m_it->fill( (this->*it_fun)(id.stID()), *node) ;
			} else if( id.isTT() && tt_fun) {
                m_tt->fill( (this->*tt_fun)(id.stID()), *node) ;
			}
		}
	}
	return StatusCode::SUCCESS ;
}
