#ifndef RichG4EventAction_h
#define RichG4EventAction_h 1
// include files  
// SE 19-8-2002.
#include "../SensDet/RichG4HitCollName.h"
// GiGa
#include <vector>
#include "GiGa/GiGaEventActionBase.h"

// forward declaration
template <class TYPE> class GiGaFactory;

class RichG4EventAction : public GiGaEventActionBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<RichG4EventAction>;
  
protected:

 /** standard constructor 
   *  @see GiGaEventActionBase
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  RichG4EventAction
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  ///  destructor (virtual and protected)
  virtual ~RichG4EventAction();

public:
  
  virtual void BeginOfEventAction ( const G4Event* );
  virtual void EndOfEventAction   ( const G4Event* );
  int RichG4CollectionID(int CollNum ) {return   m_RichG4CollectionID[CollNum]; }
  /// 
  RichG4HitCollName* RichHitCName() {return  m_RichHitCName; }

  
private:
  ///
  RichG4EventAction() ; ///< no default constructor
  RichG4EventAction( const RichG4EventAction& ) ; ///< no copy  
  RichG4EventAction& operator=( const RichG4EventAction& ) ; ///< no = 
  ///
private:
  std::vector<int> m_RichG4CollectionID;
  int m_NumRichColl;
  RichG4HitCollName* m_RichHitCName;
  ///
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

#endif  // end of RichG4EventAction_H
