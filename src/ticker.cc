#include "ticker.h"

Ticker::List Ticker::ticker_list;

Ticker::Ticker(const Ticker& ticker) : entry(ticker.entry) {
  ticker.entry = ticker_list.end();
}

void Ticker::start() {
  if (is_running()) {
    return;
  }

  ticker_list.push_front(this);
  entry = ticker_list.begin();
}

void Ticker::stop() {
  if (!is_running()) {
    return;
  }

  ticker_list.erase(entry);
  entry = ticker_list.end();
}

const Ticker& Ticker::operator=(const Ticker& ticker) {
  this->stop();
  entry = ticker.entry;
  ticker.entry = ticker_list.end();

  return *this;
}
