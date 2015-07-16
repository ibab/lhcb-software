
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <boost/assign.hpp>

#define PHOENIX_LIMIT 6
#include <boost/spirit/home/classic.hpp>
#include <boost/spirit/home/phoenix.hpp>

void readT0s(const std::string& path, double t0s[3][4][4][9])
{
  // std::vector<std::string> stationNames;
  // std::vector<std::string> layerNames;
  // std::vector<std::string> quarterNames;
  // std::vector<std::string> moduleNames;

  std::vector<std::string> stationNames =  {"T1","T2","T3"};
  std::vector<std::string> layerNames = {"X1","U","V","X2"};
  std::vector<std::string> quarterNames = {"Q0","Q1","Q2","Q3"};
  std::vector<std::string> moduleNames = {"M1","M2","M3","M4","M5","M6","M7", "M8"};

  // {
  //   using namespace boost::assign;
  //   stationNames = list_of("T1")("T2")("T3");
  //   layerNames = list_of("X1")("U")("V")("X2");
  //   quarterNames = list_of("Q0")("Q1")("Q2")("Q3");
  //   moduleNames = list_of("M1")("M2")("M3")("M4")("M5")("M6")("M7")("M8")("M9");
  // }

  std::string prefix = "CalibrationModules";

  for(int s = 0; s < 3; s++) for(int l = 0; l < 4; l++) for(int q = 0; q < 4; q++)
  {
    std::string quarterId = stationNames[s] + layerNames[l] + quarterNames[q];

    std::string fileName = path + quarterId + "@" + prefix + ".xml";

    // load file with condition for specific station-layer-quarter
    std::ifstream file(fileName.c_str());
    if(file.fail())
    {
      continue;
    }

    std::string xml;

    // read file to string
    file.seekg(0, std::ios_base::end);
    xml.resize((unsigned int)file.tellg());
    file.seekg(0, std::ios_base::beg);
    file.read(&xml.at(0), xml.size());
    file.close();

    std::vector<double> cRt[9], cRtErr[9], cT0[9];

    {
      using namespace boost::spirit::classic;
      using namespace boost::phoenix;
      using namespace boost::phoenix::arg_names;

      int m; std::string param;

      rule<phrase_scanner_t> xmlRule =
        !("<?xml" >> *(anychar_p - '>') >> '>') // header: <?xml ... >
        >> !("<!DOCTYPE" >> *(anychar_p - '>') >> '>') // header <!DOCTYPE ... >
        >> "<DDDB>"
        >> "<catalog" >> *(anychar_p - '>') >> '>' // <catalog ... >
        >> +(
        lexeme_d[
          "<condition"
            >> *(
            (
            " name" >> *space_p >> '=' >> *space_p >> '\"'
            >> *(anychar_p - 'M') >> 'M' >> int_p[ref(m) = arg1 - 1]
          >> '\"'
            )
            | (anychar_p - '>')
            )
            >> '>' // <condition ... name="...M{m + 1}" ... >
        ]
      >> +(
        lexeme_d[
          "<paramVector"
            >> *(
            (
            " name" >> *space_p >> '=' >> *space_p >> '\"'
            >> (+(anychar_p - '\"'))[boost::phoenix::ref(param) = construct<std::string>(arg1, arg2)]
          >> '\"'
            )
            | (anychar_p - '>')
            ) >> '>' // <paramVector ... name="{param}" ... >
        ]
      >> +real_p // vector of values
        [
          if_(boost::phoenix::ref(param) == "TRParameters")
          [ boost::phoenix::push_back(boost::phoenix::ref(cRt)   [boost::phoenix::ref(m)], arg1) ],
          if_(boost::phoenix::ref(param) == "STParameters")
          [ boost::phoenix::push_back(boost::phoenix::ref(cRtErr)[boost::phoenix::ref(m)], arg1) ],
          if_(boost::phoenix::ref(param) == "TZero")
          [ boost::phoenix::push_back(boost::phoenix::ref(cT0)   [boost::phoenix::ref(m)], arg1) ]
        ]
      >> "</paramVector>"
        )[boost::phoenix::ref(param) = ""]
      >> "</condition>"
        )[boost::phoenix::ref(m) = -1]
      >> "</catalog>"
        >> "</DDDB>"
        >> end_p;

      if(parse(xml.c_str(), xmlRule, space_p).full != true) return;
    }

    for(int m = 0; m < 9; m++)
    {
      double t0 = cT0[m][0];
      t0s[s][l][q][m] = t0;
    }
  }
}

