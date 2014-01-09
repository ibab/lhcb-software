#include <boost/format.hpp>

#include "GaudiKernel/ToolFactory.h"

#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram3D.h"
#include "GaudiUtils/Aida2ROOT.h"
#include "TH3D.h"

#include "Event/Particle.h"
#include "Event/RecVertex.h"

#include "Kernel/JetEnums.h"

// local
#include "PerPVOffsetJEC.h"

//------------------------------------------------------------------------------
// Implementation for class PerPVOffsetJEC
//
// Actaul code: Victor Coco
// 2013-12-19 : Pieter David (copied from PFJetMakerForDVAlg)
//------------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PerPVOffsetJEC )

//==============================================================================
// Constructor
//==============================================================================
PerPVOffsetJEC::PerPVOffsetJEC(const std::string& type, const std::string& name, const IInterface* parent)
  : GaudiTool(type, name, parent)
  , m_apply()      // property
  , m_histo_path() // property
  , m_histos(4, static_cast<TH3D*>(0))
  , m_shiftJEC()   // property
{
  declareInterface<IParticleReFitter>(this);
  declareProperty("Apply"    , m_apply      = true , "Apply JEC (otherwise, only store values)");
  declareProperty("HistoPath", m_histo_path = "JEC", "The path of the JEC histograms" );
  declareProperty("ShiftJEC" , m_shiftJEC   = 0.   , "Shift jet energy correction by n sigma");
}
//==============================================================================
// Destructor
//==============================================================================
PerPVOffsetJEC::~PerPVOffsetJEC()
{ }

//==============================================================================
// Initialization
//==============================================================================
StatusCode PerPVOffsetJEC::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) { return sc; }

  m_histos.clear();
  for( int i = 1; i < 4; ++i ) {
    std::string histoPath = boost::str(boost::format("%1%/JEC_PV%2%") % m_histo_path % i);
    AIDA::IHistogram3D* aidaHist = get<AIDA::IHistogram3D>(histoSvc(), histoPath);
    if ( aidaHist == 0 ) { return Warning("Could not find AIDA::IHistogram3D at "+histoPath, StatusCode::FAILURE); }
    m_histos.push_back(Gaudi::Utils::Aida2ROOT::aida2root(aidaHist));
  }

  return StatusCode::SUCCESS;
}

namespace {
  // Helper method for readability
  template<class T>
  const T& boundTo( const T& lower, const T& value, const T& upper )
  {
    return std::min( std::max( lower, value ), upper );
  }
}

//==============================================================================
// Main method
//==============================================================================
StatusCode PerPVOffsetJEC::reFit(LHCb::Particle& jet) const
{
  int nPVs = std::min( std::size_t(3), get<LHCb::RecVertex::Range>(LHCb::RecVertexLocation::Primary).size() );
  TH3D* histo = m_histos.at(nPVs-1);
  if ( histo == 0 ) { return Error("Problem with histo", StatusCode::FAILURE); }

  double jeteta = boundTo( 2.0, jet.momentum().Eta(), 4.8 );
  double jetpt  = boundTo( 0.0, jet.momentum().Pt()/Gaudi::Units::GeV, 499. );
  double jetcpf = jet.info(LHCb::JetIDInfo::CPF,-1.);

  double cor = 1.;
  if ( jetpt > 5. ) { cor = histo->Interpolate(jetpt, jeteta, jetcpf); }
  double corerr = histo->GetBinError(histo->FindBin(jetpt,jeteta,jetcpf));
  cor += m_shiftJEC*corerr;

  jet.addInfo( LHCb::JECInfo::JEC       , cor    );
  jet.addInfo( LHCb::JECInfo::NPVsForJEC, nPVs   );
  jet.addInfo( LHCb::JECInfo::JECError  , corerr );

  if (m_apply) { jet.setMomentum( cor*jet.momentum() ); }

  return SUCCESS;
}
