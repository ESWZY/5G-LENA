/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *   Copyright (c) 2020 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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

#include "ns3/test.h"
#include "system-scheduler-test.h"

using namespace ns3;

/**
  * \file nr-system-test-schedulers-tdma-rr.cc
  * \ingroup test
  *
  * \brief System test for TDMA - Round Robin scheduler. It checks that all the
  * packets sent are delivered correctly.
  */

/**
 * \brief The TDMA RR scheduler system test suite
 * \ingroup test
 *
 * It will check Tdma RR with:
 *
 * - DL, UL, DL and UL together
 * - UEs per beam: 1, 2, 4, 8
 * - beams: 1, 2
 * - numerologies: 0, 1
 */
class NrSystemTestSchedulerTdmaRrSuite : public TestSuite
{
public:
  /**
   * \brief constructor
   */
  NrSystemTestSchedulerTdmaRrSuite ();
};

NrSystemTestSchedulerTdmaRrSuite::NrSystemTestSchedulerTdmaRrSuite ()
  : TestSuite ("nr-system-test-schedulers-tdma-rr", SYSTEM)
{
  enum TxMode
  {
    DL,
    UL,
    DL_UL
  };

  std::list<std::string> subdivision     = {"Tdma",};
  std::list<std::string> scheds          = {"RR"};
  std::list<TxMode>      mode            = {DL, UL, DL_UL};
  std::list<uint32_t>    uesPerBeamList  = {1, 2, 4, 8};
  std::list<uint32_t>    beams           = {1, 2};
  std::list<uint32_t>    numerologies    = {0, 1, }; // Test only num 0 and 1

  for (const auto & num : numerologies)
    {
      for (const auto & subType : subdivision)
        {
          for (const auto & sched : scheds)
            {
              for (const auto & modeType : mode)
                {
                  for (const auto & uesPerBeam : uesPerBeamList)
                    {
                      for (const auto & beam : beams)
                        {
                          std::stringstream ss, schedName;
                          if (modeType == DL)
                            {
                              ss << "DL";
                            }
                          else if (modeType == UL)
                            {
                              ss << "UL";
                            }
                          else
                            {
                              ss << "DL_UL";
                            }
                          ss << ", Num " << num << ", " << subType << " " << sched << ", "
                             << uesPerBeam << " UE per beam, " << beam << " beam";
                          const bool isDl = modeType == DL || modeType == DL_UL;
                          const bool isUl = modeType == UL || modeType == DL_UL;

                          schedName << "ns3::NrMacScheduler" << subType << sched;

                          AddTestCase (new SystemSchedulerTest (ss.str(), uesPerBeam, beam, num,
                                                                       20e6, isDl, isUl,
                                                                       schedName.str()),
                                       TestCase::QUICK);
                        }
                    }
                }
            }
        }
    }
}

static NrSystemTestSchedulerTdmaRrSuite schedTdmaRrSuite;

