// $Header:

#ifndef L0MUON_MUONPADLAYOUT_H
#define L0MUON_MUONPADLAYOUT_H 1   

// Include files
#include <vector>
#include "Parameter.h" 
#include "L0mFoI.h" 
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
  MuonPadLayout(IDataProviderSvc* detSvc);
  /// Constructor to build from explicit  arguments
  MuonPadLayout(DoubleArrayProperty zstation,
        	DoubleArrayProperty bcellX,
		DoubleArrayProperty bcellY, 
		IntegerArrayProperty* regions);
  
  // Destructor
  ~MuonPadLayout();
  
  // Utility functions
  /// Create pad given its physical coordinates
  L0mPad* createPad(int st, double x, double y);
  /// Create pad given from MuonDigit
  L0mPad* createPad(MuonDigit* md);
  /// Find region determined by pad indices in possibly other region 
  int region(int nx, int ny, int nr);
  /// Find pad magnification factor for the given region
  int rfactor(int nr);
  /// Dump contents to log stream
  void print(MsgStream log);
  
private:

  std::vector< Parameter* > m_layout; 
  
};   

std::string operator+(const std::string& st, int ii);

#endif    // L0MUON_MUONPADLAYOUT_H
