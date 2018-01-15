/*Required Headers*/
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>


#define LENGTH 1024
int main(int argc,char *argv[])  //input, ip_address, port 
{
 
    char str[100];
    int listen_fd,  nsockfd;
    char recvbuf[LENGTH];
    char sendbuf[LENGTH];
    int long recv_size[1];
    int long buffer[1];
    int sum_byte=0;
    int size_file;
    char file_name_buffer[100];
    int fr_block_sz;
    int fs_block_sz;
    int sent_byte_size;
//
    struct sockaddr_in servaddr;
 
    int port= atoi(argv[2]);
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
 
    bzero( &servaddr, sizeof(servaddr));
 
    servaddr.sin_family = AF_INET;
    inet_aton(argv[1], &servaddr.sin_addr);
    
    servaddr.sin_port = htons(port); //8080

    bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
 
    listen(listen_fd, 10);
 
    nsockfd = accept(listen_fd, (struct sockaddr*) NULL, NULL);   //client socket, accept connection 
 
    
   /*receive zip file from the client and unzip it */
   //
   //
   //
    recv(nsockfd, file_name_buffer, sizeof(file_name_buffer),0); //receive the file name
    char *file_name= file_name_buffer;
    FILE *fr=fopen(file_name, "wb");
    
    size_file= recv(nsockfd, recv_size, sizeof(recv_size),0);   //receive the size of the file 
   // printf("size of the file %ld \n",recv_size[0]);
     if(recv_size[0]>0){
        while(sum_byte<recv_size[0]){   
        fr_block_sz=recv(nsockfd,recvbuf,sizeof(recvbuf),0);
         //printf("recieved!!!: %d\n",fr_block_sz);

         if(fr_block_sz < 0)
			{
			printf("Error receiving file from client to server.\n");
			}
        sum_byte=sum_byte+fr_block_sz;
        int write_sz=fwrite(recvbuf,1,fr_block_sz,fr);
        //printf("write_sz:%d\n",write_sz);
   
        bzero( recvbuf, sizeof(recvbuf));
        
        //printf("write is ok\n");
         
        if(write_sz < fr_block_sz){
			printf("File write failed on server.\n");
			}else{
                bzero( recvbuf, sizeof(recvbuf));
            }
			 if(fr_block_sz<0){
                printf(" it is a braek point right here\n");
						break;
                    }
    
                }
                printf("Ok, received file from client!\n");
                fclose(fr);
     }else{
         printf("didn't recv the size of the file\n");
     }

char unzip[100];
strcpy(unzip,"unzip ");
strcat(unzip,file_name);
system(unzip);


/* read the txt file and send it to the client */
//
//
//
char *zip;
int index;
zip = strchr(file_name, 'z');   // search for charactor z
index = (int)(zip - file_name); //return the index of z


file_name_buffer[index]='t';        //change zip  to txt
file_name_buffer[index+1]='x';      
file_name_buffer[index+2]='t';
//printf("%s\n",file_name_buffer);

send(nsockfd,file_name_buffer, sizeof(file_name_buffer), 0);    //send file name 


FILE* fs=fopen(file_name_buffer,"rb");
    if(fs==NULL){
       printf("connet open file \n");
       return 0;
    }
   // printf("the file is opend \n");

   
    fseek(fs, 0, SEEK_END);                        //go through the file 
    int long size = ftell(fs);                     //get the size of the file
    rewind(fs);  
    buffer[0]=size;
    send(nsockfd, buffer, sizeof(buffer), 0);       //send size of the file.
    //printf("size of the file %ld \n",buffer[0]);    //wtf??? size is 1021 

    memset(sendbuf, 0,sizeof(sendbuf));
    while(1)
    {   
        fs_block_sz=fread(sendbuf,1,sizeof(sendbuf),fs);
       // printf("read size: %d\n",fs_block_sz);
        if(fs_block_sz >0){
         
        sent_byte_size=send(nsockfd,sendbuf,fs_block_sz ,0);
        bzero(sendbuf,sizeof(sendbuf));
       // printf("sent bytes: %d\n",sent_byte_size);
        memset(sendbuf, 0,sizeof(sendbuf));
        
        if(sent_byte_size<0){
            printf("ERROR: Failed to send file %s.\n", file_name);
	           break;
            }
          // printf("sent line\n");
        }else{
            printf("end\n");
            break;
        }
    }

}