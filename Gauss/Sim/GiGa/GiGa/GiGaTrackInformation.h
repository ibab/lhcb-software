// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.8  2001/08/12 15:42:44  ibelyaev
/// improvements with Doxygen comments
///
/// Revision 1.7  2001/07/23 13:11:44  ibelyaev
/// the package restructurisation(II)
/// 
// ============================================================================
#ifndef      GIGA_GIGATRACKINFORMATION_H 
#define      GIGA_GIGATRACKINFORMATION_H 1 
// ============================================================================
/// STL
#include <string>
/// Geant4 
#include "G4VUserTrackInformation.hh" 

/** @class GiGaTrackInformation GiGaTrackInformation.h 
 *
 *  Class which is used to strore auxiallry information about G4Track,
 *  could be used to control storage of track 
 *   fromm G4Track objects into G4VTrajectory objects. 
 *   
 *  Class to be extenden in the future to take into account hit patterns. 
 *   
 *  @author  Vanya Belyaev
 *  @date    23/02/2001
 */

///
class GiGaTrackInformation : public G4VUserTrackInformation
{
  ///
public:
  ///
  inline          GiGaTrackInformation() ;
  virtual inline ~GiGaTrackInformation() ; 
  ///
  inline void* operator new    ( size_t );
  inline void  operator delete ( void*  );
  ///
  virtual void Print() const;   /// from base class
  ///
  /// track is forced to be saved into trajectory 
  inline GiGaTrackInformation&  setToBeStored               ( const bool ) ;
  inline const bool             toBeStored                  () const ;  
  ///
  /// track created a hit
  inline GiGaTrackInformation& setCreatedHit(const bool);
  inline const bool            createdHit() const;
  ///

private:
  ///
  /// flag indicating that track is forced to be stored into trajectory  
  bool           m_toBeStored;
  /// flag indicating that track creted a hit
  bool m_createdHit;
  ///
};
///
#include "GiGa/GiGaTrackInformation.icpp"  
///

// ============================================================================
#endif   ///<  GIGA_GIGATRACKINFORMATION_H 
// ============================================================================
