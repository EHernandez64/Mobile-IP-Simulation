/*	ForeignAgent.c
	Enrique Hernandez
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <sys/time.h>           /* for gettimeofday() */
#include <stdlib.h>
#include <strings.h>

#define TRUE 1

/* This Program Receives a packet from the HA from 2 ports
	One port for each Foreign Agent both Foreign Agents
	Forward the packet to the mobile node
 */

main(int argc,char *argv[])
{
  int sock, sock2, length;
  struct sockaddr_in source, dest;
  struct sockaddr_in source2, dest2;
  struct hostent *hp, *gethostbyname();
  int msgsock;
  char buf[1024];
  int rval,rval2, srclen, srclen2, sval;
  int i, seqno,FAnum,Pnum;
  long time;
  struct timeval rcvtime;
  struct timezone zone;
 
  /* check arguments */
  if (argc != 5) {
    printf("Usage: urecv4 FA_port1 FA_port2 MN_IP MN_Port\n");
    exit(0);
  }  
/* create socket */
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(sock < 0) {
    perror("opening datagram socket");
    exit(1);
  }
  /* name socket using wildcard for IP address and given port number */
  source.sin_family = AF_INET;
  source.sin_addr.s_addr = INADDR_ANY;
  source.sin_port = htons(atoi(argv[1]));
  if(bind(sock, (struct sockaddr *)&source, sizeof(source))) {
    perror("binding socket name");
    exit(1);
  }
  /* Second socket for different FA port*/

  sock2 = socket(AF_INET, SOCK_DGRAM, 0);
  if(sock2 < 0) {
    perror("opening datagram socket");
    exit(1);
  }
  source2.sin_family = AF_INET;
  source2.sin_addr.s_addr = INADDR_ANY;
  source2.sin_port = htons(atoi(argv[2]));
  if(bind(sock2, (struct sockaddr *)&source2, sizeof(source))) {
    perror("binding socket name");
    exit(1);
  }
  //Set destination to Mobile Node
  dest.sin_family = AF_INET;
  hp = gethostbyname(argv[3]);
  if(hp == 0) {
    printf("%s: unknown host\n",argv[3]);
    exit(2);
  }
  bcopy(hp->h_addr, &dest.sin_addr, hp->h_length);
  dest.sin_port = htons(atoi(argv[4]));

  /* find out assigned port numbers and print out */

  length = sizeof(dest);
  if(getsockname(sock, (struct sockaddr *)&source, &length)) {
    perror("getting socket name");
    exit(1);
  }
  printf("FASocket 1 has port #%d\n",ntohs(source.sin_port));
  printf("FASocket 2 has port #%d\n",ntohs(source2.sin_port));
  //For outputting IP Address in dotted decimal
  char str[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &dest.sin_addr, str, INET_ADDRSTRLEN);  
  //set that holds the sockets
  fd_set set;

  do {
      bzero(buf,sizeof(buf));
      srclen = sizeof(source);
	  srclen2 = sizeof(source2);
	  //Use Select to poll the sockets
	  FD_ZERO (&set);
	  FD_SET(sock, &set);
	  FD_SET(sock2, &set);

	  int returned = select(sock2 +1, &set, NULL,NULL,NULL);
	  //When a socket is ready it accepts that signal
	  if(FD_ISSET(sock, &set)){
		 rval = recvfrom(sock,buf,1024,0,(struct sockaddr *)&source,&srclen);
		 FAnum=1;
	  }
	  if(FD_ISSET(sock2, &set)){
		 rval2 = recvfrom(sock2,buf,1024,0,(struct sockaddr *)&source2,&srclen2);
		 FAnum=2;
	  }
	  //If a packet received forward to mobile node
	  if(rval > 0 || rval2 > 0){
      sscanf(buf,"%d %d", &seqno,&Pnum);
	  if ((sval = sendto(sock, buf, sizeof(buf), 0, (struct sockaddr *)&dest, sizeof(dest))) < 0) {
		  perror("writing on datagram socket");
	  }
	  printf("Received packet on Port# %d, SeqNo = %d Forwarded to: %s/%d\n",Pnum,seqno,str,ntohs(dest.sin_port));
      }
  } while (seqno != 60);
  close(sock);
}

