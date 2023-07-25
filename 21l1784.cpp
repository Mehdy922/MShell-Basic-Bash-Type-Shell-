#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>


using namespace std;

char** Tokenize(char* command)
{
    char** Tokens = new char*[100];
    int i = 0;
    int j = 0;
    int len = strlen(command);
    Tokens[0] = new char[len+1]; // allocate memory for first token
    int token_count = 1;

    while(i<len)
    {
        if(command[i]!=' ')
        {
            Tokens[token_count-1][j] = command[i];
            j++;
		Tokens[token_count-1][j] = '\0'; // add null terminator

        }
        else
        {
            i++;
            j=0;
            Tokens[token_count] = new char[len+1]; // allocate memory for next token
            token_count++;
            continue;
        }
        i++;
    }

    Tokens[token_count] = NULL; // set last token to NULL for execvp
    return Tokens;
}

void Checker(char** Tokens, bool* s)
{
    int pcount = 0;
    int tempo = open("temp.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    int i = 0;
    int last = 0;
    bool t = false;
    while(Tokens[i]!=NULL)
    {
        
        if(Tokens[i][0] == '>' || Tokens[i][1] == '>' ) 
        {
                int original_stdout = dup(1);
                cout << "Output Redirection" << endl;
                  cout << Tokens[last] << endl;
                cout << Tokens[last+1] << endl;
                int out = open(Tokens[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                dup2(out, 1);
            
              
                char * arr[]= {Tokens[last], Tokens[last+1], NULL};
                execvp(Tokens[last], arr);

                dup2(original_stdout,1);

                cout << endl;

                last = i+1;
                t = true;
            
        }
        else if(Tokens[i][0] == '<')
        {
            
            int original_stdin = dup(0);
            cout << "Input Redirection" << endl;
            int in = open(Tokens[i+1], O_RDONLY | O_CREAT , 0644);
            dup2(in, 0);
            
            
            if(Tokens[last+1][0]!='<'){
            char * arr[]= {Tokens[last], Tokens[last + 1], NULL};
            execvp(Tokens[last], arr);
            }
            else
            {
            char * arr[]= {Tokens[last], NULL};
            execvp(Tokens[last], arr);
            }


            dup2(original_stdin,0);
            cout << endl;
            last = i+1;
            t = true;
           
        }
        else if(Tokens[i][0] == '|')
        {
                int original_stdout = dup(1);
                int original_stdin = dup(0);
            
                cout << "Pipe" << endl;
                int fd[2];
                pipe(fd);
                int pid = fork();

                if (pid == 0)
                {
                    dup2(fd[1], 1);
                    close(fd[0]);
                    char * arr[]= {Tokens[last], Tokens[last+1], NULL};
                    execvp(Tokens[last],arr);
                    

                }
                else
                {
                    wait(NULL);

                    dup2(fd[0], 0);
                    close(fd[1]);
                    char * arr[]= {Tokens[i+1], Tokens[i+2], NULL};
                    execvp(Tokens[i+1],arr);
                 
                }

                dup2(original_stdout,1);
                dup2(original_stdin,0);
                close(fd[0]);
                close(fd[1]);
                cout << endl;
                last = i+1;
                t = true;

        }
        i++;
    
    }   
   if(t == false)
    {
        *s = true;
    }
    
}


bool isEmpty(string arr[])
{
    for(int i = 0; i<10;i++)
    {
        if(arr[i]!="\0")
        {
            return false;
        }
    }
    return true;
}

int main(int argc,char* argv[])
{
    bool exitFlag = false;
    int i = 0;
    char str[4] = {'e','x','i','t'};
    char hcheck[7] = {'h','i','s','t','o','r','y'};
    string History[10] ;
    for(int i = 0; i < 10; i++)
    {
        History[i] = "\0";
    }
    int histCount = 0;
    cout << "WELCOME TO SHELL++" << endl;
    cout << "This is a simple shell program" << endl;
    cout << "It will execute the commands you enter" << endl;
    cout << "Enter 'exit' to quit" << endl;
    cout << "Note: Commands are executed in the background by & if you want\n";
    cout<<  "      keep in mind the terminal might glitch out so it will\n";
    cout<<  "      seem like the program is not working as the line 'Enter a command:'\n";
    cout<<  "      will appear out of place as the process might complete after\n";
    cout<<  "      So just keep giving inputs after that it should run okay\n";
    cout << "----------------------------------------" << endl;
    cout << "----------------------------------------" << endl;
    
   while(!exitFlag)
   {
    //sleep(1);
    char* command = new char[1024]; // allocate 1024 bytes for command 
    cout << "Enter a command: ";
    cin.getline(command,1024);

    int len = strlen(command);
    
    while(i<len)
    { 
        if(command[i]==str[i])
        {
            i++;
            exitFlag = true;
            continue;    
        }
        else
        {
            exitFlag = false;
            break;
        }
        
    }
    i=0;
    int h = 0;
    bool dispHistory = false;
    bool latest = false;
    bool checkNUM = false;
    int num = 0;
    if(len == 2)
    {
        if(command[0] == '!')
        {
            if(command[1] == '!')
            {
                latest = true;
            }
            else
            {
                const char * numtemp = &command[1];
                num = atoi(numtemp);
                checkNUM = true;
            }
        }
    }

    i = 0;

    while(i!=7)
    {
            if(command[i]==hcheck[h])
            {
                i++;
                h++;
                dispHistory = true;
                continue;
            }
            else
            {
                dispHistory = false;
                break;
            }
    }
    
    i = 0;
    
    bool background = false;
  
    if(len>0 && command[len-1]=='&')
    {
        background = true;
        command[len-1] = '\0'; // remove '&' from command string
    }

    bool simpleCommand = false;
    char** Tokens = Tokenize(command);

    int pid = fork();
    if(pid == 0)
    {  
    Checker(Tokens, &simpleCommand);
    }
    else
    {
        wait(NULL);
    }


    if(!dispHistory&& !latest&& !checkNUM)
    {
    History[histCount] = Tokens[0];

    histCount++;
    }
    if (histCount == 11)
    {
        histCount = 0;
    }
    
    
    if(dispHistory)
    {
        if(isEmpty(History))
        {
            cout << "No commands in history" << endl;
        }
        else{
        cout << "History:" << endl;
        int curr = histCount - 1;
        bool fl = false;
        int i = histCount - 1;
        
        while(History[i]!="\0")
        {
            if(fl == true && i == curr)
            {
                break;
            }
            if(i == -1)
            {
                i = 9;
                if(History[i]=="\0" || i == curr)
                {
                    break;
                }
            }
            cout << i <<".  " << History[i] << endl;
            fl = true;
            i--;
            
        }
        dispHistory = false;
        }
    }
    else if(latest)
    {
        
        if(isEmpty(History))
        {
            cout << "No commands in history" << endl;
        }
        else
        {
            
        if(History[histCount-1]!="\0")
            cout << "Latest Command: " << History[histCount-1] << endl;
        else
        {
            cout << "No previous command" << endl;
        }
        latest = false;
        }
    }
    else if(checkNUM)
    {
        if(isEmpty(History))
        {
            cout << "No commands in history" << endl;
        }
        else{
        cout << "Command " << num << ": " << endl;
        int temp = histCount -1;
        bool ft = true;
        while(num != 0)
        {
            if(History[temp]!="\0")
            {
                temp--;
                num--;
                if(temp == -1)
                {
                    temp = 9;
                }
            }
            else
            {
                cout << "Command not found" << endl;
                ft = false;
                break;
            }
            
        }
        if(ft)
        {
            cout << History[temp] << endl;
        }

        checkNUM = false;
        }
    }
    else if(simpleCommand)
    {
    pid_t pid = fork();
    if(pid==0)
    {
        execvp(Tokens[0],Tokens);
        
    }
    else
    {
        if(background)
        {
            // do not wait for child process to finish
            cout << "Command executed in background" << endl;
        }
        else
        {
            wait(NULL); // wait for child process to finish
        }
    }
    delete[] Tokens; // free memory allocated by Tokenize
    delete[] command;
    }
   }

}