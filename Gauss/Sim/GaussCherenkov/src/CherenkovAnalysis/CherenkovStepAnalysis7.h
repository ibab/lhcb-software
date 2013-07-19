// $Id: $
#ifndef CHERENKOVANALYSIS_CHERENKOVSTEPANALYSIS7_H 
#define CHERENKOVANALYSIS_CHERENKOVSTEPANALYSIS7_H 1

// Include files
#include "GiGa/GiGaStepActionBase.h"
template <class TYPE> class GiGaFactory;
class G4Step;
class IHistogram1D;


/** @class CherenkovStepAnalysis7 CherenkovStepAnalysis7.h CherenkovAnalysis/CherenkovStepAnalysis7.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2012-06-10
 */
class CherenkovStepAnalysis7 : virtual public GiGaStepActionBase{
  friend class GiGaFactory<CherenkovStepAnalysis7>;


public: 
  /// Standard constructor
  CherenkovStepAnalysis7(const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent  ); 

  virtual ~CherenkovStepAnalysis7( ); ///< Destructor

  virtual void UserSteppingAction( const G4Step* aStep );

protected:

private:

  CherenkovStepAnalysis7();
  CherenkovStepAnalysis7(const CherenkovStepAnalysis7& );
  CherenkovStepAnalysis7& operator=(const CherenkovStepAnalysis7& );

  IHistogram1D* m_hHistoPhotDirInPhDetFrameR1;
  IHistogram1D* m_hHistoPhotDirInPhDetFrameR2;
  

};
#endif // CHERENKOVANALYSIS_CHERENKOVSTEPANALYSIS7_H
