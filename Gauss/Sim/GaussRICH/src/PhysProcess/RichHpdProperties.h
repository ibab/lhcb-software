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

       int numHpdTotRich1() {return  m_numHpdTotRich[0]; }
       int numHpdTotRich2() {return  m_numHpdTotRich[1]; }
       std::vector<int>numHpdTotRich() {return  m_numHpdTotRich; }
       std::vector<RichHpdQE*>RichHpdQEList(int richdetnum) 
       {return  m_RichHpdQEList[richdetnum]; }  
       std::vector<RichHpdPSF*>RichHpdPSFList(int richdetnum) 
       {return m_RichHpdPSFList[richdetnum]; }
       std::vector<RichHpdDeMag*>RichHpdDeMagList(int richdetnum ) 
       {return m_RichHpdDeMagList[richdetnum]; }

       RichHpdQE* getRichHpdQE(int hpdnum, int richdetnum);

       RichHpdPSF* getRichHpdPSF(int hpdnum, int richdetnum );
       
       RichHpdDeMag* getRichHpdDeMag(int hpdnum, int richdetnum );

       double RichHpdHighVoltage() {return  m_RichHpdHighVoltage; }
       void setRichHpdHighVoltage(double HpdHv);

       double RichHpdQWToSiDist() {return   m_RichHpdQWToSiDist; }
       void setRichHpdQWToSiDist( double HpdQWtoSiDist ) ;      

       double Rich1MaxZHitZCoord() {return m_Rich1MaxZHitCoord  ; }
       void setRich1MaxZHitZCoord( double RichMaximumZCoordHit ) ;      

       string HpdQWLogVolName() {return m_HpdQWLogVolName; }
       void setHpdQWLogVolName (string hpdqwLvolname );

       string HpdPhCathodeLogVolName() {return  m_HpdPhCathodeLogVolName; }
       void setHpdPhCathodeLogVolName (string hpdphCathLvolname ); 

       int numberOfRichDetectors() {return m_numberOfRichDetectors; }
       void setnumberOfRichDetectors (int numberOfRichdet);
       double HpdPhCathodeInnerRadius() {return  m_HpdPhCathodeInnerRadius; }
       void setHpdPhCathodeInnerRadius(double phCaInnRad );
      
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
  string m_HpdQWLogVolName;
  string m_HpdPhCathodeLogVolName;
  int m_numberOfRichDetectors;
  double m_HpdPhCathodeInnerRadius;
  int HpdVerboseLevel;
  
};

#endif
