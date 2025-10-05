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
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include <time.h>
#include <iostream>
#include "../src/internet/model/mp-tcp-socket-base.h"
#include <vector>
#undef B
#include <onnxruntime_cxx_api.h>
// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1   n2   n3   n4
//    point-to-point  |    |    |    |
//                    ================
//                      LAN 10.1.2.0

using namespace ns3;

std::vector<double> vec;
std::vector<double> vec2;
uint32_t sim_count = 0;

void OnSendComplete() {   
  std::cout << "sending time:" << endtime  << " seconds" << std::endl;
  vec.push_back(endtime);
}
/*
void CheckTxCompletion(Ptr<BulkSendApplication> bulkApp, uint32_t Npacket)
{
 if(bulkApp->GetTotalTxBytes() >= Npacket * 1000) {
  std::cout << "Sending time:" << Simulator::Now().GetSeconds() << "seconds" << std::endl;
 } else {
  Simulator::Schedule(Seconds(1.0), &CheckTxCompletion, bulkApp, Npacket);
 }
}
*/
/*
void OnSocketCloseTrace()
 { 
  std::cout << endtime << "CoseMulti_??????" << std::endl;
 }
*/
NS_LOG_COMPONENT_DEFINE("MpTcoBulkSendExample");
int
main (int argc, char *argv[])
{ 
  vari = 20;
  packet_size = 500;
  std::cout << vari << std::endl;
  //LogComponentEnable("MpTcpSocketBase", LOG_INFO);
//  uint32_t Npacket = 1024;
  //uint32_t run=1;
  double Npacket=1000;
  double buffer=50;
  double speed = 5;
  double delay = 15;
  gsam = 0; //If gsam is 1, you can enter the fase of smooth transition.
  uint32_t sabflows = 1;
  double loss = 0;
  endtime = 0;  
  
  CommandLine cmd;
  cmd.AddValue("Npacket", "number of packets", Npacket);
  cmd.AddValue("speed", "bps of the bottleneck link (Mbps)", speed);
  cmd.AddValue("buffer", "buffer size of routers", buffer);
  cmd.AddValue("delay", "transfer delay (ms)", delay);
  cmd.AddValue("sabflows", "number of sabflows", sabflows);
  cmd.AddValue("loss", "packet loss rate", loss);
  cmd.AddValue("gsam", "whether entering the fase of smooth transition or not", gsam);
  cmd.Parse(argc, argv);
  
  std::vector<double> input_data = {Npacket, speed, buffer, delay, loss};
  
  Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");
  Ort::SessionOptions session_options;
  Ort::Session session(env, "rf_model3.onnx", session_options);
  
  Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
  std::array<int64_t, 2> input_shape = {1, 5};
  Ort::Value input_tensor = Ort::Value::CreateTensor<double>(memory_info, input_data.data(), input_data.size(), input_shape.data(), input_shape.size());

  Ort::AllocatorWithDefaultOptions allocator;
  auto input_name_ptr = session.GetInputNameAllocated(0, allocator);
  const char* input_name_c_str = input_name_ptr.get();
  std::string input_name = input_name_c_str;

  auto output_name_ptr = session.GetOutputNameAllocated(0, allocator);
  const char* output_name_c_str = output_name_ptr.get();
  std::string output_name = output_name_c_str;

  std::vector<const char*> input_names = {input_name.c_str()};
  std::vector<const char*> output_names = {output_name.c_str()};
  auto output_tensors = session.Run(Ort::RunOptions{nullptr}, input_names.data(), &input_tensor, 1, output_names.data(), 1);
  double* output = output_tensors.front().GetTensorMutableData<double>();
  std::cout << "Predicted class: " << output[0] << std::endl;
/*
  py::module sys = py::module::import("sys");
  sys.attr("path").attr("append")("./my_model");
  py::scoped_interpreter guard{};
  py::module predict_module = py::module::import("predict_rf");
  py::object result = predict_module.attr("predict")(std::vector<double>{Npacket, speed, buffer, delay, loss});
  int label = result.cast<int>();
  std::cout << "Predicted label:" << label << std::endl; 
*/
 while(sim_count<4) {
 /* if(sim_count%2==0) {
   Npacket=1296;
   speed=23.0;
   buffer=49;
   delay=54.6;
   loss=0.000004;
   gsam=1; 
  } else {
   Npacket=1296;
   speed=23.0;
   buffer=49;
   delay=54.6;
   loss=0.000004;
   gsam=0;
  }
 */ 
  std::stringstream s;
  s << speed << "Mbps";
  std::string sp = s.str();
  std::stringstream d;
  d << delay << "ms";
  std::string de = d.str();
 
  Config::SetDefault("ns3::Ipv4GlobalRouting::FlowEcmpRouting", BooleanValue(true));
  Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(1400));
  Config::SetDefault("ns3::TcpSocket::DelAckCount", UintegerValue(0));
  Config::SetDefault("ns3::DropTailQueue::Mode", StringValue("QUEUE_MODE_PACKETS"));
  Config::SetDefault("ns3::DropTailQueue::MaxPackets", UintegerValue(buffer));
  Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(MpTcpSocketBase::GetTypeId()));
  //Config::SetDefault("ns3::TcpL4Protocol::SocketType", TypeIdValue(TcpSocketBase::GetTypeId()));
  Config::SetDefault("ns3::MpTcpSocketBase::MaxSubflows", UintegerValue(20)); // Sink
  //Config::SetDefault("ns3::MpTcpSocketBase::CongestionControl", StringValue("RTT_Compensator"));
  //Config::SetDefault("ns3::MpTcpSocketBase::PathManagement", StringValue("NdiffPorts"));
