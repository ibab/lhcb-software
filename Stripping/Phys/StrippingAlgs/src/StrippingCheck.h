// $Id: StrippingCheck.h,v 1.1 2009/10/13 13:12:35 poluekt Exp $
#ifndef STRIPPINGCHECK_H
#define STRIPPINGCHECK_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class StrippingCheck StrippingCheck.h
 *
 *
 *  @author Anton Poluektov
 *  @date   2009-10-12
 */
class StrippingCheck : public GaudiAlgorithm
{

public:

  /// Standard constructor
  StrippingCheck( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~StrippingCheck( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  unsigned int numberOfCandidates(const std::string& selalgo) const;

private:

  std::string m_inputLocation;

};

#endif // STRIPPINGCHECK_H
