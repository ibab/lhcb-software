// Include files 
#include <sstream>
#include <math.h>

// from Gaudi
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/SystemOfUnits.h"

// from PartProp
#include "Kernel/IEvtTypeSvc.h"
#include "MCInterfaces/IMCDecayFinder.h"

// local
#include "MCEventTypeFinder.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCEventTypeFinder
//
// 20/04/2002 : Olivier Dormond
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( MCEventTypeFinder )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCEventTypeFinder::MCEventTypeFinder( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_evtTypeSvc(0),
    m_mcFinders(0),
    m_inputTypes(0),
    m_decProdCut(0),
    m_mbias(30000000),
    m_incb(10000000),
    m_incc(20000000),
    m_diMuon(12000),
    m_muonp(-13),
    m_muonm(13),
    m_tau(15),
    m_acc_min(-1.0*log(tan(0.4/2.0))), //~1.60
    m_acc_max(-1.0*log(tan(0.05/2.0))), //~6
    m_decProdStr1("DecProdCut"),
    m_decProdStr2("InAcc")
{

  declareInterface<IMCEventTypeFinder>(this);
  
  //override the total list of types taken from the decfiles to a smaller sub-class of types
  declareProperty( "Types", m_inputTypes=std::vector<long unsigned int>(0) );
  
  //define the minimum acceptance for a decprodcut,  by default this is 400 mrad, eta > 1.60-ish
  //you could set this to 2.0 if you really want "in the acceptance" 
  declareProperty( "acc_min", m_acc_min=-1.0*log(tan(0.4/2.0)));
  
  //define the maximum acceptance for a decprodcut, by default this is 5 mrad eta < 6-ish.
  //could set to 4.9 if you really want "in the acceptance" 
  declareProperty( "acc_max", m_acc_max=-1.0*log(tan(0.05/2.0)));
  
}

//=============================================================================
// Standard destructor
//=============================================================================

MCEventTypeFinder::~MCEventTypeFinder( )
{

}

//=============================================================================

StatusCode MCEventTypeFinder::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;

  if(UNLIKELY(msgLevel(MSG::DEBUG))) debug() << "==> Initializing" << endmsg;

  // Retrieve the EvtTypeSvc here so that it is always done at initialization
  m_evtTypeSvc = svc<IEvtTypeSvc>( "EvtTypeSvc", true );

  if(m_inputTypes.empty()) m_allTypes=m_evtTypeSvc->allTypes();
  else vec2set(m_inputTypes,m_allTypes);

  //m_allTypes=m_evtTypeSvc->allTypes();

  if(UNLIKELY(msgLevel(MSG::DEBUG)))
    debug() << "Number of event types to look for: m_allTypes.size() "
            << m_allTypes.size() << endmsg;
  
  return sc;
  
}

//=============================================================================
// SLOW. Find all event types in this event using the decay descriptors
//=============================================================================
StatusCode MCEventTypeFinder::findEventTypes(LHCb::EventTypeSet& found)
{ 
  if(UNLIKELY(msgLevel(MSG::DEBUG))) 
    debug() << "findEventTypes called, with no passsed container"  << endmsg;
  
  //I should get the container, and iterate myself!

  //get all MC particles,
  LHCb::MCParticles* mcparts = 
    get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default );
  if( !mcparts )
  {
    fatal() << "Unable to find MC particles at '"
            << LHCb::MCParticleLocation::Default << "'" << endmsg;
    return StatusCode::FAILURE;
  }

  //send the result to the vector version

  StatusCode sc=findDecayType(found, *mcparts);
  
  if(UNLIKELY(msgLevel(MSG::DEBUG))) 
    debug() << "findEventTypes completed, with no container, returning :" << found.size() << endmsg;
  return sc; 
}

