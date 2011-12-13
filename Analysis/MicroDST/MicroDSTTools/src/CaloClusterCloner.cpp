// $Id: CaloClusterCloner.cpp,v 1.1 2010-08-13 14:33:57 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from LHCb
#include "Event/CaloCluster.h"
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"

// local
#include "CaloClusterCloner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloClusterCloner
//
// 2008-04-01 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloClusterCloner::CaloClusterCloner( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : base_class ( type, name , parent ) { }

//=============================================================================

LHCb::CaloCluster* CaloClusterCloner::operator() (const LHCb::CaloCluster* cluster)
{
  return this->clone(cluster);
}

//=============================================================================

LHCb::CaloCluster* CaloClusterCloner::clone(const LHCb::CaloCluster* cluster)
{
  LHCb::CaloCluster* clone = 
    cloneKeyedContainerItem<BasicCaloClusterCloner>(cluster);

  const std::vector<LHCb::CaloClusterEntry> & entries = cluster->entries();

  if (!entries.empty()) 
  {
    std::vector<LHCb::CaloClusterEntry> clonedEntries;
    for ( std::vector<LHCb::CaloClusterEntry>::const_iterator iCalo = entries.begin();
          iCalo != entries.end(); ++iCalo ) 
    {
      const LHCb::CaloDigit* digitClone = cloneKeyedContainerItem<BasicCaloDigitCloner>((*iCalo).digit());
      LHCb::CaloClusterEntry entryClone = (*iCalo);
      entryClone.setDigit(digitClone);
      clonedEntries.push_back(entryClone);
    }
    clone->setEntries( clonedEntries );
  }

  return clone;  
}

//=============================================================================
// Destructor
//=============================================================================
CaloClusterCloner::~CaloClusterCloner() {} 

//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloClusterCloner )
