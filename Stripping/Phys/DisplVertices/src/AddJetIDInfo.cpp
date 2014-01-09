#include "GaudiKernel/ToolFactory.h"

#include "Math/VectorUtil.h"

#include "LoKi/ParticleCuts.h"
#include "LoKi/ParticleContextCuts.h"
using namespace LoKi::Cuts;

// local
#include "AddJetIDInfo.h"

//------------------------------------------------------------------------------
// Implementation for class AddJetIDInfo
//
// Actaul code: Victor Coco
// 2013-12-19 : Pieter David (copied from PFJetMakerForDVAlg)
//------------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( AddJetIDInfo )

namespace {
  typedef LoKi::Constant<const LHCb::Particle*,double> _PDOUBLE;
  typedef LoKi::Constant<const LHCb::Particle*,bool>   _PBOOL;
}

//==============================================================================
// Constructor
//==============================================================================
AddJetIDInfo::AddJetIDInfo(const std::string& type, const std::string& name, const IInterface* parent)
  : GaudiTool(type, name, parent)
  , m_cache()
  , PFTYPE          (_PDOUBLE(-1.))
  , NSATECAL        (_PDOUBLE(-1.))
  , NSATHCAL        (_PDOUBLE(-1.))
  , NWITHPVINFO     (_PDOUBLE(-1.))
  , NWITHPVINFOCHI24(_PDOUBLE(-1.))
  , HASPVINFO       (_PBOOL(false))
{
  declareInterface<IExtraInfoTool>(this);
  clearCache();
}
//==============================================================================
// Destructor
//==============================================================================
AddJetIDInfo::~AddJetIDInfo()
{ }

//==============================================================================
// Initialize
//==============================================================================
StatusCode AddJetIDInfo::initialize()
{
  if (msgLevel(MSG::DEBUG)) { debug() << "==> initialize" << endmsg; }

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) { return sc; }

  PFTYPE = INFO(LHCb::PFParticle::Type, -10.);
  NSATECAL = SUMTREE( INFO(LHCb::JetIDInfo::NSatECAL,0.), PT > 0., 0. );
  NSATHCAL = SUMTREE( INFO(LHCb::JetIDInfo::NSatHCAL,0.), PT > 0., 0. );
  HASPVINFO = ( PFTYPE > LHCb::PFParticle::Composite && PFTYPE < LHCb::PFParticle::BadParticle ) || ( HASTRACK && !ISDOWN );
  // NOTE why not HASTRACK & TRCUT(TrHASVELO) ?
  NWITHPVINFO = NINTREE(HASPVINFO);
  NWITHPVINFOCHI24 = NINTREE(HASPVINFO && ( BPVIPCHI2("") < 9. ));

  return StatusCode::SUCCESS;
}

//==============================================================================
// Finalize
//==============================================================================
StatusCode AddJetIDInfo::finalize()
{
  if (msgLevel(MSG::DEBUG)) { debug() << "==> finalize" << endmsg; }

  // reset functors
  PFTYPE           = _PDOUBLE(-1.);
  NSATECAL         = _PDOUBLE(-1.);
  NSATHCAL         = _PDOUBLE(-1.);
  NWITHPVINFO      = _PDOUBLE(-1.);
  NWITHPVINFOCHI24 = _PDOUBLE(-1.);
  HASPVINFO        = _PBOOL(false);

  return GaudiTool::finalize();
}

