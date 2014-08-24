#ifndef GaussCherenkov_CkvG4HitCollName_h
#define GaussCherenkov_CkvG4HitCollName_h 1
#include "globals.hh"
#include <vector>

class CkvG4HitCollName {
 
public:

  CkvG4HitCollName();
  virtual ~CkvG4HitCollName();

  int NumberOfHCollectionInRICH() const
  {
    return m_NumberOfHCollectionInRICH; 
  }
  int NumberOfStdHCollectionInRICH() const
  {
    return m_NumberOfStdHCollectionInRICH; 
  }
  int NumberOfGrandHCollectionInRICH() const
  {
    return m_NumberOfGrandHCollectionInRICH; 
  }
   
  G4String RichHCName(const int HCNum ) 
  {
    return m_RichHCName[HCNum]; 
  }

  const std::vector<G4String> & RichHCNameVect() const
  {
    return m_RichHCName;
  }
  
  int RichHCSize() const
  {
    return static_cast<int>( m_RichHCName.size() ); 
  }

  int InitCollectListNumForOutput() 
  {
    return m_InitCollectListNumForOutput;
  }
  int FinalCollectListNumForOutput() 
  {
    return m_FinalCollectListNumForOutput;
  }
  void SetInitCollectListNumForOutput(int aInit) 
  {m_InitCollectListNumForOutput=aInit;}
  void SetFinalCollectListNumForOutput(int aFinal)
  {m_FinalCollectListNumForOutput= aFinal;}  


  int InitCollectListNumForStdPmtOutput() 
  {
    return m_InitCollectListNumForStdPmtOutput;
  }
  int FinalCollectListNumForStdPmtOutput() 
  {
    return m_FinalCollectListNumForStdPmtOutput;
  }

  void SetInitCollectListNumForStdPmtOutput(int aInit) 
  {m_InitCollectListNumForStdPmtOutput=aInit;}
  void SetFinalCollectListNumForStdPmtOutput(int aFinal)
  {m_FinalCollectListNumForStdPmtOutput= aFinal;}  


  int InitCollectListNumForGrandPmtOutput() 
  {
    return m_InitCollectListNumForGrandPmtOutput;
  }
  int FinalCollectListNumForGrandPmtOutput() 
  {
    return m_FinalCollectListNumForGrandPmtOutput;
  }

  void SetInitCollectListNumForGrandPmtOutput(int aInit) 
  {m_InitCollectListNumForGrandPmtOutput=aInit;}
  void SetFinalCollectListNumForGrandPmtOutput(int aFinal)
  {m_FinalCollectListNumForGrandPmtOutput= aFinal;}  

  void setCollConfigWithMixedPmtSet();
  void UseGrandPmtFlag(bool aflag) 
  {m_UseGrandPmtFlag = aflag;  }
  bool getUseGrandPmtFlag() 
  {  return m_UseGrandPmtFlag;}
  
  
  
private:

  int m_NumberOfHCollectionInRICH;
  int m_NumberOfStdHCollectionInRICH;
  int m_NumberOfGrandHCollectionInRICH;

  std::vector<G4String> m_RichHCName;
  int m_InitCollectListNumForOutput;
  int m_FinalCollectListNumForOutput;

  int m_InitCollectListNumForStdPmtOutput;
  int m_FinalCollectListNumForStdPmtOutput;

  int m_InitCollectListNumForGrandPmtOutput;
  int m_FinalCollectListNumForGrandPmtOutput;
  
  bool m_UseGrandPmtFlag;
  
};  

#endif  // end of GaussCherenkov_CkvG4HitCollName
