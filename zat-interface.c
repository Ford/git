#include "git-compat-util.h"
#include "zat-interface.h"
#include "zat/src/zat_stream.h"

int zat_enabled = 0;
int zat_recompress = 0;

int zat_to_strbuf(int level, const char *src, ssize_t len, struct strbuf *dst) {
	zatbuf zbuf = {0};

	int status = zat_exec (level, (z_const Bytef *)src, len, (in_func)NULL,0, (out_func)zatbuf_out,&zbuf );

	if (status == Z_STREAM_ERROR)
		return 0;

	if (status != Z_STREAM_END) {
		zatbuf_dump( &zbuf, 0, NULL);
		return 0;
	}

	if (!dst) {
		zatbuf_dump( &zbuf, 0, NULL);
		return 1;
	}

	zatbuf_out( &zbuf, NULL, 0 ); // flush
	char *zstr = realloc(zbuf.data, zbuf.size + 1);
	zstr[zbuf.size] = '\0';
	strbuf_attach(dst, zstr, zbuf.size, zbuf.size + 1);

	return 1;
}

struct streamzat {
	struct odb_read_stream *st;
	Bytef *data;
	size_t size;
};

static unsigned streamzat_in(struct streamzat *szat, Bytef **bufP)
{
	*bufP = szat->data;

	return odb_read_stream_read(szat->st, szat->data, szat->size);
}

static int fwrite_out(int *filep, Bytef *dat,unsigned len)
{
	ssize_t wrote = write_in_full(*filep, dat, len);

	return wrote < 0 ? Z_STREAM_ERROR : Z_OK;
}

int zat_istream_to_fd(struct odb_read_stream *st, char *buf, ssize_t readlen, ssize_t maxbuflen, int fd) {
	struct streamzat szat = { st, (Bytef *)buf, maxbuflen };

	int status = zat_exec(zat_recompress, (Bytef *)buf,readlen,
		(in_func) streamzat_in, &szat,
		(out_func)fwrite_out, &fd);

	return status == Z_STREAM_END ? 0 : -1;
}
