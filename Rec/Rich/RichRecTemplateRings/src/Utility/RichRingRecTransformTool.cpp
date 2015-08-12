// $Id: RichRingRecTransformTool.cpp,v 1.4 2009-07-30 11:22:55 jonrob Exp $
// Include files 
#include<TMath.h>

// local
#include "RichRingRecTransformTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRingRecTransformTool
//
// 2007-06-12 : Sajan EASO
//-----------------------------------------------------------------------------

using namespace Rich::Rec::TemplateRings;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RichRingRecTransformTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichRingRecTransformTool::RichRingRecTransformTool( const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent )
  : RichRingRecToolBase ( type, name , parent ),
    m_AgelRadiusToBGParam(VD(0)),
    m_AgelBGToRadiusParam(VD(0)),
    m_R1gRadiusToBGParam(VD(0)),
    m_R1gBGToRadiusParam(VD(0)),
    m_R2gRadiusToBGParam(VD(0)),
    m_R2gBGToRadiusParam(VD(0)),
    m_MinBGFit(VD(0))
{
  declareInterface<IRichRingRecTransformTool>(this);

  InitRichRingRecTransformTool();
  
}
void RichRingRecTransformTool::InitRichRingRecTransformTool()
{
  m_NumPeakSearch=8; // max number of peaks to search through.
  setRadiusToBGParam();
  setBGToRadiusParam();
  setRadiusFitLimits ();
  
}

double RichRingRecTransformTool::CartToPolRad(double x, double y, double aXc, double aYc ){
  double rsq= ( (x-aXc)*(x-aXc) ) +  ( (y- aYc)*(y- aYc) );
  double r= 0.0;
  if( rsq > 0.0 ) r= pow(rsq,0.5);  
  return r;  
}
double RichRingRecTransformTool::CartToPolPhi(double x, double y, double aXc, double aYc ){
  double th= atan2( (y- aYc) ,  (x-aXc));
  if( th < 0.0 ) th += TMath::TwoPi();
  return th;    
}

double RichRingRecTransformTool::CartToPolRadFromTHNum(int aHitNum, int aTrackNum , int rad){
  double radius=0;
  int iRich= RichDetNumFromRadiator(rad);
  if( (  ( tgD()->   NumChRdTrackValue(rad))  > 0 ) &&  ( ( tgD() ->  NumHitsTarget(iRich))> 0)) {
    double aHtY = getYHitCoord(aHitNum,aTrackNum ,rad,iRich);    
    radius = CartToPolRad(tgD()->XHitInpValue(aHitNum,iRich) , aHtY,
                          tgD()->TrackXcInpValue(aTrackNum,rad), tgD()->TrackYcInpValue(aTrackNum,rad));
  }
  
  return radius;
}

double RichRingRecTransformTool::CartToPolPhiFromTHNum(int aHitNum, int aTrackNum , int rad){
  double aPhi=0.0;
  int iRich= RichDetNumFromRadiator(rad);
  if( (  ( tgD()->   NumChRdTrackValue(rad))  > 0 ) &&  ( ( tgD() ->  NumHitsTarget(iRich))> 0)) {  
    double aHtY = getYHitCoord(aHitNum,aTrackNum ,rad,iRich);
    aPhi= CartToPolPhi(tgD()->XHitInpValue(aHitNum,iRich) , aHtY ,
                         tgD()->TrackXcInpValue(aTrackNum,rad), tgD()->TrackYcInpValue(aTrackNum,rad)); 
  }  

  return aPhi;
}

double RichRingRecTransformTool::getYHitCoord(int aHitNum, int aTrackNum , int rad, int iRich){
    double aHtY = tgD()-> YHitInpValue(aHitNum,iRich);
    
    if(rad == 0 ) {
      double aTkY = tgD()->TrackYcInpValue(aTrackNum,rad);
      if(  aTkY > 0.0 &&  aHtY < 0.0 ) {
        aHtY +=  RParam()->YAgelShift();        
      } else if ( aTkY < 0.0 &&  aHtY > 0.0 ) {
        aHtY -= RParam()->YAgelShift();
      }
    }
    
    
    return aHtY;
}

