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

    /// Standard constructor
    PackedTwoProngVertex( ) :
      //
      key(0),technique(0),chi2(0),nDoF(0),x(0),y(0),z(0),
      //
      txA(0),tyA(0),pA(0),txB(0),tyB(0),pB(0),
      //
      cov00(0),
      cov11(0),
      cov22(0),
      cov33(0),
      cov44(0),
      cov55(0),
      cov66(0),
      cov77(0),
      cov88(0),
      cov99(0),
      //
      cov10(0),
      cov20(0), cov21(0),
      cov30(0), cov31(0), cov32(0),
      cov40(0), cov41(0), cov42(0), cov43(0),
      cov50(0), cov51(0), cov52(0), cov53(0), cov54(0),
      cov60(0), cov61(0), cov62(0), cov63(0), cov64(0), cov65(0),
      cov70(0), cov71(0), cov72(0), cov73(0), cov74(0), cov75(0), cov76(0),
      cov80(0), cov81(0), cov82(0), cov83(0), cov84(0), cov85(0), cov86(0), cov87(0),
      //
      firstTrack(0),
      lastTrack(0),
      firstInfo(0),
      lastInfo(0),
      firstPid(0),
      lastPid(0)
    {}

    int key;
    int technique;
    int chi2;
    int nDoF;
    int x;
    int y;
    int z;

    int txA;
    int tyA;
    int pA;

    int txB;
    int tyB;
    int pB;

    int cov00;
    int cov11;
    int cov22;
    int cov33;
    int cov44;
    int cov55;
    int cov66;
    int cov77;
    int cov88;
    int cov99;

    short int cov10;
    short int cov20, cov21;
    short int cov30, cov31, cov32;
    short int cov40, cov41, cov42, cov43;
    short int cov50, cov51, cov52, cov53, cov54;
    short int cov60, cov61, cov62, cov63, cov64, cov65;
    short int cov70, cov71, cov72, cov73, cov74, cov75, cov76;
    short int cov80, cov81, cov82, cov83, cov84, cov85, cov86, cov87;

    unsigned short int firstTrack;
    unsigned short int lastTrack;
    unsigned short int firstInfo;
    unsigned short int lastInfo;
    unsigned short int firstPid;
    unsigned short int lastPid;

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

    /** Standard constructor
     *  Packing version must be set to 0 by default, for compatibility with
     *  data written before the version was added */
    PackedTwoProngVertices( ) : m_packingVersion(0) { }

    virtual ~PackedTwoProngVertices( ) {}; ///< Destructor

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

    /// Data packing version
    char m_packingVersion;

  };

} // End of LHCb namespace

#endif // EVENT_PACKEDTWOPRONGVERTEX_H
