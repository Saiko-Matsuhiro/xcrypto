#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <assert.h>



char* hash_to_pass(char* password);
int check_pass(char* password);
char* read_file_to_buffer(const char* filename, size_t* out_size);
int code_file(char* file, size_t size, char* password);
int write_file(char* file, char* name, size_t size);



int main(){
    char password[1024];
    memset(password, 0, 1024);
    int fd = open("config.txt", O_RDWR | O_CREAT, 0600);
    if (read(fd, password, 32)<32){
        printf("create password\n");
        char pass[1024];
        if (scanf("%1023s", pass)!=1){
            return -1;
        }
        lseek(fd, 0, SEEK_SET); 
        hash_to_pass(pass);
        if(write(fd, pass, 32)<0){
            perror("Ошибка функции write"); 
        }
    }
    close(fd);
    
    printf("enter password\n");
    if (scanf("%1023s", password) != 1) {
        return -1;
    }
    if (check_pass(password)!=0){
        return -1;
    }
    char name[1024];
    char* file;
    size_t size;
    while (1){
        printf("enter file name for code/decode or write exit\n");
        if (scanf("%1023s", name) != 1) {
            return -1;
        }
        if (strcmp(name, "exit")==0){
            break;
        }
        file = read_file_to_buffer(name, &size);
        if (file == NULL){
            printf("error\n");
            continue;
        }

        code_file(file, size, password);
    
        write_file(file, name, size);

        free(file);
    }


    
    return 0;

}

int write_file(char* file, char* name, size_t size){
    int fd = open(name, O_WRONLY);
    write(fd, file, size);
    close(fd);
}


int code_file(char* file, size_t size, char* password){
    for (int i = 0; i<size; i++){
        file[i] = password[(i%256)+767]^file[i];
    }
}


char* hash_to_pass(char* password){
    if (password[0]=='\0'){
        return NULL;
    }
    for (int i = 0; i<1024; i++){
        if (password[i]=='\0'){
            int last_pass_char = i;
            for (int j = i; j<1024; j++){
                password[j] = password[j-1] ^ password[j-last_pass_char];
            }
            break;
        }
    }
    for (int i = 0; i<1023; i++){
        password[i]=((password[i]*password[i+1])%127)+1;
    }
    char between;
    char new;
    for (int i = 0, j = 1023; i<512; i++, j--){
        between = password[(password[i]*password[j])%1024];
        password[(password[i]*password[j])%1024] = ((password[i]^password[j])%127)+1;
        new = password[j]^between;
        password[j] = ((password[i]^between)%127)+1;
        password[i] = (new%127)+1;
    }

    for (int i = 0; i<32; i++){
        password[i] = password[(password[i]*password[i])%1024];
    }
    
    return password;
}


int check_pass(char* password){
    
    int fd = open("config.txt", O_RDONLY);
    char hashpass[32];
    if (read(fd, hashpass, 32)<32){
        return -1;
    }
    close(fd);
    char* pass = hash_to_pass(password);
    if (memcmp(hashpass, pass, 32)==0){
        return 0;
    } else{
        return -1;
    }
}


char* read_file_to_buffer(const char* filename, size_t* out_size) {
    struct stat file;
    
    
    if (stat(filename, &file) != 0) {
        return NULL;
    }
    
    size_t size = file.st_size;
    if (size == 0) {
        return NULL;
    }

    
    int fd = open(filename, O_RDWR);
    if (fd < 0) {
        return NULL;
    }
    
    
    char* read_f = malloc(size);
    if (read_f == NULL) {
        close(fd);
        return NULL;
    }

    char* read_f_new = read_f;
    size_t remaining_size = size;


    while (remaining_size > 0) {
        ssize_t read_s = read(fd, read_f_new, remaining_size);
        
        if (read_s < 0) {
            free(read_f);
            close(fd);
            return NULL;
        }
        
        if (read_s == 0) {
            break; 
        }

        remaining_size -= read_s;
        read_f_new += read_s; 
    }

    close(fd);


    *out_size = size - remaining_size;
    
    return read_f;
}