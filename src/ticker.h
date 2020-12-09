#ifndef _TICKER_H
#define _TICKER_H

#include <list>
#include "main.h"

/**
 * anything that is capable of ticking will derive from this
 */
class Ticker {
 public:
  typedef list<Ticker*> List;
  typedef List::iterator iterator;

  Ticker() : entry(ticker_list.end()) {}
  Ticker(const Ticker& ticker);  ///< gotta swap ownership
  virtual ~Ticker() { this->stop(); }

  virtual void tick(int ms) = 0;  ///< advance ms milliseconds
  virtual void start();           ///< start being updated
  virtual void stop();            ///< stop being updated
  bool is_running() { return (entry != ticker_list.end()); }

  const Ticker& operator=(const Ticker& ticker);

  static List ticker_list;

 private:
  mutable List::iterator entry;
};

#endif  // header guard
