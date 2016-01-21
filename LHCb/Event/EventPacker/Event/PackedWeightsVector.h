// $Id: PackedWeightsVector.h,v 1.5 2010-03-09 18:35:20 jonrob Exp $
#ifndef EVENT_PackedWeightsVector_H
#define EVENT_PackedWeightsVector_H 1

#include <string>

// Kernel
#include "Event/StandardPacker.h"

// Event
#include "Event/WeightsVector.h"

// Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/StatusCode.h"

namespace LHCb
{
  // -----------------------------------------------------------------------

  /** @struct PackedWeight Event/PackedWeightsVector.h
   *
   *  Packed int to weight
   *
   *  @author Christopher Rob Jones
   *  @date   2010-09-22
   */
  struct PackedWeight
  {
    /// Default constructor
    PackedWeight() = default;

    /// Constructor from values
    PackedWeight( const int       _key , 
                  const short int _weight )
      : key(_key), weight(_weight)
    {}

    int key{0};           ///< key of the track this weight is associated with
    short int weight{0};  ///< Weight of this track in the vertex
  };

  /** @struct PackedWeights Event/PackedWeightsVector.h
   *
   *  Packed vector of Weights
   *
   *  @author Christopher Rob Jones
   *  @date   2010-09-22
   */
  struct PackedWeights
  {
    /// Default constructor
    PackedWeights() 
      : firstWeight(0), lastWeight(0), pvKey(0)
    {}

    unsigned short int firstWeight;  ///< index to first weight
    unsigned short int lastWeight;   ///< index to last weight
    unsigned int pvKey;              ///< The PV Key
  };

  // -----------------------------------------------------------------------

  static const CLID CLID_PackedWeightsVector = 1555;

  /// Namespace for locations in TDS
  namespace PackedWeightsVectorLocation
  {
    static const std::string& Default = "pRec/Vertex/Weights";
  }

  /** @class PackedWeightsVectors Event/PackedWeightsVector.h
   *
   *  Packed WeightsVectors
   *
   *  @author Christopher Rob Jones
   *  @date   2010-09-22
   */
  class PackedWeightsVector : public DataObject
  {

  public:

    /// Vector of packed objects
    typedef std::vector<LHCb::PackedWeights> WeightsVector;

    /// Vector of raw weights
    typedef std::vector<LHCb::PackedWeight> WeightVector;

  public:
    
    /// Default Packing Version
    static char defaultPackingVersion() { return 0; }

  public:

    /// Standard constructor
    PackedWeightsVector( ) : m_packingVersion(defaultPackingVersion()) { }

    /// Destructor
    virtual ~PackedWeightsVector( ) { }

    /// Class ID
    static const CLID& classID() { return CLID_PackedWeightsVector; }

    /// Class ID
    virtual const CLID& clID() const { return PackedWeightsVector::classID(); }

  public:

    /// Write access to the data vector
    WeightsVector & data()             { return m_weights; }

    /// Read access to the data vector
    const WeightsVector & data() const { return m_weights; }

    /// Write access to the data vector
    WeightVector & weights()             { return m_weight; }

    /// Read access to the data vector
    const WeightVector & weights() const { return m_weight; }

    /// Set the packing version
    void setPackingVersion( const char ver ) { m_packingVersion = ver; }

    /// Access the packing version
    char packingVersion() const { return m_packingVersion; }

  private:

    /// Data packing version (not used as yet, but for any future schema evolution)
    char m_packingVersion;

    /// The packed data objects
    WeightsVector m_weights; 
    
    /// Vector of all weights
    WeightVector m_weight; 

  };

  // -----------------------------------------------------------------------

  /** @class WeightsVectorPacker Event/PackedWeightsVector.h
   *
   *  Utility class to handle the packing and unpacking of the WeightsVectors
   *
   *  @author Christopher Rob Jones
   *  @date   2010-09-22
   */
  class WeightsVectorPacker
  {
  public:

    // These are required by the templated algorithms
    typedef LHCb::WeightsVector              Data;
    typedef LHCb::PackedWeights              PackedData;
    typedef LHCb::WeightsVectors             DataVector;
    typedef LHCb::PackedWeightsVector        PackedDataVector;
    static const std::string& packedLocation()   { return LHCb::PackedWeightsVectorLocation::Default; }
    static const std::string& unpackedLocation() { return LHCb::WeightsVectorLocation::Default; }

  private:

    /// Default Constructor hidden
    WeightsVectorPacker() {}

  public:

    /// Default Constructor
    WeightsVectorPacker( const GaudiAlgorithm & parent ) : m_pack(&parent) {}

  public:

    /// Pack Data
    void pack( const DataVector & weightsV,
               PackedDataVector & pweightsV ) const;

    /// Unpack Data
    void unpack( const PackedDataVector & pweightsV,
                 DataVector             & weightsV ) const;

    /// Compare two data vectors to check the packing -> unpacking performance
    StatusCode check( const DataVector & dataA,
                      const DataVector & dataB ) const;

  private:

    /// Access the parent algorithm
    const GaudiAlgorithm& parent() const { return *(m_pack.parent()); }

  private:

    /// Standard packing of quantities into integers ...
    StandardPacker m_pack;

  };

  // -----------------------------------------------------------------------

}

#endif // EVENT_PackedWeightsVector_H
