// $Id: PVChecker.cpp,v 1.3 2010-01-18 08:43:10 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include "Event/MCParticle.h"
#include "Event/MCVertex.h"
#include "Event/Track.h"
#include "Event/MCHeader.h"

// local
#include "PVChecker.h"

using namespace LHCb ;
using namespace Gaudi::Units;

//-----------------------------------------------------------------------------
// Implementation file for class : PVChecker
//
// 2006-05-25 : Yuehong Xie
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PVChecker )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  PVChecker::PVChecker( const std::string& name,
                        ISvcLocator* pSvcLocator)
    : DaVinciTupleAlgorithm ( name , pSvcLocator )
{ }

//=============================================================================
// Destructor
//=============================================================================
PVChecker::~PVChecker() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PVChecker::execute() {

  debug() << "==> Execute" << endmsg;

  setFilterPassed(false);

  StatusCode sc = StatusCode::SUCCESS ;

  LHCb::RecVertices* PVs = get<LHCb::RecVertices>(LHCb::RecVertexLocation::Primary);
  long nPV = PVs->size();

  verbose() << "Found " << nPV << " Primary vertices" << endmsg;

  Tuples::Tuple ntuple = GaudiTupleAlg::nTuple( 300, "PVChecker");

  for( LHCb::RecVertices::const_iterator ipv = PVs->begin();
       ipv != PVs->end(); ipv++ ) {
    const LHCb::RecVertex* tmppv = *ipv;

    double x = tmppv->position().x();
    double y = tmppv->position().y();
    double z = tmppv->position().z();

    double xerr = sqrt(tmppv->covMatrix()(0,0));
    double yerr = sqrt(tmppv->covMatrix()(1,1));
    double zerr = sqrt(tmppv->covMatrix()(2,2));
    double nTrPV = 1.*tmppv->tracks().size();

    bool isPVtrue =false;
    double xmc = -9999.;
    double ymc = -9999.;
    double zmc = -9999.;
    double xpull = -9999.;
    double ypull = -9999.;
    double zpull = -9999.;

    int nTrMatched = 0;


    const LHCb::MCVertex* MCPV = PV2MCVertex(tmppv, nTrMatched);
    if (MCPV) {
      isPVtrue = true;
      xmc = MCPV->position().X();
      ymc = MCPV->position().Y();
      zmc = MCPV->position().Z();
      if(xerr!=0) xpull=(x-xmc)/xerr;
      if(yerr!=0) ypull=(y-ymc)/yerr;
      if(zerr!=0) zpull=(z-zmc)/zerr;
    }

    ntuple->column("x",x);
    ntuple->column("y",y);
    ntuple->column("z",z);
    ntuple->column("xerr",xerr);
    ntuple->column("yerr",yerr);
    ntuple->column("zerr",zerr);
    ntuple->column("nTrPV",nTrPV);

    ntuple->column("isPVtrue",isPVtrue);
    ntuple->column("xmc",xmc);
    ntuple->column("ymc",ymc);
    ntuple->column("zmc",zmc);
    ntuple->column("xpull",xpull);
    ntuple->column("ypull",ypull);
    ntuple->column("zpull",zpull);
    ntuple->column("nTrMatched",nTrMatched);

    ntuple->column("nPV", nPV);

    sc = ntuple->write();
    if( sc.isFailure() )
      return Error( "Cannot fill ntuple" );

  }

  return sc;
}

//=============================================================================
//  associate a reconstructed PV to a MCVertex
//=============================================================================

const LHCb::MCVertex* PVChecker::PV2MCVertex(const LHCb::RecVertex* pv, int & nTrMatched )
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
    if( same>max && diff < 5.0*mm ) {
      max=same;
      best=mcPV;
    }
  }

  nTrMatched = max;
  return best;

}

int PVChecker::countMatchedPVTrks(const RecVertex* pv,
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

    //    for( LHCb::MCParticle* mcPart = m_track2MCLink->first(track);
    //         NULL != mcPart;
    //         mcPart = m_track2MCLink->next(track) ) {
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
const LHCb::RecVertex* PVChecker::MCPV2PV(const LHCb::MCVertex* mcpv, int & nTrMatched)
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
    if( same>max && diff < 5.0*mm ) {
      max=same;
      best=tmppv;
    }
  }

  nTrMatched = max;
  return best;

}

