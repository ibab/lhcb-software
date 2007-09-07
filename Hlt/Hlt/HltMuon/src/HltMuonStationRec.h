// $Id: HltMuonStationRec.h,v 1.4 2007-09-07 19:49:24 sandra Exp $
#ifndef HLTMUONSTATIONREC_H 
#define HLTMUONSTATIONREC_H 1

// Include files

#include "HltMuonRegion.h"
#include "GaudiKernel/IMessageSvc.h" 
/** @class HltMuonStationRec HltMuonStationRec.h
 *  Represent one of the muon stations
 *
 *  @author Olivier Callot
 *  @date   2003-06-06
 */
class HltMuonStationRec {
public:
  /// Standard constructor
  HltMuonStationRec( IMessageSvc *svc ) {p_svc=svc;}; 

  virtual ~HltMuonStationRec( ) {
       //for ( std::vector<HltMuonRegion*>::iterator itR = m_region.begin();
         //       m_region.end() != itR; ++itR ) 
          //delete *itR;          


}; ///< Destructor

  void setZ( double z )        { m_z = z;    }
  double z( )            const { return m_z; }


  void addRegion( HltMuonRegion* reg )  { m_region.push_back( reg ); }
  std::vector<HltMuonRegion*>& region() { return m_region; }

  HltMuonRegion* region( int k ) { return m_region[k]; }

  void clearRegions () {
    for ( std::vector<HltMuonRegion*>::iterator itR = m_region.begin();
          m_region.end() != itR; ++itR ) {
      (*itR)->clearPoints();
      (*itR)->clearTiles();

    }

  }  
  
  void findCoincidence( double x, double y, unsigned int regionBefore,
HltMuonPoint*& ref ) {
  //MsgStream log(p_svc, "findPoint");  
  double deltaYmin = 9999.;
  double deltaXmin = 9999.;
  HltMuonPoint* candidate=NULL;
  for(int region=0;region<4;region++){
    
    for ( std::vector<HltMuonPoint>::iterator itP = 
            m_region[region]->points().begin();
          m_region[region]->points().end() != itP; itP++ ) {
//      log<<MSG::INFO<<region<<" "<<itP->x()<<" "<<
//	itP->y()<<" "<<x- itP->x()<<" "<<y- 
//      itP->y()<<endreq;
      if(fabs(x- itP->x())<1000){
        double deltaY = fabs(y- itP->y());
        if(fabs(deltaY)<fabs(deltaYmin)-10){
        //same margin in Y
          deltaXmin=x-itP->x();
          deltaYmin=deltaY;
          candidate=&(*itP);          
        }else if(fabs(deltaY)<fabs(deltaYmin)+10){
          if(fabs(x-itP->x())<fabs(deltaXmin)-10){
            deltaXmin=x-itP->x();
            deltaYmin=deltaY;
            candidate=&(*itP);
          }
          
        }
      }
    }
  }
  

  if(candidate!=NULL){
//	log<<MSG::INFO<<" fine "<<deltaYmin<<" "<<deltaXmin<<endreq;
  if(fabs(deltaYmin)<m_Ymax[regionBefore]&&
     fabs(deltaXmin)<m_Xmax[regionBefore]){      
    ref=candidate;      
  }   
  }  return;
  
  }
  
  



  
   
  
  void setMaxY(double y,unsigned int region)
  {m_Ymax[region]=y;
  }
  void setMaxX(double x,unsigned int region)
  {m_Xmax[region]=x;
  }


  
protected:

private:
  double      m_z;
  std::vector<HltMuonRegion*> m_region;
  double m_Ymax[4];
  double m_Xmax[4];
IMessageSvc* p_svc;
};
#endif // HLTMUONSTATIONREC_H
