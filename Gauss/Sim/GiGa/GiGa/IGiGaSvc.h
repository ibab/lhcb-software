// $Id: IGiGaSvc.h,v 1.4 2002-12-07 14:27:51 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef GIGA_IGIGASVC_H
#define GIGA_IGIGASVC_H 1 
/// STD and STL 
#include   <utility>
/// Include files from the Framework
#include  "GaudiKernel/Kernel.h"
#include  "GaudiKernel/StatusCode.h"
#include  "GaudiKernel/IService.h"
/// GiGa
#include  "GiGa/IIDIGiGaSvc.h"

// Forward declaration from G4
class      G4Event               ;
class      G4PrimaryVertex       ;
class      G4HCofThisEvent       ;
class      G4VHitsCollection     ;
class      G4VTrajectory         ; 
class      G4TrajectoryContainer ;

/** @class IGiGaSvc IGiGaSvc.h GiGa/IGiGaSvc.h
 *
 *  definition of the abstract interface to Geant4 Service 
 *   for event-by-event communications with Geant4 
 *
 *  @author Vanya Belyaev
 */

class IGiGaSvc : virtual public IService
{
public:
  
  /// the type for extraction the hit colelction with given ID 
  typedef  std::pair<const int,G4VHitsCollection*>          CollectionPair     ;
  /// the type for extraction the hit colelction with given name 
  typedef  std::pair<const std::string,G4VHitsCollection*>  CollectionNamePair ;
 
public:
  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IGiGaSvc; }
  
  /// virtual destructor
  virtual ~IGiGaSvc(){};
  
  /**  Query the interface  
   *   @param riid  interface identifier 
   *   @param ppvI  placeholder to return teh interface itself 
   *   @return status code 
   */
  virtual StatusCode queryInterface ( const InterfaceID & riid, 
                                      void**              ppvI ) = 0 ;

  /**  initialize 
   *   @return status code 
   */
  virtual StatusCode                    initialize() = 0 ;

  /**  initialize 
   *   @return status code 
   */
  virtual StatusCode                    finalize  () = 0 ;

public:
  
  /** add  primary vertex into GiGa/G4 
   * 
   *  @param  vertex pointer to primary vertex 
   *  @return  self-reference ot IGiGaSvc interface 
   */
  virtual IGiGaSvc&   operator <<         ( G4PrimaryVertex  * vertex   ) = 0 ;
  
  
  /** get the whole event  object from GiGa/G4 
   * 
   *  @param   event pointer to whole event  
   *  @return  self-reference ot IGiGaSvc interface 
   */
  virtual IGiGaSvc& operator >> ( const G4Event*         & event        ) = 0 ;

  /** get the all hit collections from GiGa/G4 
   * 
   *  @param   collections  pointer to all hit collections  
   *  @return  self-reference ot IGiGaSvc interface 
   */
  virtual IGiGaSvc& operator >> ( G4HCofThisEvent*       & collections  ) = 0 ;

  /** get the concrete hit collection from GiGa/G4 
   * 
   *  @param   collection  reference to collection pair   
   *  @return  self-reference ot IGiGaSvc interface 
   */
  virtual IGiGaSvc& operator >> ( CollectionPair         & collection   ) = 0 ;

  /** get the concrete hit collection from GiGa/G4 
   * 
   *  @param   collection  reference to collection pair   
   *  @return  self-reference ot IGiGaSvc interface 
   */
  virtual IGiGaSvc& operator >> ( CollectionNamePair     & collection   ) = 0 ;
  
  /** get all trajectories(trajectory container) from GiGa/G4 
   *                  implementation of IGiGaSvc abstract interface 
   *
   *  NB: errors are reported through exception thrown
   * 
   *  @param   trajectories  pointer to trajectory conatiner   
   *  @return  self-reference ot IGiGaSvc interface 
   */
  virtual IGiGaSvc& operator >> ( G4TrajectoryContainer* & trajectories ) = 0 ;
  
  /** add  primary vertex into GiGa/G4  
   *
   *  @param  vertex pointer to primary vertex 
   *  @return status code  
   */
  virtual StatusCode  addPrimaryKinematics( G4PrimaryVertex  * vertex   ) = 0 ;


  /** get the whole event  object from GiGa/G4 
   *
   *  @param  event pointer to whole event 
   *  @return status code  
   */
  virtual StatusCode retrieveEvent          ( const G4Event*          & ) = 0 ;

  
  /** get the all hit collections from GiGa/G4 
   *                  implementation of IGiGaSvc abstract interface 
   * 
   *  @param   collections  pointer to all hit collections  
   *  @return  status code  
   */
  virtual StatusCode retrieveHitCollections ( G4HCofThisEvent*        & ) = 0 ;


  /** get the concrete hit collection from GiGa/G4 
   *                  implementation of IGiGaSvc abstract interface 
   *
   *  @param   collection  reference to collection pair   
   *  @return  status code 
   */
  virtual StatusCode retrieveHitCollection  ( CollectionPair          & ) = 0 ;

  /** get the concrete hit collection from GiGa/G4 
   *                  implementation of IGiGaSvc abstract interface 
   *
   *  @param   collection  reference to collection pair   
   *  @return  status code 
   */
  virtual StatusCode retrieveHitCollection  ( CollectionNamePair      & ) = 0 ;
  
  /** get all trajectories(trajectory container) from GiGa/G4 
   *                  implementation of IGiGaSvc abstract interface 
   *
   *  NB: errors are reported throw exception
   * 
   *  @param   trajectories  pointer to trajectory conatiner   
   *  @return  self-reference ot IGiGaSvc interface 
   */
  virtual StatusCode retrieveTrajectories   ( G4TrajectoryContainer*  & ) = 0 ;

};

// ============================================================================
#endif ///< GIGA_GIGASVC_H
// ============================================================================







































