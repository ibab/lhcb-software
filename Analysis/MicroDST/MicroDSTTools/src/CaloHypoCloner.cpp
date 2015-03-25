
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from LHCb
#include "Event/CaloHypo.h"
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"

// MicroDST
#include "MicroDST/ICloneCaloCluster.h"

// local
#include "CaloHypoCloner.h"

// STL
#include <sstream>

//-----------------------------------------------------------------------------
// Implementation file for class : CaloHypoCloner
//
// 2008-04-01 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================

CaloHypoCloner::CaloHypoCloner( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : base_class              ( type, name, parent  ),
    m_caloClusterCloner     ( NULL                ),
    m_caloClusterClonerName ( "CaloClusterCloner" ),
    m_mcPcloner             ( NULL )
{
  declareProperty( "CloneHypos",    m_cloneHypos    = true  );
  declareProperty( "CloneClusters", m_cloneClusters = true  );
  declareProperty( "CloneDigits",   m_cloneDigits   = false );
  declareProperty( "ICloneMCParticle",
                   m_mcpClonerName = "MCParticleCloner" );
  declareProperty( "CloneMCLinks",   m_cloneMCLinks   = false );
  //setProperty( "OutputLevel", 1 );
}

//=============================================================================

StatusCode CaloHypoCloner::initialize()
{
  const StatusCode sc = base_class::initialize();
  if ( sc.isFailure() ) return sc;

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  m_caloClusterCloner = tool<ICloneCaloCluster>( m_caloClusterClonerName,
                                                 this->parent() );

  if ( m_cloneMCLinks )
  {
    info() << "Will clone MC Links" << endmsg;
    m_mcPcloner = tool<ICloneMCParticle>( m_mcpClonerName, this->parent() );
  }
  else
  {
    info() << "Will NOT clone MC Links" << endmsg;
  }

  return sc;
}

//=============================================================================

// Method that handles various Gaudi "software events"
void CaloHypoCloner::handle ( const Incident& /* incident */ )
{
  // Only one Incident type, so skip type check
  clonedHypoList().clear();
}

//=============================================================================

LHCb::CaloHypo* CaloHypoCloner::operator() (const LHCb::CaloHypo* hypo)
{
  return this->clone(hypo);
}

//=============================================================================

LHCb::CaloHypo* CaloHypoCloner::clone(const LHCb::CaloHypo* hypo)
{
  if ( !hypo )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "CaloHypo pointer is NULL !" << endmsg;
    return NULL;
  }

  if ( !hypo->parent() )
  {
    this->Warning( "Cannot clone a CaloHypo with no parent !" ).ignore();
    return NULL;
  }

  // Is this object in the veto list ?
  if ( isVetoed(hypo) ) { return const_cast<LHCb::CaloHypo*>(hypo); }

  LHCb::CaloHypo* clone =
    cloneKeyedContainerItem<BasicCaloHypoCloner>(hypo);
  if ( !clone ) return clone;

  // Clone linked hypos
  clone->clearHypos();
  if ( m_cloneHypos )
  {
    const SmartRefVector<LHCb::CaloHypo> & hypos = hypo->hypos();
    if ( !hypos.empty() )
    {
      SmartRefVector<LHCb::CaloHypo> clonedHypos;
      for ( SmartRefVector<LHCb::CaloHypo>::const_iterator iCalo = hypos.begin();
            iCalo != hypos.end(); ++iCalo )
      {
        if ( *iCalo )
        {
          LHCb::CaloHypo * _hypo = (*this)(*iCalo);
          if ( _hypo ) { clonedHypos.push_back(_hypo); }
        }
        else
        {
          std::ostringstream mess;
          mess << "CaloHypo in '" << tesLocation(hypo)
               << "' has null hypo SmartRef -> skipping";
          Warning( mess.str(), StatusCode::SUCCESS ).ignore();
        }
      }
      clone->setHypos(clonedHypos);
    }
  }

  clone->clearDigits();
  if ( m_cloneDigits )
  {
    const SmartRefVector<LHCb::CaloDigit> & digits = hypo->digits();
    if ( !digits.empty() )
    {
      SmartRefVector<LHCb::CaloDigit> clonedDigits;
      for ( SmartRefVector<LHCb::CaloDigit>::const_iterator iCalo = digits.begin();
            iCalo != digits.end(); ++iCalo )
      {
        if ( *iCalo )
        {
          LHCb::CaloDigit * _digit =
            cloneKeyedContainerItem<BasicCaloDigitCloner>(*iCalo);
          if ( _digit ) { clonedDigits.push_back( _digit ); }
        }
        else
        {
          std::ostringstream mess;
          mess << "CaloHypo in '" << tesLocation(hypo)
               << "' has null digit SmartRef -> skipping";
          Warning( mess.str(), StatusCode::SUCCESS ).ignore();
        }
      }
      clone->setDigits(clonedDigits);
    }
  }

  clone->clearClusters();
  if ( m_cloneClusters )
  {
    const SmartRefVector<LHCb::CaloCluster> & clusters = hypo->clusters();
    if ( !clusters.empty() )
    {
      SmartRefVector<LHCb::CaloCluster> clonedClusters;
      for ( SmartRefVector<LHCb::CaloCluster>::const_iterator iCalo = clusters.begin();
            iCalo != clusters.end(); ++iCalo )
      {
        if ( *iCalo )
        {
          LHCb::CaloCluster * _cluster = (*m_caloClusterCloner)(*iCalo);
          if ( _cluster ) { clonedClusters.push_back( _cluster ); }
        }
        else
        {
          std::ostringstream mess;
          mess << "CaloHypo in '" << tesLocation(hypo)
               << "' has null cluster SmartRef -> skipping";
          Error( mess.str() ).ignore();
        }
      }
      clone->setClusters(clonedClusters);
    }
  }

  // clone MC Links
  if ( m_cloneMCLinks ) { cloneMCLinks(hypo,clone); }

  return clone;
}

