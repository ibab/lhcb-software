// $Id: TupleToolBase.h,v 1.2 2010-02-04 10:09:35 rlambert Exp $
#ifndef DV_TUPLETOOLBASE_H
#define DV_TUPLETOOLBASE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleTool.h"
#include "CaloUtils/CaloParticle.h"

/** @class TupleToolBase TupleToolBase.h jborel/TupleToolBase.h
 *
 * \brief Shared functions/options for all TupleTools
 *
 *
 * \sa DecayTreeTuple
 *
 *  @author Rob Lambert
 *  @date   2010-01-19
 */
class TupleToolBase : public GaudiTupleTool
{

public:

  /// Standard constructor
  TupleToolBase( const std::string& type,
                 const std::string& name,
                 const IInterface* parent) :
    GaudiTupleTool ( type, name , parent )
  {
    declareProperty("ExtraName",m_extraName="",
                    "prepend the name of any variable with this string");
    declareProperty("Verbose",m_verbose=false,
                    "add extra variables for this tool");
    declareProperty("MaxPV", m_maxPV = 100  , "Maximal number of PVs considered");
  }

  virtual ~TupleToolBase( ) { } ///< Destructor

protected:

  inline const std::string& extraName() const { return m_extraName; }
  inline bool isVerbose()               const { return m_verbose;   }

  /// Returns the full name for the tuple variables
  inline std::string fullName( const std::string & head ="" ) const
  {
    return ( m_extraName.empty() ? head :
             head.empty()        ? m_extraName+'_' :
             head+'_'+m_extraName );
  }

  /// generic templated method to check if a container exists without loading the data on demand service
  /// (which exist<> would do)
  /// @author Ivan.Belyaev@nikhef.nl
  template <class TYPE>
  inline bool safeExist ( const std::string& location )
  {
    DataObject* obj = NULL ;
    StatusCode sc = evtSvc()->findObject( location , obj ) ;
    return sc.isSuccess() && obj && dynamic_cast<TYPE*>( obj ) ;
  }

  /// generic templated method to extract the number of entries in a given location.
  /// usage int n = number<LHCb::Particles>('/Event/Phys/MyParts/Particles')
  template<class CLASS>
  inline int number( const std::string& location)
  {
    return ( safeExist<CLASS>(location) ? (int)(get<CLASS>(location))->size() : -1 );
  }

  /// Get the TES location of an object.
  const std::string objectLocation(const DataObject* pObject) const
  {
    return (!pObject ? "Null DataObject" :
            (pObject->registry() ? pObject->registry()->identifier() : "UnRegistered"));
  }

protected:

  /// Check if a pure CALO Particle
  inline bool isPureNeutralCalo(const LHCb::Particle* P)const
  {
    LHCb::CaloParticle caloP(  (LHCb::Particle*) P );
    return caloP.isPureNeutralCalo();
  }

protected:

  std::string m_extraName; ///<Extra title to add to head
  bool m_verbose;///<Add extra items to the tuple
  unsigned int m_maxPV ; ///< max number of PVs (constant)

};

#endif // DV_TUPLETOOLBASE
