// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/L0Muon/MuonLogicalPadLayout.h,v 1.1 2001-07-09 19:02:55 atsareg Exp $

#ifndef L0MUON_MUONLOGICALPADLAYOUT_H
#define L0MUON_MUONLOGICALPADLAYOUT_H 1   

// Include files
#include <vector>
#include "GaudiKernel/SmartDataPtr.h"  
#include "GaudiKernel/Property.h"  

#include "L0Muon/Parameter.h"

// Forward declarations

class L0mPad;
class MuonDigit;
class MsgStream;

/** @class MuonLogicalPadLayout MuonLogicalPadLayout.h MuonLogicalPadLayout.h 

   Contains all the parameters of the Muon system logical layout
   
   @author Andrei Tsaregorodtsev
   @date 6 July 2001 
*/

class MuonLogicalPadLayout {

public:
  /// Constructor to build from XML description 
  MuonLogicalPadLayout(IDataProviderSvc* detSvc, MsgStream& log);
  /// Constructor to build from explicit  arguments
  MuonLogicalPadLayout(std::vector<double> zpos,
                       std::vector<int>* regions);
  
  // Destructor
  ~MuonLogicalPadLayout();
  
  // Utility functions
  /// Create pad given from MuonDigit
  L0mPad* createPad(MuonDigit* md);
  
private:

  int m_lunitX[5][4]; 
  int m_lunitY[5][4]; 
  int m_npadX[5][4]; 
  int m_npadY[5][4]; 
  
  std::vector< Parameter* > m_layout;   
  
  /// Helper stuff to decode Muon Pad ID
  struct ID {
    int station;
    int quarter;
    int region;
    int nx;
    int ny;
  };

  ID getId(unsigned int id);     
  
};   

#endif    // L0MUON_MUONLOGICALPADLAYOUT_H
