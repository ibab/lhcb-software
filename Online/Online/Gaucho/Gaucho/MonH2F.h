#ifndef GAUCHO_MONH2F_H
#define GAUCHO_MONH2F_H 1

#include "Gaucho/MonH2D.h"

class MonH2F: public MonH2D {

public:

  MonH2F(IMessageSvc* msgSvc, const std::string& source, int version=0);

};
#endif //GAUCHO_MONH2F_H