//==============================================================================
// Interface method
//==============================================================================
StatusCode AddJetIDInfo::calculateExtraInfo(const LHCb::Particle* /* top */, const LHCb::Particle* jet)
{ // TODO review implementation
  clearCache();

  double mtf;    /// Highest pT track / Jet pT
  double mpt;    /// Highest pT track
  double cpf;    /// charged pT fraction - V0s are not included
  double width;  /// jet width
  int    n90;    /// Number of items responsible for at least 90% of the jet momentum
  int    ntrk;   /// Number of tracks

  double auxptmax=-1, sumpt=0; int iitems=0;
  double tpx=0, tpy=0;
  std::vector<double> itemspt;
  ntrk=n90=width=0;

  BOOST_FOREACH( const LHCb::Particle* daug, jet->daughters() ) {
    if (msgLevel(MSG::VERBOSE)) { verbose() << "Jet daughter " << daug->particleID() << "(" << daug->particleID().threeCharge() << ") " << daug->momentum() << endmsg; }
    // FIXME remove printouts if possible
    if ( daug->info(LHCb::JetIDInfo::NSatECAL,0.) > 0 ) {
      always() << "A saturated cell ECAL" << endmsg;
    }
    if ( daug->info(LHCb::JetIDInfo::NSatHCAL,0.) > 0 ) {
      always() << "A saturated cell HCAL" << endmsg;
    }

    if ( daug->particleID().threeCharge() != 0 ) {
      ++ntrk;
      auxptmax = std::max( daug->momentum().Pt(), auxptmax );
      tpx += daug->momentum().Px();
      tpy += daug->momentum().Py();
    }
    ++iitems;
    double pt = daug->momentum().Pt();
    sumpt += pt;
    itemspt.push_back(pt);
    for ( int ii = 0; ii < iitems; ++ii ) {
      if ( itemspt[ii] < pt ) {
        double aux = itemspt[ii];
        itemspt[ii] = pt;
        pt = aux;
      }
    }
    width += ROOT::Math::VectorUtil::DeltaR(daug->momentum(), jet->momentum()) * daug->momentum().Pt();
  }
  mtf = std::min( std::max( 0., auxptmax / jet->momentum().Pt() ), 1. );
  mpt = auxptmax;
  cpf = sqrt(tpx*tpx + tpy*tpy)/jet->momentum().Pt();
  width /= sumpt;
  if (msgLevel(MSG::VERBOSE)) { verbose() << "Jet momentum " << jet->momentum() << endmsg; }

  std::sort( itemspt.begin(), itemspt.end() );

  double auxptsum = 0;
  n90=0;
  for( int ii= iitems-1; ii >= 0 ; --ii ) {
    auxptsum+=itemspt[ii];
    ++n90;
    if(auxptsum/sumpt>0.9)
      break;
  }

  setCache(LHCb::JetIDInfo::Ntracks , ntrk );
  setCache(LHCb::JetIDInfo::N90     , n90  );
  setCache(LHCb::JetIDInfo::MTF     , mtf  );
  setCache(LHCb::JetIDInfo::MPT     , mpt  );
  setCache(LHCb::JetIDInfo::CPF     , cpf  );
  setCache(LHCb::JetIDInfo::JetWidth, width);
  double nSatECAL = NSATECAL(jet);
  double nSatHCAL = NSATHCAL(jet);
  setCache(LHCb::JetIDInfo::NSatECAL   , nSatECAL);
  setCache(LHCb::JetIDInfo::NSatHCAL   , nSatHCAL);
  setCache(LHCb::JetIDInfo::NSatCalo   , nSatECAL+nSatHCAL);
  setCache(LHCb::JetIDInfo::NHasPV     , NWITHPVINFO(jet));
  setCache(LHCb::JetIDInfo::NIPChi2Inf4, NWITHPVINFOCHI24(jet));

  if ( msgLevel(MSG::DEBUG) ) { debug() << "Calculated extra infos" << endmsg; dump(); }

  return StatusCode::SUCCESS;
}

bool AddJetIDInfo::cacheValid() const
{
  BOOST_FOREACH( const double& v, m_cache ) {
    if ( v == -10. ) {
      return false;
    }
  }
  return true;
}

int AddJetIDInfo::getInfo(int index, double& value, std::string& name)
{
  if ( ( index < getFirstIndex() ) || ( index-getFirstIndex() >= getNumberOfParameters() ) || (!cacheValid()) ) {
    return 0;
  }
  value = getCache(index);
  switch (index) {
    case LHCb::JetIDInfo::Ntracks     : name = "Ntracks"     ; break;
    case LHCb::JetIDInfo::N90         : name = "N90"         ; break;
    case LHCb::JetIDInfo::MTF         : name = "MTF"         ; break;
    case LHCb::JetIDInfo::MPT         : name = "MPT"         ; break;
    case LHCb::JetIDInfo::CPF         : name = "CPF"         ; break;
    case LHCb::JetIDInfo::JetWidth    : name = "JetWidth"    ; break;
    case LHCb::JetIDInfo::NSatCalo    : name = "NSatCalo"    ; break;
    case LHCb::JetIDInfo::NSatECAL    : name = "NSatECAL"    ; break;
    case LHCb::JetIDInfo::NSatHCAL    : name = "NSatHCAL"    ; break;
    case LHCb::JetIDInfo::NHasPV      : name = "NHasPV"      ; break;
    case LHCb::JetIDInfo::NIPChi2Inf4 : name = "NIPChi2Inf4" ; break;
  }
  return 1;
}

void AddJetIDInfo::dump()
{
  double value;
  std::string name;
  for ( int i = getFirstIndex(), i_end = i+getNumberOfParameters(); i < i_end; ++i ) {
    if ( 0 != getInfo(i, value, name) ) {
      debug() << " - " << name << " : " << value << endmsg;
    } else {
      debug() << "Problem (";
      if ( ( i < getFirstIndex() ) || ( i-getFirstIndex() >= getNumberOfParameters() ) ) {
        debug() << "index out of range";
      } else if ( !cacheValid() ) {
        debug() << "cache invalid";
      } else {
        debug() << "unknown";
      }
      debug() << ")" << endmsg;
    }
  }
}