StatusCode MCEventTypeFinder::fillMCTools()
{
  
  if(UNLIKELY(msgLevel(MSG::DEBUG))) 
    debug() << "filling all MCDecayTools"  << endmsg;
  m_mcFinders.clear();
  m_decProdCut.clear();
  m_mcFinders.reserve(m_allTypes.size());
  m_decProdCut.reserve(m_allTypes.size());
  
  std::string minbias =  m_evtTypeSvc->decayDescriptor(m_mbias);

  //if minbias doesn't exist, pick the highest one:
  if(!m_allTypes.count(m_mbias))
    {
      
      for(LHCb::EventTypeSet::const_iterator iType=m_allTypes.begin();
	  iType!=m_allTypes.end(); iType++)
	{
	  if(!(*iType)) break;
	  std::string sdecay=m_evtTypeSvc->decayDescriptor(*iType);
    
	  if(sdecay=="Unknown" || sdecay=="") continue;
	  if (strcompNoSpace(sdecay,minbias))
	    {
	      info() << "Resetting min bias to EventType " << *iType << endmsg;
	      m_mbias=*iType;
	      break;
	    }
	}
    }
  //if it still doesn't exist, just add it in, all events are min bias, generally
  if(!m_allTypes.count(m_mbias)) m_allTypes.insert(m_mbias);
  if(UNLIKELY(msgLevel(MSG::DEBUG))) 
    debug() << "Min bias is set to EventType " << m_mbias << endmsg;
  

  for(LHCb::EventTypeSet::const_iterator iType=m_allTypes.begin();
      iType!=m_allTypes.end(); iType++)
  {
    if(!(*iType)) break;
    std::string sname=m_evtTypeSvc->nickName(*iType);
    std::string sdecay=m_evtTypeSvc->decayDescriptor(*iType);
    
    if(msgLevel(MSG::VERBOSE)) verbose() << "Preparing to set decay for EvtCode " << *iType << endmsg;
    //is it a DecProdCut?
    m_decProdCut.push_back(
			   sname.find(m_decProdStr1)!=std::string::npos ||
			   sname.find(m_decProdStr2)!=std::string::npos 
			   );
    
    if(*iType == m_mbias) //skip it, every event is min bias
    {
      m_mcFinders.push_back(NULL);
      continue;
    }
    
    if(sdecay=="Unknown" || sdecay=="" || strcompNoSpace(sdecay,minbias)) //skip it
    {
      if(UNLIKELY(msgLevel(MSG::DEBUG)))
        debug() << "Ignoring: " << *iType << endmsg;
      m_mcFinders.push_back(NULL);
      continue;
    }
    
    if(*iType > m_mbias+m_incb) //ignore it, shouldn't allow decays above min bias
    {
      if(UNLIKELY(msgLevel(MSG::DEBUG))) 
        debug() << "Ignoring: " << *iType << endmsg;
      m_mcFinders.push_back(NULL);
      continue;
    }
    
    std::stringstream ss (std::stringstream::in | std::stringstream::out);
    ss << "MCD" << *iType << std::endl;
    std::string strEventCode;
    ss >> strEventCode;
    
    IMCDecayFinder* anIMCD = tool<IMCDecayFinder>( "MCDecayFinder", strEventCode, this );
    if(!anIMCD)
    {
      warning() << "could not retrieve MCDecayFinder tool " << *iType << endmsg;
      m_mcFinders.push_back(NULL);
      continue;
    }
    
    if(msgLevel(MSG::VERBOSE)) verbose() << strEventCode << " DecayFinder is looking for " << sdecay << endmsg;

    if( (anIMCD->setDecay( sdecay )).isFailure() ) 
    {
      warning() << "Unable to set decay for EvtCode " << *iType << endmsg;
      m_mcFinders.push_back(NULL);
    }
    else m_mcFinders.push_back(anIMCD);
    
  }
  if(!m_mcFinders.empty() && m_mcFinders.size()==m_decProdCut.size() && m_mcFinders.size()==m_allTypes.size()) return StatusCode::SUCCESS;
  
  warning()  << "Could not fill the decay descriptors correctly" << endmsg;
  return StatusCode::FAILURE; 
  
}


