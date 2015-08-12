#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrackInterfaces/ITrackProjector.h"
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/Node.h"
#include "Event/Measurement.h"
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>
#include "AIDA/IProfile1D.h"
#include "AIDA/IHistogram1D.h"
#include "GaudiUtils/Aida2ROOT.h"
#include "DetDesc/3DTransformationFunctions.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "TProfile.h"

namespace {

  struct AlignProfile
  {
    AlignProfile(GaudiHistoAlg& alg, 
                 const std::string& dir, 
                 size_t numelements, 
                 const std::string& dofs)
      : m_dir(dir), m_parent(&alg), m_numelements(numelements)
    {
      for(int i=0; i<6; ++i)
      {
        if( dofs.find(m_dofnames[i]) != std::string::npos )
        {
          // The "G" option is important here.
          m_delta[i] = alg.bookProfile1D( dir + "/" + m_dofnames[i] + "VsElement", 
                                          dir + " " + m_dofnames[i] ,
                                          -0.5, numelements-0.5, numelements,"G" ) ;
        }
        else
        {
          m_delta[i] = 0 ;
        }
      }
    }

    void fill( size_t id,
               const LHCb::Node& node,
               const ITrackProjector::Derivatives& deriv)
    {
      const double V    = node.errMeasure2() ;
      const double Vinv = 1.0/V ;//if this causes an FPE, then I want to know!
      double R = node.errResidual2() ;
      for(int i=0; i<6; ++i)
      {
        if ( m_delta[i] != 0 ) 
        {
          double Ax = deriv(0,i) ;
          // first compute the weight, which is the 2nd derivative
          // w = A^T * V^-1 * R * V^-1 * A
          double halfd2Chi2dX2 = Ax * Vinv * R * Vinv * Ax ;
          // now compute the 1 derivative
          if( std::abs(halfd2Chi2dX2) > 1e-15 ) 
          {
            const double halfdChi2dX   = Ax * Vinv * node.residual() ;
            // now fill the profile
            m_delta[i]->fill( id, halfdChi2dX / halfd2Chi2dX2, halfd2Chi2dX2 ) ;
          }
        }
      }
    }

    ~AlignProfile()
    {
      for(int i=0; i<6; ++i)
      {
        if ( m_delta[i] != 0 ) 
        {
          TProfile* pr = Gaudi::Utils::Aida2ROOT::aida2root ( m_delta[i] ) ;
          if ( pr ) 
          {
            IHistogram1D* pull = m_parent->book(m_dir + "/" + m_dofnames[i] + "Pull",
                                                m_delta[i]->title() + " pull",-5,5) ;
            double xmax = i<=2 ? 0.1 : 0.001 ;
            IHistogram1D * delta = m_parent->book(m_dir + "/" + m_dofnames[i] + "Delta",
                                                  m_delta[i]->title(),-xmax,xmax) ;
            for(int ibin = 1; ibin<=pr->GetNbinsX(); ++ibin)
            {
              pull ->fill( fabs(pr->GetBinError(ibin)) > 0 ?
                           pr->GetBinContent(ibin)/pr->GetBinError(ibin) : 9e30 ) ;
              delta->fill( pr->GetBinContent(ibin) ) ;
            }
          }
        }
      }
    }

    static const std::vector<std::string> m_dofnames;

    IProfile1D* m_delta[6] ;
    std::string m_dir ;
    GaudiHistoAlg* m_parent ;
    size_t m_numelements ;
  } ;

  const std::vector<std::string> AlignProfile::m_dofnames = boost::assign::list_of("Tx")("Ty")("Tz")("Rx")("Ry")("Rz") ;

}

class TrackAlignMonitor : public GaudiHistoAlg
{
public:
  /** Standard construtor */
  TrackAlignMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  /** Destructor */
  virtual ~TrackAlignMonitor();

  /** Algorithm initialize */
  virtual StatusCode initialize();

  /** Algorithm initialize */
  virtual StatusCode finalize();

  /** Algorithm execute */
  virtual StatusCode execute();

private:
  enum { NumOT = 432, NumVelo = 42, NumIT=336, NumTT=288 } ;

  size_t otUniqueID(const LHCb::OTChannelID& id) {
    return (((id.station()-1)*4 + id.layer())*4 + id.quarter())*9 + id.module() ;
  } ;

  size_t veloUniqueID(const LHCb::VeloChannelID& id) {
    unsigned int station = (id.sensor()%64)/2 ;
    unsigned int side    = (id.sensor()%2) ;
    //unsigned int type    = (id.sensor()/64) ;
    //std::cout << "velo: " << side << " " << station << std::endl ;
    return side * 21 + station ;
  } ;

  size_t itUniqueID(const LHCb::STChannelID& id) {
    //std::cout << "it: " << id.station() << " " << id.layer() << " " << id.detRegion() << " " << id.sector() << std::endl ;
    return (((id.station()-1)*4 + id.layer()-1)*4 + id.detRegion()-1)*7 + id.sector()-1 ;
  }

  size_t ttUniqueID(const LHCb::STChannelID& id) {
    //std::cout << "tt: " << id.station() << " " << id.layer() << " " << id.detRegion() << " " << id.sector() << std::endl ;
    return (((id.station()-1)*2 + id.layer()-1)*3 + id.detRegion()-1)*24 + id.sector()-1 ;
  }

