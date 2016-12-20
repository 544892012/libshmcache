#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "logger.h"
#include "hash.h"
#include "shmcache.h"

int main(int argc, char *argv[])
{
	int result;
    struct shmcache_config config;
    struct shmcache_context context;
    struct shmcache_buffer key;
    struct shmcache_buffer value;
    int ttl;

    if (argc < 4) {
        fprintf(stderr, "Usage: %s <key> <value> <ttl>\n", argv[0]);
        return EINVAL;
    }

	log_init();
	g_log_context.log_level = LOG_DEBUG;

    memset(&config, 0, sizeof(config));

    strcpy(config.filename, "/tmp/shmcache.dat");
    config.max_memory = 4 * 1024 * 1024;
    config.segment_size = 1 * 1024 * 1024;
    config.max_key_count = 10000;
    config.max_value_size = 64 * 1024;
    config.type = SHMCACHE_TYPE_SHM;  //shm or mmap

    config.va_policy.avg_key_ttl = 600;
    config.va_policy.discard_memory_size = 128;
    config.va_policy.max_fail_times = 5;

    config.lock_policy.trylock_interval_us = 1000;
    config.lock_policy.detect_deadlock_interval_ms = 1000;
    config.hash_func = simple_hash;

    if ((result=shmcache_init(&context, &config)) != 0) {
        return result;
    }

    key.data = argv[1];
    key.length = strlen(key.data);
    value.data = argv[2];
    value.length = strlen(value.data);
    ttl = atoi(argv[3]);
    result = shmcache_set(&context, &key, &value, ttl);
    if (result != 0) {
        fprintf(stderr, "set fail, errno: %d\n", result);
    }

	return result;
}
