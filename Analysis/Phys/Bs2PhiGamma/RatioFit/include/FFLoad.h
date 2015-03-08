#ifndef FFLOAD_H
#define FFLOAD_H

#include <string>
#include "TChain.h"
#include "TCut.h"
#include "TH1F.h"
#include "TLeaf.h"
//#include "TWeighter2D.h"
#include "FFAux.h"

class FFLoad
{
    public:
        static TH1F get(const std::string& command, const std::vector<double>& binwalls);
    private:
};

#endif // FFLOAD_H
