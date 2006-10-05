// $Id: BackgroundCategory.cpp,v 1.20 2006-10-05 09:04:32 jpalac Exp $
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
using namespace Gaudi::Units;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BackgroundCategory::BackgroundCategory( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_ppSvc(0)
  , m_particleDescendants(0)
  //, m_pCPPAsct(0)
  //, m_pNPPAsct(0)
  //, m_pChi2PPAsct(0)
  , m_commonMother(0)
{
  IBackgroundCategory::m_cat[-1]   = "Undefined";
  IBackgroundCategory::m_cat[0]    = "Signal";
  IBackgroundCategory::m_cat[10]   = "QuasiSignal";
  IBackgroundCategory::m_cat[20]   = "FullyRecoPhysBkg";
  IBackgroundCategory::m_cat[30]   = "Reflection";
  IBackgroundCategory::m_cat[40]   = "PartRecoPhysBkg";
  IBackgroundCategory::m_cat[50]   = "LowMassBkg";
  IBackgroundCategory::m_cat[60]   = "Ghost";
  IBackgroundCategory::m_cat[70]   = "FromPV";
  IBackgroundCategory::m_cat[80]   = "AllFromSamePV";
  IBackgroundCategory::m_cat[100]  = "FromDifferentPV";
  IBackgroundCategory::m_cat[110]  = "bbar";
  IBackgroundCategory::m_cat[120]  = "ccbar";
  IBackgroundCategory::m_cat[130]  = "uds";
  IBackgroundCategory::m_cat[1000] = "LastGlobal";

  declareInterface<IBackgroundCategory>(this);
  declareProperty("LowMassBackgroundCut", m_lowMassCut = 100.*MeV) ;
  declareProperty("SoftPhotonCut", m_softPhotonCut = 300.*MeV) ;
  declareProperty("UseSoftPhotonCut", m_useSoftPhotonCut = 1) ;
  declareProperty("InclusiveDecay", m_inclusiveDecay = 0);
  declareProperty("MCmatchQualityPIDoverrideLevel", m_override = 0.8); 
  //Override decision only if match quality for PID correct match is no 
  //no worse than by 1 order of magnitude in weight compared to alternatives.
  declareProperty("ResonanceCut", m_rescut = 10.e-6);
  declareProperty("MCminWeight", m_minWeight = 0.);
}
//=============================================================================
// Destructor
//=============================================================================
BackgroundCategory::~BackgroundCategory() {}; 
//=============================================================================
const LHCb::MCParticle* BackgroundCategory::origin(const LHCb::Particle* reconstructed_mother)
{

	const LHCb::MCParticle* tmpmother;
	const LHCb::MCParticle* newcommonmother;

	int backcategory = category(reconstructed_mother);

	if (backcategory < 60) {
		verbose() << "Getting the mother step 1" << endreq;
		if (m_commonMother) {
			verbose() << "Getting the mother step 2" << endreq;
			if ( m_commonMother->mother() ) {

				verbose() << "Getting the mother step 3" << endreq;

				tmpmother = m_commonMother;

				verbose() << "Getting the mother step 4" << endreq;

				do {
					verbose() << "Getting the mother step 5" << endreq;
					newcommonmother = tmpmother;
					verbose() << "Getting the mother step 6" << endreq;
					tmpmother = newcommonmother->mother(); 
					verbose() << "Getting the mother step 7" << endreq;
				} while (tmpmother != 0); 
				verbose() << "Getting the mother step 8" << endreq;

				return newcommonmother;
			} else	return m_commonMother; 
		} else return 0;
	} else return 0;
}
//=============================================================================
bool BackgroundCategory::isStable(int pid)
//A cheap and cheerful list of particles considered stable for the purposes of 
//this tool. Not the cleverest way to do this, but works OK.
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
MCParticleVector BackgroundCategory::get_mc_mothers(MCParticleVector mc_particles_linked_to_decay, const LHCb::Particle* reconstructed_mother)
//This function is responsible for getting the original mother of each MCParticle associated to a final state product of
//our candidate particle. If there is no such mother, a 0 is entered, and the same is done if there is no matching 
//MCParticle in the first place (for ghosts)  
{
	//verbose() << "About to start getting the MC-mothers" << endreq;
	MCParticleVector mc_mothers;
	MCParticleVector::iterator iP;
	const LHCb::MCParticle* finalmother;
	const LHCb::MCParticle* tmpmother; 
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
MCParticleVector BackgroundCategory::create_finalstatedaughterarray_for_mcmother(const LHCb::MCParticle* topmother)
//Uses Patrick's tool (ParticleDescendants) to create an array of the final state products of the MCParticle
//determined to be the joint mother (if indeeed there is one) of the MCParticles associated to the final
//state particle daughters of the candidate Particle. For obvious reasons, this function is only invoked for
//background catgegories 0->50.
{
	verbose() << "Starting to create the array of final state daughters for the mc mother" << endreq;
	
	MCParticleVector finalstateproducts;
  //verbose() << "Creating final state array step 1" << endreq;
	MCParticleVector tempfinalstateproducts;
  //verbose() << "Creating final state array step 2" << endreq;
	SmartRefVector<LHCb::MCParticle>::const_iterator iP;
  //verbose() << "Creating final state array step 3" << endreq;
	SmartRefVector<LHCb::MCVertex>::const_iterator iV = topmother->endVertices().begin(); 
  //verbose() << "Creating final state array step 4" << endreq;

  verbose() << "Mother has " << (*iV)->products().size() << " daughters" << endreq;
	
	for (iP = (*iV)->products().begin(); iP != (*iV)->products().end(); ++iP) {

		//verbose() << "Creating final state array step 5" << endreq;

		SmartRefVector<LHCb::MCVertex>::const_iterator iVV = (*iP)->endVertices().begin();

		if ( (*iVV)->products().size() == 0 || isStable( (*iP)->particleID().abspid() ) ) {

			//verbose() << "Creating final state array step 6" << endreq;

			finalstateproducts.push_back(*iP);

		} else {

      //verbose() << "Creating final state array step 7" << endreq;
			tempfinalstateproducts = create_finalstatedaughterarray_for_mcmother(*iP);
      //verbose() << "Creating final state array step 8" << endreq;
			finalstateproducts.insert(finalstateproducts.end(),tempfinalstateproducts.begin(),tempfinalstateproducts.end()) ;
      //verbose() << "Creating final state array step 9" << endreq;
		}

	}

	//verbose() << "Creating final state array step 10" << endreq;

	return finalstateproducts;
}
//=============================================================================
IBackgroundCategory::categories BackgroundCategory::category(const LHCb::Particle* reconstructed_mother)
//The big "what is it?" switch. hopefully commented enough downstairs. For additional help with what 
//all the categories mean, please visit IBackgroundCategory.h 
{
	m_commonMother = 0;
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
			
			if (m_inclusiveDecay == 1) {
        	                if (condition_C(particles_in_decay, mc_particles_linked_to_decay) && condition_D(reconstructed_mother)) {
                	                return Signal;
                                }
                        }

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
//This condition checks that all the MCParticles associated to the final-state daughters of our
//candidate Particle have a common MCParticle mother.   
{
  MCParticleVector::const_iterator iP = mc_mothers_final.begin();
  ParticleVector::const_iterator iPP = particles_in_decay.begin();
  bool carryon = true;
  bool motherassignedyet = false;
  const LHCb::MCParticle* tempmother = NULL;

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

    ++iP; ++iPP;

  } while (carryon && iP != mc_mothers_final.end() );

  if (carryon) m_commonMother = tempmother; else m_commonMother = 0;

  return carryon;
}
//=============================================================================
bool BackgroundCategory::condition_B(MCParticleVector mc_particles_linked_to_decay)
//This condition checks that all the final state daughters of the MCparticle
//returned by condition A match up to the MCParticles associated to the  final state 
//daughters of the candidate Particle. In effect, condition A checked whether all
//the particles used to make our B came from on decay, and this checks if there 
//are any particles coming from said decay which we missed out in our reconstruction. 
{
	//verbose() << "Beginning to check condition B" << endreq;
	bool carryon;
  	//verbose() << "Checking condition B step 1" << endreq;
	MCParticleVector finalstateproducts = create_finalstatedaughterarray_for_mcmother(m_commonMother);
  	//verbose() << "Checking condition B step 2" << endreq;
	MCParticleVector::const_iterator iPP = finalstateproducts.begin();
  	//verbose() << "Checking condition B step 3" << endreq;
	MCParticleVector::const_iterator iP;
  	//verbose() << "Checking condition B step 4" << endreq;
	
	do {
		carryon = false;
		//verbose() << "Checking condition B step 5" << endreq;
		if (*iPP) {

			if ( (*iPP)->particleID().abspid() == 12  || //neutrinos are ignored
                             (*iPP)->particleID().abspid() == 14  || //neutrinos are ignored
                             (*iPP)->particleID().abspid() == 16  || //neutrinos are ignored
                             ( m_useSoftPhotonCut && (*iPP)->particleID().abspid() == 22 &&
                               (*iPP)->momentum().e() < m_softPhotonCut ) ) {  //soft photons are ignored
			
				carryon = true;

			}	

			verbose() << "The MC final state particle has pid : " << (*iPP)->particleID().pid() << endreq;
			//verbose() << "Checking condition B step 6" << endreq;
			if (!carryon) { //if the particle is not to be ignored
				for (iP = mc_particles_linked_to_decay.begin(); iP != mc_particles_linked_to_decay.end(); ++iP) {
					if ( *iP == 0) continue; //if no matching MCParticle
					if ( (*iP)->particleID().abspid() == 12 ) continue; //neutrinos are ignored
					if ( (*iP)->particleID().abspid() == 14 ) continue; //neutrinos are ignored
					if ( (*iP)->particleID().abspid() == 16 ) continue; //neutrinos are ignored
					if ( m_useSoftPhotonCut && (*iP)->particleID().abspid() == 22 && 
						(*iP)->momentum().e() < m_softPhotonCut ) continue; //soft photons are ignored
					verbose() << "The MC-associated particle has pid : " << (*iP)->particleID().pid() << endreq;
					//verbose() << "Checking condition B step 7" << endreq;
					SmartRefVector<LHCb::MCVertex>::const_iterator iVV = (*iP)->endVertices().begin();
					//verbose() << "Checking condition B step 7b" << endreq;
	        			if ( (*iVV)->products().size() == 0 || isStable( (*iP)->particleID().abspid() )  ) {
						//verbose() << "Checking condition B step 8" << endreq;
						verbose() << "Associated Particle:" << (*iP) << endreq;
						verbose() << "MC-final state Particle:" << (*iPP) << endreq;
						if ( carryon = ( *iP == *iPP ) ) break;
					}
				}
			}
		}

		//verbose() << "Checking condition B step 8b" << endreq;

		++iPP;

		//verbose() << "Checking condition B step 8c" << endreq;

	} while(carryon && iPP != finalstateproducts.end());

	//verbose() << "Checking condition B step 9" << endreq;

  return carryon;
}
//=============================================================================
bool BackgroundCategory::condition_C(ParticleVector particles_in_decay, MCParticleVector mc_particles_linked_to_decay)
//This condition checks if all the final state particles used to make the candidate particle are correctly
//identified (according to PID).
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
bool BackgroundCategory::condition_D(const LHCb::Particle* reconstructed_mother)
//Checks whether the head of our decay chain has the "correct" PID.
{
	bool carryon;

	if (m_ppSvc->findByPythiaID(m_commonMother->particleID().pid())->charge() == 0) 
		carryon = ( m_commonMother->particleID().abspid() == reconstructed_mother->particleID().abspid() );
	else
		carryon = ( m_commonMother->particleID().pid() == reconstructed_mother->particleID().pid() );

	return carryon;
}
//=============================================================================
bool BackgroundCategory::condition_E()
//Not implemented but meant to check whether we missed any resonances
//in the decay chain. 
{
  return true;
}
//=============================================================================
bool BackgroundCategory::condition_F(const LHCb::Particle* candidate)
//This condtion checks whether the MCParticle head of the decay chain is at most 
//a given mass (set by the property LowMassBackgroundCut) heavier than the average mass 
//for a particle of its PID.  
{
	bool carryon = false;
	
	if ( (m_commonMother->virtualMass() - m_lowMassCut) < 
       (m_ppSvc->findByPythiaID(candidate->particleID().pid())->mass()) ) carryon = true;

	return carryon;
}
//=============================================================================
bool BackgroundCategory::condition_G(MCParticleVector mc_particles_linked_to_decay, ParticleVector particles_in_decay)
//Checks if there are any ghosts in our final state Particles used to make the candidate Particle
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
//Checks if the event is a pileup (final state Particles come from at least 2 different collisions) or not.
{
	bool carryon = true;
	MCParticleVector::const_iterator iP = mc_particles_linked_to_decay.begin();
	ParticleVector::const_iterator iPP = particles_in_decay.begin();
	//const Collision* tmpcollision = NULL;
	const LHCb::MCVertex* tmpcollision = NULL;
	bool gotacollision = false;

	do {

		if ( !(*iPP)->endVertex() ) {

			if (*iP) {

				if (!gotacollision) {
					tmpcollision = (*iP)->primaryVertex();
					gotacollision = true;
				}
				else if ( (*iP)->primaryVertex() != tmpcollision ) carryon = false;

			}

		}
		++iP;
		++iPP;

	} while (carryon && iP != mc_particles_linked_to_decay.end() && iPP != particles_in_decay.end() );

	return carryon;
}
//=============================================================================
bool BackgroundCategory::condition_I(MCParticleVector mc_mothers_final)
//Checks if at least one of the final state MCParticles associated
//to the decay products of the candidate Particle had a mother
//with bottom content.  
{
	//verbose() << "Beginning to check condition J" << endreq;
	bool carryon = false;
	MCParticleVector::const_iterator iP = mc_mothers_final.begin();

	//verbose() << "Checking condition J step 1" << endreq;

	do {

		//verbose() << "Checking condition J step 2" << endreq;

		if (*iP) 
		if ( (*iP)->particleID().hasBottom() ) carryon = true;

		//verbose() << "Checking condition J step 4" << endreq;

		++iP;

	} while (!carryon && iP != mc_mothers_final.end() );

	//verbose() << "Checking condition J step 5" << endreq;

	return carryon;	
}
//=============================================================================
bool BackgroundCategory::condition_J(MCParticleVector mc_mothers_final)
//Checks if at least one of the final state MCParticles associated
//to the decay products of the candidate Particle had a mother
//with charm content.
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
	int howmanyfinalstate = 0; 
	int howmanyfromPV = 0;
	
	//MCParticleVector::const_iterator iP = mc_mothers_final.begin();

	for (MCParticleVector::const_iterator iPP = mc_particles_linked_to_decay.begin(); iPP != mc_particles_linked_to_decay.end(); ++iPP) {

		if (*iPP) {
			SmartRefVector<LHCb::MCVertex>::const_iterator iVV = (*iPP)->endVertices().begin();
			if (*iVV) { 
				if ( (*iVV)->products().size() == 0 || isStable( (*iPP)->particleID().abspid()) ) {
					++howmanyfinalstate;
					if ((*iPP)->originVertex()->isPrimary()) ++howmanyfromPV;
					/*if (*iP != *iPP) {
						bool fromshortlivedmother = true;
						const LHCb::MCParticle* tempdaughter = *iPP;
						do {

							const LHCb::MCParticle* tempmother = tempdaughter->mother();
							if (tempmother) {

								SmartRefVector<LHCb::MCVertex>::const_iterator iVT = tempmother->endVertices().begin();
								double motherflighttime = (*iVT)->timeOfFlight();

								if (motherflighttime > m_rescut) {

									fromshortlivedmother = false;
								}

							}

							tempdaughter = tempmother;

						} while (fromshortlivedmother && tempdaughter);
						if (fromshortlivedmother) ++howmanyfromPV;
						++iP;
						continue;
					} else {
						++howmanyfromPV;
					}*/
				}
			}
		}
		//++iP;
	}

	if (howmanyfromPV > 0) {
		if (howmanyfromPV == howmanyfinalstate) {
			return 99;
		} else return 1;
	} else return 0;
}
//=============================================================================
MCParticleVector BackgroundCategory::associate_particles_in_decay(ParticleVector particles_in_decay)
//Associates all the final state particles daughters of the candidate Particle. For composites, 
//attempts to use the Chi2 associator, which has to be configured properly for this (utterly
//non-essential) feature to work.
{
	//verbose() << "Beginning to associate descendants" << endreq; 
	MCParticleVector associated_mcparts;
	ParticleVector::iterator iP;

	//verbose() << "Associating step 1" << endreq;
	int debugs = 0;

	for (iP = particles_in_decay.begin() ; iP != particles_in_decay.end() ; ++iP){
		++debugs;
		//verbose() << "Associating step 2 - loop step " << debugs << endreq;
		if ( !(*iP)->endVertex() ) {
			//verbose() << "Associating step 3a - loop step " << debugs << endreq;
			const LHCb::ProtoParticle* protoTemp = (*iP)->proto();//dynamic_cast<ProtoParticle*>((*iP)->origin());
			//verbose() << "Associating step 4a - loop step " << debugs << endreq;
			verbose() << "Protoparticle is at " << protoTemp << endreq;
			const LHCb::MCParticle* mcTemp;
			if ( (*iP)->particleID().pid() == 22 || (*iP)->particleID().pid() == 111) {
				mcTemp = m_pNPPAsct->firstMCP(protoTemp);//associatedFrom(protoTemp);
                                /*verbose() << "Neutral Particle has PID = " << (*iP)->particleID().pid() << endreq;
                                verbose() << "N energy = " << (*iP)->momentum().e() << endreq;
                                verbose() << "N pt = " << (*iP)->momentum().vect().perp() << endreq;
                                verbose() << "N momentum = " << (*iP)->momentum().vect().mag() << endreq;
				verbose() << "====================================================================" << endreq;
				verbose() << "Neutral MCParticle is at " << mcTemp << endreq;
				verbose() << "Neutral MCParticle has PID = " << mcTemp->particleID().pid() << endreq;
				verbose() << "MCN energy = " << mcTemp->momentum().e() << endreq;
				verbose() << "MCN pt = " << mcTemp->momentum().vect().perp() << endreq;
				verbose() << "MCN momentum = " << mcTemp->momentum().vect().mag() << endreq; */
			} else {
				mcTemp = m_pCPPAsct->firstMCP(protoTemp);//associatedFrom(protoTemp);
				verbose() << "MCParticle is at " << mcTemp << endreq;
			}
			if (!mcTemp) {
				associated_mcparts.push_back(mcTemp);
				continue;
			}
			if (mcTemp->particleID().pid() == (*iP)->particleID().pid()) {
				//verbose() << "Associating step 5a - loop step " << debugs << endreq;
                                //verbose() << "MCParticle is at " << mcTemp << endreq;
                                associated_mcparts.push_back(mcTemp);
                                //verbose() << "Associating step 6a - loop step " << debugs << endreq;
			} /*else {
				//New commands to look for a range of particles
				LHCb::MCParticle* mc_correctPID = 0;
				LHCb::MCParticle* mc_bestQ = 0;
				LHCb::MCParticle* mc_TempDeux = 0;
				double minimumweight = m_minWeight;
				double mc_weight;
				double mc_correctPID_weight = minimumweight;

				ProtoParticle2MCAsct::ToRange mcPartRange;

				if ( (*iP)->particleID().pid() == 22 || (*iP)->particleID().pid() == 111) {
					mcPartRange = m_pNPPAsct->rangeFrom(protoTemp);
                        	} else {
					mcPartRange = m_pCPPAsct->rangeFrom(protoTemp);
                        	}

				ProtoParticle2MCAsct::ToIterator mcPartIt;

				for (mcPartIt = mcPartRange.begin(); mcPartIt!=mcPartRange.end(); ++mcPartIt) {
					mc_TempDeux = mcPartIt->to();
					verbose() << "Range MCParticle is at " << mc_TempDeux << endreq;
	                                verbose() << "Range MCParticle has PID = " << mc_TempDeux->particleID().pid() << endreq;
        	                        verbose() << "Range energy = " << mc_TempDeux->momentum().e() << endreq;
                	                verbose() << "Range pt = " << mc_TempDeux->momentum().vect().perp() << endreq;
                        	        verbose() << "Range momentum = " << mc_TempDeux->momentum().vect().mag() << endreq;
					verbose() << "Range weight = " << mcPartIt->weight() << endreq; 
					mc_weight = mcPartIt->weight();
					if ( mc_weight > minimumweight ) {
						minimumweight = mc_weight;
						mc_bestQ = mc_TempDeux;
					}
					if (mc_TempDeux->particleID().pid() == (*iP)->particleID().pid()){
						if (mc_weight > mc_correctPID_weight) {
							mc_correctPID_weight = mc_weight;
							mc_correctPID = mc_TempDeux;
						}
					}
				}
				if (mc_correctPID) {
					if (mc_correctPID == mc_bestQ) {
						verbose() << "Pushing back best match" << endreq;
                                                verbose() << "Best PID weight = " << minimumweight << endreq;
						associated_mcparts.push_back(mc_bestQ);
					} else {
						if (mc_correctPID_weight > m_override) {
							verbose() << "Pushing back best pid match" << endreq;
							verbose() << "Best PID weight = " << mc_correctPID_weight << endreq;
							associated_mcparts.push_back(mc_correctPID);
						} else {
							verbose() << "OVERRIDE! Pushing back best Q match" << endreq;
							verbose() << "Best Q match has PID = " << mc_bestQ->particleID().pid() << endreq;
							verbose() << "Best PID weight = " << mc_correctPID_weight << endreq;
							verbose() << "Best Q = " << minimumweight << endreq;
							associated_mcparts.push_back(mc_bestQ);
						}
					}
				} else {
					//No match with correct PID, we just match the best quality one there is
					verbose() << "Pushing back best Q match" << endreq;
                                        verbose() << "Best Q match has PID = " << mc_bestQ->particleID().pid() << endreq;
                                        verbose() << "Best Q = " << minimumweight << endreq;
					associated_mcparts.push_back(mc_bestQ);
				}
			}*/
		} else {
			//verbose() << "Associating step 3b - loop step " << debugs << endreq;
			const LHCb::Particle* partTemp = *iP;
			//verbose() << "Associating step 4b - loop step " << debugs << endreq;
			verbose() << "Particle is at " << partTemp << endreq;
			const LHCb::MCParticle* mcTemp = m_pChi2PPAsct->firstMCP(partTemp);//associatedFrom(partTemp);
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

	if( NULL != m_pCPPAsct ) delete m_pCPPAsct;
	if( NULL != m_pNPPAsct ) delete m_pNPPAsct;
	if( NULL != m_pChi2PPAsct ) delete m_pChi2PPAsct;

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

  m_pCPPAsct = new Particle2MCLinker( this, Particle2MCMethod::ChargedPP, "");
  m_pNPPAsct = new Particle2MCLinker( this, Particle2MCMethod::NeutralPP, "");
  m_pChi2PPAsct = new Particle2MCLinker( this, Particle2MCMethod::Composite, "");

  //m_pCPPAsct = new Particle2MCLinker( this, "ChargedPP2MC", "/MyChargedPP2MC", "");
  //m_pNPPAsct = new Particle2MCLinker( this, "NeutralPP2MC", "/MyNeutralPP2MC", "");
  //m_pChi2PPAsct = new Particle2MCLinker( this, "CompositeParticle2MCLinks", "/MyCompositeParticle2MCLinks","");

  //m_pCPPAsct = tool<ProtoParticle2MCAsct::IAsct>("ProtoParticle2MCAsct","ChargedPP2MCAsct",this);
  //m_pNPPAsct = tool<ProtoParticle2MCAsct::IAsct>("ProtoParticle2MCAsct","NeutralPP2MCAsct",this);
  //m_pChi2PPAsct = tool<Particle2MCWithChi2Asct::IAsct>("Particle2MCWithChi2Asct","Particle2MCWithChi2",this);
  m_particleDescendants = tool<IParticleDescendants>("ParticleDescendants",this);

  debug() << "Done initializing" << endreq ;
 
  return StatusCode::SUCCESS;
}
