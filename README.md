# Mobile-IP-Simulation

This is a socket program at the Application Layer which simulates Mobile IP. There are Four sub-programs which communicate and 
each simulate a real world entity. They are a Home Agent, Data Source, Foreign Agents, and Mobile Node.  

Mobile IP is typically implemented within the kernel of the average operating system (to make routing very fast.)  While 
extremely inefficient, similar functionality could be provided at the application layer.  Since it's much easier to code at 
the application layer, I will develop an application layer solution.

I have five entities to create, the Mobile Node, Home Agent, 2 Foreign Agents, and a Data Source.   We will assume that the 
Foreign Agents and Mobile Node processes run on the same physical machine. 

Since all the nodes are likely on the same network, we need to be a little creative about how we set this up.  
In Mobile IP, the Mobile Node's address must remain the same regardless of which link it is attached to.  We will assume that
the fixed home address consists of the port (and not the IP address.) 


HOW EACH PROGRAM WORKS-------------------------------------------------------------------------------------------------------

The Data Source opens a UDP port, sets the destination to the Home Agent IP/Mobile Node port, and sends a packet once per 
second,with increasing sequence numbers. The source IP/port do not matter.

The Home Agent opens a UDP port with source bound to Home Agent IP/Mobile Node port and sets destination to 
Foreign Agent IP/Foreign Agent port.  If it receives a packet on this port, it is either a registration request from the Mobile
Node or a data packet from the Data Source.  If it is a registration packet, the Home Agent changes the destination to Foreign 
Agent IP/ new Foreign Agent port.  If it is a data packet, it sends the packet to the current care-of address.  
(Note, that in real life, the HA would listen for registration packets on its own port, not the mobile node's.  It's easier to 
listen on one port than two, but you may do it either way.)

The 2 Foreign Agents open a UDP port, bind the source to Foreign Agent IP/Foreign Agent port, and set the destination to 
Foreign Agent IP/Mobile Node port.  (Remember that Foreign Agent IP = Mobile Node IP.)  Each time they rceive a packet, 
they forward it on to the mobile.

The Mobile Node opens a UDP port with source bound to Foreign Agent IP/Mobile Node port and destination set to Home Agent 
IP/Mobile Node port.  Every 5 seconds, it sends a registration request to the Home Agent with the alternate Foreign Agent's
care-of address in the packet.  (We assume that the Mobile Node knows the care-of addresses for the 2 Foreign Agents.)   
When it receives a packet, it must make sure that it has come from the right Foreign Agent (by comparing the source port to 
the currently registered Foreign Agent).  When the Mobile Node has run for 100 seconds it should exit. 

NOTE: I included a .txt file which shows the output of each program.
