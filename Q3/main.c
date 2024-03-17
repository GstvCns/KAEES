#include <stdio.h>
#include <stdlib.h>

#define MAX_NUM_ELEMENTS 32
#define ELEMENTS_MAX_VALUE 32

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

typedef int random_t;

random_t *pQuestion_3_function(random_t *array, size_t origin_size, double *average, random_t *max, random_t *min, size_t *copy_size);

void main(void)
{
    while (1)
    {
        size_t origin_array_size = rand() % (MAX_NUM_ELEMENTS + 1);
        printf("Origin array size: %lld\n", origin_array_size);

        random_t *pOrigin_array, *pCopy_array;
        pOrigin_array = (random_t *)malloc(origin_array_size * sizeof(origin_array_size));
        for (size_t i = 0; i < origin_array_size; i++)
        {
            pOrigin_array[i] = (random_t)(rand() % (ELEMENTS_MAX_VALUE + 1));
            printf("%d ", pOrigin_array[i]);
        }
        printf("\n");

        double avg_value = 0;
        random_t max_value = 0;
        random_t min_value = ELEMENTS_MAX_VALUE;
        size_t copy_array_size;

        pCopy_array = pQuestion_3_function(pOrigin_array, origin_array_size, &avg_value, &max_value, &min_value, &copy_array_size);

        printf("Average: %lf\n", avg_value);
        printf("Max: %d\n", max_value);
        printf("Min: %d\n", min_value);
        printf("Copy array size: %lld\n", copy_array_size);

        for (size_t i = 0; i < copy_array_size; i++)
        {
            printf("%d ", pCopy_array[i]);
        }
        printf("\n\n");

        free(pOrigin_array);
        free(pCopy_array);
        _sleep(10000);
    }
}

random_t *pQuestion_3_function(random_t *array, size_t origin_size, double *average, random_t *max, random_t *min, size_t *copy_size)
{
    size_t even_counter = 0;
    random_t *pTemp_array = malloc(1);
    for (size_t i = 0; i < origin_size; i++)
    {
        *max = MAX(array[i], *max);
        *min = MIN(array[i], *min);
        *average += (double)(array[i]) / origin_size;
        if (array[i] % 2 == 0)
        {
            even_counter++;
            pTemp_array = (random_t *)realloc(pTemp_array, even_counter * sizeof(random_t));
            pTemp_array[even_counter - 1] = array[i];
        }
    }
    *copy_size = even_counter;
    return pTemp_array;
}