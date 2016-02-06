// Local.
#include "HltParticleFlow.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltParticleFlow
//
// 2016-02-02 : Victor Coco, Cedric Potterat, and Philip Ilten
//-----------------------------------------------------------------------------

// Declare the algorithm factory.
DECLARE_ALGORITHM_FACTORY(HltParticleFlow)

//=============================================================================
// Constructor.
//=============================================================================
HltParticleFlow::HltParticleFlow(const string &name, ISvcLocator *svc) 
: GaudiHistoAlg(name , svc), m_prtSvc(0), m_prtSta(0), m_prtVrt(0), m_inNow(0) {
  // Declare the input/output properties.
  declareProperty("Inputs", m_inLocs,
		  "PF inputs of the form [<class name>, <object type>, "
		  "<location>].");
  declareProperty("Output", m_outLoc = "Phys/PF",
		  "PF output location prefix; objects will be placed in "
		  "<Output>/Particles, <Output>/ProtoParticles, etc.");

  // Declare the ProtoParticle properties.
  declareProperty("ProBestNames", m_proBestNames,
		  "Particle names to use when assigning the 'best' PID to "
		  "ProtoParticles.");
  declareProperty("ProBestKeys", m_proBestKeys,
		  "ExtraInfo keys to check when assigning the 'best' PID to "
		  "ProtoParticles.");
  declareProperty("ProBestMins", m_proBestMins,
		  "Minimum required ExtraInfo values when assigning the "
		  "'best' PID to ProtoParticles.");
  
  // Declare the track property members.
  declareProperty("TrkVlP", m_trkVlP = 1 * Gaudi::Units::MeV,
		  "Momentum to assign VELO Tracks.");
  declareProperty("TrkLnP", m_trkLnP = 0 * Gaudi::Units::MeV,
		  "Momentum to assign long Tracks with large curvature error.");
  declareProperty("TrkUpP", m_trkUpP = 0 * Gaudi::Units::MeV,
		  "Momentum to assign upstream Tracks with large curvature "
		  "error.");
  declareProperty("TrkDnP", m_trkDnP = 0 * Gaudi::Units::MeV,
		  "Momentum to assign downstream Tracks with large curvature "
		  "error.");
  declareProperty("TrkLnErrMax", m_trkLnErrMax = -1, 
		  "Fix momentum to long Tracks with curvature error above "
		  "this (-1 is +inf).");
  declareProperty("TrkUpErrMax", m_trkUpErrMax = -1, 
		  "Fix momentum for upstream Tracks with curvature error above "
		  "this (-1 is +inf).");
  declareProperty("TrkDnErrMax", m_trkDnErrMax = -1,
		  "Fix momentum for downstream Tracks with curvature error "
		  "above this (-1 is +inf).");

  // Declare the calorimeter property members.
  declareProperty("SprRecover", m_sprRecover = true, 
		  "Flag to perform neutral recovery using expected calorimeter "
		  "response.");
  declareProperty("SprPid", m_sprPid = 22,
		  "ID to assign SuperClusters from neutral recovery.");
  declareProperty("SprM", m_sprM = 0 * Gaudi::Units::MeV, 
		  "Mass to assign SuperClusters from neutral recovery.");
  declareProperty("EcalBest", m_ecalBest = true,
		  "Flag to only match the best ECAL CaloCluster for a Track.");
  declareProperty("HcalBest", m_hcalBest = true,
		  "Flag to only match the best HCAL CaloCluster for a Track.");
  declareProperty("EcalChi2Max", m_ecalChi2Max = 25,
		  "Maximum chi2 to match an ECAL CaloCluster with a Track "
		  "(-1 is +inf).");
  declareProperty("HcalChi2Max", m_hcalChi2Max = 25,
		  "Maximum chi2 to match an HCAL CaloCluster with a Track "
		  "(-1 is +inf).");
  declareProperty("EcalElcEStr", m_ecalElcEStr = 
		  "(-2.87675e-01)*exp(-1.89536e-04*x) + 8.86869e-01", 
		  "Function string for the ECAL energy electron response.");
  declareProperty("EcalHadEStr", m_ecalHadEStr = 
		  "(-3.79500e-01)*exp(-1.11353e-04*x) + 8.38989e-01",
		  "Function string for the ECAL energy charged hadron "
		  "response.");
  declareProperty("HcalElcEStr", m_hcalElcEStr =
		  "(-2.87675e-01)*exp(-1.89536e-04*x) + 8.86869e-01", 
		  "Function string for the HCAL energy electron response.");
  declareProperty("HcalHadEStr", m_hcalHadEStr = 
		  "(2.21942e-01)*exp(-7.76006e-05*x) + 3.51139e-01", 
		  "Function string for the HCAL energy charged hadron "
		  "response.");
  declareProperty("EcalHcalElcEStr", m_ecalHcalElcEStr = 
		  "(-2.87675e-01)*exp(-1.89536e-04*x) + 8.86869e-01", 
		  "Function string for the ECAL + HCAL energy electron "
		  "response.");
  declareProperty("EcalHcalHadEStr", m_ecalHcalHadEStr = "(-7.48397e-02 - "
		  "1.11391e-05*x - 2.51717e-09*x*x)*exp(-1.18558e-04*x) + "
		  "7.76469e-01",
		  "Function string for the ECAL + HCAL energy charged hadron "
		  "response.");
}

