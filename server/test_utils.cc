// Copyright 2021, Roman Gershman.  All rights reserved.
// See LICENSE for licensing terms.
//

#include "server/test_utils.h"

#include <absl/strings/match.h>

#include "base/logging.h"
#include "util/uring/uring_pool.h"

namespace dfly {

using namespace testing;
using namespace util;
using namespace std;

bool RespMatcher::MatchAndExplain(const RespExpr& e, MatchResultListener* listener) const {
  if (e.type != type_) {
    *listener << "\nWrong type: " << e.type;
    return false;
  }

  if (type_ == RespExpr::STRING || type_ == RespExpr::ERROR) {
    RespExpr::Buffer ebuf = e.GetBuf();
    std::string_view actual{reinterpret_cast<char*>(ebuf.data()), ebuf.size()};

    if (type_ == RespExpr::ERROR && !absl::StrContains(actual, exp_str_)) {
      *listener << "Actual does not contain '" << exp_str_ << "'";
      return false;
    }
    if (type_ == RespExpr::STRING  && exp_str_ != actual) {
      *listener << "\nActual string: " << actual;
      return false;
    }
  } else if (type_ == RespExpr::INT64) {
    auto actual = get<int64_t>(e.u);
    if (exp_int_ != actual) {
      *listener << "\nActual : " << actual << " expected: " << exp_int_;
      return false;
    }
  } else if (type_ == RespExpr::ARRAY) {
    size_t len = get<RespVec*>(e.u)->size();
    if (len != size_t(exp_int_)) {
      *listener << "Actual length " << len << ", expected: " << exp_int_;
      return false;
    }
  }

  return true;
}

void RespMatcher::DescribeTo(std::ostream* os) const {
  *os << "is ";
  switch (type_) {
    case RespExpr::STRING:
    case RespExpr::ERROR:
      *os << exp_str_;
      break;

    case RespExpr::INT64:
      *os << exp_str_;
      break;
    default:
      *os << "TBD";
      break;
  }
}

void RespMatcher::DescribeNegationTo(std::ostream* os) const {
  *os << "is not ";
}

bool RespTypeMatcher::MatchAndExplain(const RespExpr& e, MatchResultListener* listener) const {
  if (e.type != type_) {
    *listener << "\nWrong type: " << RespExpr::TypeName(e.type);
    return false;
  }

  return true;
}

void RespTypeMatcher::DescribeTo(std::ostream* os) const {
  *os << "is " << RespExpr::TypeName(type_);
}

void RespTypeMatcher::DescribeNegationTo(std::ostream* os) const {
  *os << "is not " << RespExpr::TypeName(type_);
}

void PrintTo(const RespExpr::Vec& vec, std::ostream* os) {
  *os << "Vec: [";
  if (!vec.empty()) {
    for (size_t i = 0; i < vec.size() - 1; ++i) {
      *os << vec[i] << ",";
    }
    *os << vec.back();
  }
  *os << "]\n";
}

vector<int64_t> ToIntArr(const RespVec& vec) {
  vector<int64_t> res;
  for (auto a : vec) {
    int64_t val;
    std::string_view s = ToSV(a.GetBuf());
    CHECK(absl::SimpleAtoi(s, &val)) << s;
    res.push_back(val);
  }

  return res;
}

}  // namespace dfly