//=============================================================================
// SLOW. Find all decay types from this particle using the decay descriptors
//=============================================================================
StatusCode MCEventTypeFinder::findDecayType(LHCb::EventTypeSet& found, const LHCb::MCParticle * mc_mother ) 
{
  
  if(UNLIKELY(msgLevel(MSG::DEBUG))) 
    debug() << "findDecayType called, with MCParticle*"  << endmsg;

  if(!mc_mother) 
    {
      warning() << "passed MCParticle mother does not exist" << endmsg;
      return StatusCode::SUCCESS; 
    }
  
  LHCb::MCParticle::ConstVector dummyvec(0); //stupid way, but is the only way!
  dummyvec.push_back(mc_mother);

  if(m_mcFinders.empty()) fillMCTools();  //fill the vector of tools on the first use
  std::vector<IMCDecayFinder*>::const_iterator iD=m_mcFinders.begin();
  std::vector<bool>::const_iterator iB=m_decProdCut.begin();
  
  for(LHCb::EventTypeSet::const_iterator iType=m_allTypes.begin();
      iType!=m_allTypes.end() && iD!=m_mcFinders.end() && iB!=m_decProdCut.end();
      iType++, iD++, iB++) //lowest to highest
  {
 
    if( ! (*iD) ) continue;
    if(found.count(*iType)) continue; //don't test it if it's already there!
    if(*iB) //are all stable particles in the acceptance?
    {
      bool nJPsi=false, nmuon=false, pmuon=false, nmudec=false, pmudec=false, nelectron=false, dec=true, neutrino=false;
      int ncharm=0, ntracks=0, neut=0;
      determineDecay(mc_mother, nJPsi, ncharm, nmuon, pmuon, nmudec, pmudec, nelectron, ntracks, neut, dec, neutrino);
      if(!dec) return StatusCode::SUCCESS;
    }
    
    if( (*iD)->hasDecay(dummyvec) ) found.insert(*iType);
    appendParents(found);
  }
  
  if(found.size()>0)
  {
    if(UNLIKELY(msgLevel(MSG::DEBUG))) 
      debug() << "findDecayType completed, with an MCParticle * container, found:" << found.size()  << endmsg;
    return StatusCode::SUCCESS; 
  }
  
  warning() <<  "No Event types for this event" << endmsg;
  return StatusCode::SUCCESS; 
  
}
//=============================================================================
// SLOW. Find all decay types from this particle using the decay descriptors
//=============================================================================
StatusCode MCEventTypeFinder::findDecayType(LHCb::EventTypeSet& found, const LHCb::MCParticle::ConstVector & mc_mothers ) 
{ 
  if(UNLIKELY(msgLevel(MSG::DEBUG))) 
    debug() << "findDecayType called, with MCParticle::ConstVector"  << endmsg;
  if(mc_mothers.empty()) 
    {
      warning() << "passed empty MCParticle vector" << endmsg;
      return StatusCode::SUCCESS; 
    }

  for(LHCb::MCParticle::ConstVector::const_iterator iParticle=mc_mothers.begin(); iParticle!=mc_mothers.end(); iParticle++)
    {
      if(!(*iParticle)) continue;
      
      //in acceptance?
      if((*iParticle)->pseudoRapidity()<m_acc_min || (*iParticle)->pseudoRapidity()>m_acc_max) continue;
      
      findDecayType(found, (*iParticle) ) ;
    }
  //check for dimuon events
  
  if(dimuon(mc_mothers)) appendDiMuon(found);// it's a dimuon event
  
  if(found.size()>0)
    {
      if(UNLIKELY(msgLevel(MSG::DEBUG))) 
        debug() << "findDecayType completed, with an MCParticle::ConstVector container, found:" << found.size()  << endmsg;
      return StatusCode::SUCCESS; 
    }
  
  warning() <<  "No Event types for this event" << endmsg;
  return StatusCode::SUCCESS;  
}
//=============================================================================
// SLOW. Find all decay types from this particle using the decay descriptors
//=============================================================================
StatusCode MCEventTypeFinder::findDecayType(LHCb::EventTypeSet& found, const LHCb::MCParticles & mc_mothers ) 
{ 
  if(UNLIKELY(msgLevel(MSG::DEBUG))) 
    debug() << "findDecayType called, with MCParticles"  << endmsg;

  if(mc_mothers.empty()) 
    {
      warning() << "passed empty MCParticle vector" << endmsg;
      return StatusCode::SUCCESS; 
    }

  
  for(LHCb::MCParticles::const_iterator iParticle=mc_mothers.begin(); iParticle!=mc_mothers.end(); iParticle++)
    {
      if(!(*iParticle)) continue;
      //in acceptance?
      if((*iParticle)->pseudoRapidity()<m_acc_min || (*iParticle)->pseudoRapidity()>m_acc_max) continue;
      
      findDecayType(found, (*iParticle) ) ;
    }
  
  
  if(dimuon(mc_mothers)) appendDiMuon(found);// it's a dimuon event

  if(found.size()>0)
    {
      if(UNLIKELY(msgLevel(MSG::DEBUG))) 
        debug() << "findDecayType completed, with an MCParticles container, found:" << found.size()  << endmsg;
      return StatusCode::SUCCESS; 
    }
  
  warning() <<  "No Event types for this event" << endmsg;
  return StatusCode::SUCCESS; 
}

