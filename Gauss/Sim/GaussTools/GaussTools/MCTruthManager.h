// $Id: MCTruthManager.h,v 1.5 2008-10-20 08:23:11 robbep Exp $
#ifndef COMPONENTS_MCTRUTHMANAGER_H 
#define COMPONENTS_MCTRUTHMANAGER_H 1

#include <utility>
// Include files
//#include "Event/HepMCEvent.h"
#include "HepMC/GenEvent.h"

namespace LHCb {
  class MCParticle ;
}

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
 
  void AddParticle(HepMC::FourVector& fourMom, 
                   HepMC::FourVector& prodPos, HepMC::FourVector& endPos, 
                   int pdg, int trID, int parentID, bool directParent,
                   int creatorID, LHCb::MCParticle * motherMCP , 
                   bool oscillated = false,
                   bool signal = false );
 
  const std::vector<int>& GetPrimaryBarcodes();

  int GetCreatorID(int barcode);

  const std::vector<int>& GetOscillatedBarcodes();

  const std::vector<int>& GetSignalBarcodes();

  /// returns the pre-filled MCParticle mother of the G4 particle
  LHCb::MCParticle * GetMotherMCParticle( const int barcode ) ;

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

  // vector containing bar code of signal particles
  std::vector<int> signal;

  // map containing number of 'segmentations' for each particle
  // (i.e. number of additional vertices introduced in order to attach secondary particles
  // which were created 'in-flight', for instance bremstrahlung gammas, etc)
  std::map<int,int> segmentations;

  /// Map containing links between G4 and pre-filled MCParticles
  std::map< int , LHCb::MCParticle * > m_mcparticles ;
  
  // recursive printing of the tree
  void printTree(HepMC::GenParticle*,std::string);

};
#endif // COMPONENTS_MCTRUTHMANAGER_H
