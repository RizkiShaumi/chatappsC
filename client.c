#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <pthread.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080

SOCKET sock;
int exit_flag = 0;

void* receive_handler(void* arg){
    char buffer[1024];
    while(!exit_flag){
        memset(buffer, 0, sizeof(buffer));
        int byteReceived = recv(sock, buffer, 1024, 0);
        if (byteReceived > 0)
        {
            printf("\nserver: %s\n", buffer);

            if (strcmp(buffer, "exit")==0)
            {
                printf("Disconnecting\n");
                exit_flag = 1;
                break;
            }
        }else if(byteReceived == 0){
            printf("Connection Closed\n");
            exit_flag =1;
            break;
        }else{
            printf("error code: %d\n", WSAGetLastError());
            exit_flag = 1;
            break;
            }  
        
    }
    return NULL;
}

void* send_handler(void *arg){
    char pesan[100];
    while (!exit_flag){
        printf("Masukkan Pesan : ");
        scanf("%s", &pesan);

        send(sock, pesan, strlen(pesan), 0);

        if (strcmp(pesan, "exit")==0)
        {
                closesocket(sock);
                WSACleanup();
                exit_flag = 1;
                break;            
        }
        
    }
    return NULL;
}

int main(){
    WSADATA wsa;

    char pesan[100];
    struct sockaddr_in serv_addr;

    char buffer[1024] = {0};
    pthread_t receive_trhead, send_trhead;

    if(WSAStartup(MAKEWORD(2,2), &wsa) != 0){
        printf("Gagal Inisasi Winsock : %d ", WSAGetLastError());
        return 1;
    }
    if ((sock = socket(AF_INET, SOCK_STREAM, 0))== INVALID_SOCKET)
    {
        printf("Tidak bisa membuat socket : %d", WSAGetLastError());
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr= inet_addr("127.0.0.1");

    if (connect(sock, (struct  sockaddr *)&serv_addr, sizeof(serv_addr)) <0 )
    {
        printf("\nKoneksi Gagal\n"); 
        return -1;    
    };

    printf("Connection Success\n");

    pthread_create(&receive_trhead, NULL, receive_handler, NULL);

    pthread_create(&send_trhead, NULL, send_handler, NULL);

    pthread_join(receive_trhead, NULL);

    pthread_join(send_trhead, NULL);

}