// $Id: PackedTrack.h,v 1.8 2009-11-07 12:20:26 jonrob Exp $
#ifndef EVENT_PACKEDTRACK_H
#define EVENT_PACHEDTRACK_H 1

#include <string>
#include <vector>

// Kernel
#include "Event/StandardPacker.h"

// Event
#include "Event/Track.h"

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/StatusCode.h"

namespace LHCb
{

  /** @class PackedTrack PackedTrack.h Event/PackedTrack.h
   *
   *  Packed description of a track
   *
   *  Version 2: Added new data members to PackedTrack to follow the upgraded Track.
   *
   *  @author Olivier Callot
   *  @date   2009-08-26
   */
  struct PackedTrack
  {

    long long key{0};
    int chi2PerDoF{0};
    int nDoF{0};
    unsigned int flags{0};
    unsigned short int firstId{0};
    unsigned short int lastId{0};
    unsigned short int firstState{0};
    unsigned short int lastState{0};
    unsigned short int firstExtra{0};
    unsigned short int lastExtra{0};
    //== Added for version 3, August 2009
    int likelihood{0};
    int ghostProba{0};

    //== Note that Nodes and Measurements on Track are transient only, an thus never stored.
  };

  /** @class PackedState PackedState.h Event/PackedState.h
   *
   *  Describe a packed state
   *
   *  @author Olivier Callot
   *  @date   2008-11-07
   */
  struct PackedState
  {

    int flags{0};

    int x{0};
    int y{0};
    int z{0};
    int tx{0};
    int ty{0};
    int p{0};

    int cov_00{0};
    int cov_11{0};
    int cov_22{0};
    int cov_33{0};
    int cov_44{0};
    short int cov_10{0};
    short int cov_20{0};
    short int cov_21{0};
    short int cov_30{0};
    short int cov_31{0};
    short int cov_32{0};
    short int cov_40{0};
    short int cov_41{0};
    short int cov_42{0};
    short int cov_43{0};

  };

  static const CLID CLID_PackedTracks = 1550;

  // Namespace for locations in TDS
  namespace PackedTrackLocation
  {
    static const std::string& Default  =  "pRec/Track/Best";
    static const std::string& Muon     =  "pRec/Track/Muon";
    static const std::string& InStream = "/pRec/Track/Custom";
  }

  /** @class PackedTracks PackedTrack.h Event/PackedTrack.h
   *
   *  Container of packed Tracks
   *
   *  @author Olivier Callot
   *  @date   2009-08-26
   */

  class PackedTracks : public DataObject
  {

  public:

    /// Standard constructor
    PackedTracks( )
    {
      m_vect.reserve(500);
      m_state.reserve(2000);
      m_ids.reserve(2000);
      m_extra.reserve(5000);
    }

  public:

    virtual const CLID& clID()  const { return PackedTracks::classID(); }
    static  const CLID& classID()     { return CLID_PackedTracks;       }

  public:

    std::vector<PackedTrack>&       tracks()               { return m_vect; }
    const std::vector<PackedTrack>& tracks() const         { return m_vect; }

    std::vector<int>&       ids()                          { return m_ids; }
    const std::vector<int>& ids() const                    { return m_ids; }

    std::vector<PackedState>&       states()               { return m_state; }
    const std::vector<PackedState>& states() const         { return m_state; }

    std::vector<std::pair<int,int> >&       extras()       { return m_extra; }
    const std::vector<std::pair<int,int> >& extras() const { return m_extra; }

  private:

    std::vector<PackedTrack>         m_vect;
    std::vector<PackedState>         m_state;
    std::vector<int>                 m_ids;
    std::vector<std::pair<int,int> > m_extra;

  };

  /** @class TrackPacker Event/PackedTrack.h
   *
   *  Utility class to handle the packing and unpacking of Tracks
   *
   *  @author Christopher Rob Jones
   *  @date   05/04/2012
   */
  class TrackPacker
  {

  public:

    typedef LHCb::Track                    Data;
    typedef LHCb::PackedTrack        PackedData;
    typedef LHCb::Tracks             DataVector;
    typedef LHCb::PackedTracks PackedDataVector;

  public:

    /// Default Constructor
    TrackPacker( const GaudiAlgorithm & p ) : m_pack(&p) { resetWrappingCounts(); }

  public:

    /// Pack a Track
    void pack( const Data & track,
               PackedData & ptrack,
               PackedDataVector & ptracks ) const;

    /// Pack Tracks
    void pack( const DataVector & tracks,
               PackedDataVector & ptracks ) const;

    /// Unpack a single Track
    void unpack( const PackedData       & ptrack,
                 Data                   & track,
                 const PackedDataVector & ptracks,
                 DataVector             & tracks ) const;

    /// Unpack Tracks
    void unpack( const PackedDataVector & ptracks,
                 DataVector             & tracks ) const;

    /// Compare two Track containers to check the packing -> unpacking performance
    StatusCode check( const DataVector & dataA,
                      const DataVector & dataB ) const;

    /// Compare two Tracks to check the packing -> unpacking performance
    StatusCode check( const Data & dataA,
                      const Data & dataB ) const;

  private:

    /// Convert a state to a packed state
    void convertState( const LHCb::State& state,
                       PackedDataVector & ptracks ) const;

    /// Convert a packed state to a state in a track
    void convertState( const LHCb::PackedState& pSta,
                       LHCb::Track & tra ) const;

    void compareStates( const LHCb::State& oSta,
                        const LHCb::State& tSta ) const;

  private:

    /// Access the parent algorithm
    inline const GaudiAlgorithm& parent() const { return *(m_pack.parent()); }

    /// Safe sqrt ...
    inline double safe_sqrt( const double x ) const
    { return ( x > 0 ? std::sqrt(x) : 0.0 ); }

    /// Reset wraping bug counts
    inline void resetWrappingCounts() const
    {
      m_firstIdHigh    = 0;
      m_lastIdHigh     = 0;
      m_firstStateHigh = 0;
      m_lastStateHigh  = 0;
      m_firstExtraHigh = 0;
      m_lastExtraHigh  = 0;
    }

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

  private:

    // cached data to handle wrapped ID numbers ...
    mutable int m_firstIdHigh{0};
    mutable int m_lastIdHigh{0};
    mutable int m_firstStateHigh{0};
    mutable int m_lastStateHigh{0};
    mutable int m_firstExtraHigh{0};
    mutable int m_lastExtraHigh{0};

  };

}  // End of LHCb namespace

#endif // EVENT_PACKEDTRACK_H
