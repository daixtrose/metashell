// Metashell - Interactive C++ template metaprogramming shell
// Copyright (C) 2014, Abel Sinkovics (abel@sinkovics.hu)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <metashell/console_displayer.hpp>

#include "mock_console.hpp"
#include "util.hpp"

#include <just/test.hpp>

#include <vector>

using namespace metashell;

namespace
{
  typedef std::vector<call_graph_node> call_grph;

  call_grph fib5_call_graph()
  {
    using metashell::data::instantiation_kind;
    using metashell::data::type;

    return
      call_grph{
        {frame(type("int_<fib<5>::value>")), 0, 3},
        {frame( fib<5>(), instantiation_kind::template_instantiation), 1, 2},
        {frame(  fib<3>(), instantiation_kind::template_instantiation), 2, 2},
        {frame(   fib<1>(), instantiation_kind::memoization), 3, 0},
        {frame(   fib<2>(), instantiation_kind::template_instantiation), 3, 2},
        {frame(    fib<0>(), instantiation_kind::memoization), 4, 0},
        {frame(    fib<1>(), instantiation_kind::memoization), 4, 0},
        {frame(  fib<4>(), instantiation_kind::template_instantiation), 2, 2},
        {frame(   fib<2>(), instantiation_kind::memoization), 3, 0},
        {frame(   fib<3>(), instantiation_kind::memoization), 3, 0},
        {frame( fib<5>(), instantiation_kind::memoization), 1, 0},
        {frame( type("int_<5>"),instantiation_kind::template_instantiation),1,0}
      };
  }
}

JUST_TEST_CASE(test_nothing_is_displayed_by_default)
{
  mock_console c;
  console_displayer cd(c, false, false);

  JUST_ASSERT_EQUAL("", c.content().get_string());
}

JUST_TEST_CASE(test_raw_text_is_printed)
{
  mock_console c;
  console_displayer cd(c, false, false);
  cd.show_raw_text("Hello world!");

  JUST_ASSERT_EQUAL("Hello world!\n", c.content().get_string());
}

JUST_TEST_CASE(test_raw_text_with_new_line_is_printed)
{
  mock_console c;
  console_displayer cd(c, false, false);
  cd.show_raw_text("Hello\nworld!");

  JUST_ASSERT_EQUAL("Hello\nworld!\n", c.content().get_string());
}

JUST_TEST_CASE(test_error_with_no_colors_is_printed)
{
  mock_console c;
  console_displayer cd(c, false, false);
  cd.show_error("Something went wrong");

  JUST_ASSERT_EQUAL(
    colored_string("Something went wrong\n", boost::none),
    c.content()
  );
}

JUST_TEST_CASE(test_error_with_colors_is_printed_in_red)
{
  mock_console c;
  console_displayer cd(c, false, true);
  cd.show_error("Something went wrong");

  JUST_ASSERT_EQUAL(
    colored_string("Something went wrong", color::bright_red) + "\n",
    c.content()
  );
}

JUST_TEST_CASE(test_mdb_forwardtrace_from_root_on_narrow_terminal)
{
  mock_console c(25);
  console_displayer d(c, false, false);

  d.show_call_graph(fib5_call_graph());

  JUST_ASSERT_EQUAL(
    "int_<fib<5>::value>\n"
    "+ fib<5> (TemplateInstant\n"
    "| iation)\n"
    "| + fib<3> (TemplateInsta\n"
    "| | ntiation)\n"
    "| | + fib<1> (Memoization\n"
    "| | | )\n"
    "| | ` fib<2> (TemplateIns\n"
    "| |   tantiation)\n"
    "| |   + fib<0> (Memoizati\n"
    "| |   | on)\n"
    "| |   ` fib<1> (Memoizati\n"
    "| |     on)\n"
    "| ` fib<4> (TemplateInsta\n"
    "|   ntiation)\n"
    "|   + fib<2> (Memoization\n"
    "|   | )\n"
    "|   ` fib<3> (Memoization\n"
    "|     )\n"
    "+ fib<5> (Memoization)\n"
    "` int_<5> (TemplateInstan\n"
    "  tiation)\n",
    c.content().get_string()
  );
}

JUST_TEST_CASE(test_mdb_forwardtrace_on_extremely_narrow_terminal_w0)
{
  mock_console c(0);
  console_displayer d(c, false, false);

  d.show_call_graph(fib5_call_graph());

  // The algorithm just gives up, and prints without extra line breaks
  JUST_ASSERT_EQUAL(
    "int_<fib<5>::value>\n"
    "+ fib<5> (TemplateInstantiation)\n"
    "| + fib<3> (TemplateInstantiation)\n"
    "| | + fib<1> (Memoization)\n"
    "| | ` fib<2> (TemplateInstantiation)\n"
    "| |   + fib<0> (Memoization)\n"
    "| |   ` fib<1> (Memoization)\n"
    "| ` fib<4> (TemplateInstantiation)\n"
    "|   + fib<2> (Memoization)\n"
    "|   ` fib<3> (Memoization)\n"
    "+ fib<5> (Memoization)\n"
    "` int_<5> (TemplateInstantiation)\n",
    c.content().get_string()
  );
}

JUST_TEST_CASE(test_mdb_forwardtrace_on_extremely_narrow_terminal_w1)
{
  mock_console c(1);
  console_displayer d(c, false, false);

  d.show_call_graph(fib5_call_graph());

  // The algorithm just gives up, and prints without extra line breaks
  JUST_ASSERT_EQUAL(
    "int_<fib<5>::value>\n"
    "+ fib<5> (TemplateInstantiation)\n"
    "| + fib<3> (TemplateInstantiation)\n"
    "| | + fib<1> (Memoization)\n"
    "| | ` fib<2> (TemplateInstantiation)\n"
    "| |   + fib<0> (Memoization)\n"
    "| |   ` fib<1> (Memoization)\n"
    "| ` fib<4> (TemplateInstantiation)\n"
    "|   + fib<2> (Memoization)\n"
    "|   ` fib<3> (Memoization)\n"
    "+ fib<5> (Memoization)\n"
    "` int_<5> (TemplateInstantiation)\n",
    c.content().get_string()
  );
}

