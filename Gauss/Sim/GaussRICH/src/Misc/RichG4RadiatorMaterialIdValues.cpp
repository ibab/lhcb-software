// $Id: $
// Include files 

#include <boost/lexical_cast.hpp>

// local
#include "GaussRICH/RichG4RadiatorMaterialIdValues.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichG4RadiatorMaterialIdValues
//
// 2011-06-05 : Sajan Easo
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichG4RadiatorMaterialIdValues* RichG4RadiatorMaterialIdValues::m_RichG4RadiatorMaterialIdValuesInstance=0;

RichG4RadiatorMaterialIdValues::RichG4RadiatorMaterialIdValues(  )
  :m_Rich1AgelGenericCkvRadiatorNum(0),
   m_Rich1C4F10CkvRadiatorNum(1),
   m_Rich2CF4CkvRadiatorNum(2),
   m_Rich1GaseousCkvRadiatorNum(1),
   m_Rich2GaseousCkvRadiatorNum(2),
   m_RichFilterGenericCkvRadiatorNum(4),
   m_RichFilterD263CkvRadiatorNum(5),
   m_Rich1GasQWindowCkvRadiatorNum(6),
   m_Rich2GasQWindowCkvRadiatorNum(7),
   m_RichHpdQuartzWindowCkvRadiatorNum(8),
   m_Rich1AgelTile0CkvRadiatorNum(10),
   m_Rich1AgelTile1CkvRadiatorNum(11),
   m_Rich1AgelTile2CkvRadiatorNum(12),
   m_Rich1AgelTile3CkvRadiatorNum(13),
   m_Rich1AgelTile4CkvRadiatorNum(14),
   m_Rich1AgelTile5CkvRadiatorNum(15),
   m_Rich1AgelTile6CkvRadiatorNum(16),
   m_Rich1AgelTile7CkvRadiatorNum(17),
   m_Rich1AgelTile8CkvRadiatorNum(18),
   m_Rich1AgelTile9CkvRadiatorNum(19),
   m_Rich1AgelTile10CkvRadiatorNum(20),
   m_Rich1AgelTile11CkvRadiatorNum(21),
   m_Rich1AgelTile12CkvRadiatorNum(22),
   m_Rich1AgelTile13CkvRadiatorNum(23),
   m_Rich1AgelTile14CkvRadiatorNum(24),
   m_Rich1AgelTile15CkvRadiatorNum(25),
   m_Rich1NitrogenCkvRadiatorNum(30),
   m_Rich2NitrogenCkvRadiatorNum(31),
   m_RichCO2CkvRadiatorNum(33),
   m_NumRich1AgelSubTileRadiator(300),
   m_NumRich1AgelNominalTileRadiator(16),
   m_RichPmtQuartzWindowCkvRadiatorNum(32),
   m_Rich1AgelSubTileCkvRadiatorNum(std::vector<G4int> (m_NumRich1AgelSubTileRadiator)),
   m_Rich1AgelSubTileNameSuffix(std::vector<std::string> (m_NumRich1AgelSubTileRadiator)),
   m_Rich1AgelSubTileRadCopyNumToFullTileNum(std::vector<G4int> (m_NumRich1AgelSubTileRadiator)),
   m_Rich1AgelSubTileRadCopyNumToSubTileNumInFullTile(std::vector<G4int> (m_NumRich1AgelSubTileRadiator)),
   m_Rich1AgelTileSubTileToRadCopyNum(m_NumRich1AgelNominalTileRadiator, std::vector<G4int> (50) )
  { 
   
    for (int is=0; is< m_NumRich1AgelSubTileRadiator; ++is ) {
      m_Rich1AgelSubTileCkvRadiatorNum[is]= 1000+is; 
      m_Rich1AgelSubTileRadCopyNumToFullTileNum[is]=-1; // initialization
      m_Rich1AgelSubTileRadCopyNumToSubTileNumInFullTile[is]=-1; // initialization
      
    }
    std::vector<G4int>aDummyVector(50, -1);
    
    // create aerogel sub tile labels using the tile num and subtile numbers
    int nx=5;
    int ny=5;
    int isr=-1;
    // loop through tiles
    for (int t=0; t<m_NumRich1AgelNominalTileRadiator; ++t) {
      m_Rich1AgelTileSubTileToRadCopyNum[t]=aDummyVector;  // initialize the map.
      std::string ts=boost::lexical_cast<std::string> (t);
      if(t<10) ts= "0"+ts;
      if( (t==0) || (t==1) || (t== 8) || (t== 9) ) {
        nx=5;
        ny=2;
      }else if ( ((t >=2) && (t <= 4)) || 
                 ((t>=10) && (t <=12) ) || (t== 6) || (t==14)) {
        nx=5;
        ny=5;
      }else if ( (t==5 ) || (t == 7 ) || (t==13) || (t==15)) {
        nx=3;
        ny=5;
        
      }
      // loops through sub tiles
      for(int i=0; i<ny; ++i ) {
        for(int j=0; j<nx; ++j) {
          isr++;
          int k= (10*j) + i;
          std::string sus= boost::lexical_cast<std::string>(k);
          if(k<10) sus="0"+sus;
          m_Rich1AgelSubTileNameSuffix[isr]=ts+sus;
          m_Rich1AgelSubTileRadCopyNumToFullTileNum[isr]=t;
          m_Rich1AgelSubTileRadCopyNumToSubTileNumInFullTile[isr]=k;
          m_Rich1AgelTileSubTileToRadCopyNum[t][k]=isr;
        }
      }// end loop loops through sub tiles
      
      
      
    }// end loop over tiles

    // test print
    //  for (int pp=0; pp<m_NumRich1AgelSubTileRadiator; ++pp ) {
    //   
    //  G4cout<<" AgelSubtile num label "<<pp<<"   "<<m_Rich1AgelSubTileCkvRadiatorNum[pp]<<"      "
    //        <<m_Rich1AgelSubTileNameSuffix[pp]<<  G4endl;
    //
    // }

    // end test print    
}