double  RichRingRecTransformTool::XDiffValue(int aHitNum, int aTrackNum, int  rad, int iRich){
  return (( tgD()->XHitInpValue(aHitNum,iRich))  -  ( tgD()->TrackXcInpValue(aTrackNum,rad)) );
  
}
double  RichRingRecTransformTool::YDiffValue(int aHitNum, int aTrackNum, int  rad, int iRich){
  
  double aTkY =  tgD()->TrackYcInpValue(aTrackNum,rad);
  double aHtY =  getYHitCoord(aHitNum,aTrackNum , rad,iRich);
  
  return (aHtY - aTkY);
  
}

int RichRingRecTransformTool::RichDetNumFromRadiator(int rad ){
  int iRich=0;
  if(rad== 2) {iRich =1 ; }
  return iRich;
}
int RichRingRecTransformTool::RichDetNumFromHitZ(double GlobalZHit){
  int iRich=0;
  if(GlobalZHit >   ( RConst()->Rich1Rich2ZLimit()) ) {iRich = 1; }
  return iRich;
  
}
int  RichRingRecTransformTool::SearchIndexInVect (VD aVect , double aVal){

  int aSize= (int) aVect.size();
  int aPos = aSize;
  if( aSize > 0 ) {

  VD_it  aMinElem = min_element(aVect.begin(),aVect.end());
  VD_it  aMaxElem = max_element(aVect.begin(),aVect.end());
  bool aPosFound=false;
  if( aVal <= *aMinElem || aVal <= aVect[0] ) { aPos=0;aPosFound=true; }
  if( aVal >= *aMaxElem || aVal >= aVect[aSize-1] ) {aPos=aSize; aPosFound=true;}
    if( !aPosFound) {
       int i=0;
       while(  (i< (aSize -1)) && (!aPosFound) ) {
          if( (aVal >= aVect[i]) && (aVal < aVect[i+1]) ) {
            aPos=i+1;
            aPosFound=true;
          }
        i++;
       }
    }
    
  }
  

  return aPos;
  

}

VD RichRingRecTransformTool::GetCorrelation( VD aTar, VD aTemp){
  
   int aLogicalSize =  (int) (aTemp.size())/2;
  // Here we assume that the aTar and aTemp have the same sizes.

  VD aCorr (aLogicalSize*2); 
  for(int af=0; af< aLogicalSize ; af++) {
    double ar=  aTar[2*af];     double ai=  aTar[2*af+ 1]; 
    double br=  aTemp[2*af];     double bi=  aTemp[2*af+1]; 

    double du= ar*br + ai*bi;   
    double dw= -ar*bi + ai*br;    

    double su = ar*ar + ai* ai ;   double sv = br*br + bi*bi; 



    double aCCDenSq=  su * sv;
    double aCCDen =0.0;
    if(  aCCDenSq > 0.0 ) {
        aCCDen =  pow( (su * sv) , 0.5 );
    }
    double aNur  =   du;
    double aNui  =   dw;
    if( fabs (aNur) < 0.000001) aNur=0.0;
    if( fabs (aNui) < 0.000001) aNui=0.0;
      
      double aRatioReal = 0.0; double aRatioIm =0.0;
 
      if( aCCDen != 0.0 ) { 
          aRatioReal = aNur/aCCDen; 
          aRatioIm = aNui/aCCDen;
          
      }
      aCorr[2*af]   = aRatioReal;
      aCorr[2*af+1] = aRatioIm;

      //  info<<" RichRingRecon Corr out bin real im "<<af<<"   "<<aRatioReal  <<"   "<<aRatioIm<<endmsg;

  }  
  
  return aCorr;
 
}

VI  RichRingRecTransformTool::FindPeakInRP(VVD aPolVec , int rad ){
  double aThr = RConst()->ThrCorrVectValue(rad);
  VI aPeak = getMeanPeakPolarInvA(aPolVec,aThr );
  
  return aPeak;
}