//  Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable>();
//  uv->SetStream(1); 
//  srand((unsigned int) time(NULL));
//  uint32_t seed = rand();
//  printf("%d???????????????????????????????????????????", b);
//  RngSeedManager::SetSeed(seed);
//  RngSeedManager::SetRun(run);
//  UniformVariable x(0,10);
//  uint32_t variable = x.GetValue();
//  ExponentialVariable y(2902);
//  uint32_t a = RngSeedManager::GetSeed(); 
//  printf("%d!!!!!!!!!!!!!!!!!!!!!!!!", variable);
//  printf("%driririrriri", run);



  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  NodeContainer p2pNodes2;
  p2pNodes2.Add(p2pNodes.Get(0));
  p2pNodes2.Create(1);

  NodeContainer p2pNodes3;
  p2pNodes3.Add(p2pNodes.Get(0));
  p2pNodes3.Create(1);

  NodeContainer p2pNodes4;
  p2pNodes4.Add(p2pNodes.Get(0));
  p2pNodes4.Create(1);


  NodeContainer p2pNodes5;
  p2pNodes5.Add(p2pNodes.Get(0));
  p2pNodes5.Create(1);


  NodeContainer p2pNodes6;
  p2pNodes6.Add(p2pNodes.Get(0));
  p2pNodes6.Create(1);

  NodeContainer p2pNodes7;
  p2pNodes7.Add(p2pNodes.Get(0));
  p2pNodes7.Create(1);

  NodeContainer p2pNodes8;
  p2pNodes8.Add(p2pNodes.Get(0));
  p2pNodes8.Create(1);

  NodeContainer csmaNodes;
  csmaNodes.Create(2);

  NodeContainer p2pNodes_2;
  p2pNodes_2.Add(p2pNodes.Get(1));
  p2pNodes_2.Add(csmaNodes.Get(0));

  NodeContainer p2pNodes2_2;
  p2pNodes2_2.Add(p2pNodes2.Get(1));
  p2pNodes2_2.Add(csmaNodes.Get(0));

  NodeContainer p2pNodes3_2;
  p2pNodes3_2.Add(p2pNodes3.Get(1));
  p2pNodes3_2.Add(csmaNodes.Get(0));

  NodeContainer p2pNodes4_2;
  p2pNodes4_2.Add(p2pNodes4.Get(1));
  p2pNodes4_2.Add(csmaNodes.Get(0));


  NodeContainer p2pNodes5_2;
  p2pNodes5_2.Add(p2pNodes5.Get(1));
  p2pNodes5_2.Add(csmaNodes.Get(0));


  NodeContainer p2pNodes6_2;
  p2pNodes6_2.Add(p2pNodes6.Get(1));
  p2pNodes6_2.Add(csmaNodes.Get(0));

  NodeContainer p2pNodes7_2;
  p2pNodes7_2.Add(p2pNodes7.Get(1));
  p2pNodes7_2.Add(csmaNodes.Get(0));

  NodeContainer p2pNodes8_2;
  p2pNodes8_2.Add(p2pNodes8.Get(1));
  p2pNodes8_2.Add(csmaNodes.Get(0));


  NodeContainer p2pNodes_3;
  p2pNodes_3.Add(csmaNodes.Get(1));
  p2pNodes_3.Create (1);

  NodeContainer p2pNodes2_3;
  p2pNodes2_3.Add(csmaNodes.Get(1));
  p2pNodes2_3.Create(1);

  NodeContainer p2pNodes3_3;
  p2pNodes3_3.Add(csmaNodes.Get(1));
  p2pNodes3_3.Create(1);

  NodeContainer p2pNodes4_3;
  p2pNodes4_3.Add(csmaNodes.Get(1));
  p2pNodes4_3.Create(1);


  NodeContainer p2pNodes5_3;
  p2pNodes5_3.Add(csmaNodes.Get(1));
  p2pNodes5_3.Create(1);


  NodeContainer p2pNodes6_3;
  p2pNodes6_3.Add(csmaNodes.Get(1));
  p2pNodes6_3.Create(1);

  NodeContainer p2pNodes7_3;
  p2pNodes7_3.Add(csmaNodes.Get(1));
  p2pNodes7_3.Create(1);

  NodeContainer p2pNodes8_3;
  p2pNodes8_3.Add(csmaNodes.Get(1));
  p2pNodes8_3.Create(1);



  NodeContainer p2pNodes_4;
  p2pNodes_4.Add(p2pNodes_3.Get(1));
  p2pNodes_4.Create (1);

  NodeContainer p2pNodes2_4;
  p2pNodes2_4.Add(p2pNodes2_3.Get(1));
  p2pNodes2_4.Add(p2pNodes_4.Get(1));

  NodeContainer p2pNodes3_4;
  p2pNodes3_4.Add(p2pNodes3_3.Get(1));
  p2pNodes3_4.Add(p2pNodes_4.Get(1));

  NodeContainer p2pNodes4_4;
  p2pNodes4_4.Add(p2pNodes4_3.Get(1));
  p2pNodes4_4.Add(p2pNodes_4.Get(1));


  NodeContainer p2pNodes5_4;
  p2pNodes5_4.Add(p2pNodes5_3.Get(1));
  p2pNodes5_4.Add(p2pNodes_4.Get(1));


  NodeContainer p2pNodes6_4;
  p2pNodes6_4.Add(p2pNodes6_3.Get(1));
  p2pNodes6_4.Add(p2pNodes_4.Get(1));

  NodeContainer p2pNodes7_4;
  p2pNodes7_4.Add(p2pNodes7_3.Get(1));
  p2pNodes7_4.Add(p2pNodes_4.Get(1));

  NodeContainer p2pNodes8_4;
  p2pNodes8_4.Add(p2pNodes8_3.Get(1));
  p2pNodes8_4.Add(p2pNodes_4.Get(1));


