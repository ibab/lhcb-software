
// Application to profile implementations of log( exp(x) - 1 )

// STL
#include <random>
#include <vector>
#include <iostream>
#include <string>
#include <array>

// Vc
//#include "Vc/Vc"

// VectorClass
//#include "vectorclass/vectorclass.h"

// VDT
#include "vdt/exp.h"
#include "vdt/log.h"

// Eigen
#include <Eigen/Geometry>

float logExpOriginal( const float x )
{

  // Parameters
  static const auto limitA ( 0.001f );
  static const auto limitB ( 0.01f  );
  static const auto limitC ( 0.1f   );
  static const auto limitD ( 1.0f   );
  
  // Initialise 
  auto res ( 0.0f );
  
  // pick the interpolation to use
  if ( x <= limitD )
  {
    // A collection of rational power series covering the important range
    // note by construction this function should never be called for x < limitA
    
    const auto xx    = x*x;
    const auto xxx   = xx*x;
    const auto xxxx  = xx*xx;
    const auto xxxxx = xx*xxx;
    
    if      ( x > limitC )
    {
      res = (-5.751779337152293f - 261.58791552313113f*x -
             1610.1902353909695f*xx - 291.61172549536417f*xxx +
             3733.957211885683f*xxxx + 1224.2104172168554f*xxxxx)/
        (1.0f + 79.52981108433892f*x + 953.4570349071099f*xx +
         2638.609797400796f*xxx + 1506.9612115322623f*xxxx -
         27.33558114045007f*xxxxx);
    }
    else if ( x > limitB )
    {
      res = (-7.845788509794026f - 3428.7804135353526f*x -
             228752.20145929293f*xx - 3.082032088759535e6f*xxx -
             3.836270197409883e6f*xxxx + 1.2251900378118051e7f*xxxxx)/
        (1.0f + 638.7306815040638f*x + 60430.91709817034f*xx +
         1.315432074531156e6f*xxx + 6.373056770682967e6f*xxxx +
         3.3914176474223877e6f*xxxxx);
    }
    else if ( x > limitA )
    {
      res = (-10.160864268729455f - 49897.23275778952f*x -
             3.855669108991894e7f*xx - 6.777802095268419e9f*xxx -
             2.421987003565588e11f*xxxx - 3.5610129242332263e11f*xxxxx)/
        (1.0f + 6487.897657865318f*x + 6.294785881144457e6f*xx +
         1.4333658673633337e9f*xxx + 7.670700007081306e10f*xxxx +
         6.06654149712832e11f*xxxxx);
    }
    else
    {
      // should never get here. But just in case ...
      //res = std::log( std::exp(x) - 1.0 );
      res = vdt::fast_logf( vdt::fast_expf(x) - 1.0f );
    }
  }
  else
  {
    // Very very rarely called in this regime, so just use the full fat version
    //res = std::log( std::exp(x) - 1.0 );
    res = vdt::fast_logf( vdt::fast_expf(x) - 1.0f );
  }
  
  // return
  return res;
}

float logExpOriginal( const float x, const float y )
{
  return logExpOriginal(x) - logExpOriginal(y);
}

// float logExpVectorClass( const float x )
// {

//   // Parameters
//   const auto limitA ( 0.001f );
//   const auto limitB ( 0.01f  );
//   const auto limitC ( 0.1f   );
//   const auto limitD ( 1.0f   );
  
//   // Initialise return value
//   auto res ( 0.0f );

//   // pick the interpolation to use
//   if ( x <= limitD )
//   {
//     // A collection of rational power series covering the important range
//     // note by construction this function should never be called for x < limitA
    
//     // vector with powers of x
//     const auto xxxx = ( Vec4f( x,    x,    x,    x ) *
//                         Vec4f( 1.0f, x,    x,    x ) *
//                         Vec4f( 1.0f, 1.0f, x,    x ) *
//                         Vec4f( 1.0f, 1.0f, 1.0f, x ) );
    