//=============================================================================
// Destructor
//=============================================================================
RichG4RadiatorMaterialIdValues::~RichG4RadiatorMaterialIdValues() {} 

//=============================================================================
RichG4RadiatorMaterialIdValues* RichG4RadiatorMaterialIdValues::RichG4RadiatorMaterialIdValuesInstance() 
{
  if(m_RichG4RadiatorMaterialIdValuesInstance == 0) {
    m_RichG4RadiatorMaterialIdValuesInstance= new RichG4RadiatorMaterialIdValues();
    
  }
  return m_RichG4RadiatorMaterialIdValuesInstance;
  
}
bool RichG4RadiatorMaterialIdValues::IsRich1AerogelSubTileRadNum(int aRadNum) 
{
  return ( std::find(m_Rich1AgelSubTileCkvRadiatorNum.begin(),
                                                 m_Rich1AgelSubTileCkvRadiatorNum.end(), aRadNum) !=
           (m_Rich1AgelSubTileCkvRadiatorNum.end() )); 
}

bool RichG4RadiatorMaterialIdValues::IsRich1AerogelAnyTileRad(int aRadNumber) 
{
  bool aAnyAgelRad = false;
  
  if( ( (aRadNumber >= Rich1AgelTile0CkvRadiatorNum() ) && ( aRadNumber <= Rich1AgelTile15CkvRadiatorNum())) ||
      ( IsRich1AerogelSubTileRadNum( aRadNumber ) ) ) {
    aAnyAgelRad  = true;
    
  }
  return aAnyAgelRad ;  
}

int RichG4RadiatorMaterialIdValues::Rich1AgelRadiatorNumToFullTileNum (int  aRadNum){
    int aRd=aRadNum;
    if (aRadNum >= 1000) { aRd= Rich1AgelSubTileRadCopyNumToFullTileNum(aRadNum-1000); 
    }else {
                           aRd=aRadNum - Rich1AgelTile0CkvRadiatorNum();
    }
    
    
    return aRd;
}
 int RichG4RadiatorMaterialIdValues::Rich1AgelRadiatorNumToSubTileNumInFullTile(int aRNum ){
   int aSd= -1;
   if(aRNum >= 1000) aSd= Rich1AgelSubTileRadCopyNumToSubTileNumInFullTile(aRNum-1000);
   return aSd;
 }

 int RichG4RadiatorMaterialIdValues::Rich1AgelTileSubTileToRadiatorNum (G4int aTileA, G4int aSubTileA ) 
 {
   int aR=-1;
   if((aTileA >=0)  && (aTileA < m_NumRich1AgelNominalTileRadiator) && (aSubTileA >=0)  && ( aSubTileA < 50 ) ) {
     aR=   Rich1AgelTileSubTileToRadCopyNum(aTileA,aSubTileA) + 1000;
   }
   
   return aR;
   
 }
