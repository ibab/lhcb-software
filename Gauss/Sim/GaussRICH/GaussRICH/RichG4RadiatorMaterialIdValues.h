// $Id: $
#ifndef GAUSSRICH_RICHG4RADIATORMATERIALIDVALUES_H 
#define GAUSSRICH_RICHG4RADIATORMATERIALIDVALUES_H 1

// Include files

/** @class RichG4RadiatorMaterialIdValues RichG4RadiatorMaterialIdValues.h Misc/RichG4RadiatorMaterialIdValues.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2011-06-05
 */
#include "globals.hh"
#include <vector>

class RichG4RadiatorMaterialIdValues {
public: 
  /// Standard constructor
  // RichG4RadiatorMaterialIdValues( ); 

  virtual ~RichG4RadiatorMaterialIdValues( ); ///< Destructor
  static RichG4RadiatorMaterialIdValues* RichG4RadiatorMaterialIdValuesInstance();
  G4int Rich1AgelGenericCkvRadiatorNum()  {  return m_Rich1AgelGenericCkvRadiatorNum;}
  G4int Rich1C4F10CkvRadiatorNum(){  return m_Rich1C4F10CkvRadiatorNum;}
  
  G4int Rich2CF4CkvRadiatorNum (){return m_Rich2CF4CkvRadiatorNum;}

  G4int Rich1GaseousCkvRadiatorNum(){return m_Rich1GaseousCkvRadiatorNum;}
  
  G4int Rich2GaseousCkvRadiatorNum() {return m_Rich2GaseousCkvRadiatorNum;}
  
  G4int RichFilterGenericCkvRadiatorNum() {return m_RichFilterGenericCkvRadiatorNum;}
  G4int RichFilterD263CkvRadiatorNum() {return m_RichFilterD263CkvRadiatorNum;}
  G4int Rich1GasQWindowCkvRadiatorNum() {return m_Rich1GasQWindowCkvRadiatorNum;}
  G4int Rich2GasQWindowCkvRadiatorNum() {return m_Rich2GasQWindowCkvRadiatorNum;}
  G4int RichHpdQuartzWindowCkvRadiatorNum() {return m_RichHpdQuartzWindowCkvRadiatorNum;}
  G4int Rich1AgelTile0CkvRadiatorNum() {return m_Rich1AgelTile0CkvRadiatorNum;} 
  G4int Rich1AgelTile1CkvRadiatorNum() {return m_Rich1AgelTile1CkvRadiatorNum;}
  G4int Rich1AgelTile2CkvRadiatorNum() {return m_Rich1AgelTile2CkvRadiatorNum;}
  G4int Rich1AgelTile3CkvRadiatorNum() {return m_Rich1AgelTile3CkvRadiatorNum;}
  G4int Rich1AgelTile4CkvRadiatorNum() {return m_Rich1AgelTile4CkvRadiatorNum;}
  G4int Rich1AgelTile5CkvRadiatorNum() {return m_Rich1AgelTile5CkvRadiatorNum;}
  G4int Rich1AgelTile6CkvRadiatorNum() {return m_Rich1AgelTile6CkvRadiatorNum;} 
  G4int Rich1AgelTile7CkvRadiatorNum() {return m_Rich1AgelTile7CkvRadiatorNum;}
  G4int Rich1AgelTile8CkvRadiatorNum() {return m_Rich1AgelTile8CkvRadiatorNum;}
  G4int Rich1AgelTile9CkvRadiatorNum() {return m_Rich1AgelTile9CkvRadiatorNum;}
  G4int Rich1AgelTile10CkvRadiatorNum() {return m_Rich1AgelTile10CkvRadiatorNum;}
  G4int Rich1AgelTile11CkvRadiatorNum() {return m_Rich1AgelTile11CkvRadiatorNum;}
  G4int Rich1AgelTile12CkvRadiatorNum() {return m_Rich1AgelTile12CkvRadiatorNum;}
  G4int Rich1AgelTile13CkvRadiatorNum() {return m_Rich1AgelTile13CkvRadiatorNum;}
  G4int Rich1AgelTile14CkvRadiatorNum() {return m_Rich1AgelTile14CkvRadiatorNum;}
  G4int Rich1AgelTile15CkvRadiatorNum() {return m_Rich1AgelTile15CkvRadiatorNum;}
  G4int Rich1NitrogenCkvRadiatorNum() {return m_Rich1NitrogenCkvRadiatorNum;}
  G4int Rich2NitrogenCkvRadiatorNum() {return m_Rich2NitrogenCkvRadiatorNum;}
  G4int RichCO2CkvRadiatorNum()  {  return m_RichCO2CkvRadiatorNum;}
  G4int NumRich1AgelSubTileRadiatorNum() {return m_NumRich1AgelSubTileRadiator;}
  G4int NumRich1AgelNominalTileRadiatorNum() {return m_NumRich1AgelNominalTileRadiator;}
  G4int Rich1AgelSubTileCkvRadiatorNumValue(int ast){  return m_Rich1AgelSubTileCkvRadiatorNum[ast];}
  std::vector<G4int> Rich1AgelSubTileCkvRadiatorNum()  {  return m_Rich1AgelSubTileCkvRadiatorNum;}
  std::string Rich1AgelSubTileNameSuffixValue(int asu)  {  return m_Rich1AgelSubTileNameSuffix[asu];}
  std::vector<std::string>Rich1AgelSubTileNameSuffix()  {  return m_Rich1AgelSubTileNameSuffix;}
  G4int RichPmtQuartzWindowCkvRadiatorNum()  {  return m_RichPmtQuartzWindowCkvRadiatorNum;}
  