/*
  NodeContainer p2pNodes;
  p2pNodes.Create(2);

  NodeContainer csmaNodes;
  csmaNodes.Add(p2pNodes.Get(1));
  csmaNodes.Create(1);
*/

  PointToPointHelper subflow1;
  subflow1.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  subflow1.SetChannelAttribute("Delay", StringValue(de));
  PointToPointHelper subflow2;
  subflow2.SetDeviceAttribute("DataRate", StringValue(sp));
  subflow2.SetChannelAttribute("Delay", StringValue(de));
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("2.5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = subflow1.Install (p2pNodes);
  NetDeviceContainer p2pDevices2;
  p2pDevices2 = subflow1.Install(p2pNodes2);

  NetDeviceContainer p2pDevices3;
  p2pDevices3 = subflow1.Install (p2pNodes3);
  NetDeviceContainer p2pDevices4;
  p2pDevices4 = subflow1.Install(p2pNodes4);

  NetDeviceContainer p2pDevices5;
  p2pDevices5 = subflow1.Install (p2pNodes5);
  NetDeviceContainer p2pDevices6;
  p2pDevices6 = subflow1.Install(p2pNodes6);

  NetDeviceContainer p2pDevices7;
  p2pDevices7 = subflow1.Install (p2pNodes7);
  NetDeviceContainer p2pDevices8;
  p2pDevices8 = subflow1.Install(p2pNodes8);

  NetDeviceContainer p2pDevices_2;
  p2pDevices_2 = subflow1.Install (p2pNodes_2);
  NetDeviceContainer p2pDevices2_2;
  p2pDevices2_2 = subflow1.Install(p2pNodes2_2);

  NetDeviceContainer p2pDevices3_2;
  p2pDevices3_2 = subflow1.Install (p2pNodes3_2);
  NetDeviceContainer p2pDevices4_2;
  p2pDevices4_2 = subflow1.Install(p2pNodes4_2);

  NetDeviceContainer p2pDevices5_2;
  p2pDevices5_2 = subflow1.Install (p2pNodes5_2);
  NetDeviceContainer p2pDevices6_2;
  p2pDevices6_2 = subflow1.Install(p2pNodes6_2);

  NetDeviceContainer p2pDevices7_2;
  p2pDevices7_2 = subflow1.Install (p2pNodes7_2);
  NetDeviceContainer p2pDevices8_2;
  p2pDevices8_2 = subflow1.Install(p2pNodes8_2);

  NetDeviceContainer p2pDevices_3;
  p2pDevices_3 = subflow1.Install (p2pNodes_3);
  NetDeviceContainer p2pDevices2_3;
  p2pDevices2_3 = subflow1.Install(p2pNodes2_3);

  NetDeviceContainer p2pDevices3_3;
  p2pDevices3_3 = subflow1.Install (p2pNodes3_3);
  NetDeviceContainer p2pDevices4_3;
  p2pDevices4_3 = subflow1.Install(p2pNodes4_3);

  NetDeviceContainer p2pDevices5_3;
  p2pDevices5_3 = subflow1.Install (p2pNodes5_3);
  NetDeviceContainer p2pDevices6_3;
  p2pDevices6_3 = subflow1.Install(p2pNodes6_3);

  NetDeviceContainer p2pDevices7_3;
  p2pDevices7_3 = subflow1.Install (p2pNodes7_3);
  NetDeviceContainer p2pDevices8_3;
  p2pDevices8_3 = subflow1.Install(p2pNodes8_3);

  NetDeviceContainer p2pDevices_4;
  p2pDevices_4 = subflow1.Install (p2pNodes_4);
  NetDeviceContainer p2pDevices2_4;
  p2pDevices2_4 = subflow1.Install(p2pNodes2_4);

  NetDeviceContainer p2pDevices3_4;
  p2pDevices3_4 = subflow1.Install (p2pNodes3_4);
  NetDeviceContainer p2pDevices4_4;
  p2pDevices4_4 = subflow1.Install(p2pNodes4_4);

  NetDeviceContainer p2pDevices5_4;
  p2pDevices5_4 = subflow1.Install (p2pNodes5_4);
  NetDeviceContainer p2pDevices6_4;
  p2pDevices6_4 = subflow1.Install(p2pNodes6_4);

  NetDeviceContainer p2pDevices7_4;
  p2pDevices7_4 = subflow1.Install (p2pNodes7_4);
  NetDeviceContainer p2pDevices8_4;
  p2pDevices8_4 = subflow1.Install(p2pNodes8_4);
/*
  NetDeviceContainer p2pDevices;
  p2pDevices = subflow1.Install(p2pNodes);
  NetDeviceContainer p2pDevices2;
  p2pDevices2 = subflow1.Install(p2pNodes);
*/
//  NetDeviceContainer p2pDevices3;
//  p2pDevices3 = subflow1.Install(p2pNodes);

  RngSeedManager::SetSeed(12345);
  RngSeedManager::SetRun(1);
  Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
  em->SetRate (loss);
  p2pDevices_4.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("5Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (40000)));

  NetDeviceContainer csmaDevices;
  csmaDevices = subflow2.Install (csmaNodes); //デフォルトではn1-n2はLANで接続されていますが、エラーが出るのでp2pにしています。

  //NetDeviceContainer csmaDevices2;
  //csmaDevices2 = subflow1.Install (csmaNodes);
/*
  NetDeviceContainer p2pDevices3;
  p2pDevices3 = subflow1.Install(p2pNodes);

  NetDeviceContainer p2pDevices4;
  p2pDevices4 = subflow1.Install(p2pNodes);

  NetDeviceContainer p2pDevices5;
  p2pDevices5 = subflow1.Install(p2pNodes);

  NetDeviceContainer p2pDevices6;
  p2pDevices6 = subflow1.Install(p2pNodes);

  NetDeviceContainer p2pDevices7;
  p2pDevices7 = subflow1.Install(p2pNodes);

  NetDeviceContainer p2pDevices8;
  p2pDevices8 = subflow1.Install(p2pNodes);

  NetDeviceContainer p2pDevices9;
  p2pDevices9 = subflow1.Install(p2pNodes);

  NetDeviceContainer p2pDevices10;
  p2pDevices10 = subflow1.Install(p2pNodes);
*/


  InternetStackHelper stack;
  stack.Install (p2pNodes);
  stack.Install (p2pNodes2.Get(1));
  stack.Install (p2pNodes3.Get(1));
  stack.Install (p2pNodes4.Get(1));
  stack.Install (p2pNodes5.Get(1));
  stack.Install (p2pNodes6.Get(1));
  stack.Install (p2pNodes7.Get(1));
  stack.Install (p2pNodes8.Get(1));
  stack.Install (csmaNodes);
  stack.Install (p2pNodes_3.Get(1));
  stack.Install (p2pNodes2_3.Get(1));
  stack.Install (p2pNodes3_3.Get(1));
  stack.Install (p2pNodes4_3.Get(1));
  stack.Install (p2pNodes5_3.Get(1));
  stack.Install (p2pNodes6_3.Get(1));
  stack.Install (p2pNodes7_3.Get(1));
  stack.Install (p2pNodes8_3.Get(1));
  stack.Install (p2pNodes_4.Get(1));

/*
  InternetStackHelper stack;
  stack.Install(p2pNodes.Get(0));
  stack.Install(csmaNodes);
*/ 
  Ipv4AddressHelper address;
  Ipv4InterfaceContainer p2pInterfaces_4;
  
  if (sabflows==1){
  address.SetBase("192.168.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces = address.Assign(p2pDevices);
  
  address.SetBase("192.168.9.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces_2 = address.Assign(p2pDevices_2);
 
  address.SetBase("192.168.17.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces_3 = address.Assign(p2pDevices_3);
 
  address.SetBase("192.168.25.0", "255.255.255.0");
  p2pInterfaces_4 = address.Assign(p2pDevices_4); 
  }
  else if (sabflows==2){ 
  address.SetBase("192.168.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces = address.Assign(p2pDevices);
  address.SetBase("192.168.2.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces2 = address.Assign(p2pDevices2);
  
  address.SetBase("192.168.9.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces_2 = address.Assign(p2pDevices_2);
  address.SetBase("192.168.10.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces2_2 = address.Assign(p2pDevices2_2);
  
  address.SetBase("192.168.17.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces_3 = address.Assign(p2pDevices_3);
  address.SetBase("192.168.18.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces2_3 = address.Assign(p2pDevices2_3);
   
  address.SetBase("192.168.25.0", "255.255.255.0");
  p2pInterfaces_4 = address.Assign(p2pDevices_4);
  address.SetBase("192.168.26.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces2_4 = address.Assign(p2pDevices2_4);  
  }
  else if (sabflows==3){
  address.SetBase("192.168.3.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces = address.Assign(p2pDevices);
  address.SetBase("192.168.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces2 = address.Assign(p2pDevices2);
  address.SetBase("192.168.2.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces3 = address.Assign(p2pDevices3);
  
  address.SetBase("192.168.9.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces_2 = address.Assign(p2pDevices_2);
  address.SetBase("192.168.10.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces2_2 = address.Assign(p2pDevices2_2);
  address.SetBase("192.168.11.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces3_2 = address.Assign(p2pDevices3_2);
   
  address.SetBase("192.168.17.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces_3 = address.Assign(p2pDevices_3);
  address.SetBase("192.168.18.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces2_3 = address.Assign(p2pDevices2_3);
  address.SetBase("192.168.19.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces3_3 = address.Assign(p2pDevices3_3);
  
  address.SetBase("192.168.25.0", "255.255.255.0");
  p2pInterfaces_4 = address.Assign(p2pDevices_4);
  address.SetBase("192.168.26.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces2_4 = address.Assign(p2pDevices2_4);
  address.SetBase("192.168.27.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces3_4 = address.Assign(p2pDevices3_4);
  }
  else if (sabflows==4){
  address.SetBase("192.168.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces = address.Assign(p2pDevices);
  address.SetBase("192.168.2.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces2 = address.Assign(p2pDevices2);
  address.SetBase("192.168.3.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces3 = address.Assign(p2pDevices3);
  address.SetBase("192.168.4.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces4 = address.Assign(p2pDevices4);
 
  address.SetBase("192.168.9.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces_2 = address.Assign(p2pDevices_2);
  address.SetBase("192.168.10.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces2_2 = address.Assign(p2pDevices2_2);
  address.SetBase("192.168.11.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces3_2 = address.Assign(p2pDevices3_2);
  address.SetBase("192.168.12.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces4_2 = address.Assign(p2pDevices4_2); 
  
  address.SetBase("192.168.17.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces_3 = address.Assign(p2pDevices_3);
  address.SetBase("192.168.18.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces2_3 = address.Assign(p2pDevices2_3);
  address.SetBase("192.168.19.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces3_3 = address.Assign(p2pDevices3_3);
  address.SetBase("192.168.20.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces4_3 = address.Assign(p2pDevices4_3);
 
  address.SetBase("192.168.25.0", "255.255.255.0");
  p2pInterfaces_4 = address.Assign(p2pDevices_4);
  address.SetBase("192.168.26.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces2_4 = address.Assign(p2pDevices2_4);
  address.SetBase("192.168.27.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces3_4 = address.Assign(p2pDevices3_4);
  address.SetBase("192.168.28.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces4_4 = address.Assign(p2pDevices4_4);
  }
/*  
  Ipv4AddressHelper address;
  address.SetBase("192.168.3.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces = address.Assign(p2pDevices);
  address.SetBase("192.168.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces2 = address.Assign(p2pDevices2);
  address.SetBase("192.168.2.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces3 = address.Assign(p2pDevices3);
  address.SetBase("192.168.4.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces4 = address.Assign(p2pDevices4);
  address.SetBase("192.168.5.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces5 = address.Assign(p2pDevices5);
  address.SetBase("192.168.6.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces6 = address.Assign(p2pDevices6);
  address.SetBase("192.168.7.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces7 = address.Assign(p2pDevices7);
  address.SetBase("192.168.8.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces8 = address.Assign(p2pDevices8);


  address.SetBase("192.168.9.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces_2 = address.Assign(p2pDevices_2);
  address.SetBase("192.168.10.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces2_2 = address.Assign(p2pDevices2_2);
  address.SetBase("192.168.11.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces3_2 = address.Assign(p2pDevices3_2);
  address.SetBase("192.168.12.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces4_2 = address.Assign(p2pDevices4_2);
  address.SetBase("192.168.13.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces5_2 = address.Assign(p2pDevices5_3);
  address.SetBase("192.168.14.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces6_2 = address.Assign(p2pDevices6_2);
  address.SetBase("192.168.15.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces7_2 = address.Assign(p2pDevices7_2);
  address.SetBase("192.168.16.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces8_2 = address.Assign(p2pDevices8_2);


  address.SetBase("192.168.17.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces_3 = address.Assign(p2pDevices_3);
  address.SetBase("192.168.18.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces2_3 = address.Assign(p2pDevices2_3);
  address.SetBase("192.168.19.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces3_3 = address.Assign(p2pDevices3_3);
  address.SetBase("192.168.20.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces4_3 = address.Assign(p2pDevices4_3);
  address.SetBase("192.168.21.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces5_3 = address.Assign(p2pDevices5_3);
  address.SetBase("192.168.22.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces6_3 = address.Assign(p2pDevices6_3);
  address.SetBase("192.168.23.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces7_3 = address.Assign(p2pDevices7_3);
  address.SetBase("192.168.24.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces8_3 = address.Assign(p2pDevices8_3);

  address.SetBase("192.168.25.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces_4 = address.Assign(p2pDevices_4);
  address.SetBase("192.168.26.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces2_4 = address.Assign(p2pDevices2_4);
  address.SetBase("192.168.27.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces3_4 = address.Assign(p2pDevices3_4);
  address.SetBase("192.168.28.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces4_4 = address.Assign(p2pDevices4_4);
  address.SetBase("192.168.29.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces5_4 = address.Assign(p2pDevices5_4);
  address.SetBase("192.168.30.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces6_4 = address.Assign(p2pDevices6_4);
  address.SetBase("192.168.31.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces7_4 = address.Assign(p2pDevices7_4);  
  address.SetBase("192.168.32.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces8_4 = address.Assign(p2pDevices8_4);
*/

  address.SetBase("192.168.33.0", "255.255.255.0"); 
  Ipv4InterfaceContainer csmaInterfaces = address.Assign (csmaDevices);


/*
  Ipv4AddressHelper address;
  address.SetBase("192.168.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces = address.Assign(p2pDevices);
  address.SetBase("192.168.2.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces2 = address.Assign(p2pDevices2);
//  address.SetBase("192.168.4.0", "255.255.255.0");
//  Ipv4InterfaceContainer p2pInterfaces3 = address.Assign(p2pDevices3);
  
  address.SetBase("192.168.3.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces = address.Assign(csmaDevices);
*/ 

  //address.SetBase("192.168.4.0", "255.255.255.0");
  //Ipv4InterfaceContainer csmaInterfaces2 = address.Assign(csmaDevices2);
/*  address.SetBase("192.168.5.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces3 = address.Assign(p2pDevices3); //n0-n1
  address.SetBase("192.168.6.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces4 = address.Assign(p2pDevices4); //n1-n2
  address.SetBase("192.168.7.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces5 = address.Assign(p2pDevices5);
  address.SetBase("192.168.8.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces6 = address.Assign(p2pDevices6);
//address.SetBase("192.168.4.0", "255.255.255.0");
  //Ipv4InterfaceContainer csmaInterfaces2 = address.Assign(csmaDevices);
  //Ipv4InterfaceContainer csmaInterfaces2 = address.Assign(csmaDevices2);
  address.SetBase("192.168.9.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces7 = address.Assign(p2pDevices7); //n0-n1
  address.SetBase("192.168.10.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces8 = address.Assign(p2pDevices8); //n1-n2
  address.SetBase("192.168.11.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces9 = address.Assign(p2pDevices9);
  address.SetBase("192.168.12.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces10 = address.Assign(p2pDevices10);
*/
  uint16_t port = 50000 + sim_count * 100;

  MpTcpPacketSinkHelper sink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port));
  ApplicationContainer sinkApps = sink.Install(p2pNodes_4.Get(1));
  sinkApps.Start(Seconds(0.0));
  sinkApps.Stop(Seconds(1000.0));

  MpTcpBulkSendHelper source("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address(p2pInterfaces_4.GetAddress(1)), port));
  source.SetAttribute("MaxBytes", UintegerValue(int(Npacket * 1000)));
//  source.SetAttribute("OnTime", "ns3::UniformRandomVariable[Min=1.0][Max=5.00]");
  ApplicationContainer sourceApps = source.Install(p2pNodes.Get(0));
  sourceApps.Start(Seconds(0.1));
  sourceApps.Stop(Seconds(1000.0)); 

  //Config::ConnectWithoutContext("/NodeList/0/$ns3::TcpL4Protocol/SocketList/*/Close", MakeCallback(&OnSocketCloseTrace));
  Simulator::Schedule(Seconds(1000.0), &OnSendComplete);
   
 //bulkApp->GetSocket()->SetCloseCallbacks(
//    ns3::MakeNullCallback<void, ns3::Ptr<ns3::Socket> >(),
//    ns3::MakeCallback(&OnSendComplete)
//  );
 /*
//  Address srcAddress(InetSocketAddress(p2pInterfaces.GetAddress(0), port));
  OnOffHelper source("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address(p2pInterfaces_4.GetAddress(1)), port));
  source.SetAttribute("MaxBytes", UintegerValue(int(1*1024*1024)));
//  source.SetAttribute("DataRate", StringValue("450kbps"));
//  source.SetAttribute("PacketSize", UintegerValue(1024));
//  AddressValue remoteAddress(InetSocketAddress(p2pInterfaces_4.GetAddress(1), port));
//  source.SetAttribute("Remote", remoteAddress);
//  source.SetAttribute("OnTime", StringValue("ns3::UniformRandomVariable[Min=1.0][Max=5.0]"));
//  source.SetAttribute("OffTime", StringValue("ns3::UniformRandomVariable[Min=0][Max=0.8]"));
  ApplicationContainer sourceApps = source.Install(p2pNodes.Get(0));
  sourceApps.Start(Seconds(0.0));
  sourceApps.3top(Seconds(100.0));
*/
/*

  uint16_t port2 = 50000;

  MpTcpPacketSinkHelper sink2("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port2));
  ApplicationContainer sinkApps2 = sink2.Install(csmaNodes.Get(1));
  sinkApps2.Start(Seconds(0.0));
  sinkApps2.Stop(Seconds(100.0));

  MpTcpBulkSendHelper source2("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address(csmaInterfaces.GetAddress(1)), port2));
  source2.SetAttribute("MaxBytes", UintegerValue(int(1024*1024)));
  ApplicationContainer sourceApps2 = source2.Install(p2pNodes.Get(0));
  sourceApps2.Start(Seconds(0.0));
  sourceApps2.Stop(Seconds(100.0));


  uint16_t port3 = 50001;

  MpTcpPacketSinkHelper sink3("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port3));
  ApplicationContainer sinkApps3 = sink3.Install(csmaNodes.Get(1));
  sinkApps3.Start(Seconds(0.0));
  sinkApps3.Stop(Seconds(100.0));

  MpTcpBulkSendHelper source3("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address(csmaInterfaces.GetAddress(1)), port3));
  source3.SetAttribute("MaxBytes", UintegerValue(int(55*1024)));
  ApplicationContainer sourceApps3 = source3.Install(p2pNodes.Get(0));
  sourceApps3.Start(Seconds(0.0));
  sourceApps3.Stop(Seconds(100.0));


  uint16_t port4 = 50002;

  MpTcpPacketSinkHelper sink4("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port4));
  ApplicationContainer sinkApps4 = sink4.Install(csmaNodes.Get(1));
  sinkApps4.Start(Seconds(0.0));
  sinkApps4.Stop(Seconds(100.0));

  MpTcpBulkSendHelper source4("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address(csmaInterfaces.GetAddress(1)), port4));
  source4.SetAttribute("MaxBytes", UintegerValue(int(55*1024)));
  ApplicationContainer sourceApps4 = source4.Install(p2pNodes.Get(0));
  sourceApps4.Start(Seconds(0.0));
  sourceApps4.Stop(Seconds(100.0));


  uint16_t port5 = 50003;

  MpTcpPacketSinkHelper sink5("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port5));
  ApplicationContainer sinkApps5 = sink5.Install(csmaNodes.Get(1));
  sinkApps5.Start(Seconds(0.0));
  sinkApps5.Stop(Seconds(100.0));

  MpTcpBulkSendHelper source5("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address(csmaInterfaces.GetAddress(1)), port5));
  source5.SetAttribute("MaxBytes", UintegerValue(int(55*1024)));
  ApplicationContainer sourceApps5 = source5.Install(p2pNodes.Get(0));
  sourceApps5.Start(Seconds(0.0));
  sourceApps5.Stop(Seconds(100.0));


  uint16_t port6 = 50004;

  MpTcpPacketSinkHelper sink6("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port6));
  ApplicationContainer sinkApps6 = sink6.Install(csmaNodes.Get(1));
  sinkApps6.Start(Seconds(0.0));
  sinkApps6.Stop(Seconds(100.0));

  MpTcpBulkSendHelper source6("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address(csmaInterfaces.GetAddress(1)), port6));
  source6.SetAttribute("MaxBytes", UintegerValue(int(55*1024)));
  ApplicationContainer sourceApps6 = source6.Install(p2pNodes.Get(0));
  sourceApps6.Start(Seconds(0.0));
  sourceApps6.Stop(Seconds(100.0));


  uint16_t port7 = 50005;

  MpTcpPacketSinkHelper sink7("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port7));
  ApplicationContainer sinkApps7 = sink7.Install(csmaNodes.Get(1));
  sinkApps7.Start(Seconds(0.0));
  sinkApps7.Stop(Seconds(100.0));

  MpTcpBulkSendHelper source7("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address(csmaInterfaces.GetAddress(1)), port7));
  source7.SetAttribute("MaxBytes", UintegerValue(int(55*1024)));
  ApplicationContainer sourceApps7 = source7.Install(p2pNodes.Get(0));
  sourceApps7.Start(Seconds(0.0));
  sourceApps7.Stop(Seconds(100.0));


  uint16_t port8 = 50006;

  MpTcpPacketSinkHelper sink8("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port8));
  ApplicationContainer sinkApps8 = sink8.Install(csmaNodes.Get(1));
  sinkApps8.Start(Seconds(0.0));
  sinkApps8.Stop(Seconds(100.0));

  MpTcpBulkSendHelper source8("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address(csmaInterfaces.GetAddress(1)), port8));
  source8.SetAttribute("MaxBytes", UintegerValue(int(55*1024)));
  ApplicationContainer sourceApps8 = source8.Install(p2pNodes.Get(0));
  sourceApps8.Start(Seconds(0.0));
  sourceApps8.Stop(Seconds(100.0));



  uint16_t port9 = 50007;

  MpTcpPacketSinkHelper sink9("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port9));
  ApplicationContainer sinkApps9 = sink9.Install(csmaNodes.Get(1));
  sinkApps9.Start(Seconds(0.0));
  sinkApps9.Stop(Seconds(100.0));

  MpTcpBulkSendHelper source9("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address(csmaInterfaces.GetAddress(1)), port9));
  source9.SetAttribute("MaxBytes", UintegerValue(int(55*1024)));
  ApplicationContainer sourceApps9 = source9.Install(p2pNodes.Get(0));
  sourceApps9.Start(Seconds(0.0));
  sourceApps9.Stop(Seconds(100.0));


  uint16_t port10 = 50008;

  MpTcpPacketSinkHelper sink10("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port10));
  ApplicationContainer sinkApps10 = sink10.Install(csmaNodes.Get(1));
  sinkApps10.Start(Seconds(0.0));
  sinkApps10.Stop(Seconds(100.0));

  MpTcpBulkSendHelper source10("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address(csmaInterfaces.GetAddress(1)), port10));
  source10.SetAttribute("MaxBytes", UintegerValue(int(55*1024)));
  ApplicationContainer sourceApps10 = source10.Install(p2pNodes.Get(0));
  sourceApps10.Start(Seconds(0.0));
  sourceApps10.Stop(Seconds(100.0));

  uint16_t port11 = 50009;

  MpTcpPacketSinkHelper sink11("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port11));
  ApplicationContainer sinkApps11 = sink11.Install(csmaNodes.Get(1));
  sinkApps11.Start(Seconds(0.0));
  sinkApps11.Stop(Seconds(100.0));

  MpTcpBulkSendHelper source11("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address(csmaInterfaces.GetAddress(1)), port11));
  source11.SetAttribute("MaxBytes", UintegerValue(int(55*1024)));
  ApplicationContainer sourceApps11 = source11.Install(p2pNodes.Get(0));
  sourceApps11.Start(Seconds(0.0));
  sourceApps11.Stop(Seconds(100.0));

  uint16_t port12 = 50010;

  MpTcpPacketSinkHelper sink12("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port12));
  ApplicationContainer sinkApps12 = sink12.Install(csmaNodes.Get(1));
  sinkApps12.Start(Seconds(0.0));
  sinkApps12.Stop(Seconds(100.0));

  MpTcpBulkSendHelper source12("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address(csmaInterfaces.GetAddress(1)), port12));
  source12.SetAttribute("MaxBytes", UintegerValue(int(55*1024)));
  ApplicationContainer sourceApps12 = source12.Install(p2pNodes.Get(0));
  sourceApps12.Start(Seconds(0.0));
  sourceApps12.Stop(Seconds(100.0));

*/


  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  pointToPoint.EnablePcapAll ("second-re");

  Simulator::Run ();
//  std::cout << vec[0] << std::endl;
//  std::cout << vec2[0] << std::endl;
  Simulator::Destroy ();
//  Ptr<TcpL4Protocol> tcp = p2pNodes.Get(0)->GetObject<TcpL4Protocol>();
//  std::cout << "SocketList size:" << tcp->SocketListSize() << std::endl;
  ++sim_count;
  ++sabflows; 
 }
  uint32_t index=0;
  while(index<sim_count) {
   std::cout << vec[index] << std::endl;
   ++index;
  }
  return 0;
}
