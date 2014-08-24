#ifndef GAUSSCHERENKOV_CKVGRANDSENSDET_H 
#define GAUSSCHERENKOV_CKVGRANDSENSDET_H 1

// Include files
// from GiGa
#include "GiGa/GiGaSensDetBase.h"

// local
#include "GaussCherenkov/CkvG4Hit.h"
#include "GaussCherenkov/CkvG4HitCollName.h"
#include "CkvG4GeomProp.h"
#include <map>
// forward declarations
class G4HCofThisEvent;


/** @class CkvGrandSensDet CkvGrandSensDet.h GaussCherenkov/CkvGrandSensDet.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2014-08-18
 */
class CkvGrandSensDet: virtual public GiGaSensDetBase 
{
  
public: 
  /// Standard constructor
  CkvGrandSensDet
    (const std::string& type   ,
     const std::string& name   ,
     const IInterface*  parent ); 

  virtual ~CkvGrandSensDet( ); ///< Destructor
  /// initialize
  StatusCode initialize();

  /// finalize
  StatusCode finalize();

  virtual void Initialize(G4HCofThisEvent* HCE);
  // virtual void EndOfEvent(G4HCofThisEvent* HCE);
  virtual bool ProcessHits( G4Step* step,    G4TouchableHistory* history ) ;
  virtual void clear();
  virtual void DrawAll();
  virtual void PrintAll();

protected:

private:
  CkvGrandSensDet(); // no default constructor
  CkvGrandSensDet( const CkvGrandSensDet& ); ///< no copy constructor
  CkvGrandSensDet& operator=( const CkvGrandSensDet& ) ; ///< no =

  


  bool m_RichPmtAviodDuplicateHitsActivate;
  bool m_RichPmtFlagDuplicateHitsActivate;
  CkvG4HitCollName* m_RichG4HCName;
  G4int m_NumberOfHCInRICH;
  std::vector<CkvG4HitsCollection*>  m_RichHC;
  std::vector<G4int> m_PhdHCID;


};
#endif // GAUSSCHERENKOV_CKVGRANDSENSDET_H
