// $Id: PackedMCRichOpticalPhoton.h,v 1.4 2009-11-07 12:20:26 jonrob Exp $
#ifndef EVENT_PACKEDMCRICHOPTICALPHOTON_H
#define EVENT_PACKEDMCRICHOPTICALPHOTON_H 1

#include <string>

// Kernel
#include "Event/StandardPacker.h"

// Event
#include "Event/MCRichOpticalPhoton.h"

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

namespace LHCb
{
  // -----------------------------------------------------------------------

  /** @struct PackedMCRichOpticalPhoton Event/PackedMCRichOpticalPhoton.h
   *
   *  Packed MCRichOpticalPhoton
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  struct PackedMCRichOpticalPhoton
  {
    int   key{0};
    int   hpdx{0}, hpdy{0}, hpdz{0};
    int   pmirx{0}, pmiry{0}, pmirz{0};
    int   smirx{0}, smiry{0}, smirz{0};
    int   aerox{0}, aeroy{0}, aeroz{0};
    int   theta{0}, phi{0};
    int   emisx{0}, emisy{0}, emisz{0};
    int   energy{0};
    int   pmomx{0}, pmomy{0}, pmomz{0};
    int   hpdqwx{0}, hpdqwy{0}, hpdqwz{0};
    long long mcrichhit{-1};
  };

  // -----------------------------------------------------------------------

  static const CLID CLID_PackedMCRichOpticalPhotons = 1522;

  /// Namespace for locations in TDS
  namespace PackedMCRichOpticalPhotonLocation
  {
    static const std::string& Default = "pSim/Rich/OpticalPhotons";
  }

  /** @class PackedMCRichOpticalPhotons Event/PackedMCRichOpticalPhoton.h
   *
   *  Packed MCRichOpticalPhotons
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class PackedMCRichOpticalPhotons : public DataObject
  {
  public:

    /// Vector of packed objects
    typedef std::vector<LHCb::PackedMCRichOpticalPhoton> Vector;

  public:
    
    /// Default Packing Version
    static char defaultPackingVersion() { return 1; }

  public:

    /// Class ID
    static const CLID& classID() { return CLID_PackedMCRichOpticalPhotons; }

    /// Class ID
    virtual const CLID& clID() const { return PackedMCRichOpticalPhotons::classID(); }

  public:

    /// Write access to the data vector
    Vector & data()             { return m_vect; }

    /// Read access to the data vector
    const Vector & data() const { return m_vect; }

    /// Set the packing version
    void setPackingVersion( const char ver ) { m_packingVersion = ver; }

    /// Access the packing version
    char packingVersion() const { return m_packingVersion; }

  private:

    /// Data packing version
    char   m_packingVersion{ defaultPackingVersion() };

    /// The packed data objects
    Vector m_vect; 

  };

  // -----------------------------------------------------------------------

  /** @class MCRichOpticalPhotonPacker Event/PackedMCRichOpticalPhoton.h
   *
   *  Utility class to handle the packing and unpacking of the MCRichOpticalPhotons
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class MCRichOpticalPhotonPacker
  {
  public:

    // These are required by the templated algorithms
    typedef LHCb::MCRichOpticalPhoton                    Data;
    typedef LHCb::PackedMCRichOpticalPhoton        PackedData;
    typedef LHCb::MCRichOpticalPhotons             DataVector;
    typedef LHCb::PackedMCRichOpticalPhotons PackedDataVector;
    static const std::string& packedLocation()   { return LHCb::PackedMCRichOpticalPhotonLocation::Default; }
    static const std::string& unpackedLocation() { return LHCb::MCRichOpticalPhotonLocation::Default; }

  private:

    /// Default Constructor
    MCRichOpticalPhotonPacker() {}

  public:

    /// Constructor
    MCRichOpticalPhotonPacker( const GaudiAlgorithm * parent ) : m_pack(parent) { }

  public:

    /// Pack an MCRichOpticalPhoton
    void pack( const DataVector & phots,
               PackedDataVector & pphots ) const;

    /// Unpack an MCRichOpticalPhoton
    void unpack( const PackedDataVector & pphots,
                 DataVector             & phots ) const;

    /// Compare two MCRichHits to check the packing -> unpacking performance
    StatusCode check( const DataVector & dataA,
                      const DataVector & dataB ) const;

  private:

    /// Access the parent algorithm
    const GaudiAlgorithm& parent() const { return *(m_pack.parent()); }

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

  private:
    
    /// Scale factor for photon energies
    double PhotEnScale{ 5.0e8 };

  };

  // -----------------------------------------------------------------------

}

#endif // EVENT_PACKEDMCRICHOPTICALPHOTON_H
