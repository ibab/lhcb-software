// $Id: MCDecayFinder.h,v 1.4 2002-07-26 12:57:03 odie Exp $
#ifndef TOOLS_MCDECAYFINDER_H 
#define TOOLS_MCDECAYFINDER_H 1

//#define YYDEBUG 1
#ifdef YYDEBUG
#include <stdio.h>
#include <stdlib.h>
#endif

// Include files
// from STL
#include <string>

#include "GaudiKernel/AlgTool.h"
#include "DaVinciMCTools/IMCDecayFinder.h"

class IParticlePropertySvc;
struct yy_buffer_state;

/** @class MCDecayFinder MCDecayFinder.h
 *
 *  This tool allows you to search a vector of MCParticle for a specific decay.
 *  The decay is given thru the property 'Decay' as a string which describe
 *  what you are looking for. The syntax of this expression has been designed
 *  to be as powerful as possible. It can express inclusive decays as well as
 *  (almost) any set of decay.
 *
 *  The description is made of 'particle' and 'decay'.
 *  A 'particle' is one of:
 *  - a particle name as known by the ParticlePropertySvc,
 *  - a set of 'particle's expressed like this: "{A, B, C}",
 *  - a set of 'particle's or their charge conjugate, expressed as "[A,B,C]cc",
 *  - a 'particle' with it's oscillation flag set ("[A]os") [NOT TESTED YET],
 *  - a 'wildcard', any particle with some quarks inside is expressed as "<Xq>"
 *    with up to tree quarks in place of "q". Quarks are u,d,c,s,t,b,u~,d~,...
 *  a 'decay' is one of:
 *  - an optional "(" and a 'particle' and the matching ")" if needed,
 *  - a "(", a 'particle', one of "->" or "=>", a blank separated list of decay,
 *    an optional "..." and a ")", e.g. "(B0 -> pi+ pi-)",
 *  - a "pp" one of "->" or "=>" and a blank separated list of decay.
 *  Note: the "()" are not needed around the first decay so the previous exemple
 *        could simply be written "B0 -> pi+ pi-" but they are mandatory for
 *        subdecay like "B0 -> (J/psi(1S) -> mu+ mu-) KS0".
 *  Note: If you want to find a stable particle you must explicily request that
 *        it decay to nothing but the arrow has to be dropped. So this looks
 *        like "B0 -> D_s- (K+)", here the D_s- can decay to whatever but
 *        the K+ must not decay. NB: Right now secondary iteraction are seen
 *        as decay!
 *  Note: "->" means "decay to" and
 *        "=>" means "decay to .... with some resonnances in the middle".
 *        So if you want to catch B0 to D_s- K+ with any D*_s in beetween you
 *        simply write "B0 => D_s- K+ ...". NB: The "..." is here to catch any
 *        number of particles of any kind so you get the products of the D*_s-.
 *  Note: "pp" stands for "proton - proton collision". The decay products will
 *        be matched against the particles with no mother. An implicit "..." is
 *        appended. Only the boolean information for that kind of find can be
 *        used, the returned particle is set (MCParticle *)1 in case of match
 *        NULL otherwise.
 *
 * NOTE: You realy don't want to know how it works!
 *
 *  @author Olivier Dormond
 *  @date   12/03/2002
 */
class MCDecayFinder : public AlgTool, virtual public IMCDecayFinder
{
public:
  /// Standard Constructor
  MCDecayFinder( const std::string& type,
                 const std::string& name,
                 const IInterface* parent );

  /// Destructor 
  virtual ~MCDecayFinder( ); ///< Destructor

  StatusCode initialize( void );

  /// Does the described decay exists in the event?
  bool hasDecay( const std::vector<MCParticle*> &event );

  bool hasDecay( const MCParticles &event );

  /** Try to find the (next) match of the decay in the event.
   *
   *  @return true if found, false otherwise.
   *  @param event, the vector of particles in which to search for the decay.
   *  @param previous_result, the address of the previously matched particle
   *         or NULL if you want to start from the begining. This is an In/Out
   *         parameter, a reference to a const MCParticle *.
   *  The particle pointed to by previous_result must be contained in event.
   */
  bool findDecay( const std::vector<MCParticle*> &event,
                  const MCParticle *&previous_result );

  bool findDecay( const MCParticles &event,
                  const MCParticle *&previous_result );

