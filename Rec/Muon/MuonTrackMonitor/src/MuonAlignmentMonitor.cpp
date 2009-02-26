// $Id: MuonAlignmentMonitor.cpp,v 1.1.1.1 2009-02-26 09:29:59 ggiacomo Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "MuonAlignmentMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonAlignmentMonitor
//
// 2009-02-25 
//                                                                              
//                                                        .--. ::/.             
//    a.k.a. "ciupeta"                                 -//:::::////.            
//                                                    :/:::::::/++o/            
//    a Ferrara speciality                        ..-//::::::/++oo:             
//                                              ://::::::///+/:-.               
//                                        -://::::---::://+o                    
//                                   .-::/::::::://:///++++:                    
//       ...                .---::///:::/+++/:::::/+/o+/:-                      
//   .-///////:--.-:///::--///:::/+/++ooo++++////::/:                           
//  :o++//::::::++/::::::/+////+o++++/://////+++/////                           
//   /ooo++/:/:++/////::/+++/o+++/++//::::////++++++o-                          
//     .---:/oooo++++++/+oo++o++++++//://oo+/:-::::/+/                          
//           :osssssooooosooossoo+//////::::::::::::///::-.-::--.               
//              ....   .://:-.+o+////::///////:::::::/::::/::////:              
//                            .o+//://////++++////////:-///////:--:-::::--.     
//                            -+//::///+++oooooooo++++//++++++/--:::::::////:   
//                          .//+/////++oosssssysssssosooooooo++/++/////+o+/::/  
//                      .-://::/++o++oossso/:::/+oso: .-/++:../sssoooooooo+++o  
//                     /+//////::+osssss/.                      :+sso:.  .---.  
//                    :/::://++++oo///:                                         
//                  .:/::--:/+ooo/.                                             
//                :+////////++..                                                
//              /+++////+++oo.                                                  
//            .//+ooooooosso.                                                   
//            ++/::/+o/::-.                                                     
//           .sooso+/                                                           
//            /oo+:.                                                            
//                                                                              
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuonAlignmentMonitor );



MuonAlignmentMonitor::MuonAlignmentMonitor( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{

}

MuonAlignmentMonitor::~MuonAlignmentMonitor() {} 



StatusCode MuonAlignmentMonitor::initialize() {

  const StatusCode sc = GaudiHistoAlg::initialize();
  if ( sc.isFailure() ) { return sc; }

  return StatusCode::SUCCESS;
}


StatusCode MuonAlignmentMonitor::execute() {

  setFilterPassed(true);  
  return StatusCode::SUCCESS;
}



StatusCode MuonAlignmentMonitor::finalize() {


  return   GaudiHistoAlg::finalize();
}


