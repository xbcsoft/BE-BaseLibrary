#pragma once
#include "stdafx.h"

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total_size = size * nmemb;
    printf("%.*s", (int)total_size, (char*)contents);
    return total_size;
}

int main();
