/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
*   Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
*   Copyright (c) 2015 NYU WIRELESS, Tandon School of Engineering, New York University
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

#define NS_LOG_APPEND_CONTEXT                                            \
  do                                                                     \
    {                                                                    \
      if (m_phyMacConfig)                                                \
        {                                                                \
          std::clog << " [ CellId " << m_cellId << ", ccId "             \
                    << +m_phyMacConfig->GetCcId () << "] ";              \
        }                                                                \
    }                                                                    \
  while (false);
#include <ns3/simulator.h>
#include <ns3/callback.h>
#include <ns3/node.h>
#include <ns3/packet.h>
#include <ns3/log.h>
#include "mmwave-phy.h"
#include "mmwave-phy-sap.h"
#include "mmwave-mac-pdu-tag.h"
#include "mmwave-mac-pdu-header.h"
#include "mmwave-net-device.h"
#include <map>
#include "mmwave-spectrum-value-helper.h"
#include <sstream>
#include <vector>
#include <algorithm>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MmWavePhy");

NS_OBJECT_ENSURE_REGISTERED ( MmWavePhy);

/*   SAP   */
class MmWaveMemberPhySapProvider : public MmWavePhySapProvider
{
public:
  MmWaveMemberPhySapProvider (MmWavePhy* phy);

  virtual void SendMacPdu (Ptr<Packet> p ) override;

  virtual void SendControlMessage (Ptr<MmWaveControlMessage> msg) override;

  virtual void SendRachPreamble (uint8_t PreambleId, uint8_t Rnti) override;

  virtual void SetSlotAllocInfo (SlotAllocInfo slotAllocInfo) override;

  virtual AntennaArrayModel::BeamId GetBeamId (uint8_t rnti) const override;

private:
  MmWavePhy* m_phy;
};

MmWaveMemberPhySapProvider::MmWaveMemberPhySapProvider (MmWavePhy* phy)
  : m_phy (phy)
{
  //  Nothing more to do
}

void
MmWaveMemberPhySapProvider::SendMacPdu (Ptr<Packet> p)
{
  m_phy->SetMacPdu (p);
}

void
MmWaveMemberPhySapProvider::SendControlMessage (Ptr<MmWaveControlMessage> msg)
{
  m_phy->EnqueueCtrlMessage (msg);  //May need to change
}

void
MmWaveMemberPhySapProvider::SendRachPreamble (uint8_t PreambleId, uint8_t Rnti)
{
  m_phy->SendRachPreamble (PreambleId, Rnti);
}

void
MmWaveMemberPhySapProvider::SetSlotAllocInfo (SlotAllocInfo slotAllocInfo)
{
  m_phy->PushBackSlotAllocInfo (slotAllocInfo);
}

AntennaArrayModel::BeamId
MmWaveMemberPhySapProvider::GetBeamId (uint8_t rnti) const
{
  return m_phy->GetBeamId (rnti);
}

/* ======= */

TypeId
MmWavePhy::GetTypeId ()
{
  static TypeId
    tid =
    TypeId ("ns3::MmWavePhy")
    .SetParent<Object> ()
  ;

  return tid;
}

std::vector<int>
MmWavePhy::FromRBGBitmaskToRBAssignment (const std::vector<uint8_t> rbgBitmask) const
{
  NS_ASSERT (rbgBitmask.size () == m_phyMacConfig->GetBandwidthInRbg ());
  std::vector<int> ret;

  for (uint32_t i = 0; i < rbgBitmask.size (); ++i)
    {
      if (rbgBitmask.at (i) == 1)
        {
          for (uint32_t k = 0; k < m_phyMacConfig->GetNumRbPerRbg (); ++k)
            {
              ret.push_back ((i * m_phyMacConfig->GetNumRbPerRbg ()) + k);
            }
        }
    }

  NS_ASSERT (static_cast<uint32_t> (std::count (rbgBitmask.begin (), rbgBitmask.end (), 1) * m_phyMacConfig->GetNumRbPerRbg ())
             == ret.size ());
  return ret;
}

MmWavePhy::MmWavePhy ()
{
  NS_LOG_FUNCTION (this);
  NS_FATAL_ERROR ("This constructor should not be called");
}

