// $Id: GaussTrackActionHepMC.h,v 1.1 2005-10-31 09:30:55 gcorti Exp $
#ifndef COMPONENT_GAUSSTRACKACTIONHEPMC_H 
#define COMPONENT_GAUSSTRACKACTIONHEPMC_H 1

// STD & STL 
#include <string>
#include <vector>
/// GiGa 
#include "GiGa/GiGaTrackActionBase.h"
//
#include "GaussTools/MCTruthManager.h"
/// forward decalrations 
template <class TYPE> class GiGaFactory;

/** @class GaussTrackActionHepMC GaussTrackActionHepMC.h
 *
 *  @author Witek Pokorski
 *  @date   24/07/2001
 */

class GaussTrackActionHepMC: public GiGaTrackActionBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<GaussTrackActionHepMC>;
  ///
public:
    
  /** initialize the track action  
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode initialize () ; 
  
  /** perform the pre-action
   *  @see G4UserTrackingAction
   *  @param track pointer to Geant4 track object 
   */ 
  virtual void PreUserTrackingAction  ( const G4Track* track );
  
  /** perform the post-action
   *  @see G4UserTrackingAction
   *  @param track pointer to Geant4 track object 
   */ 
  virtual void PostUserTrackingAction ( const G4Track* track );
  
protected:
  
  /** standard constructor 
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GaussTrackActionHepMC
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GaussTrackActionHepMC();

private:

  /// no default constructor
  GaussTrackActionHepMC();
  /// no copy constructor 
  GaussTrackActionHepMC( const GaussTrackActionHepMC& );
  /// no assignment
  GaussTrackActionHepMC& operator=( const GaussTrackActionHepMC& );
  
private:  

  MCTruthManager* mcmgr;
  HepLorentzVector fourmomentum;

};

#endif
