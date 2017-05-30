#include <stdio.h>   
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <netinet/in.h>  
#include <sys/socket.h> 
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <errno.h>
#include <sys/shm.h>
#include <netdb.h>
#include <time.h>
#include <arpa/inet.h>

#define PORT 4395
#define SIZE 1024
#define SIZE_SHMADD 2048
#define BACKLOG 30
#define NM_LEN 5
#define MD_LEN 16
#define VALUE "**********"

int sockfd;
int i = 0;

int get_sockfd(int port){    

    struct sockaddr_in server_addr; 

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){    
        fprintf(stderr, "Socket error:%s\n\a", strerror(errno));          
        exit(1); 
    }
    else{
        printf("Socket successful!\n"); 
    }     

    bzero(&server_addr, sizeof(struct sockaddr_in)); 
    server_addr.sin_family = AF_INET;                
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    server_addr.sin_port = htons(port);  
    
    if(bind(sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr))==-1){      
        fprintf(stderr, "Bind error:%s\n\a",strerror(errno));       
        exit(1);     
    }
    else{
        printf("Bind successful!\n");
    }  
    
    if(listen(sockfd,BACKLOG) == -1){    
        fprintf(stderr, "Listen error:%s\n\a", strerror(errno)); 
        exit(1);
    }
    else{ 
        printf("Listening.....\n");
    } 

    return sockfd;

}


int shmid_create(){

    int shmid;
     
    if((shmid = shmget(IPC_PRIVATE, SIZE_SHMADD, 0777)) < 0){
        perror("shmid error!");
        exit(1); 
    }
    else  printf("shmid success!\n");

    return shmid;
}