//=============================================================================
// Guess event types in this event based on the quarks/decays in the event
//=============================================================================
StatusCode MCEventTypeFinder::constructEventTypes(LHCb::EventTypeSet& found) 
{ 
  if(UNLIKELY(msgLevel(MSG::DEBUG))) 
    debug() << "constructEventTypes called, with no container"  << endmsg;

  //get all MC particles,
  LHCb::MCParticles* mcparts = 
    get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default );
  if( !mcparts )
  {
    fatal() << "Unable to find MC particles at '"
            << LHCb::MCParticleLocation::Default << "'" << endmsg;
    return StatusCode::FAILURE;
  }

  //send the result to the vector version

  StatusCode sc=constructDecayType(found, *mcparts);
  
  if(UNLIKELY(msgLevel(MSG::DEBUG))) 
    debug() << "constructEventTypes completed, with no container, returning :" << found.size() << endmsg;
  return sc;
  

}
  
//=============================================================================
// Guess decay types from this particle based on its decay
//=============================================================================
StatusCode MCEventTypeFinder::constructDecayType(LHCb::EventTypeSet& found, const LHCb::MCParticle * mc_mother ) 
{
  if(UNLIKELY(msgLevel(MSG::DEBUG))) 
    debug() << "constructDecayType called, with an MCParticle*"  << endmsg;

  if(!mc_mother) return StatusCode::SUCCESS;
  bool dimuon=false;
  unsigned int dec=constructDecayType(mc_mother,dimuon);
  if(dec<m_incb && !dimuon) return StatusCode::SUCCESS; 

  if(dec>m_incb) {found.insert(dec); appendParents(found);}

  if(dimuon) appendDiMuon(found);
  
  if(UNLIKELY(msgLevel(MSG::DEBUG))) 
    debug() << "constructDecayType completed, with an MCParticle *, found:" << found.size()  << endmsg;
  return StatusCode::SUCCESS; 
}
  
//=============================================================================
// Guess decay types from this particle based on its decay
//=============================================================================
StatusCode MCEventTypeFinder::constructDecayType(LHCb::EventTypeSet& found, const LHCb::MCParticle::ConstVector & mc_mothers ) 
{
  if(UNLIKELY(msgLevel(MSG::DEBUG))) 
    debug() << "constructDecayType called, with an MCParticle::ConstVector"  << endmsg;

  bool posmuon=false;
  bool negmuon=false;
  
  for(LHCb::MCParticle::ConstVector::const_iterator iParticle=mc_mothers.begin(); iParticle!=mc_mothers.end(); iParticle++)
    {
      if(!(*iParticle)) continue;
      
      //in acceptance?
      if((*iParticle)->pseudoRapidity()<m_acc_min || (*iParticle)->pseudoRapidity()>m_acc_max) continue;
      
      long int dec=constructDecayType(*iParticle);
      if(dec > int(m_incb)) found.insert(dec);
      else if(dec==m_muonp) posmuon=true;
      else if(dec==m_muonm) negmuon=true;
    }
  
  appendParents(found);
  if(posmuon && negmuon) appendDiMuon(found);// it's a dimuon event
  
  if(UNLIKELY(msgLevel(MSG::DEBUG))) 
    debug() << "constructDecayType completed, with an MCParticle::ConstVector container, found:" << found.size()  << endmsg;
  return StatusCode::SUCCESS; 
}
//=============================================================================
// Guess decay types from this particle based on its decay
//=============================================================================
StatusCode MCEventTypeFinder::constructDecayType(LHCb::EventTypeSet& found, const LHCb::MCParticles & mc_mothers ) 
{
  if(UNLIKELY(msgLevel(MSG::DEBUG))) 
    debug() << "constructDecayType called, with an MCParticles container"  << endmsg;

  bool posmuon=false;
  bool negmuon=false;
  
  for(LHCb::MCParticles::const_iterator iParticle=mc_mothers.begin(); iParticle!=mc_mothers.end(); iParticle++)
    {
      if(!(*iParticle)) continue;
      
      //in acceptance?
      if((*iParticle)->pseudoRapidity()<m_acc_min || (*iParticle)->pseudoRapidity()>m_acc_max) continue;
      
      long int dec=constructDecayType(*iParticle);
      if(dec > int(m_incb)) found.insert(dec);
      if(dec==m_muonp) posmuon=true;
      else if(dec==m_muonm) negmuon=true;
    }
  
  appendParents(found);
  if(posmuon && negmuon) appendDiMuon(found);// it's a dimuon event
  
  if(UNLIKELY(msgLevel(MSG::DEBUG))) 
    debug() << "constructDecayType completed, with an MCParticles container, found:" << found.size()  << endmsg;
  return StatusCode::SUCCESS; 
}