MmWavePhy::MmWavePhy (Ptr<MmWaveSpectrumPhy> dlChannelPhy, Ptr<MmWaveSpectrumPhy> ulChannelPhy)
  : m_downlinkSpectrumPhy (dlChannelPhy),
  m_uplinkSpectrumPhy (ulChannelPhy),
  m_cellId (0),
  m_frameNum (0),
  m_subframeNum (0),
  m_slotNum (0),
  m_varTtiNum (0),
  m_antennaNumDim1 (0),
  m_antennaNumDim2 (0),
  m_antennaArrayType (AntennaArrayBasicModel::GetTypeId())
{
  NS_LOG_FUNCTION (this);
  m_phySapProvider = new MmWaveMemberPhySapProvider (this);
  m_antennaArray = nullptr;
}

MmWavePhy::~MmWavePhy ()
{
  NS_LOG_FUNCTION (this);
  m_slotAllocInfo.clear ();
}

void
MmWavePhy::InstallAntenna ()
{
  ObjectFactory antennaFactory = ObjectFactory ();
  antennaFactory.SetTypeId (m_antennaArrayType);
  m_antennaArray = antennaFactory.Create<AntennaArrayBasicModel>();
  m_antennaArray->SetAntennaNumDim1 (m_antennaNumDim1);
  m_antennaArray->SetAntennaNumDim2 (m_antennaNumDim2);

  Ptr<const SpectrumModel> sm = MmWaveSpectrumValueHelper::GetSpectrumModel(m_phyMacConfig->GetBandwidthInRbs(),
                                                                            m_phyMacConfig->GetCenterFrequency(),
                                                                            m_phyMacConfig->GetNumScsPerRb(),
                                                                            m_phyMacConfig->GetSubcarrierSpacing());
  m_antennaArray->SetSpectrumModel (sm);
}

void
MmWavePhy::DoDispose ()
{
  NS_LOG_FUNCTION (this);
  m_controlMessageQueue.clear ();
  delete m_phySapProvider;
  Object::DoDispose ();
}

void
MmWavePhy::SetDevice (Ptr<MmWaveNetDevice> d)
{
  NS_LOG_FUNCTION (this);
  m_netDevice = d;
}

Ptr<MmWaveNetDevice>
MmWavePhy::GetDevice ()
{
  NS_LOG_FUNCTION (this);
  return m_netDevice;
}

void
MmWavePhy::SetChannel (Ptr<SpectrumChannel> c)
{
  NS_LOG_FUNCTION (this);
}

void
MmWavePhy::DoSetCellId (uint16_t cellId)
{
  NS_LOG_FUNCTION (this);
  m_cellId = cellId;
  m_downlinkSpectrumPhy->SetCellId (cellId);
  m_uplinkSpectrumPhy->SetCellId (cellId);
}


void
MmWavePhy::SetNoiseFigure (double nf)
{
  NS_LOG_FUNCTION (this);
  m_noiseFigure = nf;
}

double
MmWavePhy::GetNoiseFigure (void) const
{
  NS_LOG_FUNCTION (this);
  return m_noiseFigure;
}

void
MmWavePhy::SendRachPreamble (uint32_t PreambleId, uint32_t Rnti)
{
  NS_LOG_FUNCTION (this);
  // This function is called by the SAP, SO it has to stay at the L1L2CtrlDelay rule
  m_raPreambleId = PreambleId;
  Ptr<MmWaveRachPreambleMessage> msg = Create<MmWaveRachPreambleMessage> ();
  msg->SetRapId (PreambleId);
  EnqueueCtrlMessage (msg); // Enqueue at the end
}

void
MmWavePhy::SetMacPdu (Ptr<Packet> p)
{
  NS_LOG_FUNCTION (this);
  MmWaveMacPduTag tag;
  if (p->PeekPacketTag (tag))
    {
      NS_ASSERT ((tag.GetSfn ().m_slotNum >= 0) && (tag.GetSfn ().m_varTtiNum < m_phyMacConfig->GetSymbolsPerSlot ()));

      std::map<uint64_t, Ptr<PacketBurst> >::iterator it = m_packetBurstMap.find (tag.GetSfn ().Encode ());

      if (it == m_packetBurstMap.end ())
        {
          it = m_packetBurstMap.insert (std::pair<uint64_t, Ptr<PacketBurst> > (tag.GetSfn ().Encode (), CreateObject<PacketBurst> ())).first;
        }
      it->second->AddPacket (p);
    }
  else
    {
      NS_FATAL_ERROR ("No MAC packet PDU header available");
    }
}

