#ifndef RichHpdProperties_h
#define RichHpdProperties_h 1
#include <vector>
#include <string>
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMessageSvc.h"

#include "G4ios.hh"

#include "RichHpdQE.h"
#include "RichHpdPSF.h"
#include "RichHpdDeMag.h"


class RichHpdProperties {

public:

  RichHpdProperties(IDataProviderSvc*, IMessageSvc*);
  virtual ~RichHpdProperties();

  int numHpdTotRich1() const {return  m_numHpdTotRich[0]; }
  int numHpdTotRich2() const {return  m_numHpdTotRich[1]; }

  const std::vector<int> & numHpdTotRich() const {return m_numHpdTotRich; }
  const std::vector<RichHpdQE*> & RichHpdQEList(int richdetnum) const
  {return m_RichHpdQEList[richdetnum]; }

  const std::vector<RichHpdPSF*> & RichHpdPSFList(int richdetnum) const
  {return m_RichHpdPSFList[richdetnum]; }

  const std::vector<RichHpdDeMag*> & RichHpdDeMagList(int richdetnum ) const
  {return m_RichHpdDeMagList[richdetnum]; }

  RichHpdQE* getRichHpdQE(int hpdnum, int richdetnum);

  RichHpdPSF* getRichHpdPSF(int hpdnum, int richdetnum );

  RichHpdDeMag* getRichHpdDeMag(int hpdnum, int richdetnum );

  double RichHpdHighVoltage() const {return  m_RichHpdHighVoltage; }
  void setRichHpdHighVoltage(const double HpdHv)
  {
    m_RichHpdHighVoltage= HpdHv;
  }

  double RichHpdQWToSiDist() const {return   m_RichHpdQWToSiDist; }
  void setRichHpdQWToSiDist( const double HpdQWtoSiDist )
  {
    m_RichHpdQWToSiDist=HpdQWtoSiDist;
  }

  double Rich1MaxZHitZCoord() const {return m_Rich1MaxZHitCoord  ; }
  void setRich1MaxZHitZCoord( const double RichMaximumZCoordHit )
  {
    m_Rich1MaxZHitCoord=RichMaximumZCoordHit;
  }

  const std::string & HpdQWLogVolName() const {return m_HpdQWLogVolName; }
  void setHpdQWLogVolName (const std::string & hpdqwLvolname )
  {
    m_HpdQWLogVolName=  hpdqwLvolname;
  }

  const std::string & HpdPhCathodeLogVolName() const {return  m_HpdPhCathodeLogVolName; }
  void setHpdPhCathodeLogVolName (const std::string & hpdphCathLvolname )
  {
    m_HpdPhCathodeLogVolName=hpdphCathLvolname;
  }

  int numberOfRichDetectors() const {return m_numberOfRichDetectors; }
  void setnumberOfRichDetectors (const int numberOfRichdet)
  {
    m_numberOfRichDetectors=numberOfRichdet;
  }

  double HpdPhCathodeInnerRadius() const {return  m_HpdPhCathodeInnerRadius; }

  void setHpdPhCathodeInnerRadius(const double phCaInnRad )
  {
    m_HpdPhCathodeInnerRadius = phCaInnRad;
  }

  void setHpdMaxQuantumEff(const double aMaxQE)
  {
    m_HpdMaxQuantumEff= aMaxQE;
  }

  double HpdMaxQuantumEff() const
  {
    return  m_HpdMaxQuantumEff;
  }

private:

  //here the first vector has a dimension of 2
  // which is the number of rich detectors in lhcb.
  std::vector<int>m_numHpdTotRich;
  std::vector<std::vector<RichHpdQE*> >m_RichHpdQEList;
  std::vector<std::vector<RichHpdPSF*> >m_RichHpdPSFList;
  std::vector<std::vector<RichHpdDeMag*> >m_RichHpdDeMagList;
  double m_RichHpdHighVoltage;
  double m_RichHpdQWToSiDist;
  double  m_Rich1MaxZHitCoord;
  std::string m_HpdQWLogVolName;
  std::string m_HpdPhCathodeLogVolName;
  int m_numberOfRichDetectors;
  double m_HpdPhCathodeInnerRadius;
  int HpdVerboseLevel;
  double m_HpdMaxQuantumEff;

};

#endif