//=============================================================================
// Given an input code, will return the list of event types this belongs too.
// e.g. all events are covered in MinimumBias, any bb-event is covered by inc-b.
// so an input of 11154101 ->  11154100    ->   11000000    -> 10000000 -> 30000000
//        [specific decay] -> [decay type] -> [hadron mode] -> [inclusive] -> [minbias]
//=============================================================================

StatusCode MCEventTypeFinder::parentEventTypes(long unsigned int evtType, LHCb::EventTypeSet & parents)
{
  if(UNLIKELY(msgLevel(MSG::DEBUG))) 
    debug() << "looking for parents to event " << evtType << endmsg;
    
  if(evtType < 10000000) return StatusCode::FAILURE; 
    
  parents.insert(evtType); //this eventType
  parents.insert(evtType-evtType%100); //decay type
  parents.insert(evtType-evtType%1000000); //hadron mode
  parents.insert(evtType-evtType%10000000); //inclusive
  if(evtType< m_mbias+m_incb) parents.insert(m_mbias);//min bias
    
  if(UNLIKELY(msgLevel(MSG::DEBUG))) 
    debug() << "found parents " << parents.size() << " first in list " << *(parents.begin()) << endmsg;
    
  return StatusCode::SUCCESS;
    
}
StatusCode MCEventTypeFinder::appendParents(LHCb::EventTypeSet & aset)
{
  if(UNLIKELY(msgLevel(MSG::DEBUG))) 
    debug() << "appending all parents, starting size:" << aset.size() << endmsg;

    const LHCb::EventTypeSet parents=aset;
    
    if(aset.empty()) { aset.insert(m_mbias); return StatusCode::SUCCESS;}
    
    
    for(LHCb::EventTypeSet::const_iterator n=parents.begin(); n!=parents.end(); n++)
      {
        parentEventTypes(*n, aset); //append parents to aset
      }
    if(UNLIKELY(msgLevel(MSG::DEBUG))) 
      debug() << "parents appended, final size:" << aset.size() << endmsg;
    return StatusCode::SUCCESS;
    
}
bool MCEventTypeFinder::set2vec(LHCb::EventTypeSet& aset,std::vector<long unsigned int>& avec)
{
  avec.clear();
  avec.reserve(aset.size());
  for(LHCb::EventTypeSet::iterator n=aset.begin(); n!=aset.end(); n++)
    {
      avec.push_back(*n);
    }
  return (avec.size() > 0);
  
}
bool MCEventTypeFinder::vec2set(std::vector<long unsigned int>& avec, LHCb::EventTypeSet& aset)
{
  aset.clear();
  for(std::vector<long unsigned int>::iterator n=avec.begin(); n!=avec.end(); n++)
    {
      aset.insert(*n);
    }
  return (aset.size() > 0);
  
}
//=============================================================================
// Guess decay types from this particle based on its decay, no acceptance cut
//=============================================================================
long int MCEventTypeFinder::constructDecayType(const LHCb::MCParticle * mc_mother, bool & dimuon ) 
{
  if(msgLevel(MSG::VERBOSE)) verbose() << "constructing the decay type of mother located at " << mc_mother << endmsg;

  
  if(!mc_mother) return 0;

  //in acceptance? Here we should assume that the acceptance check has already been made.
  //if(mc_mother->pseudoRapidity()<m_acc_min || mc_mother->pseudoRapidity()>m_acc_max) return 0; //don't check if a particle is explicitly passed
  //amuon?
  
  const LHCb::ParticleID mID = mc_mother->particleID();
  if (mID.pid()==m_muonp) return m_muonp;
  else if (mID.pid()==m_muonm) return m_muonm;
  else if(abs(mID.pid())==m_tau) return categoriseTau(mc_mother, dimuon);
  else if(mID.isMeson() || mID.isBaryon()) return categoriseHadron(mc_mother, dimuon);
  else return 0;
  
}
long int MCEventTypeFinder::categoriseHadron(const LHCb::MCParticle * mc_mother, bool & dimuon ) 
{
  
  if(!mc_mother) return 0;
  
  const LHCb::ParticleID mID = mc_mother->particleID();

  unsigned int g=0, s=0;

  //does it have a b/c-quark? 1, 2, 
  if(mID.hasBottom()) g=1; 
  else if(mID.hasCharm()) g=2;
  else return 0;
  
  //is it excited? 7.. can't tell this one :S
  
  
  
  //meson or baryon?   1,2,3,4,8 or 5,6,
  if(mID.isMeson()) s=1;
  else if(mID.isBaryon()) s=2;
  else return 0;
  
  //charm or strange d=1, u=2, s=3, c=4, t=0, b=8, ud=5, other=6
  if(s==1)
    {
      if(mID.hasTop()) s=0;
      else if(mID.hasDown()) s=1;
      else if(mID.hasUp()) s=2;
      else if(mID.hasStrange()) s=3;
      else if(mID.hasCharm()) s=4;
      else s=8;
      
    }
  else if(s==2)
    {
      if(mID.hasUp()&&mID.hasDown()) s=5;
      else s=6;
    }
  
  int dctnxu = determineDecay(mc_mother, dimuon);

  if(msgLevel(MSG::VERBOSE)) verbose() << "found the decay type " << (g*10000000+s*1000000+dctnxu) << endmsg;
  if(dimuon && msgLevel(MSG::VERBOSE)) verbose() << "--which is a dimuon type " << endmsg;
  
  return(g*10000000+s*1000000+dctnxu);
  
  
}
long int MCEventTypeFinder::determineDecay(const LHCb::MCParticle * mc_mother, bool & dimuon)
{
  bool nJPsi=false, nmuon=false, pmuon=false, nmudec=false, pmudec=false, nelectron=false, dec=true, neutrino=false;
  int ncharm=0, ntracks=0, neut=0;
  determineDecay(mc_mother, nJPsi, ncharm, nmuon, pmuon, nmudec, pmudec, nelectron, ntracks, neut, dec, neutrino);
  
  bool anymuon=nmuon||pmuon;
  dimuon=nmudec&&pmudec; //are there two reconstructible muons?
  
  //LHCb Note 2005-034
  
  int dctnxu=0;
  
  if (ntracks>9) ntracks=9;
  dctnxu+=ntracks*1000;
  if (neut>9) neut=9;
  dctnxu+=neut*100;
  
  if(ncharm >= 2 && !anymuon && !nelectron) dctnxu+=90000; //from the LHCb note :)
  else if(ncharm >= 1 && anymuon) dctnxu+=70000;
  else if(ncharm >= 1 && nelectron) dctnxu+=80000;
  else if(ncharm >= 1) dctnxu+=60000;
  else if(nJPsi && anymuon) dctnxu+=40000;
  else if(nJPsi && nelectron) dctnxu+=50000;
  else if(nJPsi) dctnxu+=30000;
  else if(anymuon) dctnxu+=10000;
  else if(nelectron) dctnxu+=20000;

  if(dec) dctnxu+=1;
  if(neutrino) dctnxu+=500000;
  else dctnxu+=100000;
  return dctnxu;
  
}

