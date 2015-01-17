#ifndef PRKERNEL_IPRTTCOUNTER_H 
#define PRKERNEL_IPRTTCOUNTER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/Track.h"


namespace LHCb{
  class MCParticle;
  class LHCbID;
}

class IHistoTool;


static const InterfaceID IID_IPrTTCounter ( "IPrTTCounter", 1, 0 );

/** @class IPrTTCounter IPrTTCounter.h PrKernel/IPrTTCounter.h
 *  
 *
 *  @author Michel De Cian & Paolo Gandini & Thomas Nikodem & Svende Braun
 *  @date   2015-01-17
 */
class IPrTTCounter : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPrTTCounter; }
  
  virtual StatusCode finalize() = 0;  
  
  virtual void initEvent(const IHistoTool* htool = NULL) = 0;

  virtual int  countAndPlot(const IHistoTool* htool,
                            const LHCb::MCParticle* part, 
                            std::vector<bool> flags,
                            std::vector<LHCb::LHCbID>& ids ) = 0;

  virtual void addSelection ( std::string name, bool writeHisto ) = 0;

  void setContainer( std::string name )  { m_container = name; };
  void setWriteHistos(int write){ m_writeHistos = write; };
  void setUseEta25Cut(bool cut){ m_eta25cut = cut;};
  void setTrackExtrapolation(bool extra){m_trackextrapolation = extra;};
  void setSelectId( int data )   { m_selectId = data; };
  void setTriggerNumbers(bool numbers){ m_triggerNumbers = numbers;};

 

  //virtual void setUse3MCLayerCut( bool cut) = 0;
    
  virtual void printStatistics() = 0;


protected:

  int          m_writeHistos;
  bool         m_eta25cut;
  bool         m_triggerNumbers;
  bool         m_trackextrapolation;
  int          m_selectId;
  std::string  m_container;


private:



};
#endif // PRKERNEL_IPRTTCOUNTER_H

