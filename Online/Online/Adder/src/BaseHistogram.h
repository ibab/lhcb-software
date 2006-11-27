#ifndef AIDAROOT_BASEHISTOGRAM_H
#define AIDAROOT_BASEHISTOGRAM_H 1

//  This file is part of the LCG PI library
//  Copyright (C) 2003 by the AIDA and LCG PI teams.  All rights reserved.
//  This library is free software and under the terms of the
//  GNU Library General Public License described in the LGPL.txt 

#include "TH1D.h" 
#include "GaudiPI/AIDA_Annotation/AIDA_Annotation.h"
#include "AIDA/IAxis.h"


namespace AIDA_ROOT {

  static const std::string nameKey = "Name";
  static const std::string titleKey = "Title";   

  class AIDA::IAxis; // from AIDA

  /**
   * internal wrapper to TH1 matching IHistogram and IProfile interfaces
   * @param R the Root histogram
   * @param B the AIDA base (either  IHistogram or IProfile)
   *
   * @author The LCG PI team (http://lcgapp.cern.ch/project/pi/)
   *
   */
  template<typename R, typename B>
  class  BaseHistogram : public virtual B { 
  public:
    typedef BaseHistogram<R,B> self;
    typedef TH1D BASEREP;
    typedef R REP;
    typedef B IBase;
    //typedef AIDA_CPP::DummyAnnotation Annotation;
 
  public:
    /** Default constructor
     */
    BaseHistogram() { 
      // rep.SetDirectory(0);
      
      // sumw2 is broken for empty profiles...
      //  initRep();
    }

  protected:
    /** change the representation
     */
    void newRep(const REP& irep) {
      rep = irep;
      initRep();
    }

  public:
    // --------------------------------------------------------------------------------
    // from IHistogram or IProfile
    // --------------------------------------------------------------------------------

    /**
     * Sum of the entries in all the IHistogram's bins,
     * i.e in-range bins, UNDERFLOW and OVERFLOW.
     * This is equivalent to the number of times the
     * method fill was invoked.
     * @return The sum of all the entries.
     *
     */
    int  allEntries (  ) const  { return int(rep.GetEntries()); }

    /**
     * Number of entries in the UNDERFLOW and OVERFLOW bins.
     * @return The number of entries outside the range of the IHistogram.
     *
     */
    // FIX ME: how to get this from Root ??
    int  extraEntries (  ) const  { return 0; }

    /**
     * Number of equivalent entries, i.e. <tt>SUM[ weight ] ^ 2 / SUM[ weight^2 ]</tt>
     * @return The number of equivalent entries.
     *
     */
    double  equivalentBinEntries (  ) const  { 
      if (sumBinHeights (  )<=0) return 0;
      Stat_t stats[11];   // cover up to 3D...
      rep.GetStats(stats);
      return stats[0]*stats[0]/stats[1]; 
}

    /**
     * Sum of in-range bin heights in the IHistogram,
     * UNDERFLOW and OVERFLOW bins are excluded.
     * @return The sum of the in-range bins heights.
     *
     */
    double  sumBinHeights (  ) const  { return rep.GetSumOfWeights(); }

    /**
     * Sum of the heights of all the IHistogram's bins,
     * i.e in-range bins, UNDERFLOW and OVERFLOW.
     * @return The sum of all the bins heights.
     *
     */
    // double  sumAllBinHeights (  ) const  { return rep.GetEntries(); }

    double  sumAllBinHeights (  ) const  { return rep.GetSum(); }

    /**
     * Sum of heights in the UNDERFLOW and OVERFLOW bins.
     * @return The sum of the heights of the out-of-range bins.
     *
     */
    double  sumExtraBinHeights (  ) const  { return  sumAllBinHeights()-sumBinHeights(); }
    //    double  sumExtraBinHeights (  ) const  { return  rep.GetBinContent(0)+rep.GetBinContent(rep.GetNbinsX()+1); }

    /**
     * Minimum height of the in-range bins,
     * i.e. not considering the UNDERFLOW and OVERFLOW bins.
     * @return The minimum height among the in-range bins.
     *
     */
    double  minBinHeight (  ) const  { return rep.GetMinimum(); }

    /**
     * Maximum height of the in-range bins,
     * i.e. not considering the UNDERFLOW and OVERFLOW bins.
     * @return The maximum height among the in-range bins.
     *
     */
    double  maxBinHeight (  ) const  { return rep.GetMaximum(); }

    /**
     * Scale the weights and the errors of all the IHistogram's bins
     * (in-range and out-of-range ones) by a given scale factor.
     * @param scaleFactor The scale factor.
     * @return false If the scale factor is negative.
     *
     */
    bool  scale ( double scaleFactor )   { rep.Scale ( scaleFactor ); return true; }

    // --------------------------------------------------------------------------------
    // from IBaseHistogram
    // --------------------------------------------------------------------------------

    /**
     * Get the Histogram's title.
     * @return The Histogram's title.
     *
     */
    std::string title (  ) const { 
      // update value in annotation 
      const std::string local = rep.GetTitle();
      m_annotation.setValue( titleKey , local ); 
      return m_annotation.value( titleKey );
    }

    /**
     * Set the histogram title.
     * @param title The title.
     * @return false If title cannot be changed.
     *
     */
    bool  setTitle ( const std::string & title )   { rep.SetTitle ( title.c_str() ); return true;}

    /**
     * Get the IAnnotation associated with the Histogram.
     * @return The IAnnotation.
     *
     */
    AIDA::IAnnotation & annotation (  )   { return m_annotation; }

    const AIDA::IAnnotation & annotation (  ) const  { return m_annotation; }

    /**
     * Reset the Histogram; as if just created.
     * @return false If something goes wrong.
     *
     */
    bool  reset (  )   { rep.Reset ( ); return true; }

    /**
     * Get the number of in-range entries in the Histogram.
     * @return The number of in-range entries.
     *
     */ 

    int  entries (  ) const  { return allEntries ( ) - extraEntries(); }
    /**
     * Add to this Histogram the contents of another Histogram.
     * @param hist The Histogram to be added to this Histogram.
     * @return false If the Histogram binnings are incompatible.
     *
     */
    bool  add ( const self & hist ) { rep.Add(&hist.rep); return true;}




    /// internal representation (THx TProfile etc)
    REP& representation() const { return rep;}




  private:
    /// initialize the representation...
    void initRep() {  
      rep.SetDirectory(0);
      //rep.Sumw2();
    }
    
  private:

    mutable REP rep;

    mutable pi::AIDA_Annotation::AIDA_Annotation m_annotation;


  };


}


#endif // AIDAROOT_BASEHISTOGRAM_H
