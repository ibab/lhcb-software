// $Id: FlavourMonitorAlgorithm.cpp,v 1.6 2002-10-21 13:24:36 gcorti Exp $
// Include files 
#include <algorithm>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IToolSvc.h"

#include "Event/EventHeader.h"
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "Event/FlavourTag.h"

// local
#include "FlavourMonitorAlgorithm.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FlavourMonitorAlgorithm
//
// 2002-07-15 : Olivier Dormond
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<FlavourMonitorAlgorithm>          s_factory ;
const        IAlgFactory& FlavourMonitorAlgorithmFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FlavourMonitorAlgorithm::FlavourMonitorAlgorithm( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator ), m_tags_locations(0), m_fractions(0),
    m_n_good(2), m_n_wrong(2), m_n_untagged(2), m_n_fractions(0), m_n_noB(0),
    m_nameMCAsct(), m_pAsctLinks(0)
{
  m_tags_locations.clear();
  m_tags_locations.push_back( FlavourTagLocation::User );
  declareProperty("TagsLocations", m_tags_locations);
  declareProperty("MCAssociator",  m_nameMCAsct = "Particle2MCAsct");
  declareProperty("StatNPartial", m_fractions );
}

//=============================================================================
// Destructor
//=============================================================================
FlavourMonitorAlgorithm::~FlavourMonitorAlgorithm() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode FlavourMonitorAlgorithm::initialize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;

  StatusCode sc = toolSvc()->retrieveTool( m_nameMCAsct, 
                                           "LinkAsct", m_pAsctLinks);
  if( sc.isFailure() || 0 == m_pAsctLinks) {
    log << MSG::FATAL << "    Unable to retrieve Link Associator tool" 
        << endreq;
    return sc;
  }

  m_n_good.resize(m_tags_locations.size());
  m_n_wrong.resize(m_tags_locations.size());
  m_n_untagged.resize(m_tags_locations.size());
  m_n_noB.resize(m_tags_locations.size());
  m_n_fractions.resize(m_tags_locations.size());

  for( unsigned int i=0; i<m_tags_locations.size(); i++ )
  {
    m_n_good[i].resize(m_fractions+2);
    m_n_wrong[i].resize(m_fractions+2);
    m_n_untagged[i].resize(m_fractions+2);
    m_n_fractions[i].resize(m_fractions+2);
    m_n_noB[i] = 0;
    for( unsigned int f=0; f<m_fractions+2; f++ )
      m_n_good[i][f] = m_n_wrong[i][f] = m_n_untagged[i][f]
        = m_n_fractions[i][f] = 0;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode FlavourMonitorAlgorithm::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  std::vector<std::string>::const_iterator loc_i;
  unsigned int i;
  for( i=0, loc_i=m_tags_locations.begin(); loc_i!=m_tags_locations.end();
       loc_i++, i++ )
  {
    SmartDataPtr<FlavourTags> tags(eventSvc(), *loc_i);
    if( tags == 0 )
      continue;
    log << MSG::DEBUG << "Will monitor " << tags->size() << " tags." << endreq;
    FlavourTags::const_iterator tag_i;
    for( tag_i=tags->begin(); tag_i!=tags->end(); tag_i++ )
    {
      // We want to find the MC B which is best associated to this
      // reconstructed B. For that we look at the MC B from which all
      // the associted MC decay products comes from and take the one
      // appearing most often as the right one.
      
      // Build list of B's final state products.
      std::list<Particle*> prods(0);
      //prods.clear();
      std::list<Particle*> mothers(0);
      //mothers.clear();
      mothers.push_back((*tag_i)->taggedB());
      log << MSG::DEBUG << "before: mothers.size() = "
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
      log << MSG::DEBUG << "after: mothers.size() = "
          << mothers.size() << endreq;
      log << MSG::DEBUG << "prods.size() = " << prods.size() << endreq;

      // Associate them to MC particles.
      std::list<Particle*>::const_iterator p_i;
      std::list<MCParticle *> mcprods(0);
      for( p_i = prods.begin(); p_i != prods.end(); p_i++ )
      {
        MCsFromParticle mcPartsLinks = m_pAsctLinks->rangeFrom( *p_i );
        if( mcPartsLinks.empty() )
          continue;
        log << MSG::DEBUG << "No link(s) to MCParticle." << endreq;
        //Particle2MCAsct::Table::iterator l_i;
        MCsFromParticleIterator l_i;
        for( l_i=mcPartsLinks.begin(); l_i!=mcPartsLinks.end(); l_i++ )
          mcprods.push_back(l_i->to());
      }
      log << MSG::DEBUG << "mcprods.size() = " << mcprods.size() << endreq;

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
      log << MSG::DEBUG << "mcBs.size() = " << mcBs.size() << endreq;

      float fraction = float(mcBs.size())/float(prods.size());
      
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
        log << MSG::DEBUG << "No B found." << endreq;
        m_n_noB[i]++;
        continue;
      }
      log << MSG::DEBUG << "B found." << endreq;
      
      fraction /= n_tot;
      fraction = 1 - fraction;
      unsigned int f = int(floor(fraction*(m_fractions+2)));
      log << MSG::DEBUG << "Association fraction: " << 1-fraction 
          << " (" << f << ')' << endreq;
      m_n_fractions[i][f]++;
      
      FlavourTag::TagResult mctag;
      if( mcB->particleID().isMeson() )
        if( mcB->particleID().pid() > 0 )
          mctag = FlavourTag::bbar;
        else
          mctag = FlavourTag::b;
      else
        if( mcB->particleID().pid() > 0 )
          mctag = FlavourTag::b;
        else
          mctag = FlavourTag::bbar;
      if( (*tag_i)->decision() == FlavourTag::none )
        m_n_untagged[i][f]++;
      else
        if( (*tag_i)->decision() == mctag )
          m_n_good[i][f]++;
        else
          m_n_wrong[i][f]++;
    }
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FlavourMonitorAlgorithm::finalize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "==> Finalize" << std::endl;

  std::vector<std::string>::const_iterator loc_i;
  unsigned int i;
  for( i=0, loc_i=m_tags_locations.begin(); loc_i!=m_tags_locations.end();
       loc_i++, i++ )
  {
    log << "Statistics for tags in : " << *loc_i << std::endl;
    log << "==========================================================\n";
    log << "Event without associated B : " << m_n_noB[i] << std::endl;
    for( unsigned int f=0; f<m_fractions+2; f++ )
    {
      double e   = double(m_n_good[i][f]+m_n_wrong[i][f])
        / double(m_n_good[i][f]+m_n_wrong[i][f]+m_n_untagged[i][f]);
      double w   = double(m_n_wrong[i][f])
        / double(m_n_good[i][f]+m_n_wrong[i][f]);
      double eff = e*pow(1-2*w,2);
      double se   = sqrt(e*(1-e)
                         / (m_n_good[i][f]+m_n_wrong[i][f]+m_n_untagged[i][f]));
      double sw   = sqrt(w*(1-w)/(m_n_good[i][f]+m_n_wrong[i][f]));
      double seff = sqrt(eff/(m_n_good[i][f]+m_n_wrong[i][f]+m_n_untagged[i][f])
                         *(4-eff*(1+3/eff)));
      log << "    B reconstructed at " << (1-f/(m_fractions+2.))*100 << "% : "
          << m_n_fractions[i][f] << std::endl;
      log << "----------------------------------------------------------\n";
      log << "Efficiency           : " << e << " ±" << se << std::endl;
      log << "Wrong-tag fraction   : " << w << " ±" << sw << std::endl;
      log << "Effective efficiency : " << eff << " ±" << seff << std::endl;
    }
  }
  log << endreq;
  return StatusCode::SUCCESS;
}

//=============================================================================


/*
    // Retrieve informations about event
    SmartDataPtr<EventHeader> evt(m_EventSvc, EventHeaderLocation::Default );
    
    if( !evt )
    {   
      log << MSG::ERROR
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
