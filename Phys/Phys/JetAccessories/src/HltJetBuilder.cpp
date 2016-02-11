// Local.
#include "HltJetBuilder.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltJetBuilder
//
// 2016-02-02 : Victor Coco, Cedric Potterat, and Philip Ilten
//-----------------------------------------------------------------------------

// Declare the algorithm factory.
DECLARE_ALGORITHM_FACTORY(HltJetBuilder)

//=============================================================================
// Static members.
//=============================================================================
const double HltJetBuilder::m_jecLimEta[m_jecNEta + 1] = {
  2.0, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.8, 3.0, 3.2, 3.8, 4.5};
const double HltJetBuilder::m_jecLimCpf[m_jecNCpf + 1] = {
  0.1, 0.3, 0.5, 0.7, 1.0};
const double HltJetBuilder::m_jecLimPhi[m_jecNPhi + 1] = {
  0, 1.0/6.0*M_PI, 1.0/3.0*M_PI, 0.5*M_PI};
const double HltJetBuilder::m_jecLimPt[m_jecNPt + 1] = {5, 298};

//=============================================================================
// Constructor.
//=============================================================================
HltJetBuilder::HltJetBuilder(const string &name, ISvcLocator *svc) 
  : DaVinciHistoAlgorithm(name , svc), m_fj(0) {
  // Declare the input/output properties.
  declareProperty("InputBans", m_inBans,
		  "Input locations of Particles to ban.");
  declareProperty("InputTags", m_inTags,
		  "Input locations of Particle to tag.");

  // Declare the jet properties.
  declareProperty("JetPid", m_jetPid = 98,
		  "Particle ID to assign the jets.");
  declareProperty("JetPtMin", m_jetPtMin = 5 * Gaudi::Units::GeV,
		  "Minimum jet pT to build with FastJet.");
  declareProperty("JetR", m_jetR = 0.5,
		  "Jet radius parameter to use with FastJet.");
  declareProperty("JetVrt", m_jetVrt = false,
		  "If true, build jets for each primary vertex, otherwise "
		  "build inclusively.");
  declareProperty("JetSort", m_jetSort = 2,
		  "Jet sorting method: 1 E, 2 pT, or 3 y.");
  declareProperty("JetInfo", m_jetInfo = true,
		  "Write out extra info for the jets.");
  declareProperty("JetEcPath", m_jetEcPath = "JEC14R05",
		  "If supplied, perform jet energy correction using histograms "
		  "from this path.");
  declareProperty("JetEcShift", m_jetEcShift = 0,
		  "Shift the jet energy correction by this (in JEC sigma).");

  // Declare the FastJet properties.
  declareProperty("FjAlg", m_fjAlg = 2,
		  "FastJet finder to use: 0 kt, 1 cambridge, 2 anti-kt, ...; "
		  "see fasjet::JetAlgorithm for more options.");
  declareProperty("FjScheme", m_fjScheme = 0,
		  "FastJet recombination scheme: 0 E, 1 pT, 2 pT^2, ...; see "
		  "fastjet::RecombinationScheme for more options.");
  declareProperty("FjStrategy", m_fjStrategy = 1,
		  "FastJet clustering strategy: 0 N3Dumb, 1 Best, 2 NlnN, ...; "
		  "see fastjet::Strategy for more options.");
}

