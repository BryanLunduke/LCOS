/* lcos-updates — parse apt-get -s Inst lines and the helper protocol.
 * Copyright (C) 2026 LCOS
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef LCOS_UPDATES_APT_PARSE_HPP
#define LCOS_UPDATES_APT_PARSE_HPP

#include <string>
#include <vector>

struct PackageUpgrade {
  std::string name;
  std::string old_version;
  std::string new_version;
};

struct SimulateResult {
  enum Status { UpToDate, Upgrades, Error, Success };
  Status status = UpToDate;
  std::string error_msg;
  std::vector<PackageUpgrade> packages;
};

/* Parse one apt-get -s "Inst name [old] (new ...)" or "Inst name (new ...)" line. */
bool parse_inst_line(const std::string& line, PackageUpgrade& out);

/* Parse full apt-get -s upgrade text. E: lines become Error. */
SimulateResult parse_apt_simulate(const std::string& text);

/* Stable helper stdout protocol. */
std::string format_protocol(const SimulateResult& result);
SimulateResult parse_protocol(const std::string& text);

#endif
