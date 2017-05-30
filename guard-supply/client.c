#include <stdio.h>
#include <netinet/in.h>  
#include <sys/socket.h> 
#include <sys/types.h> 
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <curses.h>

#define SIZE 1024
#define SIZE_SHMADD 2048
#define NM_LEN 5
#define MD_LEN 16
#define MSG_LEN 2
#define MSGKEY 75

#define ME_X 7
#define ME_Y 3
#define V_X 3
#define V_E 2

#define HE_X 7
#define HE_Y COLS-10

#define MESSAGE "=======>>>"
#define MSGR "<<<======="
#define ATTK "<<======>>"
#define BLANK "          "
#define BLANK_4 "    "
#define ENER_LEN 5

#define ATTK_T "====>>>"
#define ATTK_S "=====>>>"
#define ATTK_M "======>>>"
#define ATTK_L "=======>>>"
#define GUARD "[[[[[[[[[["
#define SUPPLY "OOOO"

#define ANTI_T "<<<===="
#define ANTI_S "<<<====="
#define ANTI_M "<<<======"
#define ANTI_L "<<<======="
#define OBSV_G "]]]]]]]]]]"

#define VALUE "**********"
#define ENERGY "OOOO"

int row;
int col;
int dir;
int opst;

char mood_cut[MD_LEN];
char mood_cut_opst[MD_LEN];

char energy[ENER_LEN];

int msgqid;

void draw(int row, int col){

    move(row++, col);
    addstr("   /\\\n");
    
    move(row++, col);
    addstr("  /  \\\n");

    move(row++, col);
    addstr(" /    \\\n");

    move(row++, col);
    addstr("/      \\\n");

    move(row++, col);
    addstr("--------\n");

    move(row++, col);
    addstr("|      |\n");

    move(row++, col);
    addstr("|      |\n");

    move(row++, col);
    addstr("|      |\n");

    move(row++, col);
    addstr("|      |\n");

    move(row++, col);
    addstr("|      |\n");

    move(row++, col);
    addstr("|      |\n");

    move(row++, col);
    addstr("|      |\n");

    move(row++, col);
    addstr("|      |\n");

    move(row++, col);
    addstr("--------\n");

    move(row = V_X, col = ME_Y);
    addstr(VALUE);

    move(row = V_X, col = HE_Y - V_E);
    addstr(VALUE);

    move(row = V_X + 12, col = ME_Y + 2);
    addstr(ENERGY);

}

int set_stop(){

    struct itimerval timeset;

    timeset.it_value.tv_sec = 0;
    timeset.it_value.tv_usec = 0;
    timeset.it_interval = timeset.it_value;

    return setitimer(ITIMER_REAL, &timeset, NULL);

}

int set_ticker(int n_msecs){

    struct itimerval timeset;

    long n_sec;
    long n_usecs;

    n_sec = n_msecs / 1000;
    n_usecs = (n_msecs % 1000) * 1000L;

    timeset.it_interval.tv_sec = n_sec;
    timeset.it_interval.tv_usec = n_usecs;
    timeset.it_value.tv_sec = n_sec;
    timeset.it_value.tv_usec = n_usecs;

    return setitimer(ITIMER_REAL, &timeset, NULL);

}

void updt_Me(char *msd, char *spl){

    /**
     * swipe mood in the past and update the aspect of me
     */

    /**
     * 1st for me
     */
    move(row = V_X, col = ME_Y);
    addstr(BLANK);
    move(row = V_X, col = ME_Y);
    addstr(mood_cut);

    /**
     * 2nd for him
     */
    move(row = V_X, col = HE_Y - V_E);
    addstr(BLANK);
    move(row = V_X, col = HE_Y - V_E);
    addstr(mood_cut_opst);

    move(row = ME_X + 5, col = HE_Y - 25);
    addstr(BLANK);

    if(!strncmp(msd, "1", 1))
        addstr(ANTI_T);
    else if(!strncmp(msd, "2", 1))
        addstr(ANTI_S);
    else if(!strncmp(msd, "3", 1))
        addstr(ANTI_M);
    else if(!strncmp(msd, "4", 1))
        addstr(ANTI_L);
    else if(!strncmp(msd, "g", 1))
        addstr(OBSV_G);
    else if(!strncmp(msd, "s", 1))
        addstr(ENERGY);
    
    if(!strncmp(spl, "1", 1)){

        if(strlen(energy) < 4){
            move(row = V_X + 12, col = ME_Y + 2);
            addstr(BLANK_4);
            energy[strlen(energy) + 1] = '\0';
            energy[strlen(energy)] = 'O';
            move(row = V_X + 12, col = ME_Y + 2);
            addstr(energy);
        }
    }

    move(row = ME_X + 5, col = ME_Y + 8);
    refresh();

}