int MCEventTypeFinder::determineDecay(const LHCb::MCParticle * mc_mother, bool & nJPsi, int & ncharm, bool & nmuon,bool & pmuon,bool & nmudec,bool & pmudec,
                                      bool & nelectron, int & ntracks, int & neut, bool & dec, bool & neutrino)
{
  if(!mc_mother) return 0;
  const LHCb::ParticleID mID = mc_mother->particleID();
  if(isStable(mc_mother))  //it's a track
    {
      if(abs(mID.pid())==22 ) return 0; //don't follow photons
      else if(abs(mID.pid())==12 || abs(mID.pid())==14 || abs(mID.pid())==16 ) neutrino=true;
      else dec=(dec && mc_mother->pseudoRapidity()>m_acc_min && mc_mother->pseudoRapidity()<m_acc_max); //check if it's not a neutrino
      
      if(mID.threeCharge()!=0) 
	{
	  ntracks++;
	  if(mID.isLepton())
	    {
	      if(mID.pid()==m_muonm)  {nmuon=true; if(dec) nmudec=true;}
	      else if (mID.pid()==m_muonp) {pmuon=true; if(dec) pmudec=true;}
	      else if(abs(mID.pid())==11)  nelectron=true;
	    }
	}
      
      else
	{
	  if(abs(mID.pid())==12 || abs(mID.pid())==14 || abs(mID.pid())==16 ) neutrino=true;
	  if(neut<8 && (abs(mID.pid())==2112 || abs(mID.pid())==130 ) ) neut+=8;//KL0
	  if(neut%8 < 4 && abs(mID.pid())==111 ) neut+=4; //pi0
	  
	}
      return 0;
      
    }
  if(!mID.hasBottom()) //add stuff for this particle
    {
      
      if(neut%2 < 1 && abs(mID.pid())==310 ) neut+=1; //Ks0
      if(mID.pid()==443)  nJPsi=true;
      else ncharm+=mID.hasCharm();
      
    }
  //iterate over decay vertices
  for(SmartRefVector<LHCb::MCVertex>::const_iterator iV=mc_mother->endVertices().begin(); 
      iV!=mc_mother->endVertices().end();  iV++) //iterate over daughters
    for(SmartRefVector<LHCb::MCParticle>::const_iterator iP=(*iV)->products().begin();
	iP!=(*iV)->products().end(); iP++)
      determineDecay(*iP, nJPsi, ncharm, nmuon, pmuon, nmudec,  pmudec, nelectron, ntracks, neut, dec, neutrino);
  
  return 0;
  
  
}
bool MCEventTypeFinder::isStable(const LHCb::MCParticle * mc_mother)
{
  if(!mc_mother) return true;
  if(mc_mother->endVertices().empty()) return true;
  unsigned int anID=abs(mc_mother->particleID().pid());
  
  if(anID == 11 || anID == 13 
     || anID == 22 || anID == 12 
     || anID == 111 ||  anID == 211 
     || anID == 130 ||  anID == 321
     || anID == 2112 ||  anID == 2212) return true;
  return false;

}


