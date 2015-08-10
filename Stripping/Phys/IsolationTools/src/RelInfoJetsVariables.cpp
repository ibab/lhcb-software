// Include files

// local
#include "RelInfoJetsVariables.h"

// global
#include <Kernel/IDVAlgorithm.h>
#include <Kernel/GetIDVAlgorithm.h>
#include "Event/Particle.h"
#include "LoKi/ParticleCuts.h"
#include "Kernel/RelatedInfoNamed.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RelInfoJetsVariables
//
// 2014-06-30 : Xabier Cid Vidal
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( RelInfoJetsVariables )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RelInfoJetsVariables::RelInfoJetsVariables( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent):
GaudiTool ( type, name , parent )
{

  declareInterface<IRelatedInfoTool>(this);
  declareProperty( "Variables",
                   m_variables,
                   "List of variables to store (store all if empty)");

  declareProperty( "ForceSamePV",
                   m_forcePV=true,
                   "Force only banned jets from same PV as initial B");

  declareProperty("LocationJetsNoMu",
                  m_loc_jetsnomu="Phys/StdJetsNoJetIDNoMuFromB/Particles",
                  "Location of the jets where muons have been removed");

  declareProperty("LocationJetsNoRemove",
                  m_loc_jetsinc="Phys/StdJetsNoJetID/Particles",
                  "Location of the jets where no particle has been removed");

  declareProperty("LocationJetsForceB",
                  m_loc_jetsb="Phys/StdJetsNoJetIDForceB/Particles",
                  "Location of the jets where the B has been forced as a single particle");

  declareProperty("UseVarsJetsNoMu",
                  m_use_jetsnomu=true,
                  "Store variables from the jets in which muons from B have been removed");

  declareProperty("UseVarsJetsWithMu",
                  m_use_jetsinc=true,
                  "Store variables from the jets in which the B muons are present");

  declareProperty("UseVarsJetsWithB",
                  m_use_jetsb=true,
                  "Store variables from the jets in which the B muons are present");


  declareProperty("IndividualParticlePID",m_ind_part=13,
                  "This is initially for Bmunu or similar, the PID that should be searched for");
}

//=============================================================================
// Destructor
//=============================================================================
RelInfoJetsVariables::~RelInfoJetsVariables() {}


