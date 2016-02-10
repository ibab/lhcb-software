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
// Constructor.
//=============================================================================
HltJetBuilder::HltJetBuilder(const string &name, ISvcLocator *svc) 
: GaudiHistoAlg(name , svc), m_fj(0) {
  // Declare the input/output properties.
  declareProperty("Inputs", m_inLocs,
		  "JB inputs of the form [<class name>, <object type>, "
		  "<location>].");
  declareProperty("Output", m_outLoc = "Phys/JB",
		  "JB output location prefix; objects will be placed in "
		  "<Output>/Particles and <Output>/Particle2Vertex.");

  // Declare the jet properties.
  declareProperty("JetPid", m_jetPid = 98,
		  "Particle ID to assign the jets.");
  declareProperty("JetPtMin", m_jetPtMin = 5 * Gaudi::Units::GeV,
		  "Minimum jet pT to build with FastJet.");
  declareProperty("JetR", m_jetR = 0.5,
		  "Jet radius parameter to use with FastJet.");
  declareProperty("JetSort", m_jetSort = 2,
		  "Jet sorting method: 1 E, 2 pT, or 3 y.");
  declareProperty("JetInfo", m_jetInfo = true,
		  "Write out extra info for the jets.");
  declareProperty("JetEcPath", m_jetEcPath = "JEC14",
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
  m_inPrts.clear(); m_inPrtsVrts.clear();
  for (unsigned int inLoc = 0; inLoc < m_inLocs.size(); ++inLoc) {
    Input in(&inLoc, &m_inLocs[inLoc]);
    if (!in.valid) warning() << in.warn << endmsg;
    else if (in.name == "Particle")        m_inPrts.push_back(in);
    else if (in.name == "Particle2Vertex") m_inPrtsVrts.push_back(in);
    else warning() << "Skipping input element " << inLoc 
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

  // Print the configuration, if requested.
  verbose() << "\n"
	    << "*-------------------------------------------------------*\n"
	    << "*              HltJetBuilder Configuration              *\n"
	    << "*-------------------------------------------------------*\n"
	    << "Inputs:\n"
	    << setw(15) << "class name" << setw(16) 
	    << "object type" << setw(40) << "location" << "\n";
  for (unsigned int inLoc = 0; inLoc < m_inLocs.size(); ++inLoc) {
    Input in(&inLoc, &m_inLocs[inLoc]); if (in.valid) in.print(verbose());}
  verbose() << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Execute.
//=============================================================================
StatusCode HltJetBuilder::execute() {
  // Clear the banned Tracks and CaloClusters from the previous event.
  m_trks.clear(); m_cals.clear();

  // Create the output containers.
  Particles *jets = new Particles();
  Particle2Vertex::WTable *jetsVrts(0);
  put(jets, m_outLoc + "/Particles");
  if (m_inPrtsVrts.size() > 0) {
    jetsVrts = new Particle2Vertex::WTable();
    put(jetsVrts, m_outLoc + "/Particle2Vertex");
  }

  // Load the Particle to RecVertex relations.
  vector<const Particle2Vertex::WTable*> prtsVrts;
  for (vector<Input>::iterator in = m_inPrtsVrts.begin();
       in != m_inPrtsVrts.end(); ++in)
    if (exist<Particle2Vertex::WTable>(in->loc))
      prtsVrts.push_back(get<Particle2Vertex::WTable>(in->loc));
    else warning() << "Could not retrieve " << in->name << "from "
		   << in->loc << "." << endmsg;
  
  // Assign the input Particles to RecVertices.
  VrtsPrts vrtsPrts; vrtsPrts[0] = vector<const Particle*>();
  for (vector<Input>::iterator in = m_inPrts.begin(); in != m_inPrts.end();
       ++in) {
    if (!exist<Particle::Range>(in->loc)) {
      warning() << "Could not retrieve " << in->name << "s from " 
		<< in->loc << "." << endmsg; continue;}
    Particle::Range prts = get<Particle::Range>(in->loc);
    if (in->ban) {
      for (Particle::Range::iterator prt = prts.begin(); prt != 
	     prts.end(); ++prt) ban(*prt);
      continue;}
    for (Particle::Range::iterator prt = prts.begin(); prt != prts.end(); 
	 ++prt) {
      if (banned(*prt)) continue;
      const VertexBase *vrt(0); double wgt(numeric_limits<double>::max());
      for (vector<const Particle2Vertex::WTable*>::iterator prtVrt = 
	     prtsVrts.begin(); prtVrt != prtsVrts.end(); ++prtVrt) {
	const Particle2Vertex::WTable::Range &vrts = (*prtVrt)->relations(*prt);
	if (vrts.size() > 0 && vrts[0].weight() < wgt) {
	  vrt = vrts[0].to(); wgt = vrts[0].weight();}
      }
      VrtsPrts::iterator vrtPrt = vrtsPrts.find(vrt);
      if (vrtPrt == vrtsPrts.end()) 
	vrtsPrts[vrt] = vector<const Particle*>(1, *prt);
      else vrtPrt->second.push_back(*prt);
    }
  }

  // Create the jets.
  vector<const Particle*> all = vrtsPrts[0];
  for (VrtsPrts::iterator vrtPrt = vrtsPrts.begin(); vrtPrt != vrtsPrts.end();
       ++vrtPrt) {
    if (vrtPrt->first)
      vrtPrt->second.insert(vrtPrt->second.end(), all.begin(), all.end());
    vector<Particle*> outs;
    if (m_fj->makeJets(vrtPrt->second.begin(), vrtPrt->second.end(), outs).
	isFailure()) return Error("FastJetMaker failed to build the jets.");

    // Add the jets.
    for (vector<Particle*>::iterator jet = outs.begin(); jet != outs.end();
	 ++jet) {
      // TO-DO: perform JEC if requested.
      // TO-DO: write info if requested.
      jets->insert(*jet);
      if (jetsVrts && vrtPrt->first) jetsVrts->relate(*jet, vrtPrt->first, 0);
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
  : valid(false), ban(false) {
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
  else if (s0 == "RecVertex" && s1 == "vertex");
  else if (s0 == "Particle2Vertex" && s1 == "table");
  else {warn += "incompatible object type, " + s1 + " for class " 
      + s0 +  "."; return;}
  name = s0; type = s1; loc = s2; valid = true;
}

// Print the input configuration to #msg.
void HltJetBuilder::Input::print(MsgStream &msg) {
  msg << setw(15) << name << setw(16) << type << setw(40) << loc << "\n";}

//=============================================================================
// Ban or check a Particle.
//=============================================================================
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
  }
  return false;
}
