// $Id: TupleToolTagging.h,v 1.2 2008-01-07 17:25:25 pkoppenb Exp $
#ifndef JBOREL_TUPLETOOLTAGGING_H
#define JBOREL_TUPLETOOLTAGGING_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticleTupleTool.h"            // Interface

class IContextTool;
class IBTaggingTool;

/** @class TupleToolTagging TupleToolTagging.h jborel/TupleToolTagging.h
 *
 * \brief Fill the tagging information in the tuple.
 *
 * 
 * @author Jeremie Borel
 * @date   2007-11-07
 */
class TupleToolTagging : public GaudiTool, virtual public IParticleTupleTool {
public:
  /// Standard constructor
  TupleToolTagging( const std::string& type,
		     const std::string& name,
		     const IInterface* parent);

  virtual ~TupleToolTagging(){}; ///< Destructor

  virtual StatusCode initialize();
  
  virtual StatusCode fill( const LHCb::Particle*, const LHCb::Particle*
			   , const std::string&, Tuples::Tuple& );


private:
  const DVAlgorithm* getParent() const;

  std::string m_inputLocation, m_toolName;
  IContextTool* m_context;
  IBTaggingTool* m_tagging;
  
  //  bool m_extendedTagging;
};

#endif // JBOREL_TUPLETOOLTAGGING_H
