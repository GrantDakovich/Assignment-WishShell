#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <math.h>
int count = 0;
char error_message[30] = "An error has occurred\n";
char* history_list[100];
char* path_list[100];

void initializer(){
   path_list[0] = "/bin/";
   path_list[1] = NULL;
}

void sys_exit(){
   exit(0);
}
int sys_cd(char* str){
   int success = chdir(str);
   return success;
}

void sys_history(int n){ 
   n = ceil(n); 
   int i;
   if (n < 0){
      i = 0;
   }
   else{
      i = n;
   }
   while (i < count){
      printf("%s",history_list[i]);
      fflush(stdout);
      i++;
   }
}

int check_slash(char* path){
   char* pch = strchr(path,'/');
   while (pch != NULL){
      if (strcmp((pch + 1),"\0") == 0){
         return 0;
      }
      pch = strchr(pch + 1,'/');
   }
   return 1;
}

void sys_path(char* arg_list[]){
   int ind = 1;
   while(arg_list[ind] != NULL){
      if (check_slash(arg_list[ind]) == 1){
         strcat(arg_list[ind],"/");
      }
      path_list[ind - 1] = arg_list[ind];
      ind++;
   }
   path_list[ind] = NULL;
}

int count_pipes(char* str){
   int pipe_count = 0;
   char *pch = strchr(str,'|');
   while (pch != NULL){
      pipe_count++;
      pch = strchr(pch + 1,'|');
   }
   return pipe_count;
}
int count_carrots(char* str){
   int carrot_count = 0;
   char* pch = strchr(str,'>');
   while (pch != NULL){ 
      carrot_count++;
      pch = strchr(pch + 1,'>');
   }
   return carrot_count;
}


