/*	DataSource.c
	Enrique Hernandez
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <sys/time.h>		/* for gettimeofday() */
#include <stdlib.h>
#include <strings.h>
#include <time.h>

#define INET_ADDRSTRLEN 16
/* This program acts as a data source to the system it simply
	creates a packet and sends it to the Home Agent every
	second.
 */
main(int argc,char *argv[])
{
  int sock;
  struct sockaddr_in source, extrasa, dest;
  struct hostent *hp, *gethostbyname();
  char buf[1024];
  int seqno, rval;
  /* Check arguments */
  if (argc != 3) {
    printf("Usage: usend4 HA_hostname MN_portnumber\n");
    exit(0);
  }
/* create socket */
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(sock < 0) {
    perror("Opening stream socket");
    exit(1);
  }
  /* now bind source address. Port is wildcard, doesn't matter. */
  source.sin_family = AF_INET;
  source.sin_addr.s_addr = INADDR_ANY;
  source.sin_port = 0;
  if (bind(sock, (struct sockaddr *)&source, sizeof(source)) < 0) {
    perror("binding socket name");
    exit(1);
  }
  dest.sin_family = AF_INET;
  hp = gethostbyname(argv[1]);
  if(hp == 0) {
    printf("%s: unknown host\n",argv[1]);
    exit(2);
  }
  bcopy(hp->h_addr, &dest.sin_addr, hp->h_length);
  dest.sin_port = htons(atoi(argv[2]));
  /* create a packet */
  int i = 1;
  //Get the Ip Address in dotted decimal
  char str[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &dest.sin_addr, str, INET_ADDRSTRLEN);
  //To get the time of day
  time_t mytime;
  mytime = time(NULL);
  while (i < 61){
	  seqno = i;
	  bzero(buf,sizeof(buf));
	  sprintf(buf, "%d\0", seqno);

	  if ((rval = sendto(sock, buf, sizeof(buf), 0, (struct sockaddr *)&dest, sizeof(dest))) < 0) {
		  perror("writing on datagram socket");
	  }
	  printf(ctime(&mytime));
	  printf("Seqno = %d ", seqno);
	  printf(" Dest = %s/%d\n",str,atoi(argv[2]));
	  //To wait 1 sec before sending next pkt
	  sleep(1);

	  i++;
  }
  close(sock);
}
