#ifndef VECTORIZATION_SUPPORT_H
#define VECTORIZATION_SUPPORT_H

#include <type_traits>

template <typename T,size_t N> struct vector_type;

template<typename T>
struct vector_type<T,1> { typedef T type; };

template<typename T>
struct vector_type<T,2> {
    // 
#ifdef __clang__
  typedef T type __attribute__ ((ext_vector_type( 2  ))); // vector of two Ts...
#else
  // icc 13 onwards should support this at well... 
  typedef T type __attribute__ ((vector_size( 2 * sizeof(T) ))); // vector of two Ts...
#endif
};

template<typename T>
struct vector_type<T,4> {
#ifdef __clang__
  typedef T type __attribute__ ((ext_vector_type( 4  ))); // vector of four Ts...
#else
  // icc 13 onwards should support this at well... 
  typedef T type __attribute__ ((vector_size( 4 * sizeof(T) ))); // vector of four Ts...
#endif
};

// C++14 Compile-time integer sequences -- this can go once we use C++14...
// #include <utility> // defines (in C++14) std::make_index_sequence and std::index_sequence
template<size_t... indexes> struct index_sequence {
    static size_t size() { return sizeof...(indexes); }
};

template<size_t currentIndex, size_t...indexes> struct make_index_sequence_helper;

template<size_t...indexes> struct make_index_sequence_helper<0, indexes...> {
    typedef index_sequence<indexes...> type;
};

template<size_t currentIndex, size_t...indexes> struct make_index_sequence_helper {
    typedef typename make_index_sequence_helper<currentIndex - 1, currentIndex - 1, indexes...>::type type;
};

template<size_t N> struct make_index_sequence : public make_index_sequence_helper<N>::type { };

// use index sequence to scatter...
#include <array>

template<typename Ret, typename Arg>
inline std::array<Ret,1> scatter_array_impl( Arg&& a, index_sequence<0> )
{
    return { std::forward<Arg>(a) };
}

template<typename Ret, typename Arg, std::size_t... I>
inline std::array<Ret,sizeof...(I)> scatter_array_impl( const Arg& a, index_sequence<I...> )
{
    return { a[I]... };
}

template<typename Ret, size_t N, typename Arg>
inline std::array<Ret,N> scatter_array(Arg&& a)
{
    return scatter_array_impl<Ret>( std::forward<Arg>(a), make_index_sequence<N>{} );
}

#endif
