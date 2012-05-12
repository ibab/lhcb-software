// $Id: CherenkovG4EventAction.h,v 1.11 2007-01-12 15:32:03 ranjard Exp $
#ifndef CherenkovG4EventAction_h
#define CherenkovG4EventAction_h 1

// Include files
// from Gaudi
#include "../SensDet/RichG4HitCollName.h"

// GiGa
#include <vector>
#include "GiGa/GiGaEventActionBase.h"
#include "CkvG4HistoFillSet1.h"
#include "CherenkovG4HistoFillSet2.h"
#include "RichG4HistoFillSet3.h"
#include "CherenkovG4HistoFillSet4.h"
#include "CherenkovG4HistoFillSet5.h"
#include "RichG4HistoFillTimer.h"
#include "CkvG4EventHitCount.h"
#include "CherenkovG4HitRecon.h"
#include "RichG4InputMon.h"

/** @class CherenkovG4EventAction CherenkovG4EventAction.h src/RichActions/CherenkovG4EventAction.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2011-03-07
 */
class CherenkovG4EventAction : public GiGaEventActionBase
{
public:

 /** standard constructor 
   *  @see GiGaEventActionBase
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  CherenkovG4EventAction
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  ///  destructor (virtual and protected)
  virtual ~CherenkovG4EventAction();

  /// initialize
  StatusCode initialize();

  /// finalize
  StatusCode finalize();
  
  virtual void BeginOfEventAction ( const G4Event* );
  virtual void EndOfEventAction   ( const G4Event* );
  int RichG4CollectionID(int CollNum ) 
   {return   m_RichG4CollectionID[CollNum]; }
  /// 
  RichG4HitCollName* RichHitCName() {return  m_RichHitCName; }
  int NumRichColl(){
    return  m_NumRichColl;
  }
    
  CkvG4HistoFillSet1* GetRichG4HistoFillSet1() 
  {
    return m_RichG4HistoFillSet1;
  }
  CherenkovG4HistoFillSet2* GetRichG4HistoFillSet2() 
  {
    return m_RichG4HistoFillSet2;
  }
  RichG4HistoFillSet3* GetRichG4HistoFillSet3() 
  {
    return m_RichG4HistoFillSet3;
  }

  CherenkovG4HistoFillSet4* GetRichG4HistoFillSet4() 
  {
    return m_RichG4HistoFillSet4;
  }
  CherenkovG4HistoFillSet5* GetCherenkovG4HistoFillSet5() 
  {
    return m_CherenkovG4HistoFillSet5;
  }


  RichG4HistoFillTimer* GetRichG4HistoFillTimer() 
  {
    return m_RichG4HistoFillTimer;
  }
  
  int RichEventActionVerboseLevel() 
  {
    return  m_RichEventActionVerboseLevel;
  }

  CkvG4EventHitCount* RichG4EventHitCounter() 
  {
    
    return m_RichG4EventHitCounter;
  }

  
  void   PrintRichG4HitCounters();
  
  
  bool RichEventActionHistoFillActivateSet1()
  {
    return  m_RichEventActionHistoFillActivateSet1;
  }
  bool RichEventActionHistoFillActivateSet2()
  {
    return  m_RichEventActionHistoFillActivateSet2;
  }
  bool RichEventActionHistoFillActivateSet3()
  {
    return  m_RichEventActionHistoFillActivateSet3;
  }
  bool RichEventActionHistoFillActivateTimer()
  {
    return  m_RichEventActionHistoFillActivateTimer;
  }
  CherenkovG4HitRecon* getRichG4HitRecon() 
  {
    return m_RichG4HitRecon;
  }

  bool RichG4EventActivateCkvRecon()
  {
    return  m_RichG4EventActivateCkvRecon;    
  }
  bool  RichG4HistoActivateQw() {
    return m_RichG4HistoActivateQw;
  }

  RichG4InputMon* getRichG4InputMon() 
  {    return m_RichG4InputMon;}
  

  bool RichG4InputMonActivate() 
  {
    return m_RichG4InputMonActivate;
  }
  bool RichG4HitReconUseStdRadHit() {
    return m_RichG4HitReconUseStdRadHit;}
  void setRichG4HitReconUseStdRadHit( bool afl ) {
    m_RichG4HitReconUseStdRadHit= afl;}
 
  bool IsRichG4FirstEvent() {return m_IsRichG4FirstEvent;}
private:
 
  CherenkovG4EventAction() ; ///< no default constructor
  CherenkovG4EventAction( const CherenkovG4EventAction& ) ; ///< no copy  
  CherenkovG4EventAction& operator=( const CherenkovG4EventAction& ) ; ///< no = 

  template <typename TOOL> inline
  void delPointer( TOOL *& tool ) {
    if ( 0 != tool ) { delete tool; tool = 0; }
  }
  
private:
 
  std::vector<int> m_RichG4CollectionID;
  int m_NumRichColl;
  int m_NumRichClassicColl;
  RichG4HitCollName* m_RichHitCName;
  CkvG4HistoFillSet1* m_RichG4HistoFillSet1;
  CherenkovG4HistoFillSet2* m_RichG4HistoFillSet2;
  RichG4HistoFillSet3* m_RichG4HistoFillSet3;
  CherenkovG4HistoFillSet4* m_RichG4HistoFillSet4;
  CherenkovG4HistoFillSet5* m_CherenkovG4HistoFillSet5;
  RichG4HistoFillTimer* m_RichG4HistoFillTimer;
  CkvG4EventHitCount* m_RichG4EventHitCounter;
  CherenkovG4HitRecon* m_RichG4HitRecon;
  RichG4InputMon* m_RichG4InputMon;
  

  int m_RichEventActionVerboseLevel;
  bool m_RichEventActionHistoFillActivateSet1;
  bool m_RichEventActionHistoFillActivateSet2;
  bool m_RichEventActionHistoFillActivateSet3;
  bool m_RichEventActionHistoFillActivateSet4;
  bool m_RichEventActionHistoFillActivateSet5;
  bool m_RichEventActionHistoFillActivateTimer;
  bool m_RichG4EventHitActivateCount;
  bool m_RichG4EventActivateCkvRecon;
  bool m_RichG4HistoActivateQw;

  bool m_RichG4HitReconUseSatHit;
  bool m_RichG4HitReconUseStdRadHit;
  bool m_RichG4HitReconUseMidRadiator;

  bool m_RichG4InputMonActivate;
  
  bool m_IsRichG4FirstEvent;

  bool m_CkvG4HitReconUseOnlySignalHit;
  bool m_CkvG4HitReconUseOnlyHighMom;


};

#endif  // end of CherenkovG4EventAction_H
