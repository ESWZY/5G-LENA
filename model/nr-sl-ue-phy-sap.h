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

#ifndef NR_SL_UE_PHY_SAP_H
#define NR_SL_UE_PHY_SAP_H

#include "nr-sl-phy-mac-common.h"

#include <stdint.h>
#include <ns3/ptr.h>
#include <ns3/nstime.h>
#include <ns3/packet-burst.h>


namespace ns3 {

/**
 * \ingroup nr
 *
 * Service Access Point (SAP) offered by the UE PHY to the UE MAC
 * for NR Sidelink
 *
 * This is the PHY SAP Provider, i.e., the part of the SAP that contains
 * the UE PHY methods called by the UE MAC
 */
class NrSlUePhySapProvider
{
public:
  /**
   * \brief Destructor
   */
  virtual ~NrSlUePhySapProvider ();

  //Sidelink Communication
  /**
   * \brief Ask the PHY the bandwidth in RBs
   *
   * \return the bandwidth in RBs
   */
  virtual uint32_t GetBwInRbs () const = 0;
  /**
   * \brief Get the slot period
   * \return the slot period (depend on the numerology)
   */
  virtual Time GetSlotPeriod () const = 0;
  /**
   * \brief Send NR Sidelink PSCCH MAC PDU
   * \param p The packet
   */
  virtual void SendPscchMacPdu (Ptr<Packet> p) = 0;
  /**
   * \brief Send NR Sidelink PSSCH MAC PDU
   * \param p The packet
   */
  virtual void SendPsschMacPdu (Ptr<Packet> p) = 0;
  /**
   * \brief Set the allocation info for NR SL slot in PHY
   * \param sfn The SfnSf
   * \param varTtiInfo The Variable TTI allocation info
   */
  virtual void SetNrSlVarTtiAllocInfo (const SfnSf &sfn, const NrSlVarTtiAllocInfo& varTtiInfo) = 0;

};


/**
 * \ingroup nr
 *
 * Service Access Point (SAP) offered by the UE MAC to the UE PHY
 * for NR Sidelink
 *
 * This is the PHY SAP User, i.e., the part of the SAP that contains the UE
 * MAC methods called by the UE PHY
*/
class NrSlUePhySapUser
{
public:
  /**
   * \brief Destructor
   */
  virtual ~NrSlUePhySapUser ();

  /**
   * \brief Gets the active Sidelink pool id used for transmission and reception
   *
   * \return The active TX pool id
   */
  virtual uint8_t GetSlActiveTxPoolId () = 0;
  /**
   * \brief Get the list Sidelink destination from UE MAC
   * \return A vector holding Sidelink communication destinations and the highest priority value among its LCs
   */
  virtual std::vector <std::pair<uint32_t, uint8_t> > GetSlDestinations () = 0;
  /**
   * \brief Receive NR SL PSSCH PHY PDU
   * \return pbu The NR SL PSSCH PHY PDU
   */
  virtual void ReceivePsschPhyPdu (Ptr<PacketBurst> pdu) = 0;
  /**
   * \brief Receive the sensing information from PHY to MAC
   * \param sfn The SfnSf
   * \param rsvp The resource reservation period in ms
   * \param rbstart The PSSCH starting resource block
   * \param rbLen The PSCSCH length in number of RBs
   * \param prio The priority
   * \param slRsrp The measured RSRP value over the used resource blocks
   * \param gapReTx1 Gap for a first retransmission in absolute slots
   * \param gapReTx2 Gap for a second retransmission in absolute slots
   */
  virtual void ReceiveSensingData (const SfnSf &sfn, uint16_t rsvp,
                                   uint16_t rbStart, uint16_t rbLen,
                                   uint8_t prio, double slRsrp,
                                   uint8_t gapReTx1, uint8_t gapReTx2) = 0;
};


/**
 * \ingroup nr
 *
 * Template for the implementation of the NrSlUePhySapProvider as a member
 * of an owner class of type C to which all methods are forwarded.
 *
 * Usually, methods are forwarded to UE PHY class, which are called by UE MAC
 * to perform NR Sidelink.
 *
 */
template <class C>
class MemberNrSlUePhySapProvider : public NrSlUePhySapProvider
{
public:
  /**
   * \brief Constructor
   *
   * \param owner The owner class
   */
  MemberNrSlUePhySapProvider (C* owner);