//     if      ( x > limitC ) // 0.1 -> 1
//     {
//       const Vec4f topP( -101.76227310588246f, -127.31825068394369f,
//                       317.31602891172594f,  135.0567120741147f );
//       const Vec4f botP(  40.31685825976414f,   202.81583582446692f,
//                        160.94697665742055f,   -2.088756969431516f );
//       res = ( ( -5.143220028201872f + horizontal_add( topP * xxxx ) ) /
//               ( 1.0f                + horizontal_add( botP * xxxx ) ) );
//     }
//     else if ( x > limitB ) // 0.01 -> 0.1
//     {
//       const Vec4f topP( -1957.455251107371f,   -63820.69082039389f,
//                         -227666.32263762745f,  431986.40491931344f );
//       const Vec4f botP( 403.96307514244734f,   20798.07111653145f,
//                         188119.3222406968f,    180043.28571064543f );
//       res = ( ( -7.4441271863249625f + horizontal_add( topP * xxxx ) ) /
//               ( 1.0f                 + horizontal_add( botP * xxxx ) ) );
//     }
//     else if ( x > limitA ) // 0.001 -> 0.01
//     {
//       const Vec4f topP( -29524.941133033215f,  -1.1896019336956682e7f,
//                       -7.860596631970513e8f, -2.795797091759987e9f );
//       const Vec4f botP( 4115.755490541176f,    2.19207068008156e6f,
//                       2.1358160394371653e8f, 2.6911720709828815e9f );
//       res = ( ( -9.760898531545386f + horizontal_add( topP * xxxx ) ) /
//               ( 1.0f                + horizontal_add( botP * xxxx ) ) );
//     }
//     else
//     {
//       // should never get here. But just in case ...
//       //res = std::log( std::exp(x) - 1.0 );
//       res = vdt::fast_logf( vdt::fast_expf(x) - 1.0f );
//     }
//   }
//   else
//   {
//     // Very very rarely called in this regime, so just use the full fat version
//     //res = std::log( std::exp(x) - 1.0 );
//     res = vdt::fast_logf( vdt::fast_expf(x) - 1.0f );
//   }
  
//   return res;
// }

// float logExpVectorClass( const float x, const float y )
// {
//   // Parameters
//   const auto limitA ( 0.001f );
//   const auto limitB ( 0.01f  );
//   const auto limitC ( 0.1f   );
//   const auto limitD ( 1.0f   );
  
//   // Initialise return value
//   auto res ( 0.0f );

//   const Vec8f x1( x,    x,    x,    x, y,    y,    y,    y ); 
//   const Vec8f x2( 1.0f, x,    x,    x, 1.0f, y,    y,    y );
//   const Vec8f x3( 1.0f, 1.0f, x,    x, 1.0f, 1.0f, y,    y );
//   const Vec8f x4( 1.0f, 1.0f, 1.0f, x, 1.0f, 1.0f, 1.0f, y );
//   const auto xxyy = x1 * x2 * x3 * x4;

//   // Compute now the vlaues for x and y

//   // A collection of rational power series covering the important range
//   // note by construction this function should never be called for x < limitA

//   if ( x <= limitD )
//   {
//     if      ( x > limitC ) // 0.1 -> 1
//     {
//       const Vec4f topP( -101.76227310588246f, -127.31825068394369f,
//                       317.31602891172594f,  135.0567120741147f );
//       const Vec4f botP(  40.31685825976414f,   202.81583582446692f,
//                        160.94697665742055f,   -2.088756969431516f );
//       res = ( ( -5.143220028201872f + horizontal_add( topP * xxyy.get_low() ) ) /
//               ( 1.0f                + horizontal_add( botP * xxyy.get_low() ) ) );
//     }
//     else if ( x > limitB ) // 0.01 -> 0.1
//     {
//       const Vec4f topP( -1957.455251107371f,   -63820.69082039389f,
//                         -227666.32263762745f,  431986.40491931344f );
//       const Vec4f botP( 403.96307514244734f,   20798.07111653145f,
//                         188119.3222406968f,    180043.28571064543f );
//       res = ( ( -7.4441271863249625f + horizontal_add( topP * xxyy.get_low() ) ) /
//               ( 1.0f                 + horizontal_add( botP * xxyy.get_low() ) ) );
//     }
//     else if ( x > limitA ) // 0.001 -> 0.01
//     {
//       const Vec4f topP( -29524.941133033215f,  -1.1896019336956682e7f,
//                       -7.860596631970513e8f, -2.795797091759987e9f );
//       const Vec4f botP( 4115.755490541176f,    2.19207068008156e6f,
//                       2.1358160394371653e8f, 2.6911720709828815e9f );
//       res = ( ( -9.760898531545386f + horizontal_add( topP * xxyy.get_low() ) ) /
//               ( 1.0f                + horizontal_add( botP * xxyy.get_low() ) ) );
//     }
//     else
//     {
//       // should never get here. But just in case ...
//       res = vdt::fast_logf( vdt::fast_expf(x) - 1.0f );
//     }
//   }
//   else
//   {
//     // Very very rarely called in this regime, so just use the full fat version
//     res = vdt::fast_logf( vdt::fast_expf(x) - 1.0f );
//   }

