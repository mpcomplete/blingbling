#include "app.h"

App* app;

int main(int argc, char* argv[]) {
  app = new App;
  return app->run();
}
