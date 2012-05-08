// $Id: RichG4EventAction.h,v 1.11 2007-01-12 15:32:03 ranjard Exp $
#ifndef RichG4EventAction_h
#define RichG4EventAction_h 1

// Include files
// from Gaudi
#include "../SensDet/RichG4HitCollName.h"

// GiGa
#include <vector>
#include "GiGa/GiGaEventActionBase.h"
#include "../RichAnalysis/RichG4HistoFillSet1.h"
#include "../RichAnalysis/RichG4HistoFillSet2.h"
#include "../RichAnalysis/RichG4HistoFillSet3.h"
#include "../RichAnalysis/RichG4HistoFillSet4.h"
#include "../RichAnalysis/RichG4HistoFillSet5.h"
#include "../RichAnalysis/RichG4HistoFillTimer.h"
#include "../RichAnalysis/RichG4EventHitCount.h"
#include "../RichAnalysis/RichG4HitRecon.h"
#include "../RichAnalysis/RichG4InputMon.h"

/** @class RichG4EventAction RichG4EventAction.h src/RichActions/RichG4EventAction.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2002-08-19
 */
class RichG4EventAction : public GiGaEventActionBase
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
  RichG4EventAction
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  ///  destructor (virtual and protected)
  virtual ~RichG4EventAction();

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
    
  RichG4HistoFillSet1* GetRichG4HistoFillSet1() 
  {
    return m_RichG4HistoFillSet1;
  }
  RichG4HistoFillSet2* GetRichG4HistoFillSet2() 
  {
    return m_RichG4HistoFillSet2;
  }
  RichG4HistoFillSet3* GetRichG4HistoFillSet3() 
  {
    return m_RichG4HistoFillSet3;
  }

  RichG4HistoFillSet4* GetRichG4HistoFillSet4() 
  {
    return m_RichG4HistoFillSet4;
  }


  RichG4HistoFillTimer* GetRichG4HistoFillTimer() 
  {
    return m_RichG4HistoFillTimer;
  }
  
  int RichEventActionVerboseLevel() 
  {
    return  m_RichEventActionVerboseLevel;
  }

  RichG4EventHitCount* RichG4EventHitCounter() 
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
  RichG4HitRecon* getRichG4HitRecon() 
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
 
  RichG4EventAction() ; ///< no default constructor
  RichG4EventAction( const RichG4EventAction& ) ; ///< no copy  
  RichG4EventAction& operator=( const RichG4EventAction& ) ; ///< no = 

  template <typename TOOL> inline
  void delPointer( TOOL *& tool ) {
    if ( 0 != tool ) { delete tool; tool = 0; }
  }
  
private:
 
  std::vector<int> m_RichG4CollectionID;
  int m_NumRichColl;
  RichG4HitCollName* m_RichHitCName;
  RichG4HistoFillSet1* m_RichG4HistoFillSet1;
  RichG4HistoFillSet2* m_RichG4HistoFillSet2;
  RichG4HistoFillSet3* m_RichG4HistoFillSet3;
  RichG4HistoFillSet4* m_RichG4HistoFillSet4;
  RichG4HistoFillSet5* m_RichG4HistoFillSet5;
  RichG4HistoFillTimer* m_RichG4HistoFillTimer;
  RichG4EventHitCount* m_RichG4EventHitCounter;
  RichG4HitRecon* m_RichG4HitRecon;
  RichG4InputMon* m_RichG4InputMon;
  

  int m_RichEventActionVerboseLevel;
  bool m_RichEventActionHistoFillActivateSet1;
  bool m_RichEventActionHistoFillActivateSet2;
  bool m_RichEventActionHistoFillActivateSet3;
  bool m_RichEventActionHistoFillActivateSet4;
  bool m_RichEventActionHistoFillActivateSet5;
  bool m_RichEventActionHistoFillActivateTimer;
  bool m_RichG4EventHitActivateCount;
  bool m_RichG4EventHitActivateCountFullAcc;
  bool m_RichG4EventActivateCkvRecon;
  bool m_RichG4HistoActivateQw;

  bool m_RichG4HitReconUseSatHit;
  bool m_RichG4HitReconUseStdRadHit;
  bool m_RichG4HitReconUseMidRadiator;

  bool m_RichG4InputMonActivate;
  
  bool m_IsRichG4FirstEvent;
  bool m_RichG4HitReconUseOnlySignalHit;
  bool m_RichG4HitReconUseOnlyHighMom;
  
};

#endif  // end of RichG4EventAction_H
