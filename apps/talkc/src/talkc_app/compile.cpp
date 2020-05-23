#include "talkc/entry.h"
#include "compileus/libtalk/lexer.h"
#include "compileus/libtalk/parser.h"

#include <iterator>
#include <sstream>


namespace talkc {

int compile(const Options& opts, std::istream& input, std::ostream& output) {
  (void)opts;
  (void)output;

  std::noskipws(input);
  
  // The talk compiler expects to operator on a contiguous buffer, so we read all the input 
  // at once.
  std::vector<char> all_data{
      std::istream_iterator<char>(input),
      std::istream_iterator<char>()
  };

  auto tokens = compileus::tokenize({all_data.data(), all_data.size()});
  auto current = tokens.cbegin();
  
  try {
      auto module = compileus::parse_module(current);
  }
  catch(compileus::Parse_error& e) {
      std::ostringstream err_writer;

      err_writer << "Compilation error: " << e.what() << "\n";
      err_writer << e.get_location_string() << "\n";

      opts.log_callback(Log_level::error, err_writer.str());
  }

  return 0;
}

}  // namespace talkc