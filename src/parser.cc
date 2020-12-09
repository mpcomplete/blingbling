#include "parser.h"
#include "app.h"

// C string functions
#include <string.h>

class StoreFunc {
 public:
  PG_File* file;
  StoreFunc(PG_File* ifile) : file(ifile) {}
  void operator()(string name, string key, string value) {
    file->write(name);
    if (!key.empty()) {
      file->write("[");
      file->write(key);
      file->write("]");
    }
    file->write(" = ");
    file->write(value);
    file->write("\n");
  }
};

/**
 * Split a string into a vector using delims
 * @param str the string to split, ex: "this is 'a string'"
 * @param delims the separators, ex: " \t"
 * @param quotes a possible set of characters which surround multiple words
 * to mark them as a single token, ex: "'"
 * @param words the pointer to vector to receive new tokens, ex (after
 * calling): {"this", "is", "a string"}
 * @returns 0 if no error, errnum if error.
 */
int strsplit(string str, string delims, string quotes, vector<string>* words) {
  size_t beg = 0, end = 0, q = 0;
  int errcode = 0;

  beg = str.find_first_not_of(delims);

  while (beg != string::npos) {
    if ((q = quotes.find(str[beg])) != string::npos) {
      beg++;
      if ((end = str.find(quotes[q], beg)) == string::npos) {
        errcode = 1;
      }
      words->push_back(str.substr(beg, end - beg));
      beg = str.find_first_not_of(delims, end == string::npos ? end : end + 1);
    } else {
      end = str.find_first_of(delims, beg);
      words->push_back(str.substr(beg, end - beg));
      beg = str.find_first_not_of(delims, end);
    }
  }
  return errcode;
}

void ParserCmd::create_from_line(string line_in) {
  line = line_in;
  if (strsplit(line, " \t\n", "\"", &words) != 0) {
    _error = Parser::ERR_SYNTAX;
    return;
  }

  if (words.size() < 1) {
    _error = Parser::ERR_EMPTY;
    return;
  }

  _error = Parser::ERR_NONE;
}

Parser::Parser(string ifilename, PG_OPEN_MODE mode)
    : filename(ifilename), line_num(0) {
  this->open(filename, mode);
}

Parser::Parser(PG_File* ifile) : file(file), filename(""), line_num(0) {}

Parser::~Parser() {
  if (file)
    delete file;
  for (HandlerMap::iterator it = handlers.begin(); it != handlers.end(); it++) {
    CmdHandler* h = (*it).second;
    delete h;
  }
}

int Parser::parse_file() {
  int retval = ERR_NONE, tmp;
  string line;

  if (!file) {
    print_error(ERR_FILE_OPEN, "");
    return ERR_FILE_OPEN;
  }

  while (!this->eof()) {
    line = this->getline();
    if ((tmp = this->parse_line(line)) != ERR_NONE) {
      retval = tmp;
      print_error(retval, line);
    }
  }

  return retval;
}

int Parser::parse_line(string real_line) {
  string::size_type com = real_line.find('#');
  string line = real_line.substr(0, com);

  ParserCmd cmd(line);

  switch (cmd.error()) {
    case ERR_EMPTY:  // empty lines are ok
      return ERR_NONE;
      break;
    case ERR_NONE:
      break;
    default:
      print_error(cmd.error(), real_line);
      return cmd.error();
      break;
  }

  return process_command(cmd);
}

int Parser::process_command(const ParserCmd& cmd) {
  CmdHandler* handler = this->lookup_handler(cmd.get_command());
  if (!handler) {
    if ((handler = this->lookup_handler("")) == NULL) {
      return 0;
    }
  }
  return (*handler)(cmd);
}

void Parser::open(string filename, PG_OPEN_MODE mode) {
  file = app->OpenFile(filename.c_str(), mode);
  if (!file) {
    PG_LogERR("couldn't open file: %s", filename.c_str());
    return;
  }
}

string Parser::getline() {
  assert(file);
  line_num++;
  return file->getline();
}

void Parser::putline(string line) {
  assert(file);
  file->putline(line);
}

bool Parser::eof() {
  assert(file);
  return file->eof();
}

void Parser::print_error(int errnum, string str) {
  switch (errnum) {
    case ERR_FILE_OPEN:
      PG_LogERR("file isn't open: %s", filename.c_str());
      break;
    case ERR_SYNTAX:
      PG_LogERR("%s:%d: syntax error with line %s", filename.c_str(), line_num,
                str.c_str());
      break;
    case ERR_BAD_COMMAND:
      // we don't care about bad commands.
#if 0
	PG_LogERR("%s:%d: bad command %s",
		filename.c_str(), line_num, str.c_str());
#endif
      break;
    default:
      PG_LogERR("%s:%d: error %d with line %s", filename.c_str(), line_num,
                errnum, str.c_str());
      break;
  }
}
