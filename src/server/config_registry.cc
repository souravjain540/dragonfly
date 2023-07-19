// Copyright 2023, Roman Gershman.  All rights reserved.
// See LICENSE for licensing terms.
//
#include "src/server/config_registry.h"

#include "base/logging.h"

namespace dfly {

using namespace std;

ConfigRegistry& ConfigRegistry::Register(std::string_view name, WriteCb cb) {
  unique_lock lk(mu_);
  auto [it, inserted] = registry_.emplace(name, std::move(cb));
  CHECK(inserted) << "Duplicate config name: " << name;

  return *this;
}

// Returns true if the value was updated.
bool ConfigRegistry::Set(std::string_view config_name, std::string_view value) {
  // TODO:
  return false;
}

ConfigRegistry config_registry;

}  // namespace dfly
