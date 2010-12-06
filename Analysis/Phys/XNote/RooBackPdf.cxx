 /***************************************************************************** 
  * Project: RooFit                                                           * 
  *                                                                           * 
  * Copyright (c) 2000-2005, Regents of the University of California          * 
  *                          and Stanford University. All rights reserved.    * 
  *                                                                           * 
  * Redistribution and use in source and binary forms,                        * 
  * with or without modification, are permitted according to the terms        * 
  * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             * 
  *****************************************************************************/ 

 // -- CLASS DESCRIPTION [PDF] -- 
 // Your description goes here... 

 #include <iostream> 

 #include "RooBackPdf.h" 
 #include "RooAbsReal.h" 

 ClassImp(RooBackPdf) 

 RooBackPdf::RooBackPdf(const char *name, const char *title, 
                        RooAbsReal& _x,
                        RooAbsReal& _mr,
                        RooAbsReal& _p1,
                        RooAbsReal& _p2,
                        RooAbsReal& _p3) :
   RooAbsPdf(name,title), 
   x("x","x",this,_x),
    mr(" mr"," mr",this,_mr),
    p1(" p1"," p1",this,_p1),
    p2(" p2"," p2",this,_p2),
    p3(" p3"," p3",this,_p3)
 { 
 } 


 RooBackPdf::RooBackPdf(const RooBackPdf& other, const char* name) :  
   RooAbsPdf(other,name), 
   x("x",this,other.x),
    mr(" mr",this,other.mr),
    p1(" p1",this,other.p1),
    p2(" p2",this,other.p2),
    p3(" p3",this,other.p3)
 { 
 } 



 Double_t RooBackPdf::evaluate() const 
 { 
   // ENTER EXPRESSION IN TERMS OF VARIABLE ARGUMENTS HERE 
   // ENTER EXPRESSION IN TERMS OF VARIABLE ARGUMENTS HERE 
   double value = x * ( p2 + (p3*x));
   return pow(x - mr, p1 )*exp(-value) ; 
 } 



