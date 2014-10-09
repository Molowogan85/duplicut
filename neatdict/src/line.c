#include <string.h>
#include "line.h"
#include "definitions.h"
#include "debug.h"


static void     dlog_obj_t_chunk(t_chunk *chunk)
{
    DLOG("------------------------------");
    DLOG("chunk->tag:           '%d'",  chunk->tag);
    DLOG("chunk->id:            '%d'",  chunk->id);
    DLOG("chunk->offset:        '%ld'", chunk->offset);
    DLOG("chunk->size:          '%ld'", chunk->size);
    DLOG("");
    DLOG("chunk->file.fd:       '%d'",  chunk->file.fd);
    DLOG("chunk->file.name:     '%s'",  chunk->file.name);
    DLOG("chunk->file.offset:   '%ld'", chunk->file.offset);
    DLOG("chunk->file.size:     '%ld'", chunk->file.size);
    DLOG("");
    DLOG("chunk->map.addr:      '%p'",  chunk->map.addr);
    DLOG("chunk->map.size:      '%ld'", chunk->map.size);
    DLOG("");
    DLOG("chunk->next:          '%p'",  chunk->next);
    DLOG("------------------------------");
}

t_line      *next_line(t_line *line, t_chunk *chunk, size_t *offset)
{
    char    *ptr;
    char    *addr;
    size_t  size;
    size_t  delta;

    if (*offset >= chunk->size)
        return (NULL);
    /* if (chunk->id != 34982) */
    /* { */
    /*     dlog_obj_t_chunk(chunk); */
    /*     DLOG("offset=%lu", *offset); */
    /*     DLOG("------------------------------"); */
    /*     DLOG("------------------------------"); */
    /* } */
    addr = (char*)(&chunk->map.addr[chunk->offset] + *offset);
    size = (size_t)(chunk->size - *offset);
    while (1)
    {
        if (*addr == DISABLED_LINE)
        {
            ptr = memchr(addr, '\n', size);
            if (ptr == NULL)
                return (NULL);
            delta = (size_t)(ptr - addr);
            addr = ptr + 1;
            size -= (delta + 1);
            *offset += (delta + 1);
        }
        else if (*addr == '\n')
        {
            addr++;
            size--;
            (*offset)++;
        }
        else
            break;
        if (*offset >= chunk->size)
            return (NULL);
    }
    ptr = memchr(addr, '\n', size);
    if (ptr == NULL)
    {
        SET_LINE(*line, addr, (size - *offset));
        *offset = size;
    }
    else
    {
        SET_LINE(*line, addr, (ptr - addr));
        *offset += LINE_SIZE(*line) + 1;
    }
    return (line);
}


int         cmp_line(t_line *l1, t_line *l2)
{
    int     ret;
    int     size;

    size = LINE_SIZE(*l1);
    if (LINE_SIZE(*l2) != size)
        ret = 1;
    else
        ret = memcmp(LINE_ADDR(*l1), LINE_ADDR(*l2), size);
    return (ret);
}