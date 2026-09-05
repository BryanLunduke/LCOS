/* lcos-updates — parse apt-get -s Inst lines and the helper protocol.
 * Copyright (C) 2026 LCOS
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "apt-parse.hpp"

#include <sstream>

static std::string trim_cr(std::string line)
{
  if (!line.empty() && line.back() == '\r')
    line.pop_back();
  return line;
}

bool parse_inst_line(const std::string& raw, PackageUpgrade& out)
{
  const std::string line = trim_cr(raw);
  const std::string prefix = "Inst ";
  if (line.compare(0, prefix.size(), prefix) != 0)
    return false;

  std::string rest = line.substr(prefix.size());
  const std::string::size_type name_end = rest.find(' ');
  if (name_end == std::string::npos || name_end == 0)
    return false;

  out.name = rest.substr(0, name_end);
  rest = rest.substr(name_end + 1);
  while (!rest.empty() && rest[0] == ' ')
    rest.erase(0, 1);
  if (rest.empty())
    return false;

  if (rest[0] == '[') {
    const std::string::size_type close = rest.find(']');
    if (close == std::string::npos || close < 2)
      return false;
    out.old_version = rest.substr(1, close - 1);
    rest = rest.substr(close + 1);
    while (!rest.empty() && rest[0] == ' ')
      rest.erase(0, 1);
  } else {
    out.old_version = "-";
  }

  if (rest.empty() || rest[0] != '(')
    return false;
  rest.erase(0, 1);
  const std::string::size_type ver_end = rest.find_first_of(" )");
  if (ver_end == std::string::npos || ver_end == 0)
    return false;
  out.new_version = rest.substr(0, ver_end);
  return !out.name.empty() && !out.new_version.empty();
}

SimulateResult parse_apt_simulate(const std::string& text)
{
  SimulateResult result;
  result.status = SimulateResult::UpToDate;

  std::istringstream in(text);
  std::string line;
  while (std::getline(in, line)) {
    line = trim_cr(line);
    if (line.compare(0, 2, "E:") == 0) {
      result.status = SimulateResult::Error;
      if (result.error_msg.empty())
        result.error_msg = line;
      continue;
    }
    PackageUpgrade pkg;
    if (parse_inst_line(line, pkg))
      result.packages.push_back(pkg);
  }

  if (result.status == SimulateResult::Error)
    return result;
  if (result.packages.empty())
    result.status = SimulateResult::UpToDate;
  else
    result.status = SimulateResult::Upgrades;
  return result;
}

std::string format_protocol(const SimulateResult& result)
{
  std::ostringstream out;
  switch (result.status) {
  case SimulateResult::UpToDate:
    out << "STATUS up-to-date\n";
    break;
  case SimulateResult::Upgrades:
    out << "STATUS upgrades\n";
    out << "COUNT " << result.packages.size() << "\n";
    for (const auto& pkg : result.packages)
      out << "PKG " << pkg.name << " " << pkg.old_version << " " << pkg.new_version
          << "\n";
    break;
  case SimulateResult::Success:
    out << "STATUS success\n";
    break;
  case SimulateResult::Error:
    out << "STATUS error\n";
    if (!result.error_msg.empty())
      out << "MSG " << result.error_msg << "\n";
    else
      out << "MSG unknown error\n";
    break;
  }
  return out.str();
}

SimulateResult parse_protocol(const std::string& text)
{
  SimulateResult result;
  result.status = SimulateResult::Error;
  result.error_msg = "No STATUS from helper";
  bool saw_status = false;

  std::istringstream in(text);
  std::string line;
  while (std::getline(in, line)) {
    line = trim_cr(line);
    if (line.compare(0, 7, "STATUS ") == 0) {
      const std::string st = line.substr(7);
      saw_status = true;
      if (st == "up-to-date") {
        result.status = SimulateResult::UpToDate;
        result.error_msg.clear();
      } else if (st == "upgrades") {
        result.status = SimulateResult::Upgrades;
        result.error_msg.clear();
      } else if (st == "success") {
        result.status = SimulateResult::Success;
        result.error_msg.clear();
      } else if (st == "error") {
        result.status = SimulateResult::Error;
        result.error_msg.clear();
      } else {
        result.status = SimulateResult::Error;
        result.error_msg = "Unknown STATUS " + st;
      }
    } else if (line.compare(0, 4, "MSG ") == 0) {
      const std::string msg = line.substr(4);
      if (result.error_msg.empty())
        result.error_msg = msg;
      else
        result.error_msg += "\n" + msg;
    } else if (line.compare(0, 4, "PKG ") == 0) {
      std::istringstream ps(line.substr(4));
      PackageUpgrade pkg;
      if (ps >> pkg.name >> pkg.old_version >> pkg.new_version)
        result.packages.push_back(pkg);
    }
  }

  if (!saw_status) {
    result.status = SimulateResult::Error;
    if (result.error_msg.empty())
      result.error_msg = "No STATUS from helper";
  }
  return result;
}
