#ifndef ZAT_INTERFACE_H
#define ZAT_INTERFACE_H

#include "strbuf.h"
#include "odb/streaming.h"

extern int zat_enabled;
extern int zat_recompress;

enum { ZAT_CLEAN = 0 };

int zat_to_strbuf(int level, const char *src, ssize_t len, struct strbuf *dst);
int zat_istream_to_fd(struct odb_read_stream *odb, char *buf, ssize_t readlen, ssize_t maxbuflen, int fd);

#endif
