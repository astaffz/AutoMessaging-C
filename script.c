#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_RETRY_AFTER_ERROR 5

size_t buffer = 513;
int scriptErrorCount = 0; 

void type(char str[]){
    INPUT inp;
    inp.type= INPUT_KEYBOARD;
    inp.ki.wScan = 0;
    inp.ki.time = 0;
    inp.ki.dwExtraInfo = 0;

    int i=0;
    for(;str[i] != '\0';i++){
          inp.ki.wVk = 0; 
        inp.ki.wScan = str[i]; 
        inp.ki.dwFlags = KEYEVENTF_UNICODE; 
        SendInput(1, &inp, sizeof(INPUT));

        inp.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP; 
        SendInput(1, &inp, sizeof(INPUT));
    }
}
void enter(){
     INPUT inp;
    inp.type= INPUT_KEYBOARD;
    inp.ki.wScan = 0;
    inp.ki.time = 0;
    inp.ki.dwExtraInfo = 0;
     inp.ki.wVk = VK_RETURN;
    inp.ki.dwFlags = 0;
    SendInput(1, &inp, sizeof(INPUT));
    inp.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &inp, sizeof(INPUT));
}
void FocusViber() {
    HWND hwnd = FindWindow(NULL, "Notepad");
    if (hwnd) {
        SetForegroundWindow(hwnd);
        SetFocus(hwnd);
        Sleep(500);  // Wait for it to be active
           printf("Viber is focused!\n");
    } else {
        printf("Viber not found!\n");
    }

}
int  allocate(char* text){
    while(!text && scriptErrorCount < MAX_RETRY_AFTER_ERROR){
    text = (char*)malloc(buffer);
    if (!text) {
        printf("ERROR 1: Memory allocation failed. Retrying... (Attempt %d of %d) !\n",scriptErrorCount++,MAX_RETRY_AFTER_ERROR);
        Sleep(900);
    }
   
  }
  if(scriptErrorCount){
    if(!text){
       printf("FATAL ERROR: Memory allocation failed after %d attempts. Exiting...", MAX_RETRY_AFTER_ERROR);
       Sleep(2000);
       free(text);
    return 1;  }
    else{
        printf("Memory allocated successfully!\n");
        scriptErrorCount=0;
    }
  }

    return 0;
}
int main() {
    // A counter of how many the script ran into an error, retries until a maximum
    int messageDelay = 1000; // Delay between messages, in milliseconds
     char *msg;
        
 allocate(msg);
    int maxIterations = 0, delay = 0;
    char exitCondition[20], delayType[20];

    // User Input
    printf("Enter your message: ");

    while(fgets(msg+strlen(msg), buffer-strlen(msg), stdin) != NULL){
         // Added strlen in the event of a cutoff (msg longer than 512), pointer arithmetic msg to the end of input
        if(msg[strlen(msg)-1] == '\n'){
            msg[strlen(msg)-1] = '\0';
            break;
        }
        char* temp; 
        buffer*=2;
        while(!temp && scriptErrorCount < MAX_RETRY_AFTER_ERROR){
             temp = (char*)realloc(msg,buffer);
            if (!temp) {
                printf("ERROR 1: Memory allocation failed. Retrying... (Attempt %d of %d) !\n",scriptErrorCount++,MAX_RETRY_AFTER_ERROR);
                Sleep(900);
            }
        }
        if(scriptErrorCount){
            if(!temp){
            printf("FATAL ERROR: Memory allocation failed after %d attempts. Exiting...", MAX_RETRY_AFTER_ERROR);
            Sleep(2000);
            free(temp);
            free(msg);
            return 1;  
            }
            else{
                printf("Memory allocated successfully!\n");
                scriptErrorCount=0;
                msg=temp;
                free(temp);
            }
        }

    }
    printf("Automated Message Script v0.1\n");
    printf("------------------------------\n");
    printf("Enter your message text: ");

    FocusViber();

   
    Sleep(1000);
    char str[] = "";
    type(str);
    enter();


    printf("Message sent. Press Backspace to exit.\n");



    // Step 3: Listen for exit condition (Backspace key)
     
    int i = 3000;
    while (1) {
        
      type("You will have the best time in this semester <33");
      enter();

        if (GetKeyState(VK_BACK)) {
            printf("Exiting...\n");
            break;
        }
        Sleep(900);  // Reduce CPU usage
        i+=1000;
    }
    free(msg);

    return 0;
}
