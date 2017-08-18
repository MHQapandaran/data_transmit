/****************** CLIENT CODE ****************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <dirent.h>

int main()
{
    int clientSocket;
    char buffer[1024];
    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    /*---- Create the socket. The three arguments are: ----*/
    /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);

    /*---- Configure settings of the server address struct ----*/
    /* Address family = Internet */
    serverAddr.sin_family = AF_INET;
    /* Set port number, using htons function to use proper byte order */
    serverAddr.sin_port = htons(1500);
    /* Set IP address to localhost */
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    /* Set all bits of the padding field to 0 */
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    /*---- Connect the socket to the server using the address struct ----*/
    addr_size = sizeof serverAddr;
    connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);

    /*---- Read the message from the server into the buffer ----*/

    // recv(clientSocket, buffer, 1024, 0);


    int menu_item;
    do
    {
        DisplayMainMenu();
        menu_item=0;
        printf("\nChoose an option between 1 and 10:");
        scanf("%d", &menu_item);
    }
    while (menu_item<1 || menu_item>8);
    { }/* end of while */
    // send
    char integer[4];                  // buffer
    *((int*)integer) = menu_item;         // 73232 is data which I want to send.
    send( clientSocket, integer, 4, 0 );        // send it
    // send(clientSocket,(char)menu_item,1,0);

    switch (menu_item)
    {
    case 1:
    {
        /*send txt file to server */
        FILE *fpr;
        /*Char array to store strings */
        char strr[1024];
        char line[256];
        /*Opening the file in "r" mode*/
        fpr = fopen("a.txt", "r");

        /*Error handling for file open*/
        if (fpr == NULL)
        {
            puts("Issue in opening the input file");
        }

        /*Loop for reading the file till end*/
            while (fgets(line, sizeof(line), fpr)) {
                send(clientSocket,line,256,0);
                        printf("%s", line); 
        }
        /*Closing the input file after reading*/
        fclose(fpr);
        break;
    }


    case 2:
    {
        send_image(clientSocket);
        // close(clientSocket);
        fflush(stdout);

    }
    break;
    // receive c.txt from server
    case 3:
    {
        char strr[1024];
        
        FILE *fpw;

        // char strr[100];

        /*Opening the file g.TXT in “w” mode for writing*/
        fpw = fopen("c_from_server.txt", "w");

        /*Error handling for output file*/
        if (fpw== NULL)
        {
            puts("Issue in opening the Output file");
        }
        /* Copied the content of str into file –
          * FILEW.TXT using pointer – fpw*/
          while(recv(clientSocket, strr, 1024, 0))
          {
        fputs(strr, fpw);
        }
        /*Closing the Output file after successful writing*/
        fclose(fpw);
        close(clientSocket);
        break;
    }

    case 4:
    {
        /* receive d.jpg from server*/
        receive_image(clientSocket);
        close(clientSocket);
    }
    break;
    case 5:

        break;
    case 6:

        break;
    case 7:
    {
        printf("Hello :),\n You can use this program to send or receive file from server.\nAlso you can use this program to remove files from current directory of server or client.\n so Let's go");
        break;
    }
    case 8:
        break;
    default:
        break;
    }/* end of switch */



    return 0;
}




void DisplayMainMenu()
{
    printf( "_____________________________________________\n\n" );
    printf( "\t\tData Transfer Menu\n\n" );
    printf( "_____________________________________________\n" );
    printf("\nPlease choose one of the following options:\n\n" );
    printf("1. Send a.txt to server\n");
    printf("2. Send b.jpg to server\n");
    printf("3. Receive c.txt from server\n");
    printf("4. Receive d.jpg from server\n");
    printf("5. Remove server Directory files\n");
    printf("6. Remove client Directory files\n");
    printf("7. Help\n");
    printf("8. Exit\n");
    printf( "_____________________________________________\n\n" );
    printf( "\t\tEnter Choice:\n\n" );
    printf( "_____________________________________________\n" );
}





int send_image(int socket)
{

    FILE *picture;
    int size, read_size, stat, packet_index;
    char send_buffer[10240], read_buffer[256];
    packet_index = 1;

    picture = fopen("b.jpg", "r");
    printf("Getting Picture Size\n");

    if(picture == NULL)
    {
        printf("Error Opening Image File");
    }

    fseek(picture, 0, SEEK_END);
    size = ftell(picture);
    fseek(picture, 0, SEEK_SET);
    printf("Total Picture size: %i\n",size);

    //Send Picture Size
    printf("Sending Picture Size\n");
    write(socket, (void *)&size, sizeof(int));

    //Send Picture as Byte Array
    printf("Sending Picture as Byte Array\n");

    do   //Read while we get errors that are due to signals.
    {
        stat=read(socket, &read_buffer , 255);
        printf("Bytes read: %i\n",stat);
    }
    while (stat < 0);

    printf("Received data in socket\n");
    printf("Socket data: %c\n", read_buffer);

    while(!feof(picture))
    {
        //while(packet_index = 1){
        //Read from the file into our send buffer
        read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, picture);

        //Send data through our socket
        do
        {
            stat = write(socket, send_buffer, read_size);
        }
        while (stat < 0);

        printf("Packet Number: %i\n",packet_index);
        printf("Packet Size Sent: %i\n",read_size);
        printf(" \n");
        printf(" \n");


        packet_index++;

        //Zero out our send buffer
        bzero(send_buffer, sizeof(send_buffer));
    }
}





int receive_image(int socket)
{
    // Start function

    int buffersize = 0, recv_size = 0,size = 0, read_size, write_size, packet_index =1,stat;

    char imagearray[10241],verify = '1';
    FILE *image;

//Find the size of the image
    do
    {
        stat = read(socket, &size, sizeof(int));
    }
    while(stat<0);

    printf("Packet received.\n");
    printf("Packet size: %i\n",stat);
    printf("Image size: %i\n",size);
    printf(" \n");

    char buffer[] = "Got it";

//Send our verification signal
    do
    {
        stat = write(socket, &buffer, sizeof(int));
    }
    while(stat<0);

    printf("Reply sent\n");
    printf(" \n");

    image = fopen("d_from_server.jpg", "w");

    if( image == NULL)
    {
        printf("Error has occurred. Image file could not be opened\n");
        return -1;
    }

//Loop while we have not received the entire file yet


    int need_exit = 0;
    struct timeval timeout = {10,0};

    fd_set fds;
    int buffer_fd, buffer_out;

    while(recv_size < size)
    {
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
            do
            {
                read_size = read(socket,imagearray, 10241);
            }
            while(read_size <0);

            printf("Packet number received: %i\n",packet_index);
            printf("Packet size: %i\n",read_size);


            //Write the currently read data into our image file
            write_size = fwrite(imagearray,1,read_size, image);
            printf("Written image size: %i\n",write_size);

            if(read_size !=write_size)
            {
                printf("error in read write\n");
            }


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
