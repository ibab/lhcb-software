// Include files:
// STL
#include <string>
#include <vector>
// GSL
#include "gsl/gsl_math.h"
//from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IAlgTool.h"
//from Kernel
#include "Kernel/DVAlgorithm.h"
// from Event 
#include "Event/Track.h"
#include "Event/RecVertex.h"
// Local
#include "bJetSeeds.h"

using namespace LHCb;
using namespace Gaudi::Units;

//-----------------------------------------------------------------------------
// Implementation file for class : bJetSeeds
// 2011-11-26 : Marcin Kucharczyk
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY(bJetSeeds);

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
bJetSeeds::bJetSeeds(const std::string& name,
                   ISvcLocator* pSvcLocator) 
  : DVAlgorithm(name,pSvcLocator)
{
  declareProperty("InputPVs",m_inputPVsName = 
                             LHCb::RecVertexLocation::Primary);
  declareProperty("VertexFitter",   m_VrtxFitterName = "OfflineVertexFitter");
  declareProperty("NrPVs",          m_nrPVs     = 1);
  declareProperty("NrSeeds",        m_nrSeeds   = 2);
  declareProperty("TrkChi2",        m_trkChi2   = 3.0);
  declareProperty("PrtMom",         m_ptrMom    = 2.0);
  declareProperty("PrtPt",          m_prtPt     = 0.6);
  declareProperty("PrtIPS",         m_prtIPS    = 2.5);
  declareProperty("DZSVPV",         m_dZSVPV    = 1.0);
  declareProperty("SumMomSV",       m_sumMomSV  = 1.0);
  declareProperty("VtxChi2",        m_vtxChi2   = 30.0);
  m_DaVinciAlg = 0;
  m_DOCA = 0;
}

//=============================================================================
// Destructor
//=============================================================================
bJetSeeds::~bJetSeeds() {}; 

