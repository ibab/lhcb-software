#define NtpTag_cxx

#include <TLorentzVector.h>
#include <TVector3.h>
#include <iostream>
#include <Riostream.h>

#include "globals.h"
#include "NtpTag.h"

extern double  m_IPPU_cut, m_thetaMin, m_distphi_cut;
extern bool IsControlChannel;

using namespace std;

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
bool NtpTag::execute(Long64_t ientry, Event& evt) {

  if (fChain == 0) return false;
  Long64_t nentries = fChain->GetEntriesFast();
  if(ientry>=nentries) return false;
  int i = LoadTree(ientry);
  if (i < 0) return false;
  fChain->GetEntry(ientry); 

  //build Event object//////////////////////////////////
  evt.setRunNr(Run);
  evt.setEventNr(eventNr);
  evt.setMultiplicity(N);
  evt.setStdOmega(Omega);
  evt.setStdTag(Tag);
  evt.setStdTagCat(TagCat);
  evt.seteventType(evType);

  int tr1,tr2,tr3;
  decode(trig, tr3,tr2,tr1);
  evt.setL0  ( tr1 ); 
  evt.setHlt1( tr2 ); 
  evt.setHlt2( tr3 ); 

  evt.setL0TisTos(L0TisTos); 
  evt.setHltTisTos(HltTisTos);
  evt.setBmass(Bmass)  ;
  evt.setBtau(tau)  ;
  evt.setBtauErr(tauErr);
  evt.setTauChi2(ctChi2);
  
  evt.setpileup(krec);

  evt.sethasOscillated(BSosc);
  evt.setbackgroundCat(bkgCat);
  evt.setProcessNr(kType);

  double BOMas= 5.279; if(abs(BOID)==531) BOMas = 5.367;
  TLorentzVector OppoB(0,0,0,0);
  if(enableMC) OppoB = build4V( BOP, BOP*sin(BOthe), BOphi, BOMas );

  Particle* partOB = new Particle(OppoB, BOID);
  if(enableMC) partOB->setEndVertexPosition(TVector3(BOx,BOy,BOz));
  if(enableMC) partOB->setHasOscillated(BOosc);
  evt.setOppositeB(partOB);

  //fill signal
  if(enableMC) evt.setTrueTag(TrueTag);
  evt.signalParts().clear();

  for (int ii=0; ii<M; ii++){

    int bid = abs(sID[ii]);
    TLorentzVector p = build4V( sP[ii], sPt[ii], sPhi[ii], sMass[ii] );

    double BMas= 5.279; if(abs(sMCID[ii])==531) BMas = 5.367;
    TLorentzVector mcp(0,0,0,0);
    if(enableMC) mcp = build4V( sMCP[ii], sMCPt[ii], sMCPhi[ii], BMas );

    Particle* signal = new Particle();
    signal->setMomentum( p );
    signal->setID( sID[ii] );
    signal->setfromB(-1);
    if(enableMC) signal->setMCMomentum( mcp );
    if(enableMC) signal->setMCID( sMCID[ii] );

    evt.addToSignalParts(signal);

    //Set Mu and Dstar
    if (bid==13) evt.setMu(signal);
    if (bid==413) evt.setDstar(signal);
    
    if(bid==531 || bid==511 || bid==521) {
      //Redefine TrueTag as the flavour of the reconstructed final state
      //as it is provided by signal selection in TES:
      if( IsControlChannel ) TrueTag = sID[ii]>0? 1 : -1 ;
      evt.setSignalB(signal);
      evt.setBSVertex(TVector3(sVx[ii],sVy[ii],sVz[ii]));
      evt.setTrueTag(TrueTag);
    }
  }

  evt.setRecVertex( TVector3(RVx,RVy,RVz) );
  if(enableMC) evt.setMCVertex ( TVector3(kx, ky, kz) );

  /////////////////////////////////////////////////////////////////
  //build Particle object
  Particles parts = evt.particles(); parts.clear();
 
  //- loop over N ---------------------------->
  for (int i=0; i<N; ++i) {

    // CUTS APPLIED TO ALL CANDIDATES:
    if(ipPU[i]    < m_IPPU_cut)    continue;//all taggers far from PU vertices
    if(distPhi[i] < m_distphi_cut) continue;
    if(acos(Pt[i]/P[i])<m_thetaMin)continue;
    //if(ip[i]      > m_IP_cut)      continue;
    //if(cloneDist[i]!=-1) continue; //no clones
    
    /*
    //cuts for PV>1
    if (krec>1) {
      if ((pow(ip[i]-ipPU[i],2)/pow(ipPU[i],2))<0.865) continue;
      if (ip[i]-nippu[i]>0.5) continue;
      if (ip[i]-ipmean[i]>0.2) continue;
    }
    */
       
    ////////////////////////////////
    Particle* apart = new Particle();
    ////////////////////////////////

    int ismu, muNSH, inHcal, iflag_e, iflag_m, iflag_k;
    decode( PIDfl[i], ismu, muNSH, inHcal, iflag_e, iflag_m, iflag_k );

    TLorentzVector p   = build4V( P[i], Pt[i], phi[i], ID[i] );
    
    TLorentzVector mcp(0,0,0,0);
    if(enableMC) mcp = build4V( MCP[i], MCPt[i], MCphi[i], MCID[i] );
 
    apart->setMomentum( p );
    apart->setID( ID[i] );
    apart->setPIDe( PIDe[i] );
    apart->setPIDk( PIDk[i] );
    apart->setPIDm( PIDm[i] );

    apart->setHasMuonFlag( iflag_m );
    apart->setInAccHcal( inHcal );
    apart->setEOverP( EOverP[i] );
    apart->setVeloCharge( veloch[i] );
  
    apart->setDOCA( DOCA[i] );
    apart->setIPSV( IPSV[i] );
 
    apart->setDistPhi( distPhi[i] );

    apart->setLikelihood( tsal[i] );
   
    apart->setMuonNShared(muNSH);
    apart->setPIDp( PIDp[i] );
    apart->setIPs( ip[i]/iperr[i] );
    apart->setIP( ip[i] );
    apart->setIPerr( iperr[i] );
    apart->setIPPU( ipPU[i] );

    //extrainfo for PV
    apart->setipmean( ipmean[i] );
    apart->setnippu( nippu[i] );
    apart->setnippuerr( nippuerr[i] );
    apart->settracks( tracks[i] );
    apart->settrackzfirst( trackzfirst[i] );
    apart->settrackp( trackp[i] );
    apart->setippubs( ippubs[i] );
    apart->setippuchi2bs( ippuchi2bs[i] );
    apart->setzpos( zpos[i] );
    apart->setzerrpos( zerrpos[i] );

    apart->setcloneDist( cloneDist[i] );
    apart->setLCS( lcs[i] );
    apart->setCharge( ch[i] );
    apart->setType( trtyp[i] );
    apart->setComesFromSVtx( vFlag[i] );

    //MC INFO
    if(enableMC) {
      apart->setMotherID( mothID[i] );
      apart->setAncestorID( ancID[i] );
      apart->setfromB( bFlag[i] );
      apart->setxFlag( xFlag[i] );
      apart->setMCMomentum(mcp);
      apart->setMCID( MCID[i] );
    }
    parts.push_back(apart);
  }

  evt.setParticles(parts);

  /////////////////////////////////////////////////////////////////
  //build Secondary Vertex object (ONLY THE SEED)

  Vertices  allSeeds;
  Particles parts_in_Seed; parts_in_Seed.clear();

  for ( int iv=0; iv<V; iv++ ) {
    int i_seed1=-1, i_seed2=-1; 
    for ( int i=0; i< (int)parts.size(); i++ ) {
      if(!vFlag[i]) continue;
      if(SecVtx_pt1[iv]==Pt[i]) i_seed1=i; //N-block index 
      if(SecVtx_pt2[iv]==Pt[i]) i_seed2=i;
    }
    if( i_seed1==-1 || i_seed2==-1 )  {
      debug()<<"could not find 2 seeds "<<endmsg;
      continue;//info missing in N-block
    }

    parts_in_Seed.push_back( parts.at(i_seed1) );
    parts_in_Seed.push_back( parts.at(i_seed2) );
    
    TVector3 SVpoint(SecVtx_x[iv]-RVx, SecVtx_y[iv]-RVy, SecVtx_z[iv]-RVz);

    Vertex* Seed = new Vertex();

    Seed->setPosition( SVpoint );
    Seed->setChi2PerDoF( SecVtx_chi2[iv] );
    Seed->setType( Vertex::Seed );
    Seed->setOutgoingParticles( parts_in_Seed );

    allSeeds.push_back( Seed );

    break; //TAKE JUST ONE SEED
  }

  evt.setSeeds( allSeeds );

  //Fill tag info direct from DaVinci run algorithm
  //build a bit flag
  int dectagDV=0, tagDV=0;
  if(property["checkDV"]==true) {
    for(int i=0; i<T; ++i) {
      if(TaggerDecision[i]==0) continue;
      int adec = TaggerDecision[i]+2;
      int aType= TaggerType[i];
      if     (aType==2) { tagDV += 10000; dectagDV += adec*10000; }
      else if(aType==3) { tagDV +=  1000; dectagDV += adec* 1000; }
      else if(aType==4) { tagDV +=   100; dectagDV += adec*  100; }
      else if(aType==5) { tagDV +=    10; dectagDV += adec*   10; }//sameS
      else if(aType==6) { tagDV +=    10; dectagDV += adec*   10; }//sameS
      else if(aType==10){ tagDV +=     1; dectagDV += adec*    1; }
    }
  } 
  evt.setTaggersFromDV(tagDV);
  evt.setTaggersDecisionFromDV(dectagDV);

  return true;
}

