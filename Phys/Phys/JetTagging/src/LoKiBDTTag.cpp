// Implementation file for class BDTTag.
// Written by Philip Ilten and Mike Williams, 2015-01-30.

// Event.
#include "Event/MCParticle.h"

// Local.
#include "LoKiBDTTag.h"

// Declaration of tool factory.
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi, BDTTag)

//==========================================================================

// Jet tagging via the secondary vertex (SVR) method of LHCb-ANA-2014-074.

//--------------------------------------------------------------------------

// Standard constructor.

LoKi::BDTTag::BDTTag(const std::string& type, const std::string& name,
		     const IInterface* parent) : GaudiTool(type, name, parent) {
  declareInterface<IJetTagTool>(this);
  m_fitTool = 0; m_dstTool = 0; m_reader  = 0;
  declareProperty("Bdt0Weights", m_bdt0Weights = 
		  "data/bdt_configs/LHCb_ANA_2014_076_BDT0.weights.xml",
		  "BDT0 TMVA weights filename and path from $JETTAGGINGROOT.");
  declareProperty("Bdt1Weights", m_bdt1Weights = 
		  "data/bdt_configs/LHCb_ANA_2014_076_BDT1.weights.xml",
		  "BDT1 TMVA weights filename and path from $JETTAGGINGROOT.");
  declareProperty("TmvaOptions", m_tmvaOptions = "Silent",
		  "Options to pass to the TMVA reader.");
  declareProperty("FitName", m_fitName = "LoKi::VertexFitter",
		  "Name of the vertex fitter tool used to create SVRs.");
  declareProperty("DstName", m_dstName = "LoKi::DistanceCalculator:PUBLIC",
		  "Name of the distance calculator tool used to create SVRs.");
  declareProperty("DR", m_dr = 0.5,
		  "The maximum dR(SVR flight direction, jet momentum) "
		  "for linking two-body SVRs.");
  declareProperty("Backwards", m_backwards = false,
		  "If true, build backwards SVRs by reversing the SVR flight "
		  "direction.");
  declareProperty("PrtSelect", m_prtSelect = true,
		  "If true, apply the default selection to the particles.");
  declareProperty("NbvSelect", m_nbvSelect = true,
		  "If true, apply the default selection to the n-body SVRs.");
  declareProperty("NbvSort", m_nbvSort = "pt",
		  "Sort the n-body SVRs by \"pt\", \"bdt0\", or \"bdt1\".");
  declareProperty("TbvLocation", m_tbvLocation = "",
		  "Optional TES location of two-body SVRs. If not set, the "
		  "two-body SVRs will be automatically built.");
  declareProperty("PrtLocation", m_prtLocation = 
		  "Phys/StdAllNoPIDsPions/Particles",
		  "TES location of particles used to build the SVRs.");
  declareProperty("PvrLocation", m_pvrLocation = RecVertexLocation::Primary,
		  "TES location of PVRs used when building the SVRs.");
  declareProperty("OdnLocation", m_odnLocation = ODINLocation::Default,
		  "TES location of ODIN.");
}

//--------------------------------------------------------------------------

// Standard destructor.

LoKi::BDTTag::~BDTTag() {}

//--------------------------------------------------------------------------

// Initialize the tagger.

