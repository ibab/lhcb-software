// $Id: GaussEventActionHepMC.h,v 1.1 2005-10-31 09:31:08 gcorti Exp $
#ifndef COMPONENTS_GAUSSEVENTACTIONHEPMC_H 
#define COMPONENTS_GAUSSEVENTACTIONHEPMC_H 1

// GiGa
#include "GiGa/GiGaEventActionBase.h"
// HepMC
#include "GaussTools/MCTruthManager.h"
/// forward declarations
template <class TYPE> class GiGaFactory;

/** @class GaussEventActionHepMC GaussEventActionHepMC.h
 *  
 *  A concrete Event Action. 
 *  It executes some Geant4 commands at begin and at the 
 *  end of each event
 *
 *  @author Witek Pokorski
 *  @date   06/09/2005
 */

class GaussEventActionHepMC: public GiGaEventActionBase
{
  /// friend factory for instantiation
  friend class GiGaFactory<GaussEventActionHepMC>;
  
public:
    
  /** performs the action at the begin of each event 
   *  @param event pointer to Geant4 event object 
   */
  virtual void BeginOfEventAction ( const G4Event* event );
  
  /** performs the action at the end  of each event 
   *  @param event pointer to Geant4 event object 
   */
  virtual void EndOfEventAction   ( const G4Event* event );
  
protected:
  
  /** standard constructor 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GaussEventActionHepMC
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent );
  
  /// destructor
  virtual ~GaussEventActionHepMC( );
  
private:
  
  ///no default constructor
  GaussEventActionHepMC();
  /// no copy constructor 
  GaussEventActionHepMC( const GaussEventActionHepMC& );  
  /// no assignement 
  GaussEventActionHepMC& operator=( const GaussEventActionHepMC& );

private:

  MCTruthManager* mcmanager;
  void DumpTree(HepMC::GenParticle*, int);
  
};

#endif // COMPONENTS_GAUSSEVENTACTIONHEPMC_H