//   if ( y <= limitD )
//   {
//     if      ( y > limitC ) // 0.1 -> 1
//     {
//       const Vec4f topP( -101.76227310588246f, -127.31825068394369f,
//                       317.31602891172594f,  135.0567120741147f );
//       const Vec4f botP(  40.31685825976414f,   202.81583582446692f,
//                        160.94697665742055f,   -2.088756969431516f );
//       res -= ( ( -5.143220028201872f + horizontal_add( topP * xxyy.get_high() ) ) /
//                ( 1.0f                + horizontal_add( botP * xxyy.get_high() ) ) );
//     }
//     else if ( y > limitB ) // 0.01 -> 0.1
//     {
//       const Vec4f topP( -1957.455251107371f,   -63820.69082039389f,
//                         -227666.32263762745f,  431986.40491931344f );
//       const Vec4f botP( 403.96307514244734f,   20798.07111653145f,
//                         188119.3222406968f,    180043.28571064543f );
//       res -= ( ( -7.4441271863249625f + horizontal_add( topP * xxyy.get_high() ) ) /
//                ( 1.0f                 + horizontal_add( botP * xxyy.get_high() ) ) );
//     }
//     else if ( y > limitA ) // 0.001 -> 0.01
//     {
//       const Vec4f topP( -29524.941133033215f,  -1.1896019336956682e7f,
//                       -7.860596631970513e8f, -2.795797091759987e9f );
//       const Vec4f botP( 4115.755490541176f,    2.19207068008156e6f,
//                       2.1358160394371653e8f, 2.6911720709828815e9f );
//       res -= ( ( -9.760898531545386f + horizontal_add( topP * xxyy.get_high() ) ) /
//                ( 1.0f                + horizontal_add( botP * xxyy.get_high() ) ) );
//     }
//     else
//     {
//       // should never get here. But just in case ...
//       res -= vdt::fast_logf( vdt::fast_expf(y) - 1.0f );
//     }
//   }
//   else
//   {
//     // Very very rarely called in this regime, so just use the full fat version
//     res -= vdt::fast_logf( vdt::fast_expf(y) - 1.0f );
//   }

//   return res;
// }

float testOriginal( const float x )
{

  const auto xx    = x*x;
  const auto xxx   = xx*x;
  const auto xxxx  = xx*xx;
 
  return (-5.143220028201872 - 101.76227310588246*x - 127.31825068394369*xx + 
     317.31602891172594*xxx + 135.0567120741147*xxxx)/
    (1 + 40.31685825976414*x + 202.81583582446692*xx + 
     160.94697665742055*xxx - 2.088756969431516*xxxx);
}

float testOriginal( const float x1, const float x2 )
{
  const auto xx1    = x1*x1;
  const auto xxx1   = xx1*x1;
  const auto xxxx1  = xx1*xx1;
  
  const auto X = (-5.143220028201872 - 101.76227310588246*x1 - 127.31825068394369*xx1 + 
                  317.31602891172594*xxx1 + 135.0567120741147*xxxx1)/
    (1 + 40.31685825976414*x1 + 202.81583582446692*xx1 + 
     160.94697665742055*xxx1 - 2.088756969431516*xxxx1);
  
  const auto xx2    = x2*x2;
  const auto xxx2   = xx2*x2;
  const auto xxxx2  = xx2*xx2;
  
  const auto Y = (-5.143220028201872 - 101.76227310588246*x2 - 127.31825068394369*xx2 + 
                  317.31602891172594*xxx2 + 135.0567120741147*xxxx2)/
    (1 + 40.31685825976414*x2 + 202.81583582446692*xx2 + 
     160.94697665742055*xxx2 - 2.088756969431516*xxxx2);

  return X - Y;
}

float testEigen( const float x )
{
  typedef ::Eigen::Array4f V4f;

  const V4f x1( x,    x,    x,    x );
  const V4f x2( 1.0f, x,    x,    x );
  const V4f x3( 1.0f, 1.0f, x,    x );
  const V4f x4( 1.0f, 1.0f, 1.0f, x );
  const auto xxxx = x1 * x2 * x3 * x4;
  
  const V4f topP( -101.76227310588246f, -127.31825068394369f,
                  317.31602891172594f,  135.0567120741147f );
  const V4f botP(  40.31685825976414f,   202.81583582446692f,
                   160.94697665742055f,  -2.088756969431516f );
  return ( ( -5.143220028201872f + (topP*xxxx).sum() ) /
           ( 1.0f                + (botP*xxxx).sum() ) );

}

