// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/L0Muon/MuonPadLayout.h,v 1.1 2001-06-07 16:46:19 atsareg Exp $

#ifndef L0MUON_MUONPADLAYOUT_H
#define L0MUON_MUONPADLAYOUT_H 1   

// Include files
#include <vector>
#include "L0Muon/Parameter.h" 
#include "GaudiKernel/SmartDataPtr.h"  
#include "GaudiKernel/Property.h"  


// Forward declarations

class L0mPad;
class MuonDigit;
class MsgStream;

/** @class MuonPadLayout MuonPadLayout.h MuonPadLayout.h 

   Contains all the parameters of the Muon system physical and
   logical layout
   
   @author Andrei Tsaregorodtsev
   @date 6 April 2001 
*/

class MuonPadLayout {

public:
  /// Constructor to build from XML description 
  MuonPadLayout(IDataProviderSvc* detSvc, MsgStream& log);
  /// Constructor to build from explicit  arguments
  MuonPadLayout(std::vector<double> zstation,
        	std::vector<double> bcellX,
		std::vector<double> bcellY, 
		std::vector<int>* regions);
  
  // Destructor
  ~MuonPadLayout();
  
  /// Layout accessor function
  Parameter* layout(int st) const { return m_layout[st-1]; };
  // Utility functions
  /// Create pad given its physical coordinates
  L0mPad* createPad(int st, double x, double y);
  /// Create pad given from MuonDigit
  L0mPad* createPad(MuonDigit* md);
  /// Find region determined by pad indices in possibly other region 
  // int region(int nx, int ny, int nr);
  /// Find pad magnification factor for the given region
  int rfactor(int nr);
  
private:

  std::vector< Parameter* > m_layout; 
  
};   

std::string operator+(const std::string& st, int ii);

// Dumping the contents to the MsgStream
MsgStream& operator<<(MsgStream& log, const MuonPadLayout& mpl);

#endif    // L0MUON_MUONPADLAYOUT_H
