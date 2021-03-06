/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2007 University of Washington
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef RON_CLIENT_H
#define RON_CLIENT_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "ns3/traced-callback.h"
#include "ns3/ron-header.h"

#include <list>
#include <set>
#include <vector>

namespace ns3 {

class Socket;
class Packet;

/**
 * \ingroup applications 
 * \defgroup ron Ron
 */

/**
 * \ingroup ron
 * \brief A Resilient Overlay Network client
 *
 * Attempts to send a packet to the server.  If failed, it will try to send it through
 * one of the overlay nodes.  It will try to forward packets from other overlay nodes
 * to the server.
 */
class RonClient : public Application 
{
public:
  static TypeId GetTypeId (void);
  RonClient ();
  virtual ~RonClient ();

  /**
   * \param ip destination ipv4 address
   * \param port destination port
   */
  void SetRemote (Ipv4Address ip, uint16_t port);

  /**
   * Set the data size of the packet (the number of bytes that are sent as data
   * to the server).  The contents of the data are set to unspecified (don't
   * care) by this call.
   *
   * \warning If you have set the fill data for the echo client using one of the
   * SetFill calls, this will undo those effects.
   *
   * \param dataSize The size of the echo data you want to sent.
   */
  void SetDataSize (uint32_t dataSize);

  /**
   * Get the number of data bytes that will be sent to the server.
   *
   * \warning The number of bytes may be modified by calling any one of the 
   * SetFill methods.  If you have called SetFill, then the number of 
   * data bytes will correspond to the size of an initialized data buffer.
   * If you have not called a SetFill method, the number of data bytes will
   * correspond to the number of don't care bytes that will be sent.
   *
   * \returns The number of data bytes.
   */
  uint32_t GetDataSize (void) const;

  /**
   * Set the data fill of the packet (what is sent as data to the server) to 
   * the zero-terminated contents of the fill string string.
   *
   * \warning The size of resulting echo packets will be automatically adjusted
   * to reflect the size of the fill string -- this means that the PacketSize
   * attribute may be changed as a result of this call.
   *
   * \param fill The string to use as the actual echo data bytes.
   */
  void SetFill (std::string fill);

  /**
   * Set the data fill of the packet (what is sent as data to the server) to 
   * the repeated contents of the fill byte.  i.e., the fill byte will be 
   * used to initialize the contents of the data packet.
   * 
   * \warning The size of resulting echo packets will be automatically adjusted
   * to reflect the dataSize parameter -- this means that the PacketSize
   * attribute may be changed as a result of this call.
   *
   * \param fill The byte to be repeated in constructing the packet data..
   * \param dataSize The desired size of the resulting echo packet data.
   */
  void SetFill (uint8_t fill, uint32_t dataSize);

  /**
   * Set the data fill of the packet (what is sent as data to the server) to
   * the contents of the fill buffer, repeated as many times as is required.
   *
   * Initializing the packet to the contents of a provided single buffer is 
   * accomplished by setting the fillSize set to your desired dataSize
   * (and providing an appropriate buffer).
   *
   * \warning The size of resulting echo packets will be automatically adjusted
   * to reflect the dataSize parameter -- this means that the PacketSize
   * attribute of the Application may be changed as a result of this call.
   *
   * \param fill The fill pattern to use when constructing packets.
   * \param fillSize The number of bytes in the provided fill pattern.
   * \param dataSize The desired size of the final echo data.
   */
  void SetFill (uint8_t *fill, uint32_t fillSize, uint32_t dataSize);

  /**
   * Add an overlay peer address to this nodes list.
   *
   * \param addr The IPv4 address of the peer to be added.
   */
  void AddPeer (Ipv4Address addr);

protected:
  virtual void DoDispose (void);

private:

  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void HandleRead (Ptr<Socket> socket);
  void SetTimeout (Time t);
  void Send (void);
  void ScheduleTransmit (Time dt);
  void CancelSends (void);

  void ForwardPacket (Ptr<Packet> packet, Ipv4Address source);
  void ProcessAck (Ptr<Packet> packet, Ipv4Address source);
  void CheckTimeout (uint32_t seq);
  void ScheduleTimeout (uint32_t seq);

  uint32_t m_count;
  Time m_interval;
  uint32_t m_size;

  uint32_t m_dataSize;
  uint8_t *m_data;

  Time m_timeout;
  Address m_local;

  uint32_t m_sent;
  Ipv4Address m_servAddress;
  Ipv4Address m_address;
  Ptr<Socket> m_socket;
  uint16_t m_port;
  std::list<EventId> m_sendEvents;
  std::set<uint32_t> m_outstandingSeqs;
  std::vector<Ipv4Address> m_peers;

  /// Callbacks for tracing
  TracedCallback<Ptr<const Packet>, uint32_t> m_sendTrace;
  TracedCallback<Ptr<const Packet>, uint32_t > m_ackTrace;
  TracedCallback<Ptr<const Packet>, uint32_t > m_forwardTrace;
};

} // namespace ns3

#endif /* RON_CLIENT_H */