float testEigen( const float x, const float y )
{
  typedef ::Eigen::Array4f V4f;

  const V4f x1( x,    x,    x,    x );
  const V4f x2( 1.0f, x,    x,    x );
  const V4f x3( 1.0f, 1.0f, x,    x );
  const V4f x4( 1.0f, 1.0f, 1.0f, x );
  const auto xxxx = x1 * x2 * x3 * x4;
  
  const V4f topP( -101.76227310588246f, -127.31825068394369f,
                  317.31602891172594f,  135.0567120741147f );
  const V4f botP(  40.31685825976414f,   202.81583582446692f,
                   160.94697665742055f,  -2.088756969431516f );
  const auto X = ( ( -5.143220028201872f + (topP*xxxx).sum() ) /
                   ( 1.0f                + (botP*xxxx).sum() ) );

  const V4f y1( y,    y,    y,    y );
  const V4f y2( 1.0f, y,    y,    y );
  const V4f y3( 1.0f, 1.0f, y,    y );
  const V4f y4( 1.0f, 1.0f, 1.0f, y );
  const auto yyyy = y1 * y2 * y3 * y4;
  
  const auto Y = ( ( -5.143220028201872f + (topP*yyyy).sum() ) /
                   ( 1.0f                + (botP*yyyy).sum() ) );

  return X - Y;
}

// float testVc( const float x )
// {
//   Vc::float_v x1( x );
//   Vc::float_v x2( Vc::One ); x2[1] = x2[2] = x2[3] = x;
//   Vc::float_v x3( Vc::One );         x3[2] = x3[3] = x;
//   Vc::float_v x4( Vc::One );                 x4[3] = x;
//   const auto xxxx = x1 * x2 * x3 * x4;

//   Vc::float_v topP; 
//   topP[0] = -101.76227310588246f;
//   topP[1] = -127.31825068394369f;
//   topP[2] =  317.31602891172594f;
//   topP[3] =  135.0567120741147f;

//   Vc::float_v botP;
//   botP[0] = 40.31685825976414f;
//   botP[1] = 202.81583582446692f;
//   botP[2] = 160.94697665742055f;
//   botP[3] = -2.088756969431516f;

//   return ( ( -5.143220028201872f + (topP*xxxx).sum() ) /
//            ( 1.0f                + (botP*xxxx).sum() ) );
// }

// float testVectorClass4f( const float x )
// {

//   const Vec4f x1( x                   ); // all get x
//   const Vec4f x2( 1.0f, x,    x,    x );
//   const Vec4f x3( 1.0f, 1.0f, x,    x );
//   const Vec4f x4( 1.0f, 1.0f, 1.0f, x );
//   const auto xxxx = x1 * x2 * x3 * x4;

//   const Vec4f topP( -101.76227310588246f, -127.31825068394369f,
//                     317.31602891172594f,  135.0567120741147f );
//   const Vec4f botP(  40.31685825976414f,   202.81583582446692f,
//                      160.94697665742055f,  -2.088756969431516f );

//   return ( ( -5.143220028201872f + horizontal_add(topP*xxxx) ) /
//            ( 1.0f                + horizontal_add(botP*xxxx) ) );
// }

// float testVectorClass8f( const float x )
// {
//   const Vec8f x1( x                   ); // all get x
//   const Vec8f x2( 1.0f, x,    x,    x, 1.0f, x,    x,    x );
//   const Vec8f x3( 1.0f, 1.0f, x,    x, 1.0f, 1.0f, x,    x );
//   const Vec8f x4( 1.0f, 1.0f, 1.0f, x, 1.0f, 1.0f, 1.0f, x );

//   const auto S = x1 * x2 * x3 * x4 *
//     Vec8f( -101.76227310588246f, -127.31825068394369f,
//            317.31602891172594f,  135.0567120741147f,
//            40.31685825976414f,   202.81583582446692f,
//            160.94697665742055f,  -2.088756969431516f );
  
//   return ( ( -5.143220028201872f + horizontal_add(S.get_low())  ) /
//            ( 1.0f                + horizontal_add(S.get_high()) ) );
// }

// float testVectorClass8f2( const float x, const float y )
// {
//   const Vec8f x1( x,    x,    x,    x, y,    y,    y,    y ); 
//   const Vec8f x2( 1.0f, x,    x,    x, 1.0f, y,    y,    y );
//   const Vec8f x3( 1.0f, 1.0f, x,    x, 1.0f, 1.0f, y,    y );
//   const Vec8f x4( 1.0f, 1.0f, 1.0f, x, 1.0f, 1.0f, 1.0f, y );
//   const auto xxxx = x1 * x2 * x3 * x4;

