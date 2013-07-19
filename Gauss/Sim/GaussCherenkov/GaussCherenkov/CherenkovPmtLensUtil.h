#ifndef GAUSSCHERENKOV_CHERENKOVPMTLENSUTIL_H 
#define GAUSSCHERENKOV_CHERENKOVPMTLENSUTIL_H 1

// Include files
#include <vector>
/** @class CherenkovPmtLensUtil CherenkovPmtLensUtil.h GaussCherenkov/CherenkovPmtLensUtil.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2012-11-15
 */
class CherenkovPmtLensUtil {
public: 

  virtual ~CherenkovPmtLensUtil( ); ///< Destructor
  static CherenkovPmtLensUtil* getInstance();
  bool isPmtModuleWithLens(int aModuleCopyNum);
  bool isPmtWithLens(int aPmtCopyNum);
  //  void InitCherenkovPmtLensUtil();
  
  

protected:

private:
  /// Standard constructor
  CherenkovPmtLensUtil( ); 
  static CherenkovPmtLensUtil* CherenkovPmtLensUtilInstance;

  void InitCherenkovPmtLensUtil();
  void TestLensExistance();


  std::vector<bool>m_RichPmtModuleLensFlag;
  int m_numModuleRich1;
  int m_LensExist;
  
};
#endif // GAUSSCHERENKOV_CHERENKOVPMTLENSUTIL_H