StatusCode LoKi::BDTTag::initialize() {

  // Set the run and event number.
  m_runNumber = -1; m_evtNumber = -1;

  // Get the tools.
  if (!GaudiTool::initialize()) {
    error() << "Failed to initialize the GaudiTool." << endmsg;
    return StatusCode::FAILURE;
  }
  m_fitTool = tool<IVertexFit>(m_fitName, this);
  if (!m_fitTool) {
    error() << "Failed to create the vertex fitter." << endmsg;
    return StatusCode::FAILURE;
  }
  m_dstTool = tool<IDistanceCalculator>(m_dstName, this);
  if (!m_fitTool) {
    error() << "Failed to create the distance calculator." << endmsg;
    return StatusCode::FAILURE;
  }

  // Create the TMVA reader.
  if (m_reader) delete m_reader;
  m_reader = new TMVA::Reader(m_tmvaOptions);
  if (!m_reader) {
    error() << "Failed to create the TMVA reader." << endmsg;
    return StatusCode::FAILURE;
  }

  // Add the variables.
  m_reader->AddVariable("FDR",   &m_vars.m_fdrMin);   
  m_reader->AddVariable("PTR",   &m_vars.m_ptSvrJet); 
  m_reader->AddVariable("NTRK",  &m_vars.m_nTrk);     
  m_reader->AddVariable("NTRKJ", &m_vars.m_nTrkJet);  
  m_reader->AddVariable("DR",    &m_vars.m_drSvrJet); 
  m_reader->AddVariable("Q",     &m_vars.m_absQSum);  
  m_reader->AddVariable("M",     &m_vars.m_m);        
  m_reader->AddVariable("MCOR",  &m_vars.m_mCor);     
  m_reader->AddVariable("FD",    &m_vars.m_fdChi2);   
  m_reader->AddVariable("SIP",   &m_vars.m_ipChi2Sum);

  // Book the two BDT methods.
  string root = getenv("JETTAGGINGROOT");
  string bdt0Weights = root + "/" + m_bdt0Weights;
  string bdt1Weights = root + "/" + m_bdt1Weights;
  ifstream bdt0File(bdt0Weights), bdt1File(bdt1Weights);
  if (!bdt0File.good()) {
    error() << "Cannot read the BDT0 file, " + bdt0Weights + "." << endmsg;
    return StatusCode::FAILURE;
  }
  if (!bdt1File.good()) {
    error() << "Cannot read the BDT1 file, " + bdt1Weights + "." << endmsg;
    return StatusCode::FAILURE;
  }
  m_reader->BookMVA("BDT0", bdt0Weights);
  m_reader->BookMVA("BDT1", bdt1Weights);
  return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------

// Finalize the tagger.

StatusCode LoKi::BDTTag::finalize() {
  if (m_reader) delete m_reader;
  return GaudiTool::finalize();
}

//--------------------------------------------------------------------------

// Calculate the tagger properties for the jet.

bool LoKi::BDTTag::calculateJetProperty(const Particle *jet, 
					map<string, double> &props) {
  
  // Create the n-body SVRs.
  if (!jet) {
    error() << "The passed jet is not valid." << endmsg; return false;
  }
  if (!nbvs(jet)) { 
    error() << "The n-body SVRs are not valid." << endmsg; return false;
  }

  // Fill the jet property information.
  int n(0);
  if (m_nbvSelect) 
    {if (m_nbvs.size() > 0 && m_nbvs[0].info(n, jet, props)) ++n;}
  else
    for (int nbv = 0; nbv < (int)m_nbvs.size(); ++nbv) 
      if (m_nbvs[nbv].info(n, jet, props)) ++n;
  props["Tag"] = n;
  if (n > 0) return true;
  else return false;
}

//--------------------------------------------------------------------------

// Calculate dR.

double LoKi::BDTTag::deltaR(const Particle *prt1, const Particle *prt2) {
  double phi1(prt1->momentum().Phi()), phi2(prt2->momentum().Phi());
  double dPhi(phi1 - phi2);
  while (dPhi > M_PI) dPhi -= 2*M_PI;
  while (dPhi <= -M_PI) dPhi += 2*M_PI;
  double dEta(prt1->momentum().Eta()- prt2->momentum().Eta());
  return sqrt(dPhi * dPhi + dEta * dEta);
}

double LoKi::BDTTag::deltaR(const Svr *svr, const Particle *prt, double sgn) {
  Gaudi::LorentzVector fv(sgn * svr->m_fv.X(), sgn * svr->m_fv.Y(),
			  sgn * svr->m_fv.Z(), 0);
  double phi1(fv.Phi()), phi2(prt->momentum().Phi());
  double dPhi(phi1 - phi2);
  while (dPhi > M_PI) dPhi -= 2*M_PI;
  while (dPhi <= -M_PI) dPhi += 2*M_PI;
  double dEta(fv.Eta()- prt->momentum().Eta());
  return sqrt(dPhi * dPhi + dEta * dEta);
}

//--------------------------------------------------------------------------

// Static comparison functions.

bool LoKi::BDTTag::comparePrtPt(const Particle *prt1, const Particle *prt2) {
  return prt1->momentum().Pt() > prt2->momentum().Pt();}
bool LoKi::BDTTag::compareSvrPt(const Svr &svr1, const Svr &svr2) {
  return svr1.m_prt.momentum().Pt() > svr2.m_prt.momentum().Pt();}
bool LoKi::BDTTag::compareSvrBdt0(const Svr &svr1, const Svr &svr2) {
  return svr1.m_bdt0 > svr2.m_bdt0;}
bool LoKi::BDTTag::compareSvrBdt1(const Svr &svr1, const Svr &svr2) {
  return svr1.m_bdt1 > svr2.m_bdt1;}

//--------------------------------------------------------------------------

// Return the vector of two-body SVRs.

const vector<LoKi::BDTTag::Svr> *LoKi::BDTTag::tbvs(bool force) {
  
  // Check if new event and if SVRs need re-building.
  const ODIN *odin = getIfExists<ODIN>(m_odnLocation);
  if (!odin)
    {error() << "Could not retrieve ODIN." << endmsg; m_tbvs.clear(); return 0;}
  if (!force && m_runNumber == (int)odin->runNumber() && 
      m_evtNumber == odin->eventNumber()) return &m_tbvs;
  m_runNumber = odin->runNumber();
  m_evtNumber = odin->eventNumber();
  m_tbvs.clear();

  // Retrieve the primary vertices.
  m_pvrs = getIfExists<RecVertices>(m_pvrLocation);
  if (!m_pvrs) {
    error() << "PVR location " + m_prtLocation + " not found." << endmsg;
    return 0;
  }

  // Create the two-body SVRs from the pre-built SVR TES location.
  if (m_tbvLocation != "") {
    Particles *svrs = getIfExists<Particles>(m_tbvLocation);
    if (!svrs) {
      error() << "SVR location " + m_tbvLocation + " not found." << endmsg;
      return 0;
    }
    for (Particles::iterator svr = svrs->begin(); svr != svrs->begin(); ++svr) {
      SmartRefVector<Particle> prts = (*svr)->daughters();
      if (prts.size() == 2) m_tbvs.push_back(Svr(this, prts[0], prts[1]));
    }
    return &m_tbvs;
  }

  // Retrieve the particles.
  m_prts = getIfExists<Particles>(m_prtLocation);
  if (!m_prts) {
    error() << "Particle location " + m_prtLocation + " not found." << endmsg;
    return 0;
  }

  // Select and sort the particles.
  // A lower minimum IP chi-squared requirement is applied for MC.
  MCParticles *gens   = getIfExists<MCParticles>(MCParticleLocation::Default);
  double ipChi2MinCut = !gens ? 16 : 13;
  if (m_prtSelect) {
    for (Particles::iterator prt = m_prts->begin(); prt != m_prts->end();) {
      const Track *trk((*prt)->proto() ? (*prt)->proto()->track() : 0);
      if (!trk)                             {m_prts->erase(prt); continue;}
      if (!(trk->pt() > 500))               {m_prts->erase(prt); continue;}
      if (!(trk->p() > 5000))               {m_prts->erase(prt); continue;}
      if (!(trk->type() == Track::Long))    {m_prts->erase(prt); continue;}
      if (!(trk->ghostProbability() < 0.2)) {m_prts->erase(prt); continue;}
      if (!(trk->chi2PerDoF() < 3))         {m_prts->erase(prt); continue;}

      // Calculate track minimum IP chi-squared.
      double ip(-1), ipChi2(-1), ipChi2Min(-1); 
      for (RecVertices::iterator pvr = m_pvrs->begin();
	   pvr != m_pvrs->end(); ++pvr) {
	m_dstTool->distance(*prt, *pvr, ip, ipChi2);
	if (ipChi2 >= 0 && (ipChi2Min == -1 || ipChi2 < ipChi2Min)) 
	  ipChi2Min = ipChi2;
      }
      if (!(ipChi2Min > ipChi2MinCut)) {m_prts->erase(prt); continue;}
      ++prt;
    }
  }
  sort(m_prts->begin(), m_prts->end(), comparePrtPt);
  
  // Create and select the two-body SVRs.
  for (Particles::iterator prt1 = m_prts->begin(); prt1 != m_prts->end();
       ++prt1)
    for (Particles::iterator prt2 = prt1 + 1; prt2 != m_prts->end(); ++prt2) 
      {Svr tbv(this, *prt1, *prt2); if (tbv.pass()) m_tbvs.push_back(tbv);}
  return &m_tbvs;
}

//--------------------------------------------------------------------------

// Return the vector of n-body SVRs.

const vector<LoKi::BDTTag::Svr> *LoKi::BDTTag::nbvs(const Particle *jet) {

  // Create the two-body SVRs.
  m_nbvs.clear();
  if (!tbvs()) {
    error() << "The two-body SVRs are not valid." << endmsg; return 0;
  }
  if (!jet) {error() << "The jet is not valid." << endmsg; return 0;}

  // Link the two-body SVRs.
  for (int tbv = 0; tbv < (int)m_tbvs.size(); ++tbv)
    if (deltaR(&m_tbvs[tbv], jet, m_backwards ? -1 : 1) <= m_dr) 
      m_nbvs.push_back(Svr(&m_tbvs[tbv]));
  double n(0);
  while (m_nbvs.size() != n) {
    n = m_nbvs.size();
    for (int nbv1 = 0; nbv1 < (int)m_nbvs.size(); ++nbv1)
      for (int nbv2 = nbv1 + 1; nbv2 < (int)m_nbvs.size();)
	if (m_nbvs[nbv1].link(m_nbvs[nbv2]))
	  m_nbvs.erase(m_nbvs.begin() + nbv2);
	else ++nbv2;
  }
  
  // Sort the n-body SVRs.
  for (int nbv = 0; nbv < (int)m_nbvs.size(); ++nbv) m_nbvs[nbv].calc(jet);
  if (m_nbvSort == "pt")
    sort(m_nbvs.begin(), m_nbvs.end(), compareSvrPt);
  else if (m_nbvSort == "bdt0")
    sort(m_nbvs.begin(), m_nbvs.end(), compareSvrBdt0);
  else if (m_nbvSort == "bdt1") 
    sort(m_nbvs.begin(), m_nbvs.end(), compareSvrBdt1);
  return &m_nbvs;
}

//==========================================================================

// Internal SVR representation for the LoKi::BDTTag jet tagger class.

//--------------------------------------------------------------------------

// Two-body constructor.

LoKi::BDTTag::Svr::Svr(BDTTag *parent, const Particle *prt1,
		       const Particle *prt2) {
  m_parent = parent;
  m_trks.push_back(prt1);
  m_trks.push_back(prt2);
  m_stored = false;
}

//--------------------------------------------------------------------------

// N-body constructor.

LoKi::BDTTag::Svr::Svr(const Svr *svr) {
  m_parent = svr->m_parent;
  m_trks   = svr->m_trks;
  if (svr->m_tbvs.size() == 0) m_tbvs.push_back(svr);
  else m_tbvs = svr->m_tbvs;
  m_stored = false;
}

//--------------------------------------------------------------------------

// Check if the two-body SVR passes the quality requirements.

bool LoKi::BDTTag::Svr::pass() {
  if (!calc())                          return false;
  if (!m_fit)                           return false;
  if (!(m_docaMin < 0.2))               return false;
  if (!(m_vrt.chi2() < 10))             return false;
  if (!(m_prt.momentum().M() > 400))    return false;
  if (!(m_prt.momentum().M() < 5279.4)) return false;
  return true;
}

//--------------------------------------------------------------------------

// Write the n-body SVR info, if it passes the quality requirements.

bool LoKi::BDTTag::Svr::info(int idx, const Particle *jet,
			     map<string, double> &props) {

  // Check if the SVR passes the n-body requirements.
  bool pass(true);
  if (!calc(jet)) return false;
  if (m_trks.size() == 2 && !(abs(m_prt.momentum().M() - 500.0) > 20))
    pass = false;
  else if (!(m_fdrMin < 15))                pass = false;
  else if (!(m_nTrkJet > 1))                pass = false;
  else if (!(m_vrt.position().Z() < 200))   pass = false;
  else if (!(m_mCor > 600))                 pass = false;
  else if (!(m_prt.momentum().Pt() > 2000)) pass = false;
  else if (!(m_tau < 1.5))                  pass = false;
  else if (!(m_fdChi2 > 32))                pass = false;
  else if (!(m_drSvrJet <= m_parent->m_dr)) pass = false;
  if (m_parent->m_nbvSelect && !pass) return false;

  // Fill the BDT properties.
  stringstream pre; pre << "Tag" << idx << "_";
  props[pre.str() + "fdrMin"] 	 = m_fdrMin;
  props[pre.str() + "ptSvrJet"]  = m_ptSvrJet;
  props[pre.str() + "nTrk"]	 = m_trks.size();
  props[pre.str() + "nTrkJet"]   = m_nTrkJet;
  props[pre.str() + "drSvrJet"]  = m_drSvrJet;
  props[pre.str() + "absQSum"]   = m_absQSum;
  props[pre.str() + "m"]	 = m_prt.momentum().M();
  props[pre.str() + "mCor"]      = m_mCor;
  props[pre.str() + "fdChi2"]    = m_fdChi2;
  props[pre.str() + "ipChi2Sum"] = m_ipChi2Sum;

  // Fill the additional properties.
  props[pre.str() + "bdt0"]      = m_bdt0;
  props[pre.str() + "bdt1"]      = m_bdt1;
  props[pre.str() + "pass"]      = pass;
  props[pre.str() + "tau"]       = m_tau;
  props[pre.str() + "z"]         = m_vrt.position().Z();
  props[pre.str() + "pt"]        = m_prt.momentum().Pt();
  props[pre.str() + "backwards"] = m_parent->m_backwards;
  return true;
}

//--------------------------------------------------------------------------

// Calculate the stored variables. Returns true if successful.

bool LoKi::BDTTag::Svr::calc(const Particle *jet, bool force) {
  if (m_stored && !force) return m_stored;
  else m_stored = false;
  m_fv.SetXYZ(0, 0, 0);
  m_fdrMin    = -1; m_absQSum   = 0;  m_ipMin     = -1; m_dsMax     = -1;  
  m_ptSvrJet  = -1; m_mCor      = -1; m_ipChi2Min = -1;	m_tau       = -1;  
  m_drSvrJet  = -1; m_fdChi2    = -1; m_docaMin   = -1;	m_bdt0      = -999;
  m_nTrkJet   = 0;  m_ipChi2Sum = 0;  m_drMax     = -1;	m_bdt1      = -999;
  if (!m_parent || !m_parent->m_fitTool || !m_parent->m_dstTool ||
      !m_parent->m_pvrs) return m_stored;
  else m_stored = true;
  m_fit = m_parent->m_fitTool->fit(m_trks, m_vrt, m_prt).isSuccess();

  // Determine position using two-body chi-squared weighted positions.
  TVector3 svrPos(0, 0, 0);
  if (m_tbvs.size() < 2)
    svrPos.SetXYZ(m_vrt.position().X(), m_vrt.position().Y(), 
		  m_vrt.position().Z());
  else {
    double sumWeights(0), weight(0);
    for (int tbv1 = 0; tbv1 < (int)m_tbvs.size(); ++tbv1) {
      weight      = 1.0 / m_tbvs[tbv1]->m_vrt.chi2();
      sumWeights += weight;
      svrPos += TVector3(m_tbvs[tbv1]->m_vrt.position().X(), 
			 m_tbvs[tbv1]->m_vrt.position().Y(),
			 m_tbvs[tbv1]->m_vrt.position().Z()) * weight;

      // Minimum two-body radial flight distance.
      if (m_fdrMin == -1 || m_tbvs[tbv1]->m_fdrMin < m_fdrMin) 
	m_fdrMin = m_tbvs[tbv1]->m_fdrMin;

      // Minimum two-body separation and dR.
      for (int tbv2 = tbv1 + 1; tbv2 < (int)m_tbvs.size(); ++tbv2) {
	double ds = sqrt((m_tbvs[tbv1]->m_vrt.position() - 
			  m_tbvs[tbv2]->m_vrt.position()).Mag2());
	double dr = deltaR(&m_tbvs[tbv1]->m_prt, &m_tbvs[tbv2]->m_prt);
	if (ds > m_dsMax) m_dsMax = ds;
	if (dr > m_drMax) m_drMax = dr;
      }
    }
    svrPos *= (1.0 / sumWeights);
    m_vrt.setPosition(Gaudi::XYZPoint(svrPos.X(), svrPos.Y(), svrPos.Z()));
    m_prt.setReferencePoint(Gaudi::XYZPoint(svrPos.X(), svrPos.Y(),
					    svrPos.Z()));
  }

  // Correct the momentum.
  Gaudi::LorentzVector pv4(0, 0, 0, 0);
  for (int trk = 0; trk < (int)m_trks.size(); ++trk)
    pv4 += m_trks[trk]->momentum();
  m_prt.setMomentum(pv4);

  // Impact parameter and flight distance information.
  TVector3 pvrPos(0, 0, 0); double ip(-1), ipChi2(-1), fd(-1), fdr(-1);
  for (RecVertices::iterator pvr = m_parent->m_pvrs->begin(); 
       pvr != m_parent->m_pvrs->end(); ++pvr) {
    m_parent->m_dstTool->distance(&m_prt, *pvr, ip, ipChi2);
    if (ipChi2 >= 0 && (m_ipChi2Min == -1 || ipChi2 < m_ipChi2Min)) 
      m_ipChi2Min = ipChi2;
    if (ip >= 0 && (m_ipMin == -1 || ip < m_ipMin)) {
      m_pvr = *pvr;
      pvrPos.SetXYZ((*pvr)->position().X(), (*pvr)->position().Y(),
		    (*pvr)->position().Z());
      m_ipMin = ip;
      m_parent->m_dstTool->distance(&m_vrt, *pvr, fd, m_fdChi2);
    }
    fdr = sqrt((m_vrt.position() - (*pvr)->position()).perp2());
    if (m_tbvs.size() < 2 && (m_fdrMin == -1 || fdr < m_fdrMin)) m_fdrMin = fdr;
  }

  // DOCA information.
  double doca;
  for (int trk1 = 0; trk1 < (int)m_trks.size(); ++trk1)
    for (int trk2 = trk1 + 1; trk2 < (int)m_trks.size(); ++trk2)
      if (m_parent->m_dstTool->
	  distance(m_trks[trk1], m_trks[trk2], doca).isSuccess())
  	if (m_docaMin == -1 || doca < m_docaMin) m_docaMin = doca;

  // IP chi-squared and absolute charge sums.
  for (int trk = 0; trk < (int)m_trks.size(); ++trk) {
    double min(-1), ip, chi2;
    m_absQSum += m_trks[trk]->charge();
    for (RecVertices::iterator pvr = m_parent->m_pvrs->begin(); 
	 pvr != m_parent->m_pvrs->end(); ++pvr) {
      m_parent->m_dstTool->distance(m_trks[trk], *pvr, ip, chi2);
      if (chi2 >= 0 && (min == -1 || chi2 < min)) min = chi2;
    }
    m_ipChi2Sum += min;
  }
  m_absQSum = abs(m_absQSum);
  
  // Corrected mass.
  m_fv = svrPos - pvrPos;
  TVector3 pv3(pv4.Px(), pv4.Py(), pv4.Pz());
  double pt2 = pv3.Cross(m_fv.Unit()).Mag2();
  m_mCor = sqrt(pv4.M2() + pt2) + sqrt(pt2);
  m_tau  = m_fv.Mag() / pv3.Mag() * 1000.0;

  // Calculate jet variables.
  if (!jet) return m_stored;
  m_ptSvrJet = m_prt.momentum().Pt() / jet->momentum().Pt();
  m_drSvrJet = deltaR(this, jet, m_parent->m_backwards ? -1 : 1);
  for (int trk = 0; trk < (int)m_trks.size(); ++trk)
    if (deltaR(m_trks[trk], jet) <= m_parent->m_dr) ++m_nTrkJet;

  // Fill the BDT structure.
  m_parent->m_vars.m_fdrMin    = m_fdrMin;
  m_parent->m_vars.m_ptSvrJet  = m_ptSvrJet;
  m_parent->m_vars.m_nTrk      = m_trks.size();
  m_parent->m_vars.m_nTrkJet   = m_nTrkJet;
  m_parent->m_vars.m_drSvrJet  = m_drSvrJet;
  m_parent->m_vars.m_absQSum   = m_absQSum;
  m_parent->m_vars.m_m         = m_prt.momentum().M();
  m_parent->m_vars.m_mCor      = m_mCor;
  m_parent->m_vars.m_fdChi2    = m_fdChi2;
  m_parent->m_vars.m_ipChi2Sum = m_ipChi2Sum;

  // Calculate the BDT responses.
  double bdt0(m_parent->m_reader->EvaluateMVA("BDT0"));
  double bdt1(m_parent->m_reader->EvaluateMVA("BDT1"));
  if (bdt0 != -999) 
    m_bdt0 = (1 - TMath::Prob(pow(abs(bdt0)/0.2, 2), 1)) * (bdt0 < 0 ? -1 : 1);
  if (bdt1 != -999)
    m_bdt1 = (1 - TMath::Prob(pow(abs(bdt1)/0.2, 2), 1)) * (bdt1 < 0 ? -1 : 1);
  return m_stored;
}

//--------------------------------------------------------------------------

// Link an n-body SVR with another n-body SVR, failure returns false.

bool LoKi::BDTTag::Svr::link(Svr &nbv) {
  if (m_tbvs.size() == 0 || nbv.m_tbvs.size() == 0) return false;
  set<const Particle*> cmb;
  for (int trk = 0; trk < (int)m_trks.size(); ++trk)
    cmb.insert(m_trks[trk]);
  for (int trk = 0; trk < (int)nbv.m_trks.size(); ++trk)
    cmb.insert(nbv.m_trks[trk]);
  if (cmb.size() >= m_trks.size() + nbv.m_trks.size()) return false;
  m_trks.clear();
  for (set<const Particle*>::iterator trk = cmb.begin(); trk != cmb.end();
       ++trk) m_trks.push_back(*trk);
  for (int tbv = 0; tbv < (int)nbv.m_tbvs.size(); ++tbv) 
    m_tbvs.push_back(nbv.m_tbvs[tbv]);
  m_stored = false;
  return true;
}

//==========================================================================
