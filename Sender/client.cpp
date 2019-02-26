// client.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Reference: https://www.cs.rutgers.edu/~pxk/417/notes/sockets/udp.html

#include "pch.h"
#include <iostream>
#include <stdlib.h>
#include <WinSock2.h>		// Windows system Step 1
#include <sys/types.h>		// Step 3a
// #include <sys/socket.h>	// UNIX systems
#define PORT 1153
#define BUFSIZE 2048

int main()
{
	// Step 1 -- Create a socket.
	// A socket, s, is created with the scoekt system call.
	//
	//		int s = socket(domain, type, protocol)
	//
	// All the parameters as well as the return values are integers:
	//
	//	domain, or address family --
	//		communication domain in which the socket should be acreated. Some of address familires are AF_INET (IP). AF_INET6 (IPv6), AF_UNIX (local channel, similar to pipes), AF_ISO (ISO protocols), and AF_NS (Xerox Netowrk Sytsmes protocols).
	//
	//	type --
	//		type of service. This is selected according to the peoperies required by the application: SOCK_STREAM (virtual circuit srevice), SOCK_DGRAM (datagram service), SOCK_ROCK (direct IP service). Check with your address family to see wheter a 
	//		particular service is available.
	//
	//	protocol --
	//		inidicaite a specifici protocol to use in supporting the sockets aorperation. This is userful in  ccases where some familities may have smore than one protocaol to support a diven type of service. The return vcalue is a file descriptor(a smaill interg=er).
	//		The analogy of createing a socket is that of requectionf a relephone line from a phone company.
	//	
	//	For UPD/UP sockets, we want to specify the IP address family (AF_INET) and datagram service (SOCK_DGRAM). Since tehre's only one form of datagram service, ther eare no varitions of the protocol, so the last argoument, protocol, is zero. Our
	//	code for createing a UDP socket looks like this:
	int fd;
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("cannot create socket");
		return 0;
	}

	//	Step 2 -- Identify (name) a socket.
	//	When we talk about naming a socket, we are talking about assigning a transport address to the socket (a port number in IP networking). In sockets, this operation is called binding an address and the bhind system call isused to do this.
	//	The analogy is that of assigning a phone number to the line that you requestd from the pohone company in step 1 or that of assigning an address to a mailbox.
	//	
	//	The transport address is defined in a socket address structure. Because sockets were designed to work with various different types of communication interfaces, the interface is very general. Instead of accepting, say, a port number as a parameter, it
	//	It takes a sockaddr structure whose actual format is determined on the address family (type of network) you're using. For example, if you're using UNIX domain sockets, bind actually creates a file in the system.
	//	
	//	The system call for bind is:
	//
	//		#include <sys/socket.h>
	//
	//		int
	//		bind(int socket, const stuct sockaddr *address, socklen_t address_len);
	//
	//	The first parameter, socket, is the socket taht was created withthe socket system call.
	//
	//	For the second parameter, the structure sockaddr is a generic container taht just allows the OS to be able to read the first coule of bytes that identify the address family. The address family determiens that caraient of the sockaddr struct to use that 
	//	contains telements that make asnese for that specific communication type. For IP networking, we use struct sockaddr_in, which is defined in the header netinet/in.h. This structure defines:
	//		struct sockaddr_in {
	//			__uint8_t       sin_len;
	//			sa_family_t     sin_family;
	//			in_port_t       sin_port;
	//			struct  in_addr sin_addr;
	//			char            sin_zero[8];
	//		};
	//	Before calling bind, we need to fill out this structure.The three key parts we need to set are :
	//
	//	sin_family
	//		The address family we used when we set up the socket.In our case, it's AF_INET.
	//		sin_port
	//		The port number(the transport address).You can explicitly assign a transport address(port) or allow the operating system to assign one.If you're a client and don't need a well - known port that others can use to locate you(since they will only respond to your messages), you can just let the operating system pick any available port number by specifying port 0. If you're a server, you'll generally pick a specific number since clients will need to know a port number to which to address messages.
	//		sin_addr
	//		The address for this socket.This is just your machine's IP address. With IP, your machine will have one IP address for each network interface. For example, if your machine has both Wi-Fi and ethernet connections, that machine will have two addresses, one for each interface. Most of the time, we don't care to specify a specific interface and can let the operating system use whatever it wants.The special address for this is 0.0.0.0, defined by the symbolic constant INADDR_ANY.
	//		Since the address structure may differ based on the type of transport used, the third parameter specifies the length of that structure.This is simply the size of the internet address structure, sizeof(struct sockaddr_in).
	//
	//		The code to bind a socket looks like this :

	struct sockaddr_in myaddr;

	/* bind to an arbitrary return address */
	/* because this is the client side, we don't care about the address */
	/* since no application will initiate communication here - it will */
	/* just send responses */
	/* INADDR_ANY is the IP address and 0 is the socket */
	/* htonl converts a long integer (e.g. address) to a network representation */
	/* htons converts a short integer (e.g. port) to a network representation */
	
	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(0);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0)
	{
		perror("bind failed");
		return 0;
	}

	//	Step 3 --. Send a message to a server from a client.
	//	With TCP sockets, we had to establish a connection before we could communicate.With UDP, our sockets are connectionless.Hence, we can send messages immediately.Since we do not have a connection, the messages have to be addressed to their destination.Instead of using a write system call, we will use sendto, which allows us to specify the destination.The address is identified through the sockaddr structure, the same way as it is in bind and as we did when using connect for TCP sockets.
	//
	//	int
	//	sendto(int socket, const void *buffer, size_t length, int flags, const struct sockaddr *dest_addr,
	//		socklen_t dest_len)
	//	The first parameter, socket, is the socket that was created with the socket system call and named via bind.The second parameter, buffer, provides the starting address of the message we want to send.length is the number of bytes that we want to send.The flags parameter is 0 and not useful for UDP sockets.The dest_addr defines the destination address and port number for the message.It uses the same sockaddr_in structure that we used in bind to identify our local address.As with bind, the final parameter is simply the length of the address structure : sizeof(struct sockaddr_in).
	//
	//	The server's address will contain the IP address of the server machine as well as the port number that corresponds to a socket listening on that port on that machine. The IP address is a four-byte (32 bit) value in network byte order (see htonl above).
	//
	//	In most cases, you'll know the name of the machine but not its IP address. An easy way of getting the IP address is with the gethostbyname library (libc) function. Gethostbyname accepts a host name as a parameter and returns a hostent structure:
	//
	//
	//	struct  hostent {
	//	char    *h_name;        /* official name of host */
	//	char    **h_aliases;    /* alias list */
	//	int     h_addrtype;     /* host address type */
	//	int     h_length;       /* length of address */
	//	char    **h_addr_list;  /* list of addresses from name server */
	//	};
	//	If all goes well, the h_addr_list will contain a list of IP addresses.There may be more than one IP addresses for a host.In practice, you should be able to use any of the addresses or you may want to pick one that matches a particular subnet.You may want to check that(h_addrtype == AF_INET) and (h_length == 4) to ensure that you have a 32 - bit IPv4 address.We'll be lazy here and just use the first address in the list.

	// Here's the code for sending a message to the address of a machine in host. The variable fd is the socket which was created with the socket system call.

	struct hostent *hp;				// host information
	struct sockaddr_in servaddr;	// server address
	const char*  my_message = "This is a test message";

	// fill in the servier's address and data.
	memset((char*)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);

	// look up the address of the server given its name
	//hp = gethostbyname(host);
	//if (!hp) {
	//	fprintf(stderr, "could not obtain address of %s\n", host);
	//	return 0;
	//}
	const char *ipstr = "127.0.0.1";
	struct in_addr ip;
	struct hostent *hp;

	if (!inet_aton(ipstr, &ip))
		errx(1, "can't parse IP address %s", ipstr);

	hp = gethostbyaddr((const void *)&ip, sizeof ip, AF_INET)

	printf("name associated with %s is %s\n", ipstr, hp->h_name);

	// put host's address into the server address structure.
	memcpy((void *)&servaddr.sin_addr, hp->h_addr_list[0], hp->h_length);

	// send a message to the server.
	if (sendto(fd, my_message, strlen(my_message), 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("sendto failed");
		return 0;
	}

	// Step 3b. Receive messages on the server.
	//	With TCP sockets, a server would set up a socket for listening via a listen system call and then call accept to wait for a connection. UDP is connectionless. A server can immediately listen for 
	//	messages once it has a socket. We use the recvfrom system call to wait for an incoming datagram on a specific transport address(IP address and port number).
	//
	//	The recvfrom call has the following syntax :
	//
	//		#include <sys/socket.h>
	//
	//		int
	//		recvfrom(int socket, void *restrict buffer, size_t length, int flags, struct sockaddr *restrict src_addr,
	//				socklen_t *restrict *src_len)
	//	The first parameter, socket is a socket that we created ahead of time(and used bind. The port number assigned to that socket via the bind call tells us on what port recvfrom will wait for data.
	//	The incoming data will be placed into the memory at buffer and no more than length bytes will be transferred(that's the size of your buffer). We will ignore flags here. You can look at the man page 
	//	for recvfrom for details on this. This parameter allows us to process out-of-band data, peek at an incoming message without removing it from the queue, or block until the request is fully satisfied. 
	//	We can safely ignore these and use 0. The src_addr parameter is a pointer to a sockaddr structure that you allocate and will be filled in by recvfrom to identify the sender of the message. The length 
	//  of this structure will be stored in src_len. If you do not care to identify the sender, you can set both of these to zero but you will then have no way to reply to the sender.
	//
	//	The recvfrom call returns the number of bytes that were read into buffer
	//
	//	Let's examine a simple server. We'll create a socket, bind it to all available IP addresses on the machine but to a specific port number.Then we will loop, receiving messages and printing their contents.
	
	/* now loop, receiving data and printing what we received. */

	//	Step 4 -- Bidiectional communication
	//	We now have a client sending a message to a server. What if the server wants to send a message back to that client ? There is no connection so the server cannot just write the response back.Fortunately, 
	//	the recvfrom call gave us the address of the server.It was placed in remaddr :
	//  int recvlen = recvfrom(s, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
	// Step 5 -- Close the socket.
	//	With TCP sockets, we saw that we can use the shutdown system call to close a socket or to terminate communication in a single direction.Since there is no concept of a connection in UDP, there is no need to 
	//	call shutdown.However, the socket still uses up a file descriptor in the kernel, so we can free that up with the close system call just as we do with files.
	closesocket(fd);
}	
	

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
