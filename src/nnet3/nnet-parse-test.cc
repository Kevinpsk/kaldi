// nnet3/nnet-parse-test.cc

// Copyright 2015  Vimal Manohar (Johns Hopkins University)

// See ../../COPYING for clarification regarding multiple authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED
// WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
// MERCHANTABLITY OR NON-INFRINGEMENT.
// See the Apache 2 License for the specific language governing permissions and
// limitations under the License.

#include "nnet3/nnet-parse.h"


namespace kaldi {
namespace nnet3 {

void UnitTestConfigLineParse() {
  std::string str;
  {
    ConfigLine cfl;
    str = "xx=yyy foo=bar  baz=123 ba=1:2";
    bool status = cfl.ParseLine(str);
    KALDI_ASSERT(status);

    KALDI_ASSERT(cfl.HasUnusedValues());
    std::string str_value;
    KALDI_ASSERT(cfl.GetValue("xx", &str_value));
    KALDI_ASSERT(str_value == "yyy");
    KALDI_ASSERT(cfl.HasUnusedValues());
    KALDI_ASSERT(cfl.GetValue("foo", &str_value));
    KALDI_ASSERT(str_value == "bar");
    KALDI_ASSERT(cfl.HasUnusedValues());
    KALDI_ASSERT(!cfl.GetValue("xy", &str_value));
    KALDI_ASSERT(cfl.GetValue("baz", &str_value));
    KALDI_ASSERT(str_value == "123");

    std::vector<int32> int_values;
    KALDI_ASSERT(!cfl.GetValue("xx", &int_values));
    KALDI_ASSERT(cfl.GetValue("baz", &int_values));
    KALDI_ASSERT(cfl.HasUnusedValues());
    KALDI_ASSERT(int_values.size() == 1 && int_values[0] == 123);
    KALDI_ASSERT(cfl.GetValue("ba", &int_values));
    KALDI_ASSERT(int_values.size() == 2 && int_values[0] == 1 && int_values[1] == 2);
    KALDI_ASSERT(!cfl.HasUnusedValues());
  }

  {
    ConfigLine cfl;
    str = "baz=x y z pp = qq ab =cd ac= bd";
    KALDI_ASSERT(!cfl.ParseLine(str));
  }
  {
    ConfigLine cfl;
    str = "baz=x y z pp = qq ab=cd ac=bd";
    KALDI_ASSERT(!cfl.ParseLine(str));
  }
  {
    ConfigLine cfl;
    str = "baz";
    KALDI_ASSERT(!cfl.ParseLine(str));
  }
  {
    ConfigLine cfl;
    str = "a=b baz";
    KALDI_ASSERT(cfl.ParseLine(str) && cfl.UnusedValues() == "a=b baz");
  }
  {
    ConfigLine cfl;
    str = "a=b baz ";
    KALDI_ASSERT(cfl.ParseLine(str) && cfl.UnusedValues() == "a=b baz");
  }
  {
    ConfigLine cfl;
    str = "a=b =c";
    KALDI_ASSERT(!cfl.ParseLine(str));
  }
  {
    ConfigLine cfl;
    str = "baz='x y z' pp=qq ab=cd ac=bd";
    KALDI_ASSERT(cfl.ParseLine(str));
    std::string str_value;
    KALDI_ASSERT(cfl.GetValue("baz", &str_value));
    KALDI_ASSERT(str_value == "x y z");
    KALDI_ASSERT(cfl.GetValue("pp", &str_value));
    KALDI_ASSERT(str_value == "qq");
    KALDI_ASSERT(cfl.UnusedValues() == "ab=cd ac=bd");
    KALDI_ASSERT(cfl.GetValue("ab", &str_value));
    KALDI_ASSERT(str_value == "cd");
    KALDI_ASSERT(cfl.UnusedValues() == "ac=bd");
    KALDI_ASSERT(cfl.HasUnusedValues());
    KALDI_ASSERT(cfl.GetValue("ac", &str_value));
    KALDI_ASSERT(str_value == "bd");
    KALDI_ASSERT(!cfl.HasUnusedValues());
  }

  {
    ConfigLine cfl;
    str = "baz= pp = qq flag=t ";
    KALDI_ASSERT(!cfl.ParseLine(str));
  }
  {
    ConfigLine cfl;
    str = " baz= pp=qq flag=t  ";
    KALDI_ASSERT(cfl.ParseLine(str));

    std::string str_value;
    KALDI_ASSERT(cfl.GetValue("baz", &str_value));
    KALDI_ASSERT(str_value == "");
    KALDI_ASSERT(cfl.GetValue("pp", &str_value));
    KALDI_ASSERT(str_value == "qq");
    KALDI_ASSERT(cfl.HasUnusedValues());
    KALDI_ASSERT(cfl.GetValue("flag", &str_value));
    KALDI_ASSERT(str_value == "t");
    KALDI_ASSERT(!cfl.HasUnusedValues());

    bool bool_value = false;
    KALDI_ASSERT(cfl.GetValue("flag", &bool_value));
    KALDI_ASSERT(bool_value);
  }

  {
    ConfigLine cfl;
    str = "_baz=a -pp=qq";
    KALDI_ASSERT(!cfl.ParseLine(str));
  }
  {
    ConfigLine cfl;
    str = "0baz=a pp=qq";
    KALDI_ASSERT(!cfl.ParseLine(str));
  }
  {
    ConfigLine cfl;
    str = "-baz=a pp=qq";
    KALDI_ASSERT(!cfl.ParseLine(str));
  }
  {
    ConfigLine cfl;
    str = "_baz'=a pp=qq";
    KALDI_ASSERT(!cfl.ParseLine(str));
  }
  {
    ConfigLine cfl;
    str = "baz=g";
    KALDI_ASSERT(cfl.ParseLine(str));
    bool flag;
    KALDI_ASSERT(!cfl.GetValue("baz", &flag));
  }
  {
    ConfigLine cfl;
    str = "_baz1=a pp=qq";
    KALDI_ASSERT(cfl.ParseLine(str));

    std::string str_value;
    KALDI_ASSERT(cfl.GetValue("_baz1", &str_value));
  }
}

void UnitTestReadConfig() {
  std::string str = " alpha=aa beta=\"b b\"# String test\n"
      "beta2='b c' beta3=bd # \n"
      "gamma=1:2:3:4  # Int Vector test\n"
      " de1ta=f  # Bool + Integer in key Comment test delta=t  \n"
      "_epsilon=-1  # Int Vector test _epsilon=1 \n"
      " zet-_a=0.15   theta=1.1# Float, -, _ test\n"
      "quoted='a b c' # quoted string\n"
      "quoted2=\"d e 'a b=c' f\" # string quoted with double quotes";

  std::istringstream is(str);
  std::vector<std::string> lines;
  ReadConfigFile(is, &lines);
  KALDI_ASSERT(lines.size() == 8);

  ConfigLine cfl;
  for (size_t i = 0; i < lines.size(); i++) {
    KALDI_ASSERT(cfl.ParseLine(lines[i]));
  }
  KALDI_ASSERT(cfl.GetValue("beta2", &str) && str == "b c");
  KALDI_ASSERT(cfl.GetValue("_epsilon", &str) && str == "-1");
  KALDI_ASSERT(cfl.GetValue("quoted", &str) && str == "a b c");
  KALDI_ASSERT(cfl.GetValue("quoted2", &str) && str == "d e 'a b=c' f");

  BaseFloat float_val = 0;
  KALDI_ASSERT(cfl.GetValue("zet-_a", &float_val) && ApproxEqual(float_val, 0.15));
}

void UnitTestDescriptorTokenize() {
  std::vector<std::string> lines;

  std::string str = "(,test )";
  KALDI_ASSERT(DescriptorTokenize(str, &lines));
  KALDI_ASSERT(lines[0] == "(" && lines[1] == "," && lines[2] == "test" && lines[3] == ")");

  str = "(,1test )";
  KALDI_ASSERT(!DescriptorTokenize(str, &lines));

  str = "t (,-1 )";
  KALDI_ASSERT(DescriptorTokenize(str, &lines));
  KALDI_ASSERT(lines.size() == 5 && lines[0] == "t" && lines[3] == "-1");

  str = "   sd , -112 )";
  KALDI_ASSERT(DescriptorTokenize(str, &lines));
  KALDI_ASSERT(lines.size() == 4 && lines[0] == "sd" && lines[2] == "-112");

  str = "   sd , +112 )";
  KALDI_ASSERT(DescriptorTokenize(str, &lines));
  KALDI_ASSERT(lines.size() == 4 && lines[0] == "sd" && lines[2] == "+112");

  str = "foo";
  KALDI_ASSERT(DescriptorTokenize(str, &lines));
  KALDI_ASSERT(lines.size() == 1 && lines[0] == "foo");

}

void UnitTestSummarizeVector() {
  // will be eyeballed by a human.
  Vector<BaseFloat> vec(9);
  vec.SetRandn();
  vec(0) = 1024.2343;
  vec(1) = 0.01;
  vec(2) = 0.001234;
  vec(3) = 0.000198;
  vec(3) = 1.98e-09;
  vec(4) = 153.0;
  vec(5) = 0.154;
  vec(6) = 1.2;
  vec(7) = 9.2;
  vec(8) = 10.8;

  KALDI_LOG << "vec = " << vec << " -> " << SummarizeVector(vec);

  vec.Resize(20, kCopyData);
  KALDI_LOG << "vec = " << vec << " -> " << SummarizeVector(vec);
}

} // namespace nnet3

} // namespace kaldi

int main() {
  using namespace kaldi;
  using namespace kaldi::nnet3;

  UnitTestConfigLineParse();
  UnitTestReadConfig();
  UnitTestDescriptorTokenize();
  UnitTestSummarizeVector();

  KALDI_LOG << "Parse tests succeeded.";

  return 0;
}
