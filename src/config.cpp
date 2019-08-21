#include "config.h"
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <fstream>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <cstdlib>

std::string home_dir() {
    return std::string(getenv("USERPROFILE"));
}
#else           // Linux, MacOS, etc...

#include <unistd.h>
#include <pwd.h>

std::string home_dir() {
  char *homedir = getenv("HOME");

  if (homedir == nullptr) {
    homedir = getpwuid(getuid())->pw_dir;
  }

  return std::string(homedir);
}

#endif

const char *CONFIG_LOOKUP_PATHS[3] = {
    "~/.todo/cdo.ini",
    "~/.cdo.ini",
    "/etc/todo/cdo.ini"
};

string join(string a, string b) {
  if (a.empty()) {
    return b;
  }

  if (b.empty()) {
    return a;
  }

  string sep = "/";

  if (a[a.length() - 1] == '/') {
    sep = "";
  }

  return a + sep + (b[0] == '/' ? b.substr(1, b.length() - 1) : b);
}

bool try_file(ifstream &is, const string &path) {
  is.open(path);
  return is.good();
}

namespace bpt = boost::property_tree;

namespace cdo {
config::config(const string &conf_path) {
  ifstream conf_file;

  if (!conf_path.empty()) {
    if (!try_file(conf_file, resolve(conf_path))) {
      cerr << "invalid or missing config file at " << conf_path << endl;
      exit(1);
    }
  } else {
    for (auto &i : CONFIG_LOOKUP_PATHS) {
      if (try_file(conf_file, resolve(i))) {
        break;
      }
    }
  }

  if (conf_file) {
    bpt::ptree pt;

    try {
      bpt::ini_parser::read_ini(conf_file, pt);
    } catch (bpt::ini_parser_error &e) {
      cerr << "failed to parse config file: invalid syntax on line " << e.line() << " - " << e.message()
           << endl;
      exit(1);
    }

    todo_dir = pt.get<string>("general.todo_dir", todo_dir);
    todo_file = pt.get<string>("general.todo_file", todo_file);
    done_file = pt.get<string>("general.done_file", done_file);
    report_file = pt.get<string>("general.report_file", report_file);
    single_file = pt.get<bool>("general.use_single_file", single_file);
    report_enabled = pt.get<bool>("general.enable_reports", report_enabled);
  }
}

string config::resolve(string base, string other) {
  if (base[0] == '~') {
    string home = home_dir();
    base = join(home, base.substr(2, base.length() - 2));
  }

  if (!other.empty()) {
    return join(base, move(other));
  }

  return base;
}
}