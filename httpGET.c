#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>      // define structures like hostent
#include<stdlib.h>
#include<strings.h>
#include<unistd.h>
#include<errno.h>
#define buffer_size 10240
int main()
{
	int sockfd;
	int portno, n, len;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	
	char title[] = "Yesterday";
	char artist[] = "The beatles";
	char buffer[buffer_size];
	/*
	if(argc < 3){
		fprintf(stderr,"usage %s hostname port\n",argv[0]);
		exit(0);
	}
	*/
	portno = 80;
    sockfd = socket(PF_INET, SOCK_STREAM, 0); //create a new socket
    if (sockfd < 0) 
        perror("ERROR opening socket");
    
    server = gethostbyname("lyrics.alsong.co.kr"); //takes a string like "www.yahoo.com", and returns a struct hostent which contains information, as IP address, address type, the length of the addresses...
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; //initialize server's address
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) //establish a connection to the server
        perror("ERROR connecting");
    /*
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    */

	char t1[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://www.w3.org/2003/05/soap-envelope\" xmlns:SOAP-ENC=\"http://www.w3.org/2003/05/soap-encoding\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:ns2=\"ALSongWebServer/Service1Soap\" xmlns:ns1=\"ALSongWebServer\" xmlns:ns3=\"ALSongWebServer/Service1Soap12\"><SOAP-ENV:Body><ns1:GetResembleLyric2><ns1:stQuery><ns1:strTitle>";
	//title
	char t2[] = "</ns1:strTitle><ns1:strArtistName>";
	//artist
	char t3[] = "</ns1:strArtistName><ns1:nCurPage>0</ns1:nCurPage></ns1:stQuery></ns1:GetResembleLyric2></SOAP-ENV:Body></SOAP-ENV:Envelope>";
	len = strlen(t1) + strlen(title) + strlen(t2) + strlen(artist) + strlen(t3);
	bzero(buffer,buffer_size);
	sprintf(buffer,"POST /alsongwebservice/service1.asmx HTTP/1.1\r\nHost: lyrics.alsong.co.kr\r\nContent-Type:text/xml;charset=utf-8\r\nContent-Length: %d\r\n\r\n",len);
	strcat(buffer, t1);
	strcat(buffer,title);
	strcat(buffer, t2);
	strcat(buffer,artist);
	strcat(buffer, t3);
	n = send(sockfd,buffer,strlen(buffer),0); //write to the socket
    if (n < 0) 
         perror("ERROR writing to socket");
    
    bzero(buffer,buffer_size);
	int buffer_p = 0;
	char end_string[] = "</soap:Envelope>";
	int end_int = 0;
	while(recv(sockfd,buffer+buffer_p,1,0) == 1)
    {
		if(buffer[buffer_p] == end_string[end_int])
			end_int++;
		if(end_int == strlen(end_string))
			break;
		buffer_p++;
    }
	printf("%s\n",buffer);
    if (n < 0) 
         perror("ERROR reading from socket");
    
    close(sockfd); //close socket
    printf("strlen(buffer) = %lu\n",strlen(buffer));
	
	printf("\n\n");
	
	char buffer2[buffer_size];
	char end_string2[] = "<strLyric>";
	char end_string3[] = "</strLyric>";
	int buffer2_start = 0;
	int j = 0;
	end_int = 0;
	for(int i = 0; i < strlen(buffer); i++)
	{
		if(end_int < strlen(end_string2) && buffer[i] != end_string2[end_int])
			end_int = 0;
		if(buffer[i] == end_string2[end_int])
			end_int++;
		if(end_int == strlen(end_string2))
		{
			buffer2_start = 1;
			end_int = 0;
		}
		if(buffer2_start == 1 && buffer[buffer_p] == end_string3[end_int])
			end_int++;
		if(buffer2_start == 1 && end_int == strlen(end_string3))
			break;
		if(buffer2_start == 1)
		{
			if(buffer[i] == '>')
				continue;
			
			if(buffer[i+1] == '/' && buffer[i+2] == 's' && buffer[i+3] == 't')
				break;
			
			if(buffer[i] == '&' && buffer[i+1] == 'l' && buffer[i+2] == 't' && buffer[i+3] == ';' && buffer[i+4] == 'b' && buffer[i+5] == 'r')
			{
				i = i+10;
				buffer2[j++] = '\n';
			}
			if(buffer[i] == '[')
			{
				i = i+10;
			}
			buffer2[j++] = buffer[i];
		}
		
		
	}
	printf("%s\n",buffer2);
    return 0;
}