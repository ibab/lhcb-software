#ifndef      GIGA_GIGATRACKINFORMATION_H 
#define      GIGA_GIGATRACKINFORMATION_H 1 
/// STL
#include <string>
/// Geant4 
#include "G4VUserTrackInformation.hh" 

/** @class GiGaTrackInfo GiGaTrackInfo.h GiGa/GiGaTrackInfo.h
    
    Class which is used to strore auxiallry information about G4Track,
    could be used to control storage of track 
    fromm G4Track objects into G4VTrajectory objects. 
    
    Class to be extenden in teh future to take into account hit patterns. 
    
    @author  Vanya Belyaev
    @date    23/02/2001
*/

///
class GiGaTrackInformation : public G4VUserTrackInformation
{
  ///
public:
  ///
  GiGaTrackInformation() ;
  virtual ~GiGaTrackInformation() ; 
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
private:
  ///
  /// flag indicating that track is forced to be stored into trajectory  
  bool           m_toBeStored;
  ///
};
///
#include "GiGa/GiGaTrackInformation.icpp"  
///

#endif   //  GIGA_GIGATRACKINFORMATION_H 
