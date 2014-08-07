// #ifndef __GCCXML__
#ifndef  INDEXEDBITARRAY_H
#define  INDEXEDBITARRAY_H 1
#include <limits>
#include <algorithm>



template <int NStations, int NLayersPerStation, int NRegionsPerLayer, typename Storage = unsigned long > 
class IndexedBitArray 
#ifndef __GCCXML__
final 
#endif 
{

    static const int NBits = NStations*NLayersPerStation*NRegionsPerLayer;
    static const int NBitsPerWord = std::numeric_limits<Storage>::digits;
    static const int NWords = 1+(NBits-1)/NBitsPerWord;

    Storage m_data[NWords];

#ifndef __GCCXML__
    static constexpr int iw(int ibit) { return ibit / NBitsPerWord; }
    static constexpr int io(int ibit) { return ibit % NBitsPerWord; }
    static constexpr int jo(int ibit) { return NBitsPerWord - ibit % NBitsPerWord; }
    static constexpr Storage ib(int ibit) { return Storage(1) << io(ibit) ; }
    static constexpr Storage allset = ~Storage(0);

    struct nbits_le_nbitsPerWord {};
    struct nbits_gt_nbitsPerWord {};

    template <int nbits>
    bool all_n_( int first, nbits_le_nbitsPerWord ) const {
        // nbits<=NBitsPerWord -- so we span at most two words...
        auto bw = iw(first);
        auto ew = iw(first+nbits);

        auto m1 = allset << io(first);
        auto m2 = allset >> jo(first+nbits);
        return ( NWords==1 || bw == ew ) ? ( ( m_data[bw] & m1 & m2 ) == ( m1 & m2 ) ) 
                                         : ( ( m_data[bw] & m1 ) == m1 && ( m_data[ew] & m2 ) == m2 );
    }
    template <int nbits>
    bool all_n_( int first, nbits_gt_nbitsPerWord ) const {
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
    template <int nbits>
    bool all_n( int first ) const { return nbits <= NBitsPerWord ? all_n_<nbits>( first, nbits_le_nbitsPerWord{} ) 
                                                                 : all_n_<nbits>( first, nbits_gt_nbitsPerWord{} ); }
#endif
public:

    IndexedBitArray() {
        clear();
    }

#ifdef __GCCXML__
    void set( int station, int layer, int region ) ;
    void clear();
    bool test( int station, int layer, int region ) const;
    bool all( int station, int layer ) const;
    bool all( int station  ) const ;
    bool all( ) const ;
#else
    void set( int station, int layer, int region ) {
        auto b = region + NRegionsPerLayer*(layer + NLayersPerStation*station);
        m_data[ iw(b) ] |=  ib(b); 
    }
    void clear() {
        std::fill( std::begin(m_data), std::end(m_data), Storage(0));
    }
    bool test( int station, int layer, int region ) const {
        auto b = region + NRegionsPerLayer*(layer + NLayersPerStation*station);
        return m_data[iw(b)] & ib(b)  ;
    }
    bool all( int station, int layer ) const {
        return all_n<NRegionsPerLayer>( NRegionsPerLayer*(layer + NLayersPerStation*station) );
    }
    bool all( int station  ) const {
        return all_n<NRegionsPerLayer*NLayersPerStation>( NRegionsPerLayer*NLayersPerStation*station );
    }
    bool all( ) const {
        return all_n<NBits>( 0 );
    }
#endif

};
#endif
