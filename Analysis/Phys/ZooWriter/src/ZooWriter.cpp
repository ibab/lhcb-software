//$Header: $

/* ZooWriter
*
* It saves all defined selections in the tree.
* Note, that it build the selections list when SaveZoo()
* is called for the first time.
*
* AZ, SHM, MS, GK,  
* PI Heidelberg 2008, 2009, 2010
*
* Note: To ensure the compatibility of your NTuples, you should not change
* anything in this Algorithm. Instead ask the maintainers of this package if you
* want to add new features.
*/

#include <memory>
#include <algorithm>
#include <iostream>
#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/assign/list_of.hpp>

#include <TProcessID.h>
#include <TMap.h>
#include <TObjString.h>

#include "Event/ODIN.h"
#include "Kernel/ITriggerTisTos.h" 
#include "Kernel/IPVReFitter.h"
#include "Kernel/ILifetimeFitter.h"
#include "Kernel/IBackgroundCategory.h"
#include "MCInterfaces/ITrackGhostClassification.h"
//#include "Kernel/IFilterCriterion.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "Kernel/IDaVinciAssociatorsWrapper.h" 
#include "MCInterfaces/IVisPrimVertTool.h"
#include "GaudiKernel/IEventTimeDecoder.h"
#include <Kernel/IDistanceCalculator.h>
#include "TrackKernel/TrackStateVertex.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "Kernel/HitPattern.h"
#include "Event/L0DUReport.h" 
#include "Event/HltDecReports.h"
#include "Event/MCTrackInfo.h" 
#include "Event/GhostTrackInfo.h"
#include "Event/MCParticle.h"
#include "TrackInterfaces/ITrackFitter.h"
#include "TrackInterfaces/ITrackHitCollector.h"
#include "PatKernel/PatForwardHit.h"
#include "PatKernel/PatTTHit.h"
#include "TfKernel/TStationHitManager.h"
#include "TfKernel/TTStationHitManager.h"
#include "TfKernel/DefaultVeloRHitManager.h"
#include "TfKernel/DefaultVeloPhiHitManager.h"
#include "Event/GenHeader.h"
#include "Event/CaloDigit.h"

#include "Event/MuonPID.h"


#include "LoKi/PhysTypes.h"
#include "LoKi/IHybridFactory.h"

#if DV_VER >= 293
#include "Event/RecSummary.h"
#endif
//#include "DecayTreeFitter/VtxFitParams.h"

using namespace Gaudi::Units;

#include "ZooWriter.h"
#include "Zoo.h"
#include "ZooStringToUIDTable.h"

DECLARE_ALGORITHM_FACTORY(ZooWriter);

using namespace Gaudi::Units;
using namespace std;

namespace {
    std::string _preambulo (const std::vector<std::string>& lines)
    {
	std::string::size_type sz = 0;
	for (std::vector<std::string>::const_iterator iline =
		lines.begin() ; lines.end() != iline ; ++iline) {
	    sz += iline->size() + 1;
	}
	std::string result ;
	result.reserve(sz); // preallocate enough space
	for (std::vector<std::string>::const_iterator iline =
		lines.begin() ; lines.end() != iline ; ++iline) {
	    if ( lines.begin() != iline ) { result += '\n'; }
	    result += (*iline);
	}
	return result;
    }
    inline std::string preambulo()
    { std::vector<std::string> m_p; return _preambulo ( m_p ); }
}


ZooWriter::ZooWriterContext::ZooWriterContext(const std::string& filename,
	const std::string& treename,
	const std::vector<std::string>& sel_collections,
	const std::vector<std::string>& sel_names, ZooWriter* const base) :
    m_perJobMapSz(0), m_evts_begun(0), m_evts_ended(0)
{
    using namespace boost::lambda;
    // open ROOT file
    m_f = new TFile(filename.c_str(), "recreate", "", 8);
    m_f->cd();
    {
	boost::shared_ptr<TTree> t(
		new TTree(treename.c_str(),"Zoo particles tree"),
		_1 ->* &TTree::Delete);
	m_T.swap(t);
	m_T->SetDirectory(m_f);
    }
    {
	boost::shared_ptr<TTree> tperjob(
		new TTree((treename+"_Shrubbery").c_str(),
		    "Zoo per job object tree"),
		_1 ->* &TTree::Delete);
	m_TperJob.swap(tperjob);
	m_TperJob->SetDirectory(m_f);
    }
    // register the per job object tree as friend of the per event one
    m_T->AddFriend(&*m_TperJob);
    {
	boost::shared_ptr<ZooObjectManager> objman(
		new ZooObjectManager(*m_T, *m_TperJob, m_evts_begun));
	m_objectManager.swap(objman);
    }
    {
	boost::shared_ptr<ZooEv> ev(new ZooEv());
	m_pev.swap(ev);
    }
    ///\todo Check what Paul has done here
    m_T->Branch("Event",m_pev->Class()->GetName(), (void**)&(m_pev), 1 << 19, 99);
    m_sel.reserve(sel_collections.size());
    vector<string>::const_iterator j = sel_collections.begin();
    for (vector<string>::const_iterator i = sel_names.begin();
	    i != sel_names.end(); ++i) {
	m_sel.push_back(KnownSelection());
	m_sel.back().name=*i;
	m_sel.back().collection=*j;
	boost::shared_ptr<ZooPArray> pref(new ZooPArray());
	m_sel.back().pref.swap(pref);
	++j;
    } 
    for (vector<KnownSelection>::iterator i = m_sel.begin();
	    i != m_sel.end(); ++i) {
	m_T->Branch(i->name.c_str(),i->pref->Class()->GetName(),(void**)&(i->pref), 1 << 19, 99);
    }
    // finish setting up the event tree
    m_T->BranchRef();
    // set the basket size for all branches to 512 kbytes
    m_T->SetBasketSize("*", 1 << 19);
    // force flushing of buffers every 128 MB
    m_T->SetAutoSave(128 << 20);
    // branch ref for per-job tree as well
    m_TperJob->BranchRef();
    // a pool for Track objects
    boost::shared_ptr<boost::object_pool<LHCb::Track> > trackpool(
	    new boost::object_pool<LHCb::Track>());
    m_trackPool = trackpool;
    // get ROOT object count
    m_objectCount = TProcessID::GetObjectCount();
}

ZooWriter::ZooWriterContext::~ZooWriterContext()
{
    if (m_evts_begun > m_evts_ended) endEvent();
    m_f->cd();
    if (m_TperJob) m_TperJob->Write("",TObject::kOverwrite);
    if (m_T) m_T->Write("",TObject::kOverwrite);
    // if the tuple became too big, ROOT will open a new file for us and
    // close the old one (thus invalidating the pointer we have); therefore
    // we just ask the tree for the file it is currently writing to
    m_f = m_T->GetCurrentFile();
    m_TperJob.reset();
    m_T.reset();
    delete m_f; m_f = 0;
    m_pev.reset();
    m_sel.clear();
    m_objectManager.reset();
    if (m_pvWouter)
	m_pvWouter.reset();
    if (m_trackPool)
	m_trackPool.reset();
}

void ZooWriter::ZooWriterContext::beginEvent()
{
    if (m_evts_begun != m_evts_ended)
	throw ZooWriterContextException("At most one incomplete event permitted!");
    if (0 == m_evts_begun) {
	// fill per job tree at the beginning of first event in job
	m_f->cd();
	// persistify mapping string - per job objects
	TMap* map = new TMap;
	map->SetOwnerKeyValue(kTRUE, kTRUE);
	// the map is also a per-job object
	objman()->zooPerJobObject("ZooPerJobObjects",
		reinterpret_cast<TObject*>(map));
	// set pointer to per job objects in ZooEv
	m_pev->setPerJobObjects(map);
	// fill map with references to per job variables
	for (std::map<std::string, TObject*>::const_iterator
		it = objman()->perJobObjMap().begin();
		objman()->perJobObjMap().end() != it; ++it) {
	    map->Add(new TObjString(it->first.c_str()), new TRef(it->second));
	}
	// save size so we have a chance to tell if people screw with the
	// per-job object map afterwards
	m_perJobMapSz = objman()->perJobObjMapSize();
	// write out a branch for each registered per job object
	for (std::map<std::string, TObject*>::const_iterator
		it = objman()->perJobObjMap().begin();
		objman()->perJobObjMap().end() != it; ++it) {
	    m_TperJob->Branch(it->first.c_str(), it->second, 1 << 16, 99);
	}
	// fill the per-job object tree
	m_TperJob->SetBasketSize("*", 1 << 16);
	m_TperJob->SetAutoSave(128 << 20);
	m_TperJob->Fill();
	m_TperJob->OptimizeBaskets(1 << 16);
    } else {
	if (m_perJobMapSz != objman()->perJobObjMapSize())
	    throw ZooWriterContextException(
		    "Not allowed to change per Job objects after first event!");
    }
    ++m_evts_begun;
    m_objectCount = TProcessID::GetObjectCount();
}

void ZooWriter::ZooWriterContext::endEvent()
{
    if (m_evts_begun != m_evts_ended + 1)
	throw ZooWriterContextException("At most one incomplete event permitted!");
    m_f->cd();
    // commit the event to the tree
    m_T->Fill();

    // clear internal data structures for next event
    for(vector<KnownSelection>::iterator i=m_sel.begin();i!=m_sel.end();++i)
	i->pref->clear();
    m_pev->Clear();
    m_objectManager->Clear();
    // delete emergency PV
    if (m_pvWouter)
	m_pvWouter.reset();
    // clear m_fittedTracks
    if (!m_fittedTracks.empty()) {
	m_fittedTracks.clear();
	// free allocated tracks
	boost::shared_ptr<boost::object_pool<LHCb::Track> > trackpool(
		new boost::object_pool<LHCb::Track>());
	m_trackPool.swap(trackpool);
    }
    TProcessID::SetObjectCount(m_objectCount);
    // optimize buffer sizes after first 250 events
    ++m_evts_ended;
    if (4096 == m_evts_ended) m_T->OptimizeBaskets(1 << 19);
    // make sure we stil are looking at the most recent per-job objects
    m_TperJob->GetEntry(0); 
}

ZooWriter::ZooWriter(const std::string& name, ISvcLocator *svc) :
#if DV_VER < 322
    DVAlgorithm (name,svc), 
#else
    DaVinciTupleAlgorithm (name,svc), 
#endif
    m_context(0),
    m_dist(0),
    m_bkg(0),
    m_TriggerTisTos(0),
#if DV_VER >= 256 // DaVinci >= v25r6
    m_L0TriggerTisTos(0),
