#pragma once


#define ESTIMATE_COMPRESSED_SIZE(src)		(src * 2 + 0xf)

int LzmaEnc(const void *src, int src_len, void *dst, int dst_buf_len);
bool LzmaDec(const void *compressed_data, int compressed_len, void *dst, int original_len);
