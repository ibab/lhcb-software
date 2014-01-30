// $Id: $
#ifndef PRCOUNTER_H
#define PRCOUNTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Event/MCParticle.h"
#include "Event/Track.h"
#include "Linker/LinkerTool.h"
#include "PrKernel/IPrCounter.h"


/** @class PrCounter PrCounter.h
 *  This is a counter for track efficiency measurement.
 *
 *  @author Olivier Callot
 *  @date   2005-06-10
 *  Modified by Wenbin Qian for the VP Pat Efficiency
 *  @adpate to A-Team framework 2007-08-20 SHM
 */


class PrCounter : virtual public IPrCounter, public GaudiTool {
public:

  /// Standard constructor
  PrCounter( const std::string& type,
              const std::string& name,
              const IInterface* parent);

  virtual ~PrCounter( ); ///< Destructor

  virtual StatusCode finalize();

  void initEvent(const IHistoTool* htool, const int nPV);

  int countAndPlot(const IHistoTool* htool,const LHCb::MCParticle* part, std::vector<bool> flags,
                       std::vector<LHCb::LHCbID>& ids, const int nPV);

  void addSelection ( std::string name, bool writeHisto );

  void printStatistics();

  int  nbTrack() const { return m_nbTrack; }
  int  nbGhost() const { return m_nbGhost; }

protected:

private:
  typedef LinkerTool<LHCb::Track, LHCb::MCParticle> MyAsct;
  typedef MyAsct::DirectType            Table;
  typedef MyAsct::DirectType::Range     Range;
  typedef Table::iterator               iterator;

  typedef MyAsct::InverseType           InvTable;
  typedef InvTable::Range               InvRange;
  typedef InvTable::iterator            InvIterator;

  MyAsct*         m_link;
  const InvTable* m_invTable;

  std::string  m_title;
  unsigned int m_titleSize;

  bool         m_validData;

  // Event variables
  int          m_nbTrack;                 ///< Tracks for this event
  int          m_nbGhost;                 ///< Ghosts for this event


  // total variables
  int          m_totTrack;                ///< Total number of tracks processed
  int          m_totGhost;                ///< Total number of ghosts
  int          m_totTrackTrigger;                ///< Total number of tracks processed
  int          m_totGhostTrigger;                ///< Total number of ghosts
  double       m_fracGhost;
  double       m_nEvent;

  std::vector<std::string> m_name;    ///< Name of the sub-counters
  std::vector<bool>        m_writeHisto;    ///< Make histograms for this container
  std::vector<int>         m_wanted;  ///< Nb MC tracks measurable.
  std::vector<int>         m_counted; ///< counters for statistics
  std::vector<int>         m_clone;   ///< counters for clones
  std::vector<double>      m_purity;  ///< Sum of purity (linker weight)
  std::vector<double>      m_hitEff;  ///< Sum of hitEfficiency
};

#endif // PRCOUNTER_H