//=============================================================================

void CaloHypoCloner::cloneMCLinks( const LHCb::CaloHypo* hypo,
                                   const LHCb::CaloHypo* cloneHypo )
{

  // Linker typedefs
  typedef LinkerWithKey<LHCb::MCParticle,LHCb::CaloHypo>  Linker;
  typedef LinkerTool<LHCb::CaloHypo,LHCb::MCParticle>     Asct;
  typedef Asct::DirectType                                Table;
  typedef Asct::DirectType::Range                         Range;

  // has this clone already been done
  if ( std::find( clonedHypoList().begin(),
                  clonedHypoList().end(),
                  cloneHypo ) == clonedHypoList().end() )
  {

    // location in TES of original hypo
    const std::string hypoLoc = objectLocation( hypo->parent() );

    // try and load the linker tool
    Asct linker = Asct( evtSvc(), hypoLoc );
    const Table * table = linker.direct();

    // If we found a table, try and use it
    if ( table )
    {
      // location in TES of cloned hypos
      const std::string cloneLoc = objectLocation( cloneHypo->parent() );

      // Create a new linker for the cloned tracks
      Linker clonedLinks( evtSvc(), msgSvc(), cloneLoc );

      // Loop over relations for original hypo
      for ( const auto& Rentry : table->relations(hypo) )
      {
        // get cloned MCParticle
        const LHCb::MCParticle * clonedMCP = (*m_mcPcloner)( Rentry.to() );
        if ( clonedMCP )
        {
          // if cloning worked, fill relation in linker with original weight
          clonedLinks.link( cloneHypo, clonedMCP, Rentry.weight() );
        }
      }

    }

    // save in the list
    clonedHypoList().push_back(cloneHypo);

  }

}

//=============================================================================

CaloHypoCloner::~CaloHypoCloner() {}

//=============================================================================

DECLARE_TOOL_FACTORY( CaloHypoCloner )

//=============================================================================
