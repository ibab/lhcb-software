// $Id: PackedCaloHypo.h,v 1.4 2009-11-07 12:20:26 jonrob Exp $
#ifndef EVENT_PACKEDCALOHYPO_H
#define EVENT_PACKEDCALOHYPO_H 1

// Include files
// Include files
#include "GaudiKernel/DataObject.h"
#include <string>
#include <vector>

namespace LHCb {

  /** @struct PackedCaloHypo Event/PackedCaloHypo.h
   *  Packed description of a CaloHypo
   *
   *  @author Olivier Callot
   *  @date   2008-11-10
   */
  struct PackedCaloHypo
  {

    /// Default Constructor
    PackedCaloHypo() :
      key(0), hypothesis(0), lh(0), z(0),
      posX(0), posY(0), posE(0),
      cov00(0), cov11(0), cov22(0),
      cov10(0), cov20(0), cov21(0), cerr10(0),
      centX(0), centY(0),
      cerr00(0), cerr11(0),
      firstDigit(0), lastDigit(0),
      firstCluster(0), lastCluster(0),
      firstHypo(0), lastHypo(0)
    {}

    /// copy constructor
    PackedCaloHypo( const PackedCaloHypo& c ) :
      key( c.key ), hypothesis( c.hypothesis ), lh( c.lh ), z( c.z ),
      posX( c.posX ), posY( c.posY ), posE( c.posE ),
      cov00( c.cov00 ), cov11( c.cov11 ), cov22( c.cov22 ),
      cov10( c.cov10 ), cov20( c.cov20 ), cov21( c.cov21 ), cerr10( c.cerr10 ),
      centX( c.centX ), centY( c.centY ),
      cerr00( c.cerr00 ), cerr11( c.cerr11 ),
      firstDigit( c.firstDigit ), lastDigit( c.lastDigit ),
      firstCluster( c.firstCluster ), lastCluster( c.lastCluster ),
      firstHypo( c.firstHypo ), lastHypo( c.lastHypo )
    {}

    int key;
    int hypothesis;
    int lh;
    // from CaloPosition
    int z;
    // position (3 ) + 3x3 symetric covariance matrix
    int posX;
    int posY;
    int posE;

    int cov00;
    int cov11;
    int cov22;
    short int cov10;
    short int cov20;
    short int cov21;
    short int cerr10;    // non diagonal terms of the x,y spread matrix.
    // center in x,y + 2x2 symetric covariance matrix
    int centX;
    int centY;

    int cerr00;
    int cerr11;

    unsigned short int firstDigit;
    unsigned short int lastDigit;
    unsigned short int firstCluster;
    unsigned short int lastCluster;
    unsigned short int firstHypo;
    unsigned short int lastHypo;
  };


  static const CLID CLID_PackedCaloHypos = 1551;

  // Namespace for locations in TDS
  namespace PackedCaloHypoLocation
  {
    static const std::string& Photons      = "pRec/Calo/Photons";
    static const std::string& Electrons    = "pRec/Calo/Electrons";
    static const std::string& MergedPi0s   = "pRec/Calo/MergedPi0s";
    static const std::string& SplitPhotons = "pRec/Calo/SplitPhotons";
  }

  /** @class PackedCaloHypos Event/PackedCaloHypo.h
   *  Vector of packed CaloHypos
   *
   *  @author Olivier Callot
   *  @date   2008-11-10
   */

  class PackedCaloHypos : public DataObject
  {

  public:

    /// Standard constructor
    PackedCaloHypos( )
    {
      m_vect.reserve(100);
      m_refs.reserve(1000);
    }

    virtual ~PackedCaloHypos( ) {}; ///< Destructor
    virtual const CLID& clID()  const { return PackedCaloHypos::classID(); }
    static  const CLID& classID()     { return CLID_PackedCaloHypos;       }

    void addEntry( PackedCaloHypo& obj ) { m_vect.push_back( obj ); }
    std::vector<PackedCaloHypo>::const_iterator begin() const { return m_vect.begin(); }
    std::vector<PackedCaloHypo>::const_iterator end()   const { return m_vect.end(); }
    std::vector<PackedCaloHypo>& hypos()                      { return m_vect; }
    const std::vector<PackedCaloHypo>& hypos() const          { return m_vect; }

    void addRef( int i ) { m_refs.push_back( i ); }
    /// Avoid hidden method
    virtual unsigned long addRef() { return DataObject::addRef(); }
    std::vector<int>::const_iterator beginRefs() const { return m_refs.begin(); }
    unsigned int  sizeRef()                      const { return m_refs.size(); }
    std::vector<int>& refs()                           { return m_refs; }
    const std::vector<int>& refs() const               { return m_refs; }

  private:

    std::vector<PackedCaloHypo> m_vect;
    std::vector<int>       m_refs;

  };

}  // End of LHCb namespace

#endif // EVENT_PACKEDCALOHYPO_H
