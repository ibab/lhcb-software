// $Id: LinkerEntry.h,v 1.1 2005-01-27 14:52:21 cattanem Exp $
#ifndef LINKER_LINKERENTRY_H 
#define LINKER_LINKERENTRY_H 1

/** @class LinkerEntry LinkerEntry.h Linker/LinkerEntry.h
 *  Describe an entry of a relation, expanded to pointer
 *
 *  @author Olivier Callot
 *  @date   2005-01-19
 */
template <class SOURCE, class TARGET > class LinkerEntry {
public: 
  /** constructor, with the needed information
   *  @param src     source of the relation
   *  @param tgt     target of the relation
   *  @param weight  weight of the relation
   */
  LinkerEntry( const SOURCE* src, const TARGET* tgt, double weight ){
    m_src = src;
    m_target = tgt;
    m_weight = weight;
  }; 

  virtual ~LinkerEntry( ) {}; ///< Destructor

  /** accessor to the source
   *  @return   the source information
   */
  const SOURCE* from()   const { return m_src; }

  /** accessor to the target
   *  @return   the target information
   */
  const TARGET* to()     const { return m_target;}

  /** accessor to the weight
   *  @return   the weight information
   */
  double  weight() const { return m_weight; }

protected:

private:
  const SOURCE* m_src;
  const TARGET* m_target;
  double  m_weight;
};
#endif // LINKER_LINKERENTRY_H
