// Include files 
#include "BTaggingTool.h"

//--------------------------------------------------------------------------
// Implementation file for class : BTaggingTool
//
// Author: Marco Musy
//--------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<BTaggingTool>          s_factory ;
const        IToolFactory& BTaggingToolFactory = s_factory ; 

//==========================================================================
BTaggingTool::BTaggingTool( const std::string& type,
                            const std::string& name,
                            const IInterface* parent ) :
  GaudiTool ( type, name, parent ), m_eventSvc(0), m_Geom(0) {

  declareInterface<IBTaggingTool>(this);

  declareProperty( "IPPU_cut",     m_IPPU_cut = 3.0 );
  declareProperty( "thetaMin_cut", m_thetaMin = 0.012 );

  declareProperty( "SecondaryVertexName",m_SecondaryVertexToolName="TDR" );
  declareProperty( "CombinationTechnique", m_CombinationTechnique ="NNet");
  declareProperty( "ProbMin", m_ProbMin   = 0.52);
  declareProperty( "VchOmega", m_VchOmega = 0.40);

  declareProperty( "RequireTrigger", m_RequireTrigger    = false );
  declareProperty( "RequireL1Tampering", m_RequireL1Tamp = false );
  declareProperty( "RequireL0", m_RequireL0 = false );
  declareProperty( "RequireL1", m_RequireL1 = false );
  declareProperty( "RequireHLT",m_RequireHLT= false );
  declareProperty( "EnableMuonTagger",m_EnableMuonTagger        = true );
  declareProperty( "EnableElectronTagger",m_EnableElectronTagger= true );
  declareProperty( "EnableKaonOSTagger",m_EnableKaonOSTagger    = true );
  declareProperty( "EnableKaonSSTagger",m_EnableKaonSSTagger    = true );
  declareProperty( "EnablePionTagger",m_EnablePionTagger        = true );
  declareProperty( "EnableVertexChargeTagger",
                   m_EnableVertexChargeTagger= true );
  m_nnet = 0;
  m_svtool = 0;
  m_taggerMu=m_taggerEle=m_taggerKaon=0;
  m_taggerKaonS=m_taggerPionS=m_taggerVtx=0 ;
}
BTaggingTool::~BTaggingTool() {}; 

