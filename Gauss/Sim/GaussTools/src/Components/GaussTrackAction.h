// $Id: GaussTrackAction.h,v 1.2 2003-05-05 13:51:27 witoldp Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2003/04/10 08:51:06  witoldp
// added missing header file
//
// Revision 1.1  2002/12/12 15:19:33  witoldp
// major repackaging
//
// Revision 1.3  2002/11/08 11:05:06  witoldp
// many changes - see release.notes
//
// Revision 1.2  2002/10/30 14:00:37  witoldp
// changes following introduction vector of pointers to hits in GiGaTrackInformation
//
// Revision 1.1  2002/09/26 18:10:55  ibelyaev
//  repackageing: add all concrete implementations from GiGa
//
// Revision 1.7  2002/05/07 12:21:37  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
#ifndef       GIGA_GaussTrackAction_H
#define       GIGA_GaussTrackAction_H 1 
// ============================================================================
// STL 
#include <string>
#include <vector>
// GiGa
#include "GiGa/GiGaTrackActionBase.h"
// forward declarations 
template <class TYPE> class GiGaFactory;
class G4Track;
class G4particleDefinition;

/** @class GaussTrackAction GaussTrackAction.h 
 *
 * Gauss tracking action deals with storing tracks
 * and handles track information objects
 *
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author  Witek Pokorski Witold.Pokorski@cern.ch
 *  
 *  @date    23/01/2001
 */


class GaussTrackAction: virtual public GiGaTrackActionBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<GaussTrackAction>;

public: 
  /// useful typedefs
  typedef  std::vector<std::string>                  TypeNames; 
  typedef  std::vector<const G4ParticleDefinition*>  PartDefs; 
  ///
protected:

  /** standard constructor 
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GaussTrackAction
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GaussTrackAction();

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

  /// Should all tracks to be stored?
  inline bool      storeAll             () const ; 
  /// Should primaries be stored ? 
  inline bool      storePrimaries       () const ; 
  /// Should tracks with kinetic energy over some threshold be stored ?  
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

  GaussTrackAction() ; ///< no default constructor
  GaussTrackAction( const GaussTrackAction& ) ; ///< no copy 
  GaussTrackAction& operator=( const GaussTrackAction& ) ; 

private:

  //  Flags:
  ///  all tracks to be stored         
  bool            m_storeAll              ;
  ///  all primaries are stored
  bool            m_storePrimaries        ;
  ///  all tracks  with kinetic energy over threshold are stored  
  bool            m_storeByOwnEnergy      ; 
  ///  all tracks  with given type are stored 
  bool            m_storeByOwnType        ; 
  /** all tracks which has a daughter 
   *  with kinetic energy over threshold are stored 
   */
  bool            m_storeByChildEnergy    ; 
  ///  all tracks which has a daughter of given type are stored 
  bool            m_storeByChildType      ; 
  ///  all tracks which are explicitely marked to be stored are stored 
  bool            m_storeMarkedTracks     ; 
  /// threshold for own kinetic energy 
  double          m_ownEnergyThreshold    ;
  /// threshold for child kinetic energy 
  double          m_childEnergyThreshold  ;
  ///  
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

};

// ============================================================================
#include "GaussTrackAction.icpp"
// ============================================================================

// ============================================================================
#endif  //    GIGA_GaussTrackAction_H
// ============================================================================
















