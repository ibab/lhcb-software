// $Id: L0MuonMuonComp.h,v 1.4 2010-03-08 15:07:35 jucogan Exp $
#ifndef COMPONENT_L0MUONMUONCOMP_H 
#define COMPONENT_L0MUONMUONCOMP_H 1

// Include files
#include <fstream>

//#include "Kernel/IEventTimeDecoder.h"
#include "MuonDAQ/IMuonRawBuffer.h"
#include "MuonKernel/MuonSystemLayout.h"

#include "L0MuonMonitorBase.h"
#include "L0MuonPadsHistos.h"
#include "L0MuonChannelsHistos.h"
#include "L0MuonOptLinksHistos.h"

/** @class L0MuonMuonComp L0MuonMuonComp.h component/L0MuonMuonComp.h
 *  
 *
 *  @author Julien Cogan
 *  @date   2008-04-08
 */
  

class L0MuonMuonComp : public L0MuonMonitorBase {
public: 
  /// Standard constructor
  L0MuonMuonComp( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0MuonMuonComp( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  StatusCode compareTiles(std::vector<std::pair<LHCb::MuonTileID,double > > & muontiles);
  
protected:

private:

  StatusCode getMuonTilesTAE(std::vector<std::pair<LHCb::MuonTileID, std::pair<int,int> > > & muontiles);
  StatusCode getMuonPadsTAE(std::vector<std::pair<LHCb::MuonTileID, int > > & muonpads);
  StatusCode getCandPadsTAE(std::vector<std::pair<LHCb::MuonTileID, int > > & candpads);
  StatusCode isMuonTruncatedTAE( bool & truncated);

  bool diffCandAndData(std::vector<std::pair<LHCb::MuonTileID,int > > & candpads,
                       std::vector<std::pair<LHCb::MuonTileID,int > > & datapads);

  bool diffL0MuonAndMuon(std::vector<std::pair<LHCb::MuonTileID,int > > & l0muontiles,
                         std::vector<std::pair<LHCb::MuonTileID,std::pair<int,int> > > & muontiles,
                         std::vector<std::pair<LHCb::MuonTileID,int > > & diff);
  
  void errorSummary() ;
  void olsCounterSummary() ;
  void tilesCounterSummary(const std::map<LHCb::MuonTileID,int> &tiles_counter) ;
  
  std::map<LHCb::MuonTileID,int> m_tiles_mismatch;
  std::map<LHCb::MuonTileID,int> m_tiles_not_aligned;
  std::map<LHCb::MuonTileID,int> m_tiles_not_centered;
  std::map<LHCb::MuonTileID,int> m_tiles_missing_in_muon;
  std::map<LHCb::MuonTileID,int> m_tiles_missing_in_l0muon;
  std::map<LHCb::MuonTileID,int> m_tiles_present;
  std::map<LHCb::MuonTileID,int> m_tiles_present_bis;
  std::map<LHCb::MuonTileID,int> m_tiles_absent;              

  AIDA::IHistogram1D * m_h_mismatch;          
  AIDA::IHistogram1D * m_h_not_aligned;       
  AIDA::IHistogram1D * m_h_not_centered;      
  AIDA::IHistogram1D * m_h_missing_in_muon;   
  AIDA::IHistogram1D * m_h_missing_in_l0muon; 
  AIDA::IHistogram1D * m_h_present;           

  int m_event_counter;

  std::vector<LHCb::MuonTileID> m_full_tile_list;

  bool m_muonZS;                 /// use Muon ZeroSuppressed banks
  std::string m_outputFileName;  /// Output file name
  bool m_pdm;                    /// Special treatment for pdm runs
  bool m_splash;                 /// Print splash event in output file 
  bool m_tae_ignore_border;      /// Don't consider TS at the border of the TAE in the comparison
  bool m_use_central_finetime;   /// Use the muon fine time to restrict comparision to central hit in the TS 

  std::ofstream m_fout;
  
  //  IEventTimeDecoder* m_odin;

  L0MuonChannelsHistos* m_channelHist_l0muon;
  L0MuonChannelsHistos* m_channelHist_muon;
  L0MuonChannelsHistos* m_channelHist_l0muononly;
  L0MuonChannelsHistos* m_channelHist_muononly;
  L0MuonChannelsHistos* m_channelHist_mismatch;
  L0MuonOptLinksHistos* m_optlinkHist_mismatch;
  L0MuonOptLinksHistos* m_optlinkHist_error;  
  L0MuonPadsHistos*     m_padHist_addCand;
  L0MuonOptLinksHistos* m_optlinkHist_addCand;
  
  IMuonRawBuffer* m_muonBuffer;

  void initFullTileList(); 

  
};
#endif // COMPONENT_L0MUONMUONCOMP_H