#endif
    m_extrapolator (0),
    m_linkerTool_Links (0),
    m_evts (0)
{
    declareProperty( "InputCollections",     m_sel_collections);
    declareProperty( "DecayName" ,           m_sel_names);
    declareProperty( "WriteMC",              m_writeMC = false);
    declareProperty( "WriteMCtrees",         m_writeMCtrees = false);
    declareProperty( "MCList",               m_MCList);
    declareProperty( "WriteDLL",             m_writeDLL = false);
    declareProperty( "IntelligentPV",        m_intelligentPV = false);
    declareProperty( "SecondIpSig",          m_secondIpSig = false);
    declareProperty( "OnlyTreefitter",       m_onlyTreefitter = false);
    declareProperty( "MinTracksPV",          m_minTracksPV = 0);

    declareProperty( "TaggingList",          m_taggingList);
    declareProperty( "BackgroundList",       m_backgroundList);
    declareProperty( "TriggerList",          m_triggerList);
    // set defaults
    std::vector<std::string> L0Name(1, "Hlt1L0.*Decision");
    std::vector<std::string> Hlt1Name(1, "Hlt1.*Decision");
    std::vector<std::string> Hlt2Name(1, "Hlt2.*Decision");
    declareProperty( "L0SelectionName" ,     m_L0Name = StringArrayProperty(L0Name));
    declareProperty( "HLT1SelectionName" ,   m_Hlt1Name = StringArrayProperty(Hlt1Name));
    declareProperty( "HLT2SelectionName" ,   m_Hlt2Name = StringArrayProperty(Hlt2Name));

    declareProperty( "Filename"   ,          m_filename="Zoo.root");
    declareProperty( "Tree"       ,          m_treename="Forest");
    
    declareProperty( "CovarianceList",       m_covarianceList);

    declareProperty( "TreefitName",     m_treefit_names);
    declareProperty( "TreefitMassConstraints", m_treefit_constraints);

    // the following switches should only be touched by experts ....
    declareProperty( "PVReFitterName",       m_PVReFitterName = "AdaptivePVReFitter");
    declareProperty( "LinkToList",           m_linkToList);
    declareProperty( "LinkFromList",         m_linkFromList);
    declareProperty( "WriteHitPattern",      m_writeHitPattern = false);
    declareProperty( "WriteExpectedHitPattern",  m_writeExpectedHitPattern = false);
    declareProperty( "WriteCollectedHitPattern", m_writeCollectedHitPattern = false);
    declareProperty( "ExtraInfoList",	     m_extraInfoList);
    declareProperty( "WriteTrackInfo",       m_writeTrackInfo = false);
    declareProperty( "PackedStatesList",     m_packedStatesList);
    declareProperty( "PackedStatesExtrapolate",
	    m_packedStatesExtrapolate = true);
    declareProperty( "PackedStatesZList",    m_packedStatesZList);
    declareProperty( "PackedStateAtPocaToZAxis", m_packedStateAtPocaToZAxis = false);
    declareProperty( "MakeEmergencyPV",      m_makeEmergencyPV = false);
    declareProperty( "WriteOccupancies",     m_writeOccupancies = false);
    declareProperty( "WriteLHCbIDs",         m_writeLHCbIDs = false);
    declareProperty( "WriteMCGenEventInfo",  m_writeMCGenEventInfo = false);
    declareProperty( "MCAcceptanceEtaRegion", m_mcAcceptanceEtaRegion = boost::assign::list_of(2.)(5.));
    declareProperty( "MCAcceptanceMinP",     m_mcAcceptanceMinP = 1000. * Gaudi::Units::MeV);
    declareProperty( "MCAcceptanceCharged",  m_mcAcceptanceCharged = true);
    declareProperty( "MCAcceptanceNeutral",  m_mcAcceptanceNeutral = false);
    declareProperty( "MCAcceptanceMinCtau",  m_mcAcceptanceMinCtau = 50. * Gaudi::Units::cm);
    declareProperty( "ParticleInfoList",     m_particleinfoList);
    declareProperty( "LoKiFunctors",         m_lokinames);
}

StatusCode ZooWriter::initialize  ()
{
    
#if DV_VER < 322
    DVAlgorithm::initialize();
#else
    DaVinciTupleAlgorithm::initialize();
#endif

    m_parts.clear();
    for (unsigned k = 0 ; k < m_sel_names.value().size() ; ++k)
      m_parts.push_back(0);

    m_dist = distanceCalculator();
    m_pvReFitter = tool<IPVReFitter>(m_PVReFitterName, this );
    m_lifetimeFitter = tool<ILifetimeFitter>( "LoKi::LifetimeFitter", this);
    m_TriggerTisTos = tool<ITriggerTisTos>( "TriggerTisTos",this);
#if DV_VER >= 256
    m_L0TriggerTisTos = tool<ITriggerTisTos>( "L0TriggerTisTos",this);
#endif
    m_bkg = tool<IBackgroundCategory>( "BackgroundCategory", this );
    m_linkerTool_Links = tool<IDaVinciAssociatorsWrapper>("DaVinciAssociatorsWrapper","Wrapper_Links",this);
    m_odinDecoder = tool<IEventTimeDecoder>("OdinTimeDecoder");
    m_extrapolator = tool<ITrackExtrapolator>("TrackMasterExtrapolator", this);
    m_ghostClassification = tool<ITrackGhostClassification>("LongGhostClassification",this);
    m_visPrimVertTool = tool<IVisPrimVertTool>("VisPrimVertTool");
    m_hitCollector = tool<ITrackHitCollector>("TrackHitCollector", this);
    m_expectedHitsTool = tool<TrackExpectedHitsTool>("TrackExpectedHitsTool", this);
    m_trackFitter = tool<ITrackFitter>("TrackInitFit");
    m_hitManagerT = tool<Tf::TStationHitManager<PatForwardHit> >(
	    "PatTStationHitManager");
    m_hitManagerTT = tool<Tf::TTStationHitManager <PatTTHit> >(
	    "PatTTStationHitManager");
    m_hitManagerVeloR = tool<Tf::DefaultVeloRHitManager>(
	    "Tf::DefaultVeloRHitManager");
    m_hitManagerVeloPhi = tool<Tf::DefaultVeloPhiHitManager>(
	    "Tf::DefaultVeloPhiHitManager");

    m_magFieldSvc = svc<ILHCbMagnetSvc>( "MagneticFieldSvc", true );
    
    // write covariance list and packed states list are needed sorted
    std::sort(m_covarianceList.begin(), m_covarianceList.end());
    std::sort(m_packedStatesList.begin(), m_packedStatesList.end());

    // make sure selection/collection names make sense  
    if (m_sel_names.value().empty()) {
        error() << "No selections are specified to be saved!" << endmsg;
        return StatusCode::FAILURE;
    }
    if (m_sel_names.value().size() != m_sel_collections.value().size()) {
        error() <<  "The list of particle collections and names "
            "to store do not have the same length!" << endmsg;
        return StatusCode::FAILURE;
    }
    if ((m_linkFromList.value().empty() && !m_linkToList.value().empty()) ||
	    (!m_linkFromList.value().empty() && m_linkToList.value().empty())) {
	error() << "Cannot have one of LinkedFromList and LinkedToList empty "
	    "and the other filled!" << endmsg;
	return StatusCode::FAILURE;
    }
    // check that each particle collection is only linked once
    BOOST_FOREACH(const std::string& str,  m_linkFromList.value()) {
	if (std::count(m_linkFromList.value().begin(),
		    m_linkFromList.value().end(), str) != 1) {
	    error() << "Particle collection " << str << " must appear at most "
		"once in LinkFromList!" << endmsg;
	    return StatusCode::FAILURE;
	}
    }
    BOOST_FOREACH(const std::string& str,  m_linkToList.value()) {
	if (std::count(m_linkToList.value().begin(),
		    m_linkToList.value().end(), str) != 1) {
	    error() << "Particle collection " << str << " must appear at most "
		"once in LinkToList!" << endmsg;
	    return StatusCode::FAILURE;
	}
    }

    // Check if user provided sensible "in acceptance" criteria
    if (m_mcAcceptanceEtaRegion.size() != 2 ||
	    m_mcAcceptanceEtaRegion[0] >= m_mcAcceptanceEtaRegion[1]) {
	error() << "You must provide an eta acceptance region in the format [etamin, etamax]!" <<
	    endmsg;
	return StatusCode::FAILURE;
    }
    if (m_mcAcceptanceMinP < 0.) {
	error() << "MCAcceptanceMinP must be non-negative!" << endmsg;
	return StatusCode::FAILURE;
    }
    if (m_mcAcceptanceMinCtau < 0.) {
	error() << "MCAcceptanceMinCtau must be non-negative!" << endmsg;
	return StatusCode::FAILURE;
    }

    // create ZooWriterContext
    boost::shared_ptr<ZooWriterContext> ctx(
	    new ZooWriterContext(m_filename, m_treename,
		m_sel_collections.value(), m_sel_names.value(), this));
    m_ctx.swap(ctx);

    LoKi::IHybridFactory* factory = tool<LoKi::IHybridFactory>(
	    "LoKi::Hybrid::Tool/HybridFactory:PUBLIC", this);

    objman()->zooPerJobObject<ZooStringToUIDTable>(
	    std::string("ZooLoKiBlockUIDTable"),
	    ZooStringToUIDTable(m_lokinames.value()));
    ZooStringToUIDTable* lokitable =
	static_cast<ZooStringToUIDTable*>(
		objman()->zooPerJobObject(std::string("ZooLoKiBlockUIDTable")));
    BOOST_FOREACH(const std::string& funname, m_lokinames.value()) {
      LoKi::PhysTypes::Fun functor(
	      LoKi::BasicFunctors<const LHCb::Particle*>::Constant(
		  std::numeric_limits<double>::quiet_NaN()));
      if (factory->get(funname, functor, preambulo()).isFailure()) {
	  warning() << "Unable to create LoKi functor '" << funname <<
	      "', ignoring this one..." << std::endl;
	  continue;
      }
      UInt_t hash = lokitable->hash(funname);
      m_lokifuns.push_back(std::make_pair(hash, functor));
    }
    release(factory);
    // write Option string to per-job tree
    {
	typedef std::vector<Property*> Properties;
	const Properties& properties = this->getProperties();
	// hash map to contain string -> string lookup table
	// use standard ROOT objects here, nothing fancy, since we want to be
	// sure we can read this beast even if the Zoo data format changes
	// (that's also the reason we convert everything to strings)
	objman()->zooPerJobObject("ZooOptions",
		dynamic_cast<TObject*>(new TMap(8 + properties.size())));
	TMap* options = dynamic_cast<TMap*>(
		objman()->zooPerJobObject("ZooOptions"));
	// table owns its elements
	options->SetOwnerKeyValue(kTRUE, kTRUE);
	// put in Zoo/DaVinci version, ZooWriter instance name
	options->Add(new TObjString("Zoo version"),
		new TObjString("$Id$"));
	options->Add(new TObjString("Zoo instance name"),
		new TObjString(name().c_str()));
	// we need some preprocessor magic to make DV_VER into a string
#define _STR(x) #x // stringify argument
#define STR(x) _STR(x) // expand x and stringify
	options->Add(new TObjString("DaVinci version"),
		new TObjString(STR(DV_VER)));
#undef STR
#undef _STR
	// dump all properties probably more than we'll ever need, but we
	// won't have to think of updating the list of stuff to dump - we add
	// a new property to the ZooWriter, and it's added automatically
	BOOST_FOREACH(const Property* property, properties) {
	    options->Add(new TObjString(property->name().c_str()),
		    new TObjString(property->toString().c_str()));
	}
    }

    return StatusCode::SUCCESS;
}

StatusCode ZooWriter::finalize()
{
    always() << "==== ZooWriter summary ====" << endmsg;
    always() << m_evts << " events processed" << endmsg;
    for (unsigned k = 0 ; k < m_sel_names.value().size() ; ++k) {
      always() << m_parts[k] << " particles of decay " << m_sel_names.value()[k] << endmsg;
    }
    // release context
    m_ctx.reset();

#if DV_VER < 322
    return DVAlgorithm::finalize();
#else
    return DaVinciTupleAlgorithm::finalize();
#endif

}

ZooWriter::~ZooWriter () { }

void ZooWriter::addLoKi(ZooP* p,const LHCb::Particle* part)
{
    if (m_lokifuns.empty()) return;

    ZooStringToUIDTable* table = static_cast<ZooStringToUIDTable*>(
	    objman()->zooPerJobObject(std::string("ZooLoKiBlockUIDTable")));
    ZooLoKiBlock::KeyValueVector eivector;
    eivector.reserve(m_lokifuns.size());
    for (unsigned i = 0 ; i < m_lokifuns.size() ; ++i) {
	const float val = m_lokifuns[i].second(part);
	if (std::isnan(val)) continue;
	eivector.push_back(std::make_pair(m_lokifuns[i].first, Float_t(val)));
    }

    ZooLoKiBlock*& loki = objman()->getOrCreateMappingFor<ZooLoKiBlock>(part);
    if (loki) {
	loki->insert(eivector.begin(), eivector.end());
    } else {
	loki = p->AddInfo<ZooLoKiBlock>(*objman(),
		ZooLoKiBlock(*table, eivector));
    }
}