  bool IsRich1AerogelSubTileRadNum(int aRadNum);
  
  bool IsRich1AerogelAnyTileRad(int aRadNumber);

  G4int Rich1AgelSubTileRadCopyNumToFullTileNum(G4int aCpNum) 
  {  return m_Rich1AgelSubTileRadCopyNumToFullTileNum[aCpNum]  ;}

  G4int Rich1AgelSubTileRadCopyNumToSubTileNumInFullTile(G4int aCopyNum)
  {  return m_Rich1AgelSubTileRadCopyNumToSubTileNumInFullTile [aCopyNum];}

  G4int Rich1AgelTileSubTileToRadCopyNum(G4int aTile, G4int aSubTile ) 
  { return m_Rich1AgelTileSubTileToRadCopyNum [aTile] [aSubTile];}
  
  int Rich1AgelRadiatorNumToFullTileNum(int aRadNum);
  int Rich1AgelRadiatorNumToSubTileNumInFullTile(int aRNum);
  int Rich1AgelTileSubTileToRadiatorNum(G4int aTileA, G4int aSubTileA );
  
    
protected:

private:
  /// Standard constructor
  RichG4RadiatorMaterialIdValues( ); 
  static RichG4RadiatorMaterialIdValues* m_RichG4RadiatorMaterialIdValuesInstance;

G4int m_Rich1AgelGenericCkvRadiatorNum;
G4int m_Rich1C4F10CkvRadiatorNum; //to be phased out
G4int m_Rich2CF4CkvRadiatorNum; // to be phased out
G4int m_Rich1GaseousCkvRadiatorNum;
G4int m_Rich2GaseousCkvRadiatorNum;
G4int m_RichFilterGenericCkvRadiatorNum;
G4int m_RichFilterD263CkvRadiatorNum;
G4int m_Rich1GasQWindowCkvRadiatorNum;
G4int m_Rich2GasQWindowCkvRadiatorNum;
G4int m_RichHpdQuartzWindowCkvRadiatorNum;
G4int m_Rich1AgelTile0CkvRadiatorNum;
G4int m_Rich1AgelTile1CkvRadiatorNum;
G4int m_Rich1AgelTile2CkvRadiatorNum;
G4int m_Rich1AgelTile3CkvRadiatorNum;
G4int m_Rich1AgelTile4CkvRadiatorNum;
G4int m_Rich1AgelTile5CkvRadiatorNum;
G4int m_Rich1AgelTile6CkvRadiatorNum;
G4int m_Rich1AgelTile7CkvRadiatorNum;
G4int m_Rich1AgelTile8CkvRadiatorNum;
G4int m_Rich1AgelTile9CkvRadiatorNum;
G4int m_Rich1AgelTile10CkvRadiatorNum;
G4int m_Rich1AgelTile11CkvRadiatorNum;
G4int m_Rich1AgelTile12CkvRadiatorNum;
G4int m_Rich1AgelTile13CkvRadiatorNum;
G4int m_Rich1AgelTile14CkvRadiatorNum;
G4int m_Rich1AgelTile15CkvRadiatorNum;
G4int m_Rich1NitrogenCkvRadiatorNum;
G4int m_Rich2NitrogenCkvRadiatorNum;
G4int m_RichCO2CkvRadiatorNum;
G4int m_NumRich1AgelSubTileRadiator;
G4int m_NumRich1AgelNominalTileRadiator;
G4int m_RichPmtQuartzWindowCkvRadiatorNum;

std::vector<G4int>m_Rich1AgelSubTileCkvRadiatorNum;
std::vector<std::string>m_Rich1AgelSubTileNameSuffix;

std::vector<G4int> m_Rich1AgelSubTileRadCopyNumToFullTileNum;
std::vector<G4int> m_Rich1AgelSubTileRadCopyNumToSubTileNumInFullTile;
std::vector<std::vector<G4int> > m_Rich1AgelTileSubTileToRadCopyNum;
  
};
#endif // GAUSSRICH_RICHG4RADIATORMATERIALIDVALUES_H
