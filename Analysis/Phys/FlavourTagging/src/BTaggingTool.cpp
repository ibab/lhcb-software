// Include files 
#include "BTaggingTool.h"
// from DaVinci
//#include "Kernel/StringUtils.h"
#include "GaudiKernel/IIncidentSvc.h"

//--------------------------------------------------------------------------
// Implementation file for class : BTaggingTool
//
// Author: Marco Musy
//--------------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( BTaggingTool );

//=========================================================================
BTaggingTool::BTaggingTool( const std::string& type,
                            const std::string& name,
                            const IInterface* parent ) :
  GaudiTool ( type, name, parent ) {

  declareInterface<IBTaggingTool>(this);
  declareInterface<IIncidentListener>(this);

  declareProperty( "CombineTaggersName",
                   m_CombineTaggersName = "CombineTaggersProbability" );
  declareProperty( "UseVtxChargeWithoutOS", m_UseVtxOnlyWithoutOS = false );

  declareProperty( "IPPU_cut",     m_IPPU_cut    = 3.0 );
  declareProperty( "thetaMin_cut", m_thetaMin    = 0.012 );
  declareProperty( "distphi_cut",  m_distphi_cut = 0.005 );

  declareProperty( "RequireTrigger",  m_RequireTrigger = false );
  declareProperty( "RequireL0",       m_RequireL0      = false );
  declareProperty( "RequireL1",       m_RequireL1      = false );
  declareProperty( "RequireHLT",      m_RequireHLT     = false );
  declareProperty( "EnableMuonTagger",    m_EnableMuon    = true );
  declareProperty( "EnableElectronTagger",m_EnableElectron= true );
  declareProperty( "EnableKaonOSTagger",  m_EnableKaonOS  = true );
  declareProperty( "EnableKaonSSTagger",  m_EnableKaonSS  = true );
  declareProperty( "EnablePionTagger",    m_EnablePionSS  = true );
  declareProperty( "EnableVertexChargeTagger",m_EnableVertexCharge= true);
  declareProperty( "EnableJetSameTagger", m_EnableJetSame = false );
  declareProperty( "OutputLocation",
                   m_outputLocation = "/Event/Phys/BTaggingTool" );
  m_util = 0;
  m_descend =0;
  m_taggerMu=m_taggerEle=m_taggerKaon=0;
  m_taggerKaonS=m_taggerPionS=m_taggerVtx=0 ;
}
BTaggingTool::~BTaggingTool() {}; 

