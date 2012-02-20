// $Id: TutorialStepAnalysisA.h,v 1.2 2004-02-10 14:24:17 jonesc Exp $
#ifndef TUTORIALANALYSIS_TUTORIALSTEPANALYSISA_H
#define TUTORIALANALYSIS_YUTORIALSTEPANALYSISA_H 1

// Include files
/// GiGa
#include "GiGa/GiGaStepActionBase.h"
template <class TYPE> class GiGaFactory;
class G4Step;

/** @class TutorialStepAnalysisA TutorialStepAnalysisA.h RichAnalysis/TutorialStepAnalysisA.h
 *
 *
 *  @author Sajan EASO
 *  @date   2003-10-29
 */

class TutorialStepAnalysisA:virtual public  GiGaStepActionBase {
  friend class GiGaFactory<TutorialStepAnalysisA>;

public:
  /// Standard constructor
  TutorialStepAnalysisA
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;

  virtual ~TutorialStepAnalysisA( ); ///< Destructor
  virtual void UserSteppingAction( const G4Step* aStep );

protected:

private:
  TutorialStepAnalysisA();
  TutorialStepAnalysisA(const  TutorialStepAnalysisA& );
  TutorialStepAnalysisA& operator=(const  TutorialStepAnalysisA& );

  std::string m_tutorialMasterLogVolName;
  std::string m_tutorialPixelContainerLogVolName;
  std::string m_tutorialPixelLogVolName;
 
  int m_tutorialNumPixelContainer;
  G4double m_tutorialEnergyMin;
  

};
#endif // TUTORIALANALYSIS_TUTORIALSTEPANALYSISA_H
