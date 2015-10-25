// $Id: BackgroundCategory.cpp,v 1.61 2010-03-01 22:07:05 gligorov Exp $

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "BackgroundCategory.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BackgroundCategory
//
// 2005-11-23 : Vladimir Gligorov
//
//-----------------------------------------------------------------------------

using namespace Gaudi::Units;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BackgroundCategory::BackgroundCategory( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_ppSvc(NULL)
  , m_particleDescendants(NULL)
  , m_smartAssociator(NULL)
  , m_printDecay(NULL)
  , m_commonMother(NULL)
{
  IBackgroundCategory::m_cat[-1]   = "Undefined";
  IBackgroundCategory::m_cat[0]    = "Signal";
  IBackgroundCategory::m_cat[10]   = "QuasiSignal";
  IBackgroundCategory::m_cat[20]   = "FullyRecoPhysBkg";
  IBackgroundCategory::m_cat[30]   = "Reflection";
  IBackgroundCategory::m_cat[40]   = "PartRecoPhysBkg";
  IBackgroundCategory::m_cat[50]   = "LowMassBkg";
  IBackgroundCategory::m_cat[60]   = "Ghost";
  IBackgroundCategory::m_cat[63]   = "Clone";
  IBackgroundCategory::m_cat[66]   = "Hierarchy";
  IBackgroundCategory::m_cat[70]   = "FromPV";
  IBackgroundCategory::m_cat[80]   = "AllFromSamePV";
  IBackgroundCategory::m_cat[100]  = "FromDifferentPV";
  IBackgroundCategory::m_cat[110]  = "bbar";
  IBackgroundCategory::m_cat[120]  = "ccbar";
  IBackgroundCategory::m_cat[130]  = "uds";
  IBackgroundCategory::m_cat[1000] = "LastGlobal";

  declareInterface<IBackgroundCategory>(this);
  declareProperty("LowMassBackgroundCut", m_lowMassCut     = 100.*MeV );
  declareProperty("SoftPhotonCut", m_softPhotonCut         = 300.*MeV );
  declareProperty("UseSoftPhotonCut", m_useSoftPhotonCut   = true     );
  declareProperty("InclusiveDecay", m_inclusiveDecay       = false    );
  declareProperty("SemileptonicDecay", m_semileptonicDecay = false    );
  declareProperty("NumNeutrinos", m_numNeutrinos           = 0        );
  declareProperty("MCmatchQualityPIDoverrideLevel", m_override = 0.0  );
  declareProperty("Calo2MCWeight", m_caloWeight=0.5); // equivalent of m_ovverride for neutral calorimetric objects
  declareProperty("ResonanceCut", m_rescut = 10.e-6);
  declareProperty("MCminWeight", m_minWeight = 0.);
  declareProperty("vetoNeutralRedo", m_vetoN = false);
  declareProperty("IgnoreQuarks",m_ignoreQuarks = true);  
}

//=============================================================================
// Destructor
//=============================================================================
BackgroundCategory::~BackgroundCategory() {}

//=============================================================================
IBackgroundCategory::categories
BackgroundCategory::category( const LHCb::Particle* reconstructed_mother,
                              const LHCb::Particle* )
//The big "what is it?" switch. hopefully commented enough downstairs. For additional help with what
//all the categories mean, please visit IBackgroundCategory.h
{

  //Check if the tool was actually given a particle to categorise
  if ( !reconstructed_mother )
  {
    Exception("Given nothing to classify. Bye!");
  }

  //Initialize the value of the common mother to zero
  m_commonMother = NULL;

  if ( !m_smartAssociator )
  {
    Exception("Something failed when making the associators. Bye!");
  }

  if ( !m_particleDescendants )
  {
    Exception("Something failed when making the particle descendents tool. Bye!");
  }

  //Some debug information at VERBOSE level
  if (msgLevel(MSG::VERBOSE)) verbose() << "About to start processing the categorisation tree for a "
                                        <<  reconstructed_mother->particleID().pid()
                                        << " "
                                        << reconstructed_mother->momentum()
                                        << endmsg;

  if (m_printDecay) m_printDecay->printTree ( reconstructed_mother );
  //We use Patrick's tool to get all the particles in the decay tree.
  ParticleVector particles_in_decay = m_particleDescendants->descendants(reconstructed_mother);
  if (msgLevel(MSG::VERBOSE)) verbose() << "Back from ParticleDescendants with the daughters"
                                        << endmsg ;
  if (particles_in_decay.empty())
  {
    return Undefined;
  }
  else
  {
    if (msgLevel(MSG::VERBOSE)) verbose() << "Descendents are "
                                          << particles_in_decay
                                          << endmsg;
  }

  //Now we have to associate each one of them to an MCParticle if possible
  const MCParticleVector mc_particles_linked_to_decay =
    associate_particles_in_decay(particles_in_decay,reconstructed_mother);

  //First of all check for ghosts
  if ( areAnyFinalStateParticlesGhosts(mc_particles_linked_to_decay,
                                       particles_in_decay) )
  {
    if (msgLevel(MSG::VERBOSE)) verbose() << "It is a ghost background" << endmsg;
    //This is a ghost
    return Ghost;
  }

  //Next check if two Particles are associated to the same particle,
  //if so this is a clone type background
  if (foundClones(mc_particles_linked_to_decay))
  {
    if (msgLevel(MSG::VERBOSE)) verbose() << "This is a clone background" << endmsg;
    return Clone;
  }

  //Now we have to check if any of the associated MCParticles
  //are also the mothers of another associated MCParticle.
  //In this case we have combined something with its own mother to
  //create a new particle which is clearly unphysical. We'll
  //call this 'hierarchy' background until someone complains.
  if ( hierarchyProblem(mc_particles_linked_to_decay) )
  {
    if (msgLevel(MSG::VERBOSE)) verbose() << "This is a hierarchy background" << endmsg;
    return Hierarchy;
  }

  //Now to create a vector with the final mothers of all these mc particles.
  MCParticleVector mc_mothers_final = get_mc_mothers(mc_particles_linked_to_decay);

  //First we test if the final state particles have a common mother;if it succeeds,
  //we have signal or 'physics backgrounds', else 'combinatorics'
  //For a list of what the conditions are, see the respective test functions
  if ( doAllFinalStateParticlesHaveACommonMother(mc_mothers_final,
                                                 mc_particles_linked_to_decay,
                                                 particles_in_decay) )
  {
    if (msgLevel(MSG::VERBOSE)) verbose() << "Checked if all have common mother"
                                          << endmsg;
    // We are in the territory of signal decays and
    // "physics" backgrounds (meaning we have reconstructed some actual decay
    // that occured, even if not the one we were after).
    if (isTheDecayFullyReconstructed(mc_particles_linked_to_decay) ) {
      if (msgLevel(MSG::VERBOSE)) verbose() << "Checked if fully reconstructed" << endmsg;
      if (areAllFinalStateParticlesCorrectlyIdentified(particles_in_decay,
                                                       mc_particles_linked_to_decay) ) {
        if (msgLevel(MSG::VERBOSE)) verbose() << "Checked if no reflections" << endmsg;
        if (isTheMotherCorrectlyIdentified(reconstructed_mother) ) {
          if (msgLevel(MSG::VERBOSE)) verbose() << "Checked if correct mother" << endmsg;
          if (!wereAnyResonancesMissed(reconstructed_mother) ) {
            if (msgLevel(MSG::VERBOSE)) verbose() << "This is signal" << endmsg;
            //This is a signal decay
            return Signal;
          } else {
            if (msgLevel(MSG::VERBOSE)) verbose() << "This is quasi-signal" << endmsg;
            //This is a quasi-signal decay
            return QuasiSignal;
          }
        } else {
          if (msgLevel(MSG::VERBOSE)) verbose() << "This is a fully reconstructed background" << endmsg;
          //This is a fully reconstructed physics background
          return FullyRecoPhysBkg;
        }
      } else {
        if (msgLevel(MSG::VERBOSE)) verbose() << "This is a reflection" << endmsg;
        //This is a reflection
        return Reflection;
      }
    }
    else
    {
      if (msgLevel(MSG::VERBOSE)) verbose() << "Is it an inclusive decay?" << endmsg;
      if ( m_inclusiveDecay )
      {
        if (msgLevel(MSG::VERBOSE)) verbose() << "Inclusive decay type" << endmsg;
        if (areAllFinalStateParticlesCorrectlyIdentified(particles_in_decay,
                                                         mc_particles_linked_to_decay) &&
            isTheMotherCorrectlyIdentified(reconstructed_mother)) {
          return Signal;
        }
      }
      if (areAllFinalStateParticlesCorrectlyIdentified(particles_in_decay,
                                                       mc_particles_linked_to_decay) &&
          checkLowMassBackground(reconstructed_mother) ) {
        if (msgLevel(MSG::VERBOSE)) verbose() << "This is low mass background" << endmsg;
        //This is a Low-mass background
        return LowMassBkg;
      } else {
        if (msgLevel(MSG::VERBOSE)) verbose() << "This is a partially reconstructed background" << endmsg;
        //This is a partially reconstructed physics background
        return PartRecoPhysBkg;
      }
    }
  } else {
    if (msgLevel(MSG::VERBOSE)) verbose() << "This is combinatorics" << endmsg;
    //Since condition A has failed, we are in the territory of combinatorics, pileup,
    //ghost-based and other assortead junk background.
    if (!isThisAPileup(mc_particles_linked_to_decay,
                       particles_in_decay) ){
      if (msgLevel(MSG::VERBOSE)) verbose() << "It is a pileup" << endmsg;
      //A pileup
      return FromDifferentPV;
    } else {
      const int numFromPV = areAnyFinalStateParticlesFromAPrimaryVertex(mc_particles_linked_to_decay);
      if ( numFromPV > 0) {
        if (msgLevel(MSG::VERBOSE)) verbose() << "This is primary vertex background" << endmsg;
        if (numFromPV == 1) return FromPV;
        else return AllFromSamePV;
      } else {
        if (isThisBBarBackground(mc_mothers_final) ){
          if (msgLevel(MSG::VERBOSE)) verbose() << "This is bbar background" << endmsg;
          //This is a bbar event
          return bbar;
        } else {
          if (isThisCCbarBackground(mc_mothers_final) ){
            if (msgLevel(MSG::VERBOSE)) verbose() << "This is ccbar background" << endmsg;
            //This is a ccbar event
            return ccbar;
          } else {
            if (msgLevel(MSG::VERBOSE)) verbose() << "This is uds background" << endmsg;
            //This is a light flavour event
            return uds;
          }
        }
      }
    }
  }
}
//=============================================================================
const BackgroundCategory::DaughterAndPartnerVector
BackgroundCategory::getDaughtersAndPartners( const LHCb::Particle* reconstructed_mother)
//Returns the daughters of the particle being associated and their MC associated partners; this is a sort
//of 'hidden' method.
{
  if ( !reconstructed_mother )
  {
    Exception("Got NULL pointer");
  }

  const int backcategory = category(reconstructed_mother);

  if (msgLevel(MSG::VERBOSE)) verbose() << "Event is category "
                                        << backcategory
                                        << endmsg;

  for ( DaughterAndPartnerVector::const_iterator iP = m_daughtersAndPartners.begin();
        iP != m_daughtersAndPartners.end(); ++iP )
  {

    if (msgLevel(MSG::VERBOSE)) verbose() << "Reconstructed particle has PID"
                                          << ((*iP).first)->particleID().pid()
                                          << endmsg;

    if ((0!=(*iP).second) && (msgLevel(MSG::VERBOSE))) verbose() << "Associated particle has PID"
                                                                 << ((*iP).second)->particleID().pid()
                                                                 << endmsg;
    else  if (msgLevel(MSG::VERBOSE)) verbose() << "There is no associated particle"
                                                << endmsg;

  }

  return m_daughtersAndPartners;
}
//=============================================================================
const LHCb::MCParticle* BackgroundCategory::origin(const LHCb::Particle* reconstructed_mother)
//Returns the MCParticle associated to your reconstructed candidate if there is one.
//Note that the candidate may still be background even if this MCParticle has the same
//PID as your composite particle!
{
  if ( !reconstructed_mother )
  {
    Exception("Got NULL pointer");
  }

  const int backcategory = category(reconstructed_mother);

  if ( backcategory < 60 )
  {
    if (msgLevel(MSG::VERBOSE)) verbose() << "Getting the common mother"
                                          << endmsg;
    if ( m_commonMother )
    {
      return m_commonMother;
    }
    else { return NULL; }
  }
  else
  {
    return NULL;
  }
}
//=============================================================================
bool BackgroundCategory::isStable(int pid)
//A cheap and cheerful list of particles considered stable for the purposes of
//this tool. Not the cleverest way to do this, but works OK.
{
  if ( pid < 0 ) pid *= -1;

  if ( pid == 11 || //electron
       pid == 22 || //photon  - photon conversion is treated elsewhere
       pid == 13 || //muon
       pid == 12 || //neutrinos
       pid == 14 ||
       pid == 16 ||
       pid == 211 || //pion (charged)
       pid == 321 || //kaon (charged)
       pid == 2212 //|| //proton
       ) return true; else return false;

}
//=============================================================================
BackgroundCategory::MCParticleVector
BackgroundCategory::get_mc_mothers(const MCParticleVector & mc_particles_linked_to_decay)
//This function is responsible for getting the original mother of each
//MCParticle associated to a final state product of
//our candidate particle. If there is no such mother,
//a 0 is entered, and the same is done if there is no matching
//MCParticle in the first place (for ghosts)
{
  MCParticleVector mc_mothers;
  MCParticleVector::const_iterator iP;

  if (msgLevel(MSG::VERBOSE)) verbose() << "Starting to find the MC mothers of the associated particles"
                                        << endmsg;

  for (iP = mc_particles_linked_to_decay.begin(); iP != mc_particles_linked_to_decay.end(); ++iP) {
    if (msgLevel(MSG::VERBOSE)) verbose() << "Finding the final mother of "
                                          << *iP
                                          << endmsg;
    mc_mothers.push_back(get_top_mother_of_MCParticle(*iP));
  }

  return mc_mothers;
}
//=============================================================================
int BackgroundCategory::topologycheck(const LHCb::MCParticle* topmother)
//This returns the sum of the PIDs of all the daughters of the common MCParticle mother
//except photons and neutrinos
{

  int sumofpids = 0;
  bool isitstable = false;

  //Check for an MCMother with a null endvertex -- really shouldn't happen
  //here though, but the Cat is often betrayed by other pieces of code...
  const SmartRefVector<LHCb::MCVertex>& motherEndVertices = topmother->endVertices();
  if ( motherEndVertices.empty() )
    Exception("The Cat found a common MC mother but it has no daughters, please report this as a bug.");
  //Assuming all went well...
  SmartRefVector<LHCb::MCVertex>::const_iterator iV = motherEndVertices.begin();

  //Print out some debug messages
  if (msgLevel(MSG::VERBOSE)) verbose() << "Printing out decay vertices of a "
                                        << topmother->particleID().abspid()
                                        << endmsg;
  if (msgLevel(MSG::VERBOSE)) verbose() << topmother->endVertices()
                                        << endmsg;

  for ( SmartRefVector<LHCb::MCParticle>::const_iterator iP = (*iV)->products().begin();
        iP != (*iV)->products().end(); ++iP )
  {

    if ( m_useSoftPhotonCut &&
         (*iP)->particleID().abspid() == 22 &&
         (*iP)->momentum().e() < m_softPhotonCut ) continue; //soft photons are ignored

    //Need to protect against MCParticles with null endVertices,
    //which are assumed to be stable
    const SmartRefVector<LHCb::MCVertex>& VV = (*iP)->endVertices();
    if ( !VV.empty() ) isitstable = (*(VV.begin()))->products().empty();
    else isitstable = true;

    //Print out some debug messages
    if (msgLevel(MSG::VERBOSE)) verbose() << "Printing out decay vertices of a "
                                          << (*iP)->particleID().abspid()
                                          << " (E=" << (*iP)->momentum().e() << ") "
                                          << endmsg;
    if (msgLevel(MSG::VERBOSE)) verbose() << (*iP)->endVertices()
                                          << endmsg;

    if (  isitstable || //if the MCParticle did not have an endVertex
          isStable( (*iP)->particleID().abspid() )  //if it is one of the MCParticles we always call stable
          ) {

      // neutrinos are not reconstructed
      if(((*iP)->particleID().abspid() != 12) &&
         ((*iP)->particleID().abspid() != 14) &&
         ((*iP)->particleID().abspid() != 16)
         ) sumofpids += (*iP)->particleID().abspid();

      if (msgLevel(MSG::VERBOSE)) verbose() << "MC sum having added a neutral is now "
                                            << sumofpids
                                            << endmsg;

    } else { //Add composites and charged particles

      sumofpids += (*iP)->particleID().abspid();
      sumofpids += topologycheck(*iP);

      if (msgLevel(MSG::VERBOSE)) verbose() << "MC sum having added a charged or composite is now "
                                            << sumofpids
                                            << endmsg;
    }

  }

  if (msgLevel(MSG::VERBOSE)) verbose() << "Final MC sum is "
                                        << sumofpids
                                        << endmsg;

  return sumofpids;
}
//=============================================================================
int BackgroundCategory::topologycheck(const LHCb::Particle* topmother)
//This returns the sum of the PIDs of all the daughters of the reconstructed particle
//that was originally passed to The Cat, except photons (we never reconstruct neutrinos anyway)
{

  int sumofpids = 0;

  ParticleVector particles_in_decay = m_particleDescendants->descendants(topmother);

  if (msgLevel(MSG::VERBOSE)) verbose() << "Mother is a "
                                        << topmother->particleID().pid()
                                        << endmsg;

  for ( ParticleVector::const_iterator iP = particles_in_decay.begin();
        iP != particles_in_decay.end(); ++iP )
  {

    if (msgLevel(MSG::VERBOSE)) verbose() << "Daughter is a "
                                          << (*iP)->particleID().pid()
                                          << endmsg;

    //Here we just need to protect against adding photons (again the resolved/merged pi0 issue)
    //We never reconstruct neutrions so no need to worry abut them.
    if ( (*iP) != 0 )
    {

      // merged pi0 : add the 2 unreconstructed photons to the sum
      if((*iP)->particleID().abspid() == 111 && (*iP)->isBasicParticle() )sumofpids += 44;
      // gamma->ee : do not add gamma (pid=22) but add the 2 rec'ed electrons (11*2=22)
      // -> transparent as gamma is considered to stable in the MC side (photon pid is added but not electrons !!)
      if( (*iP)->particleID().abspid() == 22 && !(*iP)->isBasicParticle() )continue;

      sumofpids += (*iP)->particleID().abspid();
      if (msgLevel(MSG::VERBOSE)) verbose() << "Sum is now "
                                            << sumofpids
                                            << endmsg;

    }

  }

  if (msgLevel(MSG::VERBOSE)) verbose() << "Final sum is " << sumofpids << endmsg;

  return sumofpids;
}
//=============================================================================
BackgroundCategory::MCParticleVector
BackgroundCategory::create_finalstatedaughterarray_for_mcmother(const LHCb::MCParticle* topmother)
//Uses Patrick's tool (ParticleDescendants) to create an array of the final state products of the MCParticle
//determined to be the joint mother (if indeeed there is one) of the MCParticles associated to the final
//state particle daughters of the candidate Particle. For obvious reasons, this function is only invoked for
//background catgegories 0->50.
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "Creating an array of final state daughters for the mc mother"
                                        << endmsg;

  bool isitstable = false;
  MCParticleVector finalstateproducts;
  MCParticleVector tempfinalstateproducts;

  //Check for an MCMother with a null endvertex -- really shouldn't happen
  //here though, but the Cat is often betrayed by other pieces of code...
  const SmartRefVector<LHCb::MCVertex>& motherEndVertices = topmother->endVertices();
  if (motherEndVertices.empty() )
    Exception("The Cat found a common MC mother but it has no daughters, please report this as a bug.");
  //Assuming all went well...
  SmartRefVector<LHCb::MCVertex>::const_iterator iV = motherEndVertices.begin();

  if (msgLevel(MSG::VERBOSE)) verbose() << "Mother "
                                        << topmother->particleID().pid()
                                        << " has "
                                        <<  (*iV)->products().size()
                                        << " daughters"
                                        << endmsg;

  for ( SmartRefVector<LHCb::MCParticle>::const_iterator iP = (*iV)->products().begin();
        iP != (*iV)->products().end(); ++iP )
  {

    //Need to protect against MCParticles with null endVertices,
    //which are assumed to be stable
    const SmartRefVector<LHCb::MCVertex>& VV = (*iP)->endVertices();
    if ( !VV.empty() ) { isitstable = (*(VV.begin()))->products().empty();}
    else{ isitstable = true;}

    if (msgLevel(MSG::VERBOSE))
    {
      verbose() << (*iP)->particleID().abspid()
                << " stable: "
                << isStable( (*iP)->particleID().abspid() );

      if (isitstable) verbose() << endmsg;
      else verbose() << " products: "
                     << (*(VV.begin()))->products().size()
                     << endmsg ;
    }

    if ( isitstable ||
         isStable( (*iP)->particleID().abspid() )
         )
    {

      //If it is a stable, add it to the final state array
      finalstateproducts.push_back(*iP);

    }
    else
    {

      //Otherwise recurse
      tempfinalstateproducts = create_finalstatedaughterarray_for_mcmother(*iP);
      finalstateproducts.insert(finalstateproducts.end(),
                                tempfinalstateproducts.begin(),
                                tempfinalstateproducts.end()) ;
    }

  }

  if (msgLevel(MSG::VERBOSE)) verbose() << "returning "
                                        << finalstateproducts.size()
                                        << " final state products"
                                        << endmsg ;

  return finalstateproducts;
}
//=============================================================================
const LHCb::MCParticle*
BackgroundCategory::get_top_mother_of_MCParticle(const LHCb::MCParticle* candidate)
//Gets the 'final' non-zero mother of an MCParticle as we go up the decay tree
{

  const LHCb::MCParticle* finalmother;
  const LHCb::MCParticle* tmpmother = candidate;

  //No associated particle so no mother
  if (!candidate) return 0;

  if (msgLevel(MSG::VERBOSE)) verbose() << "About to find the final mother of a "
                                        << candidate
                                        << endmsg;

  do
  {

    //go through all mothers until you find the final one
    //if 'IgnoreQuarks' is set to true (default), then will ignore mothers with
    //pythia ID below 10 (absolute) and just break at that point returning
    //the last mother before this (this assumes quarks have no parents)  
    finalmother = tmpmother;
    tmpmother = finalmother->mother();
    if (m_ignoreQuarks && tmpmother != NULL) {
        if ( tmpmother->particleID().hasQuarks() && 
            !tmpmother->particleID().isHadron() ) {
            return finalmother;
        }
    }

  } while ( tmpmother != NULL );

  return finalmother;

}
//=============================================================================
const LHCb::MCParticle*
BackgroundCategory::get_lowest_common_mother(const MCParticleVector & mc_particles_to_compare)
//Gets the lowest common mother for an array of MCParticles
//The method is, take the first mother of the first particle, check if it is common to all
//then take the second mother of the first particle, check... etc.
{

  if (msgLevel(MSG::VERBOSE)) verbose() << "Starting search for lowest common mother (single)"
                                        << endmsg;

  bool carryon = false;
  MCParticleVector::const_iterator iMCP = mc_particles_to_compare.begin();
  const LHCb::MCParticle* tempmother = (*iMCP);
  const LHCb::MCParticle* tempmother2 = (*iMCP);

  if (msgLevel(MSG::VERBOSE)) verbose() << "The tempmother has been set to "
                                        << tempmother
                                        << " with PID "
                                        << tempmother->particleID().pid()
                                        << endmsg;

  do {

    //The code begins by getting the mother of the current MCParticle
    //begin looked at; we will then check if this can be the lowest common
    //mother of the whole tree.
    if (tempmother) tempmother = tempmother->mother();
    else Exception("Something went wrong in the MCParticle tree, please report the bug."); //shouldn't happen!

    if (!tempmother)
      Exception("Something went wrong in the MCParticle tree, please report the bug."); //shouldn't happen!

    if (msgLevel(MSG::VERBOSE)) verbose() << "The current candidate for a common mother is "
                                          << tempmother
                                          << " and has PID "
                                          << tempmother->particleID().pid()
                                          << endmsg;

    for ( iMCP = mc_particles_to_compare.begin() + 1;
          iMCP != mc_particles_to_compare.end();
          ++iMCP )
    {

      carryon = true;
      if (!(*iMCP))
        Exception("The Cat is trying to find an MC mother for a ghost, please report the bug."); //shouldn't happen!
      if (msgLevel(MSG::VERBOSE)) verbose() << "Looping on MCParticle "
                                            << (*iMCP)
                                            << " and PID "
                                            << (*iMCP)->particleID().pid()
                                            << endmsg;

      tempmother2 = (*iMCP)->mother();
      if (!tempmother2) return 0; //shouldn't happen!

      if (msgLevel(MSG::VERBOSE)) verbose() << "Looping on mother "
                                            << tempmother2 << " with PID " << tempmother2->particleID().pid() << endmsg;

      do {
        if (tempmother2 != tempmother) tempmother2 = tempmother2->mother();
        else carryon = false;
        if (tempmother2 != NULL) {
          if (msgLevel(MSG::VERBOSE)) verbose() << "Not found the original mother yet, currently on "
                                                << tempmother2
                                                << " with PID "
                                                << tempmother2->particleID().pid()
                                                << endmsg;
        }
      }while (carryon && (tempmother2 != NULL));

      if (carryon) break;

    }

  } while (carryon);

  return tempmother;
}
//=============================================================================
const LHCb::MCParticle*
BackgroundCategory::get_lowest_common_mother(const MCParticleVector & mc_particles_linked_to_decay,
                                             const ParticleVector & particles_in_decay)
