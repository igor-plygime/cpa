/*
 * MIT License
 *
 * Copyright (c) 2017 Igor Sorokin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    size_t start;
    size_t len;

} Level;

static size_t levels_max = 256;

static const char* test_data[] = {
        "../bar", "/bar",
        "/foo/bar", "/foo/bar",
        "/foo/bar/../baz", "/foo/baz",
        "/foo/bar/./baz/", "/foo/bar/baz/",
        "/foo/../../baz", "/baz",
        ".", "/",
        "..", "/",
        "./bar", "/bar",
        "/../baz", "/baz",
        "/../../../../baz", "/baz",
        "/../../bar/../baz", "/baz",
        "/../../bar/../baz/", "/baz/",
        "/./baz", "/baz",
        "/././././baz", "/baz",
        "/../../bar/./baz", "/bar/baz",
        "/././bar/../baz/", "/baz/",
};

char* normalize(const char* path, Level* levels);
void test(void);

/* Work function
 *
 * Second parameter - auxiliary buffer allocated by client if used. Or NULL.
 *
 * */
char* normalize(const char* path, Level* levels)
{
    size_t normalized_len = 1;
    size_t i = 0, len;
    char* normalized;
    char* normalized_offset;
    int dir_name_start = 0, dot_start = 0, dots_start = 0;
    Level* current_level;
    size_t current_level_number = 0;
    int need_free_memory = 0;

    if (!levels)
    {
        /* Allocation of auxiliary buffer */
        levels = malloc(sizeof(Level) * levels_max);
        need_free_memory = 1;
    }

    current_level = levels;

    while (path[i])
    {
        if (path[i] == '/')
        {
            if (dir_name_start)
            {
                dir_name_start = 0;
                current_level->len++;
                normalized_len += current_level->len;
            }
            else if (dot_start)
            {
                dot_start = 0;
            }
            else if (dots_start)
            {
                dots_start = 0;

                if (current_level_number > 0)
                    current_level_number--;
            }

            i++;
            continue;
        }
        else if (path[i] == '.')
        {
            if (dir_name_start)
            {
                dir_name_start = 0;
                normalized_len += current_level->len;
            }

            if (dot_start)
            {
                dot_start = 0;
                dots_start = 1;
            }
            else
            {
                dot_start = 1;
            }

            i++;
            continue;
        }
        else
        {
            if (!dir_name_start)
            {
                dir_name_start = 1;
                current_level_number++;

                if (current_level_number == levels_max)
                    break;

                current_level = levels + current_level_number;
                current_level->start = i;
                current_level->len = 0;
            }

            current_level->len++;
            i++;
        }
    }

    normalized_len++;

    normalized = malloc(normalized_len);
    normalized[normalized_len - 1] = 0;
    normalized[0] = '/';
    normalized_offset = normalized + 1;

    for (i = 0; i <= current_level_number; i++)
    {
        len = levels[i].len;
        memcpy(normalized_offset, path + levels[i].start, len);
        normalized_offset += len;
    }

    if (need_free_memory)
        /* Don't forget to free the allocated auxiliary buffer */
        free(levels);

    return normalized;
}

void test()
{
    size_t i;
    size_t test_data_size = sizeof(test_data) / sizeof(char*);
    const char* path;
    const char* normalized;
    const char* check;

    /* Client can allocate auxiliary buffer for normalize function (second parameter)
     *
     * Level* levels = malloc(sizeof(Level) * levels_max);
     *
     * normalized = normalize(path, levels);
     *
     * */

    for (i = 0; i < test_data_size; i += 2)
    {
        path = test_data[i];
        check = test_data[i + 1];
        normalized = normalize(path, NULL);

        if (!strcmp(normalized, check))
            printf("PASS: %s\t\t\t\t%s = %s\n", path, normalized, check);
        else
            printf("FAIL: %s\t\t\t\t%s != %s\n", path, normalized, check);
    }
}

int main(int argc, const char* argv[])
{
    if (argc <= 2)
    {
        if (argv[1])
        {
            /* Client can allocate auxiliary buffer for normalize function (second parameter)
             *
             * Level* levels = malloc(sizeof(Level) * levels_max);
             *
             * normalized = normalize(path, levels);
             *
             * */
            printf("%s\n", normalize(argv[1], NULL));
        }
        else
            test();

        return 0;
    }
    else
    {
        printf("Using: normalize <path>\n"
                "Testing: normalize\n");
        return -1;
    }
}
