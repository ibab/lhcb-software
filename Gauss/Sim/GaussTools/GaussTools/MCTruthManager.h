// $Id: MCTruthManager.h,v 1.3 2006-05-02 18:29:29 gcorti Exp $
#ifndef COMPONENTS_MCTRUTHMANAGER_H 
#define COMPONENTS_MCTRUTHMANAGER_H 1

#include <utility>
// Include files
//#include "Event/HepMCEvent.h"
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

  // Offset value for intermediate dummy and split particles
  enum MCTruthBarCode {
    DummyBarCode =  20000000,
    SplitBarCode = 100000000       
  };
  
  /// PDG Id for dummy intermediate particle  
  enum MCTruthPDGID {
    DummyPDGID = -99000000 
  };

  virtual ~MCTruthManager( ); 

  static MCTruthManager* GetInstance();

  void NewEvent();
  HepMC::GenEvent* GetCurrentEvent() {
    return event;
  };

  void PrintEvent();
 
  void AddParticle(HepLorentzVector& fourMom, 
                   HepLorentzVector& prodPos, HepLorentzVector& endPos, 
                   int pdg, int trID, int parentID, bool directParent,
                   int creatorID, bool oscillated = false );
 
  const std::vector<int>& GetPrimaryBarcodes();

  int GetCreatorID(int barcode);

  const std::vector<int>& GetOscillatedBarcodes();

protected:
  MCTruthManager( );


private:
  HepMC::GenEvent* event;

  // vector containing barcodes of primary particles (not having any mother)
  std::vector<int> primarybarcodes;

  // map containing barcode of vertices and process identifier
  std::map<int,int> creators;

  // vector containing bar code of oscillated particles
  std::vector<int> oscillated;

  // map containing number of 'segmentations' for each particle
  // (i.e. number of additional vertices introduced in order to attach secondary particles
  // which were created 'in-flight', for instance bremstrahlung gammas, etc)
  std::map<int,int> segmentations;

  // recursive printing of the tree
  void printTree(HepMC::GenParticle*,std::string);

};
#endif // COMPONENTS_MCTRUTHMANAGER_H