//=============================================================================
// Initialize.
//=============================================================================
StatusCode HltJetBuilder::initialize() {
  // Retrieve the FastJet tool and configure.
  DaVinciHistoAlgorithm::initialize();
  if (!m_fj) m_fj = tool<IJetMaker>("LoKi::FastJetMaker", this);
  if (!m_fj) return Error("Could not retrieve FastJetMaker.");
  GaudiTool *fj = dynamic_cast<GaudiTool*>(m_fj);
  fj->setProperty("JetID", m_jetPid);
  fj->setProperty("PtMin", m_jetPtMin);
  fj->setProperty("RParameter", m_jetR);
  fj->setProperty("Sort", m_jetSort);
  fj->setProperty("Strategy", m_fjStrategy);
  fj->setProperty("Type", m_fjAlg);
  fj->setProperty("Recombination", m_fjScheme);

  // Retrieve the JEC histograms.
  if (!m_jetEcPath.empty()) {
    string name;
    for(int pvs = 0; pvs < m_jecNPvs; pvs++)
      for(int eta = 0; eta < m_jecNEta; eta++)
	for(int cpf = 0; cpf < m_jecNCpf; cpf++) 
	  for(int phi = 0; phi < m_jecNPhi; phi++) {
	    name = m_jetEcPath + "/JECSYS_PV" + to_string(pvs + 1) + "_ETA" + 
	      to_string(eta) + "_CPF" + to_string(cpf) + "_PHI" + 
	      to_string(phi);
	    AIDA::IHistogram1D *hst = get<AIDA::IHistogram1D>(histoSvc(), name);
	    if (!hst) return Error("No JEC histogram " + name + ".");
	    m_jecs[pvs][eta][cpf][phi] = 
	      Gaudi::Utils::Aida2ROOT::aida2root(hst);
	  }
    m_jecPvs = 0;
  } else m_jecPvs = -1;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Execute.
//=============================================================================
StatusCode HltJetBuilder::execute() {
  // Clear previous event, Ban Tracks and CaloClusters from Particles.
  m_jets.clear(); m_trks.clear(); m_cals.clear();
  Particle::Range prts;
  for (unsigned int inLoc = 0; inLoc < m_inBans.size(); ++inLoc) {
    prts = getIfExists<Particle::Range>(m_inBans[inLoc]);
    for (Particle::Range::iterator prt = prts.begin(); prt != prts.end(); ++prt)
      ban(*prt);
  }

  // Assign the input Particles by best PV position (if requested).
  vector<const Particle*> cntrPrts; VrtsPrts vrtsPrts; prts = particles();
  for (Particle::Range::iterator prt = prts.begin(); prt != prts.end(); ++prt) {
    if (banned(*prt)) continue;
    if (!m_jetVrt || LoKi::Cuts::NINTREE(LoKi::Cuts::HASTRACK)(*prt) > 0) {
      cntrPrts.push_back(*prt); continue;}
    const VertexBase *vrt = bestVertex(*prt);
    if (!vrt) {cntrPrts.push_back(*prt); continue;}
    VrtsPrts::iterator vrtPrt = vrtsPrts.find(vrt);
    if (vrtPrt == vrtsPrts.end()) 
      vrtsPrts[vrt] = vector<const Particle*>(1, *prt);
    else vrtPrt->second.push_back(*prt);
  }

  // Build the jets.
  if (m_jecPvs >= 0) m_jecPvs = primaryVertices().size();
  if (m_jetVrt) {
    for (VrtsPrts::iterator vrtPrt = vrtsPrts.begin(); vrtPrt != 
	   vrtsPrts.end(); ++vrtPrt) {
      vrtPrt->second.insert(vrtPrt->second.end(), cntrPrts.begin(),
			    cntrPrts.end());
      build(&vrtPrt->second, vrtPrt->first);
    }
  } else build(&cntrPrts, 0);

  // Tag the jets.
  for (unsigned int inLoc = 0; inLoc < m_inTags.size(); ++inLoc) {
    prts = getIfExists<Particle::Range>(m_inTags[inLoc]);
    for (Particle::Range::iterator prt = prts.begin(); prt != prts.end(); 
	 ++prt) {
      double phi((*prt)->momentum().Phi()), eta((*prt)->momentum().Eta());
      if ((*prt)->endVertex()) {
	const VertexBase *vrt = bestVertex(*prt);
	if (vrt) {Gaudi::XYZVector vec = (*prt)->endVertex()->position() - 
	    vrt->position(); phi = vec.Phi(); eta = vec.Eta();}
      }
      for (vector<Particle*>::iterator jet = m_jets.begin(); 
	   jet != m_jets.end(); ++jet) {
	phi -= (*jet)->momentum().Phi();
	eta -= (*jet)->momentum().Eta();
	while (phi > M_PI) phi -= 2*M_PI;
	while (phi <= -M_PI) phi += 2*M_PI;
	bool tag = (*jet)->hasInfo(Particle::FirstJetIndex + 100 + inLoc);
	if (!tag && sqrt(phi*phi + eta*eta) < m_jetR)
	  (*jet)->addInfo(Particle::FirstJetIndex + 100 + inLoc, (*prt)->key());
      }
    }
  }
  setFilterPassed(true);
  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize.
//=============================================================================
StatusCode HltJetBuilder::finalize() {return StatusCode::SUCCESS;}

//=============================================================================
// Additional methods.
//=============================================================================
// Build jets from #prts and add to the TES.
void HltJetBuilder::build(const vector<const Particle*> *prts,
			  const VertexBase *vrt) {
  vector<Particle*> jets;
  if (m_fj->makeJets(prts->begin(), prts->end(), jets).isFailure()) return;
  for (vector<Particle*>::iterator jet = jets.begin(); jet != jets.end();
       ++jet) {
    if (m_jetInfo || m_jecPvs >= 0) info(*jet);
    if (m_jecPvs >= 0) jec(*jet);
    if (vrt) (*jet)->setReferencePoint(vrt->position());
    else (*jet)->setReferencePoint(Gaudi::XYZPoint(0, 0, 0));
    if ((*jet)->momentum().Pt() >= m_jetPtMin) {
      markParticle(*jet); m_jets.push_back(*jet);
    } else {delete (*jet)->endVertex(); delete *jet;}
  }
}

// Add additional information.
void HltJetBuilder::info(Particle *prt) {
  double cpx(0), cpy(0), cptMax(0), nptMax(0), width(0), trks(0),
    pt(prt->momentum().Pt());
  Particle::ConstVector dtrs;
  LoKi::Extract::getParticles(prt, back_inserter(dtrs), LoKi::Cuts::HASPROTO
			      && LoKi::Cuts::ISBASIC);
  for (Particle::ConstVector::iterator dtr = dtrs.begin(); dtr != dtrs.end();
       ++dtr) {
    const Gaudi::LorentzVector &vec = (*dtr)->momentum();
    if ((*dtr)->charge() != 0) {
      if (vec.Pt() > cptMax) cptMax = vec.Pt();
      cpx += vec.Px(); cpy += vec.Py(); ++trks;
    } else if (vec.Pt() > nptMax) nptMax = vec.Pt();
    width += ROOT::Math::VectorUtil::DeltaR(vec, prt->momentum()) * vec.Pt();
  }
  prt->addInfo(LHCb::JetIDInfo::Ntracks, trks);
  prt->addInfo(LHCb::JetIDInfo::MTF, cptMax/pt);
  prt->addInfo(LHCb::JetIDInfo::MNF, nptMax/pt);
  prt->addInfo(LHCb::JetIDInfo::MPT, cptMax);
  prt->addInfo(LHCb::JetIDInfo::CPF, sqrt(cpx*cpx + cpy*cpy)/pt);
  prt->addInfo(LHCb::JetIDInfo::JetWidth, width);
}

// Perform jet energy correction.
void HltJetBuilder::jec(Particle *prt) {
  Gaudi::LorentzVector vec = prt->momentum();
  double pt(vec.Pt()/Gaudi::Units::GeV), eta(vec.Eta()), phi(vec.Phi()),
    cpf(prt->info(LHCb::JetIDInfo::CPF, 0));
  int iPvs(m_jecPvs - 1), iEta(0), iCpf(0), iPhi(0);
  phi = abs(abs(phi) - M_PI/2.0);
  if (iPvs > m_jecNPvs - 1) iPvs = m_jecNPvs - 1;
  if (iPvs < 0)             iPvs = 0;
  if (eta > m_jecLimEta[m_jecNEta]) eta = m_jecLimEta[m_jecNEta];
  if (cpf > m_jecLimCpf[m_jecNCpf]) cpf = m_jecLimCpf[m_jecNCpf];
  if (phi > m_jecLimPhi[m_jecNPhi]) phi = m_jecLimPhi[m_jecNPhi];
  if (pt  < m_jecLimPt[0])          pt  = m_jecLimPt[0];
  if (pt  > m_jecLimPt[m_jecNPt])   pt  = m_jecLimPt[m_jecNPt];
  while (eta > m_jecLimEta[iEta]) ++iEta; if (iEta > 0) --iEta;
  while (cpf > m_jecLimCpf[iCpf]) ++iCpf; if (iCpf > 0) --iCpf;
  while (phi > m_jecLimPhi[iPhi]) ++iPhi; if (iPhi > 0) --iPhi;
  TH1D* hst(m_jecs[iPvs][iEta][iCpf][iPhi]);
  double err(hst->GetBinError(hst->FindBin(pt)));
  double cor(hst->Interpolate(pt) + m_jetEcShift * err);
  prt->addInfo (LHCb::JECInfo::JEC , cor );
  prt->addInfo (LHCb::JECInfo::NPVsForJEC, m_jecPvs);
  prt->addInfo (LHCb::JECInfo::JECError, err);
  prt->setMomentum(cor*vec);
}

// Ban a Particle.
void HltJetBuilder::ban(const Particle *prt) {
  Particle::ConstVector dtrs;
  LoKi::Extract::getParticles(prt, back_inserter(dtrs), LoKi::Cuts::HASPROTO
			      && LoKi::Cuts::ISBASIC);
  for (Particle::ConstVector::iterator dtr = dtrs.begin(); dtr != dtrs.end();
       ++dtr) {
    const ProtoParticle *pro = (*dtr)->proto();
    if (pro->track()) m_trks.insert(pro->track());
    else if (pro->calo().size() > 0) {
      const CaloHypo::Clusters &cals = pro->calo()[0]->clusters();
      for (CaloHypo::Clusters::const_iterator cal = cals.begin();
	   cal != cals.end(); ++cal) m_cals.insert(*cal);
    }
  }
}

// Check if a Particle is banned.
bool HltJetBuilder::banned(const Particle *prt) {
  if (m_trks.size() == 0 && m_cals.size() == 0) return false;
  Particle::ConstVector dtrs;
  LoKi::Extract::getParticles(prt, back_inserter(dtrs), LoKi::Cuts::HASPROTO
			      && LoKi::Cuts::ISBASIC);
  for (Particle::ConstVector::iterator dtr = dtrs.begin(); dtr != dtrs.end();
       ++dtr) {
    const ProtoParticle *pro = (*dtr)->proto();
    if (pro->track()) return m_trks.find(pro->track()) != m_trks.end();
    else if (pro->calo().size() > 0) {
      const CaloHypo::Clusters &cals = pro->calo()[0]->clusters();
      for (CaloHypo::Clusters::const_iterator cal = cals.begin();
	   cal != cals.end(); ++cal)
	if (m_cals.find(*cal) != m_cals.end()) return true;
    }
  } return false;
}
