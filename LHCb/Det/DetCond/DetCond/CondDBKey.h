//$Id: CondDBKey.h,v 1.1 2001-12-17 21:22:46 andreav Exp $
#ifndef DETCOND_CONDDBKEY_H
#define DETCOND_CONDDBKEY_H 1

// Type definition
// For Windows, this hack allows at least to compile the ConditionsDBCnvSvc
#ifdef __linux__
#include "ConditionsDB/CondDBKey.h"
#else
typedef longlong CondDBKey;
#endif

#endif // DETCOND_CONDDBKEY_H

