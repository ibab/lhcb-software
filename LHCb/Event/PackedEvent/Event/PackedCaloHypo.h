// $Id: PackedCaloHypo.h,v 1.1 2008-11-17 17:07:11 ocallot Exp $
#ifndef EVENT_PACKEDCALOHYPO_H 
#define EVENT_PACKEDCALOHYPO_H 1

// Include files
// Include files
#include "GaudiKernel/DataObject.h"
#include <string>
#include <vector>

namespace LHCb {
  
  /** @class PackedCaloHypo PackedCaloHypo.h Event/PackedCaloHypo.h
   *  Packed description of a CaloHypo
   *
   *  @author Olivier Callot
   *  @date   2008-11-10
   */
  struct PackedCaloHypo {

    PackedCaloHypo()   {};

    PackedCaloHypo( const PackedCaloHypo& c ) :  // copy constructor
      key( c.key ), hypothesis( c.hypothesis ), lh( c.lh ), z( c.z ),
      posX( c.posX ), posY( c.posY ), posE( c.posE ), 
      cov00( c.cov00 ), cov11( c.cov11 ), cov22( c.cov22 ), 
      cov10( c.cov10 ), cov20( c.cov20 ), cov21( c.cov21 ), cerr10( c.cerr10 ),
      centX( c.centX ), centY( c.centY ),
      cerr00( c.cerr00 ), cerr11( c.cerr11 ), 
      firstDigit( c.firstDigit ), lastDigit( c.lastDigit ),
      firstCluster( c.firstCluster ), lastCluster( c.lastCluster ),
      firstHypo( c.firstHypo ), lastHypo( c.lastHypo )
    {};
  
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

    int firstDigit;
    int lastDigit;
    int firstCluster;
    int lastCluster;
    int firstHypo;
    int lastHypo;
  };


  static const CLID CLID_PackedCaloHypos = 1551;

  // Namespace for locations in TDS
  namespace PackedCaloHypoLocation {
    static const std::string& Photons      = "pRec/Calo/Photons";
    static const std::string& Electrons    = "pRec/Calo/Electrons";
    static const std::string& MergedPi0s   = "pRec/Calo/MergedPi0s";
    static const std::string& SplitPhotons = "pRec/Calo/SplitPhotons";
  }

  class PackedCaloHypos : public DataObject {
  public: 
    /// Standard constructor
    PackedCaloHypos( ) { 
      m_vect.reserve(100);
      m_refs.reserve(1000);
    }; 

    virtual ~PackedCaloHypos( ) {}; ///< Destructor
    virtual const CLID& clID() const { return PackedCaloHypos::classID(); }
    static  const CLID& classID()    { return CLID_PackedCaloHypos;       }

    void addEntry( PackedCaloHypo& obj ) { m_vect.push_back( obj ); }
    std::vector<PackedCaloHypo>::const_iterator begin() const { return m_vect.begin(); }
    std::vector<PackedCaloHypo>::const_iterator end()   const { return m_vect.end(); }

    void addRef( int i ) { m_refs.push_back( i ); }
    std::vector<int>::const_iterator beginRefs() const { return m_refs.begin(); }
    unsigned int  sizeRef()                      const { return m_refs.size(); }
  protected:

  private:
    std::vector<PackedCaloHypo> m_vect;
    std::vector<int>       m_refs;
  };

}  // End of LHCb namespace
#endif // EVENT_PACKEDCALOHYPO_H
