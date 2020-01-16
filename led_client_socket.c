#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <wiringPi.h>

int led = 0;

void *led_onoff(void *arg);
void *send_value(void *args);

int main(int argc, char *argv[]){
    int sock, str_len;
    char num2[] = "2";
    char num3[] = "3";
    char buf[] = "OK";
    char response[20] = "";
    char on[] = ",on\n";
    char off[] = ",off\n";
    struct sockaddr_in serv_adr;
    int s;
    int request = 0;

    pthread_t led_thread;
    pthread_t send_thread, send_thread2;

    sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_adr.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1){
        fputs("Connect error! \n",stderr);
        exit(1);
    }
    else
        puts("Connected......\n");

    wiringPiSetup();
    pinMode(29, OUTPUT);

    pthread_create(&led_thread, NULL, led_onoff, (void*)&s);
    pthread_create(&send_thread, NULL, send_value, (void*)&sock);

    printf("--------------------\n");
    printf("  input 2 : request\n");
    printf("  input 3 : quit\n");
    printf("--------------------\n");

    while(1){
        scanf("%d", &request);

        if(request == 2){
            write(sock, num2, sizeof(num2));
            printf("Send to Server : 2\n");
            if(led == 1){
                write(sock, on, sizeof(on));
                printf("Send to Server : On\n");
            } else if(led == 0){
                write(sock, off, sizeof(off));
                printf("Send to Server : Off\n");
            }
            printf("-------------------\n");
            str_len = read(sock, response, 20);
            response[str_len] = 0;
            printf("Now LED State : %s\n", response);
            request = 0;
        }
        if(request == 3){
            write(sock, num3, sizeof(num3));
            printf("Send to Server : 3\n");
            if(led == 1){
                write(sock, on, sizeof(on));
                printf("Send to Server : On\n");
            } else if(led == 0){
                write(sock, off, sizeof(off));
                printf("Send to Server : Off\n");
            }
            printf("Quit\n");
            break;
        }
    }
    close(sock);
    exit(1);

    pthread_join(led_thread, (void*)&s);
    pthread_join(send_thread, (void*)&sock);


    return 0;
}

void *led_onoff(void *arg){
    printf("led_thread Create\n");

    while(1){
        if(led == 1){
            led = 0;
            digitalWrite(29, LOW);
            delay(rand()%10000);
        }
        else if (led == 0){
            led = 1;
            digitalWrite(29, HIGH);
            delay(rand()%10000);
        }
        printf("Changed Led State Value\n");
    }
}

void *send_value(void *args){
    int thread_sock = *(int*)args;

    char buf[] = "OK";
    char num1[] = "1";
    char on[] = ",on\n";
    char off[] = ",off\n";
    while(1){
        printf("Send to Server : 1\n");
        if(led == 1){
            write(thread_sock, num1, sizeof(num1));
            write(thread_sock, on, sizeof(on));
            printf("Send to Server : On\n");
            sleep(5);
        } else if (led == 0) {
            write(thread_sock, num1, sizeof(num1));
            write(thread_sock, off, sizeof(off));
            printf("Sent to Server : Off\n");
            sleep(5);
        }
    }
}