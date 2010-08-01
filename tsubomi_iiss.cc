// $Id$
#include <iostream>
#include <string>
#include <vector>
#include "tsubomi.h"
#include "tsubomi_vertical_code.h"

// tsubomi_iiss: tsubomi inverted index simple search
using namespace std;
using namespace tsubomi;
int main(int argc, char **argv) {
  try {
   // read command line parameters
    const char *textname = "";
    const char *keyword  = "";
    bool        is_help  = false;
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
        if (argv[i][1] == '\0') { continue; }
        switch (argv[i][1]) {
          case 'h': is_help = true; break;
        }
      } else {
        if (textname[0] == '\0') {
          textname = argv[i];
        } else {
          keyword  = argv[i];
        }
      }
    }

    // write helo message and exit
    if (keyword[0] == '\0' || is_help) {
      cout << "[USAGE] tsubomi_iiss [params] textfile keyword" << endl;
      cout << "[OPTIONS]" << endl;
      cout << "  h   : print help message." << endl;
      return 0;
    }

    // search inverted index
    string indexname = string(textname) + ".index";
    vector<vertical_code *> vcs;
    read_vertical_codes(indexname.c_str(), vcs);

    string labelname = string(textname) + ".label";
    searcher tbm(labelname.c_str());
    char buf[1024];
    bool ret = tbm.get_value(keyword, buf, 1024, "\n");
    if (!ret) {
      cout << "'" << keyword << "' not found." << endl;
      clear_vertical_codes(vcs);
      return 0;
    }

    int i = 0;
    while (buf[i] != ' ') { i++; }
    i++;
    ullong id = 0;
    while ('0' <= buf[i] && buf[i] <= '9') {
      id = id * 10 + (buf[i] - '0'); i++;
    }

    vertical_code *pvc = vcs[id];
    for (ullong i = 0; i < pvc->size(); i++) {
      cout << pvc->get(i) << " ";
    }
    cout << endl;
    clear_vertical_codes(vcs);
  } catch (const char *err) {
    cerr << err << endl;
    return 1;
  }
  return 0;
}
