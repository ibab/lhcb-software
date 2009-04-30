// $Id: RelTableFunctors.h,v 1.1 2009-04-30 12:59:21 jpalac Exp $
#ifndef MICRODST_RELTABLEFUNCTORS_H 
#define MICRODST_RELTABLEFUNCTORS_H 1

// Include files
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
    typedef boost::function< const typename TABLE::From (const typename TABLE::From )> From;
    typedef boost::function< const typename TABLE::To (const typename TABLE::To )> To;
  };

  template <class TABLE>
  struct EntryClonerBase
  {
    EntryClonerBase(const typename Cloners<TABLE>::From& fromCloner,
                    const typename Cloners<TABLE>::To& toCloner)
      :
      m_fromCloner(fromCloner),
      m_toCloner(toCloner)
    {
    }

  protected:
    typename Cloners<TABLE>::From m_fromCloner;
    typename Cloners<TABLE>::To m_toCloner;

  private:
    EntryClonerBase() {}
    
  };

  template <class TABLE, bool WT = true>
  struct EntryCloner : public EntryClonerBase<TABLE>
  {
    
    EntryCloner(const typename Cloners<TABLE>::From& fromCloner,
                const typename Cloners<TABLE>::To& toCloner)
      :
      EntryClonerBase<TABLE>(fromCloner, toCloner)
    {
    }
    typename TABLE::Entry operator() (const typename TABLE::Entry& entry) const
    {
      const typename TABLE::From clonedFrom = m_fromCloner(entry.from());
      const typename TABLE::To clonedTo = m_toCloner( entry.to() );
      return typename TABLE::Entry(clonedFrom, clonedTo, entry.weight());
    }
  };

  template <class TABLE>
  struct EntryCloner<TABLE, false> : public EntryClonerBase<TABLE>
  {
    EntryCloner(const typename Cloners<TABLE>::From& fromCloner,
                const typename Cloners<TABLE>::To& toCloner)
      :
      EntryClonerBase<TABLE>(fromCloner, toCloner)
    {
    }
    typename TABLE::Entry operator() (const typename TABLE::Entry& entry) const
    {
      const typename TABLE::From clonedFrom = m_fromCloner(entry.from());
      const typename TABLE::To clonedTo = m_toCloner( entry.to() );
      return typename TABLE::Entry(clonedFrom, clonedTo);
    }

  };

  template <class TABLE>
  struct TableCloner
  {
    TableCloner(const typename Cloners<TABLE>::From& fromCloner,
                const typename Cloners<TABLE>::To& toCloner)
      :
      m_cloner(fromCloner, toCloner)
    {
    }
    TABLE* operator() (const TABLE* table) 
    {
      TABLE* cloneTable = new TABLE();

      typename TABLE::Range relations = table->relations();

      for (typename TABLE::Range::const_iterator iRel = relations.begin();
           iRel != relations.end();
           ++iRel ) {
        if ( iRel->from() ) {
          typename TABLE::Range range = table->relations(iRel->from());
          for (typename TABLE::Range::const_iterator iRange = range.begin();
               iRange != range.end();
               ++iRange) {
            typename TABLE::Entry entryClone = m_cloner(*iRange);
            if (entryClone.from() && entryClone.to() ) {
              cloneTable->add(entryClone);
            }
          } // loop on related Tos
        } // if From* found
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
