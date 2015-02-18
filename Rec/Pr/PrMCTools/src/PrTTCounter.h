// $Id: PrTTCounter.h,v 1.2 2007-02-26 12:27:30 ocallot Exp $
#ifndef PRTTCOUNTER_H 
#define PRTTCOUNTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Event/MCParticle.h"
#include "Event/Track.h"
#include "Linker/LinkerTool.h"
#include "PrKernel/IPrTTCounter.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
//#include "MCInterfaces/IIdealStateCreator.h"
#include "GaudiKernel/ToolHandle.h"
#include "TrackInterfaces/ITrackSelector.h"


/** @class PrTTCounter PrTTCounter.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2006-06-28
 *  2015-01-17 : modified by Svende Braun, Michel de Cian to work with PrChecker2.cpp
 */
class PrTTCounter : virtual public IPrTTCounter, public GaudiTool {
public: 

   /// Standard constructor
  PrTTCounter( const std::string& type, 
               const std::string& name,
               const IInterface* parent);

  virtual ~PrTTCounter( ); ///< Destructor
  virtual StatusCode initialize();
  virtual StatusCode finalize();  

  void initEvent(const IHistoTool* htool);

  int countAndPlot(const IHistoTool* htool, const LHCb::MCParticle* part, 
              std::vector<bool> flags, 
              std::vector<LHCb::LHCbID>& ids );

 
  void addSelection ( std::string name, bool writeHisto );
  
  void printStatistics();

protected:

private:
  typedef LinkerTool<LHCb::Track, LHCb::MCParticle> TrackAsct;
  typedef TrackAsct::InverseType InvTable;
  typedef InvTable::Range        InvRange;
  typedef InvTable::iterator     InvIterator;

  TrackAsct*      m_link;
  const InvTable* m_invTable;

  ITrackExtrapolator* m_extrapolator;
  //IIdealStateCreator* m_idealStateCreator;
  
  std::string  m_title;
  unsigned int m_titleSize;

  bool         m_validData;

  double       m_nbGhost;
  double       m_nbGhostHit;
  int          m_totTrack; 
  int          m_totTrackTrigger;                ///< Total number of tracks processed
  int          m_totGhostTrigger;
  
  
  std::vector<std::string> m_name;    ///< Name of the sub-counters
  std::vector<bool>        m_writeHisto;    ///< Make histograms for this container
  std::vector<double>      m_nbTrack;
  std::vector<double>      m_mcHits;          ///< Nb of MC hits on tracks
  std::vector<double>      m_foundOK;         ///< Nb of correct hits
  std::vector<double>      m_wrong;           ///< Nb of wrong ones
  std::vector<double>      m_nbTrack3;
  std::vector<double>      m_mcHits3;         ///< Nb of MC hits on tracks with >= 3 TT hits
  std::vector<double>      m_foundOK3;        ///< Nb of correct hits 3 TT hits
  std::vector<double>      m_wrong3;          ///< Nb of wrong ones 3 TT hits

  ToolHandle<ITrackSelector> m_selector;
};
#endif // PRTTCOUNTER_H
