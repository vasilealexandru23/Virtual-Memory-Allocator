#include "utils.h"

void *alloc_data(const uint64_t bytes) {
    void *data = malloc(bytes);
    if (!data) {
        printf("Eroare la alocare");
    }
    return data;
}