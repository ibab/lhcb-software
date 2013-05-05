// $Id: FlavourTagCloner.h,v 1.3 2009-07-30 10:03:01 jpalac Exp $
#ifndef FLAVOURTAGCLONER_H
#define FLAVOURTAGCLONER_H 1

#include "ObjectClonerBase.h"

#include "MicroDST/ICloneFlavourTag.h"

namespace LHCb
{
  class FlavourTag;
}

/** @class FlavourTagCloner FlavourTagCloner.h
 *
 *  Clones flavour tagging objects
 *
 *  @author Juan PALACIOS
 *  @date   2008-08-08
 */
class FlavourTagCloner : public extends1<ObjectClonerBase,ICloneFlavourTag>
{

public:

  /// Standard constructor
  FlavourTagCloner( const std::string& type,
                    const std::string& name,
                    const IInterface* parent );

  virtual ~FlavourTagCloner( ); ///< Destructor

  virtual LHCb::FlavourTag* operator() (const LHCb::FlavourTag* tag);

private:

  typedef MicroDST::BasicCopy<LHCb::FlavourTag> BasicFTCopy;

  LHCb::FlavourTag* clone(const LHCb::FlavourTag* tag);

};

#endif // FLAVOURTAGCLONER_H
