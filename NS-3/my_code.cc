#include <fstream>
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-apps-module.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-routing-table-entry.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("RipSimpleRouting");

void TearDownLink (Ptr<Node> nodeA, Ptr<Node> nodeB, uint32_t interfaceA, uint32_t interfaceB)
{
  nodeA->GetObject<Ipv4> ()->SetDown (interfaceA);
  nodeB->GetObject<Ipv4> ()->SetDown (interfaceB);
}

int main (int argc, char **argv)
{
  bool verbose = false;
  bool printRoutingTables = false;
  bool showPings = false;
  std::string SplitHorizon ("PoisonReverse");

  CommandLine cmd;
  cmd.AddValue ("verbose", "turn on log components", verbose);
  cmd.AddValue ("printRoutingTables", "Print routing tables at 30, 60 and 90 seconds", printRoutingTables);
  cmd.AddValue ("showPings", "Show Ping6 reception", showPings);
  cmd.AddValue ("splitHorizonStrategy", "Split Horizon strategy to use (NoSplitHorizon, SplitHorizon, PoisonReverse)", SplitHorizon);
  cmd.Parse (argc, argv);

  if (verbose)
    {
      LogComponentEnableAll (LogLevel (LOG_PREFIX_TIME | LOG_PREFIX_NODE));
      LogComponentEnable ("RipSimpleRouting", LOG_LEVEL_INFO);
      LogComponentEnable ("Rip", LOG_LEVEL_ALL);
      LogComponentEnable ("Ipv4Interface", LOG_LEVEL_ALL);
      LogComponentEnable ("Icmpv4L4Protocol", LOG_LEVEL_ALL);
      LogComponentEnable ("Ipv4L3Protocol", LOG_LEVEL_ALL);
      LogComponentEnable ("ArpCache", LOG_LEVEL_ALL);
      LogComponentEnable ("V4Ping", LOG_LEVEL_ALL);
    }

  if (SplitHorizon == "NoSplitHorizon")
    {
      Config::SetDefault ("ns3::Rip::SplitHorizon", EnumValue (RipNg::NO_SPLIT_HORIZON));
    }
  else if (SplitHorizon == "SplitHorizon")
    {
      Config::SetDefault ("ns3::Rip::SplitHorizon", EnumValue (RipNg::SPLIT_HORIZON));
    }
  else
    {
      Config::SetDefault ("ns3::Rip::SplitHorizon", EnumValue (RipNg::POISON_REVERSE));
    }

  NS_LOG_INFO ("Create nodes.");
  Ptr<Node> A = CreateObject<Node> ();
  Names::Add ("hostA", A);
  Ptr<Node> B = CreateObject<Node> ();
  Names::Add ("hostB", B);
  Ptr<Node> C = CreateObject<Node> ();
  Names::Add ("hostC", C);
  Ptr<Node> D = CreateObject<Node> ();
  Names::Add ("hostD", D);
  Ptr<Node> E = CreateObject<Node> ();
  Names::Add ("hostE", E);
  Ptr<Node> F = CreateObject<Node> ();
  Names::Add ("hostF", F);
  Ptr<Node> G = CreateObject<Node> ();
  Names::Add ("hostG", G);
  Ptr<Node> H = CreateObject<Node> ();
  Names::Add ("hostH", H);
  Ptr<Node> I = CreateObject<Node> ();
  Names::Add ("hostI", I);
  Ptr<Node> J = CreateObject<Node> ();
  Names::Add ("hostJ", J);
  Ptr<Node> K = CreateObject<Node> ();
  Names::Add ("hostK", K);

  Ptr<Node> R1 = CreateObject<Node> ();
  Names::Add ("RouterR1", R1);
  Ptr<Node> R2 = CreateObject<Node> ();
  Names::Add ("RouterR2", R2);
  Ptr<Node> R3 = CreateObject<Node> ();
  Names::Add ("RouterR3", R3);
  Ptr<Node> R4 = CreateObject<Node> ();
  Names::Add ("RouterR4", R4);
  Ptr<Node> R5 = CreateObject<Node> ();
  Names::Add ("RouterR5", R5);
  Ptr<Node> R6 = CreateObject<Node> ();
  Names::Add ("RouterR6", R6);

  NodeContainer net1 (A, R1);
  NodeContainer net2 (B, R1);
  NodeContainer net3 (C, R1);
  NodeContainer net4 (D, R2);
  NodeContainer net5 (E, R2);
  NodeContainer net6 (F, R2);
  NodeContainer net7 (G, R2);
  NodeContainer net8 (R1, R3);
  NodeContainer net9 (R2, R4);
  NodeContainer net10 (R3, R4);
  NodeContainer net11 (R3, R5);
  NodeContainer net12 (R4, R5);
  NodeContainer net13 (R4, R6);
  NodeContainer net14 (H, R5);
  NodeContainer net15 (I, R5);
  NodeContainer net16 (J, R6);
  NodeContainer net17 (K, R6);

  NodeContainer routers (R1, R2, R3, R4, R5);
  routers.Add(R6);
  NodeContainer nodes (A, B, C, D, E);
  nodes.Add(F);
  nodes.Add(G);
  nodes.Add(H);
  nodes.Add(I);
  nodes.Add(J);
  nodes.Add(K);


  NS_LOG_INFO ("Create channels.");
  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", DataRateValue (5000000));
  csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
  NetDeviceContainer ndc1 = csma.Install (net1);
  NetDeviceContainer ndc2 = csma.Install (net2);
  NetDeviceContainer ndc3 = csma.Install (net3);
  NetDeviceContainer ndc4 = csma.Install (net4);
  NetDeviceContainer ndc5 = csma.Install (net5);
  NetDeviceContainer ndc6 = csma.Install (net6);
  NetDeviceContainer ndc7 = csma.Install (net7);
  NetDeviceContainer ndc8 = csma.Install (net8);
  NetDeviceContainer ndc9 = csma.Install (net9);
  NetDeviceContainer ndc10 = csma.Install (net10);
  NetDeviceContainer ndc11 = csma.Install (net11);
  NetDeviceContainer ndc12 = csma.Install (net12);
  NetDeviceContainer ndc13 = csma.Install (net13);
  NetDeviceContainer ndc14 = csma.Install (net14);
  NetDeviceContainer ndc15 = csma.Install (net15);
  NetDeviceContainer ndc16 = csma.Install (net16);
  NetDeviceContainer ndc17 = csma.Install (net17);

  NS_LOG_INFO ("Create IPv4 and routing");
  RipHelper ripRouting;

  // Rule of thumb:
  // Interfaces are added sequentially, starting from 0
  // However, interface 0 is always the loopback...
  ripRouting.ExcludeInterface (R1, 1);    // Host A
  ripRouting.ExcludeInterface (R1, 2);    // Host B
  ripRouting.ExcludeInterface (R1, 3);    // Host C
  ripRouting.ExcludeInterface (R2, 1);    // Host D
  ripRouting.ExcludeInterface (R2, 2);    // Host E
  ripRouting.ExcludeInterface (R2, 3);    // Host F
  ripRouting.ExcludeInterface (R2, 4);    // Host G
  ripRouting.ExcludeInterface (R5, 3);    // Host H
  ripRouting.ExcludeInterface (R5, 4);    // Host I
  ripRouting.ExcludeInterface (R6, 2);    // Host J
  ripRouting.ExcludeInterface (R6, 3);    // Host K
 
  ripRouting.SetInterfaceMetric (R4, 3, 5);
  ripRouting.SetInterfaceMetric (R5, 2, 5);

  Ipv4ListRoutingHelper listRH;
  listRH.Add (ripRouting, 0);
//  Ipv4StaticRoutingHelper staticRh;
//  listRH.Add (staticRh, 5);

  InternetStackHelper internet;
  internet.SetIpv6StackInstall (false);
  internet.SetRoutingHelper (listRH);
  internet.Install (routers);

  InternetStackHelper internetNodes;
  internetNodes.SetIpv6StackInstall (false);
  internetNodes.Install (nodes);

  // Assign addresses.
  // The source and destination networks have global addresses
  // The "core" network just needs link-local addresses for routing.
  // We assign global addresses to the routers as well to receive
  // ICMPv6 errors.
  NS_LOG_INFO ("Assign IPv4 Addresses.");
  Ipv4AddressHelper ipv4;

  ipv4.SetBase (Ipv4Address ("10.0.0.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic1 = ipv4.Assign (ndc1);
 
  ipv4.SetBase (Ipv4Address ("10.0.1.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic2 = ipv4.Assign (ndc2);
 
  ipv4.SetBase (Ipv4Address ("10.0.2.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic3 = ipv4.Assign (ndc3);
 
  ipv4.SetBase (Ipv4Address ("10.0.3.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic4 = ipv4.Assign (ndc4);
 
  ipv4.SetBase (Ipv4Address ("10.0.4.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic5 = ipv4.Assign (ndc5);
 
  ipv4.SetBase (Ipv4Address ("10.0.5.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic6 = ipv4.Assign (ndc6);
 
  ipv4.SetBase (Ipv4Address ("10.0.6.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic7 = ipv4.Assign (ndc7);

  ipv4.SetBase (Ipv4Address ("10.0.7.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic8 = ipv4.Assign (ndc8);
 
  ipv4.SetBase (Ipv4Address ("10.0.8.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic9 = ipv4.Assign (ndc9);
 
  ipv4.SetBase (Ipv4Address ("10.0.9.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic10 = ipv4.Assign (ndc10);
 
  ipv4.SetBase (Ipv4Address ("10.0.10.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic11 = ipv4.Assign (ndc11);
 
  ipv4.SetBase (Ipv4Address ("10.0.11.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic12 = ipv4.Assign (ndc12);
 
  ipv4.SetBase (Ipv4Address ("10.0.12.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic13 = ipv4.Assign (ndc13);
 
  ipv4.SetBase (Ipv4Address ("10.0.13.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic14 = ipv4.Assign (ndc14);

  ipv4.SetBase (Ipv4Address ("10.0.14.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic15 = ipv4.Assign (ndc15);
 
  ipv4.SetBase (Ipv4Address ("10.0.15.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic16 = ipv4.Assign (ndc16);
 
  ipv4.SetBase (Ipv4Address ("10.0.16.0"), Ipv4Mask ("255.255.255.0"));
  Ipv4InterfaceContainer iic17 = ipv4.Assign (ndc17);

  Ptr<Ipv4StaticRouting> staticRouting;
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (A->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("10.0.0.2", 1 );   // Router R1
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (B->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("10.0.1.2", 1 );   // Router R1
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (C->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("10.0.2.2", 1 );   // Router R1
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (D->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("10.0.3.2", 1 );   // Router R2
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (E->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("10.0.4.2", 1 );   // Router R2
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (F->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("10.0.5.2", 1 );   // Router R2
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (G->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("10.0.6.2", 1 );   // Router R2
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (H->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("10.0.13.2", 1 );  // Router R5
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (I->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("10.0.14.2", 1 );  // Router R5
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (J->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("10.0.15.2", 1 );  // Router R6
  staticRouting = Ipv4RoutingHelper::GetRouting <Ipv4StaticRouting> (K->GetObject<Ipv4> ()->GetRoutingProtocol ());
  staticRouting->SetDefaultRoute ("10.0.16.2", 1 );  // Router R6
  

  if (printRoutingTables)
    {
      RipHelper routingHelper;

      Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> (&std::cout);

      routingHelper.PrintRoutingTableAt (Seconds (10.0), R1, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (10.0), R2, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (10.0), R3, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (10.0), R4, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (10.0), R5, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (10.0), R6, routingStream);
 
      routingHelper.PrintRoutingTableAt (Seconds (40.0), R1, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (40.0), R2, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (40.0), R3, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (40.0), R4, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (40.0), R5, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (40.0), R6, routingStream);
 
      routingHelper.PrintRoutingTableAt (Seconds (80.0), R1, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (80.0), R2, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (80.0), R3, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (80.0), R4, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (80.0), R5, routingStream);
      routingHelper.PrintRoutingTableAt (Seconds (80.0), R6, routingStream);
    }

  NS_LOG_INFO ("Create Applications.");
  uint32_t packetSize = 1024;
  Time interPacketInterval = Seconds (1.0);
  V4PingHelper ping1 ("10.0.16.1");    // for ping to K
  V4PingHelper ping2 ("10.0.13.1");    // for ping to H
 
  ping1.SetAttribute ("Interval", TimeValue (interPacketInterval));
  ping1.SetAttribute ("Size", UintegerValue (packetSize));
  ping2.SetAttribute ("Interval", TimeValue (interPacketInterval));
  ping2.SetAttribute ("Size", UintegerValue (packetSize));
  if (showPings)
    {
      ping1.SetAttribute ("Verbose", BooleanValue (true));
      ping2.SetAttribute ("Verbose", BooleanValue (true));
    }
  ApplicationContainer apps1 = ping1.Install (A);
  apps1.Start (Seconds (1.0));
  apps1.Stop (Seconds (80.0));

  ApplicationContainer apps2 = ping2.Install (G);
  apps2.Start (Seconds (1.0));
  apps2.Stop (Seconds (80.0));

  AsciiTraceHelper ascii;
  csma.EnableAsciiAll (ascii.CreateFileStream ("rip-simple-routing.tr"));
  csma.EnablePcapAll ("rip-simple-routing", true);

  Simulator::Schedule (Seconds (25), &TearDownLink, R3, R4, 2, 2);

  AnimationInterface anim ("rip-network.xml");
  /* Now, do the actual simulation. */
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds (90.0));
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");
}