//==========================================================================
StatusCode BTaggingTool::initialize() {

  StatusCode sc = service("EventDataSvc", m_eventSvc, true);
  if( sc.isFailure() ) {
    fatal() << " Unable to locate Event Data Service" << endreq;
    return sc;
  }
  m_physd = tool<IPhysDesktop> ("PhysDesktop", this);
  if(! m_physd) {
    fatal() << "Unable to retrieve PhysDesktop"<< endreq;
    return StatusCode::FAILURE;
  }
  m_Geom = tool<IGeomDispCalculator> ("GeomDispCalculator", this);
  if ( ! m_Geom ) {   
    fatal() << "GeomDispCalculator could not be found" << endreq;
    return StatusCode::FAILURE;
  }
  m_svtool = tool<ISecondaryVertexTool> (m_SecondaryVertexToolName, this);
  if(! m_svtool) {
    warning()<< "No Vertex Charge tag will be used! " 
             << m_SecondaryVertexToolName <<endreq;
  }
  m_nnet = tool<INNetTool> ("NNetTool", this);
  if(! m_nnet) {
    fatal() << "Unable to retrieve NNetTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_taggerMu = tool<ITagger> ("TaggerMuonTool", this);
  if(! m_taggerMu) {
    fatal() << "Unable to retrieve TaggerMuonTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_taggerEle = tool<ITagger> ("TaggerElectronTool", this);
  if(! m_taggerEle) {
    fatal() << "Unable to retrieve TaggerElectronTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_taggerKaon = tool<ITagger> ("TaggerKaonOppositeTool", this);
  if(! m_taggerKaon) {
    fatal() << "Unable to retrieve TaggerKaonOppositeTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_taggerKaonS = tool<ITagger> ("TaggerKaonSameTool", this);
  if(! m_taggerKaonS) {
    fatal() << "Unable to retrieve TaggerKaonSameTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_taggerPionS = tool<ITagger> ("TaggerPionSameTool", this);
  if(! m_taggerPionS) {
    fatal() << "Unable to retrieve TaggerPionSameTool"<< endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//==========================================================================
FlavourTag* BTaggingTool::tag( const Particle* AXB0 ) {
  ParticleVector p(0);
  return tag( AXB0, 0, p );
}
//==========================================================================
FlavourTag* BTaggingTool::tag( const Particle* AXB0, const Vertex* RecVert ) {
  ParticleVector p(0);
  return tag( AXB0, RecVert, p );
}
//==========================================================================
FlavourTag* BTaggingTool::tag( const Particle* AXB0, 
                               const Vertex* RecVert,
                               ParticleVector& vtags ) {
   
  FlavourTag* theTag = new FlavourTag;
  theTag->setDecision( FlavourTag::none );

  // Retrieve informations about event
  EventHeader* evt=0;
  if( exist<EventHeader> (EventHeaderLocation::Default)) {
    evt = get<EventHeader> (EventHeaderLocation::Default);
  } else {
    err() << "Unable to Retrieve Event" << endreq;
    return theTag;
  }
  // Retrieve trigger/tampering info
  int trigger=-1;
  double L0tamp=0;
  double L1tamp=100;
  debug()<<"Retrieve TrgDecision from "
         <<TrgDecisionLocation::Default<<endreq;
  TrgDecision* trg = 0;
  HltScore* hlt = 0 ;
  if (m_RequireTrigger || m_RequireL1Tamp) {
    if ( !exist<TrgDecision>(TrgDecisionLocation::Default) ){
      warning() << "No TrgDecision" << endmsg ;
    } else {
      trg = get<TrgDecision> (TrgDecisionLocation::Default);
      if ( !exist<HltScore>(HltScoreLocation::Default) ){
        debug() << "No HLT score" << endreq ;
      } else hlt = get<HltScore>(HltScoreLocation::Default) ;
    }
  } 
  if(trg) {
    // Select events on trigger
    if( m_RequireL0 ) if( !trg->L0() ) return theTag; 
    if( m_RequireL1 ) if( !trg->L1() ) return theTag; 
    if( m_RequireHLT) {
      if ( ! hlt ) return theTag;
      if ( !(hlt->decision()) ) return theTag; 
    }
    if ( 0!=hlt ) trigger = 100* hlt->decision() + 10* trg->L1() + trg->L0();
    else trigger = 10* trg->L1() + trg->L0();

    // Retrieve Tampering info
    if( m_RequireL1Tamp ) {
      if(exist<TamperingResults>(TamperingResultsLocation::Default)) {
        TamperingResults* tampres = 
          get<TamperingResults>(TamperingResultsLocation::Default);
        L0tamp= tampres->L0TOB()*100 + tampres->L0TOS()*10 + tampres->L0TIS();
        L1tamp= tampres->L1TOB()*100 + tampres->L1TOS()*10 + tampres->L1TIS();
        debug() << "Tampering (TOB,TOS,TIS)  L0: "  << L0tamp 
                << "   L1: " << L1tamp << "  Trig: "<< trigger <<endreq;
      } else warning() << "TamperingResults not found" << endreq;
    }
  }

  //----------------------------
  // Counter of events processed
  debug() << ">>>>>  Tagging Run Nr " << evt->runNum()
          << " Event " << evt->evtNum() << "  <<<<<" << endreq;

  //build desktop
  if( !(m_physd->getEventInput()) ) return theTag;
  ParticleVector parts = m_physd->particles();
  VertexVector   verts = m_physd->primaryVertices();
  if( !(m_physd->saveDesktop()) )   return theTag;
  debug() << "  Nr Vertices: "  << verts.size() 
          << "  Nr Particles: " << parts.size() <<endreq;

  //AXB0 is the signal B from selection
  bool isBd = false;
  bool isBs = false;
  bool isBu = false;
  if( AXB0->particleID().hasDown() )    isBd = true;
  if( AXB0->particleID().hasStrange() ) isBs = true;
  if( AXB0->particleID().hasUp() )      isBu = true;

  //----------------------------
  if( RecVert == 0 ) {
    //choose as primary vtx the one with smallest IP wrt B signal
    //this is a guess for the actual PV chosen by the selection.
    //in the future there will be a more solid association in DaVinci
    Hep3Vector ipVec;
    HepSymMatrix errMatrix;
    double kdmin = 1000000;
    Vertices::const_iterator iv;
    for(iv=verts.begin(); iv!=verts.end(); iv++){
      if( (*iv)->type() == Vertex::Primary ){
        double ip, iperr;
        calcIP(AXB0, *iv, ip, iperr);
        debug() <<"Vertex IP="<<ip/iperr<<endreq;
        if(iperr) if( fabs(ip/iperr) < kdmin ) {
          kdmin = fabs(ip/iperr);
          RecVert = (*iv);
        }     
      }
    }
    if( !RecVert ) {
      err() <<"No Reconstructed Vertex!! Skip." <<endreq;
      return theTag;
    }    
  }

  //build a vector of pileup vertices --------------------------
  VertexVector PileUpVtx(0); //contains all the other primary vtx's
  for(Vertices::const_iterator iv=verts.begin(); iv!=verts.end(); iv++){
    if( (*iv)->type() != Vertex::Primary ) continue;
    if( (*iv) == RecVert ) continue;
    PileUpVtx.push_back(*iv);
    debug() <<"Pileup Vtx z=" << (*iv)->position().z()/mm <<endreq;
  }

  //loop over Particles, preselect taggers /////////////////////
  ParticleVector::iterator ip;
  std::vector<const Particle*> axdaugh = FindDaughters(AXB0); //vector of B daughters
  axdaugh.push_back(AXB0);
  if(vtags.size()==0) {
    for ( ip = parts.begin(); ip != parts.end(); ip++ ){

      if( (*ip)->p()/GeV < 2.0 ) continue;               //preselection
      if( (*ip)->momentum().theta()<m_thetaMin) continue;//preselection
      if( (*ip)->charge() == 0 ) continue;               //preselection 
      if( isinTree( *ip, axdaugh ) )  continue ;         //exclude signal
      if( trackType(*ip) > 3 ) continue;                 //preselection

      //eliminate from vtags all parts coming from a pileup vtx
      //calculate the min IP wrt all pileup vtxs
      double ippu, ippuerr;
      calcIP( *ip, PileUpVtx, ippu, ippuerr );
      if(ippuerr) if( ippu/ippuerr<m_IPPU_cut ) continue;//preselection

      ///////////////////////////////////////
      vtags.push_back(*ip);               // store tagger candidate
      /////////////////////////////////////
    }
  } else {
    for ( ip = parts.begin(); ip != parts.end(); ip++ ) {
      if( ! isinTree(*ip, axdaugh) ) vtags.push_back(*ip);
    }
  }

  ///------------------------------------------------------- Choose Taggers
  //select muons (elecs, kaons) taggers in vtags
  Particle *imuon, *iele, *ikaon, *ipionS, *ikaonS;
  imuon=iele=ikaon=ipionS=ikaonS=0;
  double qk = 0;

  ///-- VertexCharge tagger
  //look for a secondary Vtx due to opposite B
  std::vector<Vertex> vvec(0);
  double Vch = 0;  //contains the vertex charge
  Vertex Vfit(0);  //secondary vertex found
  std::vector<const Particle*>  Pfit(0); //particles in it
  if(m_EnableVertexChargeTagger) if(m_svtool) {
    vvec = m_svtool->buildVertex(*RecVert, vtags);
    if(vvec.size()) {
      Vfit = vvec.at(0); //take first
      Pfit = toStdVector(Vfit.products());
      for(std::vector<const Particle*>::const_iterator ip=Pfit.begin(); ip!=Pfit.end(); ip++) Vch += (*ip)->charge();
      debug() << " Vertex charge: " << Vch << endreq;
    }
  }

  ///-- Single particle taggers
  if(m_EnableMuonTagger) {
    ParticleVector vmuons = m_taggerMu->taggers(AXB0, RecVert, vtags);
    if(vmuons.size()) imuon = vmuons.at(0); //take first
  }
  if(m_EnableElectronTagger) {
    ParticleVector vele = m_taggerEle->taggers(AXB0, RecVert, vtags);
    if(vele.size()) iele = vele.at(0);
  }
  if(m_EnableKaonOSTagger) {
    ParticleVector vkaon = m_taggerKaon->taggers(AXB0, RecVert, vtags);
    double ptmaxk = -99.0;
    for(ip=vkaon.begin(); ip!=vkaon.end(); ip++) {
      qk += (*ip)->charge(); //sum of taggers charge
      if( (*ip)->pt()/GeV > ptmaxk ) { 
        ikaon = (*ip);
        ptmaxk = (*ip)->pt()/GeV;
      }
    }
  }
  if(m_EnableKaonSSTagger) if( isBs ) { 
    ParticleVector vkaonS = m_taggerKaonS->taggers(AXB0, RecVert, vtags);
    if(vkaonS.size()) ikaonS = vkaonS.at(0);
  }
  if(m_EnablePionTagger) if( isBd || isBu ) {
    ParticleVector vpionS = m_taggerPionS->taggers(AXB0, RecVert, vtags);
    if(vpionS.size()) ipionS = vpionS.at(0);
  }
  //-----------------------------
  //end of tagger cands selection

  //--- itag is now the individual B-flavour guess of each separate tagger:
  std::vector<int> itag;
  for( int j=0; j!=6; j++ ) itag.push_back(0);
  if( imuon ) itag.at(1)= imuon->charge()>0 ? -1: 1;
  if( iele  ) itag.at(2)= iele ->charge()>0 ? -1: 1;
  if( ikaon ) itag.at(3)= ikaon->charge()>0 ? -1: 1;
  if( ikaonS) itag.at(4)= ikaonS->charge()>0?  1:-1; //SS kaon type
  if( ipionS) itag.at(4)= ipionS->charge()>0?  1:-1; //SS pion type
  if( isBu )  itag.at(4)= -itag.at(4);               //swap same-side if Bu
  if( Vch )   itag.at(5)= Vch>0?  -1: 1;             //vertex charge tagger

  //------------------------------------------------------------------------
  //Now combine the individual tagger decisions into 
  //a final B flavour tagging decision. Such decision 
  //can be made and categorised in two ways: the "TDR" 
  //approach is based on the particle type, while the 
  //"NNet" approach is based on the wrong tag fraction
  //-----------------------------------------  Combine taggers: TDR approach
  int catt=0;
  double tagdecision=0, pnsum=0.50;
  std::vector<double> pn;
  for( int j=0; j!=6; j++ ) pn.push_back(0.50);

  if(m_CombinationTechnique == "TDR") {
    //in case ele and muon both exist kill lower Pt one
    if(imuon) if(iele) {
      if(imuon->pt()/GeV > iele->pt()/GeV) iele= 0; else imuon= 0;
    } 

    //-----------------
    int ic=0;
    if( imuon ) ic+=1000;
    if( iele  ) ic+= 100;
    if( ikaon ) ic+=  10;
    if( ikaonS || ipionS ) ic+= 1;

    //if pion same side fill only category nr7
    //(pion SS is only considered when no other tagger is present)
    if((isBd || isBu ) && itag.at(4) && ic>1 ) ic--;

    if(     ic==1000) {    // only muon
      catt=1; tagdecision = itag.at(1);
    }
    else if(ic== 100) {    // only electron 
      catt=2; tagdecision = itag.at(2);
    }
    else if(ic==  10) {    // only kaon
      catt=3; if(qk) tagdecision = qk>0 ? -1 : 1;
    }
    else if(ic==1010) {   // mu-k (muon and kaon are both present)
      catt=4; tagdecision = itag.at(1)+itag.at(3);
    }
    else if(ic== 110) {   // e-k 
      catt=5; tagdecision = itag.at(2)+itag.at(3);
    }
    else if(ic==   0 && Vch!=0 ) { // vertex charge
      catt=6; tagdecision = itag.at(5);
    }
    else if(ic==   1) {   // Same Side pion or kaon only
      catt=7; tagdecision = itag.at(4);
    }
    else if(ic==1001) {   // mu-kS 
      catt=8; tagdecision = itag.at(1)+itag.at(4);
    }
    else if(ic== 101) {   // e-kS 
      catt=9; tagdecision = itag.at(2)+itag.at(4);
    }
    else if(ic==  11) {   // k-kS 
      catt=10; tagdecision= itag.at(3)+itag.at(4);
    }
    else if(ic==1011) {   // mu-k-kS 
      catt=11; tagdecision= itag.at(1)+itag.at(3)+itag.at(4);
    }
    else if(ic== 111) {   // e-k-kS 
      catt=12; tagdecision= itag.at(2)+itag.at(3)+itag.at(4);
    }
    if(tagdecision) tagdecision = tagdecision>0 ? 1 : -1;
  }

  //--------------------------------------- Combine taggers: NNet approach 
  else if(m_CombinationTechnique == "NNet") {

    HepLorentzVector ptotB = AXB0->momentum();
    double B0mass = ptotB.m()/GeV;
    double B0the  = ptotB.theta();
    double B0phi  = ptotB.phi();
    double B0p    = ptotB.vect().mag()/GeV;
    double pnsum_a, pnsum_b, rnet, IP, IPerr, ip, iperr, IPT=0.;

    if(imuon) { // a muon tagger is present
      debug()<< "Tagger Muon: " << imuon->p()/GeV <<endreq;
      calcIP(imuon, RecVert, IP, IPerr); //re-calculate IP
      if(Pfit.size()) {
        calcIP(imuon, &Vfit, ip, iperr); //re-calculate IPT
        if(!iperr) IPT = ip/iperr;
      } else IPT = -1000.; 

      //calculate the result of neural net, the higher rnet is,
      //the more reliable is the tagger:
      rnet = m_nnet->MLPm(B0p, B0the, vtags.size(), L1tamp, 
                          imuon->p()/GeV, imuon->pt()/GeV, IP/IPerr, IPT);

      //pn is the probability that the tagger is giving the
      //correct answer for the B flavour. the hard-coded numbers
      //are obtained by fitting the ratio of the 'rnet' distribution
      //for right muon tags and wrong muon tags:
      pn.at(1) = 1.0-pol3(rnet, 1.2939, -2.0406, 0.90781); //1-omega
    }
    if(iele) { //same story for all taggers, see above comments^
      debug()<< "Tagger Ele : " << iele->p()/GeV <<endreq;
      calcIP(iele, RecVert, IP, IPerr);
      if(Pfit.size()) {
        calcIP(iele, &Vfit, ip, iperr);
        if(!iperr) IPT = ip/iperr;
      } else IPT = -1000.; 
      rnet = m_nnet->MLPe(B0p, B0the, vtags.size(), L1tamp, 
                          iele->p()/GeV, iele->pt()/GeV, IP/IPerr, IPT);
      pn.at(2) = 1.0-pol4(rnet, 0.4933, -0.6766, 1.761, -1.587); 
    }
    if(ikaon) {
      debug()<< "Tagger Kaon: " << ikaon->p()/GeV <<endreq;
      calcIP(ikaon, RecVert, IP, IPerr);
      if(Pfit.size()) {
        calcIP(ikaon, &Vfit, ip, iperr);
        if(!iperr) IPT = ip/iperr;
      } else IPT = -1000.; 
      rnet = m_nnet->MLPk(B0p, B0the, vtags.size(), L1tamp, 
                          ikaon->p()/GeV, ikaon->pt()/GeV,IP/IPerr, IPT);
      pn.at(3) = 1.0-pol2(rnet, 0.52144, -0.27136);
    }
    if(ikaonS) {
      debug()<< "Tagger KaonS: " << ikaonS->p()/GeV <<endreq;
      double ang = asin((ikaonS->pt()/GeV)/(ikaonS->p()/GeV));
      double deta= log(tan(B0the/2.))-log(tan(ang/2.));
      double dphi= std::min(fabs(ikaonS->momentum().phi()-B0phi), 
                            6.283-fabs(ikaonS->momentum().phi()-B0phi));
      double dQ  = (ptotB+ikaonS->momentum()).m()/GeV - B0mass;
      calcIP(ikaonS, RecVert, IP, IPerr);
      if(Pfit.size()) {
        calcIP(ikaonS, &Vfit, ip, iperr);
        if(!iperr) IPT = ip/iperr;
      } else IPT = -1000.; 
      rnet = m_nnet->MLPkS(B0p, B0the, vtags.size(), L1tamp, 
                           ikaonS->p()/GeV, ikaonS->pt()/GeV,IP/IPerr, IPT,
                           deta, dphi, dQ);
      pn.at(4) = 1.0-pol2(rnet, 1.0007, -1.0049);
    }
    if(ipionS) {
      debug()<< "Tagger PionS: " << ipionS->p()/GeV <<endreq;
      double ang = asin((ipionS->pt()/GeV)/(ipionS->p()/GeV));
      double deta= log(tan(B0the/2.))-log(tan(ang/2.));
      double dphi= std::min(fabs(ipionS->momentum().phi()-B0phi), 
                            6.283-fabs(ipionS->momentum().phi()-B0phi));
      double dQ  = (ptotB+ipionS->momentum()).m()/GeV - B0mass;
      calcIP(ipionS, RecVert, IP, IPerr);
      if(Pfit.size()) {
        calcIP(ipionS, &Vfit, ip, iperr);
        if(!iperr) IPT = ip/iperr;
      } else IPT = -1000.; 
      rnet = m_nnet->MLPpS(B0p, B0the, vtags.size(), L1tamp, 
                           ipionS->p()/GeV, ipionS->pt()/GeV,IP/IPerr, IPT,
                           deta, dphi, dQ);
      if(rnet > m_ProbMin) pn.at(4) = 1.0-pol2(rnet, 1.0772, -1.1632); 
      else { ipionS=0; itag.at(4) = 0; }
    }
    //Vertex charge tagger is given a fixed value for omega
    //and this is tuned a posteriori
    if( Vch ) pn.at(5) = 1.0 - m_VchOmega; 
  
    //---------------------------
    //Make final tagging decision
    pnsum_a= 0.50;             //hypothesis of truetag=+1
    pnsum_b= 0.50;             //hypothesis of truetag=-1
    for( int i = 1; i != 6; i++ ) { //multiply all probabilities
      double mtag = itag.at(i);
      pnsum_a *= ((1-mtag)/2+mtag* pn.at(i) ); // p
      pnsum_b *= ((1+mtag)/2-mtag* pn.at(i) ); //(1-p)
      debug()<<"i="<<i<<" itag="<<mtag<<" pn="<<pn.at(i)<<endreq;
    }
    if(pnsum_a > pnsum_b) tagdecision = +1;
    if(pnsum_a < pnsum_b) tagdecision = -1;
    //normalise probability to the only two possible flavours:
    pnsum = std::max(pnsum_a,pnsum_b) /(pnsum_a + pnsum_b);

    //throw away poorly significant tags
    if(pnsum < m_ProbMin) {
      pnsum = 0.50;
      tagdecision = 0;
    }
    debug() << "pnsum=" << pnsum <<endreq;
    
    //sort decision into categories
    //cat=1 will be least reliable, cat=5 most reliable
    //ProbMin is a small offset to adjust for range of pnsum
    double dpnsum = pnsum-m_ProbMin;
    if(tagdecision) catt = ( (int)(10.0*dpnsum/fabs(1.-dpnsum)) ) +1;
    if(catt>5) catt=5;
    if(catt<0) catt=0;
  }
  else {
    err() << "Unknown tagger combination: " 
          << m_CombinationTechnique<<endreq;
    return theTag;
  } 
  if(tagdecision == 0) catt=0; 

  ///OUTPUT to Logfile ---------------------------------------------------
  info() << "BTAGGING TAG   " << evt->runNum()
         << std::setw(4) << evt->evtNum()
         << std::setw(4) << trigger
         << std::setw(5) << tagdecision
         << std::setw(3) << catt       //category
         << std::setw(5) << itag.at(1) //mu decision
         << std::setw(3) << itag.at(2) //ele
         << std::setw(3) << itag.at(3) //Kopp
         << std::setw(3) << itag.at(4) //Ksame/Pi_same
         << std::setw(3) << itag.at(5) //VtxCharge
         << endreq;

  ///fill FlavourTag object ---------------------------------------------
  if(      tagdecision ==  1 ) theTag->setDecision( FlavourTag::bbar );
  else if( tagdecision == -1 ) theTag->setDecision( FlavourTag::b );
  else theTag->setDecision( FlavourTag::none );
  theTag->setCategory( catt );
  theTag->setOmega( 1-pnsum );
  theTag->setTaggedB( AXB0 );

  //fill in taggers info into FlavourTag object
  if(imuon)  { 
    Tagger* tagr = new Tagger;
    tagr->setType( Tagger::OS_Muon ); 
    tagr->setDecision( itag.at(1) );
    tagr->setOmega( 1-pn.at(1) );
    tagr->addTaggerPart(*imuon);
    theTag->addTagger(*tagr); //save it <--
    delete tagr;
  }
  if(iele)  { 
    Tagger* tagr = new Tagger;
    tagr->setType( Tagger::OS_Electron ); 
    tagr->setDecision( itag.at(2) );
    tagr->setOmega( 1-pn.at(2) );
    tagr->addTaggerPart(*iele);
    theTag->addTagger(*tagr); 
    delete tagr;
  }
  if(ikaon)  { 
    Tagger* tagr = new Tagger;
    tagr->setType( Tagger::OS_Kaon ); 
    tagr->setDecision( itag.at(3) );
    tagr->setOmega( 1-pn.at(3) );
    tagr->addTaggerPart(*ikaon);
    theTag->addTagger(*tagr);
    delete tagr;
  }
  if(ikaonS)  { 
    Tagger* tagr = new Tagger;
    tagr->setType( Tagger::SS_Kaon ); 
    tagr->setDecision( itag.at(4) );
    tagr->setOmega( 1-pn.at(4) );
    tagr->addTaggerPart(*ikaonS);
    theTag->addTagger(*tagr);
    delete tagr;
  }
  if(ipionS)  { 
    Tagger* tagr = new Tagger;
    tagr->setType( Tagger::SS_Pion ); 
    tagr->setDecision( itag.at(4) );
    tagr->setOmega( 1-pn.at(4) );
    tagr->addTaggerPart(*ipionS);
    theTag->addTagger(*tagr);
    delete tagr;
  }
  if(Vch)  { 
    Tagger* tagr = new Tagger;
    tagr->setType( Tagger::VtxCharge ); 
    tagr->setDecision( itag.at(5) );
    tagr->setOmega( 1-pn.at(5) );
    for(std::vector<const Particle*>::const_iterator ip=Pfit.begin(); ip!=Pfit.end(); ip++) tagr->addTaggerPart(**ip);
    theTag->addTagger(*tagr);
    delete tagr;
  }

  return theTag;
}
//=========================================================================
StatusCode BTaggingTool::finalize() { return StatusCode::SUCCESS; }

//==========================================================================
bool BTaggingTool::isinTree( const Particle* axp, 
                             std::vector<const Particle*>& sons ) {

  std::vector<const Particle*>::const_iterator ip;
  for( ip = sons.begin(); ip != sons.end(); ip++ ){
    if( (*ip)->origin() == axp->origin() ) {
      debug() << "excluding signal part: " 
              << axp->particleID().pid() <<" with p="<<axp->p()/GeV<<endreq;
      return true;
    }
  }
  return false;
}
//==========================================================================
StatusCode BTaggingTool::calcIP( const Particle* axp, 
                                 const Vertex* RecVert, 
                                 double& ip, double& iperr) {
  ip   =-100.0;
  iperr= 0.0;
  Hep3Vector ipVec;
  HepSymMatrix errMatrix;
  StatusCode sc = 
    m_Geom->calcImpactPar(*axp, *RecVert, ip, iperr, ipVec, errMatrix);
  if( sc ) {
    ip   = ipVec.z()>0 ? ip : -ip ; 
    iperr= iperr; 
  }
  return sc;
}
//=========================================================================
StatusCode BTaggingTool::calcIP( const Particle* axp, 
                                 const VertexVector PileUpVtx,
                                 double& ip, double& ipe) {
  double ipmin = 100000.0;
  double ipminerr = 0.0;
  Vertices::const_iterator iv;
  Hep3Vector ipVec;
  HepSymMatrix errMatrix;
  StatusCode sc, lastsc=SUCCESS;

  for(iv = PileUpVtx.begin(); iv != PileUpVtx.end(); iv++){
    double ipx, ipex;
    sc = m_Geom->calcImpactPar(*axp, **iv, ipx, ipex, ipVec, errMatrix);
    if( sc ) if( ipx < ipmin ) {
      ipmin = ipx;
      ipminerr = ipex;
    } else lastsc = sc;
  }
  ip  = ipmin;
  ipe = ipminerr;
  return lastsc;
}
//==========================================================================
long BTaggingTool::trackType( const Particle* axp ) {

  long trtyp=0;
  const ContainedObject* contObj = axp->origin();
  if (contObj) {
    const ProtoParticle* proto = dynamic_cast<const ProtoParticle*>(contObj);
    if ( proto) {
      const TrStoredTrack* track = proto->track();
      if     (track->forward()     ) trtyp = 1;
      else if(track->match()       ) trtyp = 2;
      else if(track->isUpstream()  ) trtyp = 3;
      else if(track->isDownstream()) trtyp = 4;
      else if(track->isVelotrack() ) trtyp = 5;
      else if(track->isTtrack()    ) trtyp = 6;
      else if(track->isBackward()  ) trtyp = 7;
      else if(track->follow()      ) trtyp = 8;
      else if(track->ksTrack()     ) trtyp = 9;
    }
  }
  
  return trtyp;
}
//==========================================================================
std::vector<const Particle*> BTaggingTool::toStdVector( const SmartRefVector<Particle>& refvector ){
  std::vector<const Particle*>  tvector;
  for( SmartRefVector<Particle>::const_iterator ip = refvector.begin();
       ip != refvector.end(); ++ip ) tvector.push_back( *ip );
  return tvector;
}
//==========================================================================
double BTaggingTool::pol2(double x, double a0, double a1) { return a0+a1*x; }
double BTaggingTool::pol3(double x, double a0, double a1, double a2) {
  return a0+a1*x+a2*x*x;
}
double BTaggingTool::pol4(double x, double a0, double a1, double a2, double a3){
  return a0+a1*x+a2*x*x+a3*pow(x,3);
}
//==========================================================================
//return a vector containing all daughters of signal 
std::vector<const Particle*> BTaggingTool::FindDaughters( const Particle* axp ) {

  std::vector<const Particle*> apv, apv2, aplist;
  apv.push_back(axp);  
  do {
    apv2.clear();
    for( std::vector<const Particle*>::const_iterator 
           ip=apv.begin(); ip!=apv.end(); ip++ ) {
      if( (*ip)->endVertex() ) {
        std::vector<const Particle*> tmp = toStdVector((*ip)->endVertex()->products());
        for( std::vector<const Particle*> ::const_iterator 
               itmp=tmp.begin(); itmp!=tmp.end(); itmp++){
          apv2.push_back(*itmp);
          aplist.push_back(*itmp);
        }
      }
    }
    apv = apv2;
  } while ( !apv2.empty() );
   	    
  return aplist;
}
//==========================================================================
