#ifndef globals_H 
#define globals_H 1

#include <string>
#include <map>
#include "TString.h"

#define endreq endl
#define endmsg endl

/* #define MASK_BIT1 1 */
/* #define MASK_BIT2 2 */
/* #define MASK_BIT3 4 */
/* #define MASK_BIT4 8 */
/* #define MASK_BIT5 16 */
/* #define MASK_BIT6 32 */
/* #define MASK_BIT7 64 */
/* #define MASK_BIT8 128 */
/* #define MASK_BIT9 256 */

extern double PI;
extern std::map< const TString, const double > property;
extern std::map< const TString, const TString > stringproperty;
extern int DBGLEVEL;

const std::string ROJO("\x1b[91m"),ROJO2("\x1b[31m"),
	  VERDE("\x1b[32m"), AMARILLO("\x1b[33m"),
	  AZUL("\x1b[94m"),AZULCLARO("\x1b[36m"), 
	  VIOLETA("\x1b[95m"),SUBROJO("\x1b[101m"),
	  SUBVERDE("\x1b[102m"), SUBAMARILLO("\x1b[103m"), 
	  SUBAZUL("\x1b[104m"), SUBVIOLETA("\x1b[105m"),
	  SUBBLANCO("\x1b[107m"),BLANCO("\x1b[37m"), 
	  BOLD("\x1b[1m"),ENDC("\x1b[m"),FAINT("\x1b[2m"),
	  UNDERLINE("\x1b[4m"),BLINK("\x1b[5m"),
	  CLEARSCREEN("\x1b[2J"), DEL1L("\x1b[2A\n\n");

template<typename Ch, typename Traits = std::char_traits<Ch> >
struct basic_nullbuf : std::basic_streambuf<Ch, Traits> {
     typedef std::basic_streambuf<Ch, Traits> base_type;
     typedef typename base_type::int_type int_type;
     typedef typename base_type::traits_type traits_type;

     virtual int_type overflow(int_type c) {
         return traits_type::not_eof(c);
     }
};
// convenient typedefs
typedef basic_nullbuf<char> nullbuf;
typedef basic_nullbuf<wchar_t> wnullbuf;

#endif
