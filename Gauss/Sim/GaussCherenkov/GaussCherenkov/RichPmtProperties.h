#ifndef RichPmtProperties_h
#define RichPmtProperties_h 1
#include <vector>
#include <string>
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"

#include "G4ios.hh"
#include "GaussCherenkov/CkvGeometrySetupUtil.h"
#include "GaussCherenkov/RichPmtQE.h"
#include "GaussCherenkov/RichPmtPSF.h"
//#include "RichPmtDeMag.h"
class DetectorElement;
class DeRichSystem;


class RichPmtProperties {

public:

  //  RichPmtProperties();
  virtual ~RichPmtProperties();
  static RichPmtProperties* getRichPmtPropertiesInstance();
  void InitializePmtProperties( ) ;

  int numPmtTotRich1() const {return  m_numPmtTotRich[0]; }
  int numPmtTotRich2() const {return  m_numPmtTotRich[1]; }

  //  int hpdNumBegInHitCollection(int aCollBeg) 
  // {return m_hpdNumBegInHitCollection[aCollBeg];}
  // int hpdNumEndInHitCollection(int aCollEnd) 
  // {return m_hpdNumEndInHitCollection[aCollEnd];}

  const std::vector<int> & numPmtTotRich() const {return m_numPmtTotRich; }
  const std::vector<RichPmtQE*> & RichPmtQEList(int richdetnum) const
  {return m_RichPmtQEList[richdetnum]; }

  const std::vector<RichPmtPSF*> & RichPmtPSFList(int richdetnum) const
  {return m_RichPmtPSFList[richdetnum]; }

  //  const std::vector<RichPmtDeMag*> & RichPmtDeMagList(int richdetnum ) const
  // {return m_RichPmtDeMagList[richdetnum]; }

  void FillPmtQETablesAtInit( IDataProviderSvc* detSvc, IMessageSvc* msgSvc );
  void FillPmtPSFTablesAtInit( IDataProviderSvc* detSvc, IMessageSvc* msgSvc );
  //  void FillPmtDemagTablesAtInit( IDataProviderSvc* detSvc, IMessageSvc* msgSvc );

  RichPmtQE* getRichPmtQE(int hpdnum, int richdetnum);

  RichPmtPSF* getRichPmtPSF(int hpdnum, int richdetnum );

  //  RichPmtDeMag* getRichPmtDeMag(int hpdnum, int richdetnum );

  double RichPmtHighVoltage() const {return  m_RichPmtHighVoltage; }
  void setRichPmtHighVoltage(const double PmtHv)
  {
    m_RichPmtHighVoltage= PmtHv;
  }

  double RichPmtQWToSiDist() const {return   m_RichPmtQWToSiDist; }
  void setRichPmtQWToSiDist( const double PmtQWtoSiDist )
  {
    m_RichPmtQWToSiDist=PmtQWtoSiDist;
  }

  double Rich1MaxZHitZCoord() const {return m_Rich1MaxZHitCoord  ; }
  void setRich1MaxZHitZCoord( const double RichMaximumZCoordHit )
  {
    m_Rich1MaxZHitCoord=RichMaximumZCoordHit;
  }

   const std::string & LPmtQWLogVolName() const {return m_LPmtQWLogVolName; }

  const std::string & PmtQWLogVolName() const {return m_PmtQWLogVolName; }
  void setPmtQWLogVolName (const std::string & hpdqwLvolname )
  {
    m_PmtQWLogVolName=  hpdqwLvolname;
  }

  const std::string &  GrandPmtQWLogVolName(){  return m_GrandPmtQWLogVolName ;}
  const std::string &  GrandPmtPhCathodeLogVolName(){  return m_GrandPmtPhCathodeLogVolName;}
  

  const std::string & LPmtPhCathodeLogVolName() const {return  m_LPmtPhCathodeLogVolName; }
  const std::string & LPmtLensLogVolName() const {return m_LPmtLensLogVolName;}

  const std::string & PmtPhCathodeLogVolName() const {return  m_PmtPhCathodeLogVolName; }
 void setPmtPhCathodeLogVolName (const std::string & hpdphCathLvolname )
  {
    m_PmtPhCathodeLogVolName=hpdphCathLvolname;
  }

  int numberOfRichDetectors() const {return m_numberOfRichDetectors; }
  void setnumberOfRichDetectors (const int numberOfRichdet)
  {
    m_numberOfRichDetectors=numberOfRichdet;
  }

  int numberOfRichHitCollections() const 
  {    return m_numberOfRichHitCollections;}
  
  double PmtPhCathodeInnerRadius() const {return  m_PmtPhCathodeInnerRadius; }

  void setPmtPhCathodeInnerRadius(const double phCaInnRad )
  {
    m_PmtPhCathodeInnerRadius = phCaInnRad;
  }

  double PmtMaxQuantumEffFromDB() const
  {
    return  m_PmtMaxQuantumEffFromDB;
  }

 void setPmtMaxQuantumEff(const double aMaxQE)
  {
    m_PmtMaxQuantumEff= aMaxQE;
  }

  double PmtMaxQuantumEff() const
  {
    return  m_PmtMaxQuantumEff;
  }

 
  void setPmtActivateOverRideMaxQEFromDB(bool abval)
  { m_PmtActivateOverRideMaxQEFromDB=abval;}
  