VI  RichRingRecTransformTool::getMeanPeakPolarInvA(VVD aPolVec, double  aThrValue ){


  int aIndexXSave=-1;
  int aIndexYSave=-1;
  int aIndexFound=0;
  double aSumSave= 0.0;

  int xdim = (int) aPolVec.size();
  int ydim = (int) aPolVec [0].size(); // the ydim is same for each row of xdim.
  
  // create a 1d vector with values above threshold
  VD aPola ( xdim*ydim, 0.0);
  for(int ix=0; ix< xdim; ++ix) {
    for(int iy=0; iy<ydim; ++iy) {
      int iloc= ix*ydim+iy;
      double aval= fabs(aPolVec [ix] [iy]);
      if( aval > aThrValue  ) {
        aPola[iloc]= aPolVec [ix] [iy];
      } 
    }
  }

  // now loop through for example 8 peaks.

  for(int ipeak=0; ipeak< m_NumPeakSearch; ++ipeak) {

   VD_it amax= max_element(aPola.begin(), aPola.end());

   int aiPeak =  (int) (find(aPola.begin(), aPola.end(), *amax)-aPola.begin()); 
   int axpeak = aiPeak/ydim;
   int aypeak =  aiPeak- axpeak*ydim; 
   // not the best algorithm; but first find the limits in x and y.
   int m_PeakSLimitX=2; int m_PeakSLimitY=8; int aXmin=0; int aXmax=0; int aYmin=0; int aYmax=0;
   int ipx= 1; int ipy= 1;

   // now for X
   bool LmxFoundX=false; bool LmnFoundX=false;
   bool LmxFoundY=false; bool LmnFoundY=false;
   while (ipx < m_PeakSLimitX)  {

      if(!  LmxFoundX ) {
      	int aposx= (axpeak+ipx);
        if(aposx < xdim ) {
         int aposxioi= aposx*ydim+aypeak;         
         aXmax= ipx;

      	 if(aPola[aposxioi] == 0.0 ) { LmxFoundX=true;  }
        }  
      }
      
      if(!  LmnFoundX ) {
       int anegx= (axpeak-ipx);
       if(anegx > 0 ) {
          int anegxioi= anegx*ydim + aypeak;
          aXmin= ipx;
          if(aPola[anegxioi] == 0.0 ) {LmnFoundX=true; }
       }
    }
    
    ipx++;
    
   } 

   // now for Y 
  int icountYpos=0;
  int icountYneg=0;
  int icountYLim=6;
  while ( ipy < m_PeakSLimitY ) {
    if(!LmxFoundY ) {
       int aposy= (aypeak+ipy);
       if(aposy< ydim ) {
       	 int aposyioi = axpeak*ydim+ aposy;
         aYmax=ipy;
         if( aPola[aposyioi] == 0.0 ) {icountYpos++;}
         if(icountYpos > icountYLim) {LmxFoundY =true; } 
       }
    }
    if(!LmnFoundY ) {
       int anegy= (aypeak-ipy);
       if(anegy > 0 ) {
	        int anegyioi = axpeak*ydim+ anegy;
          aYmin=ipy;
          if( aPola[anegyioi] == 0.0 ){icountYneg++;}
          if(icountYneg > icountYLim)  { LmnFoundY =true; } 
       }
    }
    ipy++;
  }
 
 // now loop through the values to get the mean and the area under the peak. 
  double aSum=0;
  double aWeightSumX=0.0;
  double aWeightSumY=0.0;
  int BinSum=0;
  //  int aNumSum=0; 
  int aMeanBinx=0; int aMeanBiny=0;
  for(int ixx=axpeak-aXmin; ixx< axpeak+aXmax+1; ++ixx ) {
    for(int iyy= aypeak-aYmin; iyy< aypeak+aYmax+1; ++iyy) {
      int ioi= ixx*ydim+iyy;
      if( aPola[ioi] !=0.0 ) {
      aSum += fabs(aPola[ioi]);
      aWeightSumX+= fabs(aPola[ioi])*ixx;
      aWeightSumY+= fabs(aPola[ioi])*iyy; 
      if(  aPola[ioi] !=0.0 ) BinSum++;
    	  aPola[ioi]=0.0;
      }
    }
    
  }
  
 if(aSum !=0.0 ){  
         aMeanBinx = (int) ( aWeightSumX /aSum) ; 
         aMeanBiny = (int) ( aWeightSumY /aSum) ;
         if( aSum > aSumSave ) {
             aSumSave= aSum;
             aIndexXSave=aMeanBinx;
             aIndexYSave=  aMeanBiny;
             aIndexFound=1;  
         }
 }

  } // end of loop over peaks
   
  VI aVec(3);
  aVec[0] = aIndexXSave;
  aVec[1] = aIndexYSave;
  aVec[2] = aIndexFound;
    return aVec;  
}
VI RichRingRecTransformTool::getTrackIndexLimits( int irad) 
{
    int tkMin=0; 
    int tkMax = (int) tgD()-> NumChRdTrackValue(irad);
    if(RParam()-> ActivateMaxNumberOfTrackSegmentsInRadiator() ) {
      // Set a max limit on the upper limit on the segments to reduce cpu time.
      if(tkMax > 0 ) {
        int currentMax =tkMax;
        if(irad ==0 ) {
          currentMax = RParam()-> MaxNumTrackSegmentsInAerogel() ;
        }else if (irad ==1 ) {
          currentMax= RParam()-> MaxNumTrackSegmentsInR1gas() ;          
        }else if (irad ==2 ) {
          currentMax = RParam()-> MaxNumTrackSegmentsInR2gas() ;
        }
        if(tkMax > currentMax ) tkMax =currentMax;
      }
      
    } // end activation on the max number of segments per radiator.
    
    

    if(RParam()-> ActivateSingleTrackDebug()) {
      int aMin= RParam()->  MinTrackNumForDebug();
      int aMax = RParam()-> MaxTrackNumForDebug();
      if(tkMax > 0 ) {
        
       if(aMin < tkMax) tkMin= aMin;
      
       if(aMax < tkMax) tkMax = aMax+1;
       
      }
      
    }
    
    VI aIndex (2);
    aIndex [0]= tkMin;
    aIndex [1] = tkMax;
    return aIndex; 
}
void RichRingRecTransformTool::setRadiusFitLimits (){
  m_AgelBGTolerence=1.0;
  m_R1gBGTolerence=1.0;
  //  m_R2gBGTolerence=1.0;
  m_R2gBGTolerence=2.0;
  

  m_AgelMinRadiusFit=110.0;
  m_AgelMaxRadiusFit=320.0;
  m_R1gMinRadiusFit= 3.0;
  m_R1gMaxRadiusFit= 80.0;
  m_R2gMinRadiusFit= 20.0;
  m_R2gMaxRadiusFit= 115.0;
  
  
    m_AgelMinBGFit = 4.0;
    m_AgelMaxBGFit = 16.0;
    m_R1gMinBGFit = 16.0;
    m_R1gMaxBGFit = 80.0;
    //    m_R2gMinBGFit = 30.0;    
    m_R2gMinBGFit = 32.0;    
    m_R2gMaxBGFit = 100.0;
  

    m_MinBGFit.clear();
    m_MinBGFit.push_back( m_AgelMinBGFit);
    m_MinBGFit.push_back( m_R1gMinBGFit);
    m_MinBGFit.push_back( m_R2gMinBGFit);
    


}

