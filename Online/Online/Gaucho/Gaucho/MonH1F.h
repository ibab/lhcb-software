#ifndef GAUCHO_MONH1F_H
#define GAUCHO_MONH1F_H 1

#include "Gaucho/MonH1D.h"

class MonH1F: public MonH1D {

public:

  MonH1F(IMessageSvc* msgSvc, const std::string& source, int version=0);

};

#endif //GAUCHO_MONH1F_H
