#ifndef CONE_INFO_H
#define CONE_INFO_H 1

#include "Event/RelatedInfoMap.h"

class RelatedInfoNamed : public LHCb::RelatedInfoMap {

  public: 

    /// Index values
    enum index { 
      UNKNOWN      = -1,       // Unknown/illegal value of the index

      // Cone variables 
      CONEANGLE    = 0,        // Cone angle
      CONEMULT     = CONEANGLE+1,  // Cone multiplicity
      CONEPX       = CONEANGLE+2,  // Cone Px
      CONEPY       = CONEANGLE+3,  // Cone Py
      CONEPZ       = CONEANGLE+4,  // Cone Pz
      CONEP        = CONEANGLE+5,  // Cone P
      CONEPT       = CONEANGLE+6,  // Cone Pt
      CONEPXASYM   = CONEANGLE+7,  // Cone Px asymmetry
      CONEPYASYM   = CONEANGLE+8,  // Cone Py asymmetry
      CONEPZASYM   = CONEANGLE+9,  // Cone Pz asymmetry
      CONEPASYM    = CONEANGLE+10, // Cone P asymmetry
      CONEPTASYM   = CONEANGLE+11, // Cone Pt asymmetry
      CONEDELTAETA = CONEANGLE+12, // Cone DeltaEta
      CONEDELTAPHI = CONEANGLE+13, // Cone DeltaPhi

      // Cone variables for EW      
      EWCONEANGLE   = CONEDELTAPHI+1,  // Cone angle
      EWCONEMULT    = EWCONEANGLE+1,   // Cone multiplicity
      EWCONEPX      = EWCONEANGLE+2,   // Cone Px
      EWCONEPY      = EWCONEANGLE+3,   // Cone Py
      EWCONEPZ      = EWCONEANGLE+4,   // Cone Pz
      EWCONEVP      = EWCONEANGLE+5,   // Cone vP
      EWCONEVPT     = EWCONEANGLE+6,   // Cone vPt
      EWCONESP      = EWCONEANGLE+7,   // Cone sP
      EWCONESPT     = EWCONEANGLE+8,   // Cone sPt
      EWCONETP      = EWCONEANGLE+9,   // Cone tP
      EWCONETPT     = EWCONEANGLE+10,  // Cone tPt
      EWCONEMINPTE  = EWCONEANGLE+11,  // Cone min(Pt_e)
      EWCONEMAXPTE  = EWCONEANGLE+12,  // Cone max(Pt_e)
      EWCONEMINPTMU = EWCONEANGLE+13,  // Cone min(Pt_mu)
      EWCONEMAXPTMU = EWCONEANGLE+14,  // Cone max(Pt_mu)
      EWCONENMULT   = EWCONEANGLE+15,  // Neutral cone multiplicity
      EWCONENPX     = EWCONEANGLE+16,  // Neutral Cone Px
      EWCONENPY     = EWCONEANGLE+17,  // Neutral Cone Py
      EWCONENPZ     = EWCONEANGLE+18,  // Neutral Cone Pz
      EWCONENVP     = EWCONEANGLE+19,  // Neutral Cone vP
      EWCONENVPT    = EWCONEANGLE+20,  // Neutral Cone vPt
      EWCONENSP     = EWCONEANGLE+21,  // Neutral Cone sP
      EWCONENSPT    = EWCONEANGLE+22,  // Neutral Cone sPt

      // Vertex isolation variables
      VTXISONUMVTX            = EWCONENSPT+1,   // NumVtxWithinChi2WindowOneTrack
      VTXISODCHI2ONETRACK     = VTXISONUMVTX+1, // SmallestDeltaChi2OneTrack
      VTXISODCHI2MASSONETRACK = VTXISONUMVTX+2, // SmallestDeltaChi2MassOneTrack
      VTXISODCHI2TWOTRACK     = VTXISONUMVTX+3, // SmallestDeltaChi2OneTrack
      VTXISODCHI2MASSTWOTRACK = VTXISONUMVTX+4,  // SmallestDeltaChi2MassOneTrack

