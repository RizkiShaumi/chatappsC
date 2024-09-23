#include <stdio.h>
#include <winsock.h>

#pragma comment(lib, "Ws2_32.lib")

int main(int argc , char *argv[]){
    WSADATA wsa;
    SOCKET s;
    printf ("\nInisialisasi Winsock. . . ");
    if(WSAStartup(MAKEWORD(2,2),&wsa)!=  0){
        printf("Gagal. Kode Error : %d ",WSAGetLastError());
        return 1;
    }

    printf("\n Terinisialisasi");

    if ((s = socket(AF_INET, SOCK_STREAM , 0 )) == INVALID_SOCKET)
    {
        printf("Tidak Bisa Membuat Socket : %d ", WSAGetLastError());
    }
    printf("Socket Terbuat : %d", s);

    return 0;
    
}