//====================================================================
// Initialize DVAlg, etc
//====================================================================
StatusCode RelInfoJetsVariables::initialize() {

  // initialize the base class  (the first action)
  StatusCode sc = GaudiTool::initialize();
  if(sc.isFailure()) return sc;

  // tistos
  m_TriggerTisTosTool = tool<ITriggerTisTos>( "Hlt2TriggerTisTos","Hlt2TriggerTisTos",this );
  if ( !m_TriggerTisTosTool )
    return Error("Couldn't get requested jet tag tool");

  //initialize the nntag tool
  //m_nnjettag = tool<IJetTagTool>("LoKi::NNBTag",this);

  //initialize the tool keys
  m_keys.clear();
  if ( msgLevel(MSG::VERBOSE) ){
    verbose()<<"RelatedInfoNamed::PFALLMISSPT="<<RelatedInfoNamed::PFALLMISSPT<<endmsg;
    verbose()<<"RelatedInfoNamed::JETNOMU1PX:="<<RelatedInfoNamed::JETNOMU1PX<<endmsg;
    verbose()<<"RelatedInfoNamed::JETNOMU1PY:="<<RelatedInfoNamed::JETNOMU1PY<<endmsg;
    verbose()<<"RelatedInfoNamed::JETNOMU1PZ:="<<RelatedInfoNamed::JETNOMU1PZ<<endmsg;
    verbose()<<"RelatedInfoNamed::JETNOMU1PT:="<<RelatedInfoNamed::JETNOMU1PT<<endmsg;
    verbose()<<"RelatedInfoNamed::JETNOMU1JETWIDTH:="<<RelatedInfoNamed::JETNOMU1JETWIDTH<<endmsg;
    verbose()<<"RelatedInfoNamed::JETNOMU1NNTAG:="<<RelatedInfoNamed::JETNOMU1NNTAG<<endmsg;
    verbose()<<"RelatedInfoNamed::JETNOMU1MNF:="<<RelatedInfoNamed::JETNOMU1MNF<<endmsg;
    verbose()<<"RelatedInfoNamed::JETNOMU2PX:="<<RelatedInfoNamed::JETNOMU2PX<<endmsg;
    verbose()<<"RelatedInfoNamed::JETNOMU2PY:="<<RelatedInfoNamed::JETNOMU2PY<<endmsg;
    verbose()<<"RelatedInfoNamed::JETNOMU2PZ:="<<RelatedInfoNamed::JETNOMU2PZ<<endmsg;
    verbose()<<"RelatedInfoNamed::JETNOMU2PT:="<<RelatedInfoNamed::JETNOMU2PT<<endmsg;
    verbose()<<"RelatedInfoNamed::JETNOMU2JETWIDTH:="<<RelatedInfoNamed::JETNOMU2JETWIDTH<<endmsg;
    verbose()<<"RelatedInfoNamed::JETNOMU2NNTAG:="<<RelatedInfoNamed::JETNOMU2NNTAG<<endmsg;
    verbose()<<"RelatedInfoNamed::JETNOMU2MNF:="<<RelatedInfoNamed::JETNOMU2MNF<<endmsg;
    verbose()<<"RelatedInfoNamed::JETNOMU3PX:="<<RelatedInfoNamed::JETNOMU3PX<<endmsg;
    verbose()<<"RelatedInfoNamed::JETNOMU3PY:="<<RelatedInfoNamed::JETNOMU3PY<<endmsg;
    verbose()<<"RelatedInfoNamed::JETNOMU3PZ:="<<RelatedInfoNamed::JETNOMU3PZ<<endmsg;
    verbose()<<"RelatedInfoNamed::JETNOMU3PT:="<<RelatedInfoNamed::JETNOMU3PT<<endmsg;
    verbose()<<"RelatedInfoNamed::JETNOMU3JETWIDTH:="<<RelatedInfoNamed::JETNOMU3JETWIDTH<<endmsg;
    verbose()<<"RelatedInfoNamed::JETNOMU3NNTAG:="<<RelatedInfoNamed::JETNOMU3NNTAG<<endmsg;
    verbose()<<"RelatedInfoNamed::JETNOMU3MNF:="<<RelatedInfoNamed::JETNOMU3MNF<<endmsg;
    verbose()<<"RelatedInfoNamed::JETMU1PX:="<<RelatedInfoNamed::JETMU1PX<<endmsg;
    verbose()<<"RelatedInfoNamed::JETMU1PY:="<<RelatedInfoNamed::JETMU1PY<<endmsg;
    verbose()<<"RelatedInfoNamed::JETMU1PZ:="<<RelatedInfoNamed::JETMU1PZ<<endmsg;
    verbose()<<"RelatedInfoNamed::JETMU1PT:="<<RelatedInfoNamed::JETMU1PT<<endmsg;
    verbose()<<"RelatedInfoNamed::JETMU1JETWIDTH:="<<RelatedInfoNamed::JETMU1JETWIDTH<<endmsg;
    verbose()<<"RelatedInfoNamed::JETMU1NNTAG:="<<RelatedInfoNamed::JETMU1NNTAG<<endmsg;
    verbose()<<"RelatedInfoNamed::JETMU1MNF:="<<RelatedInfoNamed::JETMU1MNF<<endmsg;
    verbose()<<"RelatedInfoNamed::JETMU2PX:="<<RelatedInfoNamed::JETMU2PX<<endmsg;
    verbose()<<"RelatedInfoNamed::JETMU2PY:="<<RelatedInfoNamed::JETMU2PY<<endmsg;
    verbose()<<"RelatedInfoNamed::JETMU2PZ:="<<RelatedInfoNamed::JETMU2PZ<<endmsg;
    verbose()<<"RelatedInfoNamed::JETMU2PT:="<<RelatedInfoNamed::JETMU2PT<<endmsg;
    verbose()<<"RelatedInfoNamed::JETMU2JETWIDTH:="<<RelatedInfoNamed::JETMU2JETWIDTH<<endmsg;
    verbose()<<"RelatedInfoNamed::JETMU2NNTAG:="<<RelatedInfoNamed::JETMU2NNTAG<<endmsg;
    verbose()<<"RelatedInfoNamed::JETMU2MNF:="<<RelatedInfoNamed::JETMU2MNF<<endmsg;
    verbose()<<"RelatedInfoNamed::JETBPX:="<<RelatedInfoNamed::JETBPX<<endmsg;
    verbose()<<"RelatedInfoNamed::JETBPY:="<<RelatedInfoNamed::JETBPY<<endmsg;
    verbose()<<"RelatedInfoNamed::JETBPZ:="<<RelatedInfoNamed::JETBPZ<<endmsg;
    verbose()<<"RelatedInfoNamed::JETBPT:="<<RelatedInfoNamed::JETBPT<<endmsg;
    verbose()<<"RelatedInfoNamed::JETBJETWIDTH:="<<RelatedInfoNamed::JETBJETWIDTH<<endmsg;
    verbose()<<"RelatedInfoNamed::JETBNNTAG:="<<RelatedInfoNamed::JETBNNTAG<<endmsg;
    verbose()<<"RelatedInfoNamed::JETBMNF:="<<RelatedInfoNamed::JETBMNF<<endmsg;
  }


  for ( const auto& var : m_variables )
  {

    if ( msgLevel(MSG::VERBOSE) ){
      verbose()<<"var="<<var<<endmsg;
      for (int i=0; i<=RelatedInfoNamed::PFALLMISSPT; i++){
        verbose()<<"nameByIndex("<<i<<")="<< RelatedInfoNamed::nameByIndex(i) << endmsg;
        verbose()<<"var.compare(nameByIndex("<<i<<") )="<<var.compare(RelatedInfoNamed::nameByIndex(i) )<<endmsg;
      }
    }


    short int key = RelatedInfoNamed::indexByName( var );
    if (key != RelatedInfoNamed::UNKNOWN) {
      m_keys.push_back( key );
      debug() << "Adding variable " << var << ", key = " << key << endmsg;
    } else {
      Warning( std::string("Unknown variable ")+ var +", skipping" );
    }
  }

  debug()<< "Initialization correct" << endmsg;
  return StatusCode::SUCCESS;
}