void RichRingRecTransformTool::setRadiusToBGParam(){
  // first for Aerogel
  double p0a=4.188;
  double p1a=0.1822;
  double p2a= 57.55;
  double p3a=0.08115;
  double p4a = 27.18;
  m_AgelRadiusToBGParam.clear();
  m_AgelRadiusToBGParam.push_back(p0a);
  m_AgelRadiusToBGParam.push_back(p1a);
  m_AgelRadiusToBGParam.push_back(p2a);
  m_AgelRadiusToBGParam.push_back(p3a);
  m_AgelRadiusToBGParam.push_back(p4a);


  // now for Rich1Gas
  double p0b = -44.5;
  double p1b = 0.04465;
  double p2b = -1.681;
  double p3b = 0.05926;
  double p4b = 3.666;
  double p5b = -0.07339;
  double p6b = -4.155;
  m_R1gRadiusToBGParam.clear();
  m_R1gRadiusToBGParam.push_back(p0b);
  m_R1gRadiusToBGParam.push_back(p1b);
  m_R1gRadiusToBGParam.push_back(p2b);
  m_R1gRadiusToBGParam.push_back(p3b);
  m_R1gRadiusToBGParam.push_back(p4b);
  m_R1gRadiusToBGParam.push_back(p5b);
  m_R1gRadiusToBGParam.push_back(p6b);
 
  // now for Rich2Gas;

  double p0c = 31.4;
  double p1c = 0.04195;
  double p2c = -0.2419;
  double p3c =0.015;
  double p4c = 0.9777;
  double p5c = 0.02128;
  double p6c = -0.4257;
  double p7c = 0.003131;
  double p8c = -0.2419;
  
  m_R2gRadiusToBGParam.clear();
  m_R2gRadiusToBGParam.push_back(p0c);
  m_R2gRadiusToBGParam.push_back(p1c);
  m_R2gRadiusToBGParam.push_back(p2c);
  m_R2gRadiusToBGParam.push_back(p3c);
  m_R2gRadiusToBGParam.push_back(p4c);
  m_R2gRadiusToBGParam.push_back(p5c);
  m_R2gRadiusToBGParam.push_back(p6c);
  m_R2gRadiusToBGParam.push_back(p7c);
  m_R2gRadiusToBGParam.push_back(p8c);

  
}
void RichRingRecTransformTool::setBGToRadiusParam(){
  // first for Aerlgel

  double q0a = -1403.0;  
  double q1a = 0.3684;
  double q2a  = -2.807;
  double q3a  = -0.0003936;
  double q4a = 2.462;
  double q5a = 0.3679;
  double q6a = 0.3866;
  m_AgelBGToRadiusParam.clear();
  m_AgelBGToRadiusParam.push_back(q0a);
  m_AgelBGToRadiusParam.push_back(q1a);
  m_AgelBGToRadiusParam.push_back(q2a);
  m_AgelBGToRadiusParam.push_back(q3a);
  m_AgelBGToRadiusParam.push_back(q4a);
  m_AgelBGToRadiusParam.push_back(q5a);
  m_AgelBGToRadiusParam.push_back(q6a);
  
  // now for Rich1Gas

  double q0b =  -628.5;
  double q1b =  0.1174;
  double q2b = 11.36;  
  double q3b =0.01274;
  double q4b = 9.083;
  double q5b = 0.09168;
  double q6b = 0.4165;

  m_R1gBGToRadiusParam.clear();
  m_R1gBGToRadiusParam.push_back(q0b);
  m_R1gBGToRadiusParam.push_back(q1b);
  m_R1gBGToRadiusParam.push_back(q2b);
  m_R1gBGToRadiusParam.push_back(q3b);
  m_R1gBGToRadiusParam.push_back(q4b);
  m_R1gBGToRadiusParam.push_back(q5b);
  m_R1gBGToRadiusParam.push_back(q6b);

  // now for RICH2 gas  
  double q0c=-586.6;
  double q1c=0.05864;  
  double q2c=-1.291;
  double q3c=-0.00002593;
  double q4c=1.745;
  double q5c=0.06335;
  double q6c=1.072;
  double q7c=0.005147;
  double q8c=-1.291;

  m_R2gBGToRadiusParam.push_back(q0c);
  m_R2gBGToRadiusParam.push_back(q1c);
  m_R2gBGToRadiusParam.push_back(q2c);
  m_R2gBGToRadiusParam.push_back(q3c);
  m_R2gBGToRadiusParam.push_back(q4c);
  m_R2gBGToRadiusParam.push_back(q5c);
  m_R2gBGToRadiusParam.push_back(q6c);
  m_R2gBGToRadiusParam.push_back(q7c);
  m_R2gBGToRadiusParam.push_back(q8c);
  
}
double  RichRingRecTransformTool::getBGFromRadius(int irad, double aRadius){
  double aBG =-10000;
  double x=aRadius;

  if( aRadius > 0.0 ) {
    
    if(irad == 0 ) {
      VD pa= m_AgelRadiusToBGParam;
       
     if( aRadius> m_AgelMaxRadiusFit) {
       x=  m_AgelMaxRadiusFit;
     }else if ( aRadius <  m_AgelMinRadiusFit  ) {
       x =  m_AgelMinRadiusFit; 
     }
     aBG = pa[0] +(exp(pa[1]*x-pa[2]))*(cosh(pa[3]*x-pa[4]))*(cosh(pa[3]*x-pa[4]));
  
  
  }else if (irad ==1 ) {
    VD pb  =  m_R1gRadiusToBGParam;
    if( aRadius> m_R1gMaxRadiusFit ) {
      x = m_R1gMaxRadiusFit;
    }else if ( aRadius < m_R1gMinRadiusFit ) {
      x =  m_R1gMinRadiusFit;
    }
    
    aBG = pb[0]+(exp(pb[1]*x-pb[2]))*(cosh(pb[3]*x-pb[4]))*(cosh(pb[3]*x-pb[4]))/(cosh(pb[5]*x-pb[6]));
    
  }else if (irad ==2 ) {
    VD pc =  m_R2gRadiusToBGParam;
    if( aRadius> m_R2gMaxRadiusFit ) {
      x = m_R2gMaxRadiusFit;
    }else if ( aRadius < m_R2gMinRadiusFit ) {
      x =  m_R2gMinRadiusFit;
    }
    aBG = pc[0]+(exp(pc[1]*x-pc[2]))*(exp(pc[7]*x-pc[8]))*(cosh(pc[3]*x-pc[4]))*(cosh(pc[3]*x-pc[4]))/(cosh(pc[5]*x-pc[6]));
  }
    
    
  }
  
  
  return aBG;
 
}
double  RichRingRecTransformTool::getRadiusFromBG(int irad, double aBG){
  double aRadius =-1000;
  double x = aBG;
  if( aBG > 0.0 ) {
    
  if(irad == 0 ) {
    if(aBG >   (m_AgelMinBGFit - m_AgelBGTolerence) ){
      
      VD qa= m_AgelBGToRadiusParam;
       
      if(aBG >  m_AgelMaxBGFit ) {
        x =  m_AgelMaxBGFit ;
      }else if ( aBG < m_AgelMinBGFit ) {
        x =   m_AgelMinBGFit;  
      }
    
       aRadius = qa[0]+(exp(qa[1]*x-qa[2]))*(cosh(qa[3]*x-qa[4]))*(cosh(qa[3]*x-qa[4]))/(cosh(qa[5]*x-qa[6]));

     }else {

       aRadius =0.0;
       
     }
    
    
    
    
  }else if (irad ==1 ) {

    if( aBG > ( m_R1gMinBGFit - m_R1gBGTolerence)){
      
       VD qb =  m_R1gBGToRadiusParam;
       if(aBG >  m_R1gMaxBGFit ) {
          x =  m_R1gMaxBGFit ;
       }else if ( aBG < m_R1gMinBGFit ) {
          x =   m_R1gMinBGFit;  
       }
    
       aRadius =  qb[0]+(exp( qb[1]*x-qb[2]))*(cosh(qb[3]*x-qb[4]))*(cosh(qb[3]*x-qb[4]))/(cosh(qb[5]*x-qb[6]));

     }else {
      
      aRadius =0.0;
      
     }
    
    
    
  }else if (irad ==2 ) {

   if( aBG > ( m_R2gMinBGFit - m_R2gBGTolerence)){
    
    VD qc =  m_R2gBGToRadiusParam;
    if(aBG >  m_R2gMaxBGFit ) {
      x =  m_R2gMaxBGFit ;
    }else if ( aBG < m_R2gMinBGFit ) {
      x =   m_R2gMinBGFit;  
    }
 
    aRadius = qc[0]+
              (exp(qc[1]*x-qc[2]))*(exp(qc[7]*x-qc[8]))*(cosh(qc[3]*x-qc[4]))*(cosh(qc[3]*x-qc[4]))/(cosh(qc[5]*x-qc[6]));
    
   }else {
     
      aRadius  =0.0;
     
   }
   
   
   
  }
  
  
  
  
  }
  
  
  return aRadius;
 
}

