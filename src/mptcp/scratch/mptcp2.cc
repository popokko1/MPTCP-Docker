/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 *
 * Author: Morteza Kheirkhah <m.kheirkhah@sussex.ac.uk>
 */

// Network topology
//
//       n0 ----------- n1
// - Flow from n0 to n1 using MpTcpBulkSendApplication.

#include <string>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink.h"
#include "ns3/csma-module.h"
#include "ns3/network-module.h"
#include "ns3/ipv4-global-routing-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("MpTcoBulkSendExample");

int
main(int argc, char *argv[])
{
  LogComponentEnable("MpTcpSocketBase", LOG_INFO);

  Config::SetDefault("ns3::Ipv4GlobalRouting::FlowEcmpRouting", BooleanValue(true));
  Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(1400));
  Config::SetDefault("ns3::TcpSocket::DelAckCount", UintegerValue(0));
  Config::SetDefault("ns3::DropTailQueue::Mode", StringValue("QUEUE_MODE_PACKETS"));
  Config::SetDefault("ns3::DropTailQueue::MaxPackets", UintegerValue(100));
  Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(MpTcpSocketBase::GetTypeId()));
  //Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TcpSocketBase::GetTypeId()));
  Config::SetDefault("ns3::MpTcpSocketBase::MaxSubflows", UintegerValue(8)); // Sink
  //Config::SetDefault("ns3::MpTcpSocketBase::CongestionControl", StringValue("RTT_Compensator"));
  //Config::SetDefault("ns3::MpTcpSocketBase::PathManagement", StringValue("NdiffPorts"));
  NodeContainer nodes;
  nodes.Create(2);

  NodeContainer tcpNodes;
  tcpNodes.Add(nodes.Get(1));
  tcpNodes.Create(1);
  

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
  pointToPoint.SetChannelAttribute("Delay", StringValue("40ms"));

  PointToPointHelper pointToPoint2;
  pointToPoint2.SetDeviceAttribute("DataRate", StringValue("3Mbps"));
  pointToPoint2.SetChannelAttribute("Delay", StringValue("60ms"));


  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));



  NetDeviceContainer devices;
  devices = pointToPoint.Install(nodes);
  NetDeviceContainer tcpDevices;
  tcpDevices = pointToPoint.Install(tcpNodes);
  NetDeviceContainer devices2;
  devices2 = pointToPoint2.Install(nodes);
  //NetDeviceContainer devices3;
  //devices3 = pointToPoint.Install(nodes);
  //NetDeviceContainer devices4;
  //devices4 = pointToPoint.Install(nodes);

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (tcpNodes);

  InternetStackHelper internet;
  internet.Install(nodes);
  internet.Install(tcpNodes.Get(1));

  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");

  Ipv4InterfaceContainer i = ipv4.Assign(devices);
  Ipv4InterfaceContainer i2 = ipv4.Assign(devices2);
  //Ipv4InterfaceContainer i3 = ipv4.Assign(devices3);
  //Ipv4InterfaceContainer i4 = ipv4.Assign(devices4);
  //Ipv4InterfaceContainer tcpI = ipv4.Assign(tcpDevices);
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = ipv4.Assign (csmaDevices);


  //Ipv4AddressHelper ipv4_2;
  //ipv4_2.SetBase("10.1.2.0", "255.255.255.0");
  //Ipv4InterfaceContainer i2 = ipv4_2.Assign(devices2);
  //Ipv4InterfaceContainer tcpI = ipv4_2.Assign(tcpDevices);
  //uint16_t AdvertisedWindowSize();
  //Ipv4AddressHelper ipv4_3;
  //ipv4_3.SetBase("10.1.3.0", "255.255.255.0");
  //Ipv4InterfaceContainer i3 = ipv4_3.Assign(devices3);

  uint16_t port = 9;
  
  MpTcpPacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
  ApplicationContainer sinkApps = sink.Install(tcpNodes.Get(1));
  sinkApps.Start(Seconds(0.0));
  sinkApps.Stop(Seconds(20.0));

  MpTcpBulkSendHelper source("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address(csmaInterfaces.GetAddress(1)), port));
  source.SetAttribute("MaxBytes", UintegerValue(int(5*1024*1024)));
  ApplicationContainer sourceApps = source.Install(nodes.Get(0));
  sourceApps.Start(Seconds(0.0));
  sourceApps.Stop(Seconds(20.0));

  //UdpEchoServerHelper echoServer(9);

  //ApplicationContainer serverApps = echoServer.Install(tcpNodes.Get(1));
  //serverApps.Start(Seconds(1.0));
  //serverApps.Stop(Seconds(10.0));

  //UdpClientHelper echoClient(tcpI.GetAddress(1), 9);
  //echoClient.SetAttribute("MaxPackets", UintegerValue(1));
  //echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
  //echoClient.SetAttribute("PacketSize", UintegerValue(1024));

  //ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
  //clientApps.Start(Seconds(2.0));
  //clientApps.Stop(Seconds(10.0));
 
  //Ipv4GlobalRoutingHelper::PopulateRoutingTables(); 

  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop(Seconds(20.0));
  pointToPoint.EnablePcapAll("mptcp2");
  Simulator::Run();
  Simulator::Destroy();
  NS_LOG_INFO ("Done.");

}