//=============================================================================
// Initialisation
//=============================================================================
StatusCode bJetSeeds::initialize() {
  StatusCode sc = DVAlgorithm::initialize();
  if (!sc) return sc;
  if(msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;
  // Parent DVAlgorithm
  m_DaVinciAlg = Gaudi::Utils::getIDVAlgorithm(contextSvc());
  if(!m_DaVinciAlg) {
    error() << "Couldn't get parent DVAlgorithm" << endreq;
    return StatusCode::FAILURE;
  }
  // DOCA distance calculator
  m_DOCA = m_DaVinciAlg->distanceCalculator();
  if(!m_DOCA) {
    error() << "Unable to retrieve DistanceCalculator tool" << endreq;
    return StatusCode::FAILURE;
  }
  // Vertex fitter
  fitterSV = tool<IVertexFit>(m_VrtxFitterName,this);
  if(!fitterSV) {
    error() << "Unable to retrieve VertexFitter tool" << endreq;
    return StatusCode::FAILURE;
  } 

  return StatusCode::SUCCESS;
}

//=============================================================================
// Execution
//=============================================================================
StatusCode bJetSeeds::execute()
{
  if(msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;
  setFilterPassed(false);
  if(!exist<RecVertices>(m_inputPVsName)) return StatusCode::SUCCESS;
  // Primary vertices
  m_inputPVs = get<LHCb::RecVertices>(m_inputPVsName);
  if(!(m_inputPVs->size())) return StatusCode::SUCCESS;
  if(m_inputPVs->size() > m_nrPVs) return StatusCode::SUCCESS;
  // Particles
  Particle::ConstVector inputParts = this->i_particles();
  // Create seeds --------------------
  std::vector<Vertex> seed3Vec = build3PartSV(inputParts);
  if(seed3Vec.size() >= m_nrSeeds) setFilterPassed(true);
  
  return StatusCode::SUCCESS;
}


//=============================================================================
// Finalization   
//=============================================================================
StatusCode bJetSeeds::finalize() {
  if(msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;
  return DVAlgorithm::finalize();
}


//============================================================================
// Build 3-particles secondary vertex
//============================================================================
std::vector<Vertex> bJetSeeds::build3PartSV(
                               Particle::ConstVector& inputPartsVec)
{
  StatusCode sc;
  std::vector<Vertex> seedVrtcs(0);
  std::vector<Vertex> uniqueVrtcs(0);
  // Loop over all reconstructed PV's
  for(std::vector<LHCb::RecVertex*>::const_iterator iPV = m_inputPVs->begin();
      iPV != m_inputPVs->end(); ++iPV) {
    const RecVertex* recPV = (*iPV);
    Particle::ConstVector::const_iterator ip1, ip2, ip3;
    double p1IP, p1IPErr, p2IP, p2IPErr, p3IP, p3IPErr;
    Vertex sVtx;
    double zPV = recPV->position().z() / Gaudi::Units::mm;
    for(ip1 = inputPartsVec.begin(); ip1 != inputPartsVec.end(); ip1++) {
      //---------- 1st particle for seed
      if((*ip1)->charge() == 0) continue;
      if((*ip1)->proto() == NULL) continue;
      if((*ip1)->proto()->track() == NULL) continue;
      if((*ip1)->proto()->track()->type() != Track::Long) continue;
      if((*ip1)->proto()->track()->chi2PerDoF() > m_trkChi2) continue;
      // Impact parameter
      sc = partIPwrtPV(*ip1,recPV,p1IP,p1IPErr);
      // Cuts on 1st particle parameters
      if((*ip1)->p() / Gaudi::Units::GeV < m_ptrMom) continue;
      if((*ip1)->pt() / Gaudi::Units::GeV < m_prtPt) continue;
      if((*ip1)->charge() == 0) continue;
      if(p1IP / p1IPErr < m_prtIPS) continue;
      //---------- 2nd particle for seed
      for(ip2 = (ip1+1); ip2 != inputPartsVec.end(); ip2++) {
        if((*ip2)->proto() == NULL ) continue;
        if((*ip2)->proto()->track() == NULL) continue; 
        if((*ip2)->proto()->track()->type() != Track::Long) continue;
        if((*ip2)->proto()->track()->chi2PerDoF() > m_trkChi2) continue;
        // Impact parameter
        sc = partIPwrtPV(*ip2,recPV,p2IP,p2IPErr);
        // Cuts on 2nd particle parameters
        if((*ip2)->p() / Gaudi::Units::GeV < m_ptrMom) continue;
        if((*ip2)->pt() / Gaudi::Units::GeV < m_prtPt) continue;
        if((*ip2)->charge() == 0) continue;
        if(p2IP / p2IPErr < m_prtIPS) continue;
        //---------- 3rd particle for seed
        for(ip3 = (ip2+1); ip3 != inputPartsVec.end(); ip3++) {
          if((*ip3)->proto() == NULL ) continue;
          if((*ip3)->proto()->track() == NULL) continue; 
          if((*ip3)->proto()->track()->type() != Track::Long) continue;
          if((*ip3)->proto()->track()->chi2PerDoF() > m_trkChi2) continue;
          // Impact parameter
          sc = partIPwrtPV(*ip3,recPV,p3IP,p3IPErr);
          // Cuts on 2nd particle parameters
          if((*ip3)->p() / Gaudi::Units::GeV < m_ptrMom) continue;
          if((*ip3)->pt() / Gaudi::Units::GeV < m_prtPt) continue;
          if((*ip3)->charge() == 0) continue;
          if(p3IP / p3IPErr < m_prtIPS) continue;
          // Fit secondary vertex
          StatusCode scSV = fitterSV->fit(sVtx,**ip1,**ip2,**ip3);
          if(scSV.isFailure()) continue;
          double delZPVSV = -1;
          if(scSV) delZPVSV = sVtx.position().z() / Gaudi::Units::mm - zPV;
          // Cuts on secondary vertex parameters
          Gaudi::LorentzVector sumMom123 = (*ip1)->momentum() + 
                                           (*ip2)->momentum() +
                                           (*ip3)->momentum();
          if(delZPVSV < m_dZSVPV) continue;
          if(sumMom123.Pt() / Gaudi::Units::GeV < m_sumMomSV) continue;
          double sumCh = (*ip1)->charge() + (*ip2)->charge() + 
                         (*ip3)->charge();
          if(fabs(sumCh) == 3) continue;
          if(sVtx.chi2PerDoF() > m_vtxChi2) continue;
          // Push back secondary vertices
          seedVrtcs.push_back(sVtx); 
        }
      }
    }
  }
  

  //---------- Seed uniqueness: filter SV's by min chi2
  if(seedVrtcs.size() >= m_nrSeeds) filterSV3Chi2(seedVrtcs,&uniqueVrtcs);
  return uniqueVrtcs;
}


//=============================================================================
// Calculate minimum particle IP wrt PV's
//=============================================================================
StatusCode bJetSeeds::partIPwrtPV(const Particle* prt, 
                                  const VertexBase* vBase, 
                                  double& ip, double& ipErr) 
{
  ip = -100.0;
  ipErr = 0.0;
  double ipC = 0;
  double ipChi2 = 0;
  StatusCode scIP = m_DOCA->distance(prt,vBase,ipC,ipChi2);
  if((scIP) && (ipChi2 != 0)) {
     ip = ipC;
     ipErr = ipC / sqrt(ipChi2);
  }
  return scIP;
}


//=============================================================================
// Filter 3-track SV by min SV chi2
//=============================================================================
struct vcsSV3CompChi2 {
  bool operator()(const Vertex &a, const Vertex &b) const {
    Vertex A = (Vertex) a;
    Vertex B = (Vertex) b;
    return(A.chi2PerDoF() < B.chi2PerDoF());
  }
} vcsSV3CompChi2_;
//----------
int bJetSeeds::filterSV3Chi2(std::vector<Vertex> sds,
                             std::vector<Vertex>* purgedSeeds)
{
  std::vector<Vertex> tmpSeeds;
  std::vector<Vertex>::iterator is;
  if(!sds.size()) return 0;
  else if(sds.size() == 1) {(*purgedSeeds).push_back(sds.at(0)); return 0;}
  else {
    // Sort seeds vector according to VCS
    std::sort(sds.begin(),sds.end(),vcsSV3CompChi2_);
    // Save best seed
    (*purgedSeeds).push_back(sds.at(0));
    Vertex bestSeed = sds.at(0);
    // Fill temp vector with seed independent of best seed
    for(is = sds.begin(); is != sds.end(); is++) {
      if((   (*is).outgoingParticles().at(0)->key() == 
          bestSeed.outgoingParticles().at(0)->key()) || 
         (   (*is).outgoingParticles().at(1)->key() == 
          bestSeed.outgoingParticles().at(1)->key()) || 
         (   (*is).outgoingParticles().at(2)->key() == 
          bestSeed.outgoingParticles().at(2)->key()) || 
         (   (*is).outgoingParticles().at(0)->key() == 
          bestSeed.outgoingParticles().at(1)->key()) || 
         (   (*is).outgoingParticles().at(1)->key() == 
          bestSeed.outgoingParticles().at(0)->key()) || 
         (   (*is).outgoingParticles().at(0)->key() == 
          bestSeed.outgoingParticles().at(2)->key()) || 
         (   (*is).outgoingParticles().at(2)->key() == 
          bestSeed.outgoingParticles().at(0)->key()) || 
         (   (*is).outgoingParticles().at(1)->key() == 
          bestSeed.outgoingParticles().at(2)->key()) || 
         (   (*is).outgoingParticles().at(2)->key() ==
          bestSeed.outgoingParticles().at(1)->key())) continue;
      else tmpSeeds.push_back((*is));
    }
    if(!tmpSeeds.size()) return 0;
    else return filterSV3Chi2(tmpSeeds,purgedSeeds);
  }
}