//==========================================================================
StatusCode BTaggingTool::initialize() {

  m_physd = tool<IPhysDesktop> ("PhysDesktop", this);
  if(! m_physd) {
    fatal() << "Unable to retrieve PhysDesktop"<< endreq;
    return StatusCode::FAILURE;
  }
  m_physd->imposeOutputLocation(m_outputLocation);

  // Register to the Incident service to be notified 
  incSvc()->addListener( this, IncidentType::EndEvent );

  m_util = tool<ITaggingUtils> ( "TaggingUtils", this );
  if( ! m_util ) {
    fatal() << "Unable to retrieve TaggingUtils tool "<< endreq;
    return StatusCode::FAILURE;
  }
  m_descend = tool<IParticleDescendants> ( "ParticleDescendants", this );
  if( ! m_descend ) {
    fatal() << "Unable to retrieve ParticleDescendants tool "<< endreq;
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
  m_taggerVtxCh= tool<ITagger> ("TaggerVertexChargeTool", this);
  if(! m_taggerVtxCh) {
    fatal() << "Unable to retrieve TaggerVertexChargeTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_taggerJetS = tool<ITagger> ("TaggerJetSameTool", this);
  if(! m_taggerJetS) {
    fatal() << "Unable to retrieve TaggerJetSameTool"<< endreq;
    return StatusCode::FAILURE;
  }
  m_combine = tool<ICombineTaggersTool> (m_CombineTaggersName, this);
  if(! m_combine) {
    fatal() << "Unable to retrieve "<< m_CombineTaggersName << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//==========================================================================
StatusCode BTaggingTool::tag( FlavourTag& theTag, const Particle* AXB ) {
  Particle::ConstVector p(0);
  return tag( theTag, AXB, 0, p ) ;
}
//==========================================================================
StatusCode BTaggingTool::tag( FlavourTag& theTag, const Particle* AXB, 
                              const RecVertex* RecVert ) {
  Particle::ConstVector p(0);
  return tag( theTag, AXB, RecVert, p );
}
//==========================================================================
StatusCode BTaggingTool::tag( FlavourTag& theTag, const Particle* AXB, 
                              const RecVertex* RecVert,
                              Particle::ConstVector& vtags ) {

  theTag.setDecision( FlavourTag::none );
  if ( ! AXB ) {
    err() << "No B hypothesis to tag!" << endreq;
    return StatusCode::FAILURE;
  }

  //build desktop
  //const Particle::ConstVector& parts = m_physd->particles();
  //m_physd->saveDesktop(); 
  Particle::ConstVector parts ;
  if ( !exist<Particles>( m_outputLocation+"/Particles" )) {
    debug() << "Making tagging particles to be saved in " 
            << m_outputLocation << endreq ;
    if( !(m_physd->getEventInput()) ) return StatusCode::SUCCESS;
    parts = m_physd->particles();
    if( !(m_physd->saveDesktop()) ) return StatusCode::SUCCESS;
  } else {
    debug() << "Getting tagging particles saved in " 
            << m_outputLocation << endmsg ;
    const Particles* ptmp = get<Particles>( m_outputLocation+"/Particles" );
    for( Particles::const_iterator icand = ptmp->begin(); 
         icand != ptmp->end(); icand++ ) {
      parts.push_back(*icand);
    }
  }

  const RecVertex::ConstVector& verts = m_physd->primaryVertices();
  debug() << "  Nr Vertices: "  << verts.size() 
          << "  Nr Particles: " << parts.size() <<endreq;
  
  //----------------------------
  RecVertex::ConstVector::const_iterator iv;
  if( ! RecVert ) {
    //NEEDS TO BE CHANGED:
    //if the prim vtx is not provided by the user,
    //choose as primary vtx the one with smallest IP wrt B signal
    //this is a guess for the actual PV chosen by the selection.
    double kdmin = 1000000;
    for(iv=verts.begin(); iv!=verts.end(); iv++){
      double ip, iperr;
      m_util->calcIP(AXB, *iv, ip, iperr);
      debug() << "Vertex IP="<< ip <<" iperr="<<iperr<<endreq;
      if(iperr) if( fabs(ip/iperr) < kdmin ) {
        kdmin = fabs(ip/iperr);
        RecVert = (*iv);
      }     
    }
    if( ! RecVert ) {
      err() <<"No Reconstructed Vertex!! Skip." <<endreq;
      return StatusCode::SUCCESS;
    }    
  }

  //build a vector of pileup vertices --------------------------
  RecVertex::ConstVector PileUpVtx(0); //contains all the other primary vtx's
  for(iv=verts.begin(); iv!=verts.end(); iv++){
    if( (*iv) == RecVert ) continue;
    PileUpVtx.push_back(*iv);
    debug() <<"Pileup Vtx z=" << (*iv)->position().z()/mm <<endreq;
  }

  //loop over Particles, preselect taggers ///////////////////preselection
  double distphi;
  theTag.setTaggedB( AXB );
  Particle::ConstVector::const_iterator ip;
  Particle::ConstVector axdaugh = m_descend->descendants( AXB );
  axdaugh.push_back( AXB );
  if( vtags.empty() ) { //tagger candidate list is not provided, build one
    for ( ip = parts.begin(); ip != parts.end(); ip++ ){

      if( (*ip)->p()/GeV < 2.0 ) continue;               
      if( (*ip)->momentum().theta() < m_thetaMin ) continue;
      if( (*ip)->charge() == 0 ) continue;               
      if( !(*ip)->proto() )      continue;
      if( !(*ip)->proto()->track() ) continue;
      if( (*ip)->proto()->track()->type() < 3 )   continue; 
      if( (*ip)->proto()->track()->type() > 4 )   continue; 
      if( (*ip)->p()/GeV  > 200 ) continue;
      if( (*ip)->pt()/GeV >  10 ) continue;
      if( isinTree( *ip, axdaugh, distphi ) )  continue ;//exclude signal
      if( distphi < m_distphi_cut ) continue;

      //calculate the min IP wrt all pileup vtxs
      double ippu, ippuerr;
      m_util->calcIP( *ip, PileUpVtx, ippu, ippuerr );
      //eliminate from vtags all parts coming from a pileup vtx
      if(ippuerr) if( ippu/ippuerr<m_IPPU_cut ) continue; //preselection

      //////////////////////////////////
      vtags.push_back(*ip);          // store tagger candidate
      ////////////////////////////////
    }
  } else {
    //tagger candidate list is already provided, it is the user responsibility
    //to check that there is not a signal B daughter inside...
    debug()<<"User tagger candidate list of size = "<<vtags.size()<<endreq;
  }

  //AXB is the signal B from selection
  bool isBd = false; if( AXB->particleID().hasDown() )   isBd = true;
  bool isBs = false; if( AXB->particleID().hasStrange()) isBs = true;
  bool isBu = false; if( AXB->particleID().hasUp() )     isBu = true;


  ///Choose Taggers ------------------------------------------------------ 
  debug() <<"evaluate taggers" <<endreq;
  Vertex::ConstVector allVtx;
  Tagger muon, elec, kaon, kaonS, pionS, vtxCh, jetS;
  if(m_EnableMuon)     muon = m_taggerMu   -> tag(AXB, RecVert, allVtx, vtags);

  if(m_EnableElectron) elec = m_taggerEle  -> tag(AXB, RecVert, allVtx, vtags);

  if(m_EnableKaonOS)   kaon = m_taggerKaon -> tag(AXB, RecVert, allVtx, vtags);

  if(m_EnableKaonSS) if(isBs)  
                       kaonS= m_taggerKaonS-> tag(AXB, RecVert, allVtx, vtags);

  if(m_EnablePionSS) if(isBd || isBu)
                       pionS= m_taggerPionS-> tag(AXB, RecVert, allVtx, vtags);

  if(m_EnableJetSame)  jetS = m_taggerJetS -> tag(AXB, RecVert, allVtx, vtags);

  if(m_EnableVertexCharge){ //S. POSS:
    //Remove in vtags all previously built OS taggers: duplicate in vtagsplusOS
    //those parts that appear as OS taggers so that TaggerVertex can kill the tag
    Particle::ConstVector vtagsPlusOS(0);
    Particle::ConstVector::const_iterator i, j;
    for ( i=vtags.begin(); i!=vtags.end(); i++ ){
      const ProtoParticle* iproto = (*i)->proto();
      for ( j=i+1; j!=vtags.end(); j++) if(iproto==(*j)->proto()) continue;
      if( m_UseVtxOnlyWithoutOS ) {
	if(muon.type()!=0) 
	  if(muon.taggerParts().at(0).proto() == iproto ) 
	    vtagsPlusOS.push_back(*i);
	if(elec.type()!=0) 
	  if(elec.taggerParts().at(0).proto() == iproto ) 
	    vtagsPlusOS.push_back(*i);
	if(kaon.type()!=0)
	  if(kaon.taggerParts().at(0).proto() == iproto ) 
	    vtagsPlusOS.push_back(*i);
      }
      vtagsPlusOS.push_back(*i);
    }
    vtxCh = m_taggerVtxCh -> tag(AXB, RecVert, allVtx, vtagsPlusOS);
  }

  std::vector<Tagger*> taggers;
  taggers.push_back(&muon);
  taggers.push_back(&elec);
  taggers.push_back(&kaon);
  if( isBs ) taggers.push_back(&kaonS);
  if( isBu || isBd ) taggers.push_back(&pionS);
  taggers.push_back(&vtxCh);

  debug()<<"tagger mu   1-w = "<< 1-muon.omega() <<endreq;
  debug()<<"tagger ele  1-w = "<< 1-elec.omega() <<endreq;
  debug()<<"tagger kO   1-w = "<< 1-kaon.omega() <<endreq;
  debug()<<"tagger k/pS 1-w = "<< 1-kaonS.omega()<<endreq;
  debug()<<"tagger vtx  1-w = "<< 1-vtxCh.omega()<<endreq;

    
  //----------------------------------------------------------------------
  //Now combine the individual tagger decisions into 
  //one final B flavour tagging decision. Such decision 
  //can be made and categorised in two ways: the "TDR" 
  //approach is based on the particle type, while the 
  //"Prob" approach is based on the wrong tag fraction

  debug() <<"combine taggers "<< taggers.size() <<endreq;
  int category = m_combine -> combineTaggers( theTag, taggers );

  ///OUTPUT to Logfile ---------------------------------------------------
  int sameside = kaonS.decision();
  if(!sameside) sameside = pionS.decision();

  info() << "BTAGGING TAG   " 
         << std::setw(5) << theTag.decision()
         << std::setw(3) << category
         << std::setw(5) << muon.decision()
         << std::setw(3) << elec.decision()
         << std::setw(3) << kaon.decision()
         << std::setw(3) << sameside
         << std::setw(3) << vtxCh.decision()
         << endreq;

  return StatusCode::SUCCESS;
}
//=========================================================================
StatusCode BTaggingTool::finalize() { return StatusCode::SUCCESS; }

//============================================================================
bool BTaggingTool::isinTree(const Particle* axp, Particle::ConstVector& sons, 
                            double& dist_phi){
  double p_axp  = axp->p();
  double pt_axp = axp->pt();
  double phi_axp= axp->momentum().phi();
  dist_phi=1000.;

  for( Particle::ConstVector::iterator ip = sons.begin(); 
       ip != sons.end(); ip++ ){

    double dphi = fabs(phi_axp-(*ip)->momentum().phi()); 
    if(dphi>3.1416) dphi=6.283-dphi;
    dist_phi= std::min(dist_phi, dphi);

    if( (    fabs(p_axp -(*ip)->p()) < 0.1 
             && fabs(pt_axp-(*ip)->pt())< 0.01 
             && fabs(phi_axp-(*ip)->momentum().phi())< 0.1 )
        || axp->proto()==(*ip)->proto() ) {
      debug() << "excluding signal part: " << axp->particleID().pid() 
              << " with p="<<p_axp/Gaudi::Units::GeV 
              << " pt="<<pt_axp/Gaudi::Units::GeV 
              << " proto_axp,ip="<<axp->proto()<<" "<<(*ip)->proto()<<endreq;
      return true;
    }
  }
  return false;
}
//=============================================================================
// Implementation of Listener interface
//=============================================================================
void BTaggingTool::handle(const Incident&){
  StatusCode sc = m_physd->cleanDesktop();
  if (msgLevel(MSG::VERBOSE)) verbose() << "Cleaned desktop" << endmsg ;
  if (!sc) Exception("Could not clean Desktop");
  return ;
}
