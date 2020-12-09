#ifndef _SCORELIST_H
#define _SCORELIST_H

#include "main.h"

#include "parser.h"

#include <string>
#include <vector>

#define NUM_HIGHSCORES 10

struct Score {
  string name;
  int score;
  Score() {}
  Score(string n, int s) : name(n), score(s) {}
};

// high score container for each number-of-types setting
// ie, a game with only 4 tile types will have a different high score list
// than a game with 5 types.
class ScoreList {
 public:
  typedef vector<Score> Container;
  typedef Container::iterator iterator;

  ScoreList();

  // check if score is a high score
  bool is_high(int score);

  // insert a new high score into the list
  void insert(string name, int score);

  // load or save the high scores list
  void load();
  void save();

  iterator begin(int n) { return scores[n].begin(); }
  iterator end(int n) { return scores[n].end(); }

  // return the scorelist for the current number of types
  Container& curscores();

 private:
  int cmd_version(const ParserCmd& cmd);

  // which is called depends on whether the version is <1.1 or not.
  int cmd_score(const ParserCmd& cmd);
  int cmd_score_old(const ParserCmd& cmd);

  double version;  // the scorelist format version (used during load())
  vector<Container> scores;
};

#endif  // header guard
