/*	HomeAgent.c
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

/* This Program acts as the Home Agent, It receives packets from
	the data source then forwards them to the FA with the current 
	CoA.
 */

main(int argc,char *argv[])
{
  int sock, length;
  struct sockaddr_in source, dest;
  struct hostent *hp, *gethostbyname();
  int msgsock;
  char buf[1024];
  char pNum[5];
  int rval, srclen,sval;
  int i,j, seqno, FAnum;
  long time;
  struct timeval rcvtime;
  struct timezone zone;
  /* check arguments */
  if (argc != 5) {
    printf("Usage: urecv4 HA_localport FA_IP FA_Port1 FA_port2\n");
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
  //Set destination to initial FA IP and Port#
  dest.sin_family = AF_INET;
  hp = gethostbyname(argv[2]);
  if(hp == 0) {
    printf("%s: unknown host\n",argv[2]);
    exit(2);
  }
  bcopy(hp->h_addr, &dest.sin_addr, hp->h_length);
  dest.sin_port = htons(atoi(argv[3]));

  /* find out assigned port number and print out */

  length = sizeof(dest);
  if(getsockname(sock, (struct sockaddr *)&source, &length)) {
    perror("getting socket name");
    exit(1);
  }
  printf("Socket has port #%d\n",ntohs(source.sin_port));
  //For printing out IP address in dotted decimal
  char str[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &dest.sin_addr, str, INET_ADDRSTRLEN);  

  do {
	  rval = 0;
      bzero(buf,sizeof(buf));
      srclen = sizeof(source);
      while ((rval = recvfrom(sock,buf,1024,0,(struct sockaddr *)&source,&srclen))<0){
        perror("receiver recvfrom");
      }

	  if(rval > 0){
		  sscanf(buf, "%d", &seqno);
	  //If the Packet is a Reg Packet (seqno less than 0) set a new dest port#
		  if(seqno < 0){
			  if(seqno == -1) //-1 means first port
				dest.sin_port = htons(atoi(argv[3]));

			  else
				dest.sin_port = htons(atoi(argv[4]));
			  
			  printf("Received Reg Packet, New CoA = %s/%d\n",str,ntohs(dest.sin_port));
			}
	// Otherwise the packet is forwarded to FA
			else{
				//insert port number into buff (to help out MN)
				sprintf(pNum,"%d",ntohs(dest.sin_port));

				if(seqno <= 9){
					buf[1] = ' ';
					j = 2;
				}
				else{
					buf[2] = ' ';
					j = 3;
				}
				for(i =0; i < 5; i++,j++){
					buf[j] = pNum[i];
				}
				//send the packet to current CoA
				 if ((sval = sendto(sock, buf, sizeof(buf), 0, (struct sockaddr *)&dest, sizeof(dest))) < 0) {
					perror("writing on datagram socket");
				 }
				printf("SeqNo = %d Forwarded to: %s/%d\n",seqno,str,ntohs(dest.sin_port)); 
				}
	}
  } while (seqno != 60);
  close(sock);
}

