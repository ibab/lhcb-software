#ifndef CONDITIONPARSER_H_
#define CONDITIONPARSER_H_

#include "DetDesc/ParamValidDataObject.h"

/** @function ConditionParser
 *  
 *  Wrapper around the parser to convert strings to condition objects.
 *  (written with boost::spirit, http://boost.org/libs/spirit/index.html).
 *
 *  @author Marco Clemencic
 *  @date   2006-10-23
 */
bool ConditionParser(const std::string &str, std::string &name, ParamValidDataObject &cond);

#endif /*CONDITIONPARSER_H_*/
