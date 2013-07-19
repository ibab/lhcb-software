#ifndef GaussRICH_RichG4HitCollName_h
#define GaussRICH_RichG4HitCollName_h 1
#include "globals.hh"
#include <vector>

class RichG4HitCollName {
 
public:

  RichG4HitCollName();
  virtual ~RichG4HitCollName();

  int NumberOfHCollectionInRICH() const
  {
    return m_NumberOfHCollectionInRICH; 
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

  void setSuperRichFlag(bool aFlag) 
  { 
    m_SuperRichFlag= aFlag;
    
  }
  void setCollConfigWithSuperRich();
  
  
private:

  int m_NumberOfHCollectionInRICH;

  std::vector<G4String> m_RichHCName;
  int m_InitCollectListNumForOutput;
  int m_FinalCollectListNumForOutput;

  bool m_SuperRichFlag;
  
  
};  

#endif  // end of GaussRICH_RichG4HitCollName