//=============================================================================
// Initialize.
//=============================================================================
StatusCode HltParticleFlow::initialize() {
  // Initialize the ParticlePropertySvc and Particle2State tools.
  if (!m_prtSvc && service("LHCb::ParticlePropertySvc", m_prtSvc).isFailure())
    return Error("Could not retrieve the ParticlePropertySvc tool.");
  if (!m_prtSta) m_prtSta = tool<IParticle2State>("Particle2State", this);
  if (!m_prtSta) return Error("Could not retrieve the Particle2State tool.");

  // Create the inputs.
  m_inPrts.clear(); m_inPros.clear(); m_inTrks.clear(); m_inCals.clear();
  for (unsigned int inLoc = 0; inLoc < m_inLocs.size(); ++inLoc) {
    Input in(&inLoc, &m_inLocs[inLoc], m_prtSvc);
    if (!in.valid) warning() << in.warn << endmsg;
    else if (in.name == "Particle")      m_inPrts.push_back(in);
    else if (in.name == "ProtoParticle") m_inPros.push_back(in);
    else if (in.name == "Track")         m_inTrks.push_back(in);
    else if (in.name == "CaloCluster")   m_inCals.push_back(in);
    else if (in.name == "RecVertex") {
      if (!m_inVrts.valid) m_inVrts = in;
      else warning() << "RecVertex location already specified, skipping input "
	     " element " << inLoc << "." << endmsg;
    } else if (in.name == "IClusTrTable2D") {
      if (in.ecal) {
	if (!m_inEcalTrks.valid) m_inEcalTrks = in;
	else warning() << "ECAL IClusTrTable2D location already specified, "
	       "skipping input element " << inLoc << "." << endmsg;
      } else {
	if (!m_inHcalTrks.valid) m_inHcalTrks = in;
	else warning() << "HCAL IClusTrTable2D location already specified, "
	       "skipping input element " << inLoc << "." << endmsg;
      }
    } else warning() << "Skipping input element " << inLoc 
		     << " with unknown class '" << in.name << "'." << endmsg;
  }
  
  // Initialize the IRelatedPVFinder, if needed.
  if (m_inVrts.valid) {
    if (!m_prtVrt) m_prtVrt = tool<IRelatedPVFinder>
		     ("GenericParticle2PVRelator<_p2PVWithIPChi2, "
		      "OfflineDistanceCalculatorName>/P2PVWithIPChi2", this);
    if (!m_prtVrt) return Error("Could not retrieve RelatedPVFinder tool.");
  }

  // Create the calorimeter response functions, if needed.
  if (m_sprRecover) {
    m_ecalElcE     = TF1("ecalElcE", m_ecalElcEStr.c_str());
    m_ecalHadE	   = TF1("ecalHadE", m_ecalHadEStr.c_str());
    m_hcalElcE     = TF1("hcalElcE", m_hcalElcEStr.c_str());
    m_hcalHadE     = TF1("hcalHadE", m_hcalHadEStr.c_str());
    m_ecalHcalElcE = TF1("ecalHcalElcE", m_ecalHcalElcEStr.c_str());
    m_ecalHcalHadE = TF1("ecalHcalHadE", m_ecalHcalHadEStr.c_str());
    if (!m_ecalElcE.IsValid())
      return Error("Invalid ECAL energy electron response function.");
    if (!m_ecalHadE.IsValid())
      return Error("Invalid ECAL energy charged hadron response function.");
    if (!m_hcalElcE.IsValid())
      return Error("Invalid HCAL energy electron response function.");
    if (!m_hcalHadE.IsValid())
      return Error("Invalid HCAL energy charged hadron response function.");
    if (!m_ecalHcalElcE.IsValid())
      return Error("Invalid HCAL + ECAL energy electron response function.");
    if (!m_ecalHcalHadE.IsValid())
      return Error("Invalid HCAL + ECAL energy charged hadron response "
		   "function.");
  }

  // Create the "best" PID for ProtoParticle vectors.
  m_proBestMs.clear(); m_proBestPids.clear();
  if (m_proBestNames.size() != m_proBestKeys.size() ||
      m_proBestNames.size() != m_proBestMins.size())
    return Error("The ProBestNames, ProBestKeys, and ProBestMins lists must "
		 "have the same size.");
  if (!m_proBestNames.size()) {
    m_proBestNames.push_back("pi+"); m_proBestMins.push_back(-1);
    m_proBestKeys.push_back(ProtoParticle::ProbNNpi);
  }
  for (unsigned int best = 0; best < m_proBestNames.size(); ++best) {
    const ParticleProperty *prp = m_prtSvc->find(m_proBestNames[best]);
    if (!prp) return Error("Unknown particle name '" + m_proBestNames[best] +
			   "' in ProBestNames.");
    m_proBestMs.push_back(prp->mass());
    m_proBestPids.push_back(prp->pid().pid()*(prp->charge() >= 0 ? 1 : -1));
  }

  // Print the configuration, if requested.
  verbose() << "\n"
	    << "*-------------------------------------------------------*\n"
	    << "*             HltParticleFlow Configuration             *\n"
	    << "*-------------------------------------------------------*\n"
	    << "Inputs:\n"
	    << setw(4) << "idx" << setw(15) << "class name" << setw(16) 
	    << "object type" << setw(40) << "location" << "\n";
  for (unsigned int inLoc = 0; inLoc < m_inLocs.size(); ++inLoc) {
    Input in(&inLoc, &m_inLocs[inLoc], m_prtSvc);
    if (in.valid && in.name != "RecVertex" && in.name != "IClusTrTable2D")
      in.print(verbose());
  }
  if (m_inVrts.valid) m_inVrts.print(verbose(), false);
  if (m_inEcalTrks.valid) m_inEcalTrks.print(verbose(), false);
  if (m_inHcalTrks.valid) m_inHcalTrks.print(verbose(), false);
  verbose() << "\nProtoParticle best PIDs:\n"
	    << setw(25) << "particle name" << setw(25) << "extra info key"
	    << setw(25) << "extra info minimum" << "\n";
  for (unsigned int best = 0; best < m_proBestNames.size(); ++best)
    verbose() << setw(25) << m_proBestNames[best] << setw(25) 
	      << m_proBestKeys[best] << setw(25) << m_proBestMins[best] << "\n";
  verbose() << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Execute.
//=============================================================================
StatusCode HltParticleFlow::execute() {
  // Clear the stored Tracks and CaloClusters from the previous event.
  m_trks.clear(); m_calsSprs.clear();

  // Create the output containers.
  m_prts = new Particles(); m_pros = new ProtoParticles();
  m_hyps = new CaloHypos(); m_cals = new CaloClusters();
  put(m_prts, m_outLoc + "/Particles");
  put(m_pros, m_outLoc + "/ProtoParticles");
  put(m_hyps, m_outLoc + "/CaloHypos");
  put(m_cals, m_outLoc + "/CaloClusters");
  if (m_prtVrt) {
    m_prtsVrts = new Particle2Vertex::WTable();
    put(m_prtsVrts, m_outLoc + "/Particle2Vertex");
  }

  // Retrieve the vertices, if needed.
  if (m_prtVrt) {
    RecVertices *vrts(0);
    if (retrieve(vrts, m_inVrts))
      m_vrts = VertexBase::ConstVector(vrts->begin(), vrts->end());
    else m_vrts.clear();
  }

  // Retrieve the Track to CaloCluster relation tables.
  m_trksEcal = 0; m_trksHcal = 0;
  if (m_inEcalTrks.valid && retrieve(m_ecalTrks, m_inEcalTrks))
    m_trksEcal = m_ecalTrks->inverse();
  if (m_inHcalTrks.valid && retrieve(m_hcalTrks, m_inHcalTrks))
    m_trksHcal = m_hcalTrks->inverse();

  // Add the inputs.
  vector<Input>::iterator inPrt(m_inPrts.begin()), inPro(m_inPros.begin()),
    inTrk(m_inTrks.begin()), inCal(m_inCals.begin());
  for (unsigned int inLoc = 0; inLoc < m_inLocs.size(); ++inLoc) {
    if (add<Particle>(&m_inPrts, inPrt, inLoc)) continue;
    if (inPro != m_inPros.end() && inLoc == inPro->index) {
      m_inNow = &(*inPro);
      if (exist<ProtoParticles>(inPro->loc)) {
	ProtoParticles *pros = get<ProtoParticles>(inPro->loc);
	for (ProtoParticles::iterator pro = pros->begin(); pro != pros->end();
	     ++pro) add(*pro, *inPro);
      } else warning() << "Could not retrieve " << inPro->name << " from " 
		       << inPro->loc << endmsg;
      ++inPro; continue;
    }
    if (add<Track>(&m_inTrks, inTrk, inLoc)) continue;
    if (add<CaloCluster>(&m_inCals, inCal, inLoc)) continue;
  }

  // Add the neutral recovery, if requested.
  if (m_sprRecover) {
    debug() << "Adding SuperClusters from neutral recovery." << endmsg;
    for (unordered_map<const CaloCluster*, SuperClusterPtr>::iterator calSpr =
	   m_calsSprs.begin(); calSpr != m_calsSprs.end(); ++calSpr) {
      if (!calSpr->second || calSpr->second->used || calSpr->second->vec.E() 
	  <= 0) {calSpr->second->used = true; continue;}
      create(calSpr->second);
      calSpr->second->used = true;
    }
  }
  setFilterPassed(true);
  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize.
//=============================================================================
StatusCode HltParticleFlow::finalize() {return StatusCode::SUCCESS;}

//=============================================================================
// Input structure class for PF.
//=============================================================================
// Constructor.
HltParticleFlow::Input::Input(const unsigned int *idx,
			      const vector<string> *prp, 
			      const IParticlePropertySvc *svc) 
  : valid(false), ban(false), dtrs(false), ecal(false), pid(0), m(0) {
  if (!idx) {warn += "Invalid pointer to index."; return;}
  warn = "Skipping input element " + to_string(*idx) + ", ";
  if (!prp) {warn += "invalid pointer to input list."; return;}
  if (!svc) {warn += "invalid pointer to particle service."; return;}
  if (prp->size() != 3) {warn += "input list size " + to_string(prp->size()) 
      + ", must be 3."; return;}

  // Set the members.
  string s0(prp->at(0)), s1(prp->at(1)), s2(prp->at(2));
  if (s2.empty()) {warn += "empty location."; return;}
  if (s0 != "RecVertex" && s0 != "IClusTrTable2D" && s1 == "ban") ban = true;
  else if (s0 == "Particle" && s1 == "particle")   dtrs = false;
  else if (s0 == "Particle" && s1 == "daughters")  dtrs = true;
  else if (s0 == "ProtoParticle"  && s1 == "best") m = -1;
  else if (s0 == "IClusTrTable2D" && s1 == "ecal") ecal = true;
  else if (s0 == "IClusTrTable2D" && s1 == "hcal") ecal = false;
  else if (s0 == "RecVertex" && s1 == "vertex");
  else if (s0 != "Particle") {
    const ParticleProperty *prp = svc->find(s1);
    if (!prp) {warn += "unknown particle name " + s1 + "."; return;} 
    m = prp->mass(); pid = prp->pid().pid()*(prp->charge() >= 0 ? 1 : -1);
  } else {warn += "incompatible object type, " + s1 + " for class " 
      + s0 +  "."; return;}
  name = s0; type = s1; loc = s2; index = *idx; valid = true;
}

// Print the input configuration to #msg.
void HltParticleFlow::Input::print(MsgStream &msg, bool idx) {
  if (idx) msg << setw(4) << index << setw(15) << name << setw(16) << type 
	       << setw(40) << loc << "\n";
  else msg << setw(4) << " " << setw(15) << name << setw(16) << type 
	   << setw(40) << loc << "\n";
}

//=============================================================================
// Neutral energy recovery cluster class for PF.
//=============================================================================
// Constructor.
HltParticleFlow::SuperCluster::SuperCluster() : 
  used(false), ecal(false), hcal(false), vec(0, 0, 0, 0), spr(0) {}

//=============================================================================
// Add an object to the PF output Particles.
//=============================================================================
// Add a Particle to the PF output Particles.
void HltParticleFlow::add(const Particle *prt, const Input &in) {
  Particle::ConstVector dtrs;
  LoKi::Extract::getParticles(prt, back_inserter(dtrs), LoKi::Cuts::HASPROTO
			      && LoKi::Cuts::ISBASIC);
  if (in.ban) {
    for (Particle::ConstVector::iterator dtr = dtrs.begin();
	 dtr != dtrs.end(); ++dtr) use((*dtr)->proto(), 0);
    return;
  }
  for (Particle::ConstVector::iterator dtr = dtrs.begin(); dtr != dtrs.end();
       ++dtr) if (used((*dtr)->proto())) return;
  for (Particle::ConstVector::iterator dtr = dtrs.begin(); dtr != dtrs.end();
       ++dtr) {
    if (in.dtrs) create(*dtr); use((*dtr)->proto(), m_sprRecover ? *dtr : 0);}
  if (!in.dtrs) create(prt);
}

// Add a ProtoParticle to the PF output Particles
void HltParticleFlow::add(const ProtoParticle *pro, const Input &in) {
  if (in.ban) {use(pro, 0); return;}
  if (used(pro)) return;
  double m(in.m); int pid(in.pid);
  if (m < 0) {
    if (pro->charge() == 0) {m = 0; pid = 22;}
    int best(m_proBestKeys.size() - 1); double max(-1), val(0);
    for (unsigned int idx = 0; idx < m_proBestKeys.size(); ++idx) {
      val = pro->info(m_proBestKeys[idx], -1);
      if (val < m_proBestMins[idx]) continue;
      if (val < max) continue;
      max = val; best = idx;
    } m = m_proBestMs[best]; pid = m_proBestPids[best];
  }
  pid *= pro->charge() >= 0 ? 1 : -1;
  Particle *prt(create(pro, pid, m));
  if (!prt) return;
  use(pro, m_sprRecover ? prt : 0);
  if (m_prtVrt && pro->track()) relate(prt);
}

// Add a Track to the PF output Particles.
void HltParticleFlow::add(const Track *trk, const Input &in) {
  if (in.ban) {use(trk, 0); return;}
  if (used(trk)) return;
  double m(in.m); int pid(in.pid*(trk->charge() >= 0 ? 1 : -1));
  Particle *prt(create(trk, pid, m));
  if (!prt) return;
  use(trk, m_sprRecover ? prt : 0);
  if (m_prtVrt) relate(prt);
}

// Add a CaloCluster to the PF output Particles.
void HltParticleFlow::add(const CaloCluster *cal, const Input &in) {
  if (in.ban) {use(cal, SuperClusterPtr(0)); return;}
  if (used(cal)) return;
  Particle *prt(create(cal, in.pid, in.m));
  if (!prt) return;
  use(cal, SuperClusterPtr(0));
}

//=============================================================================
// Create a Particle from an object to use in the PF output Particles.
//=============================================================================
// Create a Particle from a Particle.
Particle *HltParticleFlow::create(const Particle *prt) {
  Particle *out = new Particle(*prt);
  if (m_inNow) out->addInfo(Particle::FirstJetIndex, m_inNow->index);
  m_prts->insert(out);
  return out;
}

// Create a Particle from a ProtoParticle to use in the PF output Particles.
Particle *HltParticleFlow::create(const ProtoParticle *pro, const int &id,
				  const double &m) {
  Particle *prt(0);

  // ProtoParticle with a Track.
  if (pro->track()) {
    prt = new Particle(ParticleID(id * pro->charge()));
    const Track *trk = pro->track();
    State sta = trk->firstState();
    double p(-1); int type(trk->type());
    if (type == Track::Velo) p = m_trkVlP;
    else {
      double e(abs(sta.qOverP() / sqrt(sta.errQOverP2())));
      if (m_trkLnErrMax >= 0 && type == Track::Long && 
	  e > m_trkLnErrMax) p = m_trkLnP;
      else if (m_trkUpErrMax >= 0 && type == Track::Upstream && 
	       e > m_trkUpErrMax)   p = m_trkUpP;
      else if (m_trkDnErrMax>= 0 && type == Track::Downstream && 
	       e > m_trkDnErrMax) p = m_trkDnP;
    }
    if (p > 0) sta.setQOverP(pro->charge() / p);
    if (m_prtSta->state2Particle(sta, *prt).isFailure()) {delete prt; return 0;}

  // ProtoParticle with CaloClusters.
  } else if (pro->calo().size() > 0) {
    Gaudi::LorentzVector vec(0, 0, 0, 0);
    const CaloHypo::Clusters &cals = pro->calo()[0]->clusters();
    for (CaloHypo::Clusters::const_iterator cal = cals.begin();
	 cal != cals.end(); ++cal) sum(vec, *cal);
    if (m > vec.E() || vec.P() == 0) return prt;
    double n(sqrt((vec.E()*vec.E() - m*m)) / vec.P());
    vec.SetPx(vec.Px() * n); vec.SetPy(vec.Py() * n); vec.SetPz(vec.Pz() * n);
    prt = new Particle(ParticleID(id));
    prt->setMomentum(vec);
  } else return prt;
  prt->setProto(pro); prt->setMeasuredMass(m);
  if (m_inNow) prt->addInfo(Particle::FirstJetIndex, m_inNow->index);
  m_prts->insert(prt);
  return prt;
}

// Create a Particle from a Track to use in the PF output Particles.
Particle *HltParticleFlow::create(const Track *trk, const int &id, 
				  const double &m) {
  ProtoParticle *pro = new ProtoParticle();
  pro->setTrack(trk);
  Particle *prt = create(pro, id, m);
  if (!prt) {delete pro; return prt;}
  m_pros->insert(pro);
  return prt;
}

// Create a Particle from a CaloCluster to use in the PF output Particles.
Particle *HltParticleFlow::create(const CaloCluster *cal, const int &id,
				  const double &m) {
  CaloHypo *hyp = new CaloHypo();
  ProtoParticle *pro = new ProtoParticle();
  hyp->addToClusters(cal);
  pro->addToCalo(hyp);
  Particle *prt = create(pro, id, m);
  if (!prt) {delete hyp; delete pro; return prt;}
  m_pros->insert(pro);
  m_hyps->insert(hyp);
  return prt;
}

// Create a Particle from a SuperCluster to use in the PF output Particles.
Particle *HltParticleFlow::create(const SuperClusterPtr &spr) {
  CaloPosition pos;
  pos.setParameters(Gaudi::Vector3(spr->vec.X()/spr->vec.E(), 
				   spr->vec.Y()/spr->vec.E(), spr->vec.E()));
  pos.setCenter(Gaudi::Vector2(pos.x(), pos.y()));
  pos.setZ(spr->vec.Z()/spr->vec.E());
  CaloCluster *cal = new CaloCluster();
  cal->setPosition(pos);
  Particle *prt = create(cal, m_sprPid, m_sprM);
  if (!prt) {delete cal; return prt;}
  if (m_inNow) {
    prt->eraseInfo(Particle::FirstJetIndex);
    prt->addInfo(Particle::FirstJetIndex, m_inNow->index + 1);
  }
  m_cals->insert(cal);
  return prt;
}

//=============================================================================
// Mark the Tracks and CaloClusters associated to an object as used.
//=============================================================================
// Mark the Track or CaloClusters associated to a ProtoParticle as used.
void HltParticleFlow::use(const ProtoParticle *pro, const Particle *prt) {
  if (pro->track()) use(pro->track(), prt);
  else if (pro->calo().size() > 0) {
    const CaloHypo::Clusters &cals = pro->calo()[0]->clusters();
    for (CaloHypo::Clusters::const_iterator cal = cals.begin();
	 cal != cals.end(); ++cal) use(*cal, SuperClusterPtr(0));
  }
}

// Mark a Track and associated CaloClusters as used.
void HltParticleFlow::use(const Track *trk, const Particle *prt) {
  m_trks.insert(trk);
   if (trk->type() == Track::Velo) return;
  SuperClusterPtr spr(0); Gaudi::LorentzVector vec(0, 0, 0, 0);
  if (prt) spr = SuperClusterPtr(new SuperCluster());
  if (m_trksEcal) use(trk, vec, spr, spr ? &spr->ecal : 0,
		      m_trksEcal, m_ecalBest, m_ecalChi2Max);
  if (m_trksHcal) use(trk, vec, spr, spr ? &spr->hcal : 0,
		      m_trksHcal, m_hcalBest, m_hcalChi2Max);

  // Determine the expected Particle energy and update the SuperCluster.
  if (!prt || (!spr->ecal && !spr->hcal)) return;
  double e(0), p(prt->p());
  if (prt->particleID().abspid() == 11) {
    if (spr->ecal && spr->hcal) e += p*m_ecalHcalElcE.Eval(p);
    else if (spr->hcal)         e += p*m_hcalElcE.Eval(p);
    else                        e += p*m_ecalElcE.Eval(p);
  } else if (prt->charge() != 0 && prt->particleID().isHadron()) {
    if (spr->ecal && spr->hcal) e += p*m_ecalHcalHadE.Eval(p);
    else if (spr->hcal)         e += p*m_hcalHadE.Eval(p);
    else                        e += p*m_ecalHadE.Eval(p);
  }
  if (e < vec.E()) vec *= e/vec.E();
  spr->vec -= vec;
}

// Mark the CaloClusters associated to a Track as used.
void HltParticleFlow::use(const Track *trk, Gaudi::LorentzVector &vec, 
			  SuperClusterPtr &spr, bool *calo,
			  const Calo2Track::ITrClusTable *trksCals,
			  const bool &best, const double &chi2Max) {
  const Calo2Track::ITrClusTable::Range &cals = trksCals->relations(trk);
  if (cals.size() == 0) return;
  for (Calo2Track::ITrClusTable::Range::const_iterator cal = cals.begin();
       cal != cals.end(); ++cal) {
    if (chi2Max >= 0 && cal->weight() < chi2Max) continue;
    if (calo) {sum(vec, cal->to()); (*calo) = true;}
    use(cal->to(), spr);
    if (best) break;
  }
}

// Mark a CaloCluster as used and update (optional) associated SuperCluster.
void HltParticleFlow::use(const CaloCluster *cal, SuperClusterPtr spr) {
  unordered_map<const CaloCluster*, SuperClusterPtr>::iterator calSpr = 
    m_calsSprs.find(cal);
  if (calSpr == m_calsSprs.end()) {
    if (spr && cal) sum(spr->vec, cal); m_calsSprs[cal] = spr; return;}
  if (!spr) return;
  if (!calSpr->second) {calSpr->second = spr; return;}
  while (calSpr->second->spr) calSpr->second = calSpr->second->spr;
  if (spr != calSpr->second) {
    spr->vec += calSpr->second->vec; calSpr->second->used = true;
    calSpr->second->spr = spr;
  }
  calSpr->second = spr;
}

//=============================================================================
// Check if the Tracks and/or CaloClusters of an object are used.
//=============================================================================
// Check if a ProtoParticle is used.
bool HltParticleFlow::used(const ProtoParticle *pro) {
  if (pro->track()) return used(pro->track());
  else if (pro->calo().size() > 0) {
    const CaloHypo::Clusters &cals = pro->calo()[0]->clusters();
    for (CaloHypo::Clusters::const_iterator cal = cals.begin();
	 cal != cals.end(); ++cal) if (used(*cal)) return true;
    return false;
  } else return true;
}

// Check if a Track is used.
bool HltParticleFlow::used(const Track *trk) {
  return m_trks.find(trk) != m_trks.end();}

// Check if a CaloCluster is used.
bool HltParticleFlow::used(const CaloCluster *cal) {
  return m_calsSprs.find(cal) != m_calsSprs.end();}

//=============================================================================
// Additional methods.
//=============================================================================
// Sum a Gaudi::LorentzVector and a CaloCluster.
void HltParticleFlow::sum(Gaudi::LorentzVector &vec, const CaloCluster *cal) {
  vec.SetPx(vec.Px() + cal->e()*cal->position().x()); 
  vec.SetPy(vec.Py() + cal->e()*cal->position().y()); 
  vec.SetPz(vec.Pz() + cal->e()*cal->position().z()); 
  vec.SetE(vec.E() + cal->e());
}

// Relate a Particle with a Track to a RecVertex.
void HltParticleFlow::relate(const Particle *prt) {
  const Particle2Vertex::LightWTable table =
    m_prtVrt->relatedPVs(prt, m_vrts);
  const Particle2Vertex::LightWTable::Range range = table.relations();
  m_prtsVrts->merge(range);
}