      //BDT ISOLATIONS ENUMS
      VTXISOBDTHARDFIRSTVALUE  = VTXISODCHI2MASSTWOTRACK + 1 ,
      VTXISOBDTHARDSECONDVALUE = VTXISOBDTHARDFIRSTVALUE + 1 ,
      VTXISOBDTHARDTHIRDVALUE  = VTXISOBDTHARDFIRSTVALUE + 2 ,
      VTXISOBDTSOFTFIRSTVALUE  = VTXISOBDTHARDFIRSTVALUE + 3 ,
      VTXISOBDTSOFTSECONDVALUE = VTXISOBDTHARDFIRSTVALUE + 4 ,
      VTXISOBDTSOFTTHIRDVALUE  = VTXISOBDTHARDFIRSTVALUE + 5 ,
      TRKISOBDTFIRSTVALUE      = VTXISOBDTHARDFIRSTVALUE + 6 ,
      TRKISOBDTSECONDVALUE     = VTXISOBDTHARDFIRSTVALUE + 7 ,
      TRKISOBDTTHIRDVALUE      = VTXISOBDTHARDFIRSTVALUE + 8 ,
      
      BSMUMUCDFISO           = TRKISOBDTTHIRDVALUE + 1, 
      BSMUMUOTHERBMAG        = BSMUMUCDFISO + 1,
      BSMUMUOTHERBANGLE      = BSMUMUCDFISO + 2,
      BSMUMUOTHERBBOOSTMAG   = BSMUMUCDFISO + 3,
      BSMUMUOTHERBBOOSTANGLE = BSMUMUCDFISO + 4,
      BSMUMUTRACKPLUSISO        = BSMUMUCDFISO + 5,
      BSMUMUTRACKMINUSISO        = BSMUMUCDFISO + 6,
      BSMUMUOTHERBTRACKS     = BSMUMUCDFISO + 7,

      // Jet isolation variables
      JETNOMU1PX     	 =  BSMUMUOTHERBTRACKS + 1,
      JETNOMU1PY     	 =  JETNOMU1PX + 1 ,
      JETNOMU1PZ     	 =  JETNOMU1PX + 2 ,
      JETNOMU1PT     	 =  JETNOMU1PX + 3 ,
      JETNOMU1JETWIDTH =  JETNOMU1PX + 4 ,
      JETNOMU1NNTAG    =  JETNOMU1PX + 5 ,
      JETNOMU1MNF      =  JETNOMU1PX + 6 ,
      JETNOMU2PX     	 =  JETNOMU1PX + 7 ,
      JETNOMU2PY     	 =  JETNOMU1PX + 8 ,
      JETNOMU2PZ     	 =  JETNOMU1PX + 9 ,
      JETNOMU2PT     	 =  JETNOMU1PX + 10 ,
      JETNOMU2JETWIDTH =  JETNOMU1PX + 11 ,
      JETNOMU2NNTAG    =  JETNOMU1PX + 12 ,
      JETNOMU2MNF      =  JETNOMU1PX + 13 ,
      JETNOMU3PX     	 =  JETNOMU1PX + 14 ,
      JETNOMU3PY     	 =  JETNOMU1PX + 15 ,
      JETNOMU3PZ     	 =  JETNOMU1PX + 16 ,
      JETNOMU3PT     	 =  JETNOMU1PX + 17 ,
      JETNOMU3JETWIDTH =  JETNOMU1PX + 18 ,
      JETNOMU3NNTAG    =  JETNOMU1PX + 19 ,
      JETNOMU3MNF      =  JETNOMU1PX + 20 ,
      JETMU1PX     	   =  JETNOMU1PX + 21 ,
      JETMU1PY     	   =  JETNOMU1PX + 22 ,
      JETMU1PZ     	   =  JETNOMU1PX + 23 ,
      JETMU1PT     	   =  JETNOMU1PX + 24 ,
      JETMU1JETWIDTH   =  JETNOMU1PX + 25 ,
      JETMU1NNTAG      =  JETNOMU1PX + 26 ,
      JETMU1MNF     	 =  JETNOMU1PX + 27 ,
      JETMU2PX     	   =  JETNOMU1PX + 28 ,
      JETMU2PY     	   =  JETNOMU1PX + 29 ,
      JETMU2PZ     	   =  JETNOMU1PX + 30 ,
      JETMU2PT     	   =  JETNOMU1PX + 31 ,
      JETMU2JETWIDTH   =  JETNOMU1PX + 32 ,
      JETMU2NNTAG      =  JETNOMU1PX + 33 ,
      JETMU2MNF     	 =  JETNOMU1PX + 34 ,
      JETBPX     	     =  JETNOMU1PX + 35 ,
      JETBPY     	     =  JETNOMU1PX + 36 ,
      JETBPZ     	     =  JETNOMU1PX + 37 ,
      JETBPT     	     =  JETNOMU1PX + 38 ,
      JETBJETWIDTH     =  JETNOMU1PX + 39 ,
      JETBNNTAG     	 =  JETNOMU1PX + 40 ,
      JETBMNF     	   =  JETNOMU1PX + 41 ,

