#ifndef WEBRTCJS_IVF_UTILS_H
#define WEBRTCJS_IVF_UTILS_H

static void mem_put_le16(char* mem, unsigned int val) {
  mem[0] = val;
  mem[1] = val>>8;
}

static void mem_put_le32(char *mem, unsigned int val) {
  mem[0] = val;
  mem[1] = val>>8;
  mem[2] = val>>16;
  mem[3] = val>>24;
}

void write_ivf_file_header(FILE *outfile,
    uint32_t width, uint32_t height, int frame_cnt) {
  char header[32];
  header[0] = 'D';
  header[1] = 'K';
  header[2] = 'I';
  header[3] = 'F';
  mem_put_le16(header+4, 0);            // version
  mem_put_le16(header+6, 32);           // headersize
  mem_put_le32(header+8, 0x30385056);   // headersize
  mem_put_le16(header+12, width);       // width
  mem_put_le16(header+14, height);      // height
  mem_put_le32(header+16, 100000);      // rate
  mem_put_le32(header+20, 1);           // scale
  mem_put_le32(header+24, frame_cnt);   // length
  mem_put_le32(header+28, 0);           // unused
  (void) fwrite(header, 1, 32, outfile);
}

void write_ivf_frame_header(FILE *outfile, uint32_t time_stamp,
    int32_t buffer_length) {
  char header[12];
  uint64_t ts;
  ts = (uint64_t)time_stamp;
  mem_put_le32(header, buffer_length);
  mem_put_le32(header+4, ts & 0xFFFFFFFF);
  mem_put_le32(header+8, ts >> 32);
  (void) fwrite(header, 1, 12, outfile);
}

#endif