Ptr<PacketBurst>
MmWavePhy::GetPacketBurst (SfnSf sfn)
{
  NS_LOG_FUNCTION (this);
  Ptr<PacketBurst> pburst;
  std::map<uint64_t, Ptr<PacketBurst> >::iterator it = m_packetBurstMap.find (sfn.Encode ());

  if (it == m_packetBurstMap.end ())
    {
      NS_LOG_ERROR ("GetPacketBurst(): Packet burst not found for subframe " << (unsigned)sfn.m_subframeNum << " slot" << (unsigned) sfn.m_slotNum << " tti start "  << (unsigned)sfn.m_varTtiNum);
      return pburst;
    }
  else
    {
      pburst = it->second;
      m_packetBurstMap.erase (it);
    }
  return pburst;
}

Ptr<SpectrumValue>
MmWavePhy::GetNoisePowerSpectralDensity ()
{
  Ptr<const SpectrumModel> sm = MmWaveSpectrumValueHelper::GetSpectrumModel(m_phyMacConfig->GetBandwidthInRbs(),
                                                                      m_phyMacConfig->GetCenterFrequency(),
                                                                      m_phyMacConfig->GetNumScsPerRb(),
                                                                      m_phyMacConfig->GetSubcarrierSpacing());

  return MmWaveSpectrumValueHelper::CreateNoisePowerSpectralDensity(m_noiseFigure, sm);
}

Ptr<SpectrumValue>
MmWavePhy::GetTxPowerSpectralDensity (const std::vector<int> &rbIndexVector) const
{
  Ptr<const SpectrumModel> sm = MmWaveSpectrumValueHelper::GetSpectrumModel(m_phyMacConfig->GetBandwidthInRbs(),
                                                                           m_phyMacConfig->GetCenterFrequency(),
                                                                           m_phyMacConfig->GetNumScsPerRb(),
                                                                           m_phyMacConfig->GetSubcarrierSpacing());

  return MmWaveSpectrumValueHelper::CreateTxPowerSpectralDensity  (m_txPower, rbIndexVector, sm, m_phyMacConfig->GetBandwidth());
}

uint32_t
MmWavePhy::GetCcId() const
{
  if (m_phyMacConfig != nullptr)
    {
      return m_phyMacConfig->GetCcId ();
    }
  return 777;
}

void
MmWavePhy::EnqueueCtrlMessage (const Ptr<MmWaveControlMessage> &m)
{
  NS_LOG_FUNCTION (this);

  m_controlMessageQueue.at (m_controlMessageQueue.size () - 1).push_back (m);
}

void
MmWavePhy::EnqueueCtrlMsgNow (const Ptr<MmWaveControlMessage> &msg)
{
  NS_LOG_FUNCTION (this);

  m_controlMessageQueue.at (0).push_back (msg);
}

void
MmWavePhy::InitializeMessageList ()
{
  NS_LOG_FUNCTION (this);
  m_controlMessageQueue.clear ();

  for (unsigned i = 0; i <= m_phyMacConfig->GetL1L2CtrlLatency (); i++)
    {
      m_controlMessageQueue.push_back (std::list<Ptr<MmWaveControlMessage> > ());
    }
}


std::list<Ptr<MmWaveControlMessage> >
MmWavePhy::GetControlMessages (void)
{
  NS_LOG_FUNCTION (this);
  if (m_controlMessageQueue.empty ())
    {
      std::list<Ptr<MmWaveControlMessage> > emptylist;
      return (emptylist);
    }

  if (m_controlMessageQueue.at (0).size () > 0)
    {
      std::list<Ptr<MmWaveControlMessage> > ret = m_controlMessageQueue.front ();
      m_controlMessageQueue.erase (m_controlMessageQueue.begin ());
      std::list<Ptr<MmWaveControlMessage> > newlist;
      m_controlMessageQueue.push_back (newlist);
      return (ret);
    }
  else
    {
      m_controlMessageQueue.erase (m_controlMessageQueue.begin ());
      std::list<Ptr<MmWaveControlMessage> > newlist;
      m_controlMessageQueue.push_back (newlist);
      std::list<Ptr<MmWaveControlMessage> > emptylist;
      return (emptylist);
    }
}

Ptr<MmWavePhyMacCommon>
MmWavePhy::GetConfigurationParameters (void) const
{
  NS_LOG_FUNCTION (this);
  return m_phyMacConfig;
}


MmWavePhySapProvider*
MmWavePhy::GetPhySapProvider ()
{
  NS_LOG_FUNCTION (this);
  return m_phySapProvider;
}

