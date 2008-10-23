// ============================================================================
#ifndef       GIGA_GaussPostTrackAction_H
#define       GIGA_GaussPostTrackAction_H 1 
// ============================================================================
// STL 
#include <string>
#include <vector>
// GiGa
#include "GiGa/GiGaTrackActionBase.h"
// forward declarations 
//template <class TYPE> class GiGaFactory;
class G4Track;
class G4ParticleDefinition;

/** @class GaussPostTrackAction GaussPostTrackAction.h 
 *
 * Gauss tracking action deals with storing tracks
 * and handles track information objects
 *
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author  Witek Pokorski Witold.Pokorski@cern.ch
 *  
 *  @date    23/01/2001
 */


class GaussPostTrackAction: virtual public GiGaTrackActionBase
{
  /// friend factory for instantiation 
  //  friend class GiGaFactory<GaussPostTrackAction>;

public: 
  /// useful typedefs
  typedef  std::vector<std::string>                  TypeNames; 
  typedef  std::vector<const G4ParticleDefinition*>  PartDefs; 
  ///
  //protected:

  /** standard constructor 
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GaussPostTrackAction
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GaussPostTrackAction();

public: 
  
  /** initialize the track action  
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode initialize () ; 
  
  /** finalize the action object 
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code
   */
  virtual StatusCode finalize   () ;
  
  /** perform action 
   *  @see G4UserTrackingAction
   *  @param pointer to new track opbject 
   */
  virtual void PreUserTrackingAction  ( const G4Track* ) ;

  /** perform action 
   *  @see G4UserTrackingAction
   *  @param pointer to new track opbject 
   */
  virtual void PostUserTrackingAction ( const G4Track* ) ;

protected:

  /// should tracks above some energy level be stored ?
  inline bool      storeByOwnEnergy     () const ;
  /// should tracks of some predefined types be stored ?
  inline bool      storeByOwnType       () const ; 
  /** Should tracks which produce at least one 
      child with kinetic energy over some threshold be stored ?  
  */
  inline bool      storeByChildEnergy   () const ; 
  /** Should tracks which produce at least one child of some 
      predefined types be stored ?
  */
  inline bool      storeByChildType     () const ; 
  /// Should track which marked explicitely be stored ? 
  inline bool      storeMarkedTracks    () const ; 
  /// Threshold for own   kinetic energy 
  inline double    ownEnergyThreshold   () const ; 
  /// Threshold for child kinetic energy 
  inline double    childEnergyThreshold () const ; 
  /// Own   particle types to be stored 
  inline const PartDefs& ownStoredTypes       () const ; 
  /// Child particle types to be stored 
  inline const PartDefs& childStoredTypes     () const ; 
  ///
private:

  GaussPostTrackAction() ; ///< no default constructor
  GaussPostTrackAction( const GaussPostTrackAction& ) ; ///< no copy 
  GaussPostTrackAction& operator=( const GaussPostTrackAction& ) ; 

private:
  /// Utility function to fill GaussTrack informations
  void fillGaussTrackInformation( const G4Track* track ) const ;
  
  /// Utility function to fill process information of a track
  void setProcess( const G4Track * track ) const ;
  //  Flags:
  ///  all tracks to be stored         
  bool m_storeAll;
  ///  all primaries are stored
  bool m_storePrimaries;
  /// all decays products are stored
  bool m_storeDecayProducts;
  /// all track with kinetic energy above some threshold are stored
  bool m_storeByOwnEnergy;
  ///  all tracks  with given type are stored
  bool m_storeByOwnType; 
  /** all tracks which has a daughter 
   *  with kinetic energy over threshold are stored 
   */
  bool m_storeByChildEnergy; 
  ///  all tracks which has a daughter of given type are stored 
  bool m_storeByChildType; 
  ///  all tracks which are explicitely marked to be stored are stored 
  bool m_storeMarkedTracks; 
  /// threshold for own kinetic energy 
  double m_ownEnergyThreshold;
  /// threshold for child kinetic energy 
  double m_childEnergyThreshold;
  
  /// container of names of own   types 
  TypeNames       m_ownStoredTypesNames   ;  
  /// container of names of child types 
  TypeNames       m_childStoredTypesNames ; 
  ///
  /// container of definitions of own   types 
  PartDefs        m_ownStoredTypes        ; 
  /// container of definitions  of child types 
  PartDefs        m_childStoredTypes      ; 
  ///

  /// store particles undorgoing specific process
  bool m_storeBySecondariesProcess;
  /// container for the process names
  std::vector<std::string> m_childStoredProcess;
  /// store particles produced in specific process
  bool m_storeByOwnProcess;
  /// container for own process names
  std::vector<std::string> m_ownStoredProcess;
  
  /// for flags above beside primary, forced decays and marked store up
  /// to a certain z
  bool m_storeUpToZmax;
  /// value of z max for storage
  double m_zMaxToStore;
  /// bool to keep or not RICHPhotoelectrons
  bool m_rejectRICHphe;
  /// bool to keep or not optical photons
  bool m_rejectOptPhot;

};

// ============================================================================
#include "GaussPostTrackAction.icpp"
// ============================================================================

// ============================================================================
#endif  //    GIGA_GaussPostTrackAction_H
// ============================================================================
















