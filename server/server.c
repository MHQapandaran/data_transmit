/****************** SERVER CODE ****************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

int main(){
  int welcomeSocket, newSocket, mySocket;
  char buffer[1024];
  char menu_number;
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
  
  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_port = htons(1500);
  /* Set IP address to localhost */
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*---- Bind the address struct to the socket ----*/
  bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  /*---- Listen on the socket, with 5 max connection requests queued ----*/
  if(listen(welcomeSocket,5)==0)
{    printf("Listening\n");

}
  else
    printf("Error\n");


  /*---- Accept call creates a new socket for the incoming connection ----*/
  addr_size = sizeof serverStorage;
  newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);
  char integer[4];                  // buffer
  recv(newSocket, integer, 4, 0 );         // receive it
  int menu_item=*integer;
  // printf("%d",*integer);
  printf("%d",menu_item);
  
  
  switch (menu_item)
        {
          /* receive a.txt from client*/
        case 1:
        {
        
        FILE *fpw;

        // char strr[100];

        /*Opening the file g.TXT in “w” mode for writing*/
        fpw = fopen("a_from_client.txt", "w");

        /*Error handling for output file*/
        if (fpw== NULL)
        {
              puts("Issue in opening the Output file");
        }
        /* Copied the content of str into file – 
          * FILEW.TXT using pointer – fpw*/
          while(recv(newSocket, buffer, 256, 0))
          {
        fputs(buffer, fpw);
        }
        /*Closing the Output file after successful writing*/
        fclose(fpw);
        break;
        }
              
				case 2:
        {
          /* receive c.jpg from client*/
            receive_image(newSocket);
            close(newSocket);
        }
					break;
				case 3:
        {
            /*send txt file to server */
            FILE *fpr;
            /*Char array to store strings */
            char strr[1024];
            /*Opening the file in "r" mode*/
            fpr = fopen("c.txt", "r");

            /*Error handling for file open*/
            if (fpr == NULL)
            {
              puts("Issue in opening the input file");
            }

            /*Loop for reading the file till end*/
            while(1)
            {
              if(fgets(strr,1024, fpr) ==NULL)
              break;
                    
              
              else
              {
              send(newSocket,strr,1024,0);
              }
                    
            }
            /*Closing the input file after reading*/
            fclose(fpr);
            close(newSocket);
            
        }
					break;
				case 4:
        {
          send_image(newSocket);
          close(newSocket);
          fflush(stdout);      
        }
					break;
				case 5:
        system("exec rm -rf /home/mhq/Desktop/test/server/servertemp/*"); 
					break;
				case 6:
          system("exec rm -rf /home/mhq/Desktop/test/client/clienttemp/*"); 
					break;
				default:
					break;
			}/* end of switch */


  return 0;
}






int receive_image(int socket)
{
   // Start function 

int buffersize = 0, recv_size = 0,size = 0, read_size, write_size, packet_index =1,stat;

char imagearray[10241],verify = '1';
FILE *image;

//Find the size of the image
do{
stat = read(socket, &size, sizeof(int));
}while(stat<0);

printf("Packet received.\n");
printf("Packet size: %i\n",stat);
printf("Image size: %i\n",size);
printf(" \n");

char buffer[] = "Got it";

//Send our verification signal
do{
stat = write(socket, &buffer, sizeof(int));
}while(stat<0);

printf("Reply sent\n");
printf(" \n");

image = fopen("b_from_client.jpg", "w");

if( image == NULL) {
printf("Error has occurred. Image file could not be opened\n");
return -1; }

//Loop while we have not received the entire file yet


int need_exit = 0;
struct timeval timeout = {10,0};

fd_set fds;
int buffer_fd, buffer_out;

while(recv_size < size) {
//while(packet_index < 2){

    FD_ZERO(&fds);
    FD_SET(socket,&fds);

    buffer_fd = select(FD_SETSIZE,&fds,NULL,NULL,&timeout);

    if (buffer_fd < 0)
       printf("error: bad file descriptor set.\n");

    if (buffer_fd == 0)
       printf("error: buffer read timeout expired.\n");

    if (buffer_fd > 0)
    {
        do{
               read_size = read(socket,imagearray, 10241);
            }while(read_size <0);

            printf("Packet number received: %i\n",packet_index);
        printf("Packet size: %i\n",read_size);


        //Write the currently read data into our image file
         write_size = fwrite(imagearray,1,read_size, image);
         printf("Written image size: %i\n",write_size); 

             if(read_size !=write_size) {
                 printf("error in read write\n");    }


             //Increment the total number of bytes read
             recv_size += read_size;
             packet_index++;
             printf("Total received image size: %i\n",recv_size);
             printf(" \n");
             printf(" \n");
    }

}


  fclose(image);
  printf("Image successfully Received!\n");
  return 1;
  }






int send_image(int socket){

   FILE *picture;
   int size, read_size, stat, packet_index;
   char send_buffer[10240], read_buffer[256];
   packet_index = 1;

   picture = fopen("d.jpg", "r");
   printf("Getting Picture Size\n");   

   if(picture == NULL) {
        printf("Error Opening Image File"); } 

   fseek(picture, 0, SEEK_END);
   size = ftell(picture);
   fseek(picture, 0, SEEK_SET);
   printf("Total Picture size: %i\n",size);

   //Send Picture Size
   printf("Sending Picture Size\n");
   write(socket, (void *)&size, sizeof(int));

   //Send Picture as Byte Array
   printf("Sending Picture as Byte Array\n");

   do { //Read while we get errors that are due to signals.
      stat=read(socket, &read_buffer , 255);
      printf("Bytes read: %i\n",stat);
   } while (stat < 0);

   printf("Received data in socket\n");
   printf("Socket data: %c\n", read_buffer);

   while(!feof(picture)) {
   //while(packet_index = 1){
      //Read from the file into our send buffer
      read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, picture);

      //Send data through our socket 
      do{
        stat = write(socket, send_buffer, read_size);  
      }while (stat < 0);

      printf("Packet Number: %i\n",packet_index);
      printf("Packet Size Sent: %i\n",read_size);     
      printf(" \n");
      printf(" \n");


      packet_index++;  

      //Zero out our send buffer
      bzero(send_buffer, sizeof(send_buffer));
     }
    }