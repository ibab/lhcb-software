// $Id: TupleToolCalohypo.h,v 1.1 2010-05-21 17:03:11 odescham Exp $
#ifndef _TUPLETOOLECALOHYPO_H
#define _TUPLETOOLECALOHYPO_H 1

// Include files
// from Gaudi
#include "DecayTreeTupleBase/TupleToolBase.h"
// Interface
#include "Kernel/IParticleTupleTool.h"

#include "CaloInterfaces/ICaloHypoEstimator.h"

//============================================================================
class TupleToolCaloHypo : public TupleToolBase, virtual public IParticleTupleTool {
  //==========================================================================
public:
  // Standard constructor
  TupleToolCaloHypo( const std::string& type,
                     const std::string& name,
                     const IInterface* parent);

  virtual StatusCode initialize();
  virtual ~TupleToolCaloHypo(){};
  virtual StatusCode fill(const LHCb::Particle*, const LHCb::Particle*
                          ,  const std::string& ,Tuples::Tuple& );
private:
  std::vector<std::string> m_dataList;
  bool useData( int id , int mask = 0x3);
  ICaloHypoEstimator* m_estimator;
  bool m_brem;
  bool m_charged;
};
#endif // _TUPLETOOLECALOHYPO_H