StatusCode ZooWriter::execute()
{
    //PhysDesktop no longer available for DV ver >=v27r1
#if DV_VER < 271
    if (!desktop()->cleanDesktop() || !desktop()->getEventInput()) {
	error() << "Access to desktop failed!" << endmsg;
	return StatusCode::FAILURE;
    }  
#endif
    setFilterPassed(true);
    m_evts++;

    // make sure event is started and written out properly
    EventGuard myEvent(m_ctx);

    // write the Zoo event header itself
    writeEvent();

    // write MC truth (if applicable)
    if (m_writeMC) {
	m_mcLink = m_linkerTool_Links->linker(
		Particle2MCMethod::Links, m_sel_collections.value());
	writeMC();
    }

    // write selected particle collections
    unsigned selectionid = -1;
    BOOST_FOREACH(ZooWriterContext::KnownSelection& s, sel()) {
  ++selectionid;
	if ( !exist<LHCb::Particle::Range>( s.collection ) ) continue;
	LHCb::Particle::Range parts = get<LHCb::Particle::Range>( s.collection );
	BOOST_FOREACH(const LHCb::Particle* const part, parts) {
      m_parts[selectionid]++;
      ZooP* zp = GetSaved(part);
	    s.pref->push_back(zp);
      addLoKi(zp,part);
	}
    }
    
    //decay tree fitter
    BOOST_FOREACH(ZooWriterContext::KnownSelection& s, sel()) {
      if ( !exist<LHCb::Particle::Range>( s.collection ) ) continue;
      if (std::find(m_treefit_names.value().begin(), m_treefit_names.value().end(), s.name) != m_treefit_names.value().end())
	{
	  LHCb::Particle::Range parts = get<LHCb::Particle::Range>( s.collection );
	  BOOST_FOREACH(const LHCb::Particle* const part, parts) {
	    treeFit(part);
	  }
      }
    }

    // write links 
    if (!m_linkFromList.value().empty()) writeLinks();

    return StatusCode::SUCCESS;
}


void ZooWriter::setTreefitMassConstraint(const LHCb::Particle* node, DecayTreeFitter::Fitter* treefitter)
{
  //the user has to set the constraints him/herself
  for (unsigned i = 0; i < m_treefit_constraints.size(); ++i) {
    //constrain particle mass if it is in the list
    if (std::abs(m_treefit_constraints[i]) == std::abs(node->particleID().pid()))
      treefitter->setMassConstraint(*node);
  }
  const SmartRefVector<LHCb::Particle> &ch = node->daughters();
  for (SmartRefVector<LHCb::Particle>::const_iterator i = ch.begin(); i != ch.end(); ++i) 
  {
    setTreefitMassConstraint(*i, treefitter);//traverse the tree
  }
};


void ZooWriter::getTreefitParams(const LHCb::Particle* node, DecayTreeFitter::Fitter* treefitter)
{
  //additional variables for the decaytree fitter
  double mass = std::numeric_limits<double>::quiet_NaN();
  double massErr = std::numeric_limits<double>::quiet_NaN();
  double ct = std::numeric_limits<double>::quiet_NaN();
  double ctErr = std::numeric_limits<double>::quiet_NaN();
  double ctFixed = std::numeric_limits<double>::quiet_NaN();
  double ctErrFixed = std::numeric_limits<double>::quiet_NaN();
  double fD = std::numeric_limits<double>::quiet_NaN();
  double fDErr = std::numeric_limits<double>::quiet_NaN();

  //extract all params
  const Gaudi::Math::ParticleParams* params = treefitter->fitParams( node );
  const Gaudi::Math::LorentzVectorWithError& mom = params->momentum();
  //extract mass
  mass = mom.mass().value();
  massErr = mom.mass().error();
  //extract lifetime (without mass constraint)
  ct = params->ctau().value();
  ctErr = params->ctau().error();
  const double factor = 1.0e+6/299792458.0;//this should give ns, same as bs->ct()
  ct *= factor;//TODO check!
  ctErr *= factor;

  //extract lifetime (with mass constraint)
  if (node->isBasicParticle() || fabs(massErr) < 1.0e-3)//do we already have a mass constraint?
    {
      ctFixed = ct;
      ctErrFixed = ctErr;
    }
  else//set a mass constraint
  {
    const LHCb::IParticlePropertySvc* ppsvc = ppSvc();
    const LHCb::ParticleID& pid = node->particleID();
    const LHCb::ParticleProperty* pp = ppsvc->find(pid);
    double mass = pp->mass();
    double chi2 = -1000.0;
    Gaudi::Math::ParticleParams params_fixed = Gaudi::Math::FitMass::fit(*params, mass, chi2);
    ctFixed = params_fixed.ctau().value();
    ctErrFixed = params_fixed.ctau().error();
    ctFixed *= factor;
    ctErrFixed *= factor;  
  }  
  fD = params->decayLength().value();
  fDErr = params->decayLength().error();

  //extract chi2
  short ndof = treefitter->nDof();
  double chi2 = treefitter->chiSquare();

  //extract covariance matrix  
  Gaudi::SymMatrix7x7 cov;
  cov.Place_at(params->posCovMatrix(),0,0);
  cov.Place_at(params->momCovMatrix(),3,3);
  for(int imom =0; imom<4; ++imom)
    for(int ipos=0 ; ipos<3; ++ipos)
      cov(imom+3,ipos) = params->momPosCov()(imom,ipos);

  //write ZooTreefitInfo info block
  ZooP * zp = GetSaved(node);
  if (zp != 0)
    zp->AddInfo<ZooTreefitInfo>(*objman(),
                                ZooTreefitInfo(params->position(), params->momentum(), cov, 
                                               mass, massErr,
                                               fD, fDErr, 
                                               ct, ctErr, 
                                               ctFixed, ctErrFixed, 
                                               chi2, ndof));
  
  //traverse the tree
  const SmartRefVector<LHCb::Particle> &ch = node->daughters();
  for (SmartRefVector<LHCb::Particle>::const_iterator i = ch.begin();
       i != ch.end(); ++i) {
    getTreefitParams(*i, treefitter);
  }
}

void ZooWriter::treeFit(const LHCb::Particle* p) {
#if DV_VER < 271
  const LHCb::RecVertex* pv =
    dynamic_cast<const LHCb::RecVertex*>(desktop()->relatedVertex(p));
#endif
#if DV_VER >= 271
//  const LHCb::RecVertex* pv =
//    dynamic_cast<const LHCb::RecVertex*>(this->getRelatedPV(p));
  const LHCb::RecVertex* pv =
    dynamic_cast<const LHCb::RecVertex*>(this->bestPV(p));
#endif

  //do the tree fit
  if (pv) {
    LHCb::RecVertex newPV(*pv);
    if (m_pvReFitter->remove(p, &newPV)) {
      //decaytree fitter
      DecayTreeFitter::Fitter treefitter( *p, newPV ) ;
      //apply mass constraints
      setTreefitMassConstraint(p, &treefitter);
      //perform fit
      treefitter.fit();
      //extract all fitted quantities
      getTreefitParams(p, &treefitter);
    }
  }
}