bool MCEventTypeFinder::dimuon(const LHCb::MCParticles & mc_mothers)
{
  bool pmuon=false, nmuon=false;
  
  for(LHCb::MCParticles::const_iterator iParticle=mc_mothers.begin(); iParticle!=mc_mothers.end(); iParticle++)
    {
      if(!(*iParticle)) continue;
      
      //in acceptance?
      if((*iParticle)->pseudoRapidity()<m_acc_min || (*iParticle)->pseudoRapidity()>m_acc_max) continue; //don't check if a particle is explicitly passed
      if((*iParticle)->particleID().pid()==m_muonp) pmuon=true;
      else if((*iParticle)->particleID().pid()==m_muonm) nmuon=true;
    }
  return (pmuon && nmuon);
  
}

bool MCEventTypeFinder::dimuon(const LHCb::MCParticle::ConstVector & mc_mothers)
{
  bool pmuon=false, nmuon=false;
  
  for(LHCb::MCParticle::ConstVector::const_iterator iParticle=mc_mothers.begin(); iParticle!=mc_mothers.end(); iParticle++)
    {
      if(!(*iParticle)) continue;
      
      //in acceptance?
      if((*iParticle)->pseudoRapidity()<m_acc_min || (*iParticle)->pseudoRapidity()>m_acc_max) continue; //don't check if a particle is explicitly passed
      if((*iParticle)->particleID().pid()==m_muonp) pmuon=true;
      else if((*iParticle)->particleID().pid()==m_muonm) nmuon=true;
    }
  return (pmuon && nmuon);
}
StatusCode MCEventTypeFinder::setEventTypes(const LHCb::EventTypeSet events)
{
  m_allTypes=events;
  if(m_allTypes.count(m_mbias)) warning() << "No min bias in all types, excecution may fail on certain events, unless you have another min bias" << endmsg;
  m_mcFinders.clear();
  m_decProdCut.clear();
  
  return StatusCode::SUCCESS;
}
bool MCEventTypeFinder::appendDiMuon(LHCb::EventTypeSet & found )
{
  
  if(found.count(m_incb)) found.insert(m_incb+m_diMuon);
  if(found.count(m_incc)) found.insert(m_incc+m_diMuon);
  if(found.count(m_mbias)) found.insert(m_mbias+m_diMuon);
  return true;
  
}
///are two strings the same, once all whitespace is removed
bool MCEventTypeFinder::strcompNoSpace(std::string first, std::string second)
{
  unsigned int its =0;
  while(its < first.size())
    {
      if(first[its]==' ' || first[its]=='\t') first.erase(its,1);
      else its++;
    }
  
  its=0;
  
  while(its < second.size())
    {
      if(second[its]==' ' || second[its]=='\t') second.erase(its,1);
      else its++;
    }
  return second==first;
  
}

long int MCEventTypeFinder::categoriseTau(const LHCb::MCParticle * mc_mother, bool & dimuon ) 
{
  
  unsigned int g=3, s=1, d=1;
  
  int ctnxu = determineDecay(mc_mother, dimuon);
  
  
  if(msgLevel(MSG::VERBOSE)) verbose() << "found the decay type " << (g*10000000+s*1000000+d*100000+ctnxu) << endmsg;
  if(dimuon && msgLevel(MSG::VERBOSE)) verbose() << "--which is a dimuon type " << endmsg;
  
  return(g*10000000+s*1000000+d*100000+ctnxu);
  
  
}

