#ifndef PrVeloUTChecker_CPP
#define PrVeloUTChecker_CPP
#include "GaudiKernel/PhysicalConstants.h"
#include "Event/STCluster.h"
#include "Linker/AllLinks.h"
#include "Linker/LinkedFrom.h"

#include "PrVeloUTChecker.h"

DECLARE_ALGORITHM_FACTORY( PrVeloUTChecker )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrVeloUTChecker::PrVeloUTChecker(const std::string& name, ISvcLocator* pSvcLocator ) :
  GaudiHistoAlg( name , pSvcLocator ),
  m_associator(0,""), m_associator_velo(0,""), m_associator_forward(0,"")
{
	declareProperty("VeloTTContainer" , m_VeloTTContainer  = LHCb::TrackLocation::VeloTT);
	declareProperty("VeloContainer"   , m_VeloContainer    = LHCb::TrackLocation::Velo);
	declareProperty("ForwardContainer", m_ForwardContainer = LHCb::TrackLocation::Forward);
}

//=============================================================================
// Destructor
//=============================================================================
PrVeloUTChecker::~PrVeloUTChecker() {;}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrVeloUTChecker::initialize()
{
	// Mandatory initialization of GaudiAlgorithm
	StatusCode sc = GaudiHistoAlg::initialize();
	if ( sc.isFailure() ) { return sc; }

	m_selector = tool<IMCReconstructible>("MCReconstructible","Selector",this);

	m_eff_counter.addCategory("reco'ble as long                     ");
	m_eff_counter.addCategory("reco'ble as long + reco'ed in Velo   ");
	m_eff_counter.addCategory("reco'ble as long + reco'ed by Forward");
	m_eff_counter.addCategory("reco'ble as long + in TT acceptance  ");
	m_eff_counter.addCategory("loose HLT                            ");
	m_eff_counter.addCategory("loose HLT + from B                   ");
	m_eff_counter.addCategory("loose HLT + in TT acceptance         ");
	m_eff_counter.addCategory("loose HLT + in TT acceptance + from B");
	m_eff_counter.addCategory("tight HLT                            ");
	m_eff_counter.addCategory("tight HLT + from B                   ");
	m_eff_counter.addCategory("tight HLT + in TT acceptance         ");
	m_eff_counter.addCategory("tight HLT + in TT acceptance + from B");

	m_ghosts_counter.addCategory("all         ");
	m_ghosts_counter.addCategory(" + good Velo");
	m_ghosts_counter.addCategory(" + loose HLT");
	m_ghosts_counter.addCategory(" + tight HLT");

	return StatusCode::SUCCESS;
}

