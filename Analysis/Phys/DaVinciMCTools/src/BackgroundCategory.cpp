// $Id: BackgroundCategory.cpp,v 1.13 2006-01-25 15:49:11 gligorov Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "BackgroundCategory.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BackgroundCategory
//
// 2005-11-23 : Vladimir Gligorov
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<BackgroundCategory>          s_factory ;
const        IToolFactory& BackgroundCategoryFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BackgroundCategory::BackgroundCategory( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_ppSvc(0)
  , m_particleDescendants(0)
  , m_pCPPAsct(0)
  , m_pChi2PPAsct(0)
  , m_commonmother(0)
{
  declareInterface<IBackgroundCategory>(this);
  declareProperty("LowMassBackgroundCut", m_lowMassCut = 100.*MeV) ;
  declareProperty("SoftPhotonCut", m_softPhotonCut = 300.*MeV) ;
  declareProperty("UseSoftPhotonCut", m_useSoftPhotonCut = 1) ;
  declareProperty("MCmatchQualityPIDoverrideLevel", m_override = 10.); 
  //Override decision only if match quality for PID correct match is no 
  //no worse than by 1 order of magnitude in weight compared to alternatives.
  declareProperty("MCmaxWeight", m_maxweight = 1000000.);
}
//=============================================================================
// Destructor
//=============================================================================
BackgroundCategory::~BackgroundCategory() {}; 
//=============================================================================
const MCParticle* BackgroundCategory::origin(const Particle* reconstructed_mother)
{
	//const MCParticle* background_mc_mother;

  //background_mc_mother = m_pChi2PPAsct->associatedFrom(reconstructed_mother);

	int backcategory = category(reconstructed_mother);

	if (backcategory < 60) return m_commonmother; else return 0;
}
//=============================================================================
bool BackgroundCategory::isStable(int pid)
{
	if ( pid == 11 || //electron
       pid == 22 || //photon
       pid == 13 || //muon
       pid == 12 || //neutrinos
       pid == 14 ||
       pid == 16 ||
       pid == 211 || //pion (charged)
       pid == 321 || //kaon (charged)
       pid == 2212 // || //proton
       //       pid == 111 //pion (neutral)
       ) return true; else return false; 

} 
//=============================================================================
MCParticleVector BackgroundCategory::get_mc_mothers(MCParticleVector mc_particles_linked_to_decay, const Particle* reconstructed_mother)
{
	//verbose() << "About to start getting the MC-mothers" << endreq;
	MCParticleVector mc_mothers;
	MCParticleVector::iterator iP;
	const MCParticle* finalmother;
	const MCParticle* tmpmother; 
	//verbose() << "Getting MC-mothers step 1" << endreq;

	int debugs = 0;

	for (iP = mc_particles_linked_to_decay.begin(); iP != mc_particles_linked_to_decay.end(); ++iP) {

		++debugs;
		verbose() << "Getting MC-mothers - loop step 1 - loop element " << debugs << endreq;

		tmpmother = *iP;
		//verbose() << "Getting MC-mothers - loop step 2 - loop element " << debugs << endreq;
		//verbose() << "tmpmother = " << tmpmother << endreq;
		if (tmpmother) {
			do {
				//verbose() << "Getting MC-mothers - loop step 3 - loop element " << debugs << endreq;
				finalmother = tmpmother;
				//verbose() << "Getting MC-mothers - loop step 4 - loop element " << debugs << endreq;
				//verbose() << "finalmother = " << finalmother << endreq;
				tmpmother = finalmother->mother();
				//verbose() << "Getting MC-mothers - loop step 5 - loop element " << debugs << endreq;
				//verbose() << "tmpmother = " << tmpmother << endreq;
			} while (tmpmother != 0 && finalmother->particleID().abspid() != reconstructed_mother->particleID().abspid());
			mc_mothers.push_back(finalmother);
      			verbose() << "finalmother = " << finalmother << endreq;
		} else {
			verbose() << "finalmother = 0" << endreq;
			mc_mothers.push_back(0); //Ghosts have no mother!;
		}
		//verbose() << "Getting MC-mothers - loop step 6 - loop element " << debugs << endreq;
		//verbose() << "Getting MC-mothers - loop step 7 - loop element " << debugs << endreq;
	}

	return mc_mothers;
}
//=============================================================================
MCParticleVector BackgroundCategory::create_finalstatedaughterarray_for_mcmother(const MCParticle* topmother)
{
	verbose() << "Starting to create the array of final state daughters for the mc mother" << endreq;
	
	MCParticleVector finalstateproducts;
  verbose() << "Creating final state array step 1" << endreq;
	MCParticleVector tempfinalstateproducts;
  verbose() << "Creating final state array step 2" << endreq;
	SmartRefVector<MCParticle>::const_iterator iP;
  verbose() << "Creating final state array step 3" << endreq;
	SmartRefVector<MCVertex>::const_iterator iV = topmother->endVertices().begin(); 
  verbose() << "Creating final state array step 4" << endreq;

  verbose() << "Mother has " << (*iV)->products().size() << " daughters" << endreq;
	
	for (iP = (*iV)->products().begin(); iP != (*iV)->products().end(); ++iP) {

		verbose() << "Creating final state array step 5" << endreq;

		SmartRefVector<MCVertex>::const_iterator iVV = (*iP)->endVertices().begin();

		if ( (*iVV)->products().size() == 0 || isStable( (*iP)->particleID().abspid() ) ) {

			verbose() << "Creating final state array step 6" << endreq;

			finalstateproducts.push_back(*iP);

		} else {

      verbose() << "Creating final state array step 7" << endreq;
			tempfinalstateproducts = create_finalstatedaughterarray_for_mcmother(*iP);
      verbose() << "Creating final state array step 8" << endreq;
			finalstateproducts.insert(finalstateproducts.end(),tempfinalstateproducts.begin(),tempfinalstateproducts.end()) ;
      verbose() << "Creating final state array step 9" << endreq;
		}

	}

	verbose() << "Creating final state array step 10" << endreq;

	return finalstateproducts;
}
//=============================================================================
IBackgroundCategory::categories BackgroundCategory::category(const Particle* reconstructed_mother)
{
	//First of all, we use Patrick's tool to get all the particles in the decay tree
	verbose() << "About to start processing the categorisation tree" << endreq;
	ParticleVector particles_in_decay = m_particleDescendants->descendants(reconstructed_mother);
	if (particles_in_decay.size() == 0) return Undefined; 
	verbose() << "Categorising step 1" << endreq;
	//Now we have to associate each one of them to an MCParticle if possible
	MCParticleVector mc_particles_linked_to_decay = associate_particles_in_decay(particles_in_decay);  
	verbose() << "Categorising step 2" << endreq;
	//Now to create a vector with the final mothers of all these mc particles.
	MCParticleVector mc_mothers_final = get_mc_mothers(mc_particles_linked_to_decay,reconstructed_mother);
	verbose() << "Categorising step 3" << endreq;
	//First we test condition A;if it succeeds, we test conditions B->F else G->J 
	//For a list of what the conditions are, see the respective test functions
	if (condition_A(mc_mothers_final, particles_in_decay) ) {
		verbose() << "Categorising step 4" << endreq;
		// Condition A has passed, so we are in the territory of signal decays and
		// "physics" backgrounds (meaning we have reconstructed some actual decay 
		// that occured, even if not the one we were after).
		if (condition_B(mc_particles_linked_to_decay) ) {
			verbose() << "Categorising step 5" << endreq;
			if (condition_C(particles_in_decay, mc_particles_linked_to_decay) ) {
				verbose() << "Categorising step 6" << endreq;
        //				if (condition_D(mc_mothers_final, reconstructed_mother) ) {
				if (condition_D(reconstructed_mother) ) {
					verbose() << "Categorising step 7" << endreq;
					if (condition_E() ) {
						verbose() << "Categorising step 8" << endreq;
						//This is a signal decay
						return Signal;
					} else {
						verbose() << "Categorising step 9" << endreq;
						//This is a quasi-signal decay
						return QuasiSignal;
					}
				} else {
					verbose() << "Categorising step 10" << endreq;
					//This is a fully reconstructed physics background
					return FullyRecoPhysBkg;
				}
			} else {
				verbose() << "Categorising step 11" << endreq;
				//This is a reflection
				return Reflection;
			}		
		} else {
			verbose() << "Categorising step 12" << endreq;
			if (condition_C(particles_in_decay, mc_particles_linked_to_decay) && condition_F(reconstructed_mother) ) {
				verbose() << "Categorising step 13" << endreq;
				//This is a Low-mass background
				return LowMassBkg;
			} else {
				verbose() << "Categorising step 14" << endreq;
				//This is a partially reconstructed physics background
				return PartRecoPhysBkg;
			}
		}
	} else {
		verbose() << "Categorising step 15" << endreq;
		//Since condition A has failed, we are in the territory of combinatorics, pileup,
		//ghost-based and other assortead junk background.
		if ( condition_G(mc_particles_linked_to_decay,particles_in_decay) ) {
			verbose() << "Categorising step 16" << endreq;
			//This is a ghost
			return Ghost;
		} else {
			verbose() << "Categorising step 17" << endreq;
			if (!condition_H(mc_particles_linked_to_decay,particles_in_decay) ){
				verbose() << "Categorising step 18" << endreq;
				//A pileup
				return FromDifferentPV;
			} else {
				verbose() << "Categorising step 18b" << endreq;
				int numFromPV = condition_PV(mc_mothers_final,mc_particles_linked_to_decay);
				verbose() << "Categorising step 18c" << endreq;
				if ( numFromPV > 0) {
					 verbose() << "Categorising step 18d" << endreq;
					 if (numFromPV == 1) return FromPV;
					 else return AllFromSamePV;
				} else {
					verbose() << "Categorising step 19" << endreq;
					if (condition_I(mc_mothers_final) ){
						verbose() << "Categorising step 20" << endreq;
						//This is a bbar event
						return bbar;
					} else {
						verbose() << "Categorising step 22" << endreq;
						if (condition_J(mc_mothers_final) ){
							verbose() << "Categorising step 23" << endreq;
							//This is a ccbar event
							return ccbar;
						} else {
							verbose() << "Categorising step 24" << endreq;
							//This is a light flavour event
							return uds;
						}
					}
				}
			}
		}
	}
}
//=============================================================================
bool BackgroundCategory::condition_A(MCParticleVector mc_mothers_final, ParticleVector particles_in_decay) 
{
  MCParticleVector::const_iterator iP = mc_mothers_final.begin();
  ParticleVector::const_iterator iPP = particles_in_decay.begin();
  /*if ((*iP) == 0) {
    m_commonmother = 0;
    return false;
    } //One of the final state particles has no mother, so no need to go further
    const MCParticle* tempmother = *iP;*/
  bool carryon = true;
  bool motherassignedyet = false;
  const MCParticle* tempmother = NULL;

  do {

    if ( (*iPP)->endVertex() == 0) { //final state particle

      if ((*iP) == 0 ) carryon = false; //final state particle has no mother - no need to go further
      else {
        if (!motherassignedyet) {
          tempmother = *iP ;
          motherassignedyet = true;
        }
        else carryon = ( (*iP) == tempmother ) ;
      }

    }

    /*if (*iP && *iPP) {
      SmartRefVector<MCVertex>::const_iterator iVV = (*iPP)->endVertices().begin();
      if ( (*iVV)->products().size() == 0 || isStable( (*iPP)->particleID().abspid() ) ) //final state check
      carryon = (tempmother != 0) && ( (*iP) == tempmother) ;
      }*/
    ++iP; ++iPP;

  } while (carryon && iP != mc_mothers_final.end() );

  if (carryon) m_commonmother = tempmother; else m_commonmother = 0;

  return carryon;
}
//=============================================================================
bool BackgroundCategory::condition_B(MCParticleVector mc_particles_linked_to_decay)
{
	verbose() << "Beginning to check condition B" << endreq;
	bool carryon;
  	verbose() << "Checking condition B step 1" << endreq;
	MCParticleVector finalstateproducts = create_finalstatedaughterarray_for_mcmother(m_commonmother);
  	verbose() << "Checking condition B step 2" << endreq;
	MCParticleVector::const_iterator iPP = finalstateproducts.begin();
  	verbose() << "Checking condition B step 3" << endreq;
	MCParticleVector::const_iterator iP;
  	verbose() << "Checking condition B step 4" << endreq;
	
	do {
		carryon = false;
		verbose() << "Checking condition B step 5" << endreq;
		if (*iPP) {
			verbose() << "The MC final state particle has pid : " << (*iPP)->particleID().pid() << endreq;
			verbose() << "Checking condition B step 6" << endreq;
			for (iP = mc_particles_linked_to_decay.begin(); iP != mc_particles_linked_to_decay.end(); ++iP) {
				if ( *iP == 0) continue; //if no matching MCParticle
				if ( (*iP)->particleID().abspid() == 12 ) continue; //neutrinos are ignored
				if ( (*iP)->particleID().abspid() == 14 ) continue; //neutrinos are ignored
				if ( (*iP)->particleID().abspid() == 16 ) continue; //neutrinos are ignored
				if ( m_useSoftPhotonCut && (*iP)->particleID().abspid() == 22 && 
					(*iP)->momentum().e() < m_softPhotonCut ) continue; //soft photons are ignored
				verbose() << "The MC-associated particle has pid : " << (*iP)->particleID().pid() << endreq;
				verbose() << "Checking condition B step 7" << endreq;
				SmartRefVector<MCVertex>::const_iterator iVV = (*iP)->endVertices().begin();
				verbose() << "Checking condition B step 7b" << endreq;
        			if ( (*iVV)->products().size() == 0 || isStable( (*iP)->particleID().abspid() )  ) {
					verbose() << "Checking condition B step 8" << endreq;
					verbose() << "Associated Particle:" << (*iP) << endreq;
					verbose() << "MC-final state Particle:" << (*iPP) << endreq;
					if ( carryon = ( *iP == *iPP ) ) break;
				}
			}
		}

		verbose() << "Checking condition B step 8b" << endreq;

		++iPP;

		verbose() << "Checking condition B step 8c" << endreq;

	} while(carryon && iPP != finalstateproducts.end());

	verbose() << "Checking condition B step 9" << endreq;

  return carryon;
}
//=============================================================================
bool BackgroundCategory::condition_C(ParticleVector particles_in_decay, MCParticleVector mc_particles_linked_to_decay)
{
	bool carryon = true;

	MCParticleVector::const_iterator iPmc = mc_particles_linked_to_decay.begin();
	ParticleVector::const_iterator iP = particles_in_decay.begin();

	do {
		if ( *iPmc && *iP) {
			if ( (*iP)->endVertex() == 0 ) {
				carryon = ( (*iP)->particleID().pid() == (*iPmc)->particleID().pid() );
			}
		}
		++iP; ++iPmc;
	} while (carryon && iP != particles_in_decay.end() && iPmc != mc_particles_linked_to_decay.end() );

	return carryon;
}
//=============================================================================
//bool BackgroundCategory::condition_D(MCParticleVector mc_mothers_final, const Particle* reconstructed_mother){
bool BackgroundCategory::condition_D(const Particle* reconstructed_mother){
	bool carryon;

	if (m_ppSvc->findByPythiaID(m_commonmother->particleID().pid())->charge() == 0) 
		carryon = ( m_commonmother->particleID().abspid() == reconstructed_mother->particleID().abspid() );
	else
		carryon = ( m_commonmother->particleID().pid() == reconstructed_mother->particleID().pid() );

	//MCParticleVector::const_iterator iP = mc_mothers_final.begin();
	//const MCParticle* originmatch = origin(reconstructed_mother);

	//carryon = (originmatch == *iP);

	return carryon;
}
//=============================================================================
bool BackgroundCategory::condition_E()
{
  return true;
}
//=============================================================================
bool BackgroundCategory::condition_F(const Particle* candidate)
{
	bool carryon = false;
	
	if ( (m_commonmother->virtualMass() - m_lowMassCut) < 
       (m_ppSvc->findByPythiaID(candidate->particleID().pid())->mass()) ) carryon = true;

	return carryon;
}
//=============================================================================
bool BackgroundCategory::condition_G(MCParticleVector mc_particles_linked_to_decay, ParticleVector particles_in_decay)
{
	bool carryon = true;
	MCParticleVector::const_iterator iP = mc_particles_linked_to_decay.begin();
	ParticleVector::const_iterator iPP = particles_in_decay.begin();

	do {

		if ( (*iP) == 0 && !(*iPP)->endVertex() ) carryon = false;  
		++iP;
		++iPP;

	} while (carryon && iP != mc_particles_linked_to_decay.end() && iPP != particles_in_decay.end() );

	return !carryon;

}
//=============================================================================
bool BackgroundCategory::condition_H(MCParticleVector mc_particles_linked_to_decay, ParticleVector particles_in_decay)
{
	bool carryon = true;
  MCParticleVector::const_iterator iP = mc_particles_linked_to_decay.begin();
  ParticleVector::const_iterator iPP = particles_in_decay.begin();
	const Collision* tmpcollision = NULL;
	bool gotacollision = false;

  do {

    if ( !(*iPP)->endVertex() ) {

			if (*iP) {

				if (!gotacollision) {
					tmpcollision = (*iP)->collision();
					gotacollision = true;
				}
				else if ( (*iP)->collision() != tmpcollision ) carryon = false;

			}

		}
    ++iP;
    ++iPP;

  } while (carryon && iP != mc_particles_linked_to_decay.end() && iPP != particles_in_decay.end() );

	return carryon;
}
//=============================================================================
bool BackgroundCategory::condition_I(MCParticleVector mc_mothers_final)
{
	verbose() << "Beginning to check condition J" << endreq;
	bool carryon = false;
	MCParticleVector::const_iterator iP = mc_mothers_final.begin();

	verbose() << "Checking condition J step 1" << endreq;

	do {

		verbose() << "Checking condition J step 2" << endreq;

		if (*iP) 
		if ( (*iP)->particleID().hasBottom() ) carryon = true;

		verbose() << "Checking condition J step 4" << endreq;

		++iP;

	} while (!carryon && iP != mc_mothers_final.end() );

	verbose() << "Checking condition J step 5" << endreq;

	return carryon;	
}
//=============================================================================
bool BackgroundCategory::condition_J(MCParticleVector mc_mothers_final)
{
	bool carryon = false;
	MCParticleVector::const_iterator iP = mc_mothers_final.begin();

	do {

		if (*iP) 
			if ( (*iP)->particleID().hasCharm() ) carryon = true;
		++iP;

	} while (!carryon && iP != mc_mothers_final.end() );

	return carryon;
}
//=============================================================================
int BackgroundCategory::condition_PV(MCParticleVector mc_mothers_final, MCParticleVector mc_particles_linked_to_decay) 
{
	//This function evaluates whether some of the particles in the final state
	//of the candidate come from the primary vertex. Returns 0 if none, 1 if one,
	//and 99 if all.
	int howmanyfinalstate = 0; //mc_particles_linked_to_decay.size();
	int howmanyfromPV = 0;
	
	MCParticleVector::const_iterator iP = mc_mothers_final.begin();
	//MCParticleVector::const_iterator iPP = mc_particles_linked_to_decay.begin();

	for (MCParticleVector::const_iterator iPP = mc_particles_linked_to_decay.begin(); iPP != mc_particles_linked_to_decay.end(); ++iPP) {

		if (*iPP) {
			SmartRefVector<MCVertex>::const_iterator iVV = (*iPP)->endVertices().begin();
			if (*iVV) { 
				if ( (*iVV)->products().size() == 0 || isStable( (*iPP)->particleID().abspid()) ) {
					++howmanyfinalstate;
					if (*iP != *iPP) {
						++iP;
						continue;
					} else {
						++howmanyfromPV;
					}
				}
			}
		}
		++iP;
	}

	if (howmanyfromPV > 0) {
		if (howmanyfromPV == howmanyfinalstate) {
			return 99;
		} else return 1;
	} else return 0;
}
//=============================================================================
MCParticleVector BackgroundCategory::associate_particles_in_decay(ParticleVector particles_in_decay)
{
	//verbose() << "Beginning to associate descendants" << endreq; 
	MCParticleVector associated_mcparts;
	ParticleVector::iterator iP;

	//verbose() << "Associating step 1" << endreq;
	int debugs = 0;

	for (iP = particles_in_decay.begin() ; iP != particles_in_decay.end() ; ++iP){
		++debugs;
		verbose() << "Associating step 2 - loop step " << debugs << endreq;
		if ( !(*iP)->endVertex() ) {
			//verbose() << "Associating step 3a - loop step " << debugs << endreq;
			ProtoParticle* protoTemp = dynamic_cast<ProtoParticle*>((*iP)->origin());
			//verbose() << "Associating step 4a - loop step " << debugs << endreq;
			verbose() << "Protoparticle is at " << protoTemp << endreq;
			MCParticle* mcTemp = m_pCPPAsct->associatedFrom(protoTemp);
			if (!mcTemp) {
				associated_mcparts.push_back(mcTemp);
				continue;
			}
			if (mcTemp->particleID().pid() == (*iP)->particleID().pid()) {
				//verbose() << "Associating step 5a - loop step " << debugs << endreq;
                                verbose() << "MCParticle is at " << mcTemp << endreq;
                                associated_mcparts.push_back(mcTemp);
                                //verbose() << "Associating step 6a - loop step " << debugs << endreq;
			} else {
				//New commands to look for a range of particles
				MCParticle* mc_correctPID = 0;
				MCParticle* mc_bestQ = 0;
				MCParticle* mc_TempDeux = 0;
				double maximumweight = m_maxweight;
				double mc_weight;
				double mc_correctPID_weight = maximumweight;
				ProtoParticle2MCAsct::ToRange mcPartRange = 
						m_pCPPAsct->rangeFrom(protoTemp);
				ProtoParticle2MCAsct::ToIterator mcPartIt;
				for (mcPartIt = mcPartRange.begin(); mcPartIt!=mcPartRange.end(); ++mcPartIt) {
					mc_TempDeux = mcPartIt->to();
					mc_weight = mcPartIt->weight();
					if ( mc_weight < maximumweight ) {
						maximumweight = mc_weight;
						mc_bestQ = mc_TempDeux;
					}
					if (mc_TempDeux->particleID().pid() == (*iP)->particleID().pid()){
						if (mc_weight < mc_correctPID_weight) {
							mc_correctPID_weight = mc_weight;
							mc_correctPID = mc_TempDeux;
						}
					}
				}
				if (mc_correctPID) {
					if (mc_correctPID == mc_bestQ) {
						associated_mcparts.push_back(mc_bestQ);
					} else {
						if (mc_correctPID_weight/m_override < maximumweight) {
							associated_mcparts.push_back(mc_correctPID);
						} else {
							associated_mcparts.push_back(mc_bestQ);
						}
					}
				} else {
					//No match with correct PID, we just match the best quality one there is
					associated_mcparts.push_back(mc_bestQ);
				}
			}
		} else {
			//verbose() << "Associating step 3b - loop step " << debugs << endreq;
			Particle* partTemp = *iP;
			//verbose() << "Associating step 4b - loop step " << debugs << endreq;
			verbose() << "Particle is at " << partTemp << endreq;
			MCParticle* mcTemp = m_pChi2PPAsct->associatedFrom(partTemp);
			//verbose() << "Associating step 5b - loop step " << debugs << endreq;
			verbose() << "MCParticle is at " << mcTemp << endreq;
			associated_mcparts.push_back(mcTemp);
			//verbose() << "Associating step 6b - loop step " << debugs << endreq;
		}
	} 

	return associated_mcparts;
}
//=============================================================================
StatusCode BackgroundCategory::finalize(){

	StatusCode sc = GaudiTool::finalize();
	if (!sc) return sc;
	return StatusCode::SUCCESS;

}
//=============================================================================
StatusCode BackgroundCategory::initialize(){

  debug() << "Starting to initialise Background Categorisation" << endreq;

  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;

  m_ppSvc = 0;
  sc = service("ParticlePropertySvc", m_ppSvc);
  if (!sc) return sc;

  m_pCPPAsct = tool<ProtoParticle2MCAsct::IAsct>("ProtoParticle2MCAsct","ChargedPP2MCAsct",this);
  m_pChi2PPAsct = tool<Particle2MCWithChi2Asct::IAsct>("Particle2MCWithChi2Asct","Particle2MCWithChi2",this);
  m_particleDescendants = tool<IParticleDescendants>("ParticleDescendants",this);

  debug() << "Done initializing" << endreq ;
 
  return StatusCode::SUCCESS;
}
