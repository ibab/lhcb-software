// $Id: PackedTwoProngVertex.h,v 1.2 2009-11-07 12:20:26 jonrob Exp $
#ifndef EVENT_PACKEDTWOPRONGVERTEX_H
#define EVENT_PACKEDTWOPRONGVERTEX_H 1

// Include files
#include "GaudiKernel/DataObject.h"
#include <string>
#include <vector>

namespace LHCb
{

  // Namespace for locations in TDS
  namespace PackedTwoProngVertexLocation
  {
    static const std::string& Default = "pRec/Vertex/V0";
  }

  /** @class PackedTwoProngVertex Event/PackedTwoProngVertex.h
   *
   *  Packed representation of LHCb::TwoProngVertex
   * 
   *  @author Olivier Callot
   *  @date   2009-01-21
   */
  struct PackedTwoProngVertex
  {

    int key{0};
    int technique{0};
    int chi2{0};
    int nDoF{0};
    int x{0};
    int y{0};
    int z{0};

    int txA{0};
    int tyA{0};
    int pA{0};

    int txB{0};
    int tyB{0};
    int pB{0};

    int cov00{0};
    int cov11{0};
    int cov22{0};
    int cov33{0};
    int cov44{0};
    int cov55{0};
    int cov66{0};
    int cov77{0};
    int cov88{0};
    int cov99{0};

    short int cov10{0};
    short int cov20{0}, cov21{0};
    short int cov30{0}, cov31{0}, cov32{0};
    short int cov40{0}, cov41{0}, cov42{0}, cov43{0};
    short int cov50{0}, cov51{0}, cov52{0}, cov53{0}, cov54{0};
    short int cov60{0}, cov61{0}, cov62{0}, cov63{0}, cov64{0}, cov65{0};
    short int cov70{0}, cov71{0}, cov72{0}, cov73{0}, cov74{0}, cov75{0}, cov76{0};
    short int cov80{0}, cov81{0}, cov82{0}, cov83{0}, cov84{0}, cov85{0}, cov86{0}, cov87{0};

    unsigned short int firstTrack{0};
    unsigned short int lastTrack{0};
    unsigned short int firstInfo{0};
    unsigned short int lastInfo{0};
    unsigned short int firstPid{0};
    unsigned short int lastPid{0};

  };

  static const CLID CLID_PackedTwoProngVertices = 1554;

  /** @class PackedTwoProngVertices Event/PackedTwoProngVertex.h
   *
   *  Container of packed LHCb::TwoProngVertex objects.
   * 
   *  @author Olivier Callot
   *  @date   2009-01-21
   */

  class PackedTwoProngVertices : public DataObject
  {

  public:

    /// Default Packing Version
    static char defaultPackingVersion() { return 1; }

  public:

    virtual const CLID& clID()  const { return PackedTwoProngVertices::classID(); }
    static  const CLID& classID()     { return CLID_PackedTwoProngVertices;       }

  public:

    std::vector<PackedTwoProngVertex>&       vertices()          { return m_vect; }
    const std::vector<PackedTwoProngVertex>& vertices() const    { return m_vect; }

    std::vector<long long>&       refs()                         { return m_refs; }
    const std::vector<long long>& refs() const                   { return m_refs; }

    std::vector<std::pair<int,int> >&       extras()             { return m_extra; }
    const std::vector<std::pair<int,int> >& extras() const       { return m_extra; }

  public:

    /// Set the packing version
    void setPackingVersion( const char ver ) { m_packingVersion = ver; }
    
    /// Access the packing version
    char packingVersion() const { return m_packingVersion; }
    
  private:

    std::vector<PackedTwoProngVertex>  m_vect;
    std::vector<long long>             m_refs;
    std::vector<std::pair<int,int> >   m_extra;

    /** Data packing version.  
     *  Packing version must be set to 0 by default, for compatibility with
     *  data written before the version was added */
    char m_packingVersion{0};

  };

} // End of LHCb namespace

#endif // EVENT_PACKEDTWOPRONGVERTEX_H