ZooP *ZooWriter::GetSaved(const LHCb::Particle* const p)
{
    ZooP *&zp = objman()->getOrCreateMappingFor<ZooP>(p);
    if (zp) return zp;
    zp = objman()->zooObj<ZooP>();

    ZooParticleInfo::KeyValueVector values;
    for (unsigned k = 0 ; k < m_particleinfoList.size() ; ++k) {
      double info = p->info(m_particleinfoList[k], std::numeric_limits<double>::quiet_NaN());

      if (!std::isnan(info))
         values.push_back(std::make_pair(UInt_t(m_particleinfoList[k]), Float_t(info)));
    }
    ZooParticleInfo*& pi = objman()->getOrCreateMappingFor<ZooParticleInfo>(p);
    if (pi)
      zp->AssignInfo<ZooParticleInfo>(pi);
    else {
      pi = zp->AddInfo<ZooParticleInfo>(*objman(),
            ZooParticleInfo(values));
    }

    const int abspid = std::abs(p->particleID().pid());

    const bool writeCovariance =
      !m_covarianceList.empty() &&
      (m_covarianceList.front() < 0 ||
       m_covarianceList.end() !=
       std::find(m_covarianceList.begin(),
		 m_covarianceList.end(), abspid));
    
    const Gaudi::XYZPoint nanv3(
	    std::numeric_limits<double>::quiet_NaN(),
	    std::numeric_limits<double>::quiet_NaN(),
	    std::numeric_limits<double>::quiet_NaN());
  
    const Gaudi::SymMatrix3x3 nanm3(
	    -Gaudi::SymMatrix3x3(ROOT::Math::SMatrixIdentity()));
    
    const Gaudi::SymMatrix7x7 nanm7(
	    -Gaudi::SymMatrix7x7(ROOT::Math::SMatrixIdentity()));
   
    const double nan = std::numeric_limits<double>::quiet_NaN();

    zp->m_pid = p->particleID().pid();
    
    // get PV
#if DV_VER < 271
    const LHCb::VertexBase *pv = desktop()->relatedVertex(p);
#endif
#if DV_VER >= 271
    //const LHCb::VertexBase *pv = this->getRelatedPV(p);
    const LHCb::VertexBase *pv = this->bestPV(p);
#endif

    const LHCb::RecVertex* bestpv = 0;
    if (!p->daughters().empty() && (m_intelligentPV || m_secondIpSig)) 
    {
	//this method intelligently selects the best PV AFTER PV refit
	//best PV is currently the PV with the smallest ip sig, provided the lifetime fit works
	
	//double best_chi2 = 999.0;
	double best_ipSig = 999.0;
	double best_ip = 999.0;
	double secondbest_ip = 999.0;//this should be saved, too
	double secondbest_ipSig = 999.0;//this should be saved, too

	double curr_ct, curr_ctErr, curr_chi2;
	double curr_ip, curr_ipChi2, curr_ipSig;
	const LHCb::RecVertex::Range vtcs = primaryVertices();
	BOOST_FOREACH(const LHCb::VertexBase* vtx, vtcs) {
	    if (!vtx->isPrimary()) continue;
	    //newPV
	    const LHCb::RecVertex* currpv =
		dynamic_cast<const LHCb::RecVertex*>(vtx);
	    LHCb::RecVertex newpv(currpv  ? (*currpv) : LHCb::RecVertex());
	    if (m_pvReFitter->remove(p, &newpv))
	    {	    
		bool treefit_ok = true;
		if (m_onlyTreefitter)
		{
		    DecayTreeFitter::Fitter treefitter( *p, newpv ) ;
		    treefitter.fit();//no mass constraints here
		    if (treefitter.status() != 0)
			treefit_ok = false;
		}

		//can additionally specify minimum number of tracks
		if (int(newpv.tracks().size()) > m_minTracksPV 
			&& m_dist->distance(p, &newpv, curr_ip, curr_ipChi2)
			&& (m_onlyTreefitter ?
			    treefit_ok
			    : (m_lifetimeFitter->fit(newpv, *p, curr_ct, curr_ctErr, curr_chi2) == 1)
			   )
		   )
		{
		    //selection according to smallest lifetime fit chi2
		    /*
		       if (curr_chi2 < chi2)
		       {
		       bestpv = currpv;
		       best_chi2 = curr_chi2;		      
		       }
		       */		
		    curr_ipSig = sqrt(curr_ipChi2);//negative ip chi2???
		    //selection according to smallest ipsig (similar to relatedvertex)
		    if (curr_ipSig < best_ipSig && !isnan(curr_ipSig))
		    {
			bestpv = currpv;
			secondbest_ipSig = best_ipSig;//save second best (old) values
			secondbest_ip = best_ip;
			best_ipSig = curr_ipSig;//set new best values
			best_ip = curr_ip;
		    }
		}	    
	    }
	}
	if (m_secondIpSig)
	{
	    zp->m_second_ip = secondbest_ip;
	    zp->m_second_ipSig = secondbest_ipSig;
	}
	if (m_intelligentPV)
	    pv = bestpv;
    }



    // fall back onto emergency PV if we have nothing better...
    if (0 == pv && pvWouter())
	pv = pvWouter().get();

    zp->m_particle =
      ZooPackedParticle(p->referencePoint(), p->momentum(),
			writeCovariance?p->covMatrix():nanm7);
    
    zp->AssignInfo<ZooPackedVertex>(writePackedVertex(pv, 
						      writeCovariance, false));

    const SmartRefVector<LHCb::Particle> &ch = p->daughters();
    
    if (ch.empty()) {
    
      // stable particle
     if(0 != p->charge()) 
      zp->AddInfo<ZooStable>(*objman(),
			     ZooStable(refittedTrack(p->proto()->track())->type(), p->charge()));
     else 
      zp->AddInfo<ZooStable>(*objman(),
			     ZooStable(0, p->charge()));
     
      if ( (0 != p->charge()) && (m_writeTrackInfo || !m_packedStatesList.empty()
	   || m_writeHitPattern || m_writeExpectedHitPattern ||
	   m_writeCollectedHitPattern || !m_extraInfoList.empty()))
	writeTrackInfo(zp,p);
          
      if (m_writeDLL) writeDLL(zp,p);
      
      double IP = nan, chi2 = nan;
      if (pv) {
	if (!m_dist->distance(p, pv, IP, chi2)) {
	  IP = chi2 = nan;
	} else
	  chi2 = std::sqrt(chi2);
      }
      zp->m_ip           = IP;
      zp->m_ipSig        = chi2;
    
    } else {
    
      // decay product, save children      
      for (SmartRefVector<LHCb::Particle>::const_iterator i = ch.begin();
	   i != ch.end(); ++i) {
	ZooP *zc=GetSaved(*i);
	zc->AddMother(zp);
	zp->AddChild(zc);
      }
      
      double fD = nan, fDErr = nan, fDChi2 = nan;
      double ct = nan, ctErr = nan, ctChi2 = nan;
      double IP = nan, chi2 = nan, IPSig = nan;
      int isolation = -999;
      
      // work out flight dist, proper time etc from PV with particle removed
      if (pv) {
	  const LHCb::RecVertex* rpv =
	      dynamic_cast<const LHCb::RecVertex*>(pv);
	  LHCb::RecVertex newPV(rpv?(*rpv):LHCb::RecVertex());

	  if (rpv && (m_intelligentPV || m_pvReFitter->remove(p, &newPV))) {//do not have to refit if we use intelligentPV
	      // the fitters clobber their output in case of failure
	      if (!m_dist->pathDistance(p, &newPV, fD, fDErr, fDChi2))
		  fD = fDErr = fDChi2 = nan;
	      //Use DTF for the lifetime fit without mass constraint if requested
	      if (m_onlyTreefitter)
	      {
		  DecayTreeFitter::Fitter treefitter( *p, newPV ) ;
		  treefitter.fit();//no mass constraints here
		  const Gaudi::Math::ParticleParams* params = treefitter.fitParams(p);
		  ct = params->ctau().value();//this is probably not in ps TODO
		  ctErr = params->ctau().error();
		  //const double factor = 1.0e+12/1000.0/299792458.0;//this would give ps
		  const double factor = 1.0e+6/299792458.0;//this should give ns, same as bs->ct()
		  ct *= factor;
		  ctErr *= factor;
		  ctChi2 = treefitter.chiSquare();
		  if (treefitter.status() != 0)
		      ct = ctErr = ctChi2 = nan;
	      }
	      //Else use standard offline Vertex fitter
	      else
	      {
		  if (!m_lifetimeFitter->fit(newPV,*p,ct,ctErr,ctChi2))
		      ct = ctErr = ctChi2 = nan;
	      }
	      if (!m_dist->distance(p, &newPV, IP, chi2))
		  IP = IPSig = nan;
	      else
		  IPSig = std::sqrt(chi2);
	  }
      }

      zp->m_ip	= IP;
      zp->m_ipSig	= IPSig;
      
      if (std::find(m_taggingList.begin(), m_taggingList.end(), abspid) !=
	  m_taggingList.end())
	writeTagging(zp, p);
      
      if (std::find(m_backgroundList.begin(), m_backgroundList.end(), abspid) !=
	  m_backgroundList.end()) {
	ZooMCBkg* bkg = zp->AddInfo<ZooMCBkg>(*objman());
	bkg->m_category = (int)(m_bkg->category( p ));
	bkg->m_flags = getMCBkgFlags(p);
      }

      // write ZooDecay info block
      const LHCb::Vertex *v = p->endVertex();
      zp->AddInfo<ZooDecay>(*objman(),
			    ZooDecay(p->measuredMass(), p->measuredMassErr(),
				     fD, fDErr, fDChi2, ct, ctErr, ctChi2,
				     v?v->chi2():-1., isolation, v?v->nDoF():-1));
    }
    
    if (std::find(m_triggerList.begin(), m_triggerList.end(), abspid) !=
	m_triggerList.end())
      writeTrigger(zp,p);
    
    
    if( m_writeMC ) {
	// is p a stable particle? if so, then use linker tool
	// otherwise, get the mc particle from background tool
	const LHCb::MCParticle* mcp =
	    p->isBasicParticle() ? m_mcLink->firstMCP(p) : m_bkg->origin(p);
	ZooMCP* zmcp = (0 == mcp)?reinterpret_cast<ZooMCP*>(0):
	    GetSaved(mcp);
	if (zmcp)
	    zp->AssignInfo(zmcp);

  if (p->isBasicParticle() && m_writeMCtrees && mcp) {
    const LHCb::MCParticle* buffer = mcp;
    while (buffer->mother())
      buffer = buffer->mother();
    GetSaved(buffer);
  }
	
	ZooGhostCategory * ghostCat = zp->AddInfo<ZooGhostCategory>(*objman());
	ghostCat->m_ghostCategory = 0;
  // don't ask for ghost classification in case of neutral particle
	if ( p->isBasicParticle() && !zmcp && p->proto() && p->proto()->track()){
	    LHCb::GhostTrackInfo gInfo;
	    m_ghostClassification->info(*(p->proto()->track()),gInfo);
	    ghostCat->m_ghostCategory = (gInfo.classification());
	}
	zp->AssignInfo(ghostCat);
    }
    
    return zp;
}

void ZooWriter::writeTagging(ZooP* zp, const LHCb::Particle* p)
{
    LHCb::FlavourTag theTag;

    if(!flavourTagging()->tag(theTag,p))
	warning() << "Flavour Tagging failed" << endmsg;

    std::vector<LHCb::Tagger> taggers = theTag.taggers();

    ZooTagging* tag = zp->AddInfo<ZooTagging>(*objman());

    tag->setTagDecision(theTag.decision(), theTag.omega());
    tag->setTagCategory(theTag.category());
    tag->setOsTagDecision(theTag.decisionOS(), theTag.omegaOS());
    tag->setOsTagCategory(theTag.categoryOS());

    for(std::vector<LHCb::Tagger>::iterator itag=taggers.begin();
	    itag!=taggers.end(); ++itag) {
	switch(itag->type()) {
	    case 2:
		tag->setMuonTagDecision(itag->decision(), itag->omega());
		break;
	    case 3:
		tag->setElecTagDecision(itag->decision(), itag->omega());
		break;
	    case 4:
		tag->setOsKaonTagDecision(itag->decision(), itag->omega());
		break;
    // Same Side Kaon Tagger
	    case 5:
		tag->setSsKaonTagDecision(itag->decision(), itag->omega());
                if (1 == itag->taggerParts().size())
                   tag->setSsKaon(GetSaved(itag->taggerParts().at(0)));
		break;
    // Same Side Pion Tagger
	    case 6:
		tag->setSsPionTagDecision(itag->decision(), itag->omega());
		//if (1 == itag->taggerParts().size())
		//    tag->setSsKaon(GetSaved(itag->taggerParts().at(0)));
		break;
	    case 10:
		tag->setVtxTagDecision(itag->decision(), itag->omega());
		break;
	    default:
		// FIXME: Do we need a warning here? - MS
		//warning() << "Unknown tag seen!" << endreq;
		break;
	}
    }
}

