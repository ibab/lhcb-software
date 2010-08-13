// $Id: CaloHypoCloner.cpp,v 1.2 2010-08-13 14:38:09 jpalac Exp $
// Include files 

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

//-----------------------------------------------------------------------------
// Implementation file for class : CaloHypoCloner
//
// 2008-04-01 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloHypoCloner );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloHypoCloner::CaloHypoCloner( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : 
  base_class ( type, name , parent ),
  m_caloClusterCloner(0),
  m_caloClusterClonerName("CaloClusterCloner")
{
}
//=============================================================================
StatusCode CaloHypoCloner::initialize() 
{

  debug() << "==> Initialize" << endmsg;

  StatusCode sc = base_class::initialize();
  
  if (! sc.isSuccess() ) return sc;

  m_caloClusterCloner = tool<ICloneCaloCluster>(m_caloClusterClonerName, this->parent() );
  
  return StatusCode::SUCCESS;
}
//=============================================================================
LHCb::CaloHypo* CaloHypoCloner::operator() (const LHCb::CaloHypo* hypo)
{
  return this->clone(hypo);
}
//=============================================================================
LHCb::CaloHypo* CaloHypoCloner::clone(const LHCb::CaloHypo* hypo)
{
  LHCb::CaloHypo* clone = 
    cloneKeyedContainerItem<BasicCaloHypoCloner>(hypo);

  if (!clone) return 0;

  SmartRefVector<LHCb::CaloHypo> hypos = hypo->hypos();
  if (!hypos.empty()) {
    clone->clearHypos();
    SmartRefVector<LHCb::CaloHypo>::const_iterator iCalo = hypos.begin();
    SmartRefVector<LHCb::CaloHypo>::const_iterator caloEnd = hypos.end();
    for ( ; iCalo != caloEnd; ++iCalo) {
      clone->addToHypos( (*this)(*iCalo));
    }
  }
  
  SmartRefVector<LHCb::CaloDigit> digits = hypo->digits();
  if (!digits.empty()) {
    clone->clearDigits();
    SmartRefVector<LHCb::CaloDigit>::const_iterator iCalo = digits.begin();
    SmartRefVector<LHCb::CaloDigit>::const_iterator caloEnd = digits.end();
    for ( ; iCalo != caloEnd; ++iCalo) {
      clone->addToDigits( cloneKeyedContainerItem<BasicCaloDigitCloner>(*iCalo) );
    }
  }
  
  SmartRefVector<LHCb::CaloCluster> clusters = hypo->clusters();
  if (!clusters.empty()) {
    clone->clearClusters();
    SmartRefVector<LHCb::CaloCluster>::const_iterator iCalo = clusters.begin();
    SmartRefVector<LHCb::CaloCluster>::const_iterator caloEnd = clusters.end();
    for ( ; iCalo != caloEnd; ++iCalo) {
      clone->addToClusters( (*m_caloClusterCloner)(*iCalo) );
    }
  }

  return clone;
  
}
//=============================================================================
// Destructor
//=============================================================================
CaloHypoCloner::~CaloHypoCloner() {} 

//=============================================================================
