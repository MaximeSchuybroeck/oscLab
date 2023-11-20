#include <stdio.h>
#include <time.h>

int main() {

    time_t currentTime = time(NULL);
    struct tm *tm_info = localtime(&currentTime);

    printf("%a %b %d %H:%M:%S %Y", tm_info);

    time_t current_time = time(NULL);
    printf("Current time: %ld\n", current_time);
    return 0;
}
