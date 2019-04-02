/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *   Copyright (c) 2018 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *   Copyright (c) 2015, NYU WIRELESS, Tandon School of Engineering, New York University
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
 *   Author: Marco Miozzo <marco.miozzo@cttc.es>
 *           Nicola Baldo  <nbaldo@cttc.es>
 *
 *   Modified by: Marco Mezzavilla < mezzavilla@nyu.edu>
 *                Sourjya Dutta <sdutta@nyu.edu>
 *                Russell Ford <russell.ford@nyu.edu>
 *                Menglei Zhang <menglei@nyu.edu>
 *                Biljana Bojovic <bbojovic@cttc.es>
 */

#ifndef SRC_MMWAVE_MODEL_MMWAVE_ENB_NET_DEVICE_H_
#define SRC_MMWAVE_MODEL_MMWAVE_ENB_NET_DEVICE_H_

#include "mmwave-net-device.h"
#include "ns3/event-id.h"
#include "ns3/mac48-address.h"
#include "ns3/traced-callback.h"
#include "ns3/nstime.h"
#include "mmwave-phy.h"
#include "mmwave-enb-phy.h"
#include "mmwave-enb-mac.h"
#include "mmwave-mac-scheduler.h"
#include <vector>
#include <ns3/lte-enb-rrc.h>
#include "ns3/lte-enb-component-carrier-manager.h"
#include "ns3/component-carrier-gnb.h"

namespace ns3 {
/* Add forward declarations here */
class Packet;
class PacketBurst;
class Node;
//class MmWavePhy;
class MmWaveEnbPhy;
class MmWaveEnbMac;


class MmWaveEnbNetDevice : public MmWaveNetDevice
{
public:
  static TypeId
  GetTypeId (void);

  MmWaveEnbNetDevice ();

  virtual ~MmWaveEnbNetDevice (void);

  virtual void DoDispose (void);

  virtual bool DoSend (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber);

  Ptr<MmWaveEnbMac> GetMac (uint8_t index);

  virtual Ptr<MmWavePhy> GetPhy (uint8_t index) const override;

  /**
   * Returns the PHY instance that is configured to operate on the provided
   * central carrier frequency
   * @param centerFrequency The central carrier frequency in Hz
   * @return A pointer to the PHY instance it it exist, otherwise a nullptr
   */
  virtual Ptr<MmWavePhy> GetPhyOnCenterFreq (double centerFrequency) const override;

  uint16_t GetCellId (uint8_t index) const;

  uint16_t GetCellId () const;

  uint16_t GetEarfcn (uint8_t index) const;

  void SetRrc (Ptr<LteEnbRrc> rrc);

  Ptr<LteEnbRrc> GetRrc (void);

  void SetCcMap (std::map<uint8_t, Ptr<ComponentCarrierGnb> > ccm);

  /**
   * \brief Get the size of the component carriers map
   * \return the number of cc that we have
   */
  uint32_t GetCcMapSize () const;

protected:
  virtual void DoInitialize (void);
  void UpdateConfig ();

private:
  Ptr<MmWaveMacScheduler> m_scheduler;

  Ptr<LteEnbRrc> m_rrc;

  uint16_t m_cellId; /* Cell Identifer. To uniquely identify an E-nodeB  */

  bool m_isConstructed;

  bool m_isConfigured;

  std::map<uint8_t, Ptr<ComponentCarrierGnb> > m_ccMap; /**< ComponentCarrier map */

  Ptr<LteEnbComponentCarrierManager> m_componentCarrierManager; ///< the component carrier manager of this eNb

};

}

#endif /* SRC_MMWAVE_MODEL_MMWAVE_ENB_NET_DEVICE_H_ */
