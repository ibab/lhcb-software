#ifndef PRKERNEL_IPRCOUNTER_H 
#define PRKERNEL_IPRCOUNTER_H 1

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


static const InterfaceID IID_IPrCounter ( "IPrCounter", 1, 0 );

/** @class IPrCounter IPrCounter.h PrKernel/IPrCounter.h
 *  
 *
 *  @author Michel De Cian & Paolo Gandini & Thomas Nikodem
 *  @date   2013-12-13
 */
class IPrCounter : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPrCounter; }
  
  virtual StatusCode finalize() = 0;  
  
  virtual void initEvent(const IHistoTool* htool = NULL, const int nPV = 0) = 0;

  virtual int  countAndPlot(const IHistoTool* htool,
                            const LHCb::MCParticle* part, 
                            std::vector<bool> flags,
                            std::vector<LHCb::LHCbID>& ids, 
                            const int nPV) = 0;

  virtual void addSelection ( std::string name, bool writeHisto ) = 0;

  void setContainer( std::string name )         { m_container = name; }
  void setWriteHistos( int write )              { m_writeHistos = write; }
  void setUseEta25Cut( bool cut )               { m_eta25cut = cut;}
  void setTriggerNumbers( bool numbers )        { m_triggerNumbers = numbers;}
  void setTrackExtrapolation( bool extra )      { m_trackextrapolation = extra;}
  void setSelectId( int data )                  { m_selectId = data; }
  void setTrackType( LHCb::Track::Types type )  { m_trackType = type; }
  void setGhostProbCut( double cutVal )         { m_ghostProbCut = cutVal;}
  void setTeXName(const std::string &directory, const std::string &name){
    m_writetex = true; m_texoutdir = directory; m_texoutname = name; }
  
    
  //virtual void setUse3MCLayerCut( bool cut) = 0;
    
  virtual void printStatistics() = 0;


protected:

  int          m_writeHistos;
  bool         m_eta25cut;
  bool         m_triggerNumbers;
  bool         m_trackextrapolation;
  int          m_selectId;
  std::string  m_container;
  double       m_ghostProbCut;
  bool         m_writetex;
  std::string  m_texoutname;
  std::string  m_texoutdir;

  LHCb::Track::Types m_trackType;

private:


};
#endif // PRKERNEL_IPRCOUNTER_H

