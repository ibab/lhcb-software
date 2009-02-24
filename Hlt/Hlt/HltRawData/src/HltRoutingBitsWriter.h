// $Id: HltRoutingBitsWriter.h,v 1.7 2009-02-24 13:50:27 graven Exp $
#ifndef HLTCOMMON_HLTROUTINGBITSWRITER_H 
#define HLTCOMMON_HLTROUTINGBITSWRITER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "boost/array.hpp"

#include "LoKi/HLTTypes.h"

/** @class HltRoutingBitsWriter HltRoutingBitsWriter.h
 *  
 *
 *  @author Gerhard Raven
 *  @date   2008-07-29
 */
class HltRoutingBitsWriter : public GaudiAlgorithm {
public: 
  /// Standard constructor
  HltRoutingBitsWriter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltRoutingBitsWriter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
private:
  StatusCode decode();

  enum { nBits = 3*sizeof(unsigned int)*8 };
  boost::array<LoKi::Types::HLT_Cut*,nBits> m_evaluators;
  void zeroEvaluators() { std::fill(m_evaluators.begin(),m_evaluators.end(),(LoKi::Types::HLT_Cut*)0); }
 
  std::string m_location;

  bool updateRequired() const { return m_bits_updated || m_preambulo_updated; }

  std::map<unsigned int,std::string> m_bits;
  bool m_bits_updated;
  void updateBits ( Property& );

  std::vector<std::string> m_preambulo_ ;             // the preambulo itself
  std::string m_preambulo ;                           // the preambulo itself
  bool m_preambulo_updated;
  void updatePreambulo ( Property& );
};
#endif // HLTROUTINGBITSWRITER_H