//==============================================================================
// Determine the maximum ratio of shard LHCbID by an HLT2 obj in a jet
//=============================================================================
double RelInfoJetsVariables::jetNNTag(const LHCb::Particle* jet)
{

  std::vector<const LHCb::HltObjectSummary*> hltObjs
    = m_TriggerTisTosTool->hltObjectSummaries("Hlt2Topo.*Decision",2,2);

  unsigned int num = hltObjs.size();

  std::vector< LHCb::LHCbID > AllIDs;
  AllIDs.clear();
  getJetLHCbIDs(jet, AllIDs);
  double maxRatio = 0.;
  for(unsigned int i = 0; i < num; i++){
    std::vector< LHCb::LHCbID > hltLHCbIDs;
    hltLHCbIDs.clear();
    getHltObjLHCbIDs((hltObjs[i]),hltLHCbIDs);
    double TotN = (double) hltLHCbIDs.size();
    double TotMatching = 0.0;
    double ratio = 0.;
    for(std::vector<LHCb::LHCbID>::iterator iID1 = hltLHCbIDs.begin(); iID1!= hltLHCbIDs.end(); iID1++){
      for(std::vector<LHCb::LHCbID>::iterator iAllIDs = AllIDs.begin(); iAllIDs != AllIDs.end(); iAllIDs++){
        if((*iID1).lhcbID() == (*iAllIDs).lhcbID()){
          TotMatching+=1.0;
          ratio = TotMatching/TotN;
        }
      }
    }

    if(ratio > maxRatio){
      maxRatio = ratio;
    }

  }

  return maxRatio;
}




