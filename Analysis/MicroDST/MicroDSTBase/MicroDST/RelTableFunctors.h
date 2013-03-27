// $Id: RelTableFunctors.h,v 1.4 2010-08-02 16:38:16 jpalac Exp $
#ifndef MICRODST_RELTABLEFUNCTORS_H
#define MICRODST_RELTABLEFUNCTORS_H 1

// Include files
#include "MicroDST/Functions.h"
/** @namespace MicroDST RelTableFunctors.h MicroDST/RelTableFunctors.h
 *
 *
 *  @author Juan PALACIOS
 *  @date   2009-04-17
 */
namespace MicroDST
{

  template <class TABLE>
  struct Cloners
  {
    typedef boost::function< typename TABLE::From (typename TABLE::From ) > From;
    typedef boost::function< typename TABLE::To   (typename TABLE::To   ) > To;
  };

  template <class TABLE>
  struct EntryClonerBase
  {
    EntryClonerBase( const typename Cloners<TABLE>::From& fromCloner,
                     const typename Cloners<TABLE>::To& toCloner )
      : m_fromCloner(fromCloner),
        m_toCloner(toCloner)
    { }
  protected:
    typename Cloners<TABLE>::From m_fromCloner;
    typename Cloners<TABLE>::To   m_toCloner;
  private:
    EntryClonerBase() {}
  };

  template <class TABLE, bool WT = true>
  struct EntryCloner : public EntryClonerBase<TABLE>
  {

    EntryCloner( const typename Cloners<TABLE>::From& fromCloner,
                 const typename Cloners<TABLE>::To& toCloner )
      : EntryClonerBase<TABLE>(fromCloner, toCloner)
    { }
    typename TABLE::Entry operator() (const typename TABLE::Entry& entry) const
    {
      const typename TABLE::From clonedFrom = this->m_fromCloner( entry.from() );
      const typename TABLE::To clonedTo     = this->m_toCloner  ( entry.to()   );
      return typename TABLE::Entry( clonedFrom, clonedTo, entry.weight() );
    }
  };

  template <class TABLE>
  struct EntryCloner<TABLE, false> : public EntryClonerBase<TABLE>
  {
    EntryCloner( const typename Cloners<TABLE>::From& fromCloner,
                 const typename Cloners<TABLE>::To& toCloner )
      : EntryClonerBase<TABLE>( fromCloner, toCloner )
    { }
    typename TABLE::Entry operator() ( const typename TABLE::Entry& entry ) const
    {
      const typename TABLE::From clonedFrom = this->m_fromCloner ( entry.from() );
      const typename TABLE::To   clonedTo   = this->m_toCloner   ( entry.to()   );
      return typename TABLE::Entry( clonedFrom, clonedTo );
    }
  };

  template <class TABLE>
  struct TableCloner
  {
    TableCloner( const typename Cloners<TABLE>::From& fromCloner,
                 const typename Cloners<TABLE>::To& toCloner )
      : m_cloner(fromCloner, toCloner)
    { }
    TABLE* operator() ( const TABLE* table )
    {
      TABLE* cloneTable = new TABLE();

      typename TABLE::Range relations = table->relations();
      for ( typename TABLE::Range::const_iterator iRel = relations.begin();
            iRel != relations.end(); ++iRel )
      {
        typename TABLE::Entry entryClone = m_cloner(*iRel);
        if ( isValid(entryClone.from()) && isValid(entryClone.to()) )
        {
          cloneTable->add(entryClone);
        }
      } // loop on all relations

      return cloneTable;
    }
  protected:
    EntryCloner<TABLE, TABLE::TypeTraits::weighted> m_cloner;
  private:
    TableCloner() {}
  };

}

#endif // MICRODST_TABLETRAITS_H
