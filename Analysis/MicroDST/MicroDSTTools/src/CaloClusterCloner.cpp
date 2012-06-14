// $Id: CaloClusterCloner.cpp,v 1.1 2010-08-13 14:33:57 jpalac Exp $

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "CaloClusterCloner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloClusterCloner
//
// 2008-04-01 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================

CaloClusterCloner::CaloClusterCloner( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : base_class ( type, name , parent )
{
  declareProperty( "CloneEntries", m_cloneEntries = false );
  //setProperty( "OutputLevel", 1 );
}

//=============================================================================

LHCb::CaloCluster* 
CaloClusterCloner::operator() (const LHCb::CaloCluster* cluster)
{
  return this->clone(cluster);
}

//=============================================================================

LHCb::CaloCluster* CaloClusterCloner::clone(const LHCb::CaloCluster* cluster)
{
  if ( !cluster )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "CaloCluster pointer is NULL !" << endmsg;
    return NULL;
  }

  if ( !cluster->parent() )
  {
    this->Warning( "Cannot clone a CaloCluster with no parent !" ).ignore();
    return NULL;
  }

  // Is this object in the veto list ?
  if ( isVetoed(cluster) ) { return const_cast<LHCb::CaloCluster*>(cluster); }

  LHCb::CaloCluster* clone =
    cloneKeyedContainerItem<BasicCaloClusterCloner>(cluster);
  if ( !clone ) return clone;

  // Clone the entries ?
  clone->entries().clear();
  if ( m_cloneEntries )
  {
    const std::vector<LHCb::CaloClusterEntry> & entries = cluster->entries();
    if ( !entries.empty() )
    {
      std::vector<LHCb::CaloClusterEntry> clonedEntries;
      for ( std::vector<LHCb::CaloClusterEntry>::const_iterator iCalo = entries.begin();
            iCalo != entries.end(); ++iCalo )
      {
        const LHCb::CaloDigit* digitClone =
          cloneKeyedContainerItem<BasicCaloDigitCloner>( (*iCalo).digit() );
        LHCb::CaloClusterEntry entryClone(*iCalo);
        entryClone.setDigit(digitClone);
        clonedEntries.push_back(entryClone);
      }
      clone->setEntries( clonedEntries );
    }
  }

  return clone;
}

//=============================================================================

CaloClusterCloner::~CaloClusterCloner() {}

//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloClusterCloner )

//=============================================================================
