#include "erl_yy_driver.h"
#include <cstring>
#include <fstream>

int yy_init = 1;
int yy_start = 0;
std::istream& yyin = std::cin;
std::ostream& yyout = std::cout;

ErlangDriver::ErlangDriver() : trace_scanning_(false), trace_parsing_(false) {
  variables_["one"] = 1;
  variables_["two"] = 2;
}

ErlangDriver::~ErlangDriver() {}

int ErlangDriver::parse(const std::string& f) {
  file_ = f;
  //  scan_begin();

  std::ifstream ifs;
  ifs.open("../e4compile/priv/gb_trees.erl");

  lexer_ = std::make_unique<ErlangLexer>(&ifs);
  parser_ = std::make_unique<ErlangParser>(*this);

  parser_->set_debug_level(trace_parsing_);
  int res = parser_->parse();
  //  scan_end();

  ifs.close();
  return res;
}

void ErlangDriver::error(const yy::location& l, const std::string& m) {
  std::cerr << l << ": " << m << std::endl;
}

void ErlangDriver::error(const std::string& m) {
  std::cerr << m << std::endl;
}

