// $Id: SingleParticleTaggingMonitor.cpp,v 1.3 2003-05-30 17:12:36 gcorti Exp $
// Include files 
#include <algorithm>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IToolSvc.h"

#include "Event/EventHeader.h"
#include "Event/Collision.h"
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "Event/FlavourTag.h"
#include "DaVinciMCTools/IDebugTool.h"
#include "TrAssociators/ITrReconstructible.h"

// local
#include "SingleParticleTaggingMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : SingleParticleTaggingMonitor
//
// 2002-07-15 : Olivier Dormond
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<SingleParticleTaggingMonitor>          s_factory ;
const        IAlgFactory& SingleParticleTaggingMonitorFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SingleParticleTaggingMonitor::
SingleParticleTaggingMonitor( const std::string& name,
                              ISvcLocator* pSvcLocator )
  : DVAlgorithm ( name , pSvcLocator ), m_pp(0), m_pAsctLinks(0), m_debug(0),
    m_trRecoQ(0)
{
  m_tags_locations.clear();
  m_tags_locations.push_back( FlavourTagLocation::User );
  declareProperty("BeAFilter", m_beAFilter = false);
  declareProperty("TaggerName", m_partName = "");
  declareProperty("TagsLocations", m_tags_locations);
  declareProperty("MCAssociator",  m_nameMCAsct = "Particle2MCLinksAsct");
}