//Gets the lowest common mother for an array of MCParticles, with associated particles.
//This method strips out any MCParticles associated to non-stable particles before
//finding the common mother
{

  if (msgLevel(MSG::VERBOSE)) verbose() << "Starting search for lowest common mother"
                                        << endmsg;

  MCParticleVector::const_iterator iMCP = mc_particles_linked_to_decay.begin();
  ParticleVector::const_iterator iPP = particles_in_decay.begin();

  MCParticleVector mc_particles_to_compare;
  mc_particles_to_compare.clear();

  //First we isolate only those MC particles valid for this comparison,
  //i.e. only the ones matching to a stable final state particle
  do
  {
    if ( *iPP )
    {

      if (msgLevel(MSG::VERBOSE)) verbose() << "Looking at Particle "
                                            << (*iPP)
                                            << " with PID "
                                            << (*iPP)->particleID().abspid()
                                            << endmsg;

      if ( isStable((*iPP)->particleID().abspid()) ||
           (*iPP)->isBasicParticle()
           )
      {
        //it is a stable so push back its associated MCP
        if (!(*iMCP)) return 0; //stable associated to nothing, so return 0
        mc_particles_to_compare.push_back(*iMCP);

        if (msgLevel(MSG::VERBOSE)) verbose() << "Pushed back MCParticle "
                                              << (*iMCP)
                                              << " with PID "
                                              << (*iMCP)->particleID().abspid()
                                              << endmsg;
      }
      ++iMCP; ++iPP;
    } else Exception("Your reconstructed particle had a null daughter, please report the bug."); //something went wrong

  } while ( iMCP != mc_particles_linked_to_decay.end() &&
            iPP != particles_in_decay.end()            );

  return get_lowest_common_mother(mc_particles_to_compare);
}
//=============================================================================
bool BackgroundCategory::doAllFinalStateParticlesHaveACommonMother(const MCParticleVector & mc_mothers_final,
                                                                   const MCParticleVector & mc_particles_linked_to_decay,
                                                                   const ParticleVector & particles_in_decay)