int main(int ac, char *av[]){
    
    char shmadd_buffer[SIZE_SHMADD], buffer[SIZE];      
    struct sockaddr_in client_addr;  
    int sin_size;
       
    pid_t ppid,pid;     
    int new_fd; 
    int shmid;
    char *shmadd;

    char name[NM_LEN];
    
    shmid = shmid_create();
    shmadd = shmat(shmid, 0, 0);
    
    int sockfd = get_sockfd(atoi(av[1]));

    FILE *fp = NULL;
    fp = fopen("name-sock-id.txt", "wt");
    fclose(fp);

    while(1){
             
        sin_size = sizeof(struct sockaddr_in);
        
        if((new_fd = accept(sockfd, (struct sockaddr*)(&client_addr), &sin_size)) == -1){ 
            fprintf(stderr, "Accept error:%s\n\a",strerror(errno));  
            exit(1);   
        }
        else{
            printf("Accept successful!\n"); 
        }

        printf("\nconnected%d : %s:%d \n", i, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        
        memset(buffer, 0, SIZE);
        recv(new_fd, name, NM_LEN, 0);

        fp = fopen("name-sock-id.txt", "at");
        fprintf(fp, "%d\n", new_fd);
        fclose(fp);

        ppid = fork();

        if(ppid == 0){
        
            pid = fork(); 
            int flag = 0;
            char back[128];
            char sv_msd[128];

            while(1){

                if(pid > 0){
           
                    memset(buffer, 0, SIZE);

                    if((recv(new_fd, buffer, SIZE, 0)) <= 0){
                        close(new_fd);
                        exit(1); 
                    }

                    memset(shmadd, 0, SIZE_SHMADD);
                    strncpy(shmadd, buffer, SIZE_SHMADD);
                    printf("%s\n", buffer);
                    
                }   
         
                if(pid == 0){

                    if(strcmp(shmadd_buffer, shmadd)){

                        

                        char from[NM_LEN];
                        char to[NM_LEN];
                        char mood[MD_LEN];
                        char mood_opst[MD_LEN];
                        char hurt[5];
                        char date[32];

                        char mood_rest_f[16];
                        char mood_rest_t[16];

                        char msd_f[5];
                        char msd_t[5];

                        char *s = NULL;
                        char pool[128];

                        int fd[BACKLOG];
                        int num = 0;
                        int me;
                        int he;

                        char rd_name[NM_LEN];
                        int rd_sock_id;
                        
                        /**
                         * methods from both sides
                         */
                        int f_fwd;
                        int t_fwd;

                        FILE *fp = NULL;

                        strcpy(shmadd_buffer, shmadd);
                        strcpy(pool, shmadd_buffer);

                        strcat(sv_msd, pool);

                        printf("sv_msd: %s\n", sv_msd);
                        memset(pool, 0, strlen(pool));

                        

                        flag++;

                        if(flag == 2){

                            s = strtok(sv_msd, "|");
                            strcpy(from, s);

                            s = strtok(NULL, "|");
                            strcpy(mood, s);

                            s = strtok(NULL, "|");
                            strcpy(msd_f, s);

                            s = strtok(NULL, "|");
                            strcpy(to, s);

                            s = strtok(NULL, "|");
                            strcpy(mood_opst, s);

                            s = strtok(NULL, "|");
                            strcpy(msd_t, s);

                            f_fwd = atoi(msd_f);
                            t_fwd = atoi(msd_t);

                            if(f_fwd && t_fwd){
                            
                                if(f_fwd < t_fwd){
                                    mood_rest_f[0] = '*';
                                    mood_rest_f[1] = '\0';
                                    strcpy(mood_rest_t, VALUE);
                                }

                                else if(f_fwd > t_fwd){
                                    mood_rest_t[0] = '*';
                                    mood_rest_t[1] = '\0';
                                    strcpy(mood_rest_f, VALUE);
                                }

                                else{
                                    strcpy(mood_rest_t, VALUE);
                                    strcpy(mood_rest_f, VALUE);
                                }

                            }

                            else if(!strncmp(msd_f, "g", 1) && t_fwd){
                                if(t_fwd < 4){
                                    strcpy(mood_rest_t, VALUE);
                                    strcpy(mood_rest_f, VALUE);
                                }
                                else{
                                    mood_rest_f[0] = '*';
                                    mood_rest_f[1] = '\0';
                                    strcpy(mood_rest_t, VALUE);
                                }
                            }

                            else if(!strncmp(msd_t, "g", 1) && f_fwd){
                                if(f_fwd < 4){
                                    strcpy(mood_rest_t, VALUE);
                                    strcpy(mood_rest_f, VALUE);
                                }
                                else{
                                    mood_rest_t[0] = '*';
                                    mood_rest_t[1] = '\0';
                                    strcpy(mood_rest_f, VALUE);
                                }
                            }

                            else if(!strncmp(msd_f, "s", 1) && t_fwd){
                                mood_rest_f[0] = '*';
                                mood_rest_f[1] = '\0';
                                strcpy(mood_rest_t, VALUE);
                            }

                            else if(!strncmp(msd_t, "s", 1) && f_fwd){
                                mood_rest_t[0] = '*';
                                mood_rest_t[1] = '\0';
                                strcpy(mood_rest_f, VALUE);
                            }

                            else{
                                strcpy(mood_rest_f, VALUE);
                                strcpy(mood_rest_t, VALUE);
                            }

                            /*
                             * 1000|*******|1001|*****
                             */

                            strcat(back, from);
                            strcat(back, "|");
                            strcat(back, mood_rest_f);
                            strcat(back, "|");
                            strcat(back, msd_t);
                            strcat(back, "|");

                            if(!strncmp(msd_f, "s", 1))
                                strcat(back, "1");
                            else strcat(back, "0");
                            strcat(back, "|");

                            strcat(back, to);
                            strcat(back, "|");
                            strcat(back, mood_rest_t);
                            strcat(back, "|");
                            strcat(back, msd_f);
                            strcat(back, "|");

                            if(!strncmp(msd_t, "s", 1))
                                strcat(back, "1");
                            else strcat(back, "0");
                            strcat(back, "|");

                            printf("back: %s\n", back);

                        }

                        /*fp = fopen("name-sock-id.txt", "rt");
                        while(fscanf(fp, "%d", &fd[num++]));

                        for(int i = 0; i <= num; i++){

                            if(new_fd == fd[i]){
                                
                                if(i % 2){
                                    me = fd[i];
                                    he = fd[i - 1];
                                }
                                else{
                                    me = fd[i];
                                    he = fd[i + 1];
                                }

                            }

                        }*/
                        
                        if(new_fd > 0 && flag == 2 /*&& new_fd == me || new_fd == he*/){

                            if(send(new_fd, back, strlen(back), 0) == -1){

                                perror("send");

                            }
               
                            memset(shmadd, 0, SIZE_SHMADD);
                            strcpy(shmadd, shmadd_buffer); 

                            
                            flag = 0;

                            memset(back, 0, 128);
                            memset(sv_msd, 0, strlen(sv_msd));
                            memset(from, 0, strlen(from));
                            memset(to, 0, strlen(to));
                            memset(mood, 0, strlen(mood));
                            memset(mood_opst, 0, strlen(mood_opst));
                            memset(msd_f, 0, strlen(msd_f));
                            memset(msd_t, 0, strlen(msd_t));
                            memset(mood_rest_f, 0, strlen(mood_rest_f));
                            memset(mood_rest_t, 0, strlen(mood_rest_t));
              
                        }

                    }

                }

            }

        }

    }    

    free(buffer);
    close(new_fd);
    close(sockfd);

    return 0;

}
