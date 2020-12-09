#include "scorelist.h"
#include "parser.h"
#include "prefs.h"
#include "utils.h"

ScoreList::ScoreList() : version(1.0) {}

bool ScoreList::is_high(int score) {
  if (curscores().size() < NUM_HIGHSCORES) {
    return true;
  }

  for (iterator it = curscores().begin(); it != curscores().end(); it++) {
    if (score > (*it).score) {
      return true;
    }
  }
  return false;
}

void ScoreList::insert(string name, int score) {
  iterator it;
  for (it = curscores().begin(); it != curscores().end(); it++) {
    if (score > (*it).score) {
      break;
    }
  }
  curscores().insert(it, Score(name, score));
  if (curscores().size() > NUM_HIGHSCORES) {
    curscores().resize(NUM_HIGHSCORES);
  }
}

void ScoreList::load() {
  scores.resize(prefs.num_types.max + 1);

  Parser parser(HIGHSCORES_CONF);
  parser.set_handler("version", this, &ScoreList::cmd_version);
  parser.set_handler("highscore", this, &ScoreList::cmd_score);
  // unknown commands are sent to the old scorelist handler
  parser.set_handler("", this, &ScoreList::cmd_score_old);
  parser.parse_file();
}

void ScoreList::save() {
  Parser parser(HIGHSCORES_CONF, PG_OPEN_WRITE);

  parser.write_line("version 1.1");

  for (size_t level = 0; level < scores.size(); level++) {
    for (size_t i = 0; i < scores[level].size(); i++) {
      parser.write_line("highscore " + to_string(level) + " " + to_string(i) +
                        " \"" + scores[level][i].name + "\" " +
                        to_string(scores[level][i].score));
    }
  }
}

ScoreList::Container& ScoreList::curscores() {
  return scores[prefs.num_types.val];
}

// version <version string>
int ScoreList::cmd_version(const ParserCmd& cmd) {
  if (cmd.num_args() != 1) {
    return Parser::ERR_SYNTAX;
  }

  version = cmd.get_double_arg(1);

  return 0;
}

// highscore <difficulty level> <rank> <name> <score>
int ScoreList::cmd_score(const ParserCmd& cmd) {
  if (cmd.num_args() < 4) {
    return Parser::ERR_SYNTAX;
  }

  size_t level = cmd.get_int_arg(1);
  size_t rank = cmd.get_int_arg(2);
  string name = cmd.get_str_arg(3);
  int score = cmd.get_int_arg(4);

  if (rank + 1 > scores[level].size()) {
    scores[level].resize(rank + 1);
  }
  scores[level][rank] = Score(name, score);

  return 0;
}

//          pos1              pos2       pos3  pos4
// highscore<difficulty level>[<rank>] = <name>:<score>
int ScoreList::cmd_score_old(const ParserCmd& cmd) {
  if (version > 1.0) {
    return Parser::ERR_SYNTAX;
  }

  string line = cmd.get_line();
  size_t pos1 = line.find_first_of("0123456789");
  size_t pos2 = line.find_first_of('[', pos1);
  size_t pos3 = line.find_first_of('=', pos2);
  pos3 = line.find_first_not_of("= \t", pos3);
  size_t pos4 = line.find_first_of(':', pos3);

  if (pos1 == string::npos || pos2 == string::npos || pos3 == string::npos ||
      pos4 == string::npos) {
    return Parser::ERR_SYNTAX;
  }
  int level = atoi(line.c_str() + pos1);
  int rank = atoi(line.c_str() + pos2 + 1);

  string name = line.substr(pos3, pos4 - pos3);
  int score = atoi(line.c_str() + pos4 + 1);

  if (rank + 1 > scores[level].size()) {
    scores[level].resize(rank + 1);
  }
  scores[level][rank] = Score(name, score);

  return 0;
}
