/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *   Copyright (c) 2019 CTTC
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2 as
 *   published by the Free Software Foundation;
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include <ns3/test.h>
#include <ns3/object-factory.h>
#include <ns3/mmwave-enb-phy.h>

/**
 * \file nr-lte-pattern-generation
 * \ingroup test
 * \brief Unit-testing for the LTE/NR TDD pattern
 *
 * The test considers the function MmWaveEnbPhy::GenerateStructuresFromPattern
 * and checks that the output of that function is equal to the one pre-defined.
 */
namespace ns3 {

/**
 * \brief TestSched testcase
 */
class LtePatternTestCase : public TestCase
{
public:

  /**
   * \brief The result in a single struct
   */
  struct Result
  {
    std::map<uint32_t, std::vector<uint32_t> > m_toSendDl;
    std::map<uint32_t, std::vector<uint32_t> > m_toSendUl;
    std::map<uint32_t, std::vector<uint32_t> > m_generateDl;
    std::map<uint32_t, std::vector<uint32_t> > m_generateUl;
  };

  /**
   * \brief Create LtePatternTestCase
   * \param name Name of the test
   */
  LtePatternTestCase (const std::string &name)
    : TestCase (name)
  {}

  /**
   * \brief Check if two maps are equal
   * \param a first map
   * \param b second map
   */
  void CheckMap (const std::map<uint32_t, std::vector<uint32_t> > &a,
                 const std::map<uint32_t, std::vector<uint32_t> > &b);
  /**
   * \brief Check if two vectors are equal
   * \param a first vector
   * \param b second vector
   */
  void CheckVector (const std::vector<uint32_t> &a,
                    const std::vector<uint32_t> &b);
private:
  virtual void DoRun (void) override;
  /**
   * \brief Test the output of PHY for a pattern, and compares it to the input
   * \param pattern The pattern to test
   * \param result The theoretical result
   */
  void TestPattern (const std::vector<LteNrTddSlotType> &pattern, const Result &result);
  /**
   * \brief Print the map
   * \param str the map to print
   */
  void Print (const std::map<uint32_t, std::vector<uint32_t> > &str);

