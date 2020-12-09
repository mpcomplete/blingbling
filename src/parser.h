#ifndef _PARSER_H
#define _PARSER_H

// Handles parsing of config files.  Callback ideas taken from libgfx by
// Michael Garland.

#include "main.h"
#include "utils.h"

#include <map>
#include <string>
#include <vector>
#include "pgfilearchive.h"

class ParserCmd {
 public:
  ParserCmd(string line) { create_from_line(line); }
  string get_line() const { return line; }
  int num_args() const { return words.size() - 1; }
  string get_command() const { return words[0]; }
  string get_str_arg(int n) const { return words[n]; }
  int get_int_arg(int n) const { return to_int(words[n]); }
  double get_double_arg(int n) const { return to_double(words[n]); }
  int error() const { return _error; }

 private:
  void create_from_line(string line);

  int _error;           /**< the error code (0 if none) */
  vector<string> words; /**< the list of arguments */
  string line;
};

class Parser {
 public:
  enum Error {
    ERR_NONE = 0,
    ERR_FILE_OPEN,
    ERR_SYNTAX,
    ERR_BAD_COMMAND,
    ERR_EMPTY,
  };

  typedef int Callback(const ParserCmd&);

  struct CmdHandler {
    virtual int operator()(const ParserCmd& cmd) = 0;
  };

  struct CmdFunction : public CmdHandler {
    Callback* cb;
    CmdFunction(Callback* cb0) : cb(cb0) {}
    virtual int operator()(const ParserCmd& cmd) { return (*cb)(cmd); }
  };

  template <class T>
  struct CmdMethod : public CmdHandler {
    typedef int (T::*MethodCallback)(const ParserCmd&);
    T* obj;
    MethodCallback cb;

    CmdMethod(T* obj0, MethodCallback cb0) : obj(obj0), cb(cb0) {}
    virtual int operator()(const ParserCmd& cmd) { return (obj->*cb)(cmd); }
  };

  // creates a parser after opening filename (uses OpenFile, so it can be
  // archived)
  Parser(string filename, PG_OPEN_MODE mode = PG_OPEN_READ);
  // creates a parse using the given file.
  Parser(PG_File* file);
  ~Parser();

  /**
   * sets up a handler for a command
   * @param command the command to register
   * @param handler the handler to use when command is read.
   */
  void set_handler(string command, CmdHandler* handler) {
    handlers[command] = handler;
  }

  void set_handler(string command, Callback* cb) {
    set_handler(command, new CmdFunction(cb));
  }

  template <class T>
  void set_handler(string command, T* obj, int (T::*cb)(const ParserCmd&)) {
    set_handler(command, new CmdMethod<T>(obj, cb));
  }

  /**
   * parses the entire file
   * @return 0 on success, last error code if error
   */
  int parse_file();
  /**
   * parse a single line
   * @return 0 on success, error code on error
   */
  int parse_line(string line);

  void write_line(string line) { this->putline(line); }

 private:
  typedef map<string, CmdHandler*> HandlerMap;

  int process_command(const ParserCmd& cmd);
  CmdHandler* lookup_handler(string command) { return handlers[command]; }
  void open(string filename, PG_OPEN_MODE mode = PG_OPEN_READ);
  string getline();
  void putline(string line);
  bool eof();

  // gets a word from 'line', starting at 'pos'.  A word stops when a
  // character from 'stop_at' is reached.  'pos' is updated to the
  // position where 'stop_at' was reached. if subst==true, apply variable
  // substitution to the word.
  string getword(string line, char* stop_at, size_t* pos, bool subst = false);

  void print_error(int errnum, string str);

  PG_File* file;       /**< the file we're parsing */
  string filename;     /**< the file's name */
  int line_num;        /**< the line number we're on */
  HandlerMap handlers; /**< map of command names to their handlers */
};

#endif  // header guard