void
MmWavePhy::PushBackSlotAllocInfo (const SlotAllocInfo &slotAllocInfo)
{
  NS_LOG_FUNCTION (this);

  NS_LOG_DEBUG ("ccId:" << static_cast<uint32_t> (GetCcId ()) <<
               " setting info for slot " << slotAllocInfo.m_sfnSf);

  // That's not so complex, as the list would typically be of 2 or 3 elements.
  bool updated = false;
  for (auto & alloc : m_slotAllocInfo)
    {
      if (alloc.m_sfnSf == slotAllocInfo.m_sfnSf)
        {
          NS_LOG_INFO ("Merging inside existing allocation");
          alloc.Merge (slotAllocInfo);
          updated = true;
          break;
        }
    }
  if (! updated)
    {
      m_slotAllocInfo.push_back (slotAllocInfo);
      m_slotAllocInfo.sort ();
      NS_LOG_INFO ("Pushing allocation at the end of the list");
    }

  std::stringstream output;

  for (const auto & alloc : m_slotAllocInfo)
    {
      output << alloc;
    }
  NS_LOG_INFO (output.str ());
}


bool
MmWavePhy::SlotAllocInfoExists (const SfnSf &retVal) const
{
  NS_LOG_FUNCTION (this);
  for (const auto & alloc : m_slotAllocInfo)
    {
      if (alloc.m_sfnSf == retVal)
        {
          return true;
        }
    }
  return false;
}

SlotAllocInfo
MmWavePhy::RetrieveSlotAllocInfo ()
{
  NS_LOG_FUNCTION (this);
  SlotAllocInfo ret;

  ret = *m_slotAllocInfo.begin ();
  m_slotAllocInfo.erase(m_slotAllocInfo.begin ());
  return ret;
}


SlotAllocInfo
MmWavePhy::RetrieveSlotAllocInfo (const SfnSf &sfnsf)
{
  NS_LOG_FUNCTION ("ccId:" << +GetCcId () << " slot " << sfnsf);

  SlotAllocInfo ret;

  for (auto allocIt = m_slotAllocInfo.begin(); allocIt != m_slotAllocInfo.end (); ++allocIt)
    {
      if (allocIt->m_sfnSf == sfnsf)
        {
          ret = *allocIt;
          m_slotAllocInfo.erase (allocIt);
          return ret;
        }
    }

  NS_FATAL_ERROR("Didn't found the slot");
  return SlotAllocInfo ();
}

SlotAllocInfo &
MmWavePhy::PeekSlotAllocInfo (const SfnSf &sfnsf)
{
  NS_LOG_FUNCTION (this);
  for (auto & alloc : m_slotAllocInfo)
    {
      if (alloc.m_sfnSf == sfnsf)
        {
          return alloc;
        }
    }

  NS_FATAL_ERROR ("Didn't found the slot");
}

size_t
MmWavePhy::SlotAllocInfoSize() const
{
  NS_LOG_FUNCTION (this);
  return m_slotAllocInfo.size ();
}

Ptr<AntennaArrayBasicModel>
MmWavePhy::GetAntennaArray () const
{
  return m_antennaArray;
}

void
MmWavePhy::SetAntennaArrayType (const TypeId antennaArrayTypeId)
{
  NS_ABORT_MSG_IF (m_antennaArray != nullptr, "Antenna's array type has been already configured. "
      "Antenna's type cannot be changed once that the anntena is created");
  m_antennaArrayType = antennaArrayTypeId ;
}

TypeId
MmWavePhy::GetAntennaArrayType () const
{
  return m_antennaArrayType;
}

void
MmWavePhy::SetAntennaNumDim1 (uint8_t antennaNumDim1)
{
  m_antennaNumDim1 = antennaNumDim1;
  // if the antennaArray is already created then forward the value to the antenna object
  if (m_antennaArray != nullptr)
    {
      m_antennaArray->SetAntennaNumDim1 (m_antennaNumDim1);
    }

}

uint8_t
MmWavePhy::GetAntennaNumDim1 () const
{
  return m_antennaNumDim1;
}

void
MmWavePhy::SetAntennaNumDim2 (uint8_t antennaNumDim2)
{
  m_antennaNumDim2 = antennaNumDim2;
  // if the antennaArray is already created then forward the value to the antenna object
  if (m_antennaArray != nullptr)
    {
      return m_antennaArray->SetAntennaNumDim2 (m_antennaNumDim2);
    }
}

uint8_t
MmWavePhy::GetAntennaNumDim2 () const
{
  return m_antennaNumDim2;
}

}
