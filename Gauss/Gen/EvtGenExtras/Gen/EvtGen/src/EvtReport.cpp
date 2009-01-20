//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtReport.cc
//
// Description: definitions of global functions.
//
// Modification history:
//
//    Simon Patton   June 3, 1996           Module created
//
//------------------------------------------------------------------------
//

// system include files
#include "EvtGenBase/EvtReport.hh"

//
// constants, enums and typedefs
//


std::ostream& report( Severity severity ,
                 const char* facility )
{
   int printNoFacility=1;

   if ( ( facility == 0 ) &&
        ( printNoFacility ==1) ) {
      std::cout << "There is no `facility' implemented in `report'"
                        << std::endl ;
      printNoFacility = 0 ;
   }
   if ( severity < WARNING ) {
     if (facility[0]!=0){
       std::cerr<<facility<<":";
     }
     return ( std::cerr ) ;
   }
   if (facility[0]!=0){
     std::cout<<facility<<":";
   }    
   return std::cout;
}


