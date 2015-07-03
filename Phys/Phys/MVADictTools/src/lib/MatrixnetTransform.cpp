#include "MVADictTools/MatrixnetTransform.h"
#include "Utils.h"

#include <fstream>
#include <alloca.h>
#include <math.h>

// it works - don't touch it!
#define array(type, name, fml)                            \
  unsigned int name##_count = *(unsigned int const *)fml; \
  fml += sizeof(unsigned int);                            \
  type const *name = (type const *)fml;                   \
  fml = (char const *)(name + name##_count)

#define array_check(type, name, fml, limit, info)         \
  if(fml + sizeof(unsigned int) > limit) {                \
    info << "formula truncated" << std::endl;             \
    return false;                                         \
  }                                                       \
  unsigned int name##_count = *(unsigned int const *)fml; \
  fml += sizeof(unsigned int);                            \
  if(fml + name##_count * sizeof(type) > limit) {         \
    info << "formula truncated" << std::endl;             \
    return false;                                         \
  }                                                       \
  type const *name = (type const *)fml;                   \
  fml = (char const *)(name + name##_count)

unsigned int yabs_mx_check(char const *fml, size_t len, std::ostream& info) {
  char const *limit = fml + len;
  array_check(unsigned int, bf_counts, fml, limit, info);
  array_check(float, bfs, fml, limit, info);

  unsigned int bfs_check = 0;
  while(bf_counts_count--)
    bfs_check += *(bf_counts++);
  if(bfs_check != bfs_count) {
    info << "formula incorrect #1 " << std::endl;
    return 0;
  }

  array_check(unsigned int, classes, fml, limit, info);

  if(classes_count != 0) {
    info << "multiclassification not supported" << std::endl;
    return false;
  }

  array_check(unsigned int, nf_counts, fml, limit, info);
  array_check(unsigned int, ids, fml, limit, info);
  array_check(int, tbl, fml, limit, info);

  unsigned int ids_check = 0;
  unsigned int tbl_check = 0;

  unsigned int i;
  for(i = 0; i < nf_counts_count; ++i) {
    unsigned int nf_count = *(nf_counts++);
    ids_check += nf_count * (i + 1);
    tbl_check += nf_count * (1U << (i + 1));
  }

  if(ids_check != ids_count) {
    info << "formula incorrect #2" << std::endl;
    return false;
  }

  if(tbl_check != tbl_count) {
    info << "formula incorrect #3" << std::endl;
    return false;
  }

  while(ids_count--)
    if(*(ids++) >= bfs_count) {
      info << "formula incorrect #4" << std::endl;
      return false;
    }

  if(limit < fml + 2 * sizeof(double)) {
    info << "formula truncated" << std::endl;
    return false;
  }

  if(limit > fml + 2 * sizeof(double)) {
    info << "junk at the end of formula" << std::endl;
    return false;
  }

  return true;
}

double yabs_mx_apply(char const *fml, float const *fp) {
  array(unsigned int, bf_counts, fml);
  array(float, bfs, fml);

  unsigned int *vars = (unsigned int*)alloca(bfs_count * sizeof(unsigned int));
  unsigned int *vp = vars;

  while(bf_counts_count--) {
    unsigned int bf_count = *(bf_counts++);
    float f = *(fp++);

    while(bf_count--)
      *(vp++) = f > *(bfs++) ? 1 : 0;
  }

  fml += sizeof(unsigned int); /* classes_count == 0 */

  array(unsigned int, nf_counts, fml);
  array(unsigned int, ids, fml);
  array(int, tbl, fml);

  int64_t res = 0;

  unsigned int i;
  for(i = 0; i < nf_counts_count; ++i) {
    unsigned int nf_count = *(nf_counts++);
    while(nf_count--) {
      unsigned int idx = 0;

      unsigned int ii;
      for(ii = 0; ii <= i; ++ii)
        idx += (vars[*(ids++)] << ii);

      res += tbl[idx];
      tbl += (1U << (i + 1));
    }
  }

  double bias = *(double const *)(fml); fml += sizeof(double);
  double delta_mult = *(double const *)(fml);

  double mn_out = bias + res / delta_mult;
  return exp(mn_out) / (1. + exp(mn_out));
}

#undef array


MatrixnetTransform::MatrixnetTransform()
  : m_setup_success(false)
  , m_keep_all_vars(true)
  , m_matrixnet_file("")
  , m_name("")
  , m_formula("")
  , m_debug(false)
  , m_variables(0)
{
  return;
}

MatrixnetTransform::~MatrixnetTransform() {}


bool
MatrixnetTransform::Init(const optmap& options, std::ostream& info, const bool debug ){
  m_debug = debug;
  m_setup_success = parseOpts(options, info);
  if (!m_setup_success) { return false ; }

  readWeightsFile(info);
  if (!m_setup_success) { return false ; }

  return true;
}


bool
MatrixnetTransform::operator()(const DICT& in, DICT& out) const {
  // Check initialization
  if (!m_setup_success) {
    return false;
  }

  std::vector<float> values(m_variables.size()); // vector with correct size
  std::vector<float>::iterator value = values.begin();
  std::vector<std::string>::const_iterator varname = m_variables.begin();
  for (; value!=values.end(); ++varname, ++value) { *value = (float)in[*varname]; }
  if (m_keep_all_vars) { out = in; }
  else { out.clear(); }
  out.insert(out.end(), m_name, yabs_mx_apply(m_formula.c_str(), &(values[0])));
  return true;
}


bool
MatrixnetTransform::checkWeightsFile(std::ostream& info) {
  std::ifstream fin(resolveEnv(m_matrixnet_file).c_str());
  // Check existence of WeightFile: locally
  if (fin.good()) {
    return true;
  }
  fin.close();
  // else ERROR
  if (m_matrixnet_file == "") {
    info << "ERROR  ";
    info << "WeightFile not given." << std::endl;
  } else {
    info << "WeightFile \"" << m_matrixnet_file << "\" not found." << std::endl;
    info << "  MX will not be run.  The output will be 0 for each event." << std::endl;
  }
  return false;
}


void MatrixnetTransform::readWeightsFile(std::ostream& info) {
  if (!checkWeightsFile(info)) {
    m_setup_success = false;
    return;
  }
  std::ifstream fin(resolveEnv(m_matrixnet_file).c_str(), std::ios::in | std::ios::binary);
  std::ostringstream oss;
  oss << fin.rdbuf();
  std::string temp(oss.str());
  const char * data = temp.c_str();
  int factors_number = *(reinterpret_cast<const int*>(data));
  data += sizeof(const int);
  for (int i = 0; i < factors_number; ++i) {
    int factor_length = *(reinterpret_cast<const int*>(data));
    data += sizeof(const int);
    m_variables.push_back(std::string(data, data + factor_length));
    data += factor_length;
  }
  int formula_length = *(reinterpret_cast<const int*>(data));
  data += sizeof(const int);
  m_formula = std::string(data, data + formula_length);

  if (!yabs_mx_check(m_formula.c_str(), m_formula.length(), info)) {
    m_setup_success = false;
    return;
  }
  fin.close();
  return;
}


bool MatrixnetTransform::parseOpts(const optmap& options, std::ostream& info) {
  bool pass = true;
  Options parse(options);
  parse.add<std::string>("Name", "Name of output branch (Required)",
                         m_name, info);
  parse.add<std::string>("MatrixnetFile", "File with matrixnet formula",
                         m_matrixnet_file, info);
  parse.add<bool>("KeepVars", "Keep input variables, \"1\" or \"0\"",
                  m_keep_all_vars, info, false);
  pass = parse.check(info);
  return pass;
}
