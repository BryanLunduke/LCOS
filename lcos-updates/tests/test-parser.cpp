/* Headless tests for the apt-get -s parser and helper protocol.
 * Copyright (C) 2026 LCOS
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "apt-parse.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

static int g_fails = 0;

static std::string read_file(const std::string& path)
{
  std::ifstream in(path);
  if (!in) {
    std::cerr << "cannot read " << path << "\n";
    ++g_fails;
    return {};
  }
  std::ostringstream ss;
  ss << in.rdbuf();
  return ss.str();
}

static void expect(bool ok, const char* what)
{
  if (!ok) {
    std::cerr << "FAIL: " << what << "\n";
    ++g_fails;
  } else {
    std::cout << "ok: " << what << "\n";
  }
}

int main(int argc, char** argv)
{
  const std::string dir = (argc > 1) ? argv[1] : "tests/fixtures";

  {
    const SimulateResult r = parse_apt_simulate(read_file(dir + "/apt-uptodate.txt"));
    expect(r.status == SimulateResult::UpToDate, "up-to-date fixture status");
    expect(r.packages.empty(), "up-to-date fixture has no packages");
    const std::string proto = format_protocol(r);
    expect(proto == "STATUS up-to-date\n", "up-to-date protocol text");
    const SimulateResult p = parse_protocol(proto);
    expect(p.status == SimulateResult::UpToDate, "up-to-date protocol roundtrip");
  }

  {
    const SimulateResult r = parse_apt_simulate(read_file(dir + "/apt-upgrades.txt"));
    expect(r.status == SimulateResult::Upgrades, "upgrades fixture status");
    expect(r.packages.size() == 3, "upgrades fixture COUNT 3 Inst lines");
    if (r.packages.size() >= 3) {
      expect(r.packages[0].name == "libc6", "pkg0 name");
      expect(r.packages[0].old_version == "2.36-9+deb12u3", "pkg0 old");
      expect(r.packages[0].new_version == "2.36-9+deb12u4", "pkg0 new");
      expect(r.packages[1].name == "libc-bin", "pkg1 name");
      expect(r.packages[2].name == "foo", "pkg2 name (no old version)");
      expect(r.packages[2].old_version == "-", "pkg2 old is dash");
      expect(r.packages[2].new_version == "1.0-1", "pkg2 new");
    }
    const std::string proto = format_protocol(r);
    expect(proto.find("STATUS upgrades\n") == 0, "upgrades protocol STATUS");
    expect(proto.find("COUNT 3\n") != std::string::npos, "upgrades protocol COUNT");
    expect(proto.find("PKG libc6 2.36-9+deb12u3 2.36-9+deb12u4\n") != std::string::npos,
           "upgrades protocol PKG libc6");
    expect(proto.find("PKG foo - 1.0-1\n") != std::string::npos, "upgrades protocol PKG foo");
    const SimulateResult p = parse_protocol(proto);
    expect(p.status == SimulateResult::Upgrades, "upgrades protocol roundtrip");
    expect(p.packages.size() == 3, "upgrades protocol package count");
  }

  {
    const SimulateResult r = parse_apt_simulate(read_file(dir + "/apt-error.txt"));
    expect(r.status == SimulateResult::Error, "error fixture status");
    expect(!r.error_msg.empty(), "error fixture MSG");
    expect(r.error_msg.find("Failed to fetch") != std::string::npos ||
               r.error_msg.find("E:") != std::string::npos,
           "error fixture uses apt E: line");
    const std::string proto = format_protocol(r);
    expect(proto.find("STATUS error\n") == 0, "error protocol STATUS");
    expect(proto.find("MSG ") != std::string::npos, "error protocol MSG");
    const SimulateResult p = parse_protocol(proto);
    expect(p.status == SimulateResult::Error, "error protocol roundtrip");
  }

  {
    PackageUpgrade pkg;
    expect(parse_inst_line("Inst name [old] (new origin [amd64])", pkg), "Inst old+new");
    expect(pkg.name == "name" && pkg.old_version == "old" && pkg.new_version == "new",
           "Inst old+new fields");
    expect(parse_inst_line("Inst other (1.2.3 Debian:12 [amd64])", pkg), "Inst new only");
    expect(pkg.name == "other" && pkg.old_version == "-" && pkg.new_version == "1.2.3",
           "Inst new only fields");
    expect(!parse_inst_line("Conf libc6 (1.0 Debian:12 [amd64])", pkg), "Conf is not Inst");
    expect(!parse_inst_line("Remv unused-pkg [0.1]", pkg), "Remv is not Inst");
  }

  if (g_fails != 0) {
    std::cerr << g_fails << " failure(s)\n";
    return 1;
  }
  std::cout << "all parser tests passed\n";
  return 0;
}