//   const Vec8f topP( -101.76227310588246f, -127.31825068394369f,
//                     317.31602891172594f,  135.0567120741147f,
//                     -101.76227310588246f, -127.31825068394369f,
//                     317.31602891172594f,  135.0567120741147f );
//   const Vec8f botP(  40.31685825976414f,   202.81583582446692f,
//                      160.94697665742055f,  -2.088756969431516f,
//                      40.31685825976414f,   202.81583582446692f,
//                      160.94697665742055f,  -2.088756969431516f );

//   const auto top = topP * xxxx;
//   const auto bot = botP * xxxx;

//   return ( ( ( -5.143220028201872f + horizontal_add(top.get_low())  ) /
//              ( 1.0f                + horizontal_add(bot.get_low())  ) ) -
//            ( ( -5.143220028201872f + horizontal_add(top.get_high()) ) /
//              ( 1.0f                + horizontal_add(bot.get_high()) ) ) );
// }


std::vector<float> fValues;

std::vector< std::pair<float,float> > f2Values;

void testOriginal()
{
  std::cout << "Running original tests" << std::endl;
  float f = 0.0;
  for ( const auto& x : fValues  ) { f += testOriginal(x); }
  std::cout << "Value = " << f << std::endl;
}

void testOriginal2()
{
  std::cout << "Running original tests - two value" << std::endl;
  float f = 0.0;
  for ( const auto& x : f2Values ) { f += testOriginal(x.first,x.second); }
  std::cout << "Value = " << f << std::endl;
}

void testEigen()
{
  std::cout << "Running eigen tests" << std::endl;
  float f = 0.0;
  for ( const auto& x : fValues ) { f += testEigen(x); }
  std::cout << "Value = " << f << std::endl;
}

void testEigen2()
{
  std::cout << "Running eigen tests - two values" << std::endl;
  float f = 0.0;
  for ( const auto& x : f2Values ) { f += testEigen(x.first,x.second); }
  std::cout << "Value = " << f << std::endl;
}

// void testVc()
// {
//   std::cout << "Running Vc tests" << std::endl;
//   float f = 0.0;
//   for ( const auto& x : fValues ) { f += testVc(x); }
//   std::cout << "Value = " << f << std::endl;
// }

// void testVectorClass4f()
// {
//   std::cout << "Running VectorClass 4f tests" << std::endl;
//   float f = 0.0;
//   for ( const auto& x : fValues ) { f += testVectorClass4f(x); }
//   std::cout << "Value = " << f << std::endl;
// }

// void testVectorClass8f()
// {
//   std::cout << "Running VectorClass 8f tests" << std::endl;
//   float f = 0.0;
//   for ( const auto& x : fValues ) { f += testVectorClass8f(x); }
//   std::cout << "Value = " << f << std::endl;
// }

// void testVectorClass8f2()
// {
//   std::cout << "Running VectorClass 8f tests - two values" << std::endl;
//   float f = 0.0;
//   for ( const auto& x : f2Values ) { f += testVectorClass8f2(x.first,x.second); }
//   std::cout << "Value = " << f << std::endl;
// }

void testLogExpOriginal()
{
  std::cout << "Running full logexp original - two values" << std::endl;
  float f = 0.0;
  for ( const auto& x : f2Values ) { f += logExpOriginal(x.first,x.second); }
  std::cout << "Value = " << f << std::endl;
}

// void testLogExpVectorClass()
// {
//   std::cout << "Running full logexp VectorClass - two values" << std::endl;
//   float f = 0.0;
//   for ( const auto& x : f2Values ) { f += logExpVectorClass(x.first,x.second); }
//   std::cout << "Value = " << f << std::endl;
// }

int main ( int /*argc*/, char** /*argv*/ )
{

  // randomn generator
  static std::default_random_engine gen;
  static std::uniform_real_distribution<float> genValues(0.001,1.0);

  const unsigned int nValues = 1e7;
  std::cout << "Generating " << nValues << " test values" << std::endl;
  fValues.reserve(nValues);
  f2Values.reserve(nValues);
  for ( unsigned int i = 0; i < nValues; ++i ) 
  {
    const float x = genValues(gen);
    fValues.push_back(x);
    f2Values.push_back( std::make_pair(x,genValues(gen)) );
  }

  // testOriginal();
  // testEigen();
  // testVc();
  // testVectorClass4f();
  // testVectorClass8f();

  // testOriginal2();
  // testEigen2();
  // testVectorClass8f2();

  //testLogExpOriginal();
  //testLogExpVectorClass();

  return 0;
}