//=========================================================================
//Get the LHCbID of HLT obj
//=========================================================================
StatusCode RelInfoJetsVariables::getHltObjLHCbIDs(const LHCb::HltObjectSummary * sum, std::vector< LHCb::LHCbID > & AllIDs) const{
  if (0==sum) return StatusCode::SUCCESS ;
  if(sum->substructure().size()>0){
    for ( SmartRefVector< LHCb::HltObjectSummary >::const_iterator s = sum->substructure().begin() ;
          s != sum->substructure().end() ; ++s)
      getHltObjLHCbIDs(*s,AllIDs);


    const std::vector< LHCb::LHCbID > lIDs = sum->lhcbIDsFlattened();
    AllIDs.insert(AllIDs.end(), lIDs.begin(), lIDs.end());


  }else{

    const std::vector< LHCb::LHCbID > lIDs = sum->lhcbIDsFlattened();
    AllIDs.insert(AllIDs.end(), lIDs.begin(), lIDs.end());
  }

  return StatusCode::SUCCESS ;

}




//=========================================================================
//Get daughters track the LHCbID of a jet.
//=========================================================================
StatusCode RelInfoJetsVariables::getJetLHCbIDs(const LHCb::Particle* p,
                                               std::vector< LHCb::LHCbID > & AllIDs) const{

  if(p->particleID().abspid() == 98 ){
    LHCb::Particle::ConstVector daus = p->daughtersVector();
    for(LHCb::Particle::ConstVector::iterator idaus = daus.begin(); idaus != daus.end(); idaus++)
    {
      getJetLHCbIDs( (*idaus), AllIDs );
    }
  }else{
    const LHCb::ProtoParticle * proto = p->proto() ;
    if ( proto )
    {
      if ( proto->track() ) {
        const std::vector< LHCb::LHCbID > lIDs = proto->track()->lhcbIDs();
        AllIDs.insert(AllIDs.end(), lIDs.begin(), lIDs.end());
      }
      else
      {
        for ( SmartRefVector<LHCb::Particle>::const_iterator iP = p->daughters().begin();
              iP != p->daughters().end(); ++iP )
        {
          getJetLHCbIDs( *iP, AllIDs );
        }
      }
    }
  }
  return StatusCode::SUCCESS ;
}


//=============================================================================
// Function to sort according to pT and same PV as B
//=============================================================================
void RelInfoJetsVariables::pt_sorted_samePV(const LHCb::Particles & jets_list,
                                            const int pvkey, std::vector<LHCb::Particle*> & out_list)
{
  // get DV algorithm
  IDVAlgorithm* dva = Gaudi::Utils::getIDVAlgorithm( contextSvc() ) ;
  if ( !dva ) return;

  std::vector<PtParticlePair> myPtParticleVector;
  debug()<< jets_list.size()  <<" jets to be sorted" << endmsg;
  for( LHCb::Particles::const_iterator ijet = jets_list.begin() ;
       ijet != jets_list.end() ; ++ijet){

    // if pvkey<0, we skip this check
    if (m_forcePV && pvkey>=0){
      const LHCb::VertexBase* BPV = dva->bestVertex(*ijet);
      // only add if the pv is the same as the one of the B
      if (BPV->key()!=pvkey) continue;
    }
    PtParticlePair mypair((*ijet)->pt(),(*ijet));
    myPtParticleVector.push_back(mypair);
  }
  //sort by pT (smaller to larger)
  std::sort(myPtParticleVector.begin(), myPtParticleVector.end());
  //and now reverse it to have from larger to smaller
  std::reverse(myPtParticleVector.begin(), myPtParticleVector.end());
  debug()<< myPtParticleVector.size()  <<" jets sorted" << endmsg;

  // now store only the jets again
  for( std::vector<PtParticlePair>::const_iterator ipair = myPtParticleVector.begin() ;
       ipair != myPtParticleVector.end() ; ++ipair){
    out_list.push_back(ipair->second->clone());
  }
  debug()<< "Jet list ready" << endmsg;
}

