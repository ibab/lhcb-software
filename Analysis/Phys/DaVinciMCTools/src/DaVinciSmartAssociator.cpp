// $Id: DaVinciSmartAssociator.cpp,v 1.16 2010-01-13 13:05:02 cattanem Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "DaVinciSmartAssociator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DaVinciSmartAssociator
//
// 2009-01-13 : Vladimir Gligorov
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( DaVinciSmartAssociator )

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DaVinciSmartAssociator::DaVinciSmartAssociator( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : Particle2MCAssociatorBase( type, name, parent ),
    m_weightedAssociation ( NULL ),
    m_bkg                 ( NULL ),
    m_calo2MC             ( NULL )
{
  declareProperty("Calo2MCWeight",m_caloWeight=0.5);
  declareProperty("RedoNeutral",m_redoNeutral=true);
}

//=============================================================================
// Destructor
//=============================================================================
DaVinciSmartAssociator::~DaVinciSmartAssociator() {}

//=============================================================================
// Make & return the linker
//=============================================================================
Particle2MCParticle::ToVector
DaVinciSmartAssociator::relatedMCPsImpl(const LHCb::Particle* particle,
                                        const LHCb::MCParticle::ConstVector& mcps) const
{
  // We associate according to the particle type: protoparticle associators
  // are used for neutral and charged stable tracks, otherwise we use BackCat
  // for composites. The associator wrapper makes sure the linkers thus created are
  // deleted in the correct manner.

  if (!particle) Exception("The smart associator was asked to associate a NULL particle, exiting.");

  Particle2MCParticle::ToVector associatedParts;

  if (msgLevel(MSG::VERBOSE)) verbose() << "Performing smart association on "
                                        << *particle
                                        << endmsg;

  // Now we get the association result based on the particle type

  if ( m_calo2MC->isPureNeutralCalo( particle ) && m_redoNeutral)
  {
    // pure neutral calo object (stable like gamma/mergedPi0 or composite like eta/resolvedPi0/Ks->pi0pi0/...)
    if (msgLevel(MSG::VERBOSE)) verbose() << "Associating a calorimetric particle with pid = "
                                          << particle->particleID().pid() << " "
                                          << m_calo2MC->from(particle)->descriptor()
                                          << endmsg;

    associatedParts.push_back(MCAssociation(m_calo2MC->from(particle)->findMCOrBest(particle->particleID(),
                                                                                    m_caloWeight),1));

  }
  else if ( particle->isBasicParticle() )
  { // if this is a stable
    if (msgLevel(MSG::VERBOSE)) verbose() << "Associating a basic particle with pid = "
                                          << particle->particleID().pid()
                                          << endmsg;

    associatedParts = m_weightedAssociation->relatedMCPs(particle,mcps);

    if (msgLevel(MSG::VERBOSE)) verbose() << "Associated a basic particle with pid = "
                                          << particle->particleID().pid()
                                          << endmsg;
  } 
  else
  { //If composite use BackCat
    if (msgLevel(MSG::VERBOSE)) verbose() << "Associating a composite particle with pid = "
                                          << particle->particleID().pid()
                                          << endmsg;
    if ( m_bkg )
    {
      associatedParts.push_back( MCAssociation(m_bkg->origin(particle),1.) );
    } 
    else
    {
      Exception("The BackCat tool could not be found -> exiting.");
    }
    if (msgLevel(MSG::VERBOSE)) verbose() << "Associated a composite particle with pid = "
                                          << particle->particleID().pid()
                                          << endmsg;
  }

  // check if the associaited MCPs are in the input container, if not,
  // remove the association!
  return Particle2MCParticle::FilterMCAssociations(associatedParts,mcps);
}

//=============================================================================
// initialize
//=============================================================================
StatusCode DaVinciSmartAssociator::initialize()
{
  const StatusCode sc = Particle2MCAssociatorBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Init the BackCat instance
  m_bkg = tool<IBackgroundCategory>( "BackgroundCategory", this );

  // And the P2MCPFromProtoP instance that will do the associating
  // of charged and neutral "stables"
  m_weightedAssociation = tool<IParticle2MCWeightedAssociator>("P2MCPFromProtoP",this);

  // calo->MC tool
  m_calo2MC = tool<ICalo2MCTool>("Calo2MCTool",this);

  // return
  return sc;
}

//=============================================================================
