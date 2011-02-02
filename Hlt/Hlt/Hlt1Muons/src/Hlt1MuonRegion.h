// $Id: $
#ifndef HLT1MUONREGION_H 
#define HLT1MUONREGION_H 1

// Include files

/** @class Hlt1MuonRegion Hlt1MuonRegion.h
 *  Class to contain some geometrical info about regions of a muon station.
 *
 *  @author Roel Aaij
 *  @date   2010-12-07
 */
class Hlt1MuonRegion {
public: 

   /// Standard constructor
   Hlt1MuonRegion( const unsigned int id,
                   const double xmin, const double xmax,
                   const double ymin, const double ymax )
      : m_id( id ), m_xmin( xmin ), m_xmax( xmax ),
        m_ymin( ymin ), m_ymax( ymax )
   {

   }

   virtual ~Hlt1MuonRegion( ) { }; ///< Destructor

   unsigned int id() const
   {
      return m_id;
   }

   double xmin() const
   {
      return m_xmin;
   }

   double xmax() const
   {
      return m_xmax;
   }

   double ymin() const
   {
      return m_ymin;
   }

   double ymax() const
   {
      return m_ymax;
   }

   bool overlap( const double xmin, const double xmax,
                 const double ymin, const double ymax ) const
   {
      return !( xmax < m_xmin || xmin > m_xmax || ymin > m_ymax || ymax < m_ymin );
   }

private:
   
   unsigned int m_id;

   double m_xmin;
   double m_xmax;
   double m_ymin;
   double m_ymax;

};
#endif // HLT1MUONREGION_H
