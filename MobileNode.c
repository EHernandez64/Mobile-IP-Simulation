/*	MobileNode.c
	Enrique Hernandez
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>           /* for gettimeofday() */
#include <stdlib.h>
#include <strings.h>
#define TRUE 1

/* This program acts as the Mobile Node It receives packets
   From a foreign agent and checks to see if it matches with
   the current CoA. It also sends out a registration request
   with a new CoA to the Home Agent
 */
main(int argc,char *argv[])
{
  int sock, length;
  struct sockaddr_in source, dest;
  struct hostent *hp, *gethostbyname();
  int msgsock;
  char buf[1024], Rbuf[5], status[8];
  int rval,sval, srclen, Pnum,portno;
  int i, seqno, regNum;
  //long time;
  struct timeval rcvtime;
  struct timezone zone;
  /* check arguments */
  if (argc != 6) {
    printf("Usage: urecv4 localport HA_IP HA_Port FA_port1 FA_port2\n");
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
  if(bind(sock, (struct sockaddr *)&source, sizeof(dest))) {
    perror("binding socket name");
    exit(1);
  }
  /* find out assigned port number and print out */

  length = sizeof(source);
  if(getsockname(sock, (struct sockaddr *)&source, &length)) {
    perror("getting socket name");
    exit(1);
  }
 dest.sin_family = AF_INET;
 hp = gethostbyname(argv[2]);
  if(hp == 0) {
    printf("%s: unknown host\n",argv[2]);
    exit(2);
  }
  bcopy(hp->h_addr, &dest.sin_addr, hp->h_length);
  dest.sin_port = htons(atoi(argv[3]));

  printf("Socket has port #%d\n",ntohs(source.sin_port));
  //Getting start time
  time_t sTime,cTime;
  struct timeval tv;
  gettimeofday(&tv, NULL);
  sTime = tv.tv_sec;

  int count = 0,FAnum; //To keep track of reg packets

  time_t mytime;  //To output time of received packet
  do {
	  //Getting current time
	  gettimeofday(&tv,NULL);
	  cTime = tv.tv_sec;
	  //Check if time to send REG packet
	  if((cTime - sTime) > 5){
		sTime = tv.tv_sec; //redefine start once 5 sec have gone by

		count++;
		//Create Reg packet and sent it
		//When even count change to FA1
		if(count % 2 == 0){
			regNum = -1; //num representing a port
			portno = atoi(argv[4]);
		}
		else{
			regNum = -2;
			portno = atoi(argv[5]);
		}
		printf("Send Reg packet: change to FA# %d\n",FAnum);
		sprintf(Rbuf, "%d %d\0", regNum, portno);
		//send the reg packet
		sval = sendto(sock, Rbuf, sizeof(Rbuf), 0, (struct sockaddr *)&dest, sizeof(dest));
	  }
      bzero(buf,sizeof(buf));

      srclen = sizeof(source);
      while ((rval = recvfrom(sock,buf,1024,0,(struct sockaddr *)&source,&srclen))<0){
        perror("receiver recvfrom");
      }
      sscanf(buf,"%d %d", &seqno,&Pnum);
      /* get current relative time */
	    mytime = time(NULL);
		//Check if the packet received from correct FA
		if(Pnum != portno)
			sprintf(status, "%s", "Rejected");
		else
			 sprintf(status, "%s", "Accepted");
		//Print out Time
		printf(ctime(&mytime));
		printf("Received packet, seqno = %d, FA Port# %d %s\n",seqno,Pnum, status); 
	
  } while (seqno !=60);
  close(sock);
}

