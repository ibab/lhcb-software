#ifndef DICT_MISMATCHESDICT_H
#define DICT_MISMATCHESDICT_H 1

#include <vector>
#include <Independence/DecisionMap.h>

template class std::pair<unsigned int, unsigned int>;
template class std::vector<std::pair<unsigned int, unsigned int>,
                           std::allocator< std::pair<unsigned int, unsigned int> > >;

#endif // DICT_MISMATCHESDICT_H
