#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "futaba_predictor.h"

using namespace std;
using namespace futaba;
int main(int argc, char **argv) {
  if (argc < 2) {
    cerr << "[USAGE]: futaba *.tfidf [results] < input > output";
    return 0;
  }
  int n = 10;
  if (argc >= 3) { n = atoi(argv[2]); }

  try {
    predictor p(argv[1]);

    char   *buf = NULL;
    size_t len  = 0;
    while (getline(&buf, &len, stdin) != -1) {
      buf[strlen(buf) - 1] = '\0';
      data d(buf);
      cout << d.c_;

      char sep = '\t';
      result_type r = p.predict(d, n);
      result_type::iterator i = r.begin();
      result_type::iterator e = r.end();
      while (i != e) {
        cout << sep << i->first << ":" << i->second;
        sep = ',';
        i++;
      }
      cout << endl;
    }
    if (buf) { free(buf); }
  } catch (const char *err) {
    cerr << err << endl;
    return 1;
  }
  return 0;
}
