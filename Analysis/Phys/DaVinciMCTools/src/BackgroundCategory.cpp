// $Id: BackgroundCategory.cpp,v 1.39 2008-06-23 18:09:25 gligorov Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "BackgroundCategory.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BackgroundCategory
//
// 2005-11-23 : Vladimir Gligorov
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( BackgroundCategory );

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
  declareProperty("SemileptonicDecay", m_semileptonicDecay = 0);
  declareProperty("NumNeutrinos", m_numNeutrinos = 0);
  declareProperty("MCmatchQualityPIDoverrideLevel", m_override = 0.7); 
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
const DaughterAndPartnerVector BackgroundCategory::getDaughtersAndPartners( const LHCb::Particle* reconstructed_mother)
{
  if ( NULL==reconstructed_mother){
    Exception("Got NULL pointer").ignore();
  }

  DaughterAndPartnerVector::const_iterator iP;

  int backcategory = category(reconstructed_mother);

  verbose() << "Event is category " << backcategory << endmsg;
  for (iP = m_daughtersAndPartners.begin(); iP != m_daughtersAndPartners.end(); ++iP){

    verbose() << "Reconstructed particle has PID" << ((*iP).first)->particleID().pid() << endmsg;
    if ((*iP).second != NULL) verbose() << "Associated particle has PID" << ((*iP).second)->particleID().pid() << endmsg;
    else  verbose() << "There is no associated particle" << endmsg;

  }

  return m_daughtersAndPartners;
}
//=============================================================================
const LHCb::MCParticle* BackgroundCategory::origin(const LHCb::Particle* reconstructed_mother)
{
  if ( NULL==reconstructed_mother){
    Exception("Got NULL pointer").ignore();
  }
  
  int backcategory = category(reconstructed_mother);

  if (backcategory < 60) {
    verbose() << "Getting the common mother" << endmsg;
    if (m_commonMother) return m_commonMother;
    else return 0;
  } else return 0;
}
//=============================================================================
bool BackgroundCategory::isStable(int pid)
  //A cheap and cheerful list of particles considered stable for the purposes of 
  //this tool. Not the cleverest way to do this, but works OK.
{
  if ( pid < 0 ) pid *= -1;

  if ( pid == 11 || //electron
       pid == 22 || //photon
       pid == 13 || //muon
       pid == 12 || //neutrinos
       pid == 14 ||
       pid == 16 ||
       pid == 211 || //pion (charged)
       pid == 321 || //kaon (charged)
       pid == 2212 //|| //proton
       //pid == 111 //pion (neutral)
       ) return true; else return false; 

} 
//=============================================================================
MCParticleVector BackgroundCategory::get_mc_mothers(MCParticleVector mc_particles_linked_to_decay)
  //This function is responsible for getting the original mother of each
  //MCParticle associated to a final state product of
  //our candidate particle. If there is no such mother,
  //a 0 is entered, and the same is done if there is no matching 
  //MCParticle in the first place (for ghosts)  
{
  MCParticleVector mc_mothers;
  MCParticleVector::iterator iP;

  verbose() << "Starting to find the MC mothers of the associated particles" << endmsg;

  for (iP = mc_particles_linked_to_decay.begin(); iP != mc_particles_linked_to_decay.end(); ++iP) {
    verbose() << "Finding the final mother of " << *iP << endmsg;
    mc_mothers.push_back(get_top_mother_of_MCParticle(*iP));
  }
  
  return mc_mothers;
}
//=============================================================================
int BackgroundCategory::topologycheck(const LHCb::MCParticle* topmother)
{

  int sumofpids = 0;

  SmartRefVector<LHCb::MCParticle>::const_iterator iP;
  SmartRefVector<LHCb::MCVertex>::const_iterator iV = topmother->endVertices().begin();
  verbose() << "Printing out decay vertices of a " << topmother->particleID().abspid() << endmsg;
  verbose() << topmother->endVertices() << endmsg;

  for (iP = (*iV)->products().begin(); iP != (*iV)->products().end(); ++iP) {

    SmartRefVector<LHCb::MCVertex>::const_iterator iVV = (*iP)->endVertices().begin();
    verbose() << "Printing out decay vertices of a " << (*iP)->particleID().abspid() << endmsg;
    verbose() << (*iP)->endVertices() << endmsg;
    
    if (  (*iVV)->products().empty() ||
          isStable( (*iP)->particleID().abspid() ) ||
          (*iP)->particleID().abspid() == 111

          ) {

      if(((*iP)->particleID().abspid() != 22) &&
         ((*iP)->particleID().abspid() != 12) &&
         ((*iP)->particleID().abspid() != 14) &&
         ((*iP)->particleID().abspid() != 16) 
	) sumofpids += (*iP)->particleID().abspid();

      verbose() << "MC sum having added a neutral is now " << sumofpids << endmsg;

    } else {

      sumofpids += (*iP)->particleID().abspid();      
      sumofpids += topologycheck(*iP);

      verbose() << "MC sum having added a charged is now " << sumofpids << endmsg;
    }

  }

  verbose() << "Final MC sum is " << sumofpids << endmsg;

  return sumofpids;
}
//=============================================================================
int BackgroundCategory::topologycheck(const LHCb::Particle* topmother)
{

  int sumofpids = 0;

  ParticleVector particles_in_decay = m_particleDescendants->descendants(topmother);
  ParticleVector::const_iterator iP = particles_in_decay.begin();

  verbose() << "Mother is a " << topmother->particleID().pid() << endmsg;

  for (iP = particles_in_decay.begin(); iP != particles_in_decay.end(); ++iP) {

    verbose() << "Daughter is a " << (*iP)->particleID().pid() << endmsg;

    if ((*iP) != 0) {

      if((*iP)->particleID().abspid() != 22) sumofpids += (*iP)->particleID().abspid();
      verbose() << "Sum is now " << sumofpids << endmsg;

    }

  }

  verbose() << "Final sum is " << sumofpids << endmsg;

  return sumofpids;
}
//=============================================================================
MCParticleVector BackgroundCategory::create_finalstatedaughterarray_for_mcmother(const LHCb::MCParticle* topmother)
  //Uses Patrick's tool (ParticleDescendants) to create an array of the final state products of the MCParticle
  //determined to be the joint mother (if indeeed there is one) of the MCParticles associated to the final
  //state particle daughters of the candidate Particle. For obvious reasons, this function is only invoked for
  //background catgegories 0->50.
{
  verbose() << "Starting to create the array of final state daughters for the mc mother" << endmsg;
  
  MCParticleVector finalstateproducts;
  MCParticleVector tempfinalstateproducts;
  SmartRefVector<LHCb::MCParticle>::const_iterator iP;
  SmartRefVector<LHCb::MCVertex>::const_iterator iV = topmother->endVertices().begin(); 

  verbose() << "Mother " << topmother->particleID().pid() 
            << " has " <<  (*iV)->products().size() << " daughters" << endmsg;
  
  for (iP = (*iV)->products().begin(); iP != (*iV)->products().end(); ++iP) {

    SmartRefVector<LHCb::MCVertex>::const_iterator iVV = (*iP)->endVertices().begin();

    verbose() << (*iP)->particleID().abspid() 
              << " stable: " << isStable( (*iP)->particleID().abspid() )
              << " products: " << (*iVV)->products().size() << endmsg ;

    if ( (*iVV)->products().empty() || 
         isStable( (*iP)->particleID().abspid() ) 
       ) {
    //if (isStable( (*iP)->particleID().abspid() )) {

      //If it is a stable, add it to the final state array
      finalstateproducts.push_back(*iP);

    } else {

      //Otherwise recurse
      tempfinalstateproducts = create_finalstatedaughterarray_for_mcmother(*iP);
      finalstateproducts.insert(finalstateproducts.end(),
                                tempfinalstateproducts.begin(),
                                tempfinalstateproducts.end()) ;
    }

  }

  verbose() << "returning " 
            << finalstateproducts.size() 
            << " final state products" 
            << endmsg ;

  return finalstateproducts;
}
//=============================================================================
const LHCb::MCParticle* BackgroundCategory::get_top_mother_of_MCParticle(const LHCb::MCParticle* candidate)
//Gets the 'final' non-zero mother of an MCParticle as we go up the decay tree
{

  const LHCb::MCParticle* finalmother;
  const LHCb::MCParticle* tmpmother = candidate;

  //No associated particle so no mother
  if (!candidate) return 0;

  verbose() << "About to find the final mother of a " << candidate << endmsg;

  do {
 
    //go through all mothers until you find the final one
    finalmother = tmpmother;
    tmpmother = finalmother->mother();

  } while (tmpmother != 0 );
  
  return finalmother;

}
//=============================================================================
const LHCb::MCParticle* BackgroundCategory::get_lowest_common_mother(MCParticleVector mc_particles_to_compare)
//Gets the lowest common mother for an array of MCParticles
//The method is, take the first mother of the first particle, check if it is common to all
//then take the second mother of the first particle, check... etc.
{

  verbose() << "Starting search for lowest common mother (single)" << endmsg;

  bool carryon; 
  verbose() << "Step 1" << endmsg;
  MCParticleVector::const_iterator iMCP = mc_particles_to_compare.begin();
  verbose() << "Step 2" << endmsg;
  const LHCb::MCParticle* tempmother = (*iMCP);
  verbose() << "The tempmother has been set to " << tempmother << " with PID " << tempmother->particleID().pid() << endmsg;
  verbose() << "Step 3" << endmsg;
  const LHCb::MCParticle* tempmother2;
  verbose() << "Step 4" << endmsg;

  do {

    verbose() << "Step 5" << endmsg;

    if (tempmother) tempmother = tempmother->mother();
    else return 0; //shouldn't happen!

    if (!tempmother) return 0; //shouldn't happen!

    verbose() << "The current candidate for a common mother is " << tempmother << " and has PID " << tempmother->particleID().pid() << endmsg;
 
    verbose() << "Step 7" << endmsg;

    for (iMCP = mc_particles_to_compare.begin() + 1;
         iMCP != mc_particles_to_compare.end();
         ++iMCP) {
      
      verbose() << "Step 8" << endmsg;
      carryon = true;
      verbose() << "Step 9" << endmsg;
      if (!(*iMCP)) return 0; //shouldn't happen!
      verbose() << "Looping on MCParticle " << (*iMCP) << " and PID " << (*iMCP)->particleID().pid() << endmsg;
      verbose() << "Step 10" << endmsg;

      tempmother2 = (*iMCP)->mother();
      verbose() << "Step 11" << endmsg;
      if (!tempmother2) return 0; //shouldn't happen!
      verbose() << "Step 12" << endmsg;

      verbose() << "Looping on mother " << tempmother2 << " with PID " << tempmother2->particleID().pid() << endmsg;

      do {
        verbose() << "Step 13" << endmsg;
        if (tempmother2 != tempmother) tempmother2 = tempmother2->mother();
        else carryon = false;
        if (tempmother2 != NULL) {
          verbose() << "Not found the original mother yet, currently on " 
                    << tempmother2 
                    << " with PID " 
                    << tempmother2->particleID().pid() 
                    << endmsg;
        }
        verbose() << "Step 14" << endmsg;
      }while (carryon && (tempmother2 != NULL));
      verbose() << "Step 15" << endmsg;

      if (carryon) break;
      verbose() << "Step 16" << endmsg;

    }
    verbose() << "Step 17" << endmsg;

  } while (carryon);
  verbose() << "Step 18" << endmsg;

  return tempmother;
}
//=============================================================================
const LHCb::MCParticle* BackgroundCategory::get_lowest_common_mother(MCParticleVector mc_particles_linked_to_decay,
                                                               ParticleVector particles_in_decay)