double RichRingRecTransformTool::TrackExpectedRadiusValue(int itk, int irad, int PartType){
  double aPartMomentum =  tgD()->TrackRecMomValue(itk,irad);
  double aMass=RConst()->RichParticleTrueMassValue( PartType);
  double aBG =  aPartMomentum/ aMass;
  double aExpRadius=  getRadiusFromBG(irad,aBG);  
  return aExpRadius;
  
}
double RichRingRecTransformTool::TrackExpectedRadiusFromTruePidCode(int itk, int irad, int aTruePidCode){
  int aPartType =   RConst()-> RichParticleTypeFromTruePidCodeValue(aTruePidCode);
  double aExpRadius = TrackExpectedRadiusValue(itk,irad,aPartType);
  return aExpRadius;
}
                                                                    
VD  RichRingRecTransformTool::TrackExpectedRadii(int itk, int irad){
  VD aVect; aVect.clear();
  for(int i=0; i< (RConst()->RichNumParticleSpecies()); ++i ) {  
    double aExpRadius= TrackExpectedRadiusValue(itk, irad, i);
    aVect.push_back(aExpRadius);
  }
  return aVect;
}

double RichRingRecTransformTool::getTrackMassFromRadius(int itk, int irad, double aRadius ){
  double aMass = -1000.0;
  double aPartMomentum =  tgD()->TrackRecMomValue(itk,irad);
  double aBG = getBGFromRadius( irad, aRadius);
  if(aBG > 0.0) {
    aMass =  aPartMomentum /aBG;
  }
  //test print 
  // info()<< "Transformtool irad Rec Momentem Recmass recBG recRadius "<< irad <<"  "<<aPartMomentum
  //      << aMass <<"   "<< aBG<<"  "<<aRadius<<endmsg;
  //
  // end of test print
  return aMass;
}

bool RichRingRecTransformTool::CheckBGThreshold( double aMass, int itk, int irad){
  bool aThFeas=true;
  double aPartMomentum =  tgD()->TrackRecMomValue(itk,irad);
  int i=0;
  while ( i < (RConst()->RichNumParticleSpecies()) && aThFeas ) {
   double aMassExp=RConst()->RichParticleTrueMassValue(i);
   double aBGExp = aPartMomentum/aMassExp;
   if(aBGExp < m_MinBGFit[irad]) {
     if(aMass > aMassExp ) {
       aThFeas = false;   
     }
   }
   i++;
  }
  
  return aThFeas;
  
}
double  RichRingRecTransformTool::MaxRadiusFit( int irad) 
{
  double aradius=999.0;
  
  if(irad ==0 ) {
    aradius=  m_AgelMaxRadiusFit ;
  }else if ( irad ==1 ) {
    aradius=  m_R1gMaxRadiusFit ;
  }else if ( irad ==2 ) {
    aradius=  m_R2gMaxRadiusFit ;
  }
  
  return aradius;
}


//=============================================================================
// Destructor
//=============================================================================
RichRingRecTransformTool::~RichRingRecTransformTool() {} 

//=============================================================================