//=============================================================================
// Find jet with particle
//=============================================================================
StatusCode RelInfoJetsVariables::find_jet_mu(const LHCb::Particles* list_of_jets,
                                             const LHCb::Particle mu, LHCb::Particle& myjet)
{

  debug() << "Looking for jets with muon "<< mu.key() << " inside" <<endmsg;
  LHCb::Particles jets_list;
  for( LHCb::Particles::const_iterator ijet = list_of_jets->begin() ;
       ijet != list_of_jets->end() ; ++ijet){

    // for each jet, loop in all the daughters
    for( SmartRefVector< LHCb::Particle >::const_iterator idau = (*ijet)->daughters().begin() ;
         idau != (*ijet)->daughters().end() ; ++idau){
      const LHCb::Particle dau = (*(*idau));
      // check if the daughter has the same key as the particle
      if (!(dau.proto())) continue;
      if (!(dau.proto()->track())) continue;
      if (dau.proto()->track()->key()==mu.proto()->track()->key())
      {
        jets_list.insert(*ijet,(*ijet)->key());
        debug()<< "Jet " << (*ijet)->key() << " has the muon inside!" <<endmsg;
        break;
      }

    }
  }
  // if more than 1, store the one with larger pT
  if (!jets_list.size()) return StatusCode::FAILURE;
  debug()<< jets_list.size() << " jets found with the muon inside" <<endmsg;
  std::vector<LHCb::Particle*> jets_list_sorted;
  pt_sorted_samePV(jets_list,-1, jets_list_sorted); // the -1 is to avoid any PV check
  // this is not needed because the PV of the muon should be the same as the PV of the jet (since the muon is inside)
  debug()<< " These jets have been sorted " <<endmsg;
  if (!jets_list_sorted.size()) return StatusCode::FAILURE;
  myjet = *(jets_list_sorted.at(0));
  return StatusCode::SUCCESS;
}


//=============================================================================
// Find jet with bottom
//=============================================================================
StatusCode RelInfoJetsVariables::find_jet_b(const LHCb::Particles* list_of_jets,
                                            LHCb::Particle& myjet)
{
  LHCb::Particles jets_list;
  for( LHCb::Particles::const_iterator ijet = list_of_jets->begin() ;
       ijet != list_of_jets->end() ; ++ijet){
    // for each jet, loop in all the daughters and store the jet if a B is part of the daughters
    for( SmartRefVector< LHCb::Particle >::const_iterator idau = (*ijet)->daughters().begin() ;
         idau != (*ijet)->daughters().end() ; ++idau){
      if ((*idau)->particleID().hasBottom()) {
        jets_list.insert(*ijet,(*ijet)->key());
        debug()<< "Jet " << (*ijet)->key() << " has the B inside!" <<endmsg;
        break;
      }

    }
  }
  // save the one with larger pT
  if (!jets_list.size()) return StatusCode::FAILURE;
  debug()<< jets_list.size() << " jets found with the B inside" <<endmsg;
  std::vector<LHCb::Particle*> jets_list_sorted;
  pt_sorted_samePV(jets_list,-1,jets_list_sorted); // the -1 is to avoid any PV check,
  // this is not needed because the PV of the B should be the same as the PV of the jet (since the B is inside)
  if (!jets_list_sorted.size()) return StatusCode::FAILURE;
  debug()<< " These jets have been sorted " <<endmsg;
  myjet = *(jets_list_sorted.at(0));
  return StatusCode::SUCCESS;
}


