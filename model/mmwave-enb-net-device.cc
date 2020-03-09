/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *   Copyright (c) 2019 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
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

#include "mmwave-enb-net-device.h"
#include <ns3/object-map.h>
#include <ns3/lte-enb-rrc.h>
#include <ns3/abort.h>
#include <ns3/log.h>
#include <ns3/lte-enb-component-carrier-manager.h>
#include <ns3/ipv4-l3-protocol.h>

#include "bandwidth-part-gnb.h"
#include "mmwave-enb-mac.h"
#include "mmwave-enb-phy.h"
#include "bwp-manager-gnb.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MmWaveEnbNetDevice");

NS_OBJECT_ENSURE_REGISTERED ( MmWaveEnbNetDevice);

TypeId
MmWaveEnbNetDevice::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::MmWaveEnbNetDevice").SetParent<MmWaveNetDevice> ()
    .AddConstructor<MmWaveEnbNetDevice> ()
    .AddAttribute ("LteEnbComponentCarrierManager",
                   "The component carrier manager associated to this EnbNetDevice",
                   PointerValue (),
                   MakePointerAccessor (&MmWaveEnbNetDevice::m_componentCarrierManager),
                   MakePointerChecker <LteEnbComponentCarrierManager> ())
    .AddAttribute ("BandwidthPartMap", "List of Bandwidth Part container.",
                   ObjectMapValue (),
                   MakeObjectMapAccessor (&MmWaveEnbNetDevice::m_ccMap),
                   MakeObjectMapChecker<BandwidthPartGnb> ())
    .AddAttribute ("LteEnbRrc", "The RRC layer associated with the ENB", PointerValue (),
                   MakePointerAccessor (&MmWaveEnbNetDevice::m_rrc),
                   MakePointerChecker<LteEnbRrc> ())
    ;
  return tid;
}

MmWaveEnbNetDevice::MmWaveEnbNetDevice ()
  : m_cellId (0),
  m_isConstructed (false),
  m_isConfigured (false)
{
  NS_LOG_FUNCTION (this);
}

MmWaveEnbNetDevice::~MmWaveEnbNetDevice ()
{
  NS_LOG_FUNCTION (this);
}

void
MmWaveEnbNetDevice::SetCcMap (std::map< uint8_t, Ptr<BandwidthPartGnb> > ccm)
{
  NS_ASSERT_MSG (!m_isConfigured, "attempt to set CC map after configuration");
  m_ccMap = ccm;
}

uint32_t
MmWaveEnbNetDevice::GetCcMapSize() const
{
  return static_cast<uint32_t> (m_ccMap.size ());
}

void
MmWaveEnbNetDevice::RouteIngoingCtrlMsgs (const std::list<Ptr<MmWaveControlMessage> > &msgList,
                                          uint8_t sourceBwpId)
{
  NS_LOG_FUNCTION (this);

  for (const auto & msg : msgList)
    {
      uint8_t bwpId = DynamicCast<BwpManagerGnb> (m_componentCarrierManager)->RouteIngoingCtrlMsgs (msg, sourceBwpId);
      m_ccMap.at (bwpId)->GetPhy ()->PhyCtrlMessagesReceived (msg);
    }
}

void
MmWaveEnbNetDevice::RouteOutgoingCtrlMsgs (const std::list<Ptr<MmWaveControlMessage> > &msgList,
                                           uint8_t sourceBwpId)
{
  NS_LOG_FUNCTION (this);

  for (const auto & msg : msgList)
    {
      uint8_t bwpId = DynamicCast<BwpManagerGnb> (m_componentCarrierManager)->RouteOutgoingCtrlMsg (msg, sourceBwpId);
      m_ccMap.at (bwpId)->GetPhy ()->EncodeCtrlMsg (msg);
    }
}

void
MmWaveEnbNetDevice::DoInitialize (void)
{
  NS_LOG_FUNCTION (this);
  m_isConstructed = true;
  UpdateConfig ();

  std::map<uint8_t, Ptr<BandwidthPartGnb> >::iterator it;
  for (it = m_ccMap.begin (); it != m_ccMap.end (); ++it)
    {
      it->second->Initialize ();
    }

  m_rrc->Initialize ();

  m_componentCarrierManager->Initialize ();
  //m_phy->Initialize ();
}

void
MmWaveEnbNetDevice::DoDispose ()
{
  NS_LOG_FUNCTION (this);

  m_rrc->Dispose ();
  m_rrc = nullptr;

  m_componentCarrierManager->Dispose ();
  m_componentCarrierManager = nullptr;
  // ComponentCarrierEnb::DoDispose() will call DoDispose
  // of its PHY, MAC, FFR and scheduler instance
  for (uint8_t i = 0; i < m_ccMap.size (); i++)
    {
      m_ccMap.at (i)->Dispose ();
      m_ccMap.at (i) = nullptr;
    }
  MmWaveNetDevice::DoDispose ();
}

Ptr<MmWaveEnbMac>
MmWaveEnbNetDevice::GetMac (uint8_t index) const
{
  return m_ccMap.at (index)->GetMac ();
}

Ptr<MmWaveEnbPhy>
MmWaveEnbNetDevice::GetPhy (uint8_t index) const
{
  NS_LOG_FUNCTION (this);
  return m_ccMap.at (index)->GetPhy ();
}

uint16_t
MmWaveEnbNetDevice::GetCellId () const
{
  NS_LOG_FUNCTION (this);
  return m_cellId;
}

uint16_t
MmWaveEnbNetDevice::GetCellId (uint8_t index) const
{
  NS_LOG_FUNCTION (this);
  return m_ccMap.at(index)->GetCellId ();
}

uint16_t
MmWaveEnbNetDevice::GetEarfcn (uint8_t index) const
{
  NS_LOG_FUNCTION (this);
  return m_ccMap.at (index)->GetDlEarfcn (); //Ul or Dl doesn't matter, they are the same

}

void
MmWaveEnbNetDevice::SetRrc (Ptr<LteEnbRrc> rrc)
{
  m_rrc = rrc;
}

Ptr<LteEnbRrc>
MmWaveEnbNetDevice::GetRrc (void)
{
  return m_rrc;
}

bool
MmWaveEnbNetDevice::DoSend (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION (this << packet   << dest << protocolNumber);
  NS_ASSERT_MSG (protocolNumber == Ipv4L3Protocol::PROT_NUMBER, "unsupported protocol " << protocolNumber << ", only IPv4 is supported");
  return m_rrc->SendData (packet);
}

void
MmWaveEnbNetDevice::UpdateConfig (void)
{
  NS_LOG_FUNCTION (this);

  if (m_isConstructed)
    {
      if (!m_isConfigured)
        {
          NS_LOG_LOGIC (this << " Configure cell " << m_cellId);
          // we have to make sure that this function is called only once
          NS_ASSERT (!m_ccMap.empty ());

          std::map < uint8_t, Ptr<ComponentCarrierBaseStation> > ccPhyConfMap;
          for (auto i:m_ccMap)
            {
              Ptr<ComponentCarrierBaseStation> c = i.second;
              ccPhyConfMap.insert (std::pair<uint8_t, Ptr<ComponentCarrierBaseStation> > (i.first,c));
            }

          m_rrc->ConfigureCell (ccPhyConfMap);
          m_isConfigured = true;
        }
      //m_rrc->SetCsgId (m_csgId, m_csgIndication);
    }
  else
    {
      /*
       * Lower layers are not ready yet, so do nothing now and expect
       * ``DoInitialize`` to re-invoke this function.
       */
    }
}

}
