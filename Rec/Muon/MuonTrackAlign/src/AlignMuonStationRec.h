// $Id: AlignMuonStationRec.h,v 1.4 2008-06-30 17:20:28 spozzi Exp $
#ifndef ALIGNMUONSTATIONREC_H 
#define ALIGNMUONSTATIONREC_H 1

// Include file

#include "AlignMuonRegion.h"
#include "GaudiKernel/IMessageSvc.h" 
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"

/** @class AlignMuonStationRec AlignMuonStationRec.h
 *  Represent one of the muon stations
 *
 *  @author Olivier Callot
 *  @date   2003-06-06
 */
class AlignMuonStationRec {
public:
  /// Standard constructor
  AlignMuonStationRec( IMessageSvc *svc, IRndmGenSvc* rnd_svc ) : m_flatDistribution(NULL) {p_svc=svc; p_rnd=rnd_svc;}; 

  virtual ~AlignMuonStationRec( ) {}; ///< Destructor

  void setZ( double z )        { m_z = z;}
  double z( )            const { return m_z; }
  void initialize()
  {
    m_flatDistribution =new Rndm::Numbers;
    m_flatDistribution->initialize(p_rnd, Rndm::Flat(0.0,1.0));

  }
  

  void addRegion( AlignMuonRegion* reg )  { m_region.push_back( reg ); }
  std::vector<AlignMuonRegion*>& region() { return m_region; }

  AlignMuonRegion* region( int k ) { return m_region[k]; }

  void clearRegions () {
    for ( std::vector<AlignMuonRegion*>::iterator itR = m_region.begin();
          m_region.end() != itR; ++itR ) {
      (*itR)->clearPoints();
      (*itR)->clearTiles();

    }

  }  
 void findCoincidence( double x, double y, unsigned int regionBefore, std::vector<AlignMuonPoint> &Vec_candidate ) {
   
   

   double toll=0;
   if(regionBefore==0)toll=2;  
   if(regionBefore==1)toll=4;  
   if(regionBefore==2)toll=8;  
   if(regionBefore==3)toll=10;  
   double deltaYmin = 9999.;
   double deltaXmin = 9999.;
   int counter=0;
   
   AlignMuonPoint* candidate=NULL;
   for(int region=0;region<4;region++){
   
     for ( std::vector<AlignMuonPoint>::iterator itP = m_region[region]->points().begin();
	   m_region[region]->points().end() != itP; itP++ ) { 
       
       
       double deltaX=fabs(x-itP->x());
       double deltaY = fabs(y- itP->y());
       if(fabs(deltaY)<m_Ymax[regionBefore]&&
          fabs(deltaX)<m_Xmax[regionBefore]){      

         if(fabs(x- itP->x())<1000){
           double deltaY = fabs(y- itP->y());

           //if(fabs(deltaY)<fabs(deltaYmin)-toll){ //originale
           if(fabs(deltaY)<fabs(deltaYmin)&&fabs(deltaY)<fabs((fabs(deltaYmin)-toll))){//same margin in Y

             deltaXmin=x-itP->x();
             deltaYmin=deltaY;
             candidate=&(*itP);          
             counter++;
           }else if(fabs(deltaY)<fabs((fabs(deltaYmin)+toll))){
             //if(fabs(x-itP->x())<fabs(deltaXmin)-toll){
             
             if(fabs(x-itP->x())<fabs((fabs(deltaXmin)-toll))){              
              deltaXmin=x-itP->x();
              deltaYmin=deltaY;
              candidate=(&(*itP));
              counter++;
             }else if(fabs(fabs(deltaXmin)-fabs(deltaX))<0.1){
               //choose on random
               if((*m_flatDistribution)()>0.5){
                 deltaXmin=x-itP->x();
                 deltaYmin=deltaY;
                 candidate=(&(*itP));
                 counter++;
                 
               }
               
             }
             
           }          
         }                   
       }      
     }
   }
   
   double pad_x[5][4] ={{10.1667,20.1667 ,40.333, 80.6667},
			{6.41667, 12.75,  25.5, 51.},
			{6.91667, 13.75,  27.5, 55.},
			{29.6667,  59., 118., 236.},
			{31.6667,  63., 126., 252}};
   double pad_y[5][4] ={{24.7586,49.7584 ,99.7581 , 199.757},
			{31.3585, 62.9583, 126.158, 252.557},
			{33.8585, 67., 136.158, 272.557}, 
			{36.3585, 72.9583, 146.158, 292.557},
			{38.8585, 77.9582, 156.158, 312.557}};
   
   if(candidate!=NULL){
     int Pcounter =0;
     
     Vec_candidate.push_back(*candidate);
    //     return;   // Uncomment if you don't want to add neighbour pads to the track
     
     
     for(int region=0;region<4;region++){
       for ( std::vector<AlignMuonPoint>::iterator itP = m_region[region]->points().begin();
             m_region[region]->points().end() != itP; itP++ ) { 
         
	 double deltaX=fabs(candidate->x()-itP->x());
	 double deltaY = fabs(candidate->y()- itP->y());
	 if(deltaX<2&&deltaY<2) continue;
         
	 if(deltaX < ((pad_x[candidate->tile().station()][candidate->tile().region()]+
		       pad_x[(itP)->tile().station()][(itP)->tile().region()]) /2.)*1.1 &&
	    deltaY < ((pad_y[candidate->tile().station()][candidate->tile().region()]+
		       pad_y[(itP)->tile().station()][(itP)->tile().region()])/2.)*1.1 ) {
	   Pcounter ++;
	   Vec_candidate.push_back((*itP));
           
         }
       }
     }
   }
   
   return; 
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
  std::vector<AlignMuonRegion*> m_region;
  double m_Ymax[4];
  double m_Xmax[4];
  IMessageSvc* p_svc;
  IRndmGenSvc* p_rnd;
  Rndm::Numbers* m_flatDistribution;

};
#endif // ALIGNMUONSTATIONREC_H