void ZooWriter::writeEvent()
{

//For DaVinci < v25r5  
#if DV_VER < 255
  // write primary vertices
    const LHCb::RecVertices* vtcs = primaryVertices();
    BOOST_FOREACH(const LHCb::VertexBase* vtx, *vtcs) {
	if (!vtx->isPrimary()) continue;
	ZooPackedVertex *vertex = writePackedVertex(vtx, true, true);
	zooev()->m_pvs.push_back(vertex);
    }
    // get emergency PV if we do not have a PV
    if (vtcs->empty() && m_makeEmergencyPV) {
	getPVWouter();
	if (pvWouter()) {
	    ZooPackedVertex *vertex = writePackedVertex(pvWouter().get(), true, true);
	    zooev()->m_pvs.push_back(vertex);
	    zooev()->m_usingEmergencyPV = true;
	}
    }
#endif
//For DaVinci > v25r5  
#if DV_VER >= 255
    // write primary vertices
    const LHCb::RecVertex::Range vtcs = primaryVertices();
    BOOST_FOREACH(const LHCb::VertexBase* vtx, vtcs) {
	if (!vtx->isPrimary()) continue;
	ZooPackedVertex *vertex = writePackedVertex(vtx, true, true);
	zooev()->m_pvs.push_back(vertex);
    }
    // get emergency PV if we do not have a PV
    if (vtcs.empty() && m_makeEmergencyPV) {
	getPVWouter();
	if (pvWouter()) {
	    ZooPackedVertex *vertex = writePackedVertex(pvWouter().get(),
		    true, true);
	    zooev()->m_pvs.push_back(vertex);
	    zooev()->m_usingEmergencyPV = true;
	}
    }
#endif
    // write track & MC PV multiplicity
    #if DV_VER >= 293
    /// lookup of Rec/Track/Best on MDST causes error (even with exist
    /// statement). Go for the RecSummary first.
    if (exist<LHCb::RecSummary>(LHCb::RecSummaryLocation::Default,false)) {
      // we seem to read an mdst ... the info should be in the rec-header-something
      LHCb::RecSummary* recsum = get<LHCb::RecSummary>(LHCb::RecSummaryLocation::Default,false);
      zooev()->m_trackmult = recsum->info(LHCb::RecSummary::nTracks,-999);

      //// the recsummary uses a different definition than we did so far
      //// in the rec summary the number of tracks with velo segment is stored
      //zooev()->m_nTracksVelo = recsum->info(LHCb::RecSummary::nVeloTracks,-999);

      zooev()->m_nTracksVelo = recsum->info(LHCb::RecSummary::nVeloTracks,-999)
                              -recsum->info(LHCb::RecSummary::nUpstreamTracks,999)
                              -recsum->info(LHCb::RecSummary::nLongTracks,999);

      zooev()->m_nTracksLong = recsum->info(LHCb::RecSummary::nLongTracks,-999);
      zooev()->m_nTracksUpstream = recsum->info(LHCb::RecSummary::nUpstreamTracks,-999);
      zooev()->m_nTracksDownstream = recsum->info(LHCb::RecSummary::nDownstreamTracks,-999);
      zooev()->m_nTracksT = recsum->info(LHCb::RecSummary::nTTracks,-999);
      zooev()->m_nTracksVeloBackward = recsum->info(LHCb::RecSummary::nBackTracks,-999);
    } else if (exist<LHCb::Tracks>(LHCb::TrackLocation::Default)) {
    #else
      if (exist<LHCb::Tracks>(LHCb::TrackLocation::Default)) {
    #endif
      LHCb::Tracks* alltracks;
      alltracks = get<LHCb::Tracks>(LHCb::TrackLocation::Default);
    
    zooev()->m_trackmult = alltracks->size();
    BOOST_FOREACH(const LHCb::Track* ptr, *alltracks) {
	switch (ptr->type()) {
	    default:
		// unknown track type; ignore
		break;
	    case LHCb::Track::Velo:
		++(zooev()->m_nTracksVelo);
		if (ptr->checkFlag(LHCb::Track::Backward))
		    ++(zooev()->m_nTracksVeloBackward);
		break;
	    case LHCb::Track::Long:
		++(zooev()->m_nTracksLong);
		break;
	    case LHCb::Track::Upstream:
		++(zooev()->m_nTracksUpstream);
		break;
	    case LHCb::Track::Downstream:
		++(zooev()->m_nTracksDownstream);
		break;
	    case LHCb::Track::Ttrack:
		++(zooev()->m_nTracksT);
		break;
	}
    }
    }
    

    if (m_writeMC) {
	zooev()->m_nbMCPVs = m_visPrimVertTool->countVertices();
    } else {
	zooev()->m_nbMCPVs = -1;
    }

    // set links to particle and mc particle containers in the event
    zooev()->SetP(objman()->getBankRef<ZooP>());
    zooev()->SetMCP(objman()->getBankRef<ZooMCP>());
    // get generator level event info
    if (m_writeMC && m_writeMCGenEventInfo) {
	LHCb::GenHeader* gene =
	    get<LHCb::GenHeader>(evtSvc(), LHCb::GenHeaderLocation::Default);
	std::vector<Short_t> processTypes;
	processTypes.reserve(gene->collisions().size());
	for (SmartRefVector<LHCb::GenCollision>::const_iterator
		ic = gene->collisions().begin(); gene->collisions().end() != ic; ++ic) {
	    if (*ic)
		   processTypes.push_back((*ic)->processType());
	}
	zooev()->setMCGenEventInfo(objman()->zooObj<ZooMCGenEventInfo>(
		    ZooMCGenEventInfo(processTypes)));
    }

    // write Info on MC track multiplicity in the acceptance
    if (m_writeMC) { 
	// get MC Particles and particle property service
	const LHCb::MCParticles* mcParts =
	    get<LHCb::MCParticles>( LHCb::MCParticleLocation::Default );
	const LHCb::IParticlePropertySvc* ppsvc = ppSvc();

	unsigned nMCPartInAcceptance = 0;

	BOOST_FOREACH(const LHCb::MCParticle* part, *mcParts) {
	    // check P and eta cuts
	    if (part->p() < m_mcAcceptanceMinP) continue;
	    const double eta = part->pseudoRapidity();
	    if (eta < m_mcAcceptanceEtaRegion[0] || eta > m_mcAcceptanceEtaRegion[1])
		continue;
	    // check if we are to exclude charged particles
	    const int charge = part->particleID().threeCharge();
	    if (charge) {
		if (!m_mcAcceptanceCharged) continue;
	    } else {
		if (!m_mcAcceptanceNeutral) continue;
	    }
	    // get and check PDG ctau value
	    const LHCb::ParticleProperty* pp = ppsvc->find(part->particleID());
	    if (pp && pp->ctau() < m_mcAcceptanceMinCtau) continue;
	    ++nMCPartInAcceptance;
	}

	zooev()->m_nMCPartInAcceptance = nMCPartInAcceptance;
    }
    

    // write event trigger info
    writeEventTriggerInfo(zooev());
    // write odin
    writeEventODIN(zooev());
    // write occupancies
    if (m_writeOccupancies) writeOccupancies(zooev());

    // write SPD, PRS, ECAL, HCAL multiplicity
    const LHCb::CaloDigits* digis =
	get<LHCb::CaloDigits>(LHCb::CaloDigitLocation::Spd);
    zooev()->m_spdMult = digis->size();
    digis = get<LHCb::CaloDigits>(LHCb::CaloDigitLocation::Prs);
    zooev()->m_prsMult = digis->size();
    digis = get<LHCb::CaloDigits>(LHCb::CaloDigitLocation::Ecal);
    zooev()->m_ecalMult = digis->size();
    digis = get<LHCb::CaloDigits>(LHCb::CaloDigitLocation::Hcal);
    zooev()->m_hcalMult = digis->size();

    //write magnet polarities
    zooev()->m_polarity = - (float)(m_magFieldSvc->signedRelativeCurrent()); // -- get field polarity
}

void ZooWriter::writeTrigger(ZooP* zp, const LHCb::Particle* p)
{
    ZooTrigger* trigger = zp->AddInfo<ZooTrigger>(*objman());

    ITriggerTisTos *l0tistos = m_TriggerTisTos;
    m_TriggerTisTos->setOfflineInput(*p);
#if DV_VER >= 256
    // for details on new L0 TISTOS tools, see
    // http://indico.cern.ch/getFile.py/access?contribId=4&resId=0&materialId=slides&confId=82230
    l0tistos = m_L0TriggerTisTos;
    l0tistos->setOfflineInput(*p);
#endif

    // start by filling the legacy fields
    for (unsigned idx = 0; idx < m_L0Name.value().size(); ++idx) {
	bool decisionL0 = false;
	bool tisL0 = false;
	bool tosL0 = false;
	l0tistos->setTriggerInput(m_L0Name.value()[idx]);
	std::vector<std::string> vs(l0tistos->triggerSelectionNames());
	l0tistos->selectionTisTos(vs, decisionL0, tisL0, tosL0);
	decisionL0 = 0 != l0tistos->hltObjectSummaries().size();
	trigger->SetBitsL0(idx, decisionL0, tisL0, tosL0);
    }

    for (unsigned idx = 0; idx < m_Hlt1Name.value().size(); ++idx) {
	bool decisionHLT1 = false;
	bool tisHLT1 = false;
	bool tosHLT1 = false;
	m_TriggerTisTos->triggerTisTos(*p, m_Hlt1Name.value()[idx], decisionHLT1, tisHLT1, tosHLT1);
	trigger->SetBitsHLT1(idx, decisionHLT1, tisHLT1, tosHLT1);
    }

    for (unsigned idx = 0; idx < m_Hlt2Name.value().size(); ++idx) {
	bool decisionHLT2 = false;
	bool tisHLT2 = false;
	bool tosHLT2 = false;
	m_TriggerTisTos->triggerTisTos(*p, m_Hlt2Name.value()[idx], decisionHLT2, tisHLT2, tosHLT2);
	trigger->SetBitsHLT2(idx, decisionHLT2, tisHLT2, tosHLT2);
    }

    // ok, now fill the new-style TIS/TOS fields
    const TriggerDecisions::AllTriggerDecisions::DecisionList& declist =
	TriggerDecisions::AllTriggerDecisions::alldecisions;
    boost::shared_ptr<ZooEv> zev = zooev();
    for (unsigned i = 0; i < declist.size(); ++i) {
	bool decision = false, tis = false, tos = false, tps = false;
	unsigned word = declist[i].second.first;
	UInt_t mask = declist[i].second.second;
	// don't ask for triggers which are not event present on event level
	// the string-based LHCb tools are SLOOOOOOOOOOOOOOOW!
	if (!(zev->getTriggerWord(word) & mask)) continue;
	if (0 == word) {
	    // word 0 is reserved to L0 stuff - since we cannot access L0
	    // directly, we have to take the L0XXXXXXDecision ones
	    // these are postscaled, however, so you need the check against
	    // the hltObject summary below to check the decision itself 
	    std::string decname =
		std::string("L0") + declist[i].first + "Decision";
#if DV_VER < 256
	    // fix for older DaVinci versions by prepending Hlt1 and hoping
	    // for the best
	    decname = std::string("Hlt1") + decname;
#endif
#if DV_VER >= 256
	    l0tistos->setOfflineInput(*p);
	    l0tistos->setTriggerInput(decname);
	    ITriggerTisTos::TisTosTob classifiedL0Dec =
		l0tistos->tisTosTobTrigger();
#else
	    ITriggerTisTos::TisTosDecision classifiedL0Dec =
		l0tistos->selectionTisTos(decname);
#endif
	    decision = 0 != l0tistos->hltObjectSummaries(decname).size();
	    // according to the talk mentioned above, tis/tos does not
	    // neccessarily work for L0 triggers (some L0 decisions are not
	    // based on candidates)
	    tis = classifiedL0Dec.tis();
	    tos = classifiedL0Dec.tos();
#if DV_VER >= 256
	    tps = classifiedL0Dec.tps();
#endif
	} else {
#if DV_VER >= 256
	    m_TriggerTisTos->setOfflineInput(*p);
	    m_TriggerTisTos->setTriggerInput(declist[i].first);
	    ITriggerTisTos::TisTosTob classifiedDec =
		m_TriggerTisTos->tisTosTobTrigger();
#else
	    ITriggerTisTos::TisTosDecision classifiedDec =
		m_TriggerTisTos->triggerTisTos(*p, declist[i].first);
#endif
	    decision = classifiedDec.decision();
	    tis = classifiedDec.tis();
	    tos = classifiedDec.tos();
#if DV_VER >= 256
	    tps = classifiedDec.tps();
#endif
	}
	// set decision/tis/tos words
	if (decision)
	    trigger->setTriggerDecWord(word, trigger->getTriggerDecWord(word) | mask);
	if (tis)
	    trigger->setTriggerTISWord(word, trigger->getTriggerTISWord(word) | mask);
	if (tos)
	    trigger->setTriggerTOSWord(word, trigger->getTriggerTOSWord(word) | mask);
	if (tps)
	    trigger->setTriggerTPSWord(word, trigger->getTriggerTPSWord(word) | mask);
    }
}

void ZooWriter::writePackedStates(ZooTrackInfo* trinfo, const LHCb::Particle* p)
{
    const LHCb::Track& tr = *(refittedTrack(p->proto()->track()));
    if (tr.states().empty()) return;
    const bool writeCovariance =
	!m_covarianceList.empty() &&
	(m_covarianceList.front() < 0 ||
	 m_covarianceList.end() !=
	 std::find(m_covarianceList.begin(),
	     m_covarianceList.end(),
	     std::abs(p->particleID().pid())));
    const Gaudi::Vector5 v5(
	    std::numeric_limits<double>::quiet_NaN(),
	    std::numeric_limits<double>::quiet_NaN(),
	    std::numeric_limits<double>::quiet_NaN(),
	    std::numeric_limits<double>::quiet_NaN(),
	    std::numeric_limits<double>::quiet_NaN());
    const Gaudi::SymMatrix5x5 nanm5(-Gaudi::SymMatrix5x5(ROOT::Math::SMatrixIdentity()));
    const ZooPackedState nanstate(
	    std::numeric_limits<double>::quiet_NaN(), v5, nanm5);
    ZooPackedStates* pstates = objman()->zooObj<ZooPackedStates>();
    trinfo->setStates(pstates);
    pstates->resize((m_packedStateAtPocaToZAxis?4:3) + m_packedStatesZList.size());
    // always put these states on
    const LHCb::State* state_closestToBeam =
	tr.stateAt(LHCb::State::ClosestToBeam);
    const LHCb::State* state_firstMeasurement =
	tr.stateAt(LHCb::State::FirstMeasurement);
    const LHCb::State* state_begRich2 = 
	tr.stateAt(LHCb::State::BegRich2);
    pstates->push_back(state_closestToBeam?
	    ZooPackedState(state_closestToBeam->z(),
		state_closestToBeam->stateVector(),
		writeCovariance?state_closestToBeam->covariance():nanm5) : 
	    nanstate);
    pstates->push_back(state_firstMeasurement?
	    ZooPackedState(state_firstMeasurement->z(),
		state_firstMeasurement->stateVector(),
		writeCovariance?state_firstMeasurement->covariance():nanm5) :
	    nanstate);

    pstates->push_back(state_begRich2?
	    ZooPackedState(state_begRich2->z(),
		state_begRich2->stateVector(),
		writeCovariance?state_begRich2->covariance():nanm5) :
	    nanstate);
    if (m_packedStateAtPocaToZAxis) {
	LHCb::State st;
	if (getTrackStateClosestToZAxis(tr, st)) {
	    pstates->push_back(ZooPackedState(st.z(), st.stateVector(),
			writeCovariance?st.covariance():nanm5));
	} else {
	    pstates->push_back(nanstate);
	}
    }

    // add states near/at user supplied z
    for (unsigned i = 0; i < m_packedStatesZList.size(); ++i) {
	LHCb::State st;
	if (m_packedStatesExtrapolate) {
	    StatusCode sc = m_extrapolator->propagate(tr,
		    m_packedStatesZList[i], st, p->particleID());
	    // skip state if failure to extrapolate - if a certain z is
	    // missing, we can tell
	    if (sc.isFailure()) continue;
	} else {
	    st = tr.closestState(m_packedStatesZList[i]);
	}
	pstates->push_back(ZooPackedState(st.z(),
		    st.stateVector(),
		    writeCovariance?st.covariance():nanm5));
    }
}

void ZooWriter::fillHitPattern(ZooTrackInfo* ztri, const LHCb::Particle* p)
{
    const LHCb::Track* tr = refittedTrack(p->proto()->track());

    LHCb::HitPattern muon_hitpattern;
    LHCb::MuonPIDs* muonpids = get<LHCb::MuonPIDs>( LHCb::MuonPIDLocation::Default );
    for (LHCb::MuonPIDs::const_iterator iter = muonpids->begin(); iter != muonpids->end(); ++iter ) 
      {
	LHCb::MuonPID* theID = *iter;
	const LHCb::Track* longtrack = theID->idTrack();
	const LHCb::Track* muontrack = theID->muonTrack();
	if (longtrack && muontrack && longtrack->key() == tr->key())
	  {
	    muon_hitpattern = LHCb::HitPattern(muontrack->lhcbIDs());
	  }
      }

    if (m_writeHitPattern && 0 == ztri->hitPattern()) {
	const LHCb::HitPattern hitPat(tr->lhcbIDs());
	ztri->setHitPattern(
		objman()->zooObj<ZooHitPattern>(ZooHitPattern(
			(hitPat.veloRA()).to_ulong(),
			(hitPat.veloRC()).to_ulong(),
			(hitPat.veloPhiA()).to_ulong(),
			(hitPat.veloPhiC()).to_ulong(),
			(hitPat.ot1stMonoLayer()).to_ulong(),
			(hitPat.ot2ndMonoLayer()).to_ulong(),
			(hitPat.itTopBottom()).to_ulong(),
			(hitPat.itAC()).to_ulong(),
			(hitPat.tt()).to_ulong(),
			(!tr->checkType(LHCb::Track::Muon)) ? (muon_hitpattern.muon()).to_ulong() : (hitPat.muon()).to_ulong()
			)));
    }
    if (m_writeExpectedHitPattern && 0 == ztri->expectedHitPattern()) {
	LHCb::HitPattern hitPat;
	std::vector<LHCb::LHCbID> ids;
	StatusCode sc = m_expectedHitsTool->execute(*tr, hitPat, ids);
	// in case of failure, we can't do anything about it, and the hit
	// collector already emits a warning, so we just make sure that we
	// only write out if we are successful...
	if (sc.isSuccess()) {
	    /// write LHCbIDs as well (if desired)
	    if (m_writeLHCbIDs) {
		std::sort(ids.begin(), ids.end());
		ztri->setExpectedLhcbids(
			objman()->zooObj<ZooLHCbIDBlock>(
			    ZooLHCbIDBlock(ids)));
	    }
	    ztri->setExpectedHitPattern(
		    objman()->zooObj<ZooHitPattern>(ZooHitPattern(
			    (hitPat.veloRA()).to_ulong(),
			    (hitPat.veloRC()).to_ulong(),
			    (hitPat.veloPhiA()).to_ulong(),
			    (hitPat.veloPhiC()).to_ulong(),
			    (hitPat.ot1stMonoLayer()).to_ulong(),
			    (hitPat.ot2ndMonoLayer()).to_ulong(),
			    (hitPat.itTopBottom()).to_ulong(),
			    (hitPat.itAC()).to_ulong(),
			    (hitPat.tt()).to_ulong(),
			    (hitPat.muon()).to_ulong())));
	}
    }
    if (m_writeCollectedHitPattern && 0 == ztri->collectedHitPattern()) {
	std::vector<ITrackHitCollector::IDWithResidual> ids;
	StatusCode sc = m_hitCollector->execute(*tr, ids);
	// in case of failure, we can't do anything about it, and the hit
	// collector already emits a warning, so we just make sure that we
	// only write out if we are successful...
	if (sc.isSuccess()) {
	    std::vector<LHCb::LHCbID> tmpids;
	    tmpids.reserve(ids.size());
	    BOOST_FOREACH(const ITrackHitCollector::IDWithResidual& idwr, ids)
		tmpids.push_back(idwr.m_id);
	    /// write LHCbIDs as well (if desired)
	    if (m_writeLHCbIDs) {
		std::sort(tmpids.begin(), tmpids.end());
		ztri->setCollectedLhcbids(
			objman()->zooObj<ZooLHCbIDBlock>(
			    ZooLHCbIDBlock(tmpids)));
	    }
	    const LHCb::HitPattern hitPat(tmpids);
	    ztri->setCollectedHitPattern(
		    objman()->zooObj<ZooHitPattern>(ZooHitPattern(
			    (hitPat.veloRA()).to_ulong(),
			    (hitPat.veloRC()).to_ulong(),
			    (hitPat.veloPhiA()).to_ulong(),
			    (hitPat.veloPhiC()).to_ulong(),
			    (hitPat.ot1stMonoLayer()).to_ulong(),
			    (hitPat.ot2ndMonoLayer()).to_ulong(),
			    (hitPat.itTopBottom()).to_ulong(),
			    (hitPat.itAC()).to_ulong(),
			    (hitPat.tt()).to_ulong(),
			    (hitPat.muon()).to_ulong())));
	}
    }
}

void ZooWriter::writeTrackInfo(ZooP* zp, const LHCb::Particle* p)
{
    // make sure we do not duplicate ZooTrackInfo blocks for one
    // and the same track
    const LHCb::Track *tr = refittedTrack(p->proto()->track());
    ZooTrackInfo*& ztri = 
      objman()->getOrCreateMappingFor<ZooTrackInfo>(tr);
    
    //new code to add nmuon hits
    int nMuonHits = 0;
    LHCb::MuonPIDs* muonpids = get<LHCb::MuonPIDs>( LHCb::MuonPIDLocation::Default );
    for (LHCb::MuonPIDs::const_iterator iter = muonpids->begin(); iter != muonpids->end(); ++iter ) 
      {
	LHCb::MuonPID* theID = *iter;
	const LHCb::Track* longtrack = theID->idTrack();
	const LHCb::Track* muontrack = theID->muonTrack();
	if (longtrack && muontrack && longtrack->key() == tr->key())
	  {
	    //some code for testing...
	    // 	  int nVelo = 0, nTT = 0, nIT = 0, nOT = 0, nMuon = 0;
	    // 	  int nMuonStation[5] = {0, 0, 0, 0, 0};
	    // 	  BOOST_FOREACH(LHCb::LHCbID id, muontrack->lhcbIDs()) {
	    // 	    if (id.isVelo()) ++nVelo;
	    // 	    if (id.isTT()) ++nTT;
	    // 	    if (id.isIT()) ++nIT;
	    // 	    if (id.isOT()) ++nOT;
	    // 	    if (id.isMuon()) 
	    // 	      {
	    // 		++nMuon;
	    // 		nMuonStation[id.muonID().station()]++;//apparently counts stations from 0
	    // 	      }
	    // 	  }
	    // 	  cout << "Found muon track " << muontrack->key() << " associated to track " << tr->key() << " with " << nMuon << " hits in the muon stations" << endl;
	    // 	  cout << "Other hits of the muontrack are " << nVelo << " " << nTT << " " << nIT << " " << nOT << endl;
	    // 	  cout << "Hit Pattern is " << nMuonStation[0] << " " << nMuonStation[1] << " " << nMuonStation[2] << " " << nMuonStation[3] << " " << nMuonStation[4] << endl;
	    // 	  cout << "Compare momentum of tr " << sqrt(tr->momentum().Mag2()) << " MeV with momentum of longtrack " << sqrt(longtrack->momentum().Mag2()) 
	    // 	       << " MeV and momentum of muontrack " << sqrt(muontrack->momentum().Mag2()) << " MeV" << endl;    
	    BOOST_FOREACH(LHCb::LHCbID id, muontrack->lhcbIDs()) 
	      if (id.isMuon()) 
		++nMuonHits;	  
	  }
      }
    
    if (!ztri) {
	int nVelo = 0, nTT = 0, nIT = 0, nOT = 0, nMuon = 0;
	BOOST_FOREACH(LHCb::LHCbID id, tr->lhcbIDs()) {
	    if (id.isVelo()) ++nVelo;
	    if (id.isTT()) ++nTT;
	    if (id.isIT()) ++nIT;
	    if (id.isOT()) ++nOT;
	    if (id.isMuon()) ++nMuon;
	}
	if (!tr->checkType(LHCb::Track::Muon))
	  nMuon = nMuonHits;

	ztri = zp->AddInfo<ZooTrackInfo>(*objman(), 
		ZooTrackInfo(tr->chi2(), 
		    tr->nDoF(), 
		    tr->nMeasurements(), 
		    tr->nLHCbIDs(),
		    nVelo, nTT, nIT, nOT, nMuon,
		    (int)tr->type(), 
		    tr->flags(), 
		    (int)tr->history(), 
		    (int)tr->patRecStatus(),
		    (int)tr->fitStatus(),
		    (int)tr->info(LHCb::Track::CloneDist, -1.),
		    tr->ghostProbability()));
	if (m_writeLHCbIDs) {
	    const std::vector<LHCb::LHCbID>& ids = tr->lhcbIDs();
	    ztri->setLhcbids(
		    objman()->zooObj<ZooLHCbIDBlock>(
			ZooLHCbIDBlock(ids)));
	}
    } else {
	zp->AssignInfo<ZooTrackInfo>(ztri);
    }

    // fill packed states...
    if (!m_packedStatesList.empty()) {
	if (m_packedStatesList.front() < 0 ||
		m_packedStatesList.end() !=
                    std::find(m_packedStatesList.begin(),
                        m_packedStatesList.end(),
                        std::abs(p->particleID().pid())))
	    if (0 == ztri->states()) writePackedStates(ztri, p);
    }

    if ((m_writeHitPattern && 0 == ztri->hitPattern()) ||
	    (m_writeExpectedHitPattern && 0 == ztri->expectedHitPattern()) ||
	    (m_writeCollectedHitPattern && 0 == ztri->collectedHitPattern())) {
      fillHitPattern(ztri, p);
    }
  
    if (!m_extraInfoList.empty() && 0 == ztri->extraInfo())
	writeTrackExtraInfo(ztri, p);
}

void ZooWriter::writeTrackExtraInfo(ZooTrackInfo* ztri, const LHCb::Particle* p)
{
    const LHCb::Track *tr = refittedTrack(p->proto()->track());
    ZooTrackExtraInfo::KeyValueVector eivector;
    eivector.reserve(m_extraInfoList.size());
    BOOST_FOREACH(int i, m_extraInfoList){
      double info = tr->info(i, std::numeric_limits<double>::quiet_NaN());
      if (!std::isnan(info))
	  eivector.push_back(std::make_pair(UInt_t(i), Float_t(info)));
    }

    ztri->setExtraInfo(objman()->zooObj<ZooTrackExtraInfo>(
		ZooTrackExtraInfo(eivector)));
}

void ZooWriter::writeEventTriggerInfo(boost::shared_ptr<ZooEv> event) const
{
    if (exist<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default,false)) {
	LHCb::L0DUReport* L0report =
	    get<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default,false);
	event->m_L0Dec = (int) L0report->decision(); 	

	// loop over the trigger decisions which are interesting for us
	// and set the corresponding bits...
	const TriggerDecisions::AllTriggerDecisions::DecisionList& declist =
	    TriggerDecisions::AllTriggerDecisions::alldecisions;
	for (unsigned i = 0; i < declist.size(); ++i) {
	    if(declist[i].second.first == 0) { 
		if (L0report->channelDecisionByName(declist[i].first)) {
		    event->m_triggerDecisions[declist[i].second.first] |=
			declist[i].second.second;
		}
	    }
	}
    }


    if (exist<LHCb::HltDecReports>(LHCb::HltDecReportsLocation::Default,false)) {
	// get HLT decision reports
	const LHCb::HltDecReports* HLTdecReports =
	    get<LHCb::HltDecReports>(LHCb::HltDecReportsLocation::Default,false);
	// loop over the trigger decisions which are interesting for us
	// and set the corresponding bits...
	const TriggerDecisions::AllTriggerDecisions::DecisionList& declist =
	    TriggerDecisions::AllTriggerDecisions::alldecisions;
	for (unsigned i = 0; i < declist.size(); ++i) {
	    if(declist[i].second.first != 0) {
		const LHCb::HltDecReport *dec =
		    HLTdecReports->decReport(declist[i].first);
		if(!dec) debug() << "The decision " << declist[i].first 
		<< " does not exist in the current trigger scenario" 
		<< endmsg;
		else debug() << "The decision " << declist[i].first 
		    << " returned " << dec->decision() << endmsg;
		if (dec && dec->decision()) {
		    event->m_triggerDecisions[declist[i].second.first] |=
			declist[i].second.second;
		}
	    }
	}
    }
}