  bool PmtActivateOverMaxQEFromDB()
  {    return m_PmtActivateOverRideMaxQEFromDB;}

  void setPmtDBOverRideMaxQEValue(double aqe) 
  {m_PmtDBOverRideMaxQEValue=aqe;}
  
  double PmtDBOverRideMaxQEValue()
  {  return m_PmtDBOverRideMaxQEValue;}
  
  void setPmtMaximumQuantumEfficiency();
  


  G4double  MinPhotonEnergyInRICH() 
  {
    return m_MinPhotonEnergyInRICH;
  }
  G4double  MaxPhotonEnergyInRICH()
  {
    return m_MaxPhotonEnergyInRICH;
  }

  bool UsingPmtMagneticFieldDistortion() { return m_UsingPmtMagneticFieldDistortion; }
  void setUsingPmtMagneticFieldDistortion(bool aflag) {m_UsingPmtMagneticFieldDistortion=aflag;}

  bool UseNominalPmtQE()  {return m_UseNominalPmtQE;}
  void setUseNominalPmtQE(bool aQEUseTableOption){ m_UseNominalPmtQE=aQEUseTableOption;}
  
    
  void SetCurQETableSourceOption (int aOpt ) 
  {
    m_CurQETableSourceOption=aOpt;
    
  }
  void SetPmtQEOverallScaling( double scaleFactor )
  {
    m_PmtQEScaleFactor = scaleFactor ;
  }

  void setPmtPropertiesVerboseLevel(int aLevel );

  void InitializeSiDetParam();
  
  double siDetXSize() 
  {  return m_siDetXSize;}
  double siDetYSize() 
  {  return m_siDetYSize;}
  double siDetZSize() 
  {  return m_siDetZSize;}

  double getPmtCorrectedQEFromPhotonEnergy(double photonenergy, double originalQE );

  void setActivatePmtModuleSuppressSet3(bool aAct3) 
  {m_ActivatePmtModuleSuppressSet3=aAct3;  }
  void setActivatePmtModuleSuppressSet4(bool aAct4)
  { m_ActivatePmtModuleSuppressSet4=aAct4; }
  void setActivatePmtModuleSuppressSet5(bool aAct5) 
  {m_ActivatePmtModuleSuppressSet5=aAct5;  }
  void setActivatePmtModuleSuppressSet6(bool aAct6)
  { m_ActivatePmtModuleSuppressSet6=aAct6; }


  bool getSuperRichFlag();

  

private:

  // standard constructor kept private.
  RichPmtProperties( );
  static  RichPmtProperties* RichPmtPropertiesInstance;

  //here the first vector has a dimension of 2
  // which is the number of rich detectors in lhcb.
  // the size is 1 for the case of SuperRICH.


  std::vector<DetectorElement*> getRichMasterDetectorElements ( ) ;
  DeRichSystem* getRichSystemDetectorElement ();
  
  void setQWPhCathNames ();
  CkvGeometrySetupUtil * getCkvGeometrySetup ();
  //bool getSuperRichFlag();
  

  std::vector<int>m_numPmtTotRich;
  std::vector<int>m_numPmtTotUsedRich;
  std::vector<int> m_numModuleTotRich;
  
  std::vector<std::vector<RichPmtQE*> >m_RichPmtQEList;
  std::vector<std::vector<RichPmtPSF*> >m_RichPmtPSFList;
  //  std::vector<std::vector<RichPmtDeMag*> >m_RichPmtDeMagList;
  double m_RichPmtHighVoltage;
  double m_RichPmtQWToSiDist;
  double m_Rich1MaxZHitCoord;

  std::string m_PmtQWLogVolName;
  std::string m_PmtPhCathodeLogVolName;
  std::string m_LPmtQWLogVolName;
  std::string m_LPmtPhCathodeLogVolName;
  std::string m_LPmtLensLogVolName;

  std::string  m_GrandPmtQWLogVolName ;
  std::string  m_GrandPmtPhCathodeLogVolName;
  

  int m_numberOfRichDetectors;
  int m_numberOfRichHitCollections;
  double m_PmtPhCathodeInnerRadius;
  int m_PmtVerboseLevel;
  double m_PmtMaxQuantumEffFromDB;
  double m_PmtMaxQuantumEff;

  bool m_PmtActivateOverRideMaxQEFromDB;
  double m_PmtDBOverRideMaxQEValue;


  G4double  m_MinPhotonEnergyInRICH;
  G4double  m_MaxPhotonEnergyInRICH;
  
  bool m_UsingPmtMagneticFieldDistortion;
  bool m_UseNominalPmtQE;
  

  double m_siDetXSize;
  double m_siDetYSize;
  double m_siDetZSize;
  
  std::string m_CurQEMatPathname;
  int m_CurQETableSourceOption; // 0 default means Borosilicate
                                // 1 means the UV glass
                                // 2 means nominal HPD like
  double m_PmtQEScaleFactor;

  //  bool m_UsePmtMagDistortions;
  bool m_ActivatePmtModuleSuppressSet3;
  bool m_ActivatePmtModuleSuppressSet4;
  bool m_ActivatePmtModuleSuppressSet5;
  bool m_ActivatePmtModuleSuppressSet6;  

  bool m_SuperRichFlag;
  

};

#endif
