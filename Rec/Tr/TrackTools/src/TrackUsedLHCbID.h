// $Id: TrackUsedLHCbID.h,v 1.2 2007-03-13 14:32:31 cattanem Exp $
#ifndef _TrackUsedLHCbID_H
#define _TrackUsedLHCbID_H

/** @class TrackUsedLHCbID TrackUsedLHCbID.h
 *
 * Implementation of TrackUsedLHCbID
 * check if an LHCbID is used
 *
 * @author M.Needham
 * @date   2/08/2006
 *
 * @author M. Schiller
 * @date 2015-02-21
 *  - use BloomFilters to achieve O(1) lookup instead of O(log(nHits))
 */

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "Kernel/IUsedLHCbID.h"

#include <string>
#include <vector>
#include <cstdint>

#include "Kernel/LHCbID.h"
#include "LHCbMath/BloomFilter.h"

class ITrackSelector;

namespace TrackUsedLHCbIDImplDetail {
    /// calculate number of occupied channels with safety factor
    constexpr uint64_t occCh(uint64_t nch, uint64_t occnumer,
	uint64_t occdenom, uint64_t safety = 4)
    { return (safety * nch * occnumer) / occdenom; }
    /// calculate BloomFilter capacity for detector with given number of channels and occupancy
    constexpr uint64_t cap(uint64_t nch, uint64_t occnumer, uint64_t occdenom)
    { return nch < occCh(nch, occnumer, occdenom) ? nch :
      occCh(nch, occnumer, occdenom); }
}

class TrackUsedLHCbID: public GaudiTool,
	                virtual public IUsedLHCbID,
                        virtual public IIncidentListener
{
  public:
    /** constructor */
    TrackUsedLHCbID(const std::string& type,
	const std::string& name,
	const IInterface* parent);

    /** destructor */
    virtual ~TrackUsedLHCbID();

    /** intialize */
    StatusCode initialize();

    /** Test if the LHCbID is used
     * @param id to be test tested
     *  @return true if used
     */
    bool used(const LHCb::LHCbID id) const;

    /** Implement the handle method for the Incident service.
     *  This is used to nform the tool of software incidents.
     *
     *  @param incident The incident identifier
     */
    void handle( const Incident& incident );

  private:

    void initEvent() const;

    typedef std::vector<std::string> TrackContainers;
    typedef std::vector<ITrackSelector*> Selectors;
    TrackContainers m_inputs;
    // for track selection
    std::vector<std::string> m_names;
    Selectors m_selectors;

    // set maximum number of hits expected in the BloomFilter:
    // if there's more hits in the detector than that, the number of collisions
    // (one hit mistaken for another) will rise above the threshold (1e-4)
    //
    // strategy: max(safety * maxocc * nChannels, nChannels) with safety = 4
    static constexpr uint64_t s_MaxVeloHits = TrackUsedLHCbIDImplDetail::cap( 180000u,   1u,    100u);
    static constexpr uint64_t s_MaxTTHits =   TrackUsedLHCbIDImplDetail::cap( 150000u,   2u,    100u);
    static constexpr uint64_t s_MaxITHits =   TrackUsedLHCbIDImplDetail::cap( 130000u,   2u,    100u);
    static constexpr uint64_t s_MaxOTHits =   TrackUsedLHCbIDImplDetail::cap(  54000u,  15u,    100u);
    static constexpr uint64_t s_MaxVPHits =   TrackUsedLHCbIDImplDetail::cap(4100000u, 125u, 100000u);
    static constexpr uint64_t s_MaxUTHits =   TrackUsedLHCbIDImplDetail::cap( 540000u,  18u,   1000u);
    static constexpr uint64_t s_MaxFTHits =   TrackUsedLHCbIDImplDetail::cap( 300000u,   2u,    100u);
    static constexpr uint64_t s_MaxOtherHits = 1024u; // up to 1024 which don't fit above
    static constexpr uint64_t s_denom = 1u << 20u;
    static constexpr uint64_t s_numer = 1u * s_denom / 10000u;

    /// flag bits
    enum { Initialized = 1, Velo = 2, TT = 4, T = 8, VP = 16, UT = 32, FT = 64, Other = 128 };
    mutable unsigned m_flags; ///< flags
    // since current and Upgrade detectors are never used in the same job, we
    // can eke out some memory by putting corresponding detectors in a union
    // each, so they "share" the memory by overlapping in the same physical
    // memory location
    mutable union VtxHits {
      BloomFilter<LHCb::LHCbID, s_MaxVeloHits, s_numer, s_denom> m_veloHits;
      BloomFilter<LHCb::LHCbID, s_MaxVPHits, s_numer, s_denom> m_vpHits;
      VtxHits() : m_veloHits() { }
    } m_vtx; ///< vertex detector hits
    mutable union BeforeMagnetHits {
      mutable BloomFilter<LHCb::LHCbID, s_MaxTTHits, s_numer, s_denom> m_ttHits;
      mutable BloomFilter<LHCb::LHCbID, s_MaxUTHits, s_numer, s_denom> m_utHits;
      BeforeMagnetHits() : m_ttHits() { }
    } m_bmg; ///< hits in tracking detectors before the magnet
    mutable union AfterMagnetHits {
      mutable BloomFilter<LHCb::LHCbID, s_MaxITHits + s_MaxOTHits, s_numer, s_denom> m_tHits;
      mutable BloomFilter<LHCb::LHCbID, s_MaxFTHits, s_numer, s_denom> m_ftHits;
      AfterMagnetHits() : m_tHits() { }
    } m_amg; ///< hits in tracking detectors after the magnet
    mutable BloomFilter<LHCb::LHCbID, s_MaxOtherHits, s_numer, s_denom> m_otherHits;
};

#endif
