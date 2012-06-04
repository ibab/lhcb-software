#ifndef TorchTBMcpProperties_h
#define TorchTBMcpProperties_h 1
#include <vector>
#include <string>
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"

#include "G4ios.hh"


#include "RichPmtQE.h"




class TorchTBMcpProperties {

public:

  //  TorchTBMcpProperties();
  virtual ~TorchTBMcpProperties();
  static TorchTBMcpProperties* getTorchTBMcpPropertiesInstance();
  void InitializeTorchTBMcpProperties( ) ;

  int numMcpInTorchTB() const {return  m_numMcpInTorchTB; }


  void FillTBMcpQETablesAtInit( IDataProviderSvc* detSvc, IMessageSvc* msgSvc );


  //  void FillTBMcpDemagTablesAtInit( IDataProviderSvc* detSvc, IMessageSvc* msgSvc );

  RichPmtQE* getTorchTBMcpQE(int mcpnum);


  double TorchTBMcpHighVoltage() const {return  m_TorchTBMcpHighVoltage; }
  void setTorchTBMcpHighVoltage(const double TBMcpHv)
  {    m_TorchTBMcpHighVoltage= TBMcpHv;}

  //  double TorchTBMcpQWToAnodeDist() const {return   m_TorchTBMcpQWToAnodeDist; }
  // void setTorchTBMcpQWToAnodeDist( const double TBMcpQWtoAnodeDist )
  //  {
  //  m_TorchTBMcpQWToAnodeDist=TBMcpQWtoAnodeDist;
  // }

  const std::string &  TorchTBMcpQWLogVolName() const  {return m_TorchTBMcpQWLogVolName; }

  void setTBMcpQWLogVolName (const std::string & McpqwLvolname )
  { m_TorchTBMcpQWLogVolName =  McpqwLvolname;}

  const std::string & TorchTBMcpPhCathodeLogVolName() const {return  m_TorchTBMcpPhCathodeLogVolName; }
  void setTorchTBMcpPhCathodeLogVolName (const std::string & mcpphCathLvolname )
  {
    m_TorchTBMcpPhCathodeLogVolName=mcpphCathLvolname;
  }

  int numberOfTorchHitCollections() const 
  {    return m_numberOfTorchHitCollections;}
  


 void setTorchTBMcpMaxQuantumEff(const double aMaxQE)
  {
    m_TorchTBMcpMaxQuantumEff= aMaxQE;
  }

  double TorchTBMcpMaxQuantumEff() const
  {
    return  m_TorchTBMcpMaxQuantumEff;
  }

  //  G4double  MinPhotonEnergyInTorch() 
  // {
  //  return m_MinPhotonEnergyInTorch;
  //  }
  // G4double  MaxPhotonEnergyInTorch()
  //  {
  //  return m_MaxPhotonEnergyInTorch;
  // }


  void setTorchTBMcpPropertiesVerboseLevel(int aLevel );

  void InitializeTorchTBAnodeDetParam();
  
  double TorchAnodeDetXSize() 
  {  return m_TorchAnodeDetXSize;}
  double TorchAnodeDetYSize() 
  {  return m_TorchAnodeDetYSize;}
  double TorchAnodeDetZSize() 
  {  return m_TorchAnodeDetZSize;}

  

private:

  // standard constructor kept private.
  TorchTBMcpProperties( );
  static  TorchTBMcpProperties* TorchTBMcpPropertiesInstance;

  int m_numMcpInTorchTB;
  
 
  std::vector <RichPmtQE*> m_TorchTBMcpQEList;


  double m_TorchTBMcpHighVoltage;
  
  // double m_TorchTBMcpQWToAnodeDist;
  //double m_Torch1MaxZHitCoord;
  std::string m_TorchTBMcpQWLogVolName;
  std::string m_TorchTBMcpPhCathodeLogVolName;

  int m_numberOfTorchHitCollections;


  int m_TorchTBMcpVerboseLevel;

  double m_TorchTBMcpMaxQuantumEff;

  //  G4double  m_MinPhotonEnergyInTorch;
  // G4double  m_MaxPhotonEnergyInTorch;
  

  double m_TorchAnodeDetXSize;
  double m_TorchAnodeDetYSize;
  double m_TorchAnodeDetZSize;
  
  std::string m_TorchCurQEMatPathname;

};

#endif
