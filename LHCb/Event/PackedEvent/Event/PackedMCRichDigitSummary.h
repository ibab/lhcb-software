// $Id: PackedMCRichDigitSummary.h,v 1.4 2009-11-07 12:20:26 jonrob Exp $
#ifndef EVENT_PackedMCRichDigitSummary_H
#define EVENT_PackedMCRichDigitSummary_H 1

#include <string>

// Kernel
#include "Kernel/StandardPacker.h"

// Event
#include "Event/MCRichDigitSummary.h"

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/StatusCode.h"

class GaudiAlgorithm;

namespace LHCb
{
  // -----------------------------------------------------------------------

  /** @struct PackedMCRichDigitSummary Event/PackedMCRichDigitSummary.h
   *
   *  Packed MCRichHit
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  struct PackedMCRichDigitSummary
  {
    /// Default constructor
    PackedMCRichDigitSummary()
      : history(0),
        richSmartID(0),
        mcParticle(-1)
    {}

    int history;
    int richSmartID;
    int mcParticle;
  };

  // -----------------------------------------------------------------------

  static const CLID CLID_PackedMCRichDigitSummarys = 1527;

  /// Namespace for locations in TDS
  namespace PackedMCRichDigitSummaryLocation
  {
    static const std::string& Default = "pSim/Rich/DigitSummaries";
  }

  /** @class PackedMCRichDigitSummarys Event/PackedMCRichDigitSummary.h
   *
   *  Packed MCRichDigitSummarys
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class PackedMCRichDigitSummarys : public DataObject
  {

  public:

    /// Vector of packed objects
    typedef std::vector<LHCb::PackedMCRichDigitSummary> Vector;

  public:
    
    /// Default Packing Version
    static char defaultPackingVersion() { return 0; }

  public:

    /// Standard constructor
    PackedMCRichDigitSummarys( ) : m_packingVersion(defaultPackingVersion()) { }

    /// Destructor
    virtual ~PackedMCRichDigitSummarys( ) { }

    /// Class ID
    static const CLID& classID() { return CLID_PackedMCRichDigitSummarys; }

    /// Class ID
    virtual const CLID& clID() const { return PackedMCRichDigitSummarys::classID(); }

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

    /// Data packing version (not used as yet, but for any future schema evolution)
    char   m_packingVersion;

    /// The packed data objects
    Vector m_vect; 

  };

  // -----------------------------------------------------------------------

  /** @class MCRichDigitSummaryPacker Event/PackedMCRichDigitSummary.h
   *
   *  Utility class to handle the packing and unpacking of the MCRichDigitSummarys
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class MCRichDigitSummaryPacker
  {
  public:

    // These are required by the templated algorithms
    typedef LHCb::MCRichDigitSummary                    Data;
    typedef LHCb::PackedMCRichDigitSummary        PackedData;
    typedef LHCb::MCRichDigitSummarys             DataVector;
    typedef LHCb::PackedMCRichDigitSummarys PackedDataVector;
    static const std::string& packedLocation()   { return LHCb::PackedMCRichDigitSummaryLocation::Default; }
    static const std::string& unpackedLocation() { return LHCb::MCRichDigitSummaryLocation::Default; }

  private:

    /// Default Constructor hidden
    MCRichDigitSummaryPacker() : m_parent(NULL) {}

  public:

    /// Default Constructor
    MCRichDigitSummaryPacker( GaudiAlgorithm & parent ) : m_parent(&parent) {}

  public:

    /// Pack MCRichDigitSummarys
    void pack( const DataVector & hits,
               PackedDataVector & phits ) const;

    /// Unpack MCRichDigitSummarys
    void unpack( const PackedDataVector & phits,
                 DataVector             & hits ) const;

    /// Compare two MCRichDigitSummarys to check the packing -> unpacking performance
    StatusCode check( const DataVector & dataA,
                      const DataVector & dataB ) const;

  private:

    /// Access the parent algorithm
    GaudiAlgorithm& parent() const { return *m_parent; }

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

    /// Pointer to parent algorithm
    GaudiAlgorithm * m_parent;

  };

  // -----------------------------------------------------------------------

}

#endif // EVENT_PackedMCRichDigitSummary_H