void ZooWriter::writeEventODIN(boost::shared_ptr<ZooEv> ev) const
{
    // get ODIN (ODIN decoder triggers decoding of ODIN if not yet done)
    m_odinDecoder->getTime();
    const LHCb::ODIN* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default,false);
    ev->m_eventNumber = odin->eventNumber();
    ev->m_gpsTime = odin->gpsTime();
    ev->m_detectorStatus = odin->detectorStatus();
    ev->m_runNumber = odin->runNumber();
    ev->m_eventType = odin->eventType();
    ev->m_orbitNumber = odin->orbitNumber();
    ev->m_bunchCurrent = odin->bunchCurrent();
    ev->m_calibrationStep = odin->calibrationStep();
    ev->m_TCK = odin->triggerConfigurationKey();
    ev->m_bunchID = odin->bunchId();
    ev->m_triggerType = odin->triggerType();
    ev->m_readoutType = odin->readoutType();
    ev->m_bunchCrossingType = odin->bunchCrossingType();
    ev->m_calibrationType = odin->calibrationType();
    ev->m_TAEWindow = odin->timeAlignmentEventWindow();
    ev->m_errorBits = odin->errorBits();
    ev->m_forceBit = odin->forceBit();
}

void ZooWriter::writeDLL (ZooP* zp, const LHCb::Particle* p){

    ZooDLL *&dll = objman()->getOrCreateMappingFor<ZooDLL>(p->proto());
    if (0 == dll) {
	dll = zp->AddInfo<ZooDLL>(*objman());
	dll->m_DLLe  = p->proto()->info(LHCb::ProtoParticle::CombDLLe, -999.);
	dll->m_DLLk  = p->proto()->info(LHCb::ProtoParticle::CombDLLk, -999.);
	dll->m_DLLmu = p->proto()->info(LHCb::ProtoParticle::CombDLLmu, -999.);
	dll->m_DLLp  = p->proto()->info(LHCb::ProtoParticle::CombDLLp, -999.);
	dll->m_hasRich = (int) p->proto()->hasInfo( LHCb::ProtoParticle::RichPIDStatus );
	if (p->proto()->muonPID()!=0){
	    dll->m_isMuon  = (int) p->proto()->muonPID()->IsMuon();
	    dll->m_isMuonLoose  = (int) p->proto()->muonPID()->IsMuonLoose();
	} 
    } else {
	zp->AssignInfo<ZooDLL>(dll);
    }
}