int MCEventTypeFinder::determineTauDecay(const LHCb::MCParticle * mc_mother, int & nV0,
                                         bool & nmuon,bool & pmuon,bool & nmudec,bool & pmudec,
                                         bool & nelectron, int & ntracks, int & neut, bool & dec, 
                                         int & nPip, int & nPim, int & nKp, int & nKm)

{
  if(!mc_mother) return 0;
  const LHCb::ParticleID mID = mc_mother->particleID();
  if(isStable(mc_mother))  //it's a track
    {
      if(abs(mID.pid())==22 ) return 0; //don't follow photons
      else dec=(dec && mc_mother->pseudoRapidity()>m_acc_min && mc_mother->pseudoRapidity()<m_acc_max); //check if it's not a neutrino
      
      if(mID.threeCharge()!=0) 
      {
        ntracks++;
        if(mID.isLepton())
        {
          if(mID.pid()==m_muonm)  {nmuon=true; if(dec) nmudec=true;}
          else if (mID.pid()==m_muonp) {pmuon=true; if(dec) pmudec=true;}
          else if(abs(mID.pid())==11)  nelectron=true;
        }
        else if(mID.isMeson())
        {
          
          if (mID.pid()==211) nPip++;
          else if (mID.pid()==-211) nPim++;
          else if (mID.pid()==321) nKp++;
          else if (mID.pid()==-321) nKm++;
        }
        
      }
      
      else
      {
        if(nV0==0) 
        {
          if(abs(mID.pid())==333) nV0=4; //Phi
          else if(abs(mID.pid())==313) nV0=5; //K*(892)0
          else if(abs(mID.pid())==113) nV0=6; //rho0
          else if(abs(mID.pid())==223) nV0=7; //omega0
        }
        
        if(neut<8 && (abs(mID.pid())==2112 || abs(mID.pid())==130 ) ) neut+=8;//KL0
        if(neut%8 < 4 && abs(mID.pid())==111 ) neut+=4; //pi0
      }
      return 0;
      
    }
  //iterate over decay vertices
  for(SmartRefVector<LHCb::MCVertex>::const_iterator iV=mc_mother->endVertices().begin(); 
      iV!=mc_mother->endVertices().end();  iV++) //iterate over daughters
    for(SmartRefVector<LHCb::MCParticle>::const_iterator iP=(*iV)->products().begin();
        iP!=(*iV)->products().end(); iP++)
      determineTauDecay(*iP, nV0, nmuon, pmuon, nmudec,  pmudec, nelectron, ntracks, neut, dec, 
                     nPip,nPim,nKp,nKm);
  
  return 0;
  
  
}

long int MCEventTypeFinder::determineTauDecay(const LHCb::MCParticle * mc_mother, bool & dimuon)
{
  bool nmuon=false, pmuon=false, nmudec=false, pmudec=false, nelectron=false, dec=true;
  int nPip=0, nPim=0, nKp=0, nKm=0;
  int nV0=0, ntracks=0, neut=0;
  determineTauDecay(mc_mother, nV0, nmuon, pmuon, nmudec, pmudec, nelectron, ntracks, 
                 neut, dec, nPip, nPim, nKp, nKm);
  
  bool anymuon=nmuon||pmuon;
  bool anyhadron=nPip||nPim||nKp||nKm;
  dimuon=nmudec&&pmudec; //are there two reconstructible muons?
  
  
  
  int ctnxu=0;
  
  //LHCb Note 2009-001
  
  if (ntracks>9) ntracks=9;
  ctnxu+=ntracks*1000;
  if (neut>9) neut=9;
  ctnxu+=neut*100;
  
  if(anymuon) ctnxu+=10000; 
  else if(nelectron) ctnxu+=20000; 

  if(!anyhadron) ctnxu+=0;
  else if(nKp && nKm) ctnxu+=40;
  else if(nPip && nPim) ctnxu+=10;
  else if( nPip && nKm ) ctnxu+=20;
  else if( nPim && nKp ) ctnxu+=30;
  else if( nPim && nKm ) ctnxu+=60;
  else if( nPim >= 2|| nPip >=2 ) ctnxu+=50;
  else if( nKm >= 2|| nKp >=2 ) ctnxu+=70;
  
  if(nV0<9 && nV0>3) ctnxu+=nV0;
  
  return ctnxu;
  
}