//=============================================================================
// Execute
//=============================================================================
StatusCode PrVeloUTChecker::execute()
{
	StatusCode sc = StatusCode::SUCCESS;

	// check input data
	if (!exist<LHCb::Tracks>(m_VeloTTContainer))
		return Warning( m_VeloTTContainer+" not found", StatusCode::SUCCESS, 0);
	if (!exist<LHCb::Tracks>(m_VeloContainer))
		return Warning( m_VeloContainer+" not found", StatusCode::SUCCESS, 0);
	if (!exist<LHCb::Tracks>(m_ForwardContainer))
		return Warning( m_ForwardContainer+" not found", StatusCode::SUCCESS, 0);

	// get the association tables
	m_associator  = AsctTool(evtSvc(), m_VeloTTContainer);
	m_directTable = m_associator.direct();
	if (!m_directTable)
   		return Error("Failed to find direct table for VeloTT tracks", StatusCode::FAILURE);
	m_inverseTable = m_associator.inverse();
	if (!m_inverseTable)
   		return Error("Failed to find inverse table for VeloTT tracks", StatusCode::FAILURE);

	m_associator_velo  = AsctTool(evtSvc(), m_VeloContainer);
	m_directTable_velo = m_associator_velo.direct();
	if (!m_directTable_velo)
   		return Error("Failed to find direct table for Velo tracks", StatusCode::FAILURE);
	m_inverseTable_velo = m_associator_velo.inverse();
	if (!m_inverseTable_velo)
   		return Error("Failed to find inverse table for Velo tracks", StatusCode::FAILURE);

	m_associator_forward  = AsctTool(evtSvc(), m_ForwardContainer);
	m_directTable_forward = m_associator_forward.direct();
	if (!m_directTable_forward)
   		return Error("Failed to find direct table for Forward tracks", StatusCode::FAILURE);
	m_inverseTable_forward = m_associator_forward.inverse();
	if (!m_inverseTable_forward)
   		return Error("Failed to find inverse table for Forward tracks", StatusCode::FAILURE);

	// compute performances
	computeEfficiency();
	computeGhostRate();

	return StatusCode::SUCCESS;
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrVeloUTChecker::finalize()
{
	printStatistics();
	return GaudiHistoAlg::finalize();
}

//=============================================================================
// Loop over MC particles and look for reconstructed tracks
//=============================================================================
void PrVeloUTChecker::computeEfficiency()
{
	// loop over MC particles
	const LHCb::MCParticles* particles = get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default);
	for (LHCb::MCParticles::const_iterator ip = particles->begin(); ip != particles->end(); ++ip)
	{
		if (!m_selector->isReconstructibleAs(IMCReconstructible::ChargedLong,*ip)) continue;
		if ( fabs((*ip)->particleID().pid())==11 ) continue;

		std::vector<bool> flags;
		flags.push_back(true);//reco'ble as long

		AsctTool::InverseType::Range range = m_inverseTable_velo->relations(*ip);
		flags.push_back( !(range.empty()) );//reco'ed in Velo

		range = m_inverseTable_forward->relations(*ip);
		bool reconstructed_forward = !(range.empty());
		flags.push_back( reconstructed_forward );//reco'ed by Forward

		int nTTlayers, nTThits;
      getTTtruth(*ip,nTThits,nTTlayers);
      flags.push_back( reconstructed_forward && (nTTlayers>2) );//reco'ed by Forward and in TT acceptance

      bool loose = reconstructed_forward && (*ip)->momentum().P()>3000. && (*ip)->momentum().Pt()>500.;
      bool tight = reconstructed_forward && (*ip)->momentum().P()>3000. && (*ip)->momentum().Pt()>1250.;
		bool fromB = bAncestor(*ip);

      flags.push_back( loose );//loose HLT
      flags.push_back( loose && fromB );//loose HLT from B
      flags.push_back( loose && (nTTlayers>2) );//loose HLT and in TT acceptance
      flags.push_back( loose && fromB && (nTTlayers>2));//loose HLT from B and in TT acceptance

      flags.push_back( tight );//tight HLT
      flags.push_back( tight && fromB );//tight HLT from B
      flags.push_back( tight && (nTTlayers>2) );//tight HLT and in TT acceptance
      flags.push_back( tight && fromB && (nTTlayers>2));//tight HLT from B and in TT acceptance

		//count how many are reconstructed
		range = m_inverseTable->relations(*ip);
		bool reconstructed = !(range.empty());

		if ( flags.size()!=m_eff_counter.name.size() )
			error() << "Number of flags (" << flags.size()
		           << ") does not coincide with number of categories (" << m_eff_counter.name.size()
		           << ") in the efficiency counter" << endmsg;

		m_eff_counter.count(reconstructed, flags);

		fillEffHistos((*ip)->momentum().P()/1e3 , "p [GeV/c]"    , 0., 50., reconstructed, flags);
		fillEffHistos((*ip)->momentum().Pt()/1e3, "p_{T} [GeV/c]", 0.,  3., reconstructed, flags);
		fillEffHistos((*ip)->momentum().Eta()   , "#eta"         , 1.,  6., reconstructed, flags);
		fillEffHistos((*ip)->momentum().Phi()   , "#phi [rad]"   ,-4.,  4., reconstructed, flags);
		fillEffHistos(nTTlayers                 , "MC TT layers" , 0.,  5., reconstructed, flags);
	}
}

//=============================================================================
// Loop over tracks and look for matched MC particles
//=============================================================================
void PrVeloUTChecker::computeGhostRate()
{
	// Loop over tracks
	LHCb::Tracks* tracks = get<LHCb::Tracks>(m_VeloTTContainer);
	for (LHCb::Tracks::const_iterator it = tracks->begin(); it!=tracks->end(); ++it)
	{
		std::vector<bool> flags;
		flags.push_back(true);//all tracks

		LHCb::Track* veloTr = *(((*it)->ancestors()).begin());
		AsctTool::DirectType::Range range_velo = m_directTable_velo->relations(veloTr);
		bool velo_ghost = range_velo.empty();
		flags.push_back( !velo_ghost );//good velo

      bool loose = !velo_ghost && (*it)->p()>3000. && (*it)->pt()>500.;
      bool tight = !velo_ghost && (*it)->p()>3000. && (*it)->pt()>1250.;
      flags.push_back( loose );//loose HLT
      flags.push_back( tight );//tight HLT

		//count how many are ghosts
		AsctTool::DirectType::Range range = m_directTable->relations(*it);
		bool ghost = range.empty();

		if ( flags.size()!=m_ghosts_counter.name.size() )
			error() << "Number of flags (" << flags.size()
		           << ") does not coincide with number of categories (" << m_eff_counter.name.size()
		           << ") in the ghosts counter" << endmsg;

		m_ghosts_counter.count(ghost, flags);

		fillGhostHistos((*it)->p()/1e3         , "p [GeV/c]"    , 0., 50., ghost, flags);
		fillGhostHistos((*it)->pt()/1e3        , "p_{T} [GeV/c]", 0.,  3., ghost, flags);
		fillGhostHistos((*it)->pseudoRapidity(), "#eta"         , 1.,  6., ghost, flags);
		fillGhostHistos((*it)->phi()           , "#phi [rad]"   ,-4.,  4., ghost, flags);
		fillGhostHistos((*it)->chi2PerDoF()    , "#chi^{2}/ndf" , 0.,  5., ghost, flags);
	}
}

