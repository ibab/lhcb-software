#ifndef ONLINE_GAUCHO_COUNTERADDER_H
#define ONLINE_GAUCHO_COUNTERADDER_H

#include "Gaucho/MonAdder.h"

class CounterAdder : public MonAdder
{
public:
    void add(void *buffer, int siz, MonInfo *h);
    CounterAdder(const char *myName, const char * serviceName);
    virtual ~CounterAdder();
};
#endif //  ONLINE_GAUCHO_COUNTERADDER_H
