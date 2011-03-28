// $Id: RichG4EventHitCount.h,v 1.3 2004-02-10 14:24:09 jonesc Exp $
#ifndef RICHANALYSIS_CKVG4EVENTHITCOUNT_H
#define RICHANALYSIS_CKVG4EVENTHITCOUNT_H 1

// Include files
// Include files
#include "G4Event.hh"
#include <vector>
#include "globals.hh"
/** @class CkvG4EventHitCount CkvG4EventHitCount.h
 * RichAnalysis/CkvG4EventHitCount.h
 *
 *
 *  @author Sajan EASO
 *  @date   2003-06-16
 */

class CkvG4EventHitCount {

public:

  /// Standard constructor
  CkvG4EventHitCount( );

  virtual ~CkvG4EventHitCount( ); ///< Destructor

  void RichG4CountAndClassifyHits( const G4Event* anEvent,
                                   int NumRichColl,
                                   const std::vector<int> & RichG4CollectionID ) ;

  std::vector<int> RichG4GetChTrajId(const G4Event* anEvent);

  bool Rich1TrajTraverse(const G4Event* anEvent, int trajId);
  bool Rich1AgelTrajTraverse(const G4Event* anEvent, int trajId);
  bool Rich2TrajTraverse(const G4Event* anEvent, int trajId);
  bool Rich1FiducialRegion(const G4ThreeVector & aTjPos  ) ;
  bool Rich1AgelFiducialRegion(const G4ThreeVector & aTjPos  ) ;
  bool Rich2FiducialRegion(const G4ThreeVector & aTjPos  ) ;
  bool Rich1ProdDirSelection(const  G4ThreeVector & InitMom);
  bool Rich1AgelMomSelection(const  G4ThreeVector & InitMomA);
  bool Rich1GasMomSelection(const G4ThreeVector & InitMomB);
  bool Rich2ProdDirSelection(const  G4ThreeVector & InitMom);
  bool Rich2GasMomSelection(const G4ThreeVector & InitMomB);

  void RichG4CountSaturatedHits( const G4Event* anEvent,
                                 int NumRichColl,
                                 const std::vector<int> & RichG4CollectionID ) ;
  void CkvG4CountFullAcceptSatHits( const G4Event* anEvent,
                                 int NumRichColl,
                                 const std::vector<int> & RichG4CollectionID ) ;


protected:

private:

  G4double ChTkBetaSaturatedCut;
  G4double   m_Rich1InitSlopeRMin;
  G4double   m_Rich1InitSlopeXMin;
  G4double   m_Rich1InitSlopeXMax;
  G4double   m_Rich1InitSlopeYMin;
  G4double   m_Rich1InitSlopeYMax;
  G4double   m_MomRich1AgelMin;
  G4double   m_MomRich1GasMin;
  G4double   m_MinTkPtRich1FiducialRegion;
  G4double   m_MomRich2GasMin;
  G4double   m_Rich2InitSlopeRMin;
  G4double   m_Rich2InitSlopeXMin;
  G4double   m_Rich2InitSlopeXMax;
  G4double   m_Rich2InitSlopeYMin;
  G4double   m_Rich2InitSlopeYMax;
  G4double   m_MinTkPtRich2FiducialRegion;
  G4double m_MinC4F10HighMomCutForYield;
  G4double m_MinCF4HighMomCutForYield;


};
#endif // RICHANALYSIS_RICHG4EVENTHITCOUNT_H