  virtual uint32_t GetBwInRbs () const;
  virtual Time GetSlotPeriod () const;
  virtual void SendPscchMacPdu (Ptr<Packet> p);
  virtual void SendPsschMacPdu (Ptr<Packet> p);
  virtual void SetNrSlVarTtiAllocInfo (const SfnSf &sfn, const NrSlVarTtiAllocInfo& varTtiInfo);

  // methods inherited from NrSlUePhySapProvider go here
  //NR Sidelink communication

private:
  MemberNrSlUePhySapProvider ();
  C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlUePhySapProvider<C>::MemberNrSlUePhySapProvider (C* owner)
  : m_owner (owner)
{
}

template <class C>
uint32_t
MemberNrSlUePhySapProvider<C>::GetBwInRbs () const
{
  return m_owner->DoGetBwInRbs ();
}

template <class C>
Time
MemberNrSlUePhySapProvider<C>::GetSlotPeriod () const
{
  return m_owner->DoGetSlotPeriod ();
}

template <class C>
void
MemberNrSlUePhySapProvider<C>::SendPscchMacPdu (Ptr<Packet> p)
{
  m_owner->DoSendPscchMacPdu (p);
}

template <class C>
void
MemberNrSlUePhySapProvider<C>::SendPsschMacPdu (Ptr<Packet> p)
{
  m_owner->DoSendPsschMacPdu (p);
}

template <class C>
void
MemberNrSlUePhySapProvider<C>::SetNrSlVarTtiAllocInfo (const SfnSf &sfn, const NrSlVarTtiAllocInfo& varTtiInfo)
{
  m_owner->DoSetNrSlVarTtiAllocInfo (sfn, varTtiInfo);
}


/**
 * \ingroup nr
 *
 * Template for the implementation of the NrSlUePhySapUser as a member
 * of an owner class of type C to which all methods are forwarded.
 *
 * Usually, methods are forwarded to UE MAC class, which are called by UE PHY
 * to perform NR Sidelink.
 *
 */
template <class C>
class MemberNrSlUePhySapUser : public NrSlUePhySapUser
{
public:
  /**
   * \brief Constructor
   *
   * \param owner The owner class
   */
  MemberNrSlUePhySapUser (C* owner);

  // methods inherited from NrSlUePhySapUser go here
  virtual uint8_t GetSlActiveTxPoolId ();
  virtual std::vector <std::pair<uint32_t, uint8_t> > GetSlDestinations ();
  virtual void ReceivePsschPhyPdu (Ptr<PacketBurst> pdu);
  virtual void ReceiveSensingData (const SfnSf &sfn, uint16_t rsvp,
                                   uint16_t rbStart, uint16_t rbLen,
                                   uint8_t prio, double slRsrp,
                                   uint8_t gapReTx1, uint8_t gapReTx2);

private:
  C* m_owner; ///< the owner class
};

template <class C>
MemberNrSlUePhySapUser<C>::MemberNrSlUePhySapUser (C* owner)
  : m_owner (owner)
{
}

template <class C>
uint8_t
MemberNrSlUePhySapUser<C>::GetSlActiveTxPoolId ()
{
  return m_owner->DoGetSlActiveTxPoolId ();
}

template <class C>
std::vector <std::pair<uint32_t, uint8_t> >
MemberNrSlUePhySapUser<C>::GetSlDestinations ()
{
  return m_owner->DoGetSlDestinations ();
}

template <class C>
void
MemberNrSlUePhySapUser<C>::ReceivePsschPhyPdu (Ptr<PacketBurst> pdu)
{
  m_owner->DoReceivePsschPhyPdu (pdu);
}

template <class C>
void
MemberNrSlUePhySapUser<C>::ReceiveSensingData (const SfnSf &sfn, uint16_t rsvp,
                                               uint16_t rbStart, uint16_t rbLen,
                                               uint8_t prio, double slRsrp,
                                               uint8_t gapReTx1, uint8_t gapReTx2)
{
  m_owner->DoReceiveSensingData (sfn, rsvp, rbStart, rbLen, prio, slRsrp, gapReTx1, gapReTx2);
}



} // namespace ns3


#endif // NR_SL_UE_PHY_SAP_H