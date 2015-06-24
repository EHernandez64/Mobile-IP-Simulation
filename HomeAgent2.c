/*				Urecv4.C
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
#define INET_ADDRSTRLEN 16
/* This is the receiver program. It opens a socket with the given port
 * and then begins an infinite loop. Each time through the loop it 
 * accepts a pkt and prints out the contents and the current time. 
 * Command line is `urecv4 portnumber'.  
 * The port number is a number between 1000 and 64000.
 */

main(int argc,char *argv[])
{
  int sock,sock2, length,length2;
  struct sockaddr_in source;
  struct sockaddr_in dest;
  struct hostent *hp, *gethostbyname();
 // struct hostent *hp2, *gethostbyname();
  int msgsock;
  char buf[1024];
  int rval, srclen, sval;
  int i, seqno;
  long time;
  struct timeval rcvtime;
  struct timezone zone;
 

  /* check arguments */
  if (argc != 3) {
    printf("Usage: urecv4 FAIpaddress FAport\n");
    exit(0);
  }  

/* create socket */

  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(sock < 0) {
    perror("opening datagram socket");
    exit(1);
  }

  /* name socket using wildcard for IP address and given port number */

  /* now bind source address. Port is wildcard, doesn't matter. */
  const char *sIP = "calculus";
  hp = gethostbyname(sIP);

  source.sin_family = AF_INET;
  bcopy(hp->h_addr, &source.sin_addr, hp->h_length);
  //source.sin_addr.s_addr = INADDR_ANY;
  source.sin_port = htons(50250);
  if (bind(sock, (struct sockaddr *)&source, sizeof(source)) < 0) {
    perror("binding socket name");
    exit(1);
  }

  //Create Sending package
  sock2 = socket(AF_INET, SOCK_DGRAM, 0);
  if(sock2 < 0) {
    perror("opening datagram socket");
    exit(1);
  }
  dest.sin_family = AF_INET;
 // hp2 = gethostbyname(argv[1]);
 // if(hp2 == 0) {
 //   printf("%s: unknown host\n",argv[1]);
 //   exit(2);
 // }
  dest.sin_addr.s_addr = inet_addr(argv[1]);
  dest.sin_port = htons(atoi(argv[2]));
  if (bind(sock2, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
    perror("binding socket name");
    exit(1);
  }
  /* find out assigned port number and print out */
  length = sizeof(dest);
   if(getsockname(sock2, (struct sockaddr *)&dest, &length)) {
   perror("getting socket name");
   exit(1);
  }
  length2 = sizeof(source);
  if(getsockname(sock, (struct sockaddr *)&source, &length2)) {
    perror("getting socket name");
    exit(1);
  }
  printf("Listening to port #%d\n",ntohs(source.sin_port));
  printf("Forwarding to port #%d\n",ntohs(dest.sin_port));

  char str[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &source.sin_addr, str, INET_ADDRSTRLEN);  
  printf("Source IP: %s\n",str);
  inet_ntop(AF_INET, &dest.sin_addr, str, INET_ADDRSTRLEN);
  printf("Destin IP: %s\n",str);

  do {
      bzero(buf,sizeof(buf));
      srclen = sizeof(source);
	  while ((rval = recvfrom(sock,buf,1024,0,(struct sockaddr *)&source,&srclen))<0){
				perror("receiver recvfrom");
			}
		  if( rval > 0){
			sval = sendto(sock2, buf, 1024, 0, (struct sockaddr *)&dest, sizeof(dest));

				 if(sval > 0){
					sscanf(buf, "%d", &seqno);
			 /* get current relative time 
			if (gettimeofday(&rcvtime, &zone) < 0) {
    			perror("getting time");
    			exit(1);
				}
				*/
					printf("Received packet, seqno = %d ",seqno); 
					printf("Forwarder to: %s/%d\n",str,ntohs(dest.sin_port));
			}
		  }
   } while (rval != 0);


  close(sock);
}

