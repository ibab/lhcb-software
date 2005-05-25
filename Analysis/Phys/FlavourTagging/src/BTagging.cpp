// Include files 
#include "BTagging.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BTagging
//
// Author: Marco Musy
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<BTagging>          s_factory ;
const        IAlgFactory& BTaggingFactory = s_factory ; 

//=============================================================================
BTagging::BTagging(const std::string& name,
		   ISvcLocator* pSvcLocator) : DVAlgorithm(name, pSvcLocator){

  declareProperty( "Muon_Pt_cut", m_Pt_cut_muon = 1.2 );
  declareProperty( "Muon_P_cut",  m_P_cut_muon  = 5.0 );

  declareProperty( "Ele_Pt_cut",   m_Pt_cut_ele = 1.0 );
  declareProperty( "Ele_P_cut",    m_P_cut_ele  = 5.0 );
  declareProperty( "VeloChargeMin",m_VeloChMin    = 21  );
  declareProperty( "VeloChargeMax",m_VeloChMax    = 32  );
  declareProperty( "EoverP",       m_EoverP       = 0.8 );
  declareProperty( "Ele_upstreamTrack_LCS_cut", m_lcs_eu = 2.5 );

  declareProperty( "Kaon_Pt_cut",  m_Pt_cut_kaon = 0.4 );
  declareProperty( "Kaon_P_cut",   m_P_cut_kaon  = 3.0 );
  declareProperty( "Kaon_IP_cut",  m_IP_cut_kaon   = 3.5 );
  declareProperty( "Kaon_IPPU_cut",m_IPPU_cut_kaon = 3.0 );
  declareProperty( "Kaon_SecondaryVertex_IP_cut", m_IPT_k= 8.0 );
  declareProperty( "Kaon_forwardTrack_LCS_cut", m_lcs_kf = 2.5 );
  declareProperty( "Kaon_matchTrack_LCS_cut",   m_lcs_km = 1.4 );
  declareProperty( "Kaon_upstreamTrack_LCS_cut",m_lcs_ku = 2.5 );
  declareProperty( "Kaon_forwardTrack_IP_cut",  m_IP_kf  = 2.0 );
  declareProperty( "Kaon_matchTrack_IP_cut",    m_IP_km  = 1.0 );
  declareProperty( "Kaon_upstreamTrack_IP_cut", m_IP_ku  = 1.0 );

  declareProperty( "KaonSame_Pt_cut", m_Pt_cut_kaonS = 0.4 );
  declareProperty( "KaonSame_P_cut",  m_P_cut_kaonS  = 4.0 );
  declareProperty( "KaonSame_IP_cut", m_IP_cut_kaonS   = 2.5 );
  declareProperty( "KaonSame_Phi_cut",m_phicut_kaonS   = 1.1 );
  declareProperty( "KaonSame_Eta_cut",m_etacut_kaonS   = 1.0 );
  declareProperty( "KaonSame_dQ_cut", m_dQcut_kaonS    = 1.5 );

  declareProperty( "PionSame_Pt_cut", m_Pt_cut_pionS = 0.2 );
  declareProperty( "PionSame_P_cut",  m_P_cut_pionS  = 2.0 );
  declareProperty( "PionSame_IP_cut", m_IP_cut_pionS   = 3.0 );
  declareProperty( "PionSame_dQ_cut", m_dQcut_pionS    = 3.0 );
  declareProperty( "PionSame_dQ2_cut",m_dQ2cut_pionS   = 1.0 );
  declareProperty( "PionS_upstreamTrack_LCS_cut", m_lcs_pSu = 2.5 );

  declareProperty( "TrVeloChargeName",m_veloChargeName = "TrVeloCharge" );
  declareProperty( "SecondaryVertexType", m_SVtype = "none" );
  declareProperty( "CombinationTechnique", m_CombinationTechnique = "none");
  declareProperty( "ProbMin", m_ProbMin = 0.52);
  declareProperty( "VchOmega", m_VchOmega = 0.40);

  declareProperty( "TagOutputLocation", m_TagLocation = "" );

  declareProperty( "RequireTrigger", m_RequireTrigger= false );
  declareProperty( "RequireL1Tampering", m_RequireL1Tamp = false );
  declareProperty( "RequireL0", m_RequireL0 = false );
  declareProperty( "RequireL1", m_RequireL1 = false );
  declareProperty( "RequireHLT",m_RequireHLT= false );
  declareProperty( "EnableMuonTagger",m_EnableMuonTagger= true );
  declareProperty( "EnableElectronTagger",m_EnableElectronTagger= true );
  declareProperty( "EnableKaonOSTagger",m_EnableKaonOSTagger= true );
  declareProperty( "EnableKaonSSTagger",m_EnableKaonSSTagger= true );
  declareProperty( "EnablePionTagger",m_EnablePionTagger= true );
  declareProperty( "EnableVertexChargeTagger",
		   m_EnableVertexChargeTagger= true );
  m_vtxtool = 0;
  m_nnet = 0;
}
BTagging::~BTagging() {}; 

