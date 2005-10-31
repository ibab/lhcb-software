// $Id: MCTruthManager.h,v 1.1 2005-10-31 09:28:36 gcorti Exp $
#ifndef COMPONENTS_MCTRUTHMANAGER_H 
#define COMPONENTS_MCTRUTHMANAGER_H 1

// Include files
#include "HepMC/GenEvent.h"

/** @class MCTruthManager MCTruthManager.h Components/MCTruthManager.h
 *  
 *
 *  @author Witold POKORSKI
 *  @date   2005-09-06
 */
class MCTruthManager 
{
public: 

  virtual ~MCTruthManager( ); 

  static MCTruthManager* GetInstance();

  void NewEvent();
  HepMC::GenEvent* GetCurrentEvent();
  void AddPrimaryBarcode(int);
  std::vector<int>& GetPrimaryBarcodes();

protected:
  MCTruthManager( ); 


private:
  HepMC::GenEvent* currentevent;
  std::vector<int> primarybarcodes;

};
#endif // COMPONENTS_MCTRUTHMANAGER_H
