#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_RETRIES 3
#define CHUNK_SIZE 128

int retries = 0;
HANDLE exitSemaphore = NULL;
volatile int paused = 0;

DWORD WINAPI BackspaceWatcher(LPVOID lpParam) {
    while (1) {
        if (GetAsyncKeyState(VK_BACK) & 0x8000) {
            ReleaseSemaphore(exitSemaphore, 1, NULL);
            break;
        }
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            paused = !paused;
            Sleep(300); 
        }
        Sleep(100);
    }
    return 0;
}

void type(const char *str) {
    INPUT inp = {0};
    inp.type = INPUT_KEYBOARD;

    for (int i = 0; str[i] != '\0'; i++) {
        inp.ki.wVk = 0;
        inp.ki.wScan = str[i];
        inp.ki.dwFlags = KEYEVENTF_UNICODE;
        retries = 0;
         while (SendInput(1, &inp, sizeof(INPUT)) == 0 && retries < MAX_RETRIES) {
            retries++;
            printf("Warning: SendInput failed at character '%c'. Retrying (%d/%d)...\n", str[i], retries, MAX_RETRIES);
            Sleep(50); // Short delay before retry
        }

        // Release event with retry
        inp.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
        retries = 0;
        while (SendInput(1, &inp, sizeof(INPUT)) == 0 && retries < MAX_RETRIES) {
            retries++;
            printf("Warning: SendInput key up failed at character '%c'. Retrying (%d/%d)...\n", str[i], retries, MAX_RETRIES);
            Sleep(50);
        }
        retries = 0;
    }
}

void enter() {
    INPUT inp = {0};
    inp.type = INPUT_KEYBOARD;
    inp.ki.wVk = VK_RETURN;
    SendInput(1, &inp, sizeof(INPUT));
    inp.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &inp, sizeof(INPUT));
}

char* readMessage() {
    char *buffer = NULL;
    size_t size = 0;
    int retries = 0;

    printf("Enter the message to be sent (press ENTER when done):\n");

    while (1) {
        if (size % CHUNK_SIZE == 0) {
            char *newBuffer = realloc(buffer, size + CHUNK_SIZE);
            if (!newBuffer) {
                if (retries++ >= MAX_RETRIES) {
                    printf("FATAL ERROR: Memory allocation failed after %d retries.\n", MAX_RETRIES);
                    free(buffer);
                    return NULL;
                }
                printf("Retrying memory allocation...\n");
                Sleep(500);
                continue;
            }
            buffer = newBuffer;
        }

        int c = getchar();
        if (c == EOF || c == '\n') {
            buffer[size] = '\0';
            break;
        }

        buffer[size++] = (char)c;
    }

    return buffer;
}

int getValidatedInt(const char *prompt, int allowZero) {
    int value;
    char input[32];
    while (1) {
        printf("%s", prompt);
        if (!fgets(input, sizeof(input), stdin)) {
            continue;
        }
        if (sscanf(input, "%d", &value) == 1 && (value > 0 || (allowZero && value == 0))) {
            return value;
        }
        printf("Invalid input. Please enter a %s integer.\n", allowZero ? "non-negative" : "positive");
    }
}

int main() {
      printf("Simple Spamming Script, @astaffz in C v1.0\n");
    printf("------------------------------\n");

    char *message = readMessage();
    if (!message || strlen(message) == 0) {
        printf("Invalid or empty message. Exiting.\n");
        free(message);
        return 1;
    }

    int delay = getValidatedInt("Enter delay between messages in milliseconds: ", 0);
    int repeatLimit = getValidatedInt("Enter how many times to send (infinite, until stopped = 0): ", 1);

    retries = 0;
    while (!exitSemaphore && retries < MAX_RETRIES) {
        exitSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
        if (!exitSemaphore) {
            printf("Semaphore creation failed. Retrying (%d/%d)...\n", ++retries, MAX_RETRIES);
            Sleep(500);
        }
    }
    

    if (!exitSemaphore) {
        printf("FATAL ERROR: Could not create semaphore.\n");
        free(message);
        return 1;
    }
    retries = 0;
    HANDLE watcher; 
    while (!watcher && retries < MAX_RETRIES) {
         watcher = CreateThread(NULL, 0, BackspaceWatcher, NULL, 0, NULL);
       if (!watcher) {
            printf("Semaphore creation failed. Retrying (%d/%d)...\n", retries++, MAX_RETRIES);
            Sleep(500);
        }
    }
    if(!watcher){
      printf("FATAL ERROR: Failed to create backspace watcher thread.\n");
        CloseHandle(exitSemaphore);
        free(message);
        return 1;
    }
    printf("Sending will begin in 3 seconds. Press ESC to pause/resume. Press BACKSPACE to exit.\n");
    Sleep(3000);

    int count = 0;
    while (1) {
        if (!paused) {
            type(message);
            enter();
            count++;

            if (repeatLimit > 0 && count >= repeatLimit) {
                printf("\nReached repeat limit (%d). Exiting...\n", repeatLimit);
                break;
            }

            printf("Sent %d message(s)...\r", count);
        }
        else{
            printf("\nPaused...");
            Sleep(900);
        }

        DWORD result = WaitForSingleObject(exitSemaphore, delay);
        if (result == WAIT_OBJECT_0) {
            printf("\nBackspace detected. Exiting...\n");
            break;
        }
    }

    WaitForSingleObject(watcher, INFINITE);
    CloseHandle(watcher);
    CloseHandle(exitSemaphore);
    free(message);
    return 0;
}
