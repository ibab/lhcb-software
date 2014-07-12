// #ifndef __GCCXML__
#ifndef  INDEXEDBITARRAY_H
#define  INDEXEDBITARRAY_H 1
#include <limits>
#include <algorithm>



template <size_t NStations, size_t NLayersPerStation, size_t NRegionsPerLayer, typename Storage = unsigned long > 
class IndexedBitArray 
#ifndef __GCCXML__
final 
#endif 
{

    static const size_t NBits = NStations*NLayersPerStation*NRegionsPerLayer;
    static const size_t NBitsPerWord = std::numeric_limits<Storage>::digits;
    static const size_t NWords = 1+(NBits-1)/NBitsPerWord;

    Storage m_data[NWords];

#ifndef __GCCXML__
    static constexpr size_t iw(size_t ibit) { return ibit / NBitsPerWord; }
    static constexpr size_t io(size_t ibit) { return ibit % NBitsPerWord; }
    static constexpr size_t jo(size_t ibit) { return NBitsPerWord - ibit % NBitsPerWord; }
    static constexpr Storage one  = 1;
    static constexpr Storage zero = 0;
    static constexpr Storage allset = ~zero;

    struct nbits_le_nbitsPerWord {};
    struct nbits_gt_nbitsPerWord {};

    template <size_t nbits>
    bool all_n_( size_t first, nbits_le_nbitsPerWord ) const {
        // nbits<=NBitsPerWord -- so we span at most two words...
        auto bw = iw(first);
        auto ew = iw(first+nbits);

        auto m1 = allset << io(first);
        auto m2 = allset >> jo(first+nbits);
        return ( NWords==1 || bw == ew ) ? ( ( m_data[bw] & m1 & m2 ) == ( m1 & m2 ) ) 
                                         : ( ( m_data[bw] & m1 ) == m1 && ( m_data[ew] & m2 ) == m2 );
    }
    template <size_t nbits>
    bool all_n_( size_t first, nbits_gt_nbitsPerWord ) const {
        // nbits > NBitsPerWord -- so we span at least two words...
        auto bw = iw(first);
        auto ew = iw(first+nbits);

        auto m1 = allset << io(first);
        auto m2 = allset >> jo(first+nbits);

        return ( ( m_data[bw] & m1 ) == m1 )
            && ( NWords == 2 || std::all_of( std::next(std::begin(m_data),bw+1), 
                                             std::next(std::begin(m_data),ew), 
                                             [](const Storage& w) { return w == allset; } ) ) 
            && ( ( m_data[ew] & m2 ) == m2 );
    }
    template <size_t nbits>
    bool all_n( size_t first ) const { return nbits <= NBitsPerWord ? all_n_<nbits>( first, nbits_le_nbitsPerWord{} ) 
                                                                    : all_n_<nbits>( first, nbits_gt_nbitsPerWord{} ); };
#endif
public:

    IndexedBitArray() {
        clear();
    }

#ifdef __GCCXML__
    void set( size_t station, size_t layer, size_t region ) ;
    void clear();
    bool test( size_t station, size_t layer, size_t region ) const;
    bool all( size_t station, size_t layer ) const;
    bool all( size_t station  ) const ;
    bool all( ) const ;
#else
    void set( size_t station, size_t layer, size_t region ) {
        auto b = region + NRegionsPerLayer*(layer + NLayersPerStation*station);
        m_data[ iw(b) ] |=  one << io(b) ;
    }
    void clear() {
        std::fill( std::begin(m_data), std::end(m_data), zero);
    }
    bool test( size_t station, size_t layer, size_t region ) const {
        auto b = region + NRegionsPerLayer*(layer + NLayersPerStation*station);
        return m_data[iw(b)] & ( one << io(b) ) ;
    }
    bool all( size_t station, size_t layer ) const {
        return all_n<NRegionsPerLayer>( NRegionsPerLayer*(layer + NLayersPerStation*station) );
    }
    bool all( size_t station  ) const {
        return all_n<NRegionsPerLayer*NLayersPerStation>( NRegionsPerLayer*NLayersPerStation*station );
    }
    bool all( ) const {
        return all_n<NBits>( 0 );
    }
#endif

};
#endif