int main(int argc, char* argv[]){
   initializer();
   FILE* f;
   int peter_piper[2];
   if (argc > 2){
      write(STDERR_FILENO, error_message,strlen(error_message));
      fflush(stderr);
      exit(1);
   }
   if (argc != 1){
      f = fopen(argv[1],"r");
      if (f == NULL){
         write(STDERR_FILENO, error_message,strlen(error_message));
	 fflush(stderr);
	 exit(1);
      }
   }
   int piped_pos;
   int pipe_val;
   int rdr_val;
   size_t buf = 0;
   int next_line = 1;
   while (1){
      fflush(stdout);
      fflush(stdin);
      fflush(stderr);
      pipe_val = 0;
      rdr_val = 0;
      char* rdr_str_token;
      char* arg_list[1000];
      char* piped_arg_list[1000];
      char* str = NULL;
      if (argc == 1){
         printf("wish> ");
	 fflush(stdout);
         getline(&str,&buf,stdin);
      }
      else{
         next_line = getline(&str,&buf,f);
         if (next_line == -1){
            break;
	 }
      }
	   
      history_list[count] = strdup(str);
      count++;
      
      // Check pipes and counter 
      int pipe_count = count_pipes(str);
      int carrot_count = count_carrots(str); 
      if (carrot_count > 1 || pipe_count > 1 || (pipe_count == 1 && carrot_count == 1)){
         write(STDERR_FILENO, error_message, strlen(error_message));
	 fflush(stderr);
	 exit(0);
      }
      else{
	 piped_pos = 1;
	 if(pipe_count == 1){
            char* find_bar = strchr(str,'|');
	    if (find_bar == str){
               write(STDERR_FILENO, error_message, strlen(error_message));
	       fflush(stderr);
	       exit(0);
	    }
            else{
	       char* second_arg = find_bar + 1;
               *find_bar = '\0';
	       if (str == NULL || second_arg == NULL){
                  write(STDERR_FILENO, error_message, strlen(error_message));
	          fflush(stderr);
	       }
	       pipe_val = 1;
	       
               char* piped_str_token = strtok(second_arg," \t\n");
               if (piped_str_token != NULL){
                  piped_arg_list[0] = strdup(piped_str_token);
                  while ((piped_str_token = strtok(NULL," \t\n")) != NULL){
                     piped_arg_list[piped_pos] = strdup(piped_str_token);
                     piped_pos++;
		  }
                  piped_arg_list[piped_pos] = NULL;
	       }
            }
         }
	 if (carrot_count == 1){
            char* find_carrot = strchr(str,'>');
	    if (find_carrot == str){
               write(STDERR_FILENO, error_message, strlen(error_message));
	       fflush(stderr);
	    }
	    else{
               char* to_file = find_carrot + 1;
	       *find_carrot = '\0';
               if (str == NULL || to_file == NULL){
                  write(STDERR_FILENO, error_message, strlen(error_message));
		  fflush(stderr);
	       }
	       rdr_val = 1;
	       rdr_str_token = strtok(to_file," \t\n");
	    }
	 }
      }
           
      /// Push all args
      char* str_token = strtok(str," \t\n");
      if (str_token != NULL){
         arg_list[0] = strdup(str_token);
         int pos = 1;
         while ((str_token = strtok(NULL," \t\n")) != NULL){
            arg_list[pos] = strdup(str_token);
            pos++;
         }
         arg_list[pos] = NULL; 

	 // Built in function calls 
         if(strcmp(arg_list[0],"exit") == 0){
            sys_exit();
         }
         else if(strcmp(arg_list[0],"cd") == 0){
            if (pos == 2){
               if(sys_cd(arg_list[1]) == -1){
                 write(STDERR_FILENO, error_message, strlen(error_message));
                 fflush(stderr);
		 fflush(stdout);
               }	    
	    }else{
               write(STDERR_FILENO, error_message, strlen(error_message));
	       fflush(stderr);
	       fflush(stdout);
	    }
         }
	 else if(strcmp(arg_list[0],"history") == 0){
            if (pos > 2){
               write(STDERR_FILENO, error_message, strlen(error_message));
	       fflush(stderr);
	       fflush(stdout);
	    }else{
               int max_output = count;
	       if (arg_list[1] != NULL){
                  max_output = atoi(arg_list[1]);
	       }
               sys_history(count - max_output);
            }
         }
	 else if(strcmp(arg_list[0],"path") == 0){
            sys_path(arg_list);
         } 
	 //Now we know it must be not built in or not a function
         else{
            char* file_path[100];
	    char* second_file_path[100];
	    file_path[0] = NULL;
	    int i = 0;
            while (path_list[i] != NULL){
               file_path[i] = strdup(path_list[i]);
	       strcat(file_path[i],arg_list[0]);
	       int err_no = 0;
	       if (pipe_val == 1){
                  second_file_path[i] = strdup(path_list[i]);
		  strcat(second_file_path[i],piped_arg_list[0]);
	       }
	       if (access(file_path[i],X_OK) != 0){
                  write(STDERR_FILENO, error_message, strlen(error_message));
		  fflush(stderr);

	       } 
	       else {
                  if (pipe_val == 0 && rdr_val == 0){
		     int rc = fork();
                     if (rc < 0) { // fork failed; exit
                        write(STDERR_FILENO, error_message, strlen(error_message));
                        fflush(stderr);
                     } else if (rc == 0) { // child (new process)
                        err_no = execvp(file_path[i], arg_list); // runs word count
                     } else { // parent goes down this path (main)
                        int* status = 0;
                        waitpid(rc,status,0);
                     }
		     break;
	          
	             if (err_no == -1){
                        write(STDERR_FILENO, error_message, strlen(error_message));
		        fflush(stderr);
	             }
	          }
		  else if(rdr_val == 1){
	             int rc = fork();
                     if (rc < 0) { // fork failed; exit
                        write(STDERR_FILENO, error_message, strlen(error_message));
                        fflush(stderr);
                        exit(1);
                     } else if (rc == 0) { // child (new process)
                        int out_num = open(rdr_str_token, O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
			dup2(out_num,STDOUT_FILENO);
			err_no = execv(file_path[i],arg_list);
                     } else { // parent goes down this path (main)
                        int* status = 0;
                        waitpid(rc,status,0);
                     }
                     break;

                     if (err_no == -1){
                        write(STDERR_FILENO, error_message, strlen(error_message));
                        fflush(stderr);
                     }
		  }
		  else {
                     //printf("Ready to lay the pipe\n");
                     pipe(peter_piper);
		     int rc = fork();
                     if (rc < 0){
                        write(STDERR_FILENO, error_message, strlen(error_message));
			fflush(stderr);
		     }
                     else if (rc == 0){
		        dup2(peter_piper[1],1);
		        close(peter_piper[0]);
		        execv(file_path[i],arg_list);
		     }
		     else{
                        close(peter_piper[1]);
			int* status1 = 0;
			waitpid(rc,status1,0);
			int sec_rc = fork();
			if (sec_rc == 0){
			   close(peter_piper[1]);
			   dup2(peter_piper[0],0);
			   execv(second_file_path[i],piped_arg_list);
			}
			else{
                           int* status2 = 0;
			   close(peter_piper[0]);
			   waitpid(sec_rc,status2,0);
			}
		     }
		  } 
	       }
	       i++;
	       fflush(stdout);
	    }
	 }
      }   
   }
   fclose(f);
}
