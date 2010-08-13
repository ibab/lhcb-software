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

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloClusterCloner );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloClusterCloner::CaloClusterCloner( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : 
  base_class ( type, name , parent )
{
}
//=============================================================================
StatusCode CaloClusterCloner::initialize() 
{

  debug() << "==> Initialize" << endmsg;

  StatusCode sc = base_class::initialize();
  
  if (! sc.isSuccess() ) return sc;
  
  return StatusCode::SUCCESS;
}
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

  const std::vector<LHCb::CaloClusterEntry>& entries = cluster->entries();

  if (!entries.empty()) {
    std::vector<LHCb::CaloClusterEntry> clonedEntries;
    std::vector<LHCb::CaloClusterEntry>::const_iterator iCalo = entries.begin();
    std::vector<LHCb::CaloClusterEntry>::const_iterator caloEnd = entries.end();
    for ( ; iCalo != caloEnd; ++iCalo) {
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
