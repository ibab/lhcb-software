#ifndef RichG4HitCollName_h
#define RichG4HitCollName_h 1
#include "globals.hh"
#include <vector>


class RichG4HitCollName {
 
public:

  RichG4HitCollName();
  virtual ~RichG4HitCollName();

  int NumberOfHCollectionInRICH() {return m_NumberOfHCollectionInRICH; }
  G4String  RichHCName(int HCNum ) {return m_RichHCName[HCNum]; }
  std::vector<G4String>  RichHCNameVect() 
  {
    return m_RichHCName;
  }
  
  int RichHCSize() {return (int) m_RichHCName.size(); }

private:

  int m_NumberOfHCollectionInRICH;

  std::vector<G4String> m_RichHCName;
 
};  

#endif  // end of RichG4HitCollName
