#include "Event/NamedEntry.h"
#include <ostream>

namespace Hlt
{

std::ostream& operator<<( std::ostream& os, const NamedEntry& e )
{
    return os << e.str();
}

}
