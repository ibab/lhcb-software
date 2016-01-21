// $Id: PackedFlavourTag.h,v 1.2 2010-05-19 09:04:08 jonrob Exp $
#ifndef EVENT_PackedFlavourTag_H
#define EVENT_PackedFlavourTag_H 1

#include <string>

// Kernel
#include "Event/StandardPacker.h"

// Event
#include "Event/FlavourTag.h"

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/StatusCode.h"

namespace LHCb
{

  // ----------------------------------------------------------------------------------------

  /** @struct PackedFlavourTag Event/PackedFlavourTag.h
   *
   *  Packed FlavourTag
   *
   *  @author Christopher Rob Jones
   *  @date   2013-05-03
   */
  struct PackedFlavourTag
  {
    long long key{0};        ///< reference to the original container + key

    short int decision{0};   ///< The result of the tagging algorithm
    short int omega{0};      ///< Wrong tag fraction (predicted)

    short int decisionOS{0}; ///< decision of opposite side taggers only
    short int omegaOS{0};    ///< Wrong tag fraction (predicted) using opposite side only

    long long taggedB{-1};   ///< The B for which this tag has been made

    // Taggers
    unsigned int firstTagger{0}, lastTagger{0};
  };

  // ----------------------------------------------------------------------------------------

  /** @struct PackedTagger Event/PackedFlavourTag.h
   *
   *  Packed Flavour Tagger
   *
   *  @author Christopher Rob Jones
   *  @date   2013-05-03
   */
  struct PackedTagger
  {
    unsigned short int type{0}; ///< The type of tagger
    short int      decision{0}; ///< Decision of tagger
    short int         omega{0}; ///< Wrong tag fraction of tagger from MC tuning
    // Tagging Particles
    unsigned int firstTagP{0}, lastTagP{0};
  };

  // ----------------------------------------------------------------------------------------

  static const CLID CLID_PackedFlavourTags = 1583;

  /// Namespace for locations in TDS
  namespace PackedFlavourTagLocation
  {
    static const std::string& Default  = "pPhys/FlavourTags";
    static const std::string& InStream = "/pPhys/FlavourTags";
  }

  /** @class PackedFlavourTags Event/PackedFlavourTag.h
   *
   *  Packed FlavourTags
   *
   *  @author Christopher Rob Jones
   *  @date   2013-05-03
   */
  class PackedFlavourTags : public DataObject
  {

  public:

    /// Vector of packed Flavour Tags
    typedef std::vector<LHCb::PackedFlavourTag> Vector;

    /// Vector of packed Taggers
    typedef std::vector<LHCb::PackedTagger> Taggers;

    /// Tagging Particles
    typedef std::vector<long long> TaggingParticles;

  public:
    
    /// Default Packing Version
    static char defaultPackingVersion() { return 0; }

  public:

    /// Class ID
    static const CLID& classID() { return CLID_PackedFlavourTags; }

    /// Class ID
    virtual const CLID& clID() const { return PackedFlavourTags::classID(); }

  public:

    /// Write access to the data vector
    Vector & data()             { return m_vect; }

    /// Read access to the data vector
    const Vector & data() const { return m_vect; }

    /// Set the packing version
    void setPackingVersion( const char ver ) { m_packingVersion = ver; }

    /// Access the packing version
    char packingVersion() const { return m_packingVersion; }

    /// Const access to taggers
    const Taggers& taggers() const { return m_taggers; }
    /// Access to taggers
    Taggers& taggers()             { return m_taggers; }

    /// Const access to tagging Particles
    const TaggingParticles& taggeringPs() const { return m_taggingPs; }
    /// Access to taggers
    TaggingParticles& taggeringPs()             { return m_taggingPs; }

  private:

    /// Data packing version (not used as yet, but for any future schema evolution)
    char m_packingVersion{ defaultPackingVersion() };

    /// The packed flavour tags
    Vector m_vect;

    /// Packed Taggers
    Taggers m_taggers;

    /// Tagging Particles
    TaggingParticles m_taggingPs;

  };

  // -----------------------------------------------------------------------

  /** @class FlavourTagPacker Event/PackedFlavourTag.h
   *
   *  Utility class to handle the packing and unpacking of the FlavourTags
   *
   *  @author Christopher Rob Jones
   *  @date   2009-10-13
   */
  class FlavourTagPacker
  {
  public:

    // These are required by the templated algorithms
    typedef LHCb::FlavourTag                    Data;
    typedef LHCb::PackedFlavourTag        PackedData;
    typedef LHCb::FlavourTags             DataVector;
    typedef LHCb::PackedFlavourTags PackedDataVector;
    static const std::string& packedLocation()   { return LHCb::PackedFlavourTagLocation::Default; }
    static const std::string& unpackedLocation() { return LHCb::FlavourTagLocation::Default; }

  private:

    /// Default Constructor hidden
    FlavourTagPacker() {}

  public:

    /// Default Constructor
    FlavourTagPacker( const GaudiAlgorithm & p ) : m_pack(&p) {}

  public:

    /// Pack a single FlavourTag
    void pack( const Data & ft,
               PackedData & pft,
               PackedDataVector & pfts ) const;

    /// Pack FlavourTags
    void pack( const DataVector & fts,
               PackedDataVector & pfts ) const;

    /// Unpack a single FlavourTag
    void unpack( const PackedData       & pft,
                 Data                   & ft,
                 const PackedDataVector & pfts,
                 DataVector             & fts ) const;

    /// Unpack FlavourTags
    void unpack( const PackedDataVector & pfts,
                 DataVector             & fts ) const;

    /// Compare two FlavourTag vectors to check the packing -> unpacking performance
    StatusCode check( const DataVector & dataA,
                      const DataVector & dataB ) const;

    /// Compare two FlavourTags to check the packing -> unpacking performance
    StatusCode check( const Data & dataA,
                      const Data & dataB ) const;

  private:

    /// Access the parent algorithm
    const GaudiAlgorithm& parent() const { return *(m_pack.parent()); }

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

  };

  // -----------------------------------------------------------------------

}

#endif // EVENT_PackedFlavourTag_H