void readOTIS_T0s(const std::string& path, double t0s[3][4][4][9][4])
{
  // std::vector<std::string> stationNames;
  // std::vector<std::string> layerNames;
  // std::vector<std::string> quarterNames;
  // std::vector<std::string> moduleNames;

  std::vector<std::string> stationNames =  {"T1","T2","T3"};
  std::vector<std::string> layerNames = {"X1","U","V","X2"};
  std::vector<std::string> quarterNames = {"Q0","Q1","Q2","Q3"};
  std::vector<std::string> moduleNames = {"M1","M2","M3","M4","M5","M6","M7", "M8"};

  // {
  //   using namespace boost::assign;
  //   stationNames = list_of("T1")("T2")("T3");
  //   layerNames = list_of("X1")("U")("V")("X2");
  //   quarterNames = list_of("Q0")("Q1")("Q2")("Q3");
  //   moduleNames = list_of("M1")("M2")("M3")("M4")("M5")("M6")("M7")("M8")("M9");
  // }

  std::string prefix = "CalibrationModules";

  for(int s = 0; s < 3; s++) for(int l = 0; l < 4; l++) for(int q = 0; q < 4; q++)
  {
    std::string quarterId = stationNames[s] + layerNames[l] + quarterNames[q];

    std::string fileName = path + quarterId + "@" + prefix + ".xml";

    // load file with condition for specific station-layer-quarter
    std::ifstream file(fileName.c_str());
    if(file.fail())
    {
      continue;
    }

    std::string xml;

    // read file to string
    file.seekg(0, std::ios_base::end);
    xml.resize((unsigned int)file.tellg());
    file.seekg(0, std::ios_base::beg);
    file.read(&xml.at(0), xml.size());
    file.close();

    std::vector<double> cRt[9], cRtErr[9], cT0[9];

    {
      using namespace boost::spirit::classic;
      using namespace boost::phoenix;
      using namespace boost::phoenix::arg_names;

      int m; std::string param;

      rule<phrase_scanner_t> xmlRule =
        !("<?xml" >> *(anychar_p - '>') >> '>') // header: <?xml ... >
        >> !("<!DOCTYPE" >> *(anychar_p - '>') >> '>') // header <!DOCTYPE ... >
        >> "<DDDB>"
        >> "<catalog" >> *(anychar_p - '>') >> '>' // <catalog ... >
        >> +(
        lexeme_d[
          "<condition"
            >> *(
            (
            " name" >> *space_p >> '=' >> *space_p >> '\"'
            >> *(anychar_p - 'M') >> 'M' >> int_p[ref(m) = arg1 - 1]
          >> '\"'
            )
            | (anychar_p - '>')
            )
            >> '>' // <condition ... name="...M{m + 1}" ... >
        ]
      >> +(
        lexeme_d[
          "<paramVector"
            >> *(
            (
            " name" >> *space_p >> '=' >> *space_p >> '\"'
            >> (+(anychar_p - '\"'))[boost::phoenix::ref(param) = construct<std::string>(arg1, arg2)]
          >> '\"'
            )
            | (anychar_p - '>')
            ) >> '>' // <paramVector ... name="{param}" ... >
        ]
      >> +real_p // vector of values
        [
          if_(boost::phoenix::ref(param) == "TRParameters")
          [ boost::phoenix::push_back(boost::phoenix::ref(cRt)   [boost::phoenix::ref(m)], arg1) ],
          if_(boost::phoenix::ref(param) == "STParameters")
          [ boost::phoenix::push_back(boost::phoenix::ref(cRtErr)[boost::phoenix::ref(m)], arg1) ],
          if_(boost::phoenix::ref(param) == "TZero")
          [ boost::phoenix::push_back(boost::phoenix::ref(cT0)   [boost::phoenix::ref(m)], arg1) ]
        ]
      >> "</paramVector>"
        )[boost::phoenix::ref(param) = ""]
      >> "</condition>"
        )[boost::phoenix::ref(m) = -1]
      >> "</catalog>"
        >> "</DDDB>"
        >> end_p;

      if(parse(xml.c_str(), xmlRule, space_p).full != true) return;
    }
       for(int m = 0; m < 9; m++){  
	for (int a= 0; a < 4 ; a++){
	  double t0 = cT0[m][a];
	  t0s[s][l][q][m][a] = t0;
      }
    }
  }
}