//=============================================================================
// Empty properties list
//=============================================================================
std::map <std::string,double> RelInfoJetsVariables::emptyProperties(void)
{
  std::map <std::string,double> out;
  out["Px"]= -10.;
  out["Py"]= -10.;
  out["Pz"]= -10.;
  out["PT"]= -10.;
  out["JetWidth"]= -10.;
  out["NNTag"]= -10.;
  out["MNF"]= -10.;
  debug() << "empty dict created" << endmsg;
  return out;
}


//=============================================================================
// Empty properties list
//=============================================================================
std::map <std::string,double> RelInfoJetsVariables::fillProperties(LHCb::Particle* jet)
{
  std::map <std::string,double> out;
  out["Px"]= jet->momentum().x();
  out["Py"]= jet->momentum().y();
  out["Pz"]= jet->momentum().z();
  out["PT"]= jet->pt();
  out["JetWidth"]= LoKi::Cuts::INFO(9007,-10.)(jet);
  out["MNF"]= LoKi::Cuts::INFO(9012,-10.)(jet);
  out["NNTag"]= jetNNTag(jet);
  debug() << "Dictionary created for jet " << jet->key() << endmsg;
  debug() << "with value " << out << endmsg;
  return out;
}


//=============================================================================
// Fill Output m_map
//=============================================================================
void  RelInfoJetsVariables::fillMap(){

  m_map.clear();
  for ( const auto key : m_keys ){

    float value = 0;
    switch (key) {
    case RelatedInfoNamed::JETNOMU1PX    : value = m_JetNoMu1["Px"]; break;
    case RelatedInfoNamed::JETNOMU1PY    : value = m_JetNoMu1["Py"]; break;
    case RelatedInfoNamed::JETNOMU1PZ    : value = m_JetNoMu1["Pz"]; break;
    case RelatedInfoNamed::JETNOMU1PT    : value = m_JetNoMu1["PT"]; break;
    case RelatedInfoNamed::JETNOMU1JETWIDTH    : value = m_JetNoMu1["JetWidth"]; break;
    case RelatedInfoNamed::JETNOMU1NNTAG    : value = m_JetNoMu1["NNTag"];  break;
    case RelatedInfoNamed::JETNOMU1MNF    : value = m_JetNoMu1["MNF"];  break;
    case RelatedInfoNamed::JETNOMU2PX    : value = m_JetNoMu2["Px"];   break;
    case RelatedInfoNamed::JETNOMU2PY    : value = m_JetNoMu2["Py"];   break;
    case RelatedInfoNamed::JETNOMU2PZ    : value = m_JetNoMu2["Pz"];   break;
    case RelatedInfoNamed::JETNOMU2PT    : value = m_JetNoMu2["PT"];  break;
    case RelatedInfoNamed::JETNOMU2JETWIDTH    : value = m_JetNoMu2["JetWidth"];  break;
    case RelatedInfoNamed::JETNOMU2NNTAG    : value = m_JetNoMu2["NNTag"];  break;
    case RelatedInfoNamed::JETNOMU2MNF    : value = m_JetNoMu2["MNF"]; break;
    case RelatedInfoNamed::JETNOMU3PX    : value = m_JetNoMu3["Px"]; break;
    case RelatedInfoNamed::JETNOMU3PY    : value = m_JetNoMu3["Py"]; break;
    case RelatedInfoNamed::JETNOMU3PZ    : value = m_JetNoMu3["Pz"]; break;
    case RelatedInfoNamed::JETNOMU3PT    : value = m_JetNoMu3["PT"]; break;
    case RelatedInfoNamed::JETNOMU3JETWIDTH    : value = m_JetNoMu3["JetWidth"]; break;
    case RelatedInfoNamed::JETNOMU3NNTAG    : value = m_JetNoMu3["NNTag"];  break;
    case RelatedInfoNamed::JETNOMU3MNF    : value = m_JetNoMu3["MNF"]; break;
    case RelatedInfoNamed::JETMU1PX    : value = m_JetMu1["Px"]; break;
    case RelatedInfoNamed::JETMU1PY    : value = m_JetMu1["Py"]; break;
    case RelatedInfoNamed::JETMU1PZ    : value = m_JetMu1["Pz"]; break;
    case RelatedInfoNamed::JETMU1PT    : value = m_JetMu1["PT"]; break;
    case RelatedInfoNamed::JETMU1JETWIDTH    : value = m_JetMu1["JetWidth"]; break;
    case RelatedInfoNamed::JETMU1NNTAG    : value = m_JetMu1["NNTag"]; break;
    case RelatedInfoNamed::JETMU1MNF    : value = m_JetMu1["MNF"]; break;
    case RelatedInfoNamed::JETMU2PX    : value = m_JetMu2["Px"]; break;
    case RelatedInfoNamed::JETMU2PY    : value = m_JetMu2["Py"]; break;
    case RelatedInfoNamed::JETMU2PZ    : value = m_JetMu2["Pz"]; break;
    case RelatedInfoNamed::JETMU2PT    : value = m_JetMu2["PT"]; break;
    case RelatedInfoNamed::JETMU2JETWIDTH    : value = m_JetMu2["JetWidth"]; break;
    case RelatedInfoNamed::JETMU2NNTAG    : value = m_JetMu2["NNTag"]; break;
    case RelatedInfoNamed::JETMU2MNF    : value = m_JetMu2["MNF"]; break;
    case RelatedInfoNamed::JETBPX    : value = m_JetB["Px"]; break;
    case RelatedInfoNamed::JETBPY    : value = m_JetB["Py"]; break;
    case RelatedInfoNamed::JETBPZ    : value = m_JetB["Pz"]; break;
    case RelatedInfoNamed::JETBPT    : value = m_JetB["PT"]; break;
    case RelatedInfoNamed::JETBJETWIDTH    : value = m_JetB["JetWidth"]; break;
    case RelatedInfoNamed::JETBNNTAG    : value = m_JetB["NNTag"]; break;
    case RelatedInfoNamed::JETBMNF    : value = m_JetB["MNF"]; break;

    default: value = 0.; break;
    }

    debug() << "  Inserting key = " << key << ", value = " << value << " into map" << endreq;
    m_map.insert( std::make_pair(key,value) );
  }

}


