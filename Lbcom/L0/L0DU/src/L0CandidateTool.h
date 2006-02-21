// $Id: L0CandidateTool.h,v 1.3 2006-02-21 15:56:49 ocallot Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
//
//
// ============================================================================
#ifndef L0DU_L0CANDIDATETOOL_H
#define L0DU_L0CANDIDATETOOL_H

// from GaudiKernel
#include "GaudiAlg/GaudiTool.h"
// Event
#include  "Event/IL0Candidate.h"

class L0CandidateTool : public GaudiTool, virtual public IL0Candidate {
 public:
  
  L0CandidateTool ( const std::string& type   , 
                   const std::string& name   ,
                   const IInterface*  parent ) ;

  virtual ~L0CandidateTool () ;
  
  virtual StatusCode initialize () ;

  void setUp(const std::string& dataLocation);
  void setUp(const std::string& dataLocation, bool convert);
  unsigned int digitValue(  const unsigned int data[L0DUBase::Index::Size] );
  double scale( const unsigned int   data[L0DUBase::Index::Size] );
  double value( const unsigned int   data[L0DUBase::Index::Size] );

private:
  std::string       m_dataLocation;
  bool              m_adcConvert;
  LHCb::L0ProcessorDatas* m_dataContainer;
  double m_caloEtScale;
  double m_muonPtScale;
  
};

#endif // L0DU_L0CANDIDATETOOL_H 
