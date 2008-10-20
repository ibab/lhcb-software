// $Id: DistanceCalculatorNames.h,v 1.1 2008-10-20 14:53:34 jpalac Exp $
#ifndef DISTANCECALCULATORNAMES_H 
#define DISTANCECALCULATORNAMES_H 1

// Include files
#include <string>
/** @file DistanceCalculatorNames DistanceCalculatorNames.h
 *  
 * Hold strings with names of IDistanceCalculator implementaiton. For use in
 * specializations of GenericParticle2PVRelator template implementation of
 * IRelatedPVFinder. These structs must satisfy the policy
 *
 * T::value;
 *
 * returning an std::string with the name of an IRelatedPVFinder 
 * implementaiton.
 *
 *  @author Juan PALACIOS
 *  @date   2008-10-16
 */
struct OnlineDistanceCalculatorName
{
  const static std::string value;
};
struct OfflineDistanceCalculatorName
{
  const static std::string value;
};
#endif // DISTANCECALCULATORNAMES_H
