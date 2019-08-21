#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <csignal>
#include "config.h"
#include "td/todo.h"
#include "hook.h"
#include "term.h"
#include "ui.h"

namespace po = boost::program_options;

std::unique_ptr<cdo::term> term;

void exit_handler() {
  // for cases when app terminates unexpectedly
  endwin();
}

void resize_handler(int) {
  term->resize();
}

int main(int argc, char *argv[]) {
  po::options_description desc("Command options");
  std::string confp, filep;

  desc.add_options()
      ("help,h", "Show help")
      ("conf,c", po::value<std::string>(&confp), "Custom config file location")
      ("file,f", po::value<std::string>(&filep), "Work with single specified file");

  po::variables_map vm;
  po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
  po::store(parsed, vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 0;
  }

  cdo::config conf(confp);

  if (!filep.empty()) {
    conf.singleFileSetup(filep);
  }

  cdo::td::todo td(conf);
  term = std::make_unique<cdo::term>();

  term->defineColors()
      ("highlighted_item", COLOR_CYAN)
      ("focused_window", -1, COLOR_BLUE);

  cdo::ui::task_list task_list(&td, term.get());

  std::signal(SIGWINCH, resize_handler);
  std::atexit(exit_handler);

  term->run();

  return 0;
}