void ZooWriter::writeMC()
{ 
    LHCb::MCParticles* mcParts =
	get<LHCb::MCParticles>( LHCb::MCParticleLocation::Default );

    LHCb::MCParticles::const_iterator itP;

    for ( itP = mcParts->begin(); mcParts->end() != itP; ++itP ) {
	LHCb::MCParticle* part = *itP;
	// if m_MCList is empty, just write everything
	if (m_MCList.empty()) {
	    GetSaved(part);
	} else {
	    for (unsigned i = 0; i < m_MCList.size(); ++i) {
		// ok, if it's on the list, write it out (along with its children)
	        if (m_MCList[i] == std::abs(part->particleID().pid())) {
		    if (!m_writeMCtrees)
		        GetSaved(part);
		    else {
		        const LHCb::MCParticle* buffer = part;
			while (buffer->mother())
			    buffer = buffer->mother();
			GetSaved(buffer);
		    }
		}
	    }
	}
    }
}

ZooMCP* ZooWriter::GetSaved(const LHCb::MCParticle* p)
{   
    ZooMCP *&zp = objman()->getOrCreateMappingFor<ZooMCP>(p);
    if (zp) return zp;
    zp = objman()->zooObj<ZooMCP>();

    if (p->originVertex())
	zp->m_prodVertex = p->originVertex()->position();

    const SmartRefVector<LHCb::MCVertex> &vtx = p->endVertices();

    zp->m_momentum = p->momentum();
    zp->m_pid = p->particleID().pid();

    MCTrackInfo trackInfo( evtSvc(), msgSvc() );
    zp->m_mcTrackInfo = trackInfo.fullInfo(p);

    SmartRefVector<LHCb::MCVertex>::const_iterator i=vtx.begin();
    
    if (vtx.size()>0)
      zp->m_decayVertex = (*i)->position();

    zp->m_key = p->key();

    // set flags
    if (p->hasOscillated())
	zp->m_flags |= ZooMCP::hasOscillated;
    const LHCb::MCVertex *ov = 0, *pv = 0;
    pv = p->primaryVertex();
    ov = p->originVertex();
    if (pv && ov &&
	    (pv->position() - Gaudi::XYZVector(ov->position())).R() < 1e-5)
	zp->m_flags |= ZooMCP::isFromPV;

    for(SmartRefVector<LHCb::MCVertex>::const_iterator i=vtx.begin();i!=vtx.end();++i){
	const SmartRefVector<LHCb::MCParticle> &ch = (*i)->products();

	for(SmartRefVector<LHCb::MCParticle>::const_iterator j=ch.begin();
		j!=ch.end();++j){

	    ZooMCP* zc = GetSaved(*j);
	    zc->AddMother(zp);
	    zp->AddChild(zc);
	}  
    }

    return zp;
}

void ZooWriter::writeLinks()
{
    BOOST_FOREACH(const std::string& from, m_linkFromList.value()) {
	// do not work on collections which do not exist
	if (!exist<LHCb::Particle::Range>(from)) continue;
	const LHCb::Particle::Range parts_from = get<LHCb::Particle::Range>(from);
	BOOST_FOREACH(const std::string& to, m_linkToList.value()) {
	    // do not work on collections which do not exist
	    if (!exist<LHCb::Particle::Range>(to)) continue;
	    const LHCb::Particle::Range parts_to = get<LHCb::Particle::Range>(to);

	    BOOST_FOREACH(const LHCb::Particle* const p_from, parts_from) {
		// make sure particle is saved and create a ZooLinks block if
		// needed
		ZooP* z_from = GetSaved(p_from);
		ZooLinks* links = z_from->Info<ZooLinks>() ?
		    const_cast<ZooLinks*>(z_from->Info<ZooLinks>()) :
		    z_from->AddInfo<ZooLinks>(*objman());

		BOOST_FOREACH(const LHCb::Particle* const p_to, parts_to) {
		    // do not link a particle to itself
		    if (p_from == p_to) continue;
		    // check here if there really is a link between p_from and *p_to
		    double frac = associateLink(p_from, p_to);
		    if (frac > 0.7)
			links->AddLink(GetSaved(p_to), frac);
		}
	    }
	}
    }
}

double ZooWriter::associateLink(const LHCb::Particle* p_from, const LHCb::Particle* p_to)
{
    const LHCb::Track* tr_from = refittedTrack(p_from->proto()->track());
    const LHCb::Track* tr_to   = refittedTrack(p_to->proto()->track());
    // same track: 100% identical...
    if (tr_from == tr_to) return 1.;

    // make sure tr_from is the shorter track
    if (tr_from->nLHCbIDs() > tr_to->nLHCbIDs())
	std::swap(tr_from, tr_to);

    const unsigned n_total = tr_from->nLHCbIDs();
    unsigned n_common = 0;

    BOOST_FOREACH(const LHCb::LHCbID id, tr_from->lhcbIDs()) {
	if (std::binary_search(
		    tr_to->lhcbIDs().begin(), tr_to->lhcbIDs().end(), id))
	    ++n_common;
    }

    return double(n_common)/double(n_total);
}

ZooPackedVertex* ZooWriter::writePackedVertex(
	const LHCb::VertexBase* vtx, bool writeCovariance, bool registerMapping)
{
    const Gaudi::XYZPoint nanv3(
	    std::numeric_limits<double>::quiet_NaN(),
	    std::numeric_limits<double>::quiet_NaN(),
	    std::numeric_limits<double>::quiet_NaN());
    const Gaudi::SymMatrix3x3 nanm3(
	    -Gaudi::SymMatrix3x3(ROOT::Math::SMatrixIdentity()));
    if (!registerMapping) {
      // do not register a new mapping, but use any mapping that may
	// alreeady exist
	// (idea: quick access to pre-generated Vertices (like PV), but
	// also easy to write special ones that are not shared)
	ZooPackedVertex* zv =
	    objman()->getMappingFor<ZooPackedVertex>(vtx);
	if (zv) return zv;
	return objman()->zooObj(
		ZooPackedVertex(vtx?vtx->position():nanv3,
		    (writeCovariance && vtx)?vtx->covMatrix():nanm3,
		    vtx?vtx->chi2():-1., vtx?vtx->nDoF():-1));
    }
    // get or create the mapping for the vertex given
    ZooPackedVertex*& zv =
	objman()->getOrCreateMappingFor<ZooPackedVertex>(vtx);
    // if the corresponding Zoo object exists, return it
    if (zv) return zv;
    // otherwise, register a ZooPackedVertex with the object manager
    // and fill in the relevant information
    zv = objman()->zooObj(
	    ZooPackedVertex(vtx?vtx->position():nanv3,
		(writeCovariance && vtx)?vtx->covMatrix():nanm3,
		vtx?vtx->chi2():-1., vtx?vtx->nDoF():-1));
    return zv;
}