void updt_He(char *msd, char *spl){

    move(row = V_X, col = HE_Y - V_E);
    addstr(BLANK);
    move(row = V_X, col = HE_Y - V_E);
    addstr(mood_cut);
 
    move(row = V_X, col = ME_Y);
    addstr(BLANK);
    move(row = V_X, col = ME_Y);
    addstr(mood_cut_opst);

    move(row = ME_X + 5, col = HE_Y - 25);
    addstr(BLANK);

    if(!strncmp(msd, "1", 1))
        addstr(ANTI_T);
    else if(!strncmp(msd, "2", 1))
        addstr(ANTI_S);
    else if(!strncmp(msd, "3", 1))
        addstr(ANTI_M);
    else if(!strncmp(msd, "4", 1))
        addstr(ANTI_L);
    else if(!strncmp(msd, "g", 1))
        addstr(OBSV_G);
    else if(!strncmp(msd, "s", 1)){
        addstr(ENERGY);
    }

    if(!strncmp(spl, "1", 1)){

        if(strlen(energy) < 4){
            move(row = V_X + 12, col = ME_Y + 2);
            addstr(BLANK_4);
            energy[strlen(energy) + 1] = '\0';
            energy[strlen(energy)] = 'O';
            move(row = V_X + 12, col = ME_Y + 2);
            addstr(energy);
        }
    }

    move(row = ME_X + 5, col = ME_Y + 8);
    refresh();

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

    pid_t pid;
    int sockfd, confd;
    char buffer[SIZE], buf[SIZE];
    char date[SIZE];

    char getmsg[MSG_LEN];
    char obsv[SIZE_SHMADD];

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    struct hostent *host;

    short port;
    char *name; 

    char full[32];
    strcat(full, av[1]);
    strcat(full, "|");
    strcat(full, VALUE);
    strcat(full, "|");
    strcat(full, "1");
    strcat(full, "|");
    strcat(full, "1");

    int shmid, shmid_opst;
    char *shmadd, *shmadd_opst;


    int	delay;
    int	ndelay;
    char c;

    int start;

    initscr();
    crmode();
    noecho();
    clear();

    draw(ME_X, ME_Y);
    draw(HE_X, HE_Y);

    strcpy(energy, ENERGY);

    row = ME_X + 5;
    col = ME_Y + 8;
    opst = ME_X + 6;

    dir = 1;
    delay = 10;
    start = 0;

    move(row = ME_X + 5, col = ME_Y + 8);

    shmid = shmid_create();
    shmadd = shmat(shmid, 0, 0);

    shmid_opst = shmid_create();
    shmadd_opst = shmat(shmid_opst, 0, 0);


    strncpy(shmadd, full, SIZE_SHMADD);
    strncpy(shmadd_opst, full, SIZE_SHMADD);

    if((host = gethostbyname("localhost")) == NULL) { 
        fprintf(stderr, "Gethostname error\n"); 
        exit(1); 
    }

    port = atoi(av[2]);
    name = av[1];
    
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { 
        fprintf(stderr, "Socket Error:%s\a\n", strerror(errno)); 
        exit(1); 
    } 
    
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr*)host->h_addr);
    
    if(confd = connect(sockfd, (struct sockaddr *)(&server_addr), sizeof(struct sockaddr)) == -1){ 
        fprintf(stderr, "Connect Error:%s\a\n", strerror(errno)); 
        exit(1); 
    }
    
    send(sockfd, av[1], NM_LEN, 0);

    pid = fork();

    //set_ticker(delay);

    //signal(SIGALRM, update_me); 
    //signal(SIGALRM, update_he);             

    //move(row = ME_X + 5, col = ME_Y + 8);
    //addstr(BLANK);
    //move(row = ME_X + 5, col = ME_Y + 8);

    while(1){
        
        if(pid > 0){       
          
            

            time_t timep;
            struct tm *p;
            time(&timep);
            p = localtime(&timep);
            strftime(date, sizeof(date), "%Y/%m/%d %H:%M:%S", p);

            char *s_t = NULL;
            char *s_f = NULL;

            char nm_md_t[32] = VALUE;
            char nm_md_f[32] = VALUE;

            char msd_f[5];
            char msd_t[5];

            char spl_f[5];
            char spl_t[5];

            char from[NM_LEN];
            char to[NM_LEN];

            int is_me;


            /**
             * copy from 1st and 2nd shared memory
             */

            c = getch();

            memset(nm_md_t, 0, 32);
            strcpy(nm_md_t, shmadd);
            memset(nm_md_f, 0, 32);
            strcpy(nm_md_f, shmadd_opst);
           
            memset(mood_cut, 0, MD_LEN);
            memset(mood_cut_opst, 0, MD_LEN);
            
            /**
             * extract from 1st shared memory
             */
            s_t = strtok(nm_md_t, "|");
            strcpy(from, s_t);

            s_t = strtok(NULL, "|");
            strcpy(mood_cut, s_t);

            s_t = strtok(NULL, "|");
            strcpy(msd_t, s_t);

            s_t = strtok(NULL, "|");
            strcpy(spl_f, s_t);

            /**
             * extract from 2nd shared memory
             */
            s_f = strtok(nm_md_f, "|");
            strcpy(to, s_f);

            s_f = strtok(NULL, "|");
            strcpy(mood_cut_opst, s_f);

            s_t = strtok(NULL, "|");
            strcpy(msd_f, s_t);

            s_t = strtok(NULL, "|");
            strcpy(spl_t, s_t);

            is_me = !strcmp(from, av[1]);            

            move(row = ME_X + 5, col = ME_Y + 8);
            addstr(BLANK);

            if(c == 'q'){
                endwin();
                break;
            }

            if(c == 'c'){

                move(row, col += 10);
                
                move(row = ME_X + 5, col = ME_Y + 8);

                if(is_me)
                    updt_Me(msd_t, spl_f);
                else if(!is_me)
                    updt_He(msd_f, spl_t);

                continue;

            }

            if(c == 'n'){
                draw(ME_X, ME_Y);
                draw(HE_X, HE_Y);
                strcpy(mood_cut, VALUE);
                strcpy(mood_cut_opst, VALUE);
                move(row = ME_X + 5, col = ME_Y + 8);
            }

            if(c == 'r'){
                draw(ME_X, ME_Y);
                draw(HE_X, HE_Y);
                strcpy(mood_cut, VALUE);
                strcpy(mood_cut_opst, VALUE);
                move(row = V_X + 12, col = ME_Y + 2);
                addstr(BLANK_4);
                move(row = V_X + 12, col = ME_Y + 2);
                addstr(energy);
                move(row = ME_X + 5, col = ME_Y + 8);
            }

            if(c == '1'){
                move(row, col += 10);
                addstr(BLANK);
                move(row, col += 10);
                addstr(ATTK_T);
                
                int i = 0;
                int len = strlen(energy);

                if(len - 1 >= 0){
                    move(row = V_X + 12, col = ME_Y + 2);
                    addstr(BLANK_4);

                    memset(energy, 0, ENER_LEN);
                    for(i = 0; i < len - 1; i++)
                        energy[i] = 'O';
                    energy[i++] = '\0';

                    move(row = V_X + 12, col = ME_Y + 2);
                    addstr(energy);
                }
                else continue;

                move(row = ME_X + 5, col = ME_Y + 8);
            }

            else if(c == '2'){
                move(row, col += 10);
                addstr(BLANK);
                move(row, col += 10);
                addstr(ATTK_S);

                int i = 0;
                int len = strlen(energy);

                if(len - 2 >= 0){
                    move(row = V_X + 12, col = ME_Y + 2);
                    addstr(BLANK_4);
                    
                    memset(energy, 0, ENER_LEN);
                    for(i = 0; i < len - 2; i++)
                        energy[i] = 'O';
                    energy[i++] = '\0';

                    move(row = V_X + 12, col = ME_Y + 2);
                    addstr(energy);
                }
                else continue;

                move(row = ME_X + 5, col = ME_Y + 8);
            }

            else if(c == '3'){
                move(row, col += 10);
                addstr(BLANK);
                move(row, col += 10);
                addstr(ATTK_M);

                int i = 0;
                int len = strlen(energy);

                if(len - 3 >= 0){
                    move(row = V_X + 12, col = ME_Y + 2);
                    addstr(BLANK_4);
                    
                    memset(energy, 0, ENER_LEN);
                    for(i = 0; i < len - 3; i++)
                        energy[i] = 'O';
                    energy[i++] = '\0';

                    addstr(energy);
                }
                else continue;

                move(row = ME_X + 5, col = ME_Y + 8);
            }

            else if(c == '4'){
                move(row, col += 10);
                addstr(BLANK);
                move(row, col += 10);
                addstr(ATTK_L);

                int i = 0;
                int len = strlen(energy);

                if(len - 4 >= 0){

                    move(row = V_X + 12, col = ME_Y + 2);
                    addstr(BLANK_4);
                    
                    memset(energy, 0, ENER_LEN);
                    for(i = 0; i < len - 4; i++)
                        energy[i] = 'O';
                    energy[i++] = '\0';

                    move(row = V_X + 12, col = ME_Y + 2);
                    addstr(energy);
                }
                else continue;

                move(row = ME_X + 5, col = ME_Y + 8);
            }

            else if(c == 'g'){
                move(row, col += 10);
                addstr(BLANK);
                move(row, col += 10);
                addstr(GUARD);
                move(row = ME_X + 5, col = ME_Y + 8);
            }

            else if(c == 's'){

                if(strlen(energy) >= 4)
                    continue;

                move(row, col += 10);
                addstr(BLANK);
                move(row, col += 10);
                addstr(SUPPLY);
                move(row = ME_X + 5, col = ME_Y + 8);

            }

            else continue;

            refresh();

            memset(getmsg, 0, strlen(getmsg));
            getmsg[0] = c;
            getmsg[1] = '\0';

            
            
            /*
             * 1000|*******|f|
             * 2016-09-28
             */

            

            memset(buffer, 0, sizeof(buffer));

            /**
             * tell server who are you
             */
            strcat(buffer, av[1]);
            strcat(buffer, "|");

            /**
             * tell server how many values you have
             */
            if(is_me)
                strcat(buffer, mood_cut);
            else strcat(buffer, mood_cut_opst);
            strcat(buffer, "|");

            /**
             * tell server your method
             */
            strcat(buffer, getmsg);
            strcat(buffer, "|");
       
            if(!strncmp("e", buf, 1)){
                printf("offline...\n");
                strcat(buffer,"quit...");

                if((send(sockfd, buffer, SIZE, 0)) <= 0){
                    perror("error send");
                }

                close(sockfd);
                sockfd = -1;
                exit(1);
            }

            else{
                /*strcat(buffer, "\n");
                strcat(buffer, date);
                strcat(buffer, "\n");*/

                if((send(sockfd, buffer, SIZE, 0)) <= 0){
                    perror("send");
                }

            }

            
        }
     
        else if(pid == 0){
            
            char from[5];
            char to[5];
            char mood[MD_LEN];
            char mood_opst[MD_LEN];
            char hurt[5];
            char date[32];

            char *s = NULL;
            char pool[128];

            char shm_t[128];
            char shm_f[128];

            char f_md[5];
            char t_md[5];

            char f_spl[5];
            char t_spl[5];

            int is_me;
            int is_he;

            memset(buffer, 0, SIZE);

            if(sockfd > 0){

                int rcv = recv(sockfd, buffer, SIZE, 0);

                if(rcv <= 0){
                    close(sockfd);
                    exit(1);
                }

                else if(rcv > 0){

                /*
                 * 1000|*******|1001|*****
                 */

                    strcpy(pool, buffer);

                    s = strtok(buffer, "|");
                    strcpy(from, s);

                    s = strtok(NULL, "|");
                    strcpy(mood, s);

                    s = strtok(NULL, "|");
                    strcpy(t_md, s);

                    s = strtok(NULL, "|");
                    strcpy(f_spl, s);

                    

                    s = strtok(NULL, "|");
                    strcpy(to, s);

                    s = strtok(NULL, "|");
                    strcpy(mood_opst, s);
               
                    s = strtok(NULL, "|");
                    strcpy(f_md, s);

                    s = strtok(NULL, "|");
                    strcpy(t_spl, s);

                    

                    is_me = !strcmp(from, av[1]);
                    is_he = !strcmp(to, av[1]);

                    memset(shmadd, 0, SIZE_SHMADD);
                    memset(shmadd_opst, 0, SIZE_SHMADD);

                    memset(shm_t, 0, 32);
                    memset(shm_f, 0, 32);

                    strcat(shm_t, from);
                    strcat(shm_t, "|");
                    strcat(shm_t, mood);
                    strcat(shm_t, "|");
                    strcat(shm_t, t_md);
                    strcat(shm_t, "|");
                    strcat(shm_t, f_spl);
                    strcpy(shmadd, shm_t);
                    

                    strcat(shm_f, to);
                    strcat(shm_f, "|");
                    strcat(shm_f, mood_opst);
                    strcat(shm_f, "|");
                    strcat(shm_f, f_md);
                    strcat(shm_f, "|");
                    strcat(shm_f, t_spl);
                    strcpy(shmadd_opst, shm_f);

                    

                }    
                
            }
        }
    }   

    close(sockfd);
    return 0;    
}
