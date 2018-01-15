#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
 
#define LENGTH 1024
int main(int argc,char *argv[])  //input ip_address, port, zip file
{
    struct timeval  start;
    struct timeval  end;
    int sockfd,n;
    char recvbuf[LENGTH];
    char sendbuf[LENGTH];
    int port= atoi(argv[2]);
    struct sockaddr_in servaddr;
    char file_name[100];
    char file_name_buff[100];
    int long buffer[1];
    int long recv_size[1];
    int fs_block_sz;
    int fr_block_sz;
    int sent_byte_size;   
    int size_file;
    int sum_byte=0;
  
    gettimeofday(&start, NULL);                                 //start  counting time
    double time_in_mill_start = (start.tv_sec) * 1000 + (start.tv_usec) / 1000 ;

    sockfd=socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof servaddr);
 
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(port);
 
    inet_pton(AF_INET,argv[1],&(servaddr.sin_addr));             //"10.10.1.100"
 
    connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
        
  /*send the zip file name and file size and the content to the server */
  //
  //
  //
    strcpy(file_name,"./");
    strcat(file_name, argv[3]);
    send(sockfd,file_name, sizeof(file_name), 0);    //send file name 

    
    FILE* fs=fopen(file_name,"rb");
    if(fs==NULL){
       printf("connet open file \n");
       return 0;
    }
    //printf("the file is opend \n");

   
    fseek(fs, 0, SEEK_END);                        //go through the file 
    int long size = ftell(fs);                     //get the size of the file
    rewind(fs);  
    buffer[0]=size;
    send(sockfd, buffer, sizeof(buffer), 0);       //send size of the file.
    // printf("size of the file %ld \n",buffer[0]);

    memset(sendbuf, 0,sizeof(sendbuf));
    while(1)
    {   
        fs_block_sz=fread(sendbuf,1,sizeof(sendbuf),fs);
       // printf("read size: %d\n",fs_block_sz);
        if(fs_block_sz >0){
         
        sent_byte_size=send(sockfd,sendbuf,fs_block_sz ,0);
        bzero(sendbuf,sizeof(sendbuf));
       // printf("sent bytes: %d\n",sent_byte_size);
        memset(sendbuf, 0,sizeof(sendbuf));
        
        if(sent_byte_size<0){
            printf("ERROR: Failed to send file %s.\n", file_name);
	           break;
            }
       //    printf("sent line\n");
        }else{
            printf("end.\n");
            break;
        }
    }
      

/* receive the txt file from the server*/
//
//
//
recv(sockfd, file_name_buff, sizeof(file_name_buff),0); //receive the file name
//printf("%s\n",file_name_buff);
char *txt_file= file_name_buff;
FILE *fr=fopen(txt_file, "wb");

 size_file= recv(sockfd, recv_size, sizeof(recv_size),0);   //receive the size of the file 
 //   printf("size of the file %ld \n",recv_size[0]);
    if(recv_size[0]>0){
        while(sum_byte<recv_size[0]){   
        fr_block_sz=recv(sockfd,recvbuf,sizeof(recvbuf),0);
       //  printf("recieved!!!: %d\n",fr_block_sz);

         if(fr_block_sz < 0)
			{
			printf("Error receiving file from client to server.\n");
			}
        sum_byte=sum_byte+fr_block_sz;
        int write_sz=fwrite(recvbuf,1,fr_block_sz,fr);
      //  printf("write_sz:%d\n",write_sz);
   
        bzero( recvbuf, sizeof(recvbuf));
        
        //memset(recvbuf, 0,sizeof(recvbuf));
      //  printf("write is ok\n");
         
        if(write_sz < fr_block_sz){
			printf("File write failed on server.\n");
			}else{
                bzero( recvbuf, sizeof(recvbuf));
            }
			 if(fr_block_sz<0){
             //   printf(" it is braek point right here");
						break;
                    }
    
                }
                printf("Ok received txt file from server!\n");
                fclose(fr);
     }else{
         printf("didn't recv the size of the file.\n");
     }


/*display the total time*/
close(sockfd);

gettimeofday(&end, NULL);                                                //end time counting 
double time_in_mill_end = (end.tv_sec) * 1000 + (end.tv_usec) / 1000 ;

double time_in_mill=time_in_mill_end -time_in_mill_start ;
printf("it took:%f ms.\n",time_in_mill);
}