//=============================================================================
StatusCode BTagging::initialize() {

  m_veloCharge= tool<ITrVeloCharge> (m_veloChargeName, this);
  if ( ! m_veloCharge ) {   
    fatal() << " Unable to retrieve " << m_veloChargeName << endreq;
    return StatusCode::FAILURE;
  }
  m_vtxtool = tool<ISecondaryVertexTool> ("SecondaryVertexTool");
  if(! m_vtxtool) {
    fatal() << "Unable to retrieve SecondaryVertexTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_nnet = tool<INNetTool> ("NNetTool");
  if(! m_nnet) {
    fatal() << "Unable to retrieve NNetTool"<< endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode BTagging::execute() {

  setFilterPassed( false );
   
  // Retrieve informations about event
  EventHeader* evt=0;
  if( exist<EventHeader> (EventHeaderLocation::Default)) {
    evt = get<EventHeader> (EventHeaderLocation::Default);
  } else {
    err() << "Unable to Retrieve Event" << endreq;
    return StatusCode::SUCCESS;
  }
  // Retrieve trigger/tampering info
  int trigger=-1;
  double L0tamp=0;
  double L1tamp=100;
  debug()<<"Retrieve TrgDecision from "<<TrgDecisionLocation::Default<<endreq;
  TrgDecision* trg = 0;
  HltScore* hlt = 0 ;
  if (m_RequireTrigger || m_RequireL1Tamp) {
    if ( !exist<TrgDecision>(TrgDecisionLocation::Default) ){
      warning() << "No TrgDecision" << endmsg ;
      if( m_RequireL1Tamp){
        fatal()<<"You required the Tampering: please run the Trigger!"<<endreq;
        return StatusCode::FAILURE;
      }
    } else {
      trg = get<TrgDecision> (TrgDecisionLocation::Default);
      if ( !exist<HltScore>(HltScoreLocation::Default) ){
        warning() << "No HLT score" << endreq ;
      } else hlt = get<HltScore>(HltScoreLocation::Default) ;
    }
  } 
  if(trg) {
    // Select events on trigger
    if(m_RequireL0)  {if( !trg->L0() ) return StatusCode::SUCCESS;}
    if(m_RequireL1)  {if( !trg->L1() ) return StatusCode::SUCCESS;}
    if(m_RequireHLT) {
      if ( ! hlt ) return StatusCode::SUCCESS;
      if ( !(hlt->decision()) ) return StatusCode::SUCCESS; 
    }
    if ( 0!=hlt ) trigger = trg->L0() + trg->L1() *10 + 100*hlt->decision();
    else trigger = trg->L0() + trg->L1() *10 ;

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
  //PhysDeskTop
  const ParticleVector& parts = desktop()->particles();
  const VertexVector&   verts = desktop()->primaryVertices();    
  debug() << "  Nr Vertices: "  << verts.size() 
	  << "  Nr Particles: " << parts.size() <<endreq;

  //------------------------------------------ look for signal B in selection
  Particle* AXB0 = 0; //pointer to signal B
  ParticleVector axdaugh(0);
  bool isBd = false;
  bool isBs = false;
  bool isBu = false;
  HepLorentzVector ptotB=0;
  ParticleVector::const_iterator ipart;
  double B0mass=0, B0the=0, B0phi=0, B0p=0;

  for ( ipart = parts.begin(); ipart != parts.end(); ipart++){
    if((*ipart)->particleID().hasBottom()) {
      AXB0 = (*ipart);
      axdaugh = FindDaughters(AXB0); //vector of signal B daughters
      axdaugh.push_back(AXB0);
      if( AXB0->particleID().hasDown() )    isBd = true;
      if( AXB0->particleID().hasStrange() ) isBs = true;
      if( AXB0->particleID().hasUp() )      isBu = true;
      ptotB  = AXB0->momentum();
      B0mass = ptotB.m()/GeV;
      B0the  = ptotB.theta();
      B0phi  = ptotB.phi();
      B0p    = ptotB.vect().mag()/GeV;
      debug() << "Signal B Mass=" << B0mass 
	      << " with "<<axdaugh.size()-1<< " daughters" <<endreq;
      break;
    }  
  }
  if( axdaugh.size() == 0 ) {
    debug() <<"Unselected. Skip." <<endreq;
    return StatusCode::SUCCESS;
  }

  //choose as primary vtx the one with smallest IP wrt B signal
  //this is a guess for the actual PV chosen by the selection.
  //in the future there will be a more solid association in DaVinci
  Hep3Vector ipVec;
  HepSymMatrix errMatrix;
  Vertex* RecVert = 0; 
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
    return StatusCode::SUCCESS;
  }    

  //build a vector of pileup vertices --------------------------
  VertexVector PileUpVtx(0); //contains all the other primary vtx's
  for( iv=verts.begin(); iv!=verts.end(); iv++){
    if( (*iv)->type() != Vertex::Primary ) continue;
    if( (*iv) == RecVert ) continue;
    PileUpVtx.push_back(*iv);
    debug() <<"Pileup Vtx z=" << (*iv)->position().z()/mm <<endreq;
  }

  //loop over Particles, preselect taggers /////////////////////
  ParticleVector vtags(0);
  for ( ipart = parts.begin(); ipart != parts.end(); ipart++){

    if( (*ipart)->p()/GeV < 2.0 ) continue;                    //preselection
    if( (*ipart)->momentum().theta() < 0.012 ) continue;       //preselection
    if( (*ipart)->charge() == 0 ) continue;                    //preselection 
    if( isinTree( *ipart, axdaugh ) )  continue ;              //exclude signal
    if( trackType(*ipart) > 3 ) continue;                      //preselection

    //eliminate from vtags all parts coming from a pileup vtx
    //calculate the min IP wrt all pileup vtxs
    double ippu, ippuerr;
    calcIP( *ipart, PileUpVtx, ippu, ippuerr );
    if(ippuerr) if( ippu/ippuerr < m_IPPU_cut_kaon ) continue; //preselection

    ///////////////////////////////////////
    vtags.push_back(*ipart);            // store tagger candidate
    /////////////////////////////////////
  }

  ///--------------------------------------------------------- VertexCharge
  //look for a secondary Vtx due to opposite B
  double Vch = 0;  //contains the vertex charge
  Vertex Vfit(0);  //secondary vertex found
  ParticleVector Pfit(0); //particles in it
  if(m_EnableVertexChargeTagger){
    if     (m_SVtype=="TDR")  Vfit = m_vtxtool->SVertex( *RecVert, vtags );
    else if(m_SVtype=="NNet") Vfit = m_vtxtool->SVertexNN( *RecVert, vtags );
    else warning()<< "No Vertex Charge tag will be used! "<<m_SVtype<<endreq;
    Pfit = toStdVector(Vfit.products());
    if( Pfit.size() ) {
      ParticleVector::iterator kp;
      for(kp = Pfit.begin(); kp != Pfit.end(); kp++) Vch += (*kp)->charge();
      debug() << "  Vertex charge: " << Vch << endreq;
    }
  }

  ///------------------------------------------------------- Choose Taggers
  //select muons (elecs, kaons) taggers in vtags, if more
  //than one satisfies cuts, take the highest Pt one

  Particle *imuon, *iele, *ikaon, *ipionS, *ikaonS;
  imuon=iele=ikaon=ipionS=ikaonS=0;
  double ptmaxm = -99.0;
  double ptmaxe = -99.0;
  double ptmaxk = -99.0;
  double ptmaxkS= -99.0;
  double ptmaxpS= -99.0;
  double qk = 0;

  debug() << "-------- Tagging Candidates: " << vtags.size() <<endreq;
  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) {

    Particle* axp = (*ipart);

    //calculate some useful variables first
    long   ID   = axp->particleID().pid(); 
    double Emeas= -1;
    double lcs  = 1000.;
    ContainedObject* contObj = axp->origin();
    if (contObj) {
      ProtoParticle* proto = dynamic_cast<ProtoParticle*>(contObj);
      SmartRefVector<CaloHypo>& vcalo = proto->calo();
      if(vcalo.size()==1) Emeas = (*(vcalo.begin()))->e()/GeV;
      TrStoredTrack* track = proto->track();
      if((track->measurements()).size() > 5)
	lcs = track->lastChiSq()/((track->measurements()).size()-5);
    }
    long trtyp = trackType(axp);
    double P   = axp->p()/GeV;
    double Pt  = axp->pt()/GeV;
    double deta= fabs(log(tan(B0the/2.)/tan(asin(Pt/P)/2.)));
    double dphi= fabs(axp->momentum().phi() - B0phi); 
    if(dphi>3.1416) dphi=6.2832-dphi;
    double dQ  = (ptotB+axp->momentum()).m()/GeV - B0mass;

    //calculate signed IP wrt RecVert
    double IP, IPerr;
    calcIP(axp, RecVert, IP, IPerr);
    if(!IPerr) continue;
    double IPsig = fabs(IP/IPerr);

    //calculate signed IP wrt SecondaryVertex
    double IPT = -1000, ip, iperr;
    if(Pfit.size()){
      calcIP(axp, &Vfit, ip, iperr);
      if(iperr) IPT = ip/iperr;
    }

    //calculate min IP wrt all pileup vtxs 
    double IPPU = 10000;
    calcIP( axp, PileUpVtx, ip, iperr );
    if(iperr) IPPU = ip/iperr;

    //------------------------------------------
    //select tagger, take the highest Pt of each
    switch ( abs(ID) ) {
    case 13:                          // selects OS muon tagger
      if(m_EnableMuonTagger)
	if(Pt > m_Pt_cut_muon)
	  if(P > m_P_cut_muon)
	    if( Pt > ptmaxm ) { //Pt ordering
	      imuon = axp;
	      ptmaxm = Pt;
	    }   
      break;
    case 11:                          // selects OS electron tagger     
      if(m_EnableElectronTagger) 
	if(Pt > m_Pt_cut_ele)
	  if(P > m_P_cut_ele)
	    if(Emeas/P > m_EoverP || Emeas<0) {
	      double veloch = m_veloCharge->calculate(axp);
	      if(veloch > m_VeloChMin && veloch < m_VeloChMax)
		if(trtyp == 1) 
		  if( Pt > ptmaxe ) { 
		    iele = axp;
		    ptmaxe = Pt;
		  }
	    }
      break;
    case 321:                         // selects OS kaon tagger
      if(m_EnableKaonOSTagger)
	if(Pt > m_Pt_cut_kaon)
	  if(P > m_P_cut_kaon)
	    if(IPPU > m_IPPU_cut_kaon)
	      if(Pfit.size()==0 || fabs(IPT)<m_IPT_k )
		if((trtyp==1 && lcs<m_lcs_kf && fabs(IP)<m_IP_kf) ||
		   (trtyp==2 && lcs<m_lcs_km && fabs(IP)<m_IP_km) ||
		   (trtyp==3 && lcs<m_lcs_ku && fabs(IP)<m_IP_ku) )
		  if(IPsig > m_IP_cut_kaon ) {
		    qk += axp->charge();
		    if( Pt > ptmaxk ) { 
		      ikaon = axp;
		      ptmaxk = Pt;
		    }
		  }
      if( isBs ) {                    // selects SS kaon tagger
	if(m_EnableKaonSSTagger)
	  if(Pt > m_Pt_cut_kaonS) 
	    if(P > m_P_cut_kaonS) 
	      if(IPsig < m_IP_cut_kaonS)
		if(dphi < m_phicut_kaonS)
		  if(deta < m_etacut_kaonS)
		    if(dQ < m_dQcut_kaonS)
		      if( trtyp==1 || (trtyp==3 && lcs<m_lcs_eu) )
			if( Pt > ptmaxkS ) {
			  ikaonS = axp;
			  ptmaxkS = Pt;
			}
      }	
      break;
    case 211:                         // selects SS pion tagger
      if( isBd || isBu ) {
	if(m_EnablePionTagger) 
	  if(Pt > m_Pt_cut_pionS)
	    if(P > m_P_cut_pionS)
	      if(IPsig < m_IP_cut_pionS)
		if(dQ < m_dQcut_pionS ) 
		  if(trtyp==1 || (trtyp==3 && lcs<m_lcs_pSu))
		    if( Pt > ptmaxpS ) {
		      ipionS = axp;
		      ptmaxpS = Pt;
		    }
      }	
      break;
    }
    debug()<< " trtyp="<<trtyp<< " ID="<<ID<<" P="<<P<<" Pt="<<Pt<<endreq;
    debug()<< " deta=" <<deta << " dphi="<<dphi << " dQ="<<dQ <<endreq;
    debug()<< " IPsig="<<IPsig<< " IPPU="<<IPPU << " IPT="<<IPT<<endreq;

    //-------------------------------------------------------
  }//end of tagger cands selection

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


  //------------------------------------------------
  //Now combine the individual tagger decisions into 
  //a final B flavour tagging decision. Such decision 
  //can be made and categorised in two ways: the "TDR" 
  //approach is based on the particle type, while the 
  //"NNet" approach is based on the wrong tag fraction
  //----------------------------------------------- Combine tags: TDR approach 
  int catt=0;
  double tagdecision=0;

  if(m_CombinationTechnique == "TDR") {

    //in case ele and muon both exist kill lower Pt one
    if(imuon) if(iele) {
      if(imuon->pt()/GeV > iele->pt()/GeV) iele= 0; else imuon= 0;
    } 
    //extra cut for pions SS candidates
    if(ipionS) {
      double dQ = (ptotB+ipionS->momentum()).m()/GeV - B0mass;
      if( dQ > m_dQ2cut_pionS ) ipionS=0;
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
    else if(ic==    0 && Vch!=0 ) { // vertex charge
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

  //---------------------------------------------- Combine tags: NNet approach 
  else if(m_CombinationTechnique == "NNet") {

    std::vector<double> pn;
    for( int j=0; j!=6; j++ ) pn.push_back(0.50);
    double pnsum_a, pnsum_b, rnet, IP, IPerr, ip, iperr, IPT=0.;

    if(imuon) { // a muon tagger is present
      debug()<< "tagger muon: " << imuon->p()/GeV <<endreq;
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
    if(iele) { //same story for all taggers, see above^
      debug()<< "tagger ele : " << iele->p()/GeV <<endreq;
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
      debug()<< "tagger kaon: " << ikaon->p()/GeV <<endreq;
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
      debug()<< "tagger kaonS: " << ikaonS->p()/GeV <<endreq;
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
      debug()<< "tagger pionS: " << ipionS->p()/GeV <<endreq;
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
    double pnsum = std::max(pnsum_a,pnsum_b) /(pnsum_a + pnsum_b);

    //throw away poorly significant tags
    if(pnsum < m_ProbMin) {
      pnsum = 0.50;
      tagdecision = 0;
    }
    debug() << "pnsum=" << pnsum <<endreq;
    
    //sort decision into categories
    double dpnsum = pnsum-m_ProbMin;
    if(tagdecision) catt = ( (int)(10.0*dpnsum/fabs(1.-dpnsum)) ) +1;
    if(catt>5) catt=5;
    if(catt<0) catt=0;
  }
  else {
    fatal()<<"Unknown tagger combination: "<<m_CombinationTechnique<<endreq;
    return StatusCode::FAILURE;
  } 
  if(tagdecision == 0) catt=0; 

  ///OUTPUT to Logfile -----------------------------------------------------
  info() << "BTAGGING TAG   " << evt->runNum()
	 << std::setw(4) << evt->evtNum()
	 << std::setw(4) << trigger
	 << std::setw(5) << tagdecision
	 << std::setw(3) << catt  
	 << std::setw(5) << itag.at(1)
	 << std::setw(3) << itag.at(2)
	 << std::setw(3) << itag.at(3)
	 << std::setw(3) << itag.at(4)
	 << std::setw(3) << itag.at(5)
	 << endreq;

  ///OUTPUT to TES ---------------------------------------------------------
  FlavourTag* theTag = new FlavourTag;
  theTag->setTaggedB( AXB0 );
  if(     tagdecision == 1) theTag->setDecision( FlavourTag::bbar );
  else if(tagdecision ==-1) theTag->setDecision( FlavourTag::b );
  else theTag->setDecision( FlavourTag::none );
  theTag->setCategory(catt);

  FlavourTags* tags = new FlavourTags;
  tags->insert(theTag);
  StatusCode sc= eventSvc()->registerObject( m_TagLocation + "/Tags", tags );
  if( !sc ) err() << "Unable to register tags"<< endreq;

  Particles* tagCands = new Particles;  
  if(imuon)  tagCands->insert( imuon );
  if(iele )  tagCands->insert( iele  );
  if(ikaon)  tagCands->insert( ikaon );
  if(ikaonS) tagCands->insert( ikaonS );
  if(ipionS) tagCands->insert( ipionS );

  sc= eventSvc()->registerObject( m_TagLocation + "/Taggers", tagCands);
  if( sc.isFailure() ) err() << "Unable to register candidates."<< endreq;  

  setFilterPassed( true );
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode BTagging::finalize() { return StatusCode::SUCCESS; }

//============================================================================
bool BTagging::isinTree( Particle* axp, ParticleVector& sons ) {

  for( ParticleVector::iterator ip = sons.begin(); ip != sons.end(); ip++ ){
    if( (*ip)->origin() == axp->origin() ) {
      debug() << "excluding signal part: " 
	      << axp->particleID().pid() <<" with p="<<axp->p()/GeV<<endreq;
      return true;
    }
  }
  return false;
}
//=============================================================================
StatusCode BTagging::calcIP( Particle* axp, Vertex* RecVert, 
			     double& ip, double& iperr) {
  ip   =-100.0;
  iperr= 0.0;
  Hep3Vector ipVec;
  HepSymMatrix errMatrix;
  StatusCode sc = 
    geomDispCalculator()->calcImpactPar(*axp, *RecVert, ip,
					iperr, ipVec, errMatrix);
  if( sc ) {
    ip   = ipVec.z()>0 ? ip : -ip ; 
    iperr= iperr; 
  }
  return sc;
}
//=============================================================================
StatusCode BTagging::calcIP( Particle* axp, const VertexVector PileUpVtx,
			     double& ip, double& ipe) {
  double ipmin = 100000.0;
  double ipminerr = 0.0;
  Vertices::const_iterator iv;
  Hep3Vector ipVec;
  HepSymMatrix errMatrix;
  StatusCode sc, lastsc=SUCCESS;

  for(iv = PileUpVtx.begin(); iv != PileUpVtx.end(); iv++){
    double ipx, ipex;
    sc = geomDispCalculator()->calcImpactPar(*axp, **iv, ipx,
					     ipex, ipVec, errMatrix);
    if( sc ) if( ipx < ipmin ) {
      ipmin = ipx;
      ipminerr = ipex;
    } else lastsc = sc;
  }
  ip  = ipmin;
  ipe = ipminerr;
  return lastsc;
}
//=============================================================================
long BTagging::trackType( Particle* axp ) {

  long trtyp=0;
  ContainedObject* contObj = axp->origin();
  if (contObj) {
    ProtoParticle* proto = dynamic_cast<ProtoParticle*>(contObj);
    TrStoredTrack* track = proto->track();
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
  return trtyp;
}
//=============================================================================
ParticleVector BTagging::toStdVector( SmartRefVector<Particle>& refvector ) {
  ParticleVector tvector(0);
  for( SmartRefVector<Particle>::iterator ip = refvector.begin();
       ip != refvector.end(); ++ip ) tvector.push_back( *ip );
  return tvector;
}
//=============================================================================
double BTagging::pol2(double x, double a0, double a1) { return a0+a1*x; }
double BTagging::pol3(double x, double a0, double a1, double a2) {
  return a0+a1*x+a2*x*x;
}
double BTagging::pol4(double x, double a0, double a1, double a2, double a3) {
  return a0+a1*x+a2*x*x+a3*pow(x,3);
}
//=============================================================================
//return a vector containing all daughters of signal 
ParticleVector BTagging::FindDaughters( Particle* axp ) {

  ParticleVector apv(0), apv2, aplist(0);
  apv.push_back(axp);

  do {
    apv2.clear();
    for( Particles::iterator ip=apv.begin(); ip!=apv.end(); ip++ ) {
      if( (*ip)->endVertex() ) {
	ParticleVector tmp = toStdVector((*ip)->endVertex()->products());
	for( Particles::iterator itmp=tmp.begin(); itmp!=tmp.end(); itmp++) {
	  apv2.push_back(*itmp);
	  aplist.push_back(*itmp);
	}
      }
    }
    apv = apv2;
  } while ( apv2.size() );
   	    
  return aplist;
}
//=============================================================================