//=============================================================================
// Fill Jets RelatedInfo structure
//=============================================================================
StatusCode RelInfoJetsVariables::calculateRelatedInfo( const LHCb::Particle *top,
                                                       const LHCb::Particle *part )
{

  debug() << "The top particle is " << top->key() << endmsg;
  debug() << "And the particle is " << part->key() << endmsg;

  m_JetNoMu1 = emptyProperties();
  m_JetNoMu2 = emptyProperties();
  m_JetNoMu3 = emptyProperties();
  m_JetMu1 = emptyProperties();
  m_JetMu2 = emptyProperties();
  m_JetB = emptyProperties();
  fillMap();
  debug()<< "Filled empty dicts" << endmsg;

  // only fill infos for top particle!
  if (part!=top) return StatusCode::SUCCESS;

  LHCb::Particle mu1;
  LHCb::Particle mu2;

  // get DV algorithm
  IDVAlgorithm* dva = Gaudi::Utils::getIDVAlgorithm( contextSvc() ) ;
  if ( !dva ) return StatusCode::FAILURE;

  // get bestPV of the top particle
  const LHCb::VertexBase* BPV = dva->bestVertex(top);

  // now find the RecVertex that is closest to the refitted one
  double dist,distc2_tmp,distc2;
  distc2 = 1e6;
  LHCb::VertexBase* closePV=NULL;
  LHCb::RecVertex::Container* verts = getIfExists<LHCb::RecVertex::Container>(LHCb::RecVertexLocation::Primary);
  // create a list of pairs ipchi2/vertex
  for ( LHCb::RecVertex::Container::const_iterator iv = verts->begin(); iv != verts->end(); iv++) {
    StatusCode sc = dva->distanceCalculator()->distance(BPV,(*iv),dist,distc2_tmp);
    if (sc.isFailure()) {
      debug()<<"Failure obtaining IPchi2"<<endmsg;
      continue;
    }
    if (distc2_tmp<distc2){
      closePV=(*iv);
      distc2=distc2_tmp;
    }
  }
  int bpvkey;
  if (closePV) bpvkey  = closePV->key();
  else bpvkey = 0;
  debug() << "The top particle is associated to PV " << bpvkey << endmsg;

  // this is for Bmunu (m_ind_particle = 13)
  if (top->particleID().pid()==m_ind_part ||top->particleID().pid()==((-1)*m_ind_part))
  {
    mu1 = (*top);
    m_onemu = true;
    m_use_jetsb = false;
  }
  // and this for Bsmumu
  else
  {
    mu1 = *(top->daughters().at(0));
    mu2 = *(top->daughters().at(1));
    m_onemu = false;
  }

  debug()<< "m_onemu="<<m_onemu<<endmsg;
  debug()<< "m_use_jetsb="<<m_use_jetsb<<endmsg;


  if ( m_use_jetsnomu )
  {
    debug() << "Jets no mu" <<endmsg;
    if ( exist<LHCb::Particles>(m_loc_jetsnomu) )
    {
      const LHCb::Particles* stdjets_nomu_0 = get<LHCb::Particles>(m_loc_jetsnomu);
      // sort according to pT and same PV as B
      std::vector<LHCb::Particle*> stdjets_nomu;
      pt_sorted_samePV(*stdjets_nomu_0,bpvkey,stdjets_nomu);
      debug() << "Looking at " << stdjets_nomu.size()<< "jets" << endmsg;
      if (stdjets_nomu.size()>0 ) m_JetNoMu1 = fillProperties(stdjets_nomu.at(0));
      if (stdjets_nomu.size()>1 ) m_JetNoMu2 = fillProperties(stdjets_nomu.at(1));
      if (stdjets_nomu.size()>2 ) m_JetNoMu3 = fillProperties(stdjets_nomu.at(2));
      debug() << "Jets info filled " << endmsg;
    }
    else debug() << "No jets found" << endmsg;
  }


  if (m_use_jetsinc)
  {
    debug() << "Jets with mu" <<endmsg;
    if ( exist<LHCb::Particles>(m_loc_jetsinc) )
    {
      const LHCb::Particles*  stdjets_inc = get<LHCb::Particles>(m_loc_jetsinc);
      LHCb::Particle myjet1;
      StatusCode sc1 = find_jet_mu(stdjets_inc, mu1, myjet1);
      if (sc1.isSuccess()){
        m_JetMu1 = fillProperties(&myjet1);
        debug()<< "Jets with mu1, top pT" << myjet1.pt()<< endmsg;
      }

      if (!m_onemu)
      {
        LHCb::Particle myjet2;
        StatusCode sc2 = find_jet_mu(stdjets_inc, mu2, myjet2);
        if (sc2.isSuccess()) {
          debug()<< "Jets with mu2, top pT" << myjet2.pt()<< endmsg;
          m_JetMu2 = fillProperties(&myjet2);
        }
      }
    }
    else debug() << "No jets found" << endmsg;
  }

  if (m_use_jetsb)
  {
    debug() << "Jets with B" <<endmsg;
    if ( exist<LHCb::Particles>(m_loc_jetsb) )
    {
      const LHCb::Particles* stdjets_forceb = get<LHCb::Particles>(m_loc_jetsb);
      LHCb::Particle myjet3;
      StatusCode sc3 = find_jet_b(stdjets_forceb, myjet3);
      if (sc3.isSuccess())
      {
        debug()<< "Jets with B, top pT" << myjet3.pt()<< endmsg;
        m_JetB = fillProperties(&myjet3);
      }
    }
    else debug() << "No jets found" << endmsg;
  }

  fillMap();
  return StatusCode::SUCCESS;
}


LHCb::RelatedInfoMap* RelInfoJetsVariables::getInfo(void){
  return &m_map;
}