//=============================================================================
// Get MC TT hist for particle p
//=============================================================================
void PrVeloUTChecker::getTTtruth(const LHCb::MCParticle* p, int &nhits, int &nlayers)
{
	LinkedFrom<LHCb::STCluster,LHCb::MCParticle> ttLink(evtSvc(),msgSvc(), LHCb::STClusterLocation::UTClusters);
	if (ttLink.notFound()) return;

	nhits = 0;
	std::vector<int> fired;
	const LHCb::STCluster* TTCluster = ttLink.first(p);
	for ( ; 0 != TTCluster; TTCluster = ttLink.next()) {
		if ( !TTCluster->isUT() ) continue;
		nhits++;
		int layer = TTCluster->layer() + 10*TTCluster->station();
		if ( fired.empty() || std::find(fired.begin(), fired.end(), layer) == fired.end() )
			fired.push_back(layer);
	}
	nlayers = fired.size();

	return;
}

//=============================================================================
// Look if particle mcPart comes from a b-hadron decay
//=============================================================================
bool PrVeloUTChecker::bAncestor(const LHCb::MCParticle* mcPart) const
{
   bool fromB = false;
   const LHCb::MCParticle* mother = mcPart->mother();
   while ( mother !=0 && fromB == false) {
      fromB = mother->particleID().hasBottom() && 
        ( mother->particleID().isMeson() ||  mother->particleID().isBaryon() );
      mother = mother->mother();
   }
   return fromB;
}

//=============================================================================
// Print final summary
//=============================================================================
void PrVeloUTChecker::printStatistics()
{
	always() << "**** Velo TT efficiencies:" << endmsg;
	for ( unsigned int kk = 0; m_eff_counter.name.size() > kk; ++kk ) {
		if ( 0 == m_eff_counter.den[kk] ) {
			always() << "  " << m_eff_counter.name[kk] << " -- no particles found" << endmsg;
			continue;
		}
		double eff = double( m_eff_counter.num[kk] ) / double( m_eff_counter.den[kk] );
		double err = sqrt( eff*(1.-eff) / double(m_eff_counter.den[kk]) );
		always() << "  " << m_eff_counter.name[kk] << format( " (%4.2f +/- %.2f)%%", 100.*eff, 100.*err) << endmsg;
	}

	always() << "**** Velo TT ghost rates:" << endmsg;
	for ( unsigned int kk = 0; m_ghosts_counter.name.size() > kk; ++kk ) {
		if ( 0 == m_ghosts_counter.den[kk] ) {
			always() << "  " << m_ghosts_counter.name[kk] << " -- no tracks found" << endmsg;
			continue;
		}
		double eff = double( m_ghosts_counter.num[kk] ) / double( m_ghosts_counter.den[kk] );
		double err = sqrt( eff*(1.-eff) / double(m_ghosts_counter.den[kk]) );
		always() << "  " << m_ghosts_counter.name[kk] << format( " (%4.2f +/- %.2f)%%", 100.*eff, 100.*err) << endmsg;
	}
}

//=============================================================================
// Filling histos
//=============================================================================
void PrVeloUTChecker::fillEffHistos( double var, std::string title, double min, double max, bool found, std::vector<bool> flags )
{
	for ( unsigned int kk = 0; flags.size() > kk; ++kk ) {
		if ( !flags[kk] ) continue;
		std::string histoname = "MC particle "+title+" - "+m_eff_counter.name[kk];
		plot( var, histoname, min, max);
		if ( !found ) continue;
		plot( var, "reco'ed "+histoname, min, max);
	}
}

void PrVeloUTChecker::fillGhostHistos( double var, std::string title, double min, double max, bool found, std::vector<bool> flags )
{
	for ( unsigned int kk = 0; flags.size() > kk; ++kk ) {
		if ( !flags[kk] ) continue;
		std::string histoname = "track "+title+" - "+m_ghosts_counter.name[kk];
		plot( var, histoname, min, max);
		if ( !found ) continue;
		plot( var, "ghost "+histoname, min, max);
	}
}

//=============================================================================
// Methods of MyCounter
//=============================================================================
void PrVeloUTChecker::MyCounter::addCategory( std::string s )
{
	name.push_back(s);
	num.push_back(0);
	den.push_back(0);
}

void PrVeloUTChecker::MyCounter::count( bool found, std::vector<bool> flags )
{
	for ( unsigned int kk = 0; flags.size() > kk; ++kk ) {
		if ( !flags[kk] ) continue;
		++den[kk];
		if ( !found ) continue;
		++num[kk];
	}
}

#endif //PrVeloUTChecker_CPP