  /// Enumaration types used internally.
  enum Quarks { empty, up, down, charm, strange, top, bottom, antiup,
                antidown, anticharm, antistrange, antitop, antibottom };
  enum Quantums { Q_quantum=1, B_quantum, l_quantum, I_quantum, J_quantum,
                  S_quantum, L_quantum, G_quantum, C_quantum, P_quantum,
                  CP_quantum, c_quantum, s_quantum, t_quantum, b_quantum };
  enum Relations { eq_rel=1, lesseq_rel, greatereq_rel, less_rel, greater_rel,
                   noteq_rel };

private:
  /// The opaque representation of a particle matcher
  class ParticleMatcher
  {
  public:
    ParticleMatcher( IParticlePropertySvc *ppSvc );
    ParticleMatcher( ParticleMatcher &copy );
    ParticleMatcher( std::string *name, IParticlePropertySvc *ppSvc );
    ParticleMatcher( Quarks q1, Quarks q2, Quarks q3,
                     IParticlePropertySvc *ppSvc );
    ParticleMatcher( Quantums quantum, Relations relation, double value,
                     IParticlePropertySvc *ppSvc );
    bool test( const MCParticle *part );
    void setQmark( void ) { qmark = true; }
    bool getQmark( void ) { return qmark; }
    void setConjugate( void ) { conjugate = true; }
    bool getConjugate( void ) { return conjugate; }
    void setOscillate( void ) { oscillate = true; }
    void setNotOscillate( void ) { noscillate = true; }
    bool getOscillate( void ) { return oscillate; }
    bool getNotOscillate( void ) { return noscillate; }
    void setInverse( void ) { inverse = true; }
    bool getInverse( void ) { return inverse; }
    void setStable( void ) { stable = true; }
    bool getStable( void ) { return stable; }
    bool getExact( void ) { return !qmark && !inverse
                              && !conjugate && (type==id); }
    void conjugateID( void );
  private:
    int conjugatedID( int id );
    enum Type { notest, id, quark, quantum } type;
    union {
      struct { Quarks q1, q2, q3; } quarks;
      int stdHepID;
      struct { Quantums q; Relations r; double d; } relation;
    } parms;
    bool qmark;
    bool conjugate;
    bool oscillate;
    bool noscillate;
    bool inverse;
    bool stable;
    IParticlePropertySvc *m_ppSvc;
  };

  /// The opaque representation of the decay chain.
  class Descriptor
  {
  public:
    Descriptor( IParticlePropertySvc *ppSvc, double resonnanceThreshold );
    Descriptor( Descriptor &copy );
    Descriptor( ParticleMatcher *mother, IParticlePropertySvc *ppSvc,
                double resonnanceThreshold );

    ~Descriptor();

    template<class iter> bool test( const iter first, const iter last,
                                    const MCParticle *&previous_result )
    {
      iter start;
      if( previous_result &&
          ((start=std::find(first,last,previous_result))
           == last) )
      {
        previous_result = NULL;
        return false; // Bad previous_result
      }
      if( previous_result )
        start++;
      
      if( mother == NULL ) // No mother == pp collision
      {
        std::list<const MCParticle*> prims;
        iter i;
        for( i=(previous_result ? start : first); i != last; i++ )
        {
          MCVertex *origin = (*i)->originVertex();
          if( origin && origin->mother() )
            continue;
          prims.push_back(*i);
        }
        if( skipResonnance )
          filterResonnances( prims );
        if( testDaughters(prims) )
        {
          previous_result = (const MCParticle *)1;
          return true;
        }
        return false;
      }
      
      iter part_i;
      part_i = (previous_result ? start : first);
      while( (part_i != last) && (test(*part_i) == false) )
        part_i++;
      
      if( part_i != last )
      {
        previous_result = *part_i;
        return true;
      }
      return false;
    }
    
    void setAlternate( Descriptor *a ) { alternate = a; }
    Descriptor *getAlternate( void ) { return alternate; }

    void setMother( ParticleMatcher *m ) { mother = m; }

    void addDaughter( Descriptor *daughter );
    std::vector<Descriptor *> &getDaughters( void ) { return daughters; }

    void setElipsis( void ) { elipsis = true; }
    bool getElipsis( void ) { return elipsis; }
    void setResonnance( void ) { skipResonnance = true; }
    void conjugate( void );
  private:
    bool test( const MCParticle *mother );
    bool testDaughters( std::list<const MCParticle*> &parts );
    void addNonResonnantDaughters( std::list<const MCParticle*> &parts,
                                   const MCParticle *part );
    void filterResonnances( std::list<const MCParticle*> &parts );

    ParticleMatcher *mother;
    std::vector<Descriptor *> daughters; // wildcard mother at the end!
    bool skipResonnance;
    bool elipsis;
    double m_resThreshold;
    IParticlePropertySvc *m_ppSvc;

    Descriptor *alternate;
  };

  class DescriptorError
  {
  public:
    DescriptorError( std::string s ) : msg(s) {}
    std::string &cause( void ) { return msg; }
  private:
    std::string msg;
  };

  IParticlePropertySvc *m_ppSvc;
  std::string m_source;
  Descriptor *m_decay;
  double m_resThreshold;

  bool compile( std::string &decay );

#include "mcparser.h"

  int yparse( void );
  void yerror( char *msg ) { throw DescriptorError(msg); }

  int ylex( void );

  static short int ylhs[];
  static short int ylen[];
  static short int ydefred[];
  static short int ydgoto[];
  static short int ysindex[];
  static short int yrindex[];
  static short int ygindex[];
  static short int ytable[];
  static short int ycheck[];
  static int ydebug;
  static int ynerrs;
  static int yerrflag;
  static int ychar;
  static short int *yssp;
  static YYSTYPE *yvsp;
  static YYSTYPE yval;
  static short int yss[500];
  static YYSTYPE yvs[500];
#ifdef YYDEBUG
  static char *yname[];
  static char *yrule[];
#endif
public:
  static YYSTYPE ylval;
};
#endif // TOOLS_MCDECAYFINDER_H