unsigned ZooWriter::getMCBkgFlags(const LHCb::Particle* p) const
{
    unsigned flags = 0;
    std::vector<const LHCb::Particle*> daughters;
    daughters.reserve(8);
    daughters.push_back(p);
    // put all stable daughters in array
    for (unsigned last = 0; last < daughters.size(); ) {
	for (unsigned i = last; i < daughters.size(); ++i) {
	    const LHCb::Particle*& tmp = daughters[i];
	    if (tmp->isBasicParticle()) {
		if (last == i) last = i + 1;
		continue;
	    }
	    const SmartRefVector<LHCb::Particle> &ch = tmp->daughters();
	    tmp = *(ch.begin());
	    for (SmartRefVector<LHCb::Particle>::const_iterator it = ch.begin() + 1;
		    it != ch.end(); ++it)
		daughters.push_back(*it);
	}
    }
    // go to stable daughters' mothers on MC side, check for c/b quarks
    for (unsigned i = 0; i < daughters.size(); ++i) {
	const LHCb::Particle* tmp = daughters[i];
	// is p a stable particle? if so, then use linker tool
	// otherwise, get the mc particle from background tool
	const LHCb::MCParticle* mcp =
	    tmp->isBasicParticle() ? m_mcLink->firstMCP(tmp) : m_bkg->origin(tmp);
	while (mcp) {
	    LHCb::ParticleID pid = mcp->particleID();
	    if (pid.hasCharm()) flags |= ZooMCBkg::DaughtersFromC;
	    if (pid.hasBottom()) flags |= ZooMCBkg::DaughtersFromB;
	    mcp = mcp->mother();
	}
    }
    // if associated: check if from PV
    const LHCb::MCParticle* mcp =
	    p->isBasicParticle() ? m_mcLink->firstMCP(p) : m_bkg->origin(p);
    if (mcp) {
	const LHCb::MCVertex *ov = 0, *pv = 0;
	pv = mcp->primaryVertex();
	ov = mcp->originVertex();
	if (pv && ov &&
		(pv->position() - Gaudi::XYZVector(ov->position())).R() < 1e-5)
	    flags |= ZooMCBkg::isFromPV;
    }

    return flags;
}

void ZooWriter::getPVWouter()
{
    std::vector<LHCb::State> wksp;
    LHCb::Tracks *tracks = get<LHCb::Tracks>(LHCb::TrackLocation::Default);
    wksp.clear();
    wksp.reserve(tracks->size());
    std::vector<const LHCb::State*> goodstates;
    std::vector<const LHCb::Track*> goodtracks;
    goodstates.reserve(tracks->size());
    goodtracks.reserve(tracks->size());
    // select tracks for emergency PV
    BOOST_FOREACH(const LHCb::Track* ptr, *tracks) {
	// skip tracks without hits in Velo or TT
	if (!ptr->hasVelo() && !ptr->hasTT()) continue;

	LHCb::State st = ptr->firstState();
	// estimate poca with z axis from y component (hardly affected
	// by B field)
	const double z1 = st.z() - st.y() / st.ty();
	if (z1 > 5000. || z1 < -1000.) continue;
	if (m_extrapolator->propagate(*ptr, z1, st).isFailure()) {
	    debug() << "Extrapolation failed." << endmsg;
	    continue;
	}

	// straight line extrapolation to see if DOCA to z axis is small
	// enough
	const double dz = - (st.x()*st.tx() + st.y()*st.ty()) /
	    (st.tx()*st.tx() + st.ty()*st.ty());
	const double x = st.x() + dz * st.tx();
	const double y = st.y() + dz * st.ty();

	if (std::abs(x) < 100. && std::abs(y) < 300.) {
	    // good enough
	    wksp.push_back(st);
	    goodstates.push_back(&wksp.back());
	    goodtracks.push_back(ptr);
	}
    }
    if (goodstates.size() < 2) {
	m_ctx->pvWouter().reset();
	return;
    }
    // refit (and remove) iteratively
    LHCb::TrackStateVertex vtx(goodstates);
    vtx.fit();
    while (vtx.nTracks() > 2 && vtx.chi2() / double(vtx.nDoF()) > 10.) {
	// find the worst track
	size_t worsttrack(0) ;
	double worstChi2(0.) ;
	for(size_t i = 0; i < vtx.nTracks(); ++i) {
	    double chi2 = vtx.biasedChi2(i);
	    if (worstChi2 < chi2) {
		worstChi2 = chi2;
		worsttrack = i;
	    }
	}
	goodstates.erase(goodstates.begin() + worsttrack);
	goodtracks.erase(goodtracks.begin() + worsttrack);
	// recreate a new TrackStateVertex in the place where the old one
	// was stored
	vtx.~TrackStateVertex();
	new(&vtx) LHCb::TrackStateVertex(goodstates);
	vtx.fit();
    }

    // check that position is sane
    if (std::abs(vtx.position().z()) > 500.0) {
	m_ctx->pvWouter().reset();
	return;
    }

    // convert to LHCb::VertexBase expected in the rest of the code
    boost::shared_ptr<LHCb::VertexBase>
	lbvtx(new LHCb::RecVertex(vtx.position()));
    lbvtx->setCovMatrix(vtx.covMatrix());
    lbvtx->setChi2(vtx.chi2());
    lbvtx->setNDoF(vtx.nDoF());
    BOOST_FOREACH(const LHCb::Track* ptr, goodtracks)
	reinterpret_cast<LHCb::RecVertex*>(lbvtx.get())->addToTracks(ptr);
    reinterpret_cast<LHCb::RecVertex*>(lbvtx.get())->setTechnique(LHCb::RecVertex::Primary);
    m_ctx->pvWouter().swap(lbvtx);
}

void ZooWriter::writeOccupancies(boost::shared_ptr<ZooEv> ev)
{
    // arrays
    int velo[23][2][2], tt[2][2][3][26], it[3][4][4][7], ot[3][4][4][9];
    std::fill(&velo[0][0][0], &velo[0][0][0] + 23 * 2 * 2, 0);
    std::fill(&tt[0][0][0][0], &tt[0][0][0][0] + 2 * 2 * 3 * 26, 0);
    std::fill(&it[0][0][0][0], &it[0][0][0][0] + 3 * 4 * 4 * 7, 0);
    std::fill(&ot[0][0][0][0], &ot[0][0][0][0] + 3 * 4 * 4 * 9, 0);
    // get Velo hits
    m_hitManagerVeloR->prepareHits();
    for (HitManVeloR::StationIterator it = m_hitManagerVeloR->stationsAllBegin();
	    m_hitManagerVeloR->stationsAllEnd() != it; ++it) {
	for (unsigned iZone = 0; (*it)->nZones() > iZone; ++iZone) {
	    BOOST_FOREACH(const Tf::VeloRHit* hit, (*it)->hits(iZone)) {
		LHCb::VeloChannelID id = hit->channelID();
		unsigned station = (id.sensor() & 0x3f) >> 1;
		unsigned side    = (id.sensor() & 1);
		unsigned type    = (id.sensor() >> 6) & 3;
		// now deal with the PU sensors. we reserve the first two
		// bits for those. if you leave these lines away it will
		// still work, but pile-up detectors will end up in separate
		// bitsets.
		station += (1 - (type >> 1)) << 1;
		type    &= 1;
		++velo[station][side][type];
	    }
	}
    }
    m_hitManagerVeloPhi->prepareHits();
    for (HitManVeloPhi::StationIterator it = m_hitManagerVeloPhi->stationsAllBegin();
	    m_hitManagerVeloPhi->stationsAllEnd() != it; ++it) {
	for (unsigned iZone = 0; (*it)->nZones() > iZone; ++iZone) {
	    BOOST_FOREACH(const Tf::VeloPhiHit* hit, (*it)->hits(iZone)) {
		LHCb::VeloChannelID id = hit->channelID();
		unsigned station = (id.sensor() & 0x3f) >> 1;
		unsigned side    = (id.sensor() & 1);
		unsigned type    = (id.sensor() >> 6) & 3;
		// now deal with the PU sensors. we reserve the first two
		// bits for those. if you leave these lines away it will
		// still work, but pile-up detectors will end up in separate
		// bitsets.
		station += (1 - (type >> 1)) << 1;
		type    &= 1;
		++velo[station][side][type];
	    }
	}
    }
    // get TT hits
    m_hitManagerTT->prepareHits();
    HitManTT::HitRange ttCoords = m_hitManagerTT->hits();
    BOOST_FOREACH(const PatTTHit* hit, ttCoords) {
	LHCb::STChannelID id = hit->hit()->lhcbID().stID();
	++tt[id.station() - 1][id.layer() - 1][id.detRegion() - 1][id.sector() - 1];
    }
    // get IT hits
    m_hitManagerT->prepareHits();
    HitManT::HitRange tCoords = m_hitManagerT->hits();
    BOOST_FOREACH(const PatFwdHit* hit, tCoords) {
	const Tf::STHit* sthit = hit->hit()->sthit();
	if (sthit) {
	    LHCb::STChannelID id = sthit->cluster().channelID();
	    ++it[id.station() - 1][id.layer() - 1][id.detRegion() - 1][id.sector() - 1];
	}
	const Tf::OTHit* othit = hit->hit()->othit();
	if (othit) {
	    LHCb::OTChannelID id = othit->rawhit().channel();
	    ++ot[id.station() - 1][id.quarter()][id.layer()][id.module() - 1];
	}
    }

    ev->m_occupancy = objman()->zooObj<ZooOccupancy>(
		    ZooOccupancy(velo, tt, it, ot));
}

const LHCb::Track* ZooWriter::refittedTrack(const LHCb::Track* tr)
{
    // check if we need to refit track: we do in case we are supposed to
    // - write expected/collected Hit patterns
    // - write extrapolated states for at least some particles
    if (!m_writeExpectedHitPattern && !m_writeCollectedHitPattern &&
	    !(!m_packedStatesList.empty() && !m_packedStatesZList.empty() &&
		m_packedStatesExtrapolate))
	return tr;
    // Velo tracks can not currently be refitted
    if (!tr->type() || LHCb::Track::Velo == tr->type() ||
	    LHCb::Track::VeloR == tr->type())
	return tr;
    // check if track we already have a refitted version
    LHCb::Track*& fittedtrackmapping =
	m_ctx->getOrCreateFittedTrackMapping(tr);
    // if it exists, return it
    if (fittedtrackmapping) return fittedtrackmapping;
    // ok, need to refit
    // construct new track in the pool
    LHCb::Track* fittedtrack = m_ctx->newTrackFromPool();
    if (0 == fittedtrack) throw std::bad_alloc();
    // copy old one
    fittedtrack->copy(*tr);
    // refit
    StatusCode sc = m_trackFitter->fit(*fittedtrack);
    if (sc.isFailure()) {
	warning() << "Refitting track failed!" << endreq;
	// avoid further fitting attempts by putting it into the table
	//
	// note that while casting constness of tr away is extremely
	// ugly, this should not break things since we only ever return
	// const pointers from this method
	fittedtrackmapping = const_cast<LHCb::Track*>(tr);
	return tr;
    } else {
	// refit went ok, register refitted track
	fittedtrackmapping = fittedtrack;
	// make sure we have refitted track mapping to itself so that
	// we're not refitting an already refitted track because some
	// clown accidentally passes us the wrong pointer...
	m_ctx->getOrCreateFittedTrackMapping(fittedtrack) = fittedtrack;
    }
    return fittedtrack;
}

bool ZooWriter::getTrackStateClosestToZAxis(
	const LHCb::Track& tr, LHCb::State& st) const
{
    if (tr.states().empty()) return false;
    st = tr.firstState();
    // start with an estimate from y only (less effect of B field)
    double z = st.z() - st.y() / st.ty(), dz = 0.;
    unsigned iter = 0;

    do {
	if (std::abs(z) > 1500.) return false;
	if (m_extrapolator->propagate(tr, z, st).isFailure()) {
	    debug() << "Extrapolation failed." << endmsg;
	    return false;
	}

	// straight line extrapolation
	dz = - (st.x()*st.tx() + st.y()*st.ty()) /
	    (st.tx()*st.tx() + st.ty()*st.ty());
	z += dz;
	if (iter++ > 42) return false;
    } while (std::abs(dz) < .1);
    return true;
}

// vim: sw=4:tw=78:ft=cpp
