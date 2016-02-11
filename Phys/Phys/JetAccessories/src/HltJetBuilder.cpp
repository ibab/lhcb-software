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
: GaudiHistoAlg(name , svc), 
  m_lokBpvX(LoKi::Cuts::BPV(LoKi::Cuts::VX)),
  m_lokBpvY(LoKi::Cuts::BPV(LoKi::Cuts::VY)),
  m_lokBpvZ(LoKi::Cuts::BPV(LoKi::Cuts::VZ)),
  m_lokBpvPhi(LoKi::Particles::PhiWithTheBestPV()),
  m_lokBpvEta(LoKi::Particles::PseudoRapidityWithTheBestPV()),
  m_lokTrkN(LoKi::Cuts::NINTREE(LoKi::Cuts::HASTRACK)),
  m_fj(0) {
  // Declare the input/output properties.
  declareProperty("Inputs", m_inLocs,
		  "JB inputs of the form [<class name>, <object type>, "
		  "<location>].");
  declareProperty("Output", m_outLoc = "Phys/JB",
		  "JB output location prefix; objects will be placed in "
		  "<Output>/Particles, etc.");

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
  // Create the inputs.
  m_inPrts.clear(); m_inTags.clear();
  for (unsigned int inLoc = 0; inLoc < m_inLocs.size(); ++inLoc) {
    Input in(&inLoc, &m_inLocs[inLoc]);
    if (!in.valid) warning() << in.warn << endmsg;
    else if (in.name == "Particle") {
      if (in.ban)      m_inBans.push_back(in);
      else if (in.tag) m_inTags.push_back(in);
      else             m_inPrts.push_back(in);
    } else if (in.name == "RecVertex") {
      if (!m_inVrts.valid) m_inVrts = in;
      else warning() << "RecVertex location already specified, skipping input "
	     " element " << inLoc << "." << endmsg;
    } else warning() << "Skipping input element " << inLoc 
		     << " with unknown class '" << in.name << "'." << endmsg;
  }

  // Retrieve the FastJet tool and configure.
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

  // Print the configuration, if requested.
  verbose() << "\n"
	    << "*-------------------------------------------------------*\n"
	    << "*              HltJetBuilder Configuration              *\n"
	    << "*-------------------------------------------------------*\n"
	    << "Inputs:\n"
	    << setw(4) << "idx" << setw(15) << "class name" << setw(16) 
	    << "object type" << setw(40) << "location" << "\n";
  for (unsigned int inLoc = 0; inLoc < m_inLocs.size(); ++inLoc) {
    Input in(&inLoc, &m_inLocs[inLoc]);
    if (in.valid && in.name != "RecVertex") in.print(verbose());
  }
  if (m_inVrts.valid) m_inVrts.print(verbose(), false);
  verbose() << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Execute.
//=============================================================================
StatusCode HltJetBuilder::execute() {
  // Clear previous event and create output containers.
  m_trks.clear(); m_cals.clear();
  m_jets = new Particles();
  put(m_jets, m_outLoc + "/Particles");

  // Ban Tracks and CaloClusters from Particles.
  for (vector<Input>::iterator in = m_inBans.begin(); in != m_inBans.end(); 
       ++in) {
    if (!exist<Particle::Range>(in->loc)) {
      warning() << "Could not retrieve " << in->name << "s from " 
		<< in->loc << "." << endmsg; continue;}
    Particle::Range prts = get<Particle::Range>(in->loc);
    for (Particle::Range::iterator prt = prts.begin(); prt != prts.end();
	 ++prt) ban(*prt);
  }

  // Assign the input Particles by best PV position.
  vector<const Particle*> cntrPrts; VrtsPrts vrtsPrts;
  for (vector<Input>::iterator in = m_inPrts.begin(); in != m_inPrts.end();
       ++in) {
    if (!exist<Particle::Range>(in->loc)) {
      warning() << "Could not retrieve " << in->name << "s from " 
		<< in->loc << "." << endmsg; continue;}
    Particle::Range prts = get<Particle::Range>(in->loc);
    for (Particle::Range::iterator prt = prts.begin(); prt != prts.end(); 
	 ++prt) {
      if (banned(*prt)) continue;
      if (!m_jetVrt || m_lokTrkN(*prt) > 0) cntrPrts.push_back(*prt);
      else {
	Gaudi::XYZPoint vrt(m_lokBpvX(*prt), m_lokBpvY(*prt), m_lokBpvZ(*prt));
	VrtsPrts::iterator vrtPrt = vrtsPrts.find(vrt);
	if (vrtPrt == vrtsPrts.end()) 
	  vrtsPrts[vrt] = vector<const Particle*>(1, *prt);
	else vrtPrt->second.push_back(*prt);
      }
    }
  }

  // Determine the number of PVs.
  if (m_jecPvs >= 0) {
    if (m_inVrts.valid) {
      if (exist<RecVertices>(m_inVrts.loc))
	m_jecPvs = get<RecVertices>(m_inVrts.loc)->size();
      else warning() << "Could not retrieve " << m_inVrts.name << "from "
		     << m_inVrts.loc << "." << endmsg;
    } else m_jecPvs = vrtsPrts.size();
  }

  // Build the jets.
  if (!m_jetVrt) build(&cntrPrts, 0);
  else {
    for (VrtsPrts::iterator vrtPrt = vrtsPrts.begin(); vrtPrt != vrtsPrts.end();
	 ++vrtPrt) {
      vrtPrt->second.insert(vrtPrt->second.end(), cntrPrts.begin(),
			    cntrPrts.end());
      build(&vrtPrt->second, &vrtPrt->first);
    }
  }

  // Tag the jets.
  for (vector<Input>::iterator in = m_inTags.begin(); in != m_inTags.end();
       ++in) {
    if (!exist<Particle::Range>(in->loc)) {
      warning() << "Could not retrieve " << in->name << "s from " 
		<< in->loc << "." << endmsg; continue;}
    Particle::Range prts = get<Particle::Range>(in->loc);
    for (Particle::Range::iterator prt = prts.begin(); prt != prts.end(); 
	 ++prt) {
      double prtPhi((*prt)->momentum().Phi()), prtEta((*prt)->momentum().Eta());
      if ((*prt)->endVertex()) {
	prtPhi = m_lokBpvPhi(*prt); prtEta = m_lokBpvEta(*prt);}
      for (Particles::iterator jet = m_jets->begin(); jet != m_jets->end();
	   ++jet) {
	double jetPhi((*jet)->momentum().Phi());
	double jetEta((*jet)->momentum().Eta());
	double dphi(prtPhi - jetPhi), deta(prtEta - jetEta);
	while (dphi > M_PI) dphi -= 2*M_PI;
	while (dphi <= -M_PI) dphi += 2*M_PI;
	bool tag = (*jet)->hasInfo(Particle::FirstJetIndex + 100 + in->index);
	if (!tag && sqrt(dphi*dphi + deta*deta) < m_jetR)
	  (*jet)->addInfo(Particle::FirstJetIndex + 100 + in->index, 1);
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
// Input structure class for JB.
//=============================================================================
// Constructor.
HltJetBuilder::Input::Input(const unsigned int *idx, const vector<string> *prp)
  : valid(false), ban(false), tag(false) {
  if (!idx) {warn += "Invalid pointer to index."; return;}
  warn = "Skipping input element " + to_string(*idx) + ", ";
  if (!prp) {warn += "invalid pointer to input list."; return;}
  if (prp->size() != 3) {warn += "input list size " + to_string(prp->size()) 
      + ", must be 3."; return;}

  // Set the members.
  string s0(prp->at(0)), s1(prp->at(1)), s2(prp->at(2));
  if (s2.empty()) {warn += "empty location."; return;}
  if (s0 == "Particle" && s1 == "ban") ban = true;
  else if (s0 == "Particle" && s1 == "use") ban = false;
  else if (s0 == "Particle" && s1 == "tag") tag = true;
  else if (s0 == "RecVertex" && s1 == "vertex");
  else {warn += "incompatible object type, " + s1 + " for class " 
      + s0 +  "."; return;}
  name = s0; type = s1; loc = s2; index = *idx; valid = true;
}

// Print the input configuration to #msg.
void HltJetBuilder::Input::print(MsgStream &msg, bool idx) {
  if (idx) msg << setw(4) << index << setw(15) << name << setw(16) << type 
	       << setw(40) << loc << "\n";
  else msg << setw(4) << " " << setw(15) << name << setw(16) << type 
	   << setw(40) << loc << "\n";
}

//=============================================================================
// Additional methods.
//=============================================================================
// Build jets from #prts and add to the TES.
void HltJetBuilder::build(const vector<const Particle*> *prts,
			  const Gaudi::XYZPoint *vrt) {
  vector<Particle*> jets;
  if (m_fj->makeJets(prts->begin(), prts->end(), jets).isFailure()) return;
  for (vector<Particle*>::iterator jet = jets.begin(); jet != jets.end();
       ++jet) {
    if (m_jetInfo || m_jecPvs >= 0) info(*jet);
    if (m_jecPvs >= 0) jec(*jet);
    if (vrt) (*jet)->setReferencePoint(*vrt);
    else (*jet)->setReferencePoint(Gaudi::XYZPoint(0, 0, 0));
    if ((*jet)->momentum().Pt() >= m_jetPtMin) m_jets->insert(*jet);
    else {delete (*jet)->endVertex(); delete *jet;}
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