//This condition checks that all the MCParticles associated to the final-state daughters of our
//candidate Particle have a common MCParticle mother.
{
  MCParticleVector::const_iterator iP = mc_mothers_final.begin();
  MCParticleVector::const_iterator iMCP = mc_particles_linked_to_decay.begin();
  ParticleVector::const_iterator iPP = particles_in_decay.begin();

  bool carryon = true;
  bool motherassignedyet = false;
  const LHCb::MCParticle* tempmother = NULL;

  //Begin DEBUG printouts of the finalstate MC daughters and mothers
  if (msgLevel(MSG::VERBOSE)) {

    verbose() << "####################################################################"
              << endmsg;
    verbose() << "Beginning printout of the final state mothers"
              << endmsg;
    verbose() << "********************************************************************"
              << endmsg;

    for (iP = mc_mothers_final.begin(); iP != mc_mothers_final.end(); ++iP) {
      verbose() << "Mother = "
                << (*iP)
                << endmsg;
      if (*iP){
        verbose() << "PID of mother = "
                  << (*iP)->particleID().pid()
                  << endmsg;
      }
    }

    verbose() << "********************************************************************"
              << endmsg;
    verbose() << "**********************************************"
              << endmsg;
    verbose() << "Beginning printout of the reconstructed particles"
              << endmsg;

    for (iPP = particles_in_decay.begin(); iPP != particles_in_decay.end(); ++iPP) {
      verbose() << "Reconstructed particle = "
                << (*iPP)
                << endmsg;
      if (*iPP) {
        verbose() << "PID of reconstructed particle = "
                  << (*iPP)->particleID().pid()
                  << endmsg;
      }
    }

    verbose() << "********************************************************************"
              << endmsg;
    verbose() << "**********************************************"
              << endmsg;
    verbose() << "Beginning printout of the associated particles"
              << endmsg;

    for (iMCP = mc_particles_linked_to_decay.begin(); iMCP != mc_particles_linked_to_decay.end(); ++iMCP) {
      verbose() << "Associated particle = "
                << (*iMCP)
                << endmsg;
      if (*iMCP) {
        verbose() << "PID of associated particle = "
                  << (*iMCP)->particleID().pid()
                  << endmsg;
      }
    }
    verbose() << "####################################################################"
              << endmsg;
  }
  //End DEBUG

  //Reset the positions of the counters after the debug
  iP = mc_mothers_final.begin();
  iPP = particles_in_decay.begin();

  //This first loop finds if any common mother exists for the MCParticles associated
  //to the stable daughters of the Particle whose category you are testing. This could in
  //principle be integrated with finding the lowest common mother but is done in two stages for
  //historical reasons. If you ignore the debug messages, the loop essentially consists of
  //looping over all the (stable) associated MCParticles and checking that their top level
  //mothers (found earlier and passed as an argument here) match.
  do {

    //Yet more debug messages...
    if (msgLevel(MSG::VERBOSE)) verbose() << "Condition A looping on "
                                          << (*iPP)
                                          << " and "
                                          << (*iP)
                                          << endmsg;

    if (*iPP) {

      if (msgLevel(MSG::VERBOSE)) verbose() << "PID is "
                                            << (*iPP)->particleID().abspid()
                                            << endmsg;

      verbose () << "Passed" << endmsg;

      //if ( (*iPP)->isBasicParticle()) { //final state particle
      if ( isStable((*iPP)->particleID().abspid()) ||
           (*iPP)->isBasicParticle() ) { //final state particle -- the other way

        verbose () << "Passed"
                   << endmsg;

        if ((*iP) == 0 ) {
          carryon = false; //final state particle has no mother - no need to go further
        } else {
          if (!motherassignedyet) {
            tempmother = *iP ;
            motherassignedyet = true;
          } else {
            verbose () << "Doing the check"
                       << endmsg;

            if ( (*iP) == tempmother ) {
              carryon = true;
            } else {
              carryon = false;
            }
          }
        }
      }
    } else carryon = false;

    if (msgLevel(MSG::VERBOSE)) verbose() << "Common mother is " << tempmother
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

    if (msgLevel(MSG::VERBOSE)) verbose() << "About to start looking for the lowest common mother." << endmsg;

    m_commonMother = get_lowest_common_mother(mc_particles_linked_to_decay,particles_in_decay);
    //m_commonMother = tempmother;

    if (!m_commonMother)
      Exception("Something has gone wrong when looking for the lowest common mother, please report the bug.");

    if (msgLevel(MSG::VERBOSE)) verbose() << "Found common mother "
                                          << m_commonMother->particleID().pid()
                                          << endmsg ;

    if ( m_commonMother->particleID().abspid() != 22 && isStable(m_commonMother->particleID().abspid())){
      if (msgLevel(MSG::VERBOSE)) verbose() << "Common mother is a stable "
                                            << m_commonMother->particleID().pid()
                                            << endmsg ;
      Warning("Common mother is stable.").ignore();
      m_commonMother = 0 ;
      carryon = false ;
    }
  } else m_commonMother = 0;

  return carryon;
}
//=============================================================================
bool BackgroundCategory::isTheDecayFullyReconstructed(const MCParticleVector & mc_particles_linked_to_decay)
//This condition checks that all the final state daughters of the MCparticle
//returned by condition A match up to the MCParticles associated to the  final state
//daughters of the candidate Particle. In effect, condition A checked whether all
//the particles used to make our B came from one decay, and this checks if there
//are any particles coming from said decay which we missed out in our reconstruction.
{
  bool carryon;
  bool isitstable = false;
  int neutrinosFound = 0;
  MCParticleVector finalstateproducts = create_finalstatedaughterarray_for_mcmother(m_commonMother);
  MCParticleVector::const_iterator iPP = finalstateproducts.begin();
  if (finalstateproducts.empty() ) Exception("Condition B : No final states, please report the bug.");
  MCParticleVector::const_iterator iP = mc_particles_linked_to_decay.begin();

  //Because of the special case of the merged pi0, the only non stable particle made without any
  //daughters, we need to make a dummy array here.
  //We will copy out mc_particles_linked_to_decay into it, replacing any pi0
  //particles by their daughters.
  if (msgLevel(MSG::VERBOSE)) verbose() << "About to create a temporary array to deal with the merged pi0 case"
                                        << endmsg;
  MCParticleVector mc_particles_linked_to_decay_without_pi0;
  MCParticleVector merged_pi0_daughters;
  mc_particles_linked_to_decay_without_pi0.clear();
  for (iP = mc_particles_linked_to_decay.begin(); iP != mc_particles_linked_to_decay.end(); ++iP) {
    merged_pi0_daughters.clear();
    if ((*iP) == NULL) {
      mc_particles_linked_to_decay_without_pi0.push_back(*iP);
      continue;
    }
    if (msgLevel(MSG::VERBOSE)) verbose() << "PID of associated particle = "
                                          << (*iP)->particleID().pid()
                                          << endmsg;
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
  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "####################################################################"
              << endmsg;
    verbose() << "Beginning printout of the final state daughters of the common mother"
              << endmsg;
    verbose() << "********************************************************************"
              << endmsg;
    for (iPP = finalstateproducts.begin(); iPP != finalstateproducts.end(); ++iPP) {
      verbose() << "Daughter of common mother = "
                << (*iPP)
                << endmsg;
      if (*iPP){
        verbose() << "PID of daughter = "
                  << (*iPP)->particleID().pid()
                  << endmsg;
      }
    }
    verbose() << "********************************************************************"
              << endmsg;
    verbose() << "**********************************************"
              << endmsg;
    verbose() << "Beginning printout of the associated particles"
              << endmsg;

    for (iP = mc_particles_linked_to_decay_without_pi0.begin();
         iP != mc_particles_linked_to_decay_without_pi0.end();
         ++iP) {
      verbose() << "Associated particle = "
                << (*iP)
                << endmsg;
      if (*iP) {
        verbose() << "PID of associated particle = "
                  << (*iP)->particleID().pid()
                  << endmsg;
      }
    }
    verbose() << "**********************************************"
              << endmsg;
  }

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

        if (msgLevel(MSG::VERBOSE)) verbose() << "Photon is being ignored!!"
                                              << endmsg;

        carryon = true;

      }

      if (msgLevel(MSG::VERBOSE)) verbose() << "The MC final state particle has pid : "
                                            << (*iPP)->particleID().pid()
                                            << endmsg;

      if (!carryon) { //if the particle is not to be ignored
        for (iP = mc_particles_linked_to_decay_without_pi0.begin();
             iP != mc_particles_linked_to_decay_without_pi0.end(); ++iP) {
          if ( *iP == 0) continue; //if no matching MCParticle
          if ( m_useSoftPhotonCut &&
               (*iP)->particleID().abspid() == 22 &&
               (*iP)->momentum().e() < m_softPhotonCut ) continue; //soft photons are ignored
          if (msgLevel(MSG::VERBOSE)) verbose() << "The MC-associated particle has pid : "
                                                << (*iP)->particleID().pid()
                                                << endmsg;
          const SmartRefVector<LHCb::MCVertex>& VV = (*iP)->endVertices();
          if (VV.size() != 0) isitstable = (*(VV.begin()))->products().empty();
          else isitstable = true;
          if ( isitstable || isStable( (*iP)->particleID().abspid() )  ) {
            if (msgLevel(MSG::VERBOSE)) verbose() << "Associated Particle:"
                                                  << (*iP)
                                                  << endmsg;
            if (msgLevel(MSG::VERBOSE)) verbose() << "MC-final state Particle:"
                                                  << (*iPP)
                                                  << endmsg;
            carryon = ( *iP == *iPP );
            if (carryon) break;
          }
        }
      }
    }

    ++iPP;

  } while(carryon && iPP != finalstateproducts.end());

  if (msgLevel(MSG::VERBOSE)) verbose() << "Has condition B passed or failed?" << endmsg;
  if (msgLevel(MSG::VERBOSE)) verbose() << "*********************************" << endmsg;
  if (msgLevel(MSG::VERBOSE)) verbose() << carryon << endmsg;
  if (msgLevel(MSG::VERBOSE)) verbose() << "*********************************" << endmsg;
  if (msgLevel(MSG::VERBOSE)) verbose() << "#################################" << endmsg;

  return carryon;
}
//=============================================================================
bool
BackgroundCategory::areAllFinalStateParticlesCorrectlyIdentified(const ParticleVector & particles_in_decay,
                                                                 const MCParticleVector & mc_particles_linked_to_decay)
