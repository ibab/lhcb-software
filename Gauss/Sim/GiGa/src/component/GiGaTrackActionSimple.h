/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.4  2001/07/23 13:12:29  ibelyaev
/// the package restructurisation(II)
/// 
/// ===========================================================================
#ifndef       GIGA_GiGaTrackActionSimple_H
#define       GIGA_GiGaTrackActionSimple_H 1 
/// ===========================================================================
/// STL 
#include <string>
#include <vector>
/// GiGa
#include "GiGa/GiGaTrackActionBase.h"
///
template <class TA> 
class GiGaTrackActionFactory;
class G4Track;
class G4particleDefinition;
///

/** @class GiGaTrackActionSimple GiGaTrackActionSimple.h 
 *
 *  Example of "simple" implementation of Tracking Action Class
 *  It uses GiGaTrajectory, and thats why the results(trajectories) 
 *  could be converted into Gaudi Transient Store.  
 *  It is just "Simple"!
 *  
 *
 *  @author  Vanya Belyaev
 *  @date    23/01/2001
 */


class GiGaTrackActionSimple: virtual public GiGaTrackActionBase
{
  ///
  friend class GiGaTrackActionFactory<GiGaTrackActionSimple>;
  ///
public: 
  ///
  typedef  std::vector<std::string>                  TypeNames; 
  typedef  std::vector<const G4ParticleDefinition*>  PartDefs; 
  ///
protected:
  /// constructor 
  GiGaTrackActionSimple( const std::string& , ISvcLocator* );
  ///  
  virtual ~GiGaTrackActionSimple();
  ////
public: 
  ///
  virtual StatusCode initialize () ; 
  virtual StatusCode finalize   () ;

  /**  perform action 
   *   @param pointer to new track opbject 
   */
  virtual void PreUserTrackingAction  ( const G4Track* ) ;
  /**  perform action 
   *   @param pointer to new track opbject 
   */
  virtual void PostUserTrackingAction ( const G4Track* ) ;
  ///
protected:
  ///
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
  ///
  GiGaTrackActionSimple() ; ///< no default constructor
  GiGaTrackActionSimple( const GiGaTrackActionSimple& ) ; ///< no copy 
  GiGaTrackActionSimple& operator=( const GiGaTrackActionSimple& ) ; 
  ///
private:
  ///
  ///  Flags:
  ///  all tracks to be stored         
  bool            m_storeAll              ;
  ///  all primaries are stored
  bool            m_storePrimaries        ;
  ///  all tracks  with kinetic energy over threshold are stored  
  bool            m_storeByOwnEnergy      ; 
  ///  all tracks  with given type are stored 
  bool            m_storeByOwnType        ; 
  ///  all tracks which has a daughter 
  /// with kinetic energy over threshold are stored 
  bool            m_storeByChildEnergy    ; 
  ///  all tracks which has a daughter of given type are stored 
  bool            m_storeByChildType      ; 
  ///  all tracks which are explicitely marked to be stored are stored 
  bool            m_storeMarkedTracks     ; 
  /// 
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

/// ===========================================================================
#include "GiGaTrackActionSimple.icpp"
/// ===========================================================================

/// ===========================================================================
#endif  //    GIGA_GiGaTrackActionSimple_H
/// ===========================================================================
