//Gets the lowest common mother for an array of MCParticles, with associated particles.
//This method strips out any MCParticles associated to non-stable particles before 
//finding the common mother
{

  verbose() << "Starting search for lowest common mother" << endmsg;

  MCParticleVector::const_iterator iMCP = mc_particles_linked_to_decay.begin();
  ParticleVector::const_iterator iPP = particles_in_decay.begin();

  MCParticleVector mc_particles_to_compare;
  mc_particles_to_compare.clear(); 

  //First we isolate only those MC particles valid for this comparison,
  //i.e. only the ones matching to a stable final state particle
  do{
    verbose() << "Step 1" << endmsg;
    if(*iPP) {
      verbose() << "Step 2" << endmsg;
      verbose() << "Looking at Particle " << (*iPP) << " with PID " << (*iPP)->particleID().abspid() << endmsg;
      if ( isStable((*iPP)->particleID().abspid()) ||
           (*iPP)->isBasicParticle()
         ) {
        verbose() << "Step 3" << endmsg;
        //it is a stable so push back its associated MCP
        if (!(*iMCP)) return 0; //stable associated to nothing, so return 0
        verbose() << "Step 4" << endmsg;
        mc_particles_to_compare.push_back(*iMCP);
	verbose() << "Pushed back MCParticle " << (*iMCP) << " with PID " << (*iMCP)->particleID().abspid() << endmsg; 
        verbose() << "Step 5" << endmsg;
      }
      verbose() << "Step 6" << endmsg;
      ++iMCP; ++iPP;
      verbose() << "Step 7" << endmsg;
    } else return 0; //something went wrong 
    verbose() << "Step 8" << endmsg;
  }while(iMCP != mc_particles_linked_to_decay.end() &&
         iPP != particles_in_decay.end()
        );
  verbose() << "Step 9" << endmsg;

  return get_lowest_common_mother(mc_particles_to_compare);

} 
//=============================================================================
IBackgroundCategory::categories BackgroundCategory::category(const LHCb::Particle* reconstructed_mother)
  //The big "what is it?" switch. hopefully commented enough downstairs. For additional help with what 
  //all the categories mean, please visit IBackgroundCategory.h 
{

  m_pCPPAsct = new ProtoParticle2MCLinker( this,
                                           Particle2MCMethod::ChargedPP,
                                           std::vector<std::string>(1,"") );
  m_pNPPAsct = new ProtoParticle2MCLinker( this,
                                           Particle2MCMethod::NeutralPP,
                                           std::vector<std::string>(1,"") );
  m_pChi2PPAsct = new Particle2MCLinker( this,
                                         Particle2MCMethod::Composite,
                                         std::vector<std::string>(1,"") );

  m_commonMother = 0;
  //First of all, we use Patrick's tool to get all the particles in the decay tree
  verbose() << "About to start processing the categorisation tree for a " << 
  reconstructed_mother->particleID().pid()  << endmsg;
  ParticleVector particles_in_decay = m_particleDescendants->descendants(reconstructed_mother);
  verbose() << "Back from ParticleDescendants" << endmsg ;
  if (particles_in_decay.empty()) {
    Warning("No descendants for Particle");
    return Undefined; 
  } else {
    verbose() << "Descendents are " << particles_in_decay << endreq;
  }
  //Now we have to associate each one of them to an MCParticle if possible
  MCParticleVector mc_particles_linked_to_decay = associate_particles_in_decay(particles_in_decay,reconstructed_mother);
  //Now to create a vector with the final mothers of all these mc particles.
  MCParticleVector mc_mothers_final = get_mc_mothers(mc_particles_linked_to_decay);
  //First we test if the final state particles have a common mother;if it succeeds, 
  //we have signal or 'physics backgrounds', else 'combinatorics'
  //For a list of what the conditions are, see the respective test functions
  if (doAllFinalStateParticlesHaveACommonMother(mc_mothers_final,
                                                mc_particles_linked_to_decay, 
                                                particles_in_decay) ) {
    verbose() << "Checked if all have common mother" << endmsg;
    // We are in the territory of signal decays and
    // "physics" backgrounds (meaning we have reconstructed some actual decay 
    // that occured, even if not the one we were after).
    if (isTheDecayFullyReconstructed(mc_particles_linked_to_decay) ) {
      verbose() << "Checked if fully reconstructed" << endmsg;
      if (areAllFinalStateParticlesCorrectlyIdentified(particles_in_decay, 
                                                       mc_particles_linked_to_decay) ) {
        verbose() << "Checked if no reflections" << endmsg;
        if (isTheMotherCorrectlyIdentified(reconstructed_mother) ) {
          verbose() << "Checked if correct mother" << endmsg;
          if (!wereAnyResonancesMissed(reconstructed_mother) ) {
            verbose() << "This is signal" << endmsg;
            //This is a signal decay
            return Signal;
          } else {
            verbose() << "This is quasi-signal" << endmsg;
            //This is a quasi-signal decay
            return QuasiSignal;
          }
        } else {
          verbose() << "This is a fully reconstructed background" << endmsg;
          //This is a fully reconstructed physics background
          return FullyRecoPhysBkg;
        }
      } else {
        verbose() << "This is a reflection" << endmsg;
        //This is a reflection
        return Reflection;
      }    
    } else {
      verbose() << "Is it an an inclusive decay?" << endmsg;
      if (m_inclusiveDecay == 1) {
        verbose() << "Inclusive decay type" << endmsg;
        if (areAllFinalStateParticlesCorrectlyIdentified(particles_in_decay, 
                                                         mc_particles_linked_to_decay) && 
            isTheMotherCorrectlyIdentified(reconstructed_mother)) {
          return Signal;
        }
      }
      if (areAllFinalStateParticlesCorrectlyIdentified(particles_in_decay, 
                                                       mc_particles_linked_to_decay) && 
          checkLowMassBackground(reconstructed_mother) ) {
        verbose() << "This is low mass background" << endmsg;
        //This is a Low-mass background
        return LowMassBkg;
      } else {
        verbose() << "This is a partially reconstructed background" << endmsg;
        //This is a partially reconstructed physics background
        return PartRecoPhysBkg;
      }
    }
  } else {
    verbose() << "This is combinatorics" << endmsg;
    //Since condition A has failed, we are in the territory of combinatorics, pileup,
    //ghost-based and other assortead junk background.
    if ( areAnyFinalStateParticlesGhosts(mc_particles_linked_to_decay,
                                         particles_in_decay) ) {
      verbose() << "It is a ghost background" << endmsg;
      //This is a ghost
      return Ghost;
    } else {
      if (!isThisAPileup(mc_particles_linked_to_decay,
                         particles_in_decay) ){
        verbose() << "It is a pileup" << endmsg;
        //A pileup
        return FromDifferentPV;
      } else {
        int numFromPV = areAnyFinalStateParticlesFromAPrimaryVertex(mc_particles_linked_to_decay);
        if ( numFromPV > 0) {
          verbose() << "This is primary vertex background" << endmsg;
          if (numFromPV == 1) return FromPV;
          else return AllFromSamePV;
        } else {
          if (isThisBBarBackground(mc_mothers_final) ){
            verbose() << "This is bbar background" << endmsg;
            //This is a bbar event
            return bbar;
          } else {
            if (isThisCCbarBackground(mc_mothers_final) ){
              verbose() << "This is ccbar background" << endmsg;
              //This is a ccbar event
              return ccbar;
            } else {
              verbose() << "This is uds background" << endmsg;
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
bool BackgroundCategory::doAllFinalStateParticlesHaveACommonMother(MCParticleVector mc_mothers_final, 
                                                                   MCParticleVector mc_particles_linked_to_decay,
                                                                   ParticleVector particles_in_decay)
  //This condition checks that all the MCParticles associated to the final-state daughters of our
  //candidate Particle have a common MCParticle mother.   
{
  MCParticleVector::const_iterator iP = mc_mothers_final.begin();
  MCParticleVector::const_iterator iMCP = mc_particles_linked_to_decay.begin();
  ParticleVector::const_iterator iPP = particles_in_decay.begin();
  
  bool carryon = true;
  //bool stop = false;
  bool motherassignedyet = false; 
  const LHCb::MCParticle* tempmother = NULL;

  //Begin DEBUG printouts of the finalstate MC daughters and mothers
  verbose() << "####################################################################" << endmsg;
  verbose() << "Beginning printout of the final state mothers" << endmsg;
  verbose() << "********************************************************************" << endmsg;
  for (iP = mc_mothers_final.begin(); iP != mc_mothers_final.end(); ++iP) {
    verbose() << "Mother = " << (*iP) << endmsg;
    if (*iP){
      verbose() << "PID of mother = " << (*iP)->particleID().pid() << endmsg;
    }
  }
  verbose() << "********************************************************************" << endmsg;
  verbose() << "**********************************************" << endmsg;
  verbose() << "Beginning printout of the reconstructed particles" << endmsg;
  for (iPP = particles_in_decay.begin(); iPP != particles_in_decay.end(); ++iPP) {
    verbose() << "Reconstructed particle = " << (*iPP) << endmsg;
    if (*iPP) {
      verbose() << "PID of reconstructed particle = " << (*iPP)->particleID().pid() << endmsg;
    }
  }
  verbose() << "********************************************************************" << endmsg;
  verbose() << "**********************************************" << endmsg;
  verbose() << "Beginning printout of the associated particles" << endmsg;
  for (iMCP = mc_particles_linked_to_decay.begin(); iMCP != mc_particles_linked_to_decay.end(); ++iMCP) {
    verbose() << "Associated particle = " << (*iMCP) << endmsg;
    if (*iMCP) {
      verbose() << "PID of associated particle = " << (*iMCP)->particleID().pid() << endmsg;
    }
  }
  verbose() << "####################################################################" << endmsg;
  //End DEBUG

  //Reset the positions of the counters after the debug
  iP = mc_mothers_final.begin();
  iPP = particles_in_decay.begin();

  do {
    verbose() << "Condition A looping on " << (*iPP) << " and " << (*iP) << endmsg; 
 
    if (*iPP) {

      verbose() << "PID is " << (*iPP)->particleID().abspid() << endmsg;
 
      verbose () << "Passed" << endmsg; 
 
      //if ( (*iPP)->isBasicParticle()) { //final state particle
      if ( isStable((*iPP)->particleID().abspid()) || (*iPP)->isBasicParticle() ) { //final state particle -- the other way

        verbose () << "Passed" << endmsg;
 
        if ((*iP) == 0 ) {
          carryon = false; //final state particle has no mother - no need to go further
        } else {
          if (!motherassignedyet) {
            tempmother = *iP ;
            motherassignedyet = true;
          } else {
            verbose () << "Doing the check" << endmsg;
 
            if ( (*iP) == tempmother ) {
              carryon = true;
            } else {
              carryon = false;
            }
          }
        }
      }
    } else carryon = false;
 
    verbose() << "Common mother is " << tempmother 
              << " mother is " << (*iP) 
              << " particle is " << (*iPP) 
              << " and carryon is " << carryon << endmsg;
 
    ++iP; ++iPP;
 
  } while (carryon && iP != mc_mothers_final.end() && iPP != particles_in_decay.end() );
 
  if (!tempmother) carryon = false;
 
  if (carryon) { //We have found a common mother!

    //Now we have to check that there is no MC mother further down the chain which
    //still has all the final state particles as daughters. This is relevant for cases
    //like B*->gamma B->(ourdecay) or B->Dpi where we are associating the D. 

    verbose() << "About to start looking for the lowest common mother." << endmsg;

    m_commonMother = get_lowest_common_mother(mc_particles_linked_to_decay,particles_in_decay);
    //m_commonMother = tempmother;

    if (!m_commonMother) {

      info() << "If you are reading this error message, something has gone very wrong" << endmsg;
      info() << "with the background categorization, and the brave Back Cat has not been able to" << endmsg;
      info() << "recover the error. Most likely it was betrayed by some other piece of code." << endmsg;
      info() << "Please report the error to the relevant authorities without delay. Thank you." << endmsg;

      return 0;

    }

    verbose() << "Found common mother " 
              << m_commonMother->particleID().pid() 
              << endmsg ;
    if ( isStable(m_commonMother->particleID().abspid())){
      verbose() << "Common mother is a stable " 
                << m_commonMother->particleID().pid() 
                << endmsg ;
      Warning("Common mother is stable").ignore();
      m_commonMother = 0 ;
      carryon = false ;
    }
  } else m_commonMother = 0;

  return carryon;
}
//=============================================================================
bool BackgroundCategory::isTheDecayFullyReconstructed(MCParticleVector mc_particles_linked_to_decay)
  //This condition checks that all the final state daughters of the MCparticle
  //returned by condition A match up to the MCParticles associated to the  final state 
  //daughters of the candidate Particle. In effect, condition A checked whether all
  //the particles used to make our B came from one decay, and this checks if there 
  //are any particles coming from said decay which we missed out in our reconstruction. 
{
  //verbose() << "Beginning to check condition B" << endmsg;
  bool carryon;
  int neutrinosFound = 0;
  MCParticleVector finalstateproducts = create_finalstatedaughterarray_for_mcmother(m_commonMother);
  MCParticleVector::const_iterator iPP = finalstateproducts.begin();
  if (finalstateproducts.empty() ) Exception("Condition B 3 : No final states").ignore();
  MCParticleVector::const_iterator iP = mc_particles_linked_to_decay.begin();

  //Because of the special case of the merged pi0, we need to make a dummy array here
  //We will copy out mc_particles_linked_to_decay into it, replacing any pi0
  //particles by their daughters
  verbose() << "About to create a temporary array to deal with the merged pi0 case" << endmsg;
  MCParticleVector mc_particles_linked_to_decay_without_pi0;
  MCParticleVector merged_pi0_daughters;
  mc_particles_linked_to_decay_without_pi0.clear();
  for (iP = mc_particles_linked_to_decay.begin(); iP != mc_particles_linked_to_decay.end(); ++iP) {
    merged_pi0_daughters.clear();
    if ((*iP) == NULL) {
      mc_particles_linked_to_decay_without_pi0.push_back(*iP);
      continue;
    }
    verbose() << "PID of associated particle = " << (*iP)->particleID().pid() << endmsg;
    if ((*iP)->particleID().abspid() == 111){
      //if this is a merged pi0, we push back its daughters instead
      //this is only relevant for checking condition B, which is why
      //we use a temp array to do it
      merged_pi0_daughters = create_finalstatedaughterarray_for_mcmother(*iP);
      mc_particles_linked_to_decay_without_pi0.insert(
                                                      mc_particles_linked_to_decay_without_pi0.end(),
                                                      merged_pi0_daughters.begin(),
                                                      merged_pi0_daughters.end()
                                                      ) ;
    } else mc_particles_linked_to_decay_without_pi0.push_back(*iP);
  }

  //Begin DEBUG printouts of the finalstate MC daughters and the associated particles
  verbose() << "####################################################################" << endmsg;
  verbose() << "Beginning printout of the final state daughters of the common mother" << endmsg;
  verbose() << "********************************************************************" << endmsg;
  for (iPP = finalstateproducts.begin(); iPP != finalstateproducts.end(); ++iPP) {
    verbose() << "Daughter of common mother = " << (*iPP) << endmsg;
    if (*iPP){
      verbose() << "PID of daughter = " << (*iPP)->particleID().pid() << endmsg;
    }
  }
  verbose() << "********************************************************************" << endmsg;
  verbose() << "**********************************************" << endmsg;
  verbose() << "Beginning printout of the associated particles" << endmsg;
  for (iP = mc_particles_linked_to_decay_without_pi0.begin(); iP != mc_particles_linked_to_decay_without_pi0.end(); ++iP) {
    verbose() << "Associated particle = " << (*iP) << endmsg;
    if (*iP) {
      verbose() << "PID of associated particle = " << (*iP)->particleID().pid() << endmsg;
    }
  }
  verbose() << "**********************************************" << endmsg;
  //End DEBUG
  
  iPP =  finalstateproducts.begin();
  do {
    carryon = false;
    if (*iPP) {
      if ( ( m_semileptonicDecay && (neutrinosFound < m_numNeutrinos) && 
             //neutrinos are ignored if the decay is semileptonic
             ( 
              (*iPP)->particleID().abspid() == 12  || //neutrinos are ignored
              (*iPP)->particleID().abspid() == 14  || //neutrinos are ignored
              (*iPP)->particleID().abspid() == 16
              )
             )  ||
           ( //soft photons are ignored
            m_useSoftPhotonCut && (*iPP)->particleID().abspid() == 22 &&
            (*iPP)->momentum().e() < m_softPhotonCut 
            ) 
           ) {
      
        if (    (*iPP)->particleID().abspid() == 12  || //neutrinos are ignored
                (*iPP)->particleID().abspid() == 14  || //neutrinos are ignored
                (*iPP)->particleID().abspid() == 16) neutrinosFound +=1;

        verbose() << "Photon is being ignored!!!!!!!!!!!!!!!!!!!!" << endmsg;

        carryon = true;

      }  

      verbose() << "The MC final state particle has pid : " << (*iPP)->particleID().pid() << endmsg;
      if (!carryon) { //if the particle is not to be ignored
        for (iP = mc_particles_linked_to_decay_without_pi0.begin(); 
          iP != mc_particles_linked_to_decay_without_pi0.end(); ++iP) {
          if ( *iP == 0) continue; //if no matching MCParticle
          if ( m_useSoftPhotonCut && 
            (*iP)->particleID().abspid() == 22 && 
            (*iP)->momentum().e() < m_softPhotonCut ) continue; //soft photons are ignored

          verbose() << "The MC-associated particle has pid : " << (*iP)->particleID().pid() << endmsg;
          SmartRefVector<LHCb::MCVertex>::const_iterator iVV = (*iP)->endVertices().begin();
          if ( (*iVV)->products().empty() || isStable( (*iP)->particleID().abspid() )  ) {
            verbose() << "Associated Particle:" << (*iP) << endmsg;
            verbose() << "MC-final state Particle:" << (*iPP) << endmsg;
            carryon = ( *iP == *iPP ); 
            if (carryon) break;
          }
        }
      }
    }

    ++iPP;

  } while(carryon && iPP != finalstateproducts.end());

  verbose() << "Has condition B passed or failed?" << endmsg;
  verbose() << "*********************************" << endmsg;
  verbose() << carryon << endmsg; 
  verbose() << "*********************************" << endmsg;
  verbose() << "#################################" << endmsg;

  return carryon;
}
//=============================================================================
bool BackgroundCategory::areAllFinalStateParticlesCorrectlyIdentified(ParticleVector particles_in_decay, 
                                                                      MCParticleVector mc_particles_linked_to_decay)
  //This condition checks if all the final state particles used to make the candidate particle are correctly
  //identified (according to PID).
{
  bool carryon = true;

  MCParticleVector::const_iterator iPmc = mc_particles_linked_to_decay.begin();
  ParticleVector::const_iterator iP = particles_in_decay.begin();

  do {
    if ( *iPmc && *iP) {
      //if ( (*iP)->isBasicParticle() ) {
      if( isStable((*iP)->particleID().abspid()) || 
          (*iP)->isBasicParticle()
        ){
        carryon = ( (*iP)->particleID().pid() == (*iPmc)->particleID().pid() );
      }
    }
    ++iP; ++iPmc;
  } while (carryon && iP != particles_in_decay.end() && iPmc != mc_particles_linked_to_decay.end() );

  return carryon;
}
//=============================================================================
bool BackgroundCategory::isTheMotherCorrectlyIdentified(const LHCb::Particle* reconstructed_mother)
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
bool BackgroundCategory::wereAnyResonancesMissed(const LHCb::Particle* candidate)
  //Check whether we missed any resonances in the decay chain. 
{

  if ( topologycheck(candidate) == topologycheck(m_commonMother) ) return false;
  else return true;
  
}
//=============================================================================
bool BackgroundCategory::checkLowMassBackground(const LHCb::Particle* candidate)
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
bool BackgroundCategory::areAnyFinalStateParticlesGhosts(MCParticleVector mc_particles_linked_to_decay, 
                                                         ParticleVector particles_in_decay)
  //Checks if there are any ghosts in our final state Particles used to make the candidate Particle
{
  bool carryon = true;
  MCParticleVector::const_iterator iP = mc_particles_linked_to_decay.begin();
  ParticleVector::const_iterator iPP = particles_in_decay.begin();

  do {

    if ( 
        (*iP) == 0 && 
        //(*iPP)->isBasicParticle() 
        ( isStable((*iPP)->particleID().abspid()) || (*iPP)->isBasicParticle()  )
        ) 
      carryon = false;  
    ++iP;
    ++iPP;

  } while (carryon && iP != mc_particles_linked_to_decay.end() && iPP != particles_in_decay.end() );

  return !carryon;

}
//=============================================================================
bool BackgroundCategory::isThisAPileup(MCParticleVector mc_particles_linked_to_decay, 
                                       ParticleVector particles_in_decay)
  //Checks if the event is a pileup (final state Particles come from at least 2 different collisions) or not.
{
  bool carryon = true;
  MCParticleVector::const_iterator iP = mc_particles_linked_to_decay.begin();
  ParticleVector::const_iterator iPP = particles_in_decay.begin();
  //const Collision* tmpcollision = NULL;
  const LHCb::MCVertex* tmpcollision = NULL;
  bool gotacollision = false;

  do {

    //if ( (*iPP)->isBasicParticle() ) {
    if ( isStable((*iPP)->particleID().abspid()) || (*iPP)->isBasicParticle() ) {

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
bool BackgroundCategory::isThisBBarBackground(MCParticleVector mc_mothers_final)
  //Checks if at least one of the final state MCParticles associated
  //to the decay products of the candidate Particle had a mother
  //with bottom content.  
{
  bool carryon = false;
  MCParticleVector::const_iterator iP = mc_mothers_final.begin();

  do {

    if (*iP) 
      if ( (*iP)->particleID().hasBottom() ) carryon = true;

    ++iP;

  } while (!carryon && iP != mc_mothers_final.end() );

  return carryon;  
}
//=============================================================================
bool BackgroundCategory::isThisCCbarBackground(MCParticleVector mc_mothers_final)
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
int BackgroundCategory::areAnyFinalStateParticlesFromAPrimaryVertex(MCParticleVector mc_particles_linked_to_decay) 
{
  //This function evaluates whether some of the particles in the final state
  //of the candidate come from the primary vertex. Returns 0 if none, 1 if one,
  //and 99 if all.
  int howmanyfinalstate = 0; 
  int howmanyfromPV = 0;
  
  //MCParticleVector::const_iterator iP = mc_mothers_final.begin();

  for (MCParticleVector::const_iterator iPP = mc_particles_linked_to_decay.begin(); 
       iPP != mc_particles_linked_to_decay.end(); ++iPP) {

    if (*iPP) {
      SmartRefVector<LHCb::MCVertex>::const_iterator iVV = (*iPP)->endVertices().begin();
      if (*iVV) { 
        if ( (*iVV)->products().empty() || isStable( (*iPP)->particleID().abspid()) ) {
          ++howmanyfinalstate;
          if ((*iPP)->originVertex()->isPrimary() || 
              ((*iPP)->primaryVertex()->position() - 
               (*iPP)->originVertex()->position()).Mag2() < pow(m_rescut,2)) {
            ++howmanyfromPV;
          }
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
MCParticleVector BackgroundCategory::associate_particles_in_decay(ParticleVector particles_in_decay,const LHCb::Particle* reconstructed_mother)
  //Associates all the final state particles daughters of the candidate Particle. For composites, 
  //attempts to use the Chi2 associator, which has to be configured properly for this (utterly
  //non-essential) feature to work. The reconstructed mother is used to check the neutral association.
{
  //verbose() << "Beginning to associate descendants" << endmsg; 
  MCParticleVector associated_mcparts;
  ParticleVector::iterator iP;
  MCParticleVector::iterator iPP;
  DaughterAndPartnerPair temp_pair;
  DaughterAndPartnerVector tempDaughtersAndPartners;

  //verbose() << "Associating step 1" << endmsg;
  int verboses = 0;
  bool associating_a_neutral = false;
  bool found_neutral_mother = false;

  for (iP = particles_in_decay.begin() ; iP != particles_in_decay.end() ; ++iP){
    ++verboses;
    verbose() << "About to check if we are dealing with a basic particle" << endmsg;
    verbose() << "Particle PID is " << (*iP)->particleID().pid() << endmsg;
    //if ( (*iP)->isBasicParticle() ) {
    if ( isStable((*iP)->particleID().abspid()) || (*iP)->isBasicParticle() ) {
      //verbose() << "Associating step 3a - loop step " << verboses << endmsg;
      verbose() << "About to get the relevant protoparticle" << endmsg;
      const LHCb::ProtoParticle* protoTemp = (*iP)->proto();//dynamic_cast<ProtoParticle*>((*iP)->origin());
      //verbose() << "Associating step 4a - loop step " << verboses << endmsg;
      verbose() << "Protoparticle of " << (*iP)->particleID().pid() << " is at " << protoTemp << endmsg;
      const LHCb::MCParticle* mcTemp;
      if ( (*iP)->particleID().pid() == 22 || (*iP)->particleID().pid() == 111) {
        mcTemp = m_pNPPAsct->firstMCP(protoTemp);//associatedFrom(protoTemp);
        //mcTemp = m_pNPPAsct->firstMCP(*iP);

        /*verbose() << "Neutral Particle has PID = " << (*iP)->particleID().pid() << endmsg;
          verbose() << "N energy = " << (*iP)->momentum().e() << endmsg;
          verbose() << "N pt = " << (*iP)->momentum().vect().perp() << endmsg;
          verbose() << "N momentum = " << (*iP)->momentum().vect().mag() << endmsg;
          verbose() << "====================================================================" << endmsg;
          verbose() << "Neutral MCParticle is at " << mcTemp << endmsg;
          verbose() << "Neutral MCParticle has PID = " << mcTemp->particleID().pid() << endmsg;
          verbose() << "MCN energy = " << mcTemp->momentum().e() << endmsg;
          verbose() << "MCN pt = " << mcTemp->momentum().vect().perp() << endmsg;
          verbose() << "MCN momentum = " << mcTemp->momentum().vect().mag() << endmsg; */
      } else {
        mcTemp = m_pCPPAsct->firstMCP(protoTemp);//associatedFrom(protoTemp);
        //mcTemp = m_pCPPAsct->firstMCP(*iP);

        /*verbose() << "First associated MCParticle is at " << mcTemp ; 
          if ( NULL!=mcTemp) verbose() << " (" << mcTemp->particleID().pid() << ")" << endmsg;
          else verbose() << endmsg;
          verbose() << "Its weight is " << m_pCPPAsct->weightMCP() << endmsg;

          //FOR DEBUG ONLY
          mcTemp = m_pCPPAsct->nextMCP();
          while (mcTemp != NULL) {

          verbose() << "There is ANOTHER matching MC particle!!" << endmsg;
          verbose() << "Next associated MCParticle is at " << mcTemp << endmsg;
          verbose() << "Its weight is " << m_pCPPAsct->weightMCP() << endmsg;

          mcTemp = m_pCPPAsct->nextMCP();

          }
          //mcTemp = m_pCPPAsct->firstMCP(protoTemp);
          mcTemp = m_pCPPAsct->firstMCP(*iP);*/
        //END OF DEBUG

      }
      if (!mcTemp) {
        associated_mcparts.push_back(mcTemp);
        continue;
      }
      //if (mcTemp->particleID().pid() == (*iP)->particleID().pid()) {
      if (0==1) { //turned off for verbose
        //verbose() << "Associating step 5a - loop step " << verboses << endmsg;
        //verbose() << "MCParticle is at " << mcTemp << endmsg;
        verbose() << "PIDs match so pushing back the first associated MCParticle "<< endmsg;
        associated_mcparts.push_back(mcTemp);
        //verbose() << "Associating step 6a - loop step " << verboses << endmsg;
      } else {

        verbose() << "PIDs don't match so searching through the array of associated MCParticles"<< endmsg;

        //associated_mcparts.push_back(mcTemp);

        //}
        //New commands to look for a range of particles
        const LHCb::MCParticle* mc_correctPID = NULL;
        const LHCb::MCParticle* mc_bestQ = NULL;
        const LHCb::MCParticle* mc_TempDeux = NULL;
        double minimumweight = m_minWeight;
        double mc_weight = 0.;
        double mc_correctPID_weight = minimumweight;
        
        ProtoParticle2MCLinker::ToRange mcPartRange;
        //Particle2MCLinker::ToRange mcPartRange;

        verbose() << "About to get the array of matching particles" << endmsg;
  
        if ( (*iP)->particleID().pid() == 22 || (*iP)->particleID().pid() == 111) {
          mcPartRange = m_pNPPAsct->rangeFrom(protoTemp);
          associating_a_neutral = true;
          //mcPartRange = m_pNPPAsct->rangeFrom(*iP);
        } else {
          mcPartRange = m_pCPPAsct->rangeFrom(protoTemp);
          associating_a_neutral = false;
          //mcPartRange = m_pCPPAsct->rangeFrom(*iP);
        }

        verbose() << "Got the array of matching particles OK!" << endmsg;

        ProtoParticle2MCLinker::ToIterator mcPartIt;
        //Particle2MCLinker::ToIterator mcPartIt;

        int looper = 1;

        for (mcPartIt = mcPartRange.begin(); mcPartIt!=mcPartRange.end(); ++mcPartIt) {
          verbose() << "About to get the " << looper << "th match" << endmsg;
          mc_TempDeux = (*mcPartIt).to();
          verbose() << "To Range MCParticle is at " << mc_TempDeux << endmsg;
          verbose() << "To Range MCParticle has PID = " << mc_TempDeux->particleID().pid() << endmsg;
          verbose() << "To Range energy = " << mc_TempDeux->momentum().E() << endmsg;
          verbose() << "Range pt = " << mc_TempDeux->momentum().Pt() << endmsg;
          verbose() << "Range momentum = " << mc_TempDeux->momentum().P() << endmsg;
          verbose() << "Range weight = " << (*mcPartIt).weight() << endmsg; 
          mc_weight = (*mcPartIt).weight();
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
          if (associating_a_neutral && 
              ( reconstructed_mother->particleID().pid() == 
                mc_TempDeux->particleID().pid()
              )
             ) {
            found_neutral_mother = true;
          }
          looper++;
        }
        if (mc_correctPID) {
          if ((mc_correctPID == mc_bestQ)) {
            verbose() << "Pushing back best match" << endmsg;
            verbose() << "Best PID weight = " << minimumweight << endmsg;
            associated_mcparts.push_back(mc_bestQ);
          } else {
            if ((mc_correctPID_weight > m_override) || (associating_a_neutral && found_neutral_mother)) {
              verbose() << "Pushing back best pid match" << endmsg;
              verbose() << "Best PID weight = " << mc_correctPID_weight << endmsg;
              associated_mcparts.push_back(mc_correctPID);
            } else {
              verbose() << "OVERRIDE! Pushing back best Q match" << endmsg;
              verbose() << "Best Q match has PID = " << mc_bestQ->particleID().pid() << endmsg;
              verbose() << "Best PID weight = " << mc_correctPID_weight << endmsg;
              verbose() << "Best Q = " << minimumweight << endmsg;
              associated_mcparts.push_back(mc_bestQ);
            }
          }
        } else {
          //No match with correct PID, we just match the best quality one there is
          verbose() << "Pushing back best Q match" << endmsg;
          verbose() << "Best Q match has PID = " << mc_bestQ->particleID().pid() << endmsg;
          verbose() << "Best Q = " << minimumweight << endmsg;
          associated_mcparts.push_back(mc_bestQ);
        }
      }
    } else {
      //verbose() << "Associating step 3b - loop step " << verboses << endmsg;
      const LHCb::Particle* partTemp = *iP;
      //verbose() << "Associating step 4b - loop step " << verboses << endmsg;
      verbose() << "Composite Particle is at " << partTemp << endmsg;
      const LHCb::MCParticle* mcTemp = m_pChi2PPAsct->firstMCP(partTemp);//associatedFrom(partTemp);
      //verbose() << "Associating step 5b - loop step " << verboses << endmsg;
      verbose() << "Composite MCParticle is at " << mcTemp << endmsg;
      associated_mcparts.push_back(mcTemp);
      //associated_mcparts.push_back(0);
      //verbose() << "Associating step 6b - loop step " << verboses << endmsg;
    }
  } 

  //Now write the array of ''daughter--associated partner'' pairs
  iPP = associated_mcparts.begin();
  for (iP = particles_in_decay.begin() ; iP != particles_in_decay.end() ; ++iP){

    temp_pair = DaughterAndPartnerPair(*iP,*iPP);
    tempDaughtersAndPartners.push_back(temp_pair);

    ++iPP;

  }

  m_daughtersAndPartners = tempDaughtersAndPartners;

  DaughterAndPartnerVector::const_iterator iDAP;

  for (iDAP = m_daughtersAndPartners.begin(); iDAP != m_daughtersAndPartners.end(); ++iDAP){

    verbose() << "Reconstructed particle has PID " << ((*iDAP).first)->particleID().pid() << endmsg;
    if ((*iDAP).second != NULL) verbose() << "Associated particle has PID " << ((*iDAP).second)->particleID().pid() << endmsg;
    else  verbose() << "There is no associated particle" << endmsg;

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

  verbose() << "Starting to initialise Background Categorisation" << endmsg;

  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;

  m_ppSvc = 0;
  sc = service("ParticlePropertySvc", m_ppSvc);
  if (!sc) return sc;

  /*m_pCPPAsct = new ProtoParticle2MCLinker( this, 
                                           Particle2MCMethod::ChargedPP, 
                                           std::vector<std::string>(1,"") );
  m_pNPPAsct = new ProtoParticle2MCLinker( this, 
                                           Particle2MCMethod::NeutralPP,
                                           std::vector<std::string>(1,"") );
  //m_pCPPAsct = new Particle2MCLinker( this, Particle2MCMethod::Links, "");
  //m_pNPPAsct = new Particle2MCLinker( this, Particle2MCMethod::Links, "");
  m_pChi2PPAsct = new Particle2MCLinker( this, 
                                         Particle2MCMethod::Composite,
                                         std::vector<std::string>(1,"") );*/

  //m_pCPPAsct = new Particle2MCLinker( this, "ChargedPP2MC", "/MyChargedPP2MC", "");
  //m_pNPPAsct = new Particle2MCLinker( this, "NeutralPP2MC", "/MyNeutralPP2MC", "");
  //m_pChi2PPAsct = new Particle2MCLinker( this, "CompositeParticle2MCLinks", "/MyCompositeParticle2MCLinks","");

  //m_pCPPAsct = tool<ProtoParticle2MCAsct::IAsct>("ProtoParticle2MCAsct","ChargedPP2MCAsct",this);
  //m_pNPPAsct = tool<ProtoParticle2MCAsct::IAsct>("ProtoParticle2MCAsct","NeutralPP2MCAsct",this);
  //m_pChi2PPAsct = tool<Particle2MCWithChi2Asct::IAsct>("Particle2MCWithChi2Asct","Particle2MCWithChi2",this);
  m_particleDescendants = tool<IParticleDescendants>("ParticleDescendants",this);

  verbose() << "Done initializing" << endmsg ;
 
  return StatusCode::SUCCESS;
}