  void transformToLocal( ITrackProjector::Derivatives& deriv,
                         const DetectorElement& element ) const ;

private:
  std::string m_trackLocation;
  ToolHandle<ITrackProjector> m_projector ;
  bool m_useLocalFrame ;
  double m_maxTrackChi2PerDoF ;
  std::string m_veloDofs ;
  std::string m_ttDofs ;
  std::string m_otDofs ;
  std::string m_itDofs ;
  AlignProfile* m_velo ;
  AlignProfile* m_it ;
  AlignProfile* m_ot ;
  AlignProfile* m_tt ;
  typedef ROOT::Math::SMatrix<double,6,6> Matrix6x6 ;
  typedef std::map<const DetectorElement*, Matrix6x6> Jacobians ;
  mutable Jacobians m_jacobians ;
} ;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TrackAlignMonitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackAlignMonitor::TrackAlignMonitor( const std::string& name,
                                      ISvcLocator* pSvcLocator)
: GaudiHistoAlg( name , pSvcLocator ),
  m_projector("TrackProjector"), m_velo(0), m_it(0), m_ot(0), m_tt(0)
{
  declareProperty( "TrackLocation", m_trackLocation = LHCb::TrackLocation::Default  );
  declareProperty( "UseLocalFrame", m_useLocalFrame = true );
  declareProperty( "MaxTrackChi2PerDoF", m_maxTrackChi2PerDoF = 10 ) ;
  declareProperty( "VeloDofs", m_veloDofs = "TxTyRz" ) ;
  declareProperty( "TTDofs", m_ttDofs = "TxRz" ) ;
  declareProperty( "ITDofs", m_itDofs = "TxRz" ) ;
  declareProperty( "OTDofs", m_otDofs = "TxRz" ) ;

}

//=============================================================================
// Destructor
//=============================================================================
TrackAlignMonitor::~TrackAlignMonitor()
{
}


//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackAlignMonitor::initialize()
{
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if(sc.isFailure()) return sc ;
  sc = m_projector.retrieve() ;
  setHistoTopDir("Track/") ;
  m_velo = new AlignProfile(*this, "Velo", NumVelo, m_veloDofs) ;
  m_ot   = new AlignProfile(*this, "OT",   NumOT,   m_otDofs) ;
  m_it   = new AlignProfile(*this, "IT",   NumIT,   m_itDofs) ;
  m_tt   = new AlignProfile(*this, "TT",   NumTT,   m_ttDofs) ;
  return sc ;
}
//=============================================================================
// Finalize
//=============================================================================
StatusCode TrackAlignMonitor::finalize()
{
  StatusCode sc = m_projector.release() ;
  if(sc.isFailure()) return sc ;
  info() << "Number of elements in jacobian map: " << m_jacobians.size() << endmsg ;
  delete m_velo ;
  delete m_ot ;
  delete m_it ;
  delete m_tt ;
  return GaudiHistoAlg::finalize() ;
}

StatusCode TrackAlignMonitor::execute()
{
  // make a list of calo positions, depending on the subsystem use clusters or cells
  LHCb::Track::Range tracks = get<LHCb::Track::Range>( m_trackLocation ) ;

  //fix by RWL to get rid of stupid nightly warnings about bracketing. 16/11/2009
  BOOST_FOREACH( const LHCb::Track* track, tracks) {
    if( ( (track->hasT() && track->hasVelo() ) ||
          track->checkFlag(LHCb::Track::Backward))  &&
        (track->chi2PerDoF()< m_maxTrackChi2PerDoF)
        ) {
      BOOST_FOREACH( const LHCb::Node* node, track->nodes() ) {
        if( node->type() == LHCb::Node::HitOnTrack ) {
          const LHCb::Measurement& meas = node->measurement() ;
          // we'll evaluate everything in global coordinates, but to
          // make rotations more meaningful we'll go to the center of
          // the local frame.

          // first ignore global position. we'll do that later
          LHCb::StateVector state(node->state().stateVector(),node->state().z());
          ITrackProjector::Derivatives deriv =
            m_projector->alignmentDerivatives(state,meas,Gaudi::XYZPoint(0,0,0)) ;
          if( m_useLocalFrame && meas.detectorElement() )
            transformToLocal( deriv, *meas.detectorElement() ) ;
          LHCb::LHCbID id = meas.lhcbID() ;
          if(       id.isOT() ) {
            m_ot->fill( otUniqueID(id.otID()), *node, deriv ) ;
          } else if(id.isVelo() ) {
            m_velo->fill( veloUniqueID(id.veloID()), *node, deriv ) ;
          } else if(id.isIT() ) {
            m_it->fill( itUniqueID(id.stID()), *node, deriv ) ;
          } else if(id.isTT() ) {
            m_tt->fill( ttUniqueID(id.stID()), *node, deriv ) ;
          }
        }
      }
    }
  }
  return StatusCode::SUCCESS ;
}

void TrackAlignMonitor::transformToLocal( ITrackProjector::Derivatives& deriv,
                                          const DetectorElement& element ) const
{
  Jacobians::iterator it = m_jacobians.find( &element ) ;

  // insert a new jacobian if it wasn't there yet
  if( it == m_jacobians.end() ) {
    Gaudi::Transform3D localToGlobal = element.geometry()->toGlobalMatrix() ;
    Matrix6x6 jacobian = DetDesc::getTransformJacobian( localToGlobal ) ;
    //m_jacobians[&element] = jacobian ;
    std::pair<Jacobians::iterator, bool> location =
      m_jacobians.insert( std::make_pair(&element,jacobian) ) ;
    it = location.first ;
  }

  ITrackProjector::Derivatives globalderiv = deriv ;
  deriv = globalderiv * it->second ;
}