  bool m_verbose = false; //!< Print the generated structure
};


void
LtePatternTestCase::DoRun()
{
  auto one = {LteNrTddSlotType::DL,
              LteNrTddSlotType::S,
              LteNrTddSlotType::UL,
              LteNrTddSlotType::UL,
              LteNrTddSlotType::DL,
              LteNrTddSlotType::DL,
              LteNrTddSlotType::S,
              LteNrTddSlotType::UL,
              LteNrTddSlotType::UL,
              LteNrTddSlotType::DL,
             };

  Result a = { {
                 { 0, {0, } },
                 { 1, {1, } },
                 { 4, {4, } },
                 { 5, {5, } },
                 { 6, {6, } },
                 { 9, {9, } },
               },
               {
                 { 0, {2, } },
                 { 1, {3, } },
                 { 5, {7, } },
                 { 6, {8, } },
               },
               {
                 { 2, {4, } },
                 { 3, {5, } },
                 { 4, {6, } },
                 { 7, {9, } },
                 { 8, {0, } },
                 { 9, {1, } },
               },
               {
                 { 3, {7, } },
                 { 4, {8, } },
                 { 8, {2, } },
                 { 9, {3, } },
               } };
  TestPattern (one, a);


  Result b = {
    {
      { 0, {0, } },
      { 1, {1, } },
      { 3, {3, } },
      { 4, {4, } },
      { 5, {5, } },
      { 6, {6, } },
      { 8, {8, } },
      { 9, {9, } },
    },
    {
      { 0, {2, } },
      { 5, {7, } },
    },
    {
      { 1, {3, } },
      { 2, {4, } },
      { 3, {5, } },
      { 4, {6, } },
      { 6, {8, } },
      { 7, {9, } },
      { 8, {0, } },
      { 9, {1, } },
    },
    {
      { 3, {7, } },
      { 8, {2, } },
    },
  };
  auto two = {LteNrTddSlotType::DL,
              LteNrTddSlotType::S,
              LteNrTddSlotType::UL,
              LteNrTddSlotType::DL,
              LteNrTddSlotType::DL,
              LteNrTddSlotType::DL,
              LteNrTddSlotType::S,
              LteNrTddSlotType::UL,
              LteNrTddSlotType::DL,
              LteNrTddSlotType::DL,
             };

  TestPattern (two, b);


  Result c = {
    {
      { 0, {0, } },
      { 1, {1, } },
      { 5, {5, } },
      { 6, {6, } },
      { 7, {7, } },
      { 8, {8, } },
      { 9, {9, } },
    },
    {
      { 0, {2, } },
      { 1, {4, 3, } },
    },
    {
      { 3, {5, } },
      { 4, {6, } },
      { 5, {7, } },
      { 6, {8, } },
      { 7, {9, } },
      { 8, {0, } },
      { 9, {1, } },
    },
    {
      { 8, {2, } },
      { 9, {3, 4, } },
    }

  };
  auto three = {LteNrTddSlotType::DL,
                LteNrTddSlotType::S,
                LteNrTddSlotType::UL,
                LteNrTddSlotType::UL,
                LteNrTddSlotType::UL,
                LteNrTddSlotType::DL,
                LteNrTddSlotType::DL,
                LteNrTddSlotType::DL,
                LteNrTddSlotType::DL,
                LteNrTddSlotType::DL,
               };

  TestPattern (three, c);


  Result d = {
    {
      { 0, {0, } },
      { 1, {1, } },
      { 4, {4, } },
      { 5, {5, } },
      { 6, {6, } },
      { 7, {7, } },
      { 8, {8, } },
      { 9, {9, } },
    },
    {
      { 0, {2, } },
      { 1, {3, } },
    },
    {
      { 2, {4, } },
      { 3, {5, } },
      { 4, {6, } },
      { 5, {7, } },
      { 6, {8, } },
      { 7, {9, } },
      { 8, {0, } },
      { 9, {1, } },
    },
    {
      { 8, {2, } },
      { 9, {3, } },
    }
  };
  auto four = {LteNrTddSlotType::DL,
               LteNrTddSlotType::S,
               LteNrTddSlotType::UL,
               LteNrTddSlotType::UL,
               LteNrTddSlotType::DL,
               LteNrTddSlotType::DL,
               LteNrTddSlotType::DL,
               LteNrTddSlotType::DL,
               LteNrTddSlotType::DL,
               LteNrTddSlotType::DL,
              };

  TestPattern (four, d);



  Result e = {
    {
      { 0, {0, } },
      { 1, {1, } },
      { 3, {3, } },
      { 4, {4, } },
      { 5, {5, } },
      { 6, {6, } },
      { 7, {7, } },
      { 8, {8, } },
      { 9, {9, } },
    },
    {
      { 0, {2, } },
    },
    {
      { 1, {3, } },
      { 2, {4, } },
      { 3, {5, } },
      { 4, {6, } },
      { 5, {7, } },
      { 6, {8, } },
      { 7, {9, } },
      { 8, {0, } },
      { 9, {1, } },
    },
    {
      { 8, {2, } },
    }
  };
  auto five = {LteNrTddSlotType::DL,
               LteNrTddSlotType::S,
               LteNrTddSlotType::UL,
               LteNrTddSlotType::DL,
               LteNrTddSlotType::DL,
               LteNrTddSlotType::DL,
               LteNrTddSlotType::DL,
               LteNrTddSlotType::DL,
               LteNrTddSlotType::DL,
               LteNrTddSlotType::DL,
              };

  TestPattern (five, e);


  Result f = {
    {
      { 0, {0, } },
      { 1, {1, } },
      { 5, {5, } },
      { 6, {6, } },
      { 9, {9, } },
    },
    {
      { 0, {2, } },
      { 1, {4, 3, } },
      { 5, {7, } },
      { 6, {8, } },
    },
    {
      { 3, {5, } },
      { 4, {6, } },
      { 7, {9, } },
      { 8, {0, } },
      { 9, {1, } },
    },
    {
      { 3, {7, } },
      { 4, {8, } },
      { 8, {2, } },
      { 9, {3, 4, } },
    }
  };
  auto six = {LteNrTddSlotType::DL,
              LteNrTddSlotType::S,
              LteNrTddSlotType::UL,
              LteNrTddSlotType::UL,
              LteNrTddSlotType::UL,
              LteNrTddSlotType::DL,
              LteNrTddSlotType::S,
              LteNrTddSlotType::UL,
              LteNrTddSlotType::UL,
              LteNrTddSlotType::DL,
             };
  TestPattern(six, f);


  Result g = {
    {
      { 0, {0, } },
      { 1, {1, } },
      { 5, {5, } },
      { 6, {6, } },
    },
    {
      { 0, {2, } },
      { 1, {4, 3, } },
      { 5, {7, } },
      { 6, {9, 8, } },
    },
    {
      { 3, {5, } },
      { 4, {6, } },
      { 8, {0, } },
      { 9, {1, } },
    },
    {
      { 3, {7, } },
      { 4, {8, 9, } },
      { 8, {2, } },
      { 9, {3, 4, } },
    }
  };
  auto zero = {LteNrTddSlotType::DL,
               LteNrTddSlotType::S,
               LteNrTddSlotType::UL,
               LteNrTddSlotType::UL,
               LteNrTddSlotType::UL,
               LteNrTddSlotType::DL,
               LteNrTddSlotType::S,
               LteNrTddSlotType::UL,
               LteNrTddSlotType::UL,
               LteNrTddSlotType::UL,
              };

  TestPattern (zero, g);

  Result h = { {
                 { 0, {0, } },
                 { 1, {1, } },
                 { 2, {2, } },
                 { 3, {3, } },
                 { 4, {4, } },
                 { 5, {5, } },
                 { 6, {6, } },
                 { 7, {7, } },
                 { 8, {8, } },
                 { 9, {9, } },
               },
               {
                 { 0, {2, } },
                 { 1, {3, } },
                 { 2, {4, } },
                 { 3, {5, } },
                 { 4, {6, } },
                 { 5, {7, } },
                 { 6, {8, } },
                 { 7, {9, } },
                 { 8, {0, } },
                 { 9, {1, } },
               },
               {
                 { 0, {2, } },
                 { 1, {3, } },
                 { 2, {4, } },
                 { 3, {5, } },
                 { 4, {6, } },
                 { 5, {7, } },
                 { 6, {8, } },
                 { 7, {9, } },
                 { 8, {0, } },
                 { 9, {1, } },
               },
               {
                 { 0, {4, } },
                 { 1, {5, } },
                 { 2, {6, } },
                 { 3, {7, } },
                 { 4, {8, } },
                 { 5, {9, } },
                 { 6, {0, } },
                 { 7, {1, } },
                 { 8, {2, } },
                 { 9, {3, } },
               }
             };

  auto nr = {LteNrTddSlotType::F,
             LteNrTddSlotType::F,
             LteNrTddSlotType::F,
             LteNrTddSlotType::F,
             LteNrTddSlotType::F,
             LteNrTddSlotType::F,
             LteNrTddSlotType::F,
             LteNrTddSlotType::F,
             LteNrTddSlotType::F,
             LteNrTddSlotType::F,
            };

  TestPattern (nr, h);
}

void
LtePatternTestCase::Print(const std::map<uint32_t, std::vector<uint32_t> > &str)
{
  std::cout << "{" << std::endl;
  for (const auto & v : str)
    {
      std::cout << " { " << v.first << ", {";
      for (const auto & i : v.second)
        {
          std::cout << i << ", ";
        }
      std::cout << "} }," << std::endl;
    }
  std::cout << "}" << std::endl;
}


void
LtePatternTestCase::CheckVector (const std::vector<uint32_t> &a,
                                 const std::vector<uint32_t> &b)
{
  NS_TEST_ASSERT_MSG_EQ (a.size (), b.size (), "Two vectors have different length");
  for (uint32_t i = 0; i < a.size (); ++i)
    {
      NS_TEST_ASSERT_MSG_EQ (a[i], b[i], "Values in vector diffes");
    }
}

void
LtePatternTestCase::CheckMap (const std::map<uint32_t, std::vector<uint32_t> > &a,
                              const std::map<uint32_t, std::vector<uint32_t> > &b)
{
  NS_TEST_ASSERT_MSG_EQ (a.size (), b.size (), "Two maps have different length");

  for (const std::pair<const uint32_t, std::vector<uint32_t>> & v : a)
    {
      CheckVector (a.at(v.first), b.at(v.first));
    }
}

void
LtePatternTestCase::TestPattern (const std::vector<LteNrTddSlotType> &pattern,
                                 const Result &result)
{
  std::map<uint32_t, std::vector<uint32_t> > toSendDl;
  std::map<uint32_t, std::vector<uint32_t> > toSendUl;
  std::map<uint32_t, std::vector<uint32_t> > generateDl;
  std::map<uint32_t, std::vector<uint32_t> > generateUl;

  MmWaveEnbPhy::GenerateStructuresFromPattern (pattern, &toSendDl, &toSendUl, &generateDl, &generateUl, 0, 2, 2);

  if (m_verbose)
    {
      Print (toSendDl);
      Print (toSendUl);
      Print (generateDl);
      Print (generateUl);
    }

  CheckMap (toSendDl, result.m_toSendDl);
  CheckMap (toSendUl, result.m_toSendUl);
  CheckMap (generateDl, result.m_generateDl);
  CheckMap (generateUl, result.m_generateUl);
}

/**
 * \brief The NrLtePatternTestSuite class
 */
class NrLtePatternTestSuite : public TestSuite
{
public:
  NrLtePatternTestSuite () : TestSuite ("nr-lte-pattern-generation", UNIT)
  {
    AddTestCase(new LtePatternTestCase ("LTE TDD Pattern test"), QUICK);
  }
};

static NrLtePatternTestSuite nrLtePatternTestSuite; //!< Pattern test suite

}; // namespace ns3