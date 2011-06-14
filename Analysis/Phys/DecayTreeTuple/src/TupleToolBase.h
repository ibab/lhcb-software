// $Id: TupleToolBase.h,v 1.2 2010-02-04 10:09:35 rlambert Exp $
#ifndef DV_TUPLETOOLBASE_H
#define DV_TUPLETOOLBASE_H 1


// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleTool.h"
#include "CaloUtils/CaloParticle.h"

//namespace LHCb {
//  class Particle;
//  class VertexBase;
//};

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
class TupleToolBase : public GaudiTupleTool {
public:
  /// Standard constructor
  TupleToolBase( const std::string& type,
                 const std::string& name,
                 const IInterface* parent) :
    GaudiTupleTool ( type, name , parent )
    , m_extraName("")
    , m_verbose(0)
  {
    
    declareProperty("ExtraName",m_extraName="",
                    "prepend the name of any variable with this string");
    declareProperty("Verbose",m_verbose=false,
                    "add extra variables for this tool");
  }
  
  
  virtual ~TupleToolBase( ){}; ///< Destructor
  
  const std::string& extraName()const {return m_extraName;};
  bool isVerbose()const {return m_verbose;} ;
  std::string fullName(const std::string head="") const 
  {
    if (m_extraName=="") return head;
    else if(head=="") return (m_extraName+'_');
    else return (head+'_'+m_extraName);
  }
  
  /// generic templated method to check if a container exists without loading the data on demand service
  /// (which exist<> would do)
  /// @author Ivan.Belyaev@nikhef.nl
  template <class TYPE>
  bool safeExist  ( const std::string& location ) 
  {
    DataObject* obj = 0 ;
    StatusCode sc = evtSvc()->findObject( location , obj ) ;
    return sc.isSuccess() && 0 != obj && 0 != dynamic_cast<TYPE*>( obj ) ;
  }
  /// generic templated method to extract the number of entries in a given location.
  /// usage int n = number<LHCb::Particles>('/Event/Phys/MyParts/Particles')
  template<class CLASS> 
  int number( const std::string& location){
    if (safeExist<CLASS>(location)){
      return (get<CLASS>(location))->size() ;
    } else return -1 ;
  }
  
protected:
  
  //DVAlgorithm* m_dva;
  std::string m_extraName; ///<Extra title to add to head
  bool m_verbose;///<Add extra items to the tuple
  bool isPureNeutralCalo(const LHCb::Particle* P)const{
    LHCb::CaloParticle caloP(  (LHCb::Particle*) P );
    return caloP.isPureNeutralCalo();
  }
  
private:
  
  
};
#endif // DV_TUPLETOOLBASE
