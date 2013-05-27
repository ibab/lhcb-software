#ifndef PATKERNEL_IPATTTCOUNTER_H 
#define PATKERNEL_IPATTTCOUNTER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace LHCb{
  class MCParticle;
  class LHCbID;
}


static const InterfaceID IID_IPatTTCounter ( "IPatTTCounter", 1, 0 );

/** @class IPatTTCounter IPatTTCounter.h PatKernel/IPatTTCounter.h
 *  
 *
 *  @author Wenbin Qian
 *  @date   2011-03-21
 */
class IPatTTCounter : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPatTTCounter; }
  
  virtual StatusCode finalize() = 0;  
  
  virtual void initEvent() = 0;
  
  virtual void count( const LHCb::MCParticle* part, 
              std::vector<bool> flags, 
              std::vector<LHCb::LHCbID>& ids ) = 0;
  
  virtual void setContainer( std::string name ) = 0 ;
  
  virtual void addSelection ( std::string name ) = 0;
  
  virtual void printStatistics() = 0;


protected:

private:

};
#endif // PATKERNEL_IPATTTCOUNTER_H
