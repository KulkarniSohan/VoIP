/*
This example reads from the default PCM device
and writes to standard output for 5 seconds of data.
*/

/* Use the newer ALSA API */
#define PCM_DEVICE "default"
#define ALSA_PCM_NEW_HW_PARAMS_API

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>

#include <alsa/asoundlib.h>
//#include "client1_record.h"

int sendFileData(int sockfd,char *buf,int size)
{
	int n;
	again : if((n = write(sockfd,buf,size)) < 0)
	{
		if((n < 0) && (errno == EINTR))			
		{
			goto again;
		}
		if(n < 0)
		{
			printf("\n\tWrite Error\n");
			exit(1); /* return*/
		}
	}
}

int sendFileName(int sockfd,char *filename)
{
	int n;
	agian1 : if((n = write(sockfd,filename,strlen(filename))) < 0)
	{
		/*if((n < 0) && (errno == EINTR))//interrupt
		{
			goto agian1;
		}
		*/
		if(n < 0)
		{
			printf("\n\tWrite error\n");
			exit(1); 
		}
	}
	
	return 0;
}

int fun(char *buffer,int size,char *filename) 
{
	struct sockaddr_in servaddr;
	
	bzero(&servaddr,sizeof(servaddr));
	int sockfd,connfd;
	
	sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);// socket fd
	if(sockfd < 0)
	{
		perror("\n\tSocket");
		printf("\n\tSocket Error\n");
		exit(1);
	}
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5579);
	
	if((inet_pton(sockfd,"127.0.0.1",&servaddr.sin_addr)) <= 0)// convert(ip addrss) string to int
	{
		printf("\n\tIp Address Error\n");
		perror("\n\tip");
		//exit(1);
	}
	
	/* servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); */
	
	connfd = connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));//connect

	if(connfd < 0)
	{
		printf("\n\tConnect Error\n");
		perror("\n\tConnect\n");
		exit(1);
	}
	
	//sendFileName(sockfd,filename);
	sendFileData(sockfd,buffer,size);
	//printf("\n\tExiting\n");
}	

//-------------------------------------------------------------------------------------------------------------

int main()
{
	int fp,ff;
  unsigned int pcm, tmp, dir;
  int buff_size;
	char filename[20] = "123.wav";
  long loops;
  int rc;
  int size;
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  unsigned int val;
  
  snd_pcm_uframes_t frames;
  char *buff,*ipaddress;
  int rate, channels, seconds;
//printf("aaa\n");
  /* Open PCM device for recording (capture). */
  rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_CAPTURE, 0);

  if (rc < 0) 
  {
    fprintf(stderr,"unable to open pcm device: %s\n",   snd_strerror(rc));
    exit(1);
  }
  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(handle, params);

  /* Set the desired hardware parameters. */

  /* Interleaved mode */
  snd_pcm_hw_params_set_access(handle, params,  SND_PCM_ACCESS_RW_INTERLEAVED);

  /* Signed 16-bit little-endian format */
  snd_pcm_hw_params_set_format(handle, params,SND_PCM_FORMAT_U8);

  /* Two channels (stereo) */
  snd_pcm_hw_params_set_channels(handle, params, 1);

  /* 44100 bits/second sampling rate (CD quality) */
  val = 8000;
  
  snd_pcm_hw_params_set_rate_near(handle, params,  &val, &dir);

  /* Set period size to 32 frames. */
  frames = 32;
  snd_pcm_hw_params_set_period_size_near(handle,params, &frames, &dir);

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);
  if (rc < 0) 
  {
    fprintf(stderr,"unable to set hw parameters: %s\n", snd_strerror(rc));
    exit(1);
  }

  /* Use a buff large enough to hold one period */

  snd_pcm_hw_params_get_period_size(params,  &frames, &dir);
  size = frames * 4; /* 2 bytes/sample, 2 channels */
  buff = (char *) malloc(size);

  /* We want to loop for 5 seconds */

  snd_pcm_hw_params_get_period_time(params, &val, &dir);
  loops = 50000000 / val;

  fp=open("123.wav",O_WRONLY);
  ipaddress = malloc(sizeof(char) * 4);

  /*printf("\n\tEnter ip : ");
  scanf("%s",ipaddress);*/

//  printf("sending data\n");
  
  for (loops=500000 / val;loops > 0;loops--) 
  {
		bzero(buff,sizeof(buff));
		rc = snd_pcm_readi(handle, buff, frames);
		write(fp,buff,128);
		fun(buff,128,filename);
	
	//	fucntion1(ipaddress,buff,size);
		
		if (rc == -EPIPE) 
		{
		  /* EPIPE means overrun */
		  fprintf(stderr, "overrun occurred\n");
		  snd_pcm_prepare(handle);
		} 
		else if (rc < 0) 
		{
		  fprintf(stderr,"error from read: %s\n",  snd_strerror(rc));
		} 
		else if (rc != (int)frames) 
		{
		  fprintf(stderr, "short read, read %d frames\n", rc);
		}
		
	/*	rc = write(1, buff, size);
		if (rc != size)
		  fprintf(stderr,"short write: wrote %d bytes\n", rc);
	
  */
 	}
  close(fp);
  //printf("sending data\n");
 
 /* client1 call(222.wav)*/
  snd_pcm_drain(handle);
  snd_pcm_close(handle);
  
	free(buff);

	return 0;
}

