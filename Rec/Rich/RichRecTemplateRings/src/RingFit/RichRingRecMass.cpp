// Include files 
#include "Event/RichRecTrack.h"


// local
#include "RichRingRecMass.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRingRecMass
//
// 2007-07-06 : Sajan EASO
//-----------------------------------------------------------------------------

using namespace Rich::Rec::TemplateRings;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RichRingRecMass )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichRingRecMass::RichRingRecMass( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  :RichRingRecToolBase  ( type, name , parent )
{
  declareInterface<IRichRingRecMass>(this);  
    
}
void RichRingRecMass::RichReconMassForTrackSegment(int itk, int irad ){
  
  double aRadius =  RRslt()->TrackFoundMeanRadiusValue(itk,irad);
  double aMass = Tfm()->getTrackMassFromRadius(itk,irad,aRadius);
  // get the BG for debug purposes
  double aBG = Tfm()->getBGFromRadius( irad, aRadius);
  
  RRslt()->setTrackSegReconMass(itk,irad,aMass);
  RRslt()->setTrackSegReconBG(itk,irad,aBG);
  
}

StatusCode  RichRingRecMass::bestReconstructedMassForRichTrack(VI SegmentIndex, int currentTrackIndex ){
  StatusCode sc= StatusCode::SUCCESS;
   
  // first check if the track is below the threshold in each radiator

  int nrd = RConst()->maxNumRadiator();
  VB SegSelected (nrd, false);
  VD SegBGVect(nrd , -10.0);
  VD SegMomVect(nrd , -10.0);
  VD SegMassVect(nrd, -100.0 );
  VD SegRadiusVect(nrd, -100.0);
  
  VI aTkSegIndexInRadiator (nrd, -100);
  
  //  info()<<"Best mass select: Current track Num "<< currentTrackIndex<<endmsg;
  int arcount=0;
  VB  radiatorPresent(3, false);
  
   for(int irad = RParam()->MaxRadiator(); irad >= RParam()->MinRadiator();    --irad){
     
     int tkIndexInRad=  tgD()->FindIndexValue(SegmentIndex[irad], irad);
     if(  tkIndexInRad >= 0 ) {
       double aradius =  RRslt()->TrackFoundMeanRadiusValue(tkIndexInRad,irad);
       double aMass =  RRslt()->TrackSegReconMassValue(tkIndexInRad,irad);
       double aRecMom = tgD()-> TrackRecMomValue(tkIndexInRad,irad);
       double aRecBg =  RRslt()->TrackSegReconBGValue(tkIndexInRad,irad);
       
       if( (aradius > 0.0) && (aMass > 0.0) && (aRecBg > 0.0) && (aRecMom > 0.0)  ) {

         arcount++;
         
         SegBGVect [irad ]  =  aRecBg ;
         SegMassVect [irad ] = aMass;
         SegMomVect  [irad] = aRecMom;
         SegRadiusVect [irad] = aradius;
         radiatorPresent[irad]= true;
         aTkSegIndexInRadiator[irad] = tkIndexInRad;         
         
       }
       
       
     }
     
     
     
   } // end loop through radiators
   int aRadSelectedLight=-1;
   int aRadSelectedHeavy=-1;
   
   if( arcount > 0 ) {

     // case 1 only one radiator present   
     if( arcount == 1 ) {
       if(radiatorPresent[0]) aRadSelectedLight=0;
       if(radiatorPresent[1]) aRadSelectedLight=1;
       if(radiatorPresent[2]) aRadSelectedLight=2;
       aRadSelectedHeavy=aRadSelectedLight;       
     }else {
       // multiple radiators
       aRadSelectedLight=SelectRadiatorFromSegReconLight( SegMomVect, SegBGVect, SegRadiusVect, SegMassVect ,
                                                          radiatorPresent, arcount);             
       aRadSelectedHeavy=SelectRadiatorFromSegReconHeavy( SegMomVect, SegBGVect, SegRadiusVect, SegMassVect ,
                                                           radiatorPresent,arcount);
       
     }  
     
   }   
   
   if(  aRadSelectedLight > -1 ) {
     RRslt()->StoreBestReconMassInfo(  SegMassVect[aRadSelectedLight],aRadSelectedLight, 
                        aTkSegIndexInRadiator[aRadSelectedLight],currentTrackIndex, 0 );
     
   }
   if( aRadSelectedHeavy > -1 ) {  
     RRslt()->StoreBestReconMassInfo(  SegMassVect[aRadSelectedHeavy],aRadSelectedHeavy, 
                                       aTkSegIndexInRadiator[aRadSelectedHeavy],currentTrackIndex, 1 );
   }
   
   if(aRadSelectedLight== -1 &&  aRadSelectedHeavy == -1 ) {     
     verbose()<<" RichRingRecMass: No best mass found for richrectracknum segments  "
           << currentTrackIndex<< "   "<< SegmentIndex<< endmsg;   }
   
   RRslt()->StoreSelectedTrackIndex(aTkSegIndexInRadiator, currentTrackIndex);

   return sc;
   
}


int RichRingRecMass::SelectRadiatorFromSegReconLight( VD SegMomVect, VD SegBGVect, 
                 VD SegRadiusVect, VD SegMassVect,VB radiatorPresent, int arcount){

  double aPionThrInRich1Gas = RConst()-> RichParticleCherenkovThreshold(1,2);  

  int iradSel =-1;
  bool aRadSelected=false;
  if(arcount > 1) {

   // case A:
  // check for aerogel threshold in momentum.  if momentum below pion threshold for rich1gas
  // always select aerogel, if it exists.
      int irad=0;
      while (irad < 3 && (!aRadSelected) ) {
        if(  SegRadiusVect [irad] > 0.0 && SegMassVect [irad ] > 0.0 && SegMomVect [irad ] >0.0 ) {
          if(  SegMomVect [irad ] < aPionThrInRich1Gas ) { // always select aerogel
            iradSel =0;
            aRadSelected=true;
          }
        }
        irad++;   
      }
    
  // case B:
  // two radiators are present, but momentum above pion thr in Rich1Gas.
      if(!aRadSelected && (arcount ==2) ) {
         if( !radiatorPresent[0]) {
                         iradSel =SelectFromPairOfBGA ( SegBGVect [1],   SegBGVect [2],SegMomVect[1], 
                                           SegMassVect [1 ], SegMassVect [2 ], 1,2 );              

         } else if( !radiatorPresent[2]){
                         iradSel =SelectFromPairOfBGA ( SegBGVect [0],   SegBGVect [1],SegMomVect[1],
                                           SegMassVect [0 ], SegMassVect [1 ], 0,1 );
                         
         }else if ( !radiatorPresent[1]){
                         if( SegMomVect[2]  < (RConst()->RichParticleCherenkovThreshold(2,2)) ) {
                            iradSel =0;
             
                        }else {
                         iradSel =SelectFromPairOfBGA ( SegBGVect [0],   SegBGVect [2],SegMomVect[2],
                                           SegMassVect [0 ], SegMassVect [2 ], 0,2 );
                        }
           
         }
         
         if(iradSel > -1 ) aRadSelected=true;
         
      }
      
      
      // case C
      // three radiators present, bt momentum above pion the in rich1Gas
    
      if( ( !aRadSelected)  && (arcount ==3) ) { 
      
        iradSel = SelectFromPairOfBGA ( SegBGVect [1], SegBGVect [2], SegMomVect[1],
                                       SegMassVect [1], SegMassVect [2], 1,2);
              
        aRadSelected=true;
        
      }   
      
  }

  return iradSel;
  
}

int RichRingRecMass::SelectRadiatorFromSegReconHeavy( VD SegMomVect, VD SegBGVect, 
                 VD SegRadiusVect, VD SegMassVect,VB radiatorPresent, int arcount){

  double aPionThrInRich1Gas = RConst()-> RichParticleCherenkovThreshold(1,2);  

  int iradSel =-1;
  bool aRadSelected=false;
  if(arcount > 1) {

   // case A:
  // check for aerogel threshold in momentum.  if momentum below pion threshold for rich1gas
  // always select aerogel, if it exists.
      int irad=0;
      while (irad < 3 && (!aRadSelected) ) {
        if(  SegRadiusVect [irad] > 0.0 && SegMassVect [irad ] > 0.0 && SegMomVect [irad ] >0.0 ) {
          if(  SegMomVect [irad ] < aPionThrInRich1Gas ) { // always select aerogel
            iradSel =0;
            aRadSelected=true;
          }
        }
        irad++;   
      }
    
  // case B:
  // two radiators are present, but momentum above pion thr in Rich1Gas.
      if(!aRadSelected && (arcount ==2) ) {
         if( !radiatorPresent[0]) {
                         iradSel =SelectFromPairOfBGB ( SegBGVect [1],   SegBGVect [2],SegMomVect[1], 
                                           SegMassVect [1 ], SegMassVect [2 ], 1,2 );              

         } else if( !radiatorPresent[2]){
                         iradSel =SelectFromPairOfBGB ( SegBGVect [0],   SegBGVect [1],SegMomVect[1],
                                           SegMassVect [0 ], SegMassVect [1 ], 0,1 );
                         
         }else if ( !radiatorPresent[1]){
                         if( SegMomVect[2]  < (RConst()->RichParticleCherenkovThreshold(2,2)) ) {
                            iradSel =0;
             
                        }else {
                         iradSel =SelectFromPairOfBGB ( SegBGVect [0],   SegBGVect [2],SegMomVect[2],
                                           SegMassVect [0 ], SegMassVect [2 ], 0,2 );
                        }
           
         }
         
         if(iradSel > -1 ) aRadSelected=true;
         
      }
      
      
      // case C
      // three radiators present, bt momentum above pion the in rich1Gas
    
      if( ( !aRadSelected)  && (arcount ==3) ) {

        if( SegMomVect[2]  > (RConst()->RichParticleCherenkovThreshold(2,3)) ) {                
            iradSel = SelectFromPairOfBGB ( SegBGVect [1], SegBGVect [2], SegMomVect[1],
                               SegMassVect [1], SegMassVect [2], 1,2);        
        }else {          
            iradSel = SelectFromPairOfBGB ( SegBGVect [0], SegBGVect [1], SegMomVect[1],
                                       SegMassVect [0], SegMassVect [1], 0,1);
        }
        
        
        aRadSelected=true;
        
      }   
      
  }

  return iradSel;
  
}

int RichRingRecMass::SelectFromPairOfBGA (double aBG1, double aBG2, double aMom , 
                                         double aMass1, double aMass2, int irad1, int irad2){  
   int aradSel = irad2;  
   double aMaxBGLim=RConst()->MaxBGLim();
   double aRichHeavyMassBegin = RConst()->RichHeavyMassBegin();

   debug()<<"Rich Select par from BG "<< aBG1<<"  "<< aBG2<<"  "<<aMom<<"  "
          <<aMass1<<"  "<<aMass2<<"   "<<irad1<<"  "<<irad2<<"  "<<aRichHeavyMassBegin<<endmsg;
   
   // test selection   
   //   if( ( aMom < RConst()->RichParticleCherenkovThreshold(irad2,3)) &&
   //      ( aMass1 >  aRichHeavyMassBegin )  ) {
   //    aradSel =1;
   // } else if ( ( aMom < RConst()->RichParticleCherenkovThreshold(irad1,3)) &&( aMass2 >  aRichHeavyMassBegin )  ) {
   //     aradSel =2;
   // }else {
   // end test selection

      if( (aBG1 > aMaxBGLim) && (aBG2 <  aMaxBGLim) ) {
             aradSel =irad2;
      }else if ( (aBG1 < aMaxBGLim) && (aBG2 >  aMaxBGLim) ) {
             aradSel = irad1;
      }else if ( aBG1 < aBG2 ) {
             aradSel =irad2;
      }else {
             aradSel =irad1;
      }
      //   } 
   return aradSel;  
}

int RichRingRecMass::SelectFromPairOfBGB (double aBG1, double aBG2, double aMom , 
                                         double aMass1, double aMass2, int irad1, int irad2){  
   int aradSel = 1;  
   double aMaxBGLim=RConst()->MaxBGLim();
   double aRichHeavyMassBegin = RConst()->RichHeavyMassBegin();

   debug()<<"Rich Select par from BG "<< aBG1<<"  "<< aBG2<<"  "<<aMom<<"  "
          <<aMass1<<"  "<<aMass2<<"   "<<irad1<<"  "<<irad2<<"  "<<aRichHeavyMassBegin<<endmsg;
   
   // test selection   
   if( ( aMom < RConst()->RichParticleCherenkovThreshold(irad2,3)) ) {
     
       if(   aMass2 >  aRichHeavyMassBegin  ) {
         aradSel =irad1;
       }else if ( aMass2 <=  aRichHeavyMassBegin   ) {
         if( aBG1 < aBG2) {
           aradSel =irad1;
         }else {           
           aradSel =irad2;
         }
         
       }   
       
   } else if ( aMom < RConst()->RichParticleCherenkovThreshold(irad1,3) ) {
     
         if ( aMass1 >  aRichHeavyMassBegin   ) {
            aradSel =irad2;
         }else if (  aMass1 <=  aRichHeavyMassBegin   ) {
            if( aBG1 < aBG2) {
             aradSel =irad1;
            }else {           
             aradSel =irad2;
            }   
         }         
         

    }else {
   // end test selection

      if( (aBG1 > aMaxBGLim) && (aBG2 <  aMaxBGLim) ) {
             aradSel =irad2;
      }else if ( (aBG1 < aMaxBGLim) && (aBG2 >  aMaxBGLim) ) {
             aradSel = irad1;
      }else if ( aBG1 < aBG2 ) {
             aradSel =irad2;
      }else {
             aradSel =irad1;
      }
    }
     
   return aradSel;  
}




  

//=============================================================================
// Destructor
//=============================================================================
RichRingRecMass::~RichRingRecMass() {} 

//=============================================================================