//This condition checks if all the final state particles used to make the candidate particle are correctly
//identified (according to PID).
{
  bool carryon = true;

  MCParticleVector::const_iterator iPmc = mc_particles_linked_to_decay.begin();
  ParticleVector::const_iterator iP = particles_in_decay.begin();

  do {
    if ( *iPmc && *iP) {
      if( isStable((*iP)->particleID().abspid()) || (*iP)->isBasicParticle() ){

        if( (*iP)->particleID().abspid() != 22 || m_calo2MC->isPureNeutralCalo( *iP ) ){ // gamma->ee is considered as composite

          carryon = ( (*iP)->particleID().pid() == (*iPmc)->particleID().pid() );
        }

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

  if (!m_ppSvc->find(m_commonMother->particleID())) {
    warning() << "The common mother has a particle ID,"<<m_commonMother->particleID()<<"unknown to the particle service!" << endmsg;
    warning() << "I cannot tell if this is the particle you are looking for, so am classifying as 20, unless you are\
                  looking for an inclusive decay, in which case it will be classified 40/50. " << endmsg;
    return 0;
  }

  if (m_ppSvc->find(m_commonMother->particleID())->charge() == 0)
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

  if (!m_ppSvc->find(candidate->particleID())) {
    warning() << "The reconstructed particle has a particle ID,"<<candidate->particleID()<<"unknown to the particle service!" << endmsg;
    warning() << "I cannot tell its mass, so cannot tell if your background is low mass or not; as such I am classifying it 40." << endmsg;
    return 0;
  }

  if ( (m_commonMother->virtualMass() - m_lowMassCut) <
       (m_ppSvc->find(candidate->particleID())->mass()) ) carryon = true;

  return carryon;
}
//=============================================================================
bool BackgroundCategory::areAnyFinalStateParticlesGhosts(const MCParticleVector & mc_particles_linked_to_decay,
                                                         const ParticleVector & particles_in_decay)
//Checks if there are any ghosts in our final state Particles used to make the candidate Particle
{
  bool carryon = true;
  MCParticleVector::const_iterator iP = mc_particles_linked_to_decay.begin();
  ParticleVector::const_iterator iPP = particles_in_decay.begin();

  do {

    if (
        (*iP) == 0 &&
        ( isStable((*iPP)->particleID().abspid()) || (*iPP)->isBasicParticle()  )
        ) {
      carryon = false;
      // info() <<"======== IT IS A GHOST ======= " << (*iPP)->particleID().abspid() << endmsg;
    }
    ++iP;
    ++iPP;

  } while (carryon && iP != mc_particles_linked_to_decay.end() && iPP != particles_in_decay.end() );

  return !carryon;

}
//=============================================================================
bool BackgroundCategory::isThisAPileup(const MCParticleVector & mc_particles_linked_to_decay,
                                       const ParticleVector & particles_in_decay)
//Checks if the event is a pileup (final state Particles come from at least 2 different collisions) or not.
{
  bool carryon = true;
  MCParticleVector::const_iterator iP = mc_particles_linked_to_decay.begin();
  ParticleVector::const_iterator iPP = particles_in_decay.begin();
  const LHCb::MCVertex* tmpcollision = NULL;
  bool gotacollision = false;

  do {

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
bool BackgroundCategory::isThisBBarBackground(const MCParticleVector & mc_mothers_final)
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
bool BackgroundCategory::isThisCCbarBackground(const MCParticleVector & mc_mothers_final)
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
int BackgroundCategory::areAnyFinalStateParticlesFromAPrimaryVertex(const MCParticleVector & mc_particles_linked_to_decay)
{
  //This function evaluates whether some of the particles in the final state
  //of the candidate come from the primary vertex. Returns 0 if none, 1 if one,
  //and 99 if all.
  int howmanyfinalstate = 0;
  int howmanyfromPV = 0;

  for (MCParticleVector::const_iterator iPP = mc_particles_linked_to_decay.begin();
       iPP != mc_particles_linked_to_decay.end(); ++iPP) {

    if (*iPP) {
      SmartRefVector<LHCb::MCVertex> VV = (*iPP)->endVertices();
      if (VV.size() != 0) {
        SmartRefVector<LHCb::MCVertex>::const_iterator iVV = VV.begin();
        if (*iVV) {
          if ( (*iVV)->products().empty() || isStable( (*iPP)->particleID().abspid()) ) {
            ++howmanyfinalstate;
            //Need to check if the origin vertex is the primary or is indistinguishable from
            //it (for short lived resonances).
            if ( (*iPP)->originVertex()->isPrimary() ||
                 ( (*iPP)->primaryVertex()->position() -
                   (*iPP)->originVertex()->position()
                   ).Mag2() < pow(m_rescut,2)
                 ) {
              ++howmanyfromPV;
            }
          }
        }
      } else {
        if ( (*iPP)->originVertex()->isPrimary() ||
             ( (*iPP)->primaryVertex()->position() -
               (*iPP)->originVertex()->position()
               ).Mag2() < pow(m_rescut,2)
             ) {
          ++howmanyfromPV;
        }
      }
    }
  }

  if (howmanyfromPV > 0) {
    if (howmanyfromPV == howmanyfinalstate) {
      return 99;
    } else return 1;
  } else return 0;
}
//=============================================================================
BackgroundCategory::MCParticleVector
BackgroundCategory::associate_particles_in_decay(const ParticleVector & particles_in_decay,
                                                 const LHCb::Particle* reconstructed_mother)
//Associates all the final state particles daughters of the candidate Particle.
//Ignores composites. The reconstructed mother is used to check the neutral association.
{
  MCParticleVector associated_mcparts;
  ParticleVector::const_iterator iP;
  MCParticleVector::iterator iPP;
  DaughterAndPartnerPair temp_pair;
  DaughterAndPartnerVector tempDaughtersAndPartners;

  bool associating_a_neutral = false;
  bool found_neutral_mother = false;

  for (iP = particles_in_decay.begin() ; iP != particles_in_decay.end() ; ++iP){

    if ((*iP) == NULL) { //No particle, null association
      associated_mcparts.push_back(NULL);
      continue;
    }

    if (msgLevel(MSG::VERBOSE)) verbose() << "About to check if we are dealing with a basic particle"
                                          << endmsg;
    if (msgLevel(MSG::VERBOSE)) verbose() << "Particle PID is "
                                          << (*iP)->particleID().pid()
                                          << endmsg;

    // ======  pure neutral calorimetric *basic* particle : gamma & pi0-merged (pi0-resolved is treated as a composite particle)
    if( (*iP)->isBasicParticle() && m_calo2MC->isPureNeutralCalo( *iP ) && !m_vetoN){
      if(NULL == m_calo2MC)
        Exception("Something failed when making the calo->MC associators. Bye!");
      // associating neutral is done here :

      if (m_calo2MC->from(*iP)) {
        const LHCb::MCParticle* mcp = m_calo2MC->from(*iP)->findMCOrBest( (*iP)->particleID() , m_caloWeight );
        if (mcp) {
          verbose() << "Neutral Calo MC associated : PID = " << mcp->particleID().pid()
                    << " | quality  : " << m_calo2MC->quality(mcp) << endmsg;
        }
        associated_mcparts.push_back( mcp );
      } else {
        associated_mcparts.push_back( NULL );
      }

      // and should be removed below ...
    }
    // ====== other basic or stables particles (Warning : including gamma->ee as gamma is considered as stable)
    else if ( isStable((*iP)->particleID().abspid()) || (*iP)->isBasicParticle() )  {

      //Look at the full range of associated particles
      const LHCb::MCParticle* mc_correctPID = NULL;
      const LHCb::MCParticle* mc_bestQ = NULL;
      const LHCb::MCParticle* mc_TempDeux = NULL;
      double minimumweight = m_minWeight;
      double mc_weight = 0.;
      double mc_correctPID_weight = minimumweight;

      Particle2MCParticle::ToVector mcPartRange;

      if (msgLevel(MSG::VERBOSE)) verbose() << "About to get the array of matching particles"
                                            << endmsg;

      // ===== gamma->ee
      if ( (*iP)->particleID().pid() == 22 && !m_vetoN) {
        const LHCb::MCParticle* mm = this->origin(*iP);
        if(mm)mcPartRange.push_back( MCAssociation(mm,1.) );
        associating_a_neutral = true;
        //info() << " ============ " << mm << "===============" << endmsg;
        //if( mm ) info()<< mm->particleID().pid() << endmsg;
      } else {
        mcPartRange = m_smartAssociator->relatedMCPs(*iP);
        associating_a_neutral = false;
      }

      if (mcPartRange.size() == 0) { //ghost
        associated_mcparts.push_back(NULL);
        continue;
      }

      if (msgLevel(MSG::VERBOSE)) verbose() << "Got the array of matching particles OK!"
                                            << endmsg;

      Particle2MCParticle::ToVector::const_iterator mcPartIt;

      int looper = 1;

      for (mcPartIt = mcPartRange.begin(); mcPartIt!=mcPartRange.end(); ++mcPartIt) {

        if (msgLevel(MSG::VERBOSE)) verbose() << "About to get the "
                                              << looper
                                              << "th match"
                                              << endmsg;
        mc_TempDeux = (*mcPartIt).to();

        //Lots of debug messages...
        if (msgLevel(MSG::VERBOSE)) verbose() << "MCParticle is at "
                                              << mc_TempDeux
                                              << endmsg;
        if (msgLevel(MSG::VERBOSE)) verbose() << "MCParticle has PID = "
                                              << mc_TempDeux->particleID().pid()
                                              << endmsg;
        if (msgLevel(MSG::VERBOSE)) verbose() << "MCParticle energy = "
                                              << mc_TempDeux->momentum().E()
                                              << endmsg;
        if (msgLevel(MSG::VERBOSE)) verbose() << "Range pt = "
                                              << mc_TempDeux->momentum().Pt()
                                              << endmsg;
        if (msgLevel(MSG::VERBOSE)) verbose() << "Range momentum = "
                                              << mc_TempDeux->momentum().P()
                                              << endmsg;
        if (msgLevel(MSG::VERBOSE)) verbose() << "Range weight = "
                                              << (*mcPartIt).weight()
                                              << endmsg;

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
            (( reconstructed_mother->charge() != 0 && (reconstructed_mother->particleID().pid() ==
                                                       mc_TempDeux->particleID().pid())
               ) ||
             ( reconstructed_mother->charge() == 0 && (reconstructed_mother->particleID().abspid() ==
                                                       mc_TempDeux->particleID().abspid())
               )
             )
            ) {
          found_neutral_mother = true;
        }
        looper++;
      }
      if (mc_correctPID) {
        if ( mc_correctPID == mc_bestQ ) {
          if (msgLevel(MSG::VERBOSE)) verbose() << "Pushing back best match"
                                                << endmsg;
          if (msgLevel(MSG::VERBOSE)) verbose() << "Best PID weight = "
                                                << minimumweight
                                                << endmsg;
          associated_mcparts.push_back(mc_bestQ);
        } else {
          if ((mc_correctPID_weight >= m_override) || (associating_a_neutral && found_neutral_mother)) {
            if (msgLevel(MSG::VERBOSE)) verbose() << "Pushing back best pid match"
                                                  << endmsg;
            if (msgLevel(MSG::VERBOSE)) verbose() << "Best PID weight = "
                                                  << mc_correctPID_weight
                                                  << endmsg;
            associated_mcparts.push_back(mc_correctPID);
          } else {
            if (msgLevel(MSG::VERBOSE)) verbose() << "OVERRIDE! Pushing back best Q match"
                                                  << endmsg;
            if (msgLevel(MSG::VERBOSE)) verbose() << "Best Q match has PID = "
                                                  << mc_bestQ->particleID().pid()
                                                  << endmsg;
            if (msgLevel(MSG::VERBOSE)) verbose() << "Best PID weight = "
                                                  << mc_correctPID_weight
                                                  << endmsg;
            if (msgLevel(MSG::VERBOSE)) verbose() << "Best Q = "
                                                  << minimumweight
                                                  << endmsg;
            associated_mcparts.push_back(mc_bestQ);
          }
        }
      } else if (mc_bestQ) {
        //No match with correct PID, we just match the best quality one there is
        if (msgLevel(MSG::VERBOSE)) verbose() << "Pushing back best Q match"
                                              << endmsg;
        if (msgLevel(MSG::VERBOSE)) verbose() << "Best Q match has PID = "
                                              << mc_bestQ->particleID().pid()
                                              << endmsg;
        if (msgLevel(MSG::VERBOSE)) verbose() << "Best Q = "
                                              << minimumweight
                                              << endmsg;
        associated_mcparts.push_back(mc_bestQ);
      } else associated_mcparts.push_back(NULL); //ghost

    } else {//Just ignore composites
      associated_mcparts.push_back(NULL);
    }
  }

  //Now write the array of ''daughter--associated partner'' pairs
  iPP = associated_mcparts.begin();
  for (iP = particles_in_decay.begin() ; iP != particles_in_decay.end() ; ++iP)
  {
    temp_pair = DaughterAndPartnerPair(*iP,*iPP);
    tempDaughtersAndPartners.push_back(temp_pair);
    ++iPP;
  }

  m_daughtersAndPartners = tempDaughtersAndPartners;

  DaughterAndPartnerVector::const_iterator iDAP;

  for ( iDAP = m_daughtersAndPartners.begin(); iDAP != m_daughtersAndPartners.end(); ++iDAP )
  {

    if (msgLevel(MSG::VERBOSE)) {
      verbose() << "Reconstructed particle has PID "
                << ((*iDAP).first)->particleID().pid()
                << endmsg;
      if ((*iDAP).second != NULL) verbose() << "Associated particle has PID "
                                            << ((*iDAP).second)->particleID().pid()
                                            << endmsg;
      else verbose() << "There is no associated particle"
                     << endmsg;
    }

  }

  return associated_mcparts;
}
//=============================================================================
bool BackgroundCategory::foundClones(const MCParticleVector & mc_particles_linked_to_decay) {
  //Checks if two particles were linked to the same MCParticle in which case this is a
  //clone background. Shouldn't happen for charged particles but could happen for neutrals

  //Do this as a simple double for loop, I realize it is not the most efficient
  //way but I can't be bothered
  for (MCParticleVector::const_iterator iPP1 = mc_particles_linked_to_decay.begin();
       iPP1 != (mc_particles_linked_to_decay.end()-1); ++iPP1) {
    if (!(*iPP1)) continue; //We already checked for ghosts any null associations
                            //here are intermediates and we don't care about them
    for (MCParticleVector::const_iterator iPP2 = (iPP1+1);
         iPP2 != mc_particles_linked_to_decay.end(); ++iPP2) {
      if (!(*iPP2)) continue; //As above
      if ((*iPP1) == (*iPP2)) return true;
    }
  }
  return false;

}
//=============================================================================
bool BackgroundCategory::hierarchyProblem(const MCParticleVector & mc_particles_linked_to_decay) {
  //Check that there is no overlap between the associated MCParticles. We have
  //already checked that none of the final state particles are ghosts or clones,
  //but now we need to check that we didn't combine a particle with its own parent.

  //We need to take each particle, save its mothers, then check that they
  //don't match any of the other MCParticles
  MCParticleVector tempmothers;
  tempmothers.clear();
  const LHCb::MCParticle* tempmother = 0;

  for (MCParticleVector::const_iterator iPP1 = mc_particles_linked_to_decay.begin();
       iPP1 != (mc_particles_linked_to_decay.end()); ++iPP1) {

    if (!(*iPP1)) continue;
    //First make the array of mothers
    tempmother = (*iPP1)->mother();
    if (tempmother)
      do {
        tempmothers.push_back(tempmother);
        tempmother = tempmother->mother();
      } while (tempmother);

    //Now check against all the other particles
    for (MCParticleVector::const_iterator iPP2 = mc_particles_linked_to_decay.begin();
         iPP2 != (mc_particles_linked_to_decay.end()); ++iPP2) {

      if (!(*iPP2) || (iPP1 == iPP2)) continue;
      //Check all the mothers
      for (MCParticleVector::const_iterator iPPM = tempmothers.begin();
           iPPM != (tempmothers.end()); ++iPPM) {
        if (!(*iPPM)) break; //Shoudln't happen...
        if ((*iPPM) == (*iPP2) && (*iPPM)->particleID().pid() != 22) return true;  // Warning gamma->ee is a 'stable' composite !
      }

    }
    tempmothers.clear();

  }
  return false;

}

//=============================================================================
StatusCode BackgroundCategory::initialize()
{
  //Initialize and get the required tools
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  if (msgLevel(MSG::VERBOSE))
    verbose() << "Starting to initialise Background Categorisation"
              << endmsg;

  sc = service("LHCb::ParticlePropertySvc", m_ppSvc);
  if ( sc.isFailure() ) return sc;

  m_particleDescendants = tool<IParticleDescendants>("ParticleDescendants",this);
  m_smartAssociator = tool<IParticle2MCWeightedAssociator>("P2MCPFromProtoP",this);
  // calo->MC tool
  m_calo2MC = tool<ICalo2MCTool>("Calo2MCTool",this);

  if (msgLevel(MSG::VERBOSE)) m_printDecay = tool<IPrintDecay>("PrintDecayTreeTool",this);

  if (msgLevel(MSG::VERBOSE)) verbose() << "Done initializing"
                                        << endmsg ;

  return sc;
}
//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( BackgroundCategory )

//=============================================================================
