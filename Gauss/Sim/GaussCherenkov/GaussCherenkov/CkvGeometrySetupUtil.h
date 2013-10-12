#ifndef GAUSSCHERENKOV_CKVGEOMETRYSETUPUTIL_H 
#define GAUSSCHERENKOV_CKVGEOMETRYSETUPUTIL_H 1

// Include files

/** @class CkvGeometrySetupUtil CkvGeometrySetupUtil.h GaussCherenkov/CkvGeometrySetupUtil.h
 *  
 *
 *  @author Sajan Easo
 *  @date   2012-12-12
 */
class CkvGeometrySetupUtil {
public: 

  virtual ~CkvGeometrySetupUtil( ); ///< Destructor
  static CkvGeometrySetupUtil* getCkvGeometrySetupUtilInstance();
  bool isSuperRich() {return  m_superRichFlag;}
  
  bool isClassicTwoRich()  {  return  m_classicTwoRichFlag;}
  
  bool hasHorizontalRich1(){    return  m_horizontalRich1Flag;}
  
  bool hasOptimizedVerticalRich1(){ return m_optVerticalRichFlag;}
  
  bool hasOptimizedHorizontalRich1(){return   m_optHorizontalRichFlag;}

  bool Rich2_UseGrandPmt() 
  {  return  m_Rich2_UseGrandPmt;}
  
    
  


protected:

private:
  /// Standard constructor
  CkvGeometrySetupUtil();
  
  void InitCkvGeometrySuperRichFlag();
  void InitCkvGeometryTwoRichFlags();


private:
 static CkvGeometrySetupUtil* CkvGeometrySetupUtilInstance;

  bool m_superRichFlag;
  bool m_classicTwoRichFlag;
  bool m_horizontalRich1Flag;
  bool m_optVerticalRichFlag;
  bool m_optHorizontalRichFlag;
  int m_Rich1PmtArrayConfig;  // 0 for standard arrangement
  int m_Rich2PmtArrayConfig;  // 0 for standard arrangement:all pmts=R11265.
                              // 1  all pmts= grandPMT=H12700
  bool m_Rich2_UseGrandPmt;
  

  
  
};
#endif // GAUSSCHERENKOV_CKVGEOMETRYSETUPUTIL_H
