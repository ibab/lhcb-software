// $Id: TupleToolBase.h,v 1.2 2010-02-04 10:09:35 rlambert Exp $
#ifndef DV_TUPLETOOLBASE_H
#define DV_TUPLETOOLBASE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleTool.h"

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
  
  const std::string& extraName() {return m_extraName;};
  bool isVerbose() {return m_verbose;};
  std::string fullName(const std::string head="") 
  {
    if (m_extraName=="") return head;
    else if(head=="") return (m_extraName+'_');
    else return (head+'_'+m_extraName);
  }
  
  
protected:
  
  //DVAlgorithm* m_dva;
  std::string m_extraName; ///<Extra title to add to head
  bool m_verbose;///<Add extra items to the tuple
  
private:
  
  
};
#endif // DV_TUPLETOOLBASE