      // cylinder isolation variables
      CYLBESTD0               =  JETBMNF + 1 ,
      CYLBESTKS     	        =  CYLBESTD0 + 1 ,
      CYLTRACKSNCYL           =  CYLBESTD0 + 2 ,
      CYLMINPT     	          =  CYLBESTD0 + 3 ,
      CYLMAXPT     	          =  CYLBESTD0 + 4 ,
      CYLAVPT     	          =  CYLBESTD0 + 5 ,
      CYLMAXPTTRIPCHI2        =  CYLBESTD0 + 6 ,
      CYLMAXPTTRDOCACHI2      =  CYLBESTD0 + 7 ,
      CYLMAXPTTRBMASS     	  =  CYLBESTD0 + 8 ,
      CYLMINIPCHI2     	      =  CYLBESTD0 + 9 ,
      CYLMAXIPCHI2     	      =  CYLBESTD0 + 10 ,
      CYLAVIPCHI2     	      =  CYLBESTD0 + 11 ,
      CYLMAXIPCHI2TRPT     	  =  CYLBESTD0 + 12 ,
      CYLMAXIPCHI2TRDOCACHI2  =  CYLBESTD0 + 13 ,
      CYLMAXIPCHI2TRBMASS     =  CYLBESTD0 + 14 ,
      CYLMINDOCACHI2     	    =  CYLBESTD0 + 15 ,
      CYLMAXDOCACHI2     	    =  CYLBESTD0 + 16 ,
      CYLAVDOCACHI2     	    =  CYLBESTD0 + 17 ,
      CYLMINDOCATRPT     	    =  CYLBESTD0 + 18 ,
      CYLMINDOCATRIPCHI2     	=  CYLBESTD0 + 19 ,
      CYLMINDOCATRBMASS     	=  CYLBESTD0 + 20 ,
      CYLBESTBMASS     	      =  CYLBESTD0 + 21 ,
      CYLBESTBMASSTRPT     	  =  CYLBESTD0 + 22 ,
      CYLBESTBMASSTRIPCHI2    =  CYLBESTD0 + 23 ,
      CYLBESTBMASSTRDOCACHI2  =  CYLBESTD0 + 24 ,

      // Particle Flow related variables
      PFCHARGEDMASS    =  CYLBESTBMASSTRDOCACHI2 + 1 ,
      PFALLMASS     	 =  PFCHARGEDMASS + 1 ,
      PFCHARGEDTOTPT   =  PFCHARGEDMASS + 2 ,
      PFALLTOTPT     	 =  PFCHARGEDMASS + 3 ,
      PFCHARGEDMISSPT  =  PFCHARGEDMASS + 4 ,
      PFALLMISSPT      =  PFCHARGEDMASS + 5 , 

