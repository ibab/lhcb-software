// $Id: MCTruthManager.cpp,v 1.1 2005-10-31 09:29:44 gcorti Exp $
// Include files 



// local
#include "GaussTools/MCTruthManager.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCTruthManager
//
// 2005-09-06 : Witold POKORSKI
//-----------------------------------------------------------------------------
static MCTruthManager* instance = 0;

MCTruthManager::MCTruthManager(  ):currentevent(0)
{
  primarybarcodes.clear();
}

MCTruthManager::~MCTruthManager() 
{} 

MCTruthManager* MCTruthManager::GetInstance()
{
  if (instance == 0 )
  {
    instance = new MCTruthManager();
  }
  return instance;
}


void MCTruthManager::NewEvent()
{
  // clear the vector of primary barcordes
  primarybarcodes.clear();

  // we delete the old event  
  delete currentevent;

  // and then we instanciate a new one
  currentevent = new HepMC::GenEvent();
}

HepMC::GenEvent* MCTruthManager::GetCurrentEvent()
{
  return currentevent;
}

void MCTruthManager::AddPrimaryBarcode(int code)
{
  primarybarcodes.push_back(code);
}

std::vector<int>& MCTruthManager::GetPrimaryBarcodes()
{
  return primarybarcodes;
}
