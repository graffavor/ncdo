#define BOOST_SPIRIT_UNICODE
#include <iostream>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_optional.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/fusion/include/vector.hpp>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <algorithm>
#include "td/todo.h"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::standard_wide;
namespace ph = boost::phoenix;
namespace fusion = boost::fusion;

namespace cdo {
namespace td {
template<typename Iterator>
bool parse_task(Iterator first, Iterator last, task &t) {
  using qi::lexeme;
  using qi::_1;
  using qi::no_skip;

  fusion::vector<int, int, int> cr, cp;
  vector<wchar_t> tmp;

  auto r = qi::phrase_parse(
      first,
      last,
      (
          -(qi::lit("x")[ph::ref(t.done) = true])
              >> -(lexeme[qi::int_ >> '-' >> qi::int_ >> '-' >> qi::int_][ph::ref(cp) = _1])
              >> -(lexeme['(' >> ascii::alpha[ph::ref(t.priority) = _1] >> ')'])
              >> -(lexeme[qi::int_ >> '-' >> qi::int_ >> '-' >> qi::int_][ph::ref(cr) = _1])
              >> no_skip[lexeme[+(ascii::char_)][ph::ref(tmp) = _1]]
      ),
      ascii::space
  );

  if (fusion::at_c<0>(cr) != 0) {
    t.created = bdt::date(fusion::at_c<0>(cr), fusion::at_c<1>(cr), fusion::at_c<2>(cr));
  }

  if (fusion::at_c<0>(cp) != 0) {
    t.completed = bdt::date(fusion::at_c<0>(cp), fusion::at_c<1>(cp), fusion::at_c<2>(cp));
  }

  if (r) {
    t.desc = wstring(tmp.begin(), tmp.end());
  }

  return r;
}

wstring format_task(const task &t) {
  wstringstream ss;

  if (t.done) {
    ss << "x ";
  }

  if (!t.completed.is_not_a_date()) {
    ss << t.completed.year() << '-'
       << std::setfill(L'0') << std::setw(2) << t.completed.month().as_number() << '-'
       << std::setfill(L'0') << std::setw(2) << t.completed.day() << ' ';
  }

  if (t.priority > 0) {
    ss << "(" << t.priority << ") ";
  }

  if (!t.created.is_not_a_date()) {
    ss << t.created.year() << '-'
       << std::setfill(L'0') << std::setw(2) << t.created.month().as_number() << '-'
       << std::setfill(L'0') << std::setw(2) << t.created.day() << ' ';
  }

  ss << t.desc;

  return ss.str();
}

wostream &operator<<(wostream &os, const task &t) {
  os << format_task(t);
  return os;
}

todo::todo(cdo::config &c) {
  todo_file = c.getTodoFile();
  done_file = c.getDoneFile();
  report_file = c.getReportFile();

  std::wifstream ifile(todo_file, ios_base::in);

  if (!ifile) {
    // no reason try to open other files if we cannot read todo file in single file mode
    if (c.createIfNotExists()) {
      return;
    }

    std::cerr << "cannot open todo.txt file" << std::endl;
    exit(1);
  }

  ifile.imbue(std::locale(std::locale(""), new std::codecvt_utf8<wchar_t>));

  unsigned int pos = 0;

  wstring tmp;
  while (std::getline(ifile, tmp)) {
    task t;
    if (parse_task(tmp.begin(), tmp.end(), t)) {
      t.rel_pos = pos++;
      tasks_.push_back(t);
    }
    tmp.clear();
  }
  ifile.close();

  if (!done_file.empty() && done_file != todo_file) {
    ifile.open(done_file, ios_base::in);
    if (!ifile) {
      std::cerr << "cannot open done.txt file" << std::endl;
      exit(1);
    }

    ifile.imbue(std::locale(std::locale(""), new std::codecvt_utf8<wchar_t>));

    while (std::getline(ifile, tmp)) {
      task t;
      if (parse_task(tmp.begin(), tmp.end(), t)) {
        t.rel_pos = pos++;
        tasks_.push_back(t);
      }
      tmp.clear();
    }
  } else {
    done_in_todo = true;
  }

  for (auto &i: tasks_) {
    extract_task_fields(i.desc);
  }
}

todo::~todo() {
}

void todo::setComplete(const task &t, bool value) {
  for (auto &i : tasks_) {
    if (i == t) {
      i.done = value;
      is_changed = true;
    }
  }
}

void todo::setPriority(const task &t, char value) {
  for (auto &i : tasks_) {
    if (i == t) {
      i.priority = value;
      is_changed = true;
    }
  }
}

void todo::addTask(const wstring &str, int pos) {
  task tmp;
  tmp.desc = str;
  if (pos == -1 || pos == (int) tasks_.size())
    tasks_.emplace_back(tmp);
  else if (pos < (int) tasks_.size())
    tasks_.emplace(tasks_.begin() + pos, tmp);

  extract_task_fields(str);
  is_changed = true;
}

void todo::updateTask(const wstring &str, int pos) {
  tasks_[pos].desc = str;
  extract_task_fields(str);
  is_changed = true;
}

void todo::sort() {
  std::stable_sort(tasks_.begin(), tasks_.end());
}

void todo::placeBefore(const task &target, const task &ref) {
  auto ti = std::find(tasks_.begin(), tasks_.end(), target);
  auto ri = std::find(tasks_.begin(), tasks_.end(), ref);

  if (ti == tasks_.end() || ri == tasks_.end()) {
    return;
  }

  tasks_.erase(ti);
  tasks_.emplace(ri, target);
  is_changed = true;
}

void todo::placeAfter(const task &target, const task &ref) {
  auto ti = std::find(tasks_.begin(), tasks_.end(), target);
  auto ri = std::find(tasks_.begin(), tasks_.end(), ref);

  if (ti == tasks_.end() || ri == tasks_.end()) {
    return;
  }

  tasks_.emplace(ri + 1, target);
  tasks_.erase(ti);
  is_changed = true;
}

void todo::remove(const task &t) {
  auto ti = std::find(tasks_.begin(), tasks_.end(), t);
  if (ti != tasks_.end()) {
    tasks_.erase(ti);
    is_changed = true;
  }
}

void todo::extract_task_fields(const wstring &desc) {
  wstringstream ss(desc);
  wstring tstr;

  while (ss >> tstr) {
    if (tstr[0] == '+') {
      projects_.emplace(tstr.substr(1, tstr.length() - 1));
    }
    if (tstr[0] == '@') {
      contexts_.emplace(tstr.substr(1, tstr.length() - 1));
    }
  }
}

void todo::save() {
  wofstream out(todo_file, ios::out | ios::trunc);

  if (!out) {
    throw std::system_error(errno, std::system_category(), "failed to open file " + todo_file);
  }

  for (auto &i : tasks_) {
    if (!i.done) {
      out << format_task(i) << std::endl;
    }
  }

  if (!done_in_todo) {
    out.close();
    out.open(done_file, ios::out | ios::trunc);

    if (!out) {
      throw std::system_error(errno, std::system_category(), "failed to open file " + done_file);
    }
  }

  for (auto &i : tasks_) {
    if (i.done) {
      out << format_task(i) << std::endl;
    }
  }

  out.close();
  is_changed = false;
}
}
}