      BSTAUTAUTRKISOBDTFIRSTVALUE = PFALLMISSPT + 1, 
      BSTAUTAUTRKISOBDTSECONDVALUE = BSTAUTAUTRKISOBDTFIRSTVALUE + 1, 
      BSTAUTAUTRKISOBDTTHIRDVALUE = BSTAUTAUTRKISOBDTFIRSTVALUE + 2, 
      BSTAUTAUTAUISOBDTFIRSTVALUE = BSTAUTAUTRKISOBDTFIRSTVALUE + 3, 
      BSTAUTAUTAUISOBDTSECONDVALUE = BSTAUTAUTRKISOBDTFIRSTVALUE + 4, 
      BSTAUTAUTAUISOBDTTHIRDVALUE = BSTAUTAUTRKISOBDTFIRSTVALUE + 5, 
      BSTAUTAUMUONISOBDTFIRSTVALUE = BSTAUTAUTRKISOBDTFIRSTVALUE + 6, 
      BSTAUTAUMUONISOBDTSECONDVALUE = BSTAUTAUTRKISOBDTFIRSTVALUE + 7, 
      BSTAUTAUMUONISOBDTTHIRDVALUE = BSTAUTAUTRKISOBDTFIRSTVALUE + 8, 
      BSTAUTAUTRKISOFIRSTVALUE = BSTAUTAUTRKISOBDTFIRSTVALUE + 9, 
      BSTAUTAUTAUISOFIRSTVALUE = BSTAUTAUTRKISOBDTFIRSTVALUE + 10,
      BSTAUTAUTAUISOSECONDVALUE = BSTAUTAUTRKISOBDTFIRSTVALUE + 11, 
      BSTAUTAUMUONISOFIRSTVALUE = BSTAUTAUTRKISOBDTFIRSTVALUE + 12, 
      BSTAUTAUMUONISOSECONDVALUE = BSTAUTAUTRKISOBDTFIRSTVALUE + 13, 
      BSTAUTAUCDFISO = BSTAUTAUTRKISOBDTFIRSTVALUE + 14, 
      
      BSMUMUTRACKMINUSISOTWO = BSTAUTAUCDFISO + 1, 
      BSMUMUTRACKPLUSISOTWO  = BSMUMUTRACKMINUSISOTWO + 1, 
      ISOTWOBODYMASSISOPLUS  = BSMUMUTRACKMINUSISOTWO + 2, 
      ISOTWOBODYMASSISOMINUS = BSMUMUTRACKMINUSISOTWO + 3, 
      ISOTWOBODYCHI2ISOPLUS  = BSMUMUTRACKMINUSISOTWO + 4, 
      ISOTWOBODYCHI2ISOMINUS = BSMUMUTRACKMINUSISOTWO + 5, 
      ISOTWOBODYISO5PLUS     = BSMUMUTRACKMINUSISOTWO + 6, 
      ISOTWOBODYISO5MINUS    = BSMUMUTRACKMINUSISOTWO + 7, 
      ISOTWOBODYQMINUS       = BSMUMUTRACKMINUSISOTWO + 8, 
      ISOTWOBODYQPLUS        = BSMUMUTRACKMINUSISOTWO + 9, 

      //ZVtop isolation 
      ZVISO = ISOTWOBODYQPLUS + 1

    };

