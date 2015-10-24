// $Id: ProperTimeChecker.cpp,v 1.10 2010-06-03 07:23:20 jpalac Exp $
// Include files

// from Gaudi
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/Track.h"
#include "Event/MCHeader.h"

#include "Kernel/ILifetimeFitter.h"
#include "Kernel/IPVReFitter.h"

// local
#include "ProperTimeChecker.h"

using namespace LHCb ;
using namespace Gaudi::Units;

//-----------------------------------------------------------------------------
// Implementation file for class : ProperTimeChecker
//
// 2006-05-25 : Yuehong Xie
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ProperTimeChecker )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  ProperTimeChecker::ProperTimeChecker( const std::string& name,
                                        ISvcLocator* pSvcLocator)
    : DaVinciTupleAlgorithm ( name , pSvcLocator )
{
  declareProperty( "ParticlePath", m_particlePath );
  declareProperty( "pidToCheck", m_pidToCheck );
  declareProperty( "fillNtuplePVReFit", m_reFitPV = true);
  declareProperty( "fillNtuplePVSignalBRemoval", m_removeBFromPV = true);
  declareProperty( "fillNtuplePVMCSecondaryRemoval", m_removeMCSecondaryFromPV = true);
  declareProperty( "PVReFitterName", m_PVReFitterName = "LoKi::PVReFitter:PUBLIC",
                   "PV refitter algorithm name (':PUBLIC' at end of algo name makes sure a public instance is used)" );
    


}
//=============================================================================
// Destructor
//=============================================================================
ProperTimeChecker::~ProperTimeChecker() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ProperTimeChecker::initialize()
{
  StatusCode sc = DaVinciTupleAlgorithm::initialize();
  if (!sc) return sc;

  debug() << "==> Initialize" << endmsg;
  // For Particle -> MCParticle association
  m_pLinker = new Particle2MCLinker(this,
                                    Particle2MCMethod::Composite,
                                    std::vector<std::string>(1,m_particlePath));

  m_timeFitter =  tool<ILifetimeFitter> ("PropertimeFitter" , this);
  m_pvReFitter = tool<IPVReFitter>(m_PVReFitterName, this );
  m_cheatedPVReFitter = tool<IPVReFitter>("CheatedPVReFitter",this );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ProperTimeChecker::execute() {

  debug() << "==> Execute" << endmsg;

  setFilterPassed(false);

  StatusCode sc = StatusCode::SUCCESS ;

  // Retrieve the particles and vertices from local storage
  const Particle::Range parts = this->particles();

  LHCb::RecVertices* PVs = get<LHCb::RecVertices>(LHCb::RecVertexLocation::Primary);
  long nPV = PVs->size();

  verbose() << "Found " << parts.size() << " particles in local storage" << endmsg;

  Tuples::Tuple ntuple = GaudiTupleAlg::nTuple( 200, "ProperTimeChecker");

  for ( LHCb::Particle::Range::const_iterator it = parts.begin() ; it!=parts.end() ; ++it){

    debug() << " (ID= " <<  (*it)->particleID().pid()
            << ") has momentum " << (*it)->momentum()/GeV
            << " and mass " <<  (*it)->measuredMass()/GeV << " GeV" << endmsg;


    if ( abs((*it)->particleID().pid())!=m_pidToCheck ) continue ;
    const LHCb::Particle* Part = *it;
    const LHCb::MCParticle* MCP = m_pLinker->firstMCP( Part ); ;
    if (0==MCP) debug() << "no MCParticle found " <<endmsg;
    else debug() << "MCParticle is found " << endmsg;
    if (0==MCP) continue ;

    ntuple->column("partTrue",1.);
    ntuple->column("partMass",Part->measuredMass());
    ntuple->column("partMassErr",Part->measuredMassErr());
    ntuple->column("partChi2",Part->endVertex()->chi2());
    ntuple->column("partDoF",Part->endVertex()->nDoF());
    ntuple->column("partVx",Part->referencePoint().X());
    ntuple->column("partVy",Part->referencePoint().Y());
    ntuple->column("partVz",Part->referencePoint().Z());
    ntuple->column("partVxDiff",Part->referencePoint().X()-MCP->endVertices().front()->position().X());
    ntuple->column("partVyDiff",Part->referencePoint().Y()-MCP->endVertices().front()->position().Y());
    ntuple->column("partVzDiff",Part->referencePoint().Z()-MCP->endVertices().front()->position().Z());
    ntuple->column("partVxPull",(Part->referencePoint().X()-MCP->endVertices().front()->position().X())
                   /sqrt(Part->posCovMatrix()(0,0)));
    ntuple->column("partVyPull",(Part->referencePoint().Y()-MCP->endVertices().front()->position().Y())
                   /sqrt(Part->posCovMatrix()(1,1)));
    ntuple->column("partVzPull",(Part->referencePoint().Z()-MCP->endVertices().front()->position().Z())
                   /sqrt(Part->posCovMatrix()(2,2)));

    ntuple->column("partPx",Part->momentum().X());
    ntuple->column("partPy",Part->momentum().Y());
    ntuple->column("partPz",Part->momentum().Z());
    ntuple->column("partPxDiff",Part->momentum().X()-MCP->momentum().X());
    ntuple->column("partPyDiff",Part->momentum().Y()-MCP->momentum().Y());
    ntuple->column("partPzDiff",Part->momentum().Z()-MCP->momentum().Z());
    ntuple->column("partPxPull",(Part->momentum().X()-MCP->momentum().X())
                   /sqrt(Part->momCovMatrix()(0,0)));
    ntuple->column("partPyPull",(Part->momentum().Y()-MCP->momentum().Y())
                   /sqrt(Part->momCovMatrix()(1,1)));
    ntuple->column("partPzPull",(Part->momentum().Z()-MCP->momentum().Z())
                   /sqrt(Part->momCovMatrix()(2,2)));

    bool selPVOk = false;
    double selPVx = -9999.;
    double selPVy = -9999.;
    double selPVz = -9999.;
    double selPVxErr = -9999.;
    double selPVyErr = -9999.;
    double selPVzErr = -9999.;
    long selPVnTr = 0;
    long selPVnDF = 0;
    double selPVChi2 = -9999.;

    double time0 =-9999.;
    bool timeFitOk = false;
    double timeFitChi2 = -9999.;
    double time = -9999.;
    double timeErr =-9999.;

    //    const LHCb::RecVertex* bestPV = m_relatedPV->bestPV(Part);
    const LHCb::RecVertex* bestPV = closestPV(Part);
    if (bestPV) {
      selPVOk = true;
      selPVx = bestPV->position().x();
      selPVy = bestPV->position().y();
      selPVz = bestPV->position().z();
      selPVxErr = sqrt(bestPV->covMatrix()(0,0));
      selPVyErr = sqrt(bestPV->covMatrix()(1,1));
      selPVzErr = sqrt(bestPV->covMatrix()(2,2));
      selPVnTr= bestPV->tracks().size();
      selPVnDF = bestPV->nDoF();
      selPVChi2 = bestPV->chi2();

      Gaudi::XYZVector tmpL = Part->endVertex()->position() -
        bestPV->position();
      time0 = (1/(picosecond*c_light))*(Part->measuredMass())*
        tmpL.Dot(Part->momentum().Vect())/Part->momentum().Vect().mag2();

      StatusCode fitsc = m_timeFitter->fit(*bestPV, *Part, time, timeErr, timeFitChi2);
      if (fitsc.isSuccess()) timeFitOk = true;
      time/=picosecond;
      timeErr/=picosecond;
    }

    long selMCPVkey = -1;
    double selMCPVx = -9999.;
    double selMCPVy = -9999.;
    double selMCPVz = -9999.;

    if (bestPV) {
      const LHCb::MCVertex* selMCPV = PV2MCVertex(bestPV);
      if (selMCPV) {
        selMCPVkey = selMCPV->key();
        selMCPVx = selMCPV->position().X();
        selMCPVy = selMCPV->position().Y();
        selMCPVz = selMCPV->position().Z();
      }
    }

    long sigMCPVkey = -1;
    double sigMCPVx =-9999.;
    double sigMCPVy =-9999.;
    double sigMCPVz =-9999.;
    bool sigMCPVrec = false;
    double mctime =-9999.;

    if(MCP && MCP->originVertex()) {
      sigMCPVkey = MCP->originVertex()->key();
      sigMCPVx = MCP->originVertex()->position().X();
      sigMCPVy = MCP->originVertex()->position().Y();
      sigMCPVz = MCP->originVertex()->position().Z();
      Gaudi::XYZVector tmp = MCP->endVertices().front()->position() -
        MCP->originVertex()->position();
      double  mcL = tmp.R();
      mctime = mcL*MCP->momentum().mass()/
        MCP->momentum().Vect().R()/(picosecond*c_light);

      const LHCb::RecVertex* recpv =MCPV2PV(MCP->originVertex());
      if(recpv) sigMCPVrec = true;
    }

    ntuple->column("selPVOk", selPVOk);
    ntuple->column("selPVx", selPVx);
    ntuple->column("selPVy", selPVy);
    ntuple->column("selPVz", selPVz);
    ntuple->column("selPVxErr", selPVxErr);
    ntuple->column("selPVyErr", selPVyErr);
    ntuple->column("selPVzErr", selPVzErr);
    ntuple->column("selPVnTr",  selPVnTr);
    ntuple->column("selPVnDF",  selPVnDF);
    ntuple->column("selPVChi2",  selPVChi2);

    ntuple->column("selMCPVkey", selMCPVkey);
    ntuple->column("selMCPVx", selMCPVx);
    ntuple->column("selMCPVy", selMCPVy);
    ntuple->column("selMCPVz", selMCPVz);

    ntuple->column("sigMCPVkey", sigMCPVkey);
    ntuple->column("sigMCPVx", sigMCPVx);
    ntuple->column("sigMCPVy", sigMCPVy);
    ntuple->column("sigMCPVz", sigMCPVz);
    ntuple->column("sigMCPVrec", sigMCPVrec);

    ntuple->column("time0", time0);
    ntuple->column("timeFitOk", timeFitOk);
    ntuple->column("timeFitChi2", timeFitChi2);

    ntuple->column("time", time);
    ntuple->column("timeErr", timeErr);
    ntuple->column("mctime", mctime);

    double cosPF = -9999.;
    if(bestPV) {
      Gaudi::XYZVector dist = Part->referencePoint()-bestPV->position();
      const Gaudi::XYZVector vmom = Part->momentum().Vect();
      cosPF = vmom.Dot(dist)/vmom.R()/dist.R();
    }
    ntuple->column("cosPF", cosPF);

    ntuple->column("nPV", nPV);

    if(m_reFitPV) {

      bool rfPVOk = false;
      double rfPVx = -9999.;
      double rfPVy = -9999.;
      double rfPVz = -9999.;
      double rfPVxErr = -9999.;
      double rfPVyErr = -9999.;
      double rfPVzErr = -9999.;
      long rfPVnTr = 0;
      long rfPVnDF = 0;
      double rfPVChi2 = -9999.;

      bool rfpv_timeFitOk = false;
      double rfpv_time = -9999.;
      double rfpv_timeErr = -9999.;
      double rfpv_timeFitChi2 = -9999.;
      double rfpv_cosPF = -9999.;

      if(bestPV) {

        LHCb::Particle newPart(*Part);
        LHCb::RecVertex newPV(*bestPV);
        StatusCode rfsc = m_pvReFitter->reFit(&newPV);

        if(rfsc.isSuccess()) {
          rfPVOk = true;
          rfPVx = newPV.position().x();
          rfPVy = newPV.position().y();
          rfPVz = newPV.position().z();
          rfPVxErr = sqrt(newPV.covMatrix()(0,0));
          rfPVyErr = sqrt(newPV.covMatrix()(1,1));
          rfPVzErr = sqrt(newPV.covMatrix()(2,2));
          rfPVnTr= newPV.tracks().size();
          rfPVnDF = newPV.nDoF();
          rfPVChi2 = newPV.chi2();


          StatusCode fitsc = m_timeFitter->fit(newPV, newPart, rfpv_time, rfpv_timeErr, rfpv_timeFitChi2);
          if (fitsc.isSuccess()) rfpv_timeFitOk = true;
          rfpv_time/=picosecond;
          rfpv_timeErr/=picosecond;

          Gaudi::XYZVector dist = newPart.referencePoint()-newPV.position();
          const Gaudi::XYZVector vmom = newPart.momentum().Vect();
          rfpv_cosPF = vmom.Dot(dist)/vmom.R()/dist.R();

        }
      }

      ntuple->column("rfPVOk", rfPVOk);
      ntuple->column("rfPVx", rfPVx);
      ntuple->column("rfPVy", rfPVy);
      ntuple->column("rfPVz", rfPVz);
      ntuple->column("rfPVxErr", rfPVxErr);
      ntuple->column("rfPVyErr", rfPVyErr);
      ntuple->column("rfPVzErr", rfPVzErr);
      ntuple->column("rfPVnTr",  rfPVnTr);
      ntuple->column("rfPVnDF",  rfPVnDF);
      ntuple->column("rfPVChi2",  rfPVChi2);

      ntuple->column("rfpv_timeFitOk", rfpv_timeFitOk);
      ntuple->column("rfpv_time", rfpv_time);
      ntuple->column("rfpv_timeErr", rfpv_timeErr);
      ntuple->column("rfpv_timeFitChi2", rfpv_timeFitChi2);
      ntuple->column("rfpv_cosPF", rfpv_cosPF);

    }

    if(m_removeBFromPV) {
      bool rfPVOk = false;
      double rfPVx = -9999.;
      double rfPVy = -9999.;
      double rfPVz = -9999.;
      double rfPVxErr = -9999.;
      double rfPVyErr = -9999.;
      double rfPVzErr = -9999.;
      long rfPVnTr = 0;
      long rfPVnDF = 0;
      double rfPVChi2 = -9999.;

      bool rfpv_timeFitOk = false;
      double rfpv_time = -9999.;
      double rfpv_timeErr = -9999.;
      double rfpv_timeFitChi2 = -9999.;
      double rfpv_cosPF = -9999.;

      if(bestPV) {

        LHCb::Particle newPart(*Part);
        LHCb::RecVertex newPV(*bestPV);
        StatusCode rfsc = m_pvReFitter->remove(&newPart, &newPV);

        if(rfsc.isSuccess()) {
          rfPVOk = true;
          rfPVx = newPV.position().x();
          rfPVy = newPV.position().y();
          rfPVz = newPV.position().z();
          rfPVxErr = sqrt(newPV.covMatrix()(0,0));
          rfPVyErr = sqrt(newPV.covMatrix()(1,1));
          rfPVzErr = sqrt(newPV.covMatrix()(2,2));
          rfPVnTr= newPV.tracks().size();
          rfPVnDF = newPV.nDoF();
          rfPVChi2 = newPV.chi2();

          StatusCode fitsc = m_timeFitter->fit(newPV, newPart, rfpv_time, rfpv_timeErr, rfpv_timeFitChi2);
          if (fitsc.isSuccess()) rfpv_timeFitOk = true;
          rfpv_time/=picosecond;
          rfpv_timeErr/=picosecond;

          Gaudi::XYZVector dist = newPart.referencePoint()-newPV.position();
          const Gaudi::XYZVector vmom = newPart.momentum().Vect();
          rfpv_cosPF = vmom.Dot(dist)/vmom.R()/dist.R();

        }
      }

      ntuple->column("rmbPVOk", rfPVOk);
      ntuple->column("rmbPVx", rfPVx);
      ntuple->column("rmbPVy", rfPVy);
      ntuple->column("rmbPVz", rfPVz);
      ntuple->column("rmbPVxErr", rfPVxErr);
      ntuple->column("rmbPVyErr", rfPVyErr);
      ntuple->column("rmbPVzErr", rfPVzErr);
      ntuple->column("rmbPVnTr",  rfPVnTr);
      ntuple->column("rmbPVnDF",  rfPVnDF);
      ntuple->column("rmbPVChi2",  rfPVChi2);

      ntuple->column("rmbpv_timeFitOk", rfpv_timeFitOk);
      ntuple->column("rmbpv_time", rfpv_time);
      ntuple->column("rmbpv_timeErr", rfpv_timeErr);
      ntuple->column("rmbpv_timeFitChi2", rfpv_timeFitChi2);
      ntuple->column("rmbpv_cosPF", rfpv_cosPF);

    }

    if(m_removeMCSecondaryFromPV) {

      bool rfPVOk = false;
      double rfPVx = -9999.;
      double rfPVy = -9999.;
      double rfPVz = -9999.;
      double rfPVxErr = -9999.;
      double rfPVyErr = -9999.;
      double rfPVzErr = -9999.;
      long rfPVnTr = 0;
      long rfPVnDF = 0;
      double rfPVChi2 = -9999.;

      bool rfpv_timeFitOk = false;
      double rfpv_time = -9999.;
      double rfpv_timeErr = -9999.;
      double rfpv_timeFitChi2 = -9999.;
      double rfpv_cosPF = -9999.;

      if(bestPV) {

        LHCb::Particle newPart(*Part);
        LHCb::RecVertex newPV(*bestPV);
        StatusCode rfsc = m_cheatedPVReFitter->reFit(&newPV);

        if(rfsc.isSuccess()) {
          rfPVOk = true;
          rfPVx = newPV.position().x();
          rfPVy = newPV.position().y();
          rfPVz = newPV.position().z();
          rfPVxErr = sqrt(newPV.covMatrix()(0,0));
          rfPVyErr = sqrt(newPV.covMatrix()(1,1));
          rfPVzErr = sqrt(newPV.covMatrix()(2,2));
          rfPVnTr= newPV.tracks().size();
          rfPVnDF = newPV.nDoF();
          rfPVChi2 = newPV.chi2();

          StatusCode fitsc = m_timeFitter->fit(newPV, newPart, rfpv_time, rfpv_timeErr, rfpv_timeFitChi2);
          if (fitsc.isSuccess()) rfpv_timeFitOk = true;
          rfpv_time/=picosecond;
          rfpv_timeErr/=picosecond;

          Gaudi::XYZVector dist = newPart.referencePoint()-newPV.position();
          const Gaudi::XYZVector vmom = newPart.momentum().Vect();
          rfpv_cosPF = vmom.Dot(dist)/vmom.R()/dist.R();

        }
      }

      ntuple->column("rmsPVOk", rfPVOk);
      ntuple->column("rmsPVx", rfPVx);
      ntuple->column("rmsPVy", rfPVy);
      ntuple->column("rmsPVz", rfPVz);
      ntuple->column("rmsPVxErr", rfPVxErr);
      ntuple->column("rmsPVyErr", rfPVyErr);
      ntuple->column("rmsPVzErr", rfPVzErr);
      ntuple->column("rmsPVnTr",  rfPVnTr);
      ntuple->column("rmsPVnDF",  rfPVnDF);
      ntuple->column("rmsPVChi2",  rfPVChi2);

      ntuple->column("rmspv_timeFitOk", rfpv_timeFitOk);
      ntuple->column("rmspv_time", rfpv_time);
      ntuple->column("rmspv_timeErr", rfpv_timeErr);
      ntuple->column("rmspv_timeFitChi2", rfpv_timeFitChi2);
      ntuple->column("rmspv_cosPF", rfpv_cosPF);

    }


    sc = ntuple->write();
    if( sc.isFailure() )
      return Error( "Cannot fill ntuple" );


  }

  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ProperTimeChecker::finalize() {

  debug() << "==> Finalize" << endmsg;

  delete m_pLinker;

  return DaVinciTupleAlgorithm::finalize();
}

//=============================================================================
//  associate a reconstructed PV to a MCVertex
//=============================================================================

const LHCb::MCVertex* ProperTimeChecker::PV2MCVertex(const LHCb::RecVertex* pv )
{
  int max=0;

  const LHCb::MCVertex* best=0;
  if(!pv) return best;

  LHCb::MCHeader* mch = get<LHCb::MCHeader>(LHCb::MCHeaderLocation::Default);
  for ( SmartRefVector< LHCb::MCVertex >::const_iterator imcpv = mch->primaryVertices().begin() ;
        imcpv != mch->primaryVertices().end() ; ++imcpv ){
    const LHCb::MCVertex* mcPV = *imcpv;
    if( mcPV->mother() != NULL ) continue;
    if( mcPV->position().z() > 1000.0 ) continue;
    if(mcPV->type()!=LHCb::MCVertex::ppCollision) continue;
    int same=countMatchedPVTrks(pv,mcPV);
    double diff = fabs(pv->position().z() - mcPV->position().z());
    if( same>max && diff < 2.0*mm ) {
      max=same;
      best=mcPV;
    }
  }

  return best;

}

int ProperTimeChecker::countMatchedPVTrks(const RecVertex* pv,
                                          const LHCb::MCVertex* mcPV)
{
  int count=0;
  if(!mcPV) return count;
  if(mcPV->type()!=LHCb::MCVertex::ppCollision) return count;
  if(!pv) return count;
  //  if(!(pv->isPrimary()))return count;

  if( mcPV->mother() != NULL ) return count;

  if( mcPV->position().z() > 1000.0 ) return count;

  SmartRefVector<LHCb::Track> pvtracks = pv->tracks();
  for( SmartRefVector<LHCb::Track>::iterator trIt = pvtracks.begin();
       pvtracks.end() != trIt; trIt++) {
    LHCb::Track* track = *trIt;
    if(!track) continue;

    LinkedTo<LHCb::MCParticle,LHCb::Track> directLink( evtSvc(), msgSvc(), TrackLocation::Default );

    for( LHCb::MCParticle* mcPart = directLink.first(track);
         NULL != mcPart;
         mcPart = directLink.next() ) {
      const LHCb::MCVertex* originv2=mcPart->originVertex();
      if(!originv2) continue;
      Gaudi::XYZVector tmpL = originv2->position() - mcPV->position();
      double disv = tmpL.R();
      if( fabs(disv)<0.000000001*mm ) {count++;break;}
    }
  }

  return count;

}


//=============================================================================
//  associate a MCVertex to a rec'ed PV
//=============================================================================
const LHCb::RecVertex* ProperTimeChecker::MCPV2PV(const LHCb::MCVertex* mcpv)
{

  LHCb::RecVertices* PVs = get<LHCb::RecVertices>(LHCb::RecVertexLocation::Primary);

  const LHCb::RecVertex* best=0;
  if(!mcpv) return best;

  int max=0;

  for( LHCb::RecVertices::const_iterator ipv = PVs->begin();
       ipv != PVs->end(); ipv++ ) {
    const LHCb::RecVertex* tmppv = *ipv;
    int same=countMatchedPVTrks(tmppv,mcpv);
    double diff = fabs(tmppv->position().z() - mcpv->position().z());
    if( same>max && diff < 2.0*mm ) {
      max=same;
      best=tmppv;
    }
  }

  return best;

}

//=============================================================================
// find the PV with smallest IP significance
//=============================================================================
const LHCb::RecVertex* ProperTimeChecker::closestPV(const LHCb::Particle* part)
{
  const LHCb::RecVertex* bestPV=0;

  double smallest = 999999.;
  LHCb::RecVertices* PVs = get<LHCb::RecVertices>(LHCb::RecVertexLocation::Primary);
  for( LHCb::RecVertices::const_iterator ipv = PVs->begin();
       ipv != PVs->end(); ipv++ ) {
    const LHCb::RecVertex* tmppv = *ipv;
    double tmpip=9999.;
    double tmpipchi2=1000.;
    StatusCode sc = distanceCalculator()->distance(part,tmppv,tmpip,tmpipchi2);
    if(sc.isSuccess()) {
      if(tmpipchi2<smallest) {
        bestPV = tmppv;
        smallest = tmpipchi2;
      }
    }
  }

  return bestPV;
}

