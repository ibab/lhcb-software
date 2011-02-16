// $Id: PackedTrack.h,v 1.8 2009-11-07 12:20:26 jonrob Exp $
#ifndef EVENT_PACKEDTRACK_H
#define EVENT_PACHEDTRACK_H 1

// Include files
#include "GaudiKernel/DataObject.h"
#include <string>
#include <vector>

namespace LHCb
{

  /** @class PackedTrack PackedTrack.h Event/PackedTrack.h
   *  Packed description of a track
   *
   *  Version 2: Added new data members to PackedTrack to follow the upgraded Track.
   *
   *  @author Olivier Callot
   *  @date   2009-08-26
   */
  struct PackedTrack
  {

    /// Default Constructor
    PackedTrack() :
      key(0), chi2PerDoF(0), nDoF(0), flags(0),
      firstId(0), lastId(0),
      firstState(0), lastState(0),
      firstExtra(0), lastExtra(0),
      likelihood(0), ghostProba(0)
    { }

    /// Copy Constructor
    PackedTrack( const PackedTrack& c ) :
      key( c.key ), chi2PerDoF( c.chi2PerDoF ), nDoF( c.nDoF ), flags( c.flags ),
      firstId( c.firstId ), lastId (c.lastId ),
      firstState( c.firstState ), lastState( c.lastState ),
      firstExtra( c.firstExtra ), lastExtra( c.lastExtra ),
      likelihood( c.likelihood ), ghostProba( c.ghostProba )
    { }

    int key;
    int chi2PerDoF;
    int nDoF;
    unsigned int flags;
    unsigned short int firstId;
    unsigned short int lastId;
    unsigned short int firstState;
    unsigned short int lastState;
    unsigned short int firstExtra;
    unsigned short int lastExtra;
    //== Added for version 3, August 2009
    int likelihood;
    int ghostProba;

    //== Note that Nodes and Measurements on Track are transient only, an thus never stored.
  };


  /** @class PackedState PackedState.h Event/PackedState.h
   *  Describe a packed state
   *
   *  @author Olivier Callot
   *  @date   2008-11-07
   */

  struct PackedState
  {

    /// Standard constructor
    PackedState( ) :
      flags(0),
      //
      x(0),y(0),z(0),tx(0),ty(0),p(0),
      //
      cov_00(0),
      cov_11(0),
      cov_22(0),
      cov_33(0),
      cov_44(0),
      cov_10(0),
      cov_20(0),
      cov_21(0),
      cov_30(0),
      cov_31(0),
      cov_32(0),
      cov_40(0),
      cov_41(0),
      cov_42(0),
      cov_43(0)
    { }

    int flags;

    int x;
    int y;
    int z;
    int tx;
    int ty;
    int p;

    int cov_00;
    int cov_11;
    int cov_22;
    int cov_33;
    int cov_44;
    short int cov_10;
    short int cov_20;
    short int cov_21;
    short int cov_30;
    short int cov_31;
    short int cov_32;
    short int cov_40;
    short int cov_41;
    short int cov_42;
    short int cov_43;

  };

  static const CLID CLID_PackedTracks = 1550;

  // Namespace for locations in TDS
  namespace PackedTrackLocation
  {
    static const std::string& Default = "pRec/Track/Best";
    static const std::string& Muon    = "pRec/Track/Muon";
  }

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

    virtual ~PackedTracks( ) {}; ///< Destructor
    virtual const CLID& clID()  const { return PackedTracks::classID(); }
    static  const CLID& classID()     { return CLID_PackedTracks;       }

    void addEntry( PackedTrack& obj ) { m_vect.push_back( obj ); }
    std::vector<PackedTrack>::const_iterator begin() const { return m_vect.begin(); }
    std::vector<PackedTrack>::const_iterator end()   const { return m_vect.end(); }
    std::vector<PackedTrack>& tracks() { return m_vect; }
    const std::vector<PackedTrack>& tracks() const { return m_vect; }

    void addId( int i ) { m_ids.push_back( i ); }
    std::vector<int>::const_iterator beginIds() const { return m_ids.begin(); }
    unsigned int  sizeId()                      const { return m_ids.size(); }
    std::vector<int>& ids() { return m_ids; }
    const std::vector<int>& ids() const { return m_ids; }

    void addState( PackedState& obj ) { m_state.push_back( obj ); }
    std::vector<PackedState>::const_iterator beginState() const { return m_state.begin(); }
    unsigned int sizeState()                         const { return m_state.size(); }
    std::vector<PackedState>& states() { return m_state; }
    const std::vector<PackedState>& states() const { return m_state; }

    void addExtra( int a, int b ) { std::pair<int,int> tmp( a, b ); m_extra.push_back( tmp ); }
    std::vector<std::pair<int,int> >::const_iterator beginExtra() const { return m_extra.begin(); }
    unsigned int sizeExtra()                         const { return m_extra.size(); }
    std::vector<std::pair<int,int> >& extras() { return m_extra; }
    const std::vector<std::pair<int,int> >& extras() const { return m_extra; }

  private:

    std::vector<PackedTrack> m_vect;
    std::vector<PackedState> m_state;
    std::vector<int>    m_ids;
    std::vector<std::pair<int,int> > m_extra;

  };

}  // End of LHCb namespace

#endif // EVENT_PACKEDTRACK_H