    static std::string nameByIndex(int key) {
      switch (key) {
      case RelatedInfoNamed::CONEANGLE : return "CONEANGLE"; 
      case RelatedInfoNamed::CONEMULT  : return "CONEMULT"; 
      case RelatedInfoNamed::CONEPX    : return "CONEPX"; 
      case RelatedInfoNamed::CONEPY    : return "CONEPY"; 
      case RelatedInfoNamed::CONEPZ    : return "CONEPZ"; 
      case RelatedInfoNamed::CONEP     : return "CONEP"; 
      case RelatedInfoNamed::CONEPT    : return "CONEPT"; 
      case RelatedInfoNamed::CONEPXASYM    : return "CONEPXASYM"; 
      case RelatedInfoNamed::CONEPYASYM    : return "CONEPYASYM"; 
      case RelatedInfoNamed::CONEPZASYM    : return "CONEPZASYM"; 
      case RelatedInfoNamed::CONEPASYM     : return "CONEPASYM"; 
      case RelatedInfoNamed::CONEPTASYM    : return "CONEPTASYM"; 
      case RelatedInfoNamed::CONEDELTAETA  : return "CONEDELTAETA"; 
      case RelatedInfoNamed::CONEDELTAPHI  : return "CONEDELTAPHI"; 
	
      case RelatedInfoNamed::EWCONEANGLE : return "EWCONEANGLE"; 
      case RelatedInfoNamed::EWCONEMULT  : return "EWCONEMULT"; 
      case RelatedInfoNamed::EWCONEPX    : return "EWCONEPX"; 
      case RelatedInfoNamed::EWCONEPY    : return "EWCONEPY"; 
      case RelatedInfoNamed::EWCONEPZ    : return "EWCONEPZ"; 
      case RelatedInfoNamed::EWCONEVP    : return "EWCONEVP"; 
      case RelatedInfoNamed::EWCONEVPT   : return "EWCONEVPT"; 
      case RelatedInfoNamed::EWCONESP    : return "EWCONESP"; 
      case RelatedInfoNamed::EWCONESPT   : return "EWCONESPT"; 
      case RelatedInfoNamed::EWCONETP    : return "EWCONETP"; 
      case RelatedInfoNamed::EWCONETPT   : return "EWCONETPT"; 
      case RelatedInfoNamed::EWCONEMINPTE   : return "EWCONEMINPTE"; 
      case RelatedInfoNamed::EWCONEMAXPTE   : return "EWCONEMAXPTE"; 
      case RelatedInfoNamed::EWCONEMINPTMU  : return "EWCONEMINPTMU"; 
      case RelatedInfoNamed::EWCONEMAXPTMU  : return "EWCONEMAXPTMU"; 
      case RelatedInfoNamed::EWCONENMULT  : return "EWCONENMULT"; 
      case RelatedInfoNamed::EWCONENPX    : return "EWCONENPX"; 
      case RelatedInfoNamed::EWCONENPY    : return "EWCONENPY"; 
      case RelatedInfoNamed::EWCONENPZ    : return "EWCONENPZ"; 
      case RelatedInfoNamed::EWCONENVP    : return "EWCONENVP"; 
      case RelatedInfoNamed::EWCONENVPT   : return "EWCONENVPT"; 
      case RelatedInfoNamed::EWCONENSP    : return "EWCONENSP"; 
      case RelatedInfoNamed::EWCONENSPT   : return "EWCONENSPT"; 
	
      case RelatedInfoNamed::VTXISONUMVTX            : return "VTXISONUMVTX"; 
      case RelatedInfoNamed::VTXISODCHI2ONETRACK     : return "VTXISODCHI2ONETRACK"; 
      case RelatedInfoNamed::VTXISODCHI2MASSONETRACK : return "VTXISODCHI2MASSONETRACK"; 
      case RelatedInfoNamed::VTXISODCHI2TWOTRACK     : return "VTXISODCHI2TWOTRACK";
      case RelatedInfoNamed::VTXISODCHI2MASSTWOTRACK : return "VTXISODCHI2MASSTWOTRACK";
	
      case RelatedInfoNamed::VTXISOBDTHARDFIRSTVALUE : return "VTXISOBDTHARDFIRSTVALUE" ;
      case RelatedInfoNamed::VTXISOBDTHARDSECONDVALUE : return "VTXISOBDTHARDSECONDVALUE" ;
      case RelatedInfoNamed::VTXISOBDTHARDTHIRDVALUE : return "VTXISOBDTHARDTHIRDVALUE" ;
      case RelatedInfoNamed::VTXISOBDTSOFTFIRSTVALUE : return "VTXISOBDTSOFTFIRSTVALUE" ;
      case RelatedInfoNamed::VTXISOBDTSOFTSECONDVALUE : return "VTXISOBDTSOFTSECONDVALUE" ;
      case RelatedInfoNamed::VTXISOBDTSOFTTHIRDVALUE : return "VTXISOBDTSOFTTHIRDVALUE" ;
      case RelatedInfoNamed::TRKISOBDTFIRSTVALUE : return "TRKISOBDTFIRSTVALUE" ;
      case RelatedInfoNamed::TRKISOBDTSECONDVALUE : return "TRKISOBDTSECONDVALUE" ;
      case RelatedInfoNamed::TRKISOBDTTHIRDVALUE : return "TRKISOBDTTHIRDVALUE" ;
	
      case RelatedInfoNamed::BSMUMUCDFISO : return "BSMUMUCDFISO";
      case RelatedInfoNamed::BSMUMUOTHERBMAG : return "BSMUMUOTHERBMAG";
      case RelatedInfoNamed::BSMUMUOTHERBANGLE : return "BSMUMUOTHERBANGLE";
      case RelatedInfoNamed::BSMUMUOTHERBBOOSTMAG : return "BSMUMUOTHERBBOOSTMAG";
      case RelatedInfoNamed::BSMUMUOTHERBBOOSTANGLE : return "BSMUMUOTHERBBOOSTANGLE";
      case RelatedInfoNamed::BSMUMUTRACKPLUSISO : return "BSMUMUTRACKPLUSISO"; 
      case RelatedInfoNamed::BSMUMUTRACKMINUSISO : return "BSMUMUTRACKMINUSISO";
      case RelatedInfoNamed::BSMUMUOTHERBTRACKS : return "BSMUMUOTHERBTRACKS";

      case RelatedInfoNamed::JETNOMU1PX : return "JETNOMU1PX" ;
      case RelatedInfoNamed::JETNOMU1PY : return "JETNOMU1PY" ;
      case RelatedInfoNamed::JETNOMU1PZ : return "JETNOMU1PZ" ;
      case RelatedInfoNamed::JETNOMU1PT : return "JETNOMU1PT" ;
      case RelatedInfoNamed::JETNOMU1JETWIDTH : return "JETNOMU1JETWIDTH" ;
      case RelatedInfoNamed::JETNOMU1NNTAG : return "JETNOMU1NNTAG" ;
      case RelatedInfoNamed::JETNOMU1MNF : return "JETNOMU1MNF" ;
      case RelatedInfoNamed::JETNOMU2PX : return "JETNOMU2PX" ;
      case RelatedInfoNamed::JETNOMU2PY : return "JETNOMU2PY" ;
      case RelatedInfoNamed::JETNOMU2PZ : return "JETNOMU2PZ" ;
      case RelatedInfoNamed::JETNOMU2PT : return "JETNOMU2PT" ;
      case RelatedInfoNamed::JETNOMU2JETWIDTH : return "JETNOMU2JETWIDTH" ;
      case RelatedInfoNamed::JETNOMU2NNTAG : return "JETNOMU2NNTAG" ;
      case RelatedInfoNamed::JETNOMU2MNF : return "JETNOMU2MNF" ;
      case RelatedInfoNamed::JETNOMU3PX : return "JETNOMU3PX" ;
      case RelatedInfoNamed::JETNOMU3PY : return "JETNOMU3PY" ;
      case RelatedInfoNamed::JETNOMU3PZ : return "JETNOMU3PZ" ;
      case RelatedInfoNamed::JETNOMU3PT : return "JETNOMU3PT" ;
      case RelatedInfoNamed::JETNOMU3JETWIDTH : return "JETNOMU3JETWIDTH" ;
      case RelatedInfoNamed::JETNOMU3NNTAG : return "JETNOMU3NNTAG" ;
      case RelatedInfoNamed::JETNOMU3MNF : return "JETNOMU3MNF" ;
      case RelatedInfoNamed::JETMU1PX : return "JETMU1PX" ;
      case RelatedInfoNamed::JETMU1PY : return "JETMU1PY" ;
      case RelatedInfoNamed::JETMU1PZ : return "JETMU1PZ" ;
      case RelatedInfoNamed::JETMU1PT : return "JETMU1PT" ;
      case RelatedInfoNamed::JETMU1JETWIDTH : return "JETMU1JETWIDTH" ;
      case RelatedInfoNamed::JETMU1NNTAG : return "JETMU1NNTAG" ;
      case RelatedInfoNamed::JETMU1MNF : return "JETMU1MNF" ;
      case RelatedInfoNamed::JETMU2PX : return "JETMU2PX" ;
      case RelatedInfoNamed::JETMU2PY : return "JETMU2PY" ;
      case RelatedInfoNamed::JETMU2PZ : return "JETMU2PZ" ;
      case RelatedInfoNamed::JETMU2PT : return "JETMU2PT" ;
      case RelatedInfoNamed::JETMU2JETWIDTH : return "JETMU2JETWIDTH" ;
      case RelatedInfoNamed::JETMU2NNTAG : return "JETMU2NNTAG" ;
      case RelatedInfoNamed::JETMU2MNF : return "JETMU2MNF" ;
      case RelatedInfoNamed::JETBPX : return "JETBPX" ;
      case RelatedInfoNamed::JETBPY : return "JETBPY" ;
      case RelatedInfoNamed::JETBPZ : return "JETBPZ" ;
      case RelatedInfoNamed::JETBPT : return "JETBPT" ;
      case RelatedInfoNamed::JETBJETWIDTH : return "JETBJETWIDTH" ;
      case RelatedInfoNamed::JETBNNTAG : return "JETBNNTAG" ;
      case RelatedInfoNamed::JETBMNF : return "JETBMNF" ;

      case RelatedInfoNamed::CYLBESTD0 : return "CYLBESTD0" ;
      case RelatedInfoNamed::CYLBESTKS : return "CYLBESTKS" ;
      case RelatedInfoNamed::CYLTRACKSNCYL : return "CYLTRACKSNCYL" ;
      case RelatedInfoNamed::CYLMINPT : return "CYLMINPT" ;
      case RelatedInfoNamed::CYLMAXPT : return "CYLMAXPT" ;
      case RelatedInfoNamed::CYLAVPT : return "CYLAVPT" ;
      case RelatedInfoNamed::CYLMAXPTTRIPCHI2 : return "CYLMAXPTTRIPCHI2" ;
      case RelatedInfoNamed::CYLMAXPTTRDOCACHI2 : return "CYLMAXPTTRDOCACHI2" ;
      case RelatedInfoNamed::CYLMAXPTTRBMASS : return "CYLMAXPTTRBMASS" ;
      case RelatedInfoNamed::CYLMINIPCHI2 : return "CYLMINIPCHI2" ;
      case RelatedInfoNamed::CYLMAXIPCHI2 : return "CYLMAXIPCHI2" ;
      case RelatedInfoNamed::CYLAVIPCHI2 : return "CYLAVIPCHI2" ;
      case RelatedInfoNamed::CYLMAXIPCHI2TRPT : return "CYLMAXIPCHI2TRPT" ;
      case RelatedInfoNamed::CYLMAXIPCHI2TRDOCACHI2 : return "CYLMAXIPCHI2TRDOCACHI2" ;
      case RelatedInfoNamed::CYLMAXIPCHI2TRBMASS : return "CYLMAXIPCHI2TRBMASS" ;
      case RelatedInfoNamed::CYLMINDOCACHI2 : return "CYLMINDOCACHI2" ;
      case RelatedInfoNamed::CYLMAXDOCACHI2 : return "CYLMAXDOCACHI2" ;
      case RelatedInfoNamed::CYLAVDOCACHI2 : return "CYLAVDOCACHI2" ;
      case RelatedInfoNamed::CYLMINDOCATRPT : return "CYLMINDOCATRPT" ;
      case RelatedInfoNamed::CYLMINDOCATRIPCHI2 : return "CYLMINDOCATRIPCHI2" ;
      case RelatedInfoNamed::CYLMINDOCATRBMASS : return "CYLMINDOCATRBMASS" ;
      case RelatedInfoNamed::CYLBESTBMASS : return "CYLBESTBMASS" ;
      case RelatedInfoNamed::CYLBESTBMASSTRPT : return "CYLBESTBMASSTRPT" ;
      case RelatedInfoNamed::CYLBESTBMASSTRIPCHI2 : return "CYLBESTBMASSTRIPCHI2" ;
      case RelatedInfoNamed::CYLBESTBMASSTRDOCACHI2 : return "CYLBESTBMASSTRDOCACHI2" ;
      case RelatedInfoNamed::PFCHARGEDMASS : return "PFCHARGEDMASS" ;
      case RelatedInfoNamed::PFALLMASS : return "PFALLMASS" ;
      case RelatedInfoNamed::PFCHARGEDTOTPT : return "PFCHARGEDTOTPT" ;
      case RelatedInfoNamed::PFALLTOTPT : return "PFALLTOTPT" ;
      case RelatedInfoNamed::PFCHARGEDMISSPT : return "PFCHARGEDMISSPT" ;
      case RelatedInfoNamed::PFALLMISSPT : return "PFALLMISSPT" ;

      case RelatedInfoNamed::BSTAUTAUTRKISOBDTFIRSTVALUE : return "BSTAUTAUTRKISOBDTFIRSTVALUE" ; 
      case RelatedInfoNamed::BSTAUTAUTRKISOBDTSECONDVALUE : return "BSTAUTAUTRKISOBDTSECONDVALUE" ; 
      case RelatedInfoNamed::BSTAUTAUTRKISOBDTTHIRDVALUE : return "BSTAUTAUTRKISOBDTTHIRDVALUE" ; 
      case RelatedInfoNamed::BSTAUTAUTAUISOBDTFIRSTVALUE : return "BSTAUTAUTAUISOBDTFIRSTVALUE" ; 
      case RelatedInfoNamed::BSTAUTAUTAUISOBDTSECONDVALUE : return "BSTAUTAUTAUISOBDTSECONDVALUE" ; 
      case RelatedInfoNamed::BSTAUTAUTAUISOBDTTHIRDVALUE : return "BSTAUTAUTAUISOBDTTHIRDVALUE" ; 
      case RelatedInfoNamed::BSTAUTAUMUONISOBDTFIRSTVALUE : return "BSTAUTAUMUONISOBDTFIRSTVALUE" ; 
      case RelatedInfoNamed::BSTAUTAUMUONISOBDTSECONDVALUE : return "BSTAUTAUMUONISOBDTSECONDVALUE" ; 
      case RelatedInfoNamed::BSTAUTAUMUONISOBDTTHIRDVALUE : return "BSTAUTAUMUONISOBDTTHIRDVALUE" ; 
      case RelatedInfoNamed::BSTAUTAUTRKISOFIRSTVALUE : return "BSTAUTAUTRKISOFIRSTVALUE" ; 
      case RelatedInfoNamed::BSTAUTAUTAUISOFIRSTVALUE : return "BSTAUTAUTAUISOFIRSTVALUE" ; 
      case RelatedInfoNamed::BSTAUTAUTAUISOSECONDVALUE : return "BSTAUTAUTAUISOSECONDVALUE" ; 
      case RelatedInfoNamed::BSTAUTAUMUONISOFIRSTVALUE : return "BSTAUTAUMUONISOFIRSTVALUE" ; 
      case RelatedInfoNamed::BSTAUTAUMUONISOSECONDVALUE : return "BSTAUTAUMUONISOSECONDVALUE" ; 
      case RelatedInfoNamed::BSTAUTAUCDFISO : return "BSTAUTAUCDFISO" ; 

      case RelatedInfoNamed::BSMUMUTRACKMINUSISOTWO : return "BSMUMUTRACKMINUSISOTWO";
      case RelatedInfoNamed::BSMUMUTRACKPLUSISOTWO : return "BSMUMUTRACKPLUSISOTWO";
      case RelatedInfoNamed::ISOTWOBODYMASSISOPLUS : return "ISOTWOBODYMASSISOPLUS"; 
      case RelatedInfoNamed::ISOTWOBODYMASSISOMINUS : return "ISOTWOBODYMASSISOMINUS"; 
      case RelatedInfoNamed::ISOTWOBODYCHI2ISOPLUS : return "ISOTWOBODYCHI2ISOPLUS"; 
      case RelatedInfoNamed::ISOTWOBODYCHI2ISOMINUS : return "ISOTWOBODYCHI2ISOMINUS";
      case RelatedInfoNamed::ISOTWOBODYISO5PLUS : return "ISOTWOBODYISO5PLUS";
      case RelatedInfoNamed::ISOTWOBODYISO5MINUS : return "ISOTWOBODYISO5MINUS";
      case RelatedInfoNamed::ISOTWOBODYQMINUS : return "ISOTWOBODYQMINUS";
      case RelatedInfoNamed::ISOTWOBODYQPLUS : return "ISOTWOBODYQPLUS";

      case RelatedInfoNamed::ZVISO : return "ZVISO"; 

        default : return "UNKNOWN"; 
      }
    }
    
    static short int indexByName(std::string name) {
      short i;
      for (i=0; i<=RelatedInfoNamed::ZVISO; i++) {
        if ( name.compare(nameByIndex(i) ) == 0 ) return i; 
      }
      return RelatedInfoNamed::UNKNOWN; 
    }

}; 

#endif