//=============================================================================
// Destructor
//=============================================================================
SingleParticleTaggingMonitor::~SingleParticleTaggingMonitor() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode SingleParticleTaggingMonitor::initialize()
{
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialise" << endreq;

  StatusCode sc = loadTools();
  if(sc.isFailure()) {
    msg << MSG::ERROR << "Unable to load tools" << endreq;
    return StatusCode::FAILURE;
  }
  
  sc = toolSvc()->retrieveTool( m_nameMCAsct, "LinkAsct", m_pAsctLinks);
  if( sc.isFailure() || 0 == m_pAsctLinks) {
    msg << MSG::FATAL << "    Unable to retrieve Link Associator tool" 
        << endreq;
    return sc;
  }

  sc = toolSvc()->retrieveTool( "DebugTool", m_debug );
  if( sc.isFailure() ) {
    msg << MSG::FATAL << "Unable to retrieve Debug tool." << endreq;
    return sc;
  }

  sc = toolSvc()->retrieveTool( "TrReconstructible", m_trRecoQ );
  if( sc.isFailure() ) {
    msg << MSG::FATAL << "Unable to retrieve TrReconstructible tool." << endreq;
    return sc;
  }

  if( m_tags_locations.size() == 0 )
  {
    msg << MSG::FATAL << "Nothing to monitor!" << endreq;
    return StatusCode::FAILURE;
  }

  if( !(m_pp = ppSvc()->find(m_partName)) )
  {
    msg << MSG::FATAL << "Couldn't find the tagger particle named '"
        << m_partName << "'." << endreq;
    return StatusCode::FAILURE;
  }

  m_n_reco_B = 0;
  m_n_mc_B = 0;
  m_n_no_comp = 0;
  m_n_too_much_comps = 0;
  m_n_no_lepton = 0;
  m_n_too_much_leptons = 0;
  m_n_no_c_to_k = 0;
  m_n_too_much_c_to_ks = 0;
  m_n_velo_reco = 0;
  m_n_tt1_reco = 0;
  m_n_seed_reco = 0;
  m_n_velo_and_seed_reco = 0;
  m_n_velo_unreco = 0;
  m_n_tt1_unreco = 0;
  m_n_seed_unreco = 0;
  m_n_velo_and_seed_unreco = 0;
  m_n_not_reconstructed = 0;
  m_n_opposite_charge = 0;
  m_n_wrong_charge = 0;
  m_n_taggable = 0;
  m_n_comp_oscillated = 0;
  m_n_wrong_tagger = 0;
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode SingleParticleTaggingMonitor::execute() {

  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Execute" << endreq;

  std::vector<std::string>::const_iterator loc_i;
  unsigned int l;
  for( l=0, loc_i=m_tags_locations.begin(); loc_i!=m_tags_locations.end();
       loc_i++, l++ )
  {
    SmartDataPtr<FlavourTags> tags(eventSvc(), *loc_i);
    if( tags == 0 )
      continue;
    msg << MSG::DEBUG << "Will monitor " << tags->size() << " tags." << endreq;
    FlavourTags::const_iterator tag_i;
    for( tag_i=tags->begin(); tag_i!=tags->end(); tag_i++ )
    {
      // Set a flag to only count event with at least a tag.
      m_n_reco_B++;

      // We want to find the MC B which is best associated to this
      // reconstructed B. For that we look at the MC B from which all
      // the associted MC decay products comes from and take the one
      // appearing most often as the right one.
      
      // Build list of B's final state products.
      std::list<Particle*> prods(0);
      std::list<Particle*> mothers(0);
      mothers.push_back((*tag_i)->taggedB());
      msg << MSG::DEBUG << "before: mothers.size() = "
          << mothers.size() << endreq;
      std::list<Particle*>::iterator m_i;
      for( m_i = mothers.begin(); m_i != mothers.end(); m_i++ )
      {
        Vertex *vtx = (*m_i)->endVertex();
        if( vtx != 0 )
        {
          SmartRefVector<Particle>::const_iterator d_i;
          for( d_i=vtx->products().begin(); d_i!=vtx->products().end(); d_i++ )
            mothers.push_back(const_cast<Particle*>(d_i->target()));
        }
        else
          prods.push_back(*m_i);
      }
      msg << MSG::DEBUG << "after: mothers.size() = "
          << mothers.size() << endreq;
      msg << MSG::DEBUG << "prods.size() = " << prods.size() << endreq;

      // Associate them to MC particles.
      std::list<Particle*>::const_iterator p_i;
      std::list<MCParticle *> mcprods(0);
      for( p_i = prods.begin(); p_i != prods.end(); p_i++ )
      {
        MCsFromParticleLinks mcPartsLinks = m_pAsctLinks->rangeFrom( *p_i );
        if( mcPartsLinks.empty() )
          continue;
        msg << MSG::DEBUG << "No link(s) to MCParticle." << endreq;
        MCsFromParticleLinksIterator l_i;
        for( l_i=mcPartsLinks.begin(); l_i!=mcPartsLinks.end(); l_i++ )
          mcprods.push_back(l_i->to());
      }
      msg << MSG::DEBUG << "mcprods.size() = " << mcprods.size() << endreq;

      // Find the B it comes from.
      std::list<const MCParticle *> mcBs(0);
      std::list<MCParticle *>::const_iterator a_i;
      for( a_i = mcprods.begin(); a_i != mcprods.end(); a_i++ )
      {
        const MCParticle *mother = *a_i;
        while( (mother = mother->mother()) != 0 )
        {
          if( mother->particleID().hasBottom() )
            mcBs.push_back(mother);
        }
      }
      msg << MSG::DEBUG << "mcBs.size() = " << mcBs.size() << endreq;

      // Find the most frequent B in the list.
      const MCParticle *mcB = 0;
      unsigned int max_n = 0, n_tot = 0;
      while( !mcBs.empty() )
      {
        unsigned int n = std::count(mcBs.begin(), mcBs.end(), mcBs.front());
        if( n>max_n )
        {
          max_n = n;
          mcB = mcBs.front();
        }
        mcBs.remove( mcBs.front() );
        n_tot++;
      }

      if( mcB == 0 )
      {
        msg << MSG::DEBUG << "No B found." << endreq;
        continue;
      }
      msg << MSG::DEBUG << "B found." << endreq;

      m_n_mc_B++;

      // Now let see if this one is taggable!

      SmartDataPtr<MCParticles> mcparts(eventSvc(),MCParticleLocation::Default);
      if( !mcparts )
      {
        msg << MSG::FATAL << "Didn't found the container of the MCParticles at "
            << MCParticleLocation::Default << endreq;
        return StatusCode::FAILURE;
      }

      // Find the other(s) B in the event.
      std::vector<const MCParticle *> mc_comps;

      MCParticles::const_iterator mc_i;
      for( mc_i=mcparts->begin(); mc_i != mcparts->end(); mc_i++ )
      {
        ParticleID pid = (*mc_i)->particleID();
        // Keep only the particle with a b/bbar quark which are not our tagged B
        // We probably also need to skip the exited state. NOT DONE YET!
        if( pid.hasBottom() && (*mc_i != mcB) )
          mc_comps.push_back(*mc_i);
      }
      if( mc_comps.size() == 0 )
      {
        msg << MSG::DEBUG << "Untaggable because of no compagnion found!"
            << endreq;
        m_n_no_comp++;
        if( m_beAFilter )
          setFilterPassed(false);
        continue;
      }
      if( mc_comps.size() > 1 )
      {
        msg << MSG::DEBUG << "Untaggable because the event has "
            << mc_comps.size() << " B!" << endreq;
        m_n_too_much_comps++;
        if( m_beAFilter )
          setFilterPassed(false);
        continue;
      }

      // Now we have a reasonnable amount of B.
      // Let's study the decay of the compagnion.

      // We need only the particle of the relevant ID.
      // But we are not interested in the same informations
      // if it's a lepton or kaon.

      // The ParticleID is much more clever than the ParticleProperty!
      ParticleID tagger_pid(m_pp->jetsetID());

      msg << MSG::DEBUG << "Looking for a " << m_pp->particle()
          << " with ID " << tagger_pid.pid() << endreq;
 
      const MCParticle *mc_tagger = NULL;

      if( tagger_pid.isLepton() )
      { // The leptons

        // Check the number of lepton as direct daughters.
        unsigned int n_leptons = 0;
        SmartRefVector<MCVertex>::const_iterator iv;
        for ( iv = mc_comps[0]->endVertices().begin();
              iv != mc_comps[0]->endVertices().end(); iv++ )
        {
          SmartRefVector<MCParticle>::const_iterator idau;
          for ( idau = (*iv)->products().begin();
                idau != (*iv)->products().end(); idau++ )
          {
            if( (*idau)->particleID().abspid() == tagger_pid.abspid() )
            {
              n_leptons++;
              mc_tagger = idau->target();
            }
          }
        }
        if( n_leptons == 0 )
        {
          msg << MSG::DEBUG << "No direct lepton. Can't tag!" << endreq;
          m_n_no_lepton++;
          if( m_beAFilter )
            setFilterPassed(false);
          continue;
        }
        if( n_leptons > 1 )
        {
          msg << MSG::DEBUG << "Too much direct lepton (" << n_leptons
              << "). Can't tag!" << endreq;
          m_n_too_much_leptons++;
          if( m_beAFilter )
            setFilterPassed(false);
          continue;
        }
        
        // We have just one lepton comming directly from the B.
        // Let's see the reconstruction.
        // We simply drop to common code.
      } // End lepton
      else if( tagger_pid.abspid() == abs(ppSvc()->find("K+")->jetsetID()) )
      { // The kaon

        // Check the number of kaon as second-daughters comming from a charmed.
        // Also check in the direct daughters to warn for possible mis-tag.
        unsigned int n_1st_kaons = 0;
        unsigned int n_2nd_kaons = 0;
        SmartRefVector<MCVertex>::const_iterator iv1;
        for ( iv1 = mc_comps[0]->endVertices().begin();
              iv1 != mc_comps[0]->endVertices().end(); iv1++ )
        {
          SmartRefVector<MCParticle>::const_iterator idau1;
          for ( idau1 = (*iv1)->products().begin();
                idau1 != (*iv1)->products().end(); idau1++ )
          {
            SmartRefVector<MCVertex>::const_iterator iv2;
            for ( iv2 = (*idau1)->endVertices().begin();
                  iv2 != (*idau1)->endVertices().end(); iv2++ )
            {
              SmartRefVector<MCParticle>::const_iterator idau2;
              for ( idau2 = (*iv2)->products().begin();
                    idau2 != (*iv2)->products().end(); idau2++ )
              {
                if( (*idau2)->particleID().abspid() == tagger_pid.abspid()
                    && (*idau2)->mother()->particleID().hasCharm() )
                {
                  n_2nd_kaons++;
                  mc_tagger = idau2->target();
                }
              }
            }
            if( (*idau1)->particleID().abspid() == tagger_pid.abspid() )
              n_1st_kaons++;
          }
        }
        if( n_2nd_kaons == 0 )
        {
          msg << MSG::DEBUG
              << "No kaon from charmed daughter of the B. Can't tag!"
              << endreq;
          m_n_no_c_to_k++;
          if( m_beAFilter )
            setFilterPassed(false);
          continue;
        }
        if( n_2nd_kaons > 1 )
        {
          msg << MSG::DEBUG << "Too much kaons (" << n_2nd_kaons
              << ") from charmed daughter of the B. Can't tag!" << endreq;
          m_n_too_much_c_to_ks++;
          if( m_beAFilter )
            setFilterPassed(false);
          continue;
        }

        // We have the right number of kaons from a charmed daughter of the B.
        // Let see the reconstruction.
        // We simply drop to common code.
      } // End kaon

      msg << MSG::DEBUG << "The tagger is a " << mc_tagger->particleID().pid()
          << endreq;

      // Check the expected tagger is reconstructible.
      if( !m_trRecoQ->hasVelo(const_cast<MCParticle *>(mc_tagger)) )
      {
        msg << MSG::DEBUG << "The tagger is not reconstructible by the VELO."
            << endreq;
        m_n_velo_unreco++;
      }
      else
        m_n_velo_reco++;
      if( !m_trRecoQ->hasTT1(const_cast<MCParticle *>(mc_tagger)) )
      {
        msg << MSG::DEBUG << "The tagger is not reconstructible by the TT1."
            << endreq;
        m_n_tt1_unreco++;
      }
      else
        m_n_tt1_reco++;
      if( !m_trRecoQ->hasVelo(const_cast<MCParticle *>(mc_tagger)) )
      {
        msg << MSG::DEBUG
            << "The tagger is not reconstructible by the seeding stations."
            << endreq;
        m_n_seed_unreco++;
      }
      else
        m_n_seed_reco++;
      if( !m_trRecoQ->hasVeloAndSeed(const_cast<MCParticle *>(mc_tagger)) )
      {
        msg << MSG::DEBUG <<
       "The tagger is not reconstructible by the VELO and the seeding stations."
            << endreq;
        m_n_velo_and_seed_unreco++;
      }
      else
        m_n_velo_and_seed_reco++;

      // Check the expected tagger is reconstructed.
      ParticlesToMCLinks partsLinks = m_pAsctLinks->rangeTo( mc_tagger );
      if( partsLinks.empty() )
      {
        msg << MSG::DEBUG
            << "The expected MC tagger is not reconstructed. Can't tag!"
            << endreq;
        m_n_not_reconstructed++;
        if( m_beAFilter )
          setFilterPassed(false);
        continue;
      }
      // Check we don't have some silly reconstruction.
      ParticlesToMCLinksIterator l_i;
      ParticleVector plus_taggers;
      ParticleVector minus_taggers;
      for( l_i=partsLinks.begin(); l_i!=partsLinks.end(); l_i++ )
      {
        if( l_i->to()->particleID().threeCharge() > 0 )
          plus_taggers.push_back(l_i->to());
        else if( l_i->to()->particleID().threeCharge() < 0 )
          minus_taggers.push_back(l_i->to());
        else
          msg << MSG::DEBUG << "Charged particle reconstructed as neutral!!!!"
              << endreq;
      }
      // For instance a particle reconstructed multiple times
      // with opposite charge.
      if( plus_taggers.size() > 0 && minus_taggers.size() > 0 )
      {
        msg << MSG::DEBUG <<
          "Particle has been reconstructed multiple times with opposite charge!"
            << " Can't tag" << endreq;
        m_n_opposite_charge++;
        if( m_beAFilter )
          setFilterPassed(false);
        continue;
      }
      // Check the charge was correctly reconstructed.
      if( (mc_tagger->particleID().threeCharge() > 0
           && plus_taggers.size() == 0)
          || (mc_tagger->particleID().threeCharge() < 0
              && minus_taggers.size() == 0) )
      {
        msg << MSG::DEBUG <<
          "The charge of the particle has not been reconstructed correctly."
            << " Can't tag!" << endreq;
        m_n_wrong_charge++;
        if( m_beAFilter )
          setFilterPassed(false);
        continue;
      }
      
      // Now only the cuts might remove the tagger.
      msg << MSG::DEBUG <<
        "Tagger reconstructed successfully (still need to pass the cuts)."
          << endreq;
      m_n_taggable++;

      // Check if the compagnion has oscillated so the tag will be wrong.
      if( mcB->hasOscillated() )
        m_n_comp_oscillated++;

      // Check if the expected tagger match the one found.
      if( (*tag_i)->decision() == FlavourTag::none )
        continue;
      bool expected_tagger = false;
      bool expected_tagger_protop = false;
      ParticleVector::const_iterator t_i;
      for( t_i=plus_taggers.begin(); t_i!=plus_taggers.end(); t_i++ )
      {
        if( (*tag_i)->tagger() == (*t_i) )
          expected_tagger = true;
        if( (*tag_i)->tagger()->origin() == (*t_i)->origin() )
          expected_tagger_protop = true;        
      }
      for( t_i=minus_taggers.begin(); t_i!=minus_taggers.end(); t_i++ )
      {
        if( (*tag_i)->tagger() == (*t_i) )
          expected_tagger = true;
        if( (*tag_i)->tagger()->origin() == (*t_i)->origin() )
          expected_tagger_protop = true;        
      }
      if( !expected_tagger && !expected_tagger_protop )
      {
        msg << MSG::DEBUG << "The right tagger was missed!" << endreq;
        m_n_wrong_tagger++;
      }
    }
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode SingleParticleTaggingMonitor::finalize()
{
  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << "==> Finalize" << std::endl
      << "\t m_n_reco_B               " << m_n_reco_B << std::endl
      << "\t m_n_mc_B                 " << m_n_mc_B << std::endl
      << "\t ------------------------------" << std::endl
      << "\t m_n_no_comp              " << m_n_no_comp << std::endl
      << "\t m_n_too_much_comps       " << m_n_too_much_comps << std::endl
      << "\t m_n_no_lepton            " << m_n_no_lepton << std::endl
      << "\t m_n_too_much_leptons     " << m_n_too_much_leptons << std::endl
      << "\t m_n_no_c_to_k            " << m_n_no_c_to_k << std::endl
      << "\t m_n_too_much_c_to_ks     " << m_n_too_much_c_to_ks << std::endl
      << "\t ------------------------------" << std::endl
      << "\t m_n_velo_unreco          " << m_n_velo_unreco << std::endl
      << "\t m_n_tt1_unreco           " << m_n_tt1_unreco << std::endl
      << "\t m_n_seed_unreco          " << m_n_seed_unreco << std::endl
      << "\t m_n_velo_and_seed_unreco " << m_n_velo_and_seed_unreco << std::endl
      << "\t ------------------------------" << std::endl
      << "\t m_n_velo_reco            " << m_n_velo_reco << std::endl
      << "\t m_n_tt1_reco             " << m_n_tt1_reco << std::endl
      << "\t m_n_seed_reco            " << m_n_seed_reco << std::endl
      << "\t m_n_velo_and_seed_reco   " << m_n_velo_and_seed_reco << std::endl
      << "\t ------------------------------" << std::endl
      << "\t m_n_not_reconstructed    " << m_n_not_reconstructed << std::endl
      << "\t ------------------------------" << std::endl
      << "\t m_n_opposite_charge      " << m_n_opposite_charge << std::endl
      << "\t m_n_wrong_charge         " << m_n_wrong_charge << std::endl
      << "\t ------------------------------" << std::endl
      << "\t m_n_taggable             " << m_n_taggable << std::endl
      << "\t m_n_comp_oscillated      " << m_n_comp_oscillated << std::endl
      << "\t m_n_wrong_tagger         " << m_n_wrong_tagger << std::endl
      << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================


/*
    // Retrieve informations about event
    SmartDataPtr<EventHeader> evt(m_EventSvc, EventHeaderLocation::Default );
    
    if( !evt )
    {   
      msg << MSG::ERROR
          << "Could not retrieve event header. Will put fake !!!!" << endreq;
      m_n_run = 0;
      m_n_event = -1;
    }
    else
    {
      m_n_run = evt->runNum();
      m_n_event = evt->evtNum();
    }
*/
