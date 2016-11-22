#!/usr/bin/python

'''
example of using various TCIntf options.
reconfigures a single interface using intf.config()
to use different traffic control commands to test
bandwidth, loss, and delay
'''

from mininet.net import Mininet
from mininet.log import setLogLevel, info
from mininet.link import TCLink
from mininet.cli import CLI
from mininet.node import Controller

def intfOptions():
    "run various traffic control commands on a single interface"
    net = Mininet( controller=Controller )
    net.addController( 'c0' )
   
    # This topology is designed to mirror the topology from the IETF MBM presentation
    h1 = net.addHost( 'h1' )
    h2 = net.addHost( 'h2' )
    h3 = net.addHost( 'h3' )
    h4 = net.addHost( 'h4' )
    s1 = net.addSwitch( 's1' )
    s2 = net.addSwitch( 's2' )
    link1 = net.addLink( h1, s1, cls=TCLink )
    link2 = net.addLink( h2, s1, cls=TCLink )
    link3 = net.addLink( s1, s2, cls=TCLink )
    link4 = net.addLink( h3, s2, cls=TCLink )
    link5 = net.addLink( h4, s2, cls=TCLink )
    net.start()

    # Add some bandwidth constraints and delay and loss 
    link1.intf1.config( bw=200, delay='33ms', loss=0)
    link2.intf1.config( bw=200, delay='1ms', loss=0)
    link3.intf1.config( bw=100, delay='1ms', loss=0)
    link4.intf1.config( bw=200, delay='1ms', loss=0)
    link5.intf1.config( bw=200, delay='33ms', loss=0)

    # Run iperf3 on all hosts
    for host in [h1, h2, h3, h4]:
        host.cmd('iperf3 -s &')

    # Run mbm_server on all hosts    
    for host in [h1, h2, h3, h4]:
        host.cmd('../src/server/mbm_server &') 

    # Turn off segmentation offloads 
    h1.cmd('ethtool -K h1-eth0  gso off tso off gro off')
    h2.cmd('ethtool -K h2-eth0  gso off tso off gro off')
    h3.cmd('ethtool -K h3-eth0  gso off tso off gro off')
    h4.cmd('ethtool -K h4-eth0  gso off tso off gro off')

    # Iptables rules crafted to drop a single packet   
    # To delete this rule run 'h3 iptables -F'
    h3.cmd('iptables-restore iptables-rules')
    
    # Example command 
    info( '*** Try running\n' )
    info( '*** h2 iperf3 -c h3\n' )
    info( '*** or \n' )
    info( '*** h2 ../src/client/mbm_client -a h3 -p 5001 -r 20000 -t 50 -m 1500 -b 1 \n' )
    CLI( net )

    net.stop()

if __name__ == '__main__':
    setLogLevel( 'info' )
    intfOptions()
