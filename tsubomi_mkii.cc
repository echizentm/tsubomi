// $Id$
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include "tsubomi.h"
#include "tsubomi_vertical_code.h"

// tsubomi_mkii: tsubomi make inverted index
using namespace std;
using namespace tsubomi;
int main(int argc, char **argv) {
  try {
   // read command line parameters
    const char *textname = "";
    bool        is_help  = false;
    for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {
        if (argv[i][1] == '\0') { continue; }
        switch (argv[i][1]) {
          case 'h': is_help = true; break;
        }
      } else {
        textname = argv[i];
      }
    }

    // write helo message and exit
    if (textname[0] == '\0' || is_help) {
      cout << "[USAGE] tsubomi_mkii [params] textfile" << endl;
      cout << "[OPTIONS]" << endl;
      cout << "  h   : print help message." << endl;
      return 0;
    }

    // read textfile and make inverted index
    string indexname = string(textname) + ".index";
    string labelname = string(textname) + ".label";

    std::ifstream ifs;
    ifs.open(textname, std::ios::in);
    if (!ifs) { throw "error at main(). file cannot open for read."; }
    std::ofstream ofs;
    ofs.open(labelname.c_str(), std::ios::out | std::ios::trunc);
    if (!ofs) { throw "error at main(). file cannot open for write."; }

    vector<ullong> nums;
    ullong         num       = 0;
    ullong         id        = 0;
    bool           is_append = false;
    bool           is_label  = true;
    char           ch;
    ifs.get(ch);
    while (!ifs.eof()) {
      if (is_label) {
        string label = "";
        while (ch != ' ') {
          if (ch == '\n') { throw "error at main(). textfile is invalid."; }
          label += ch;
          ifs.get(ch);
        }
        ofs << label << " " << id << endl;
        id++;
        is_label = false;
      }
      if ('0' <= ch && ch <= '9') {
        num = num * 10 + (ch - '0');
      } else {
        if (num > 0) { nums.push_back(num); num = 0; }
        if (ch == '\n') {
          sort(nums.begin(), nums.end());
          ullong        prev = 0;
          vertical_code vc;

          vector<ullong>::iterator it = nums.begin();
          while (it != nums.end()) {
            vc.push(*it - prev);
            prev = *it;
            it++;
          }
          vc.write(indexname.c_str(), is_append);
          is_append = true;
          nums.clear();
          is_label = true;
        }
      }
      ifs.get(ch);
    }
    ifs.close();
    ofs.close();

    // mkary for *.label
    indexer tbm(labelname.c_str());
    tbm.mkary("\n");
  } catch (const char *err) {
    cerr << err << endl;
    return 1;
  }
  return 0;
}
