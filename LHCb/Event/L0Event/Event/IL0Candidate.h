// $Id: IL0Candidate.h,v 1.1 2006-02-21 16:00:34 ocallot Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2006/02/20 09:38:46  cattanem
// hide interface, not needed outside this package
//
// Revision 1.1  2006/02/18 11:29:36  ocallot
// Completely new implementation
//
//
//
// ============================================================================
#ifndef L0DU_IL0CANDIDATE_H
#define L0DU_IL0CANDIDATE_H

// STD & STL 
#include <string>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// Local
#include "Event/L0ProcessorData.h"

static const InterfaceID IID_L0Candidate( "IL0Candidate", 1, 0 );



/** @class IL0Candidate IL0Candidate.h
 * 
 * Generic interface for tools decoding the container of L0Processor data
 *
 * Candidate : src/L0CandidateTool
 *
 */

class IL0Candidate : public virtual IAlgTool {

 public:

  static const InterfaceID& interfaceID() { return IID_L0Candidate; }

    
  /** The main methods
   *
   */
  virtual void setUp(const std::string& dataLocation)                         = 0;
  virtual void setUp(const std::string& dataLocation, bool convert)           = 0;
  virtual unsigned int digitValue( const unsigned int data[L0DUBase::Index::Size] ) = 0;
  virtual double scale(  const unsigned int  data[L0DUBase::Index::Size] ) = 0;
  virtual double value(  const unsigned int  data[L0DUBase::Index::Size] ) = 0;
  
};
#endif // L0DU_IL0CANDIDATE_H 
