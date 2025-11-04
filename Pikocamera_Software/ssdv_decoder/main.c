/**
 * @brief Decode SSDV packets from .bin file and create image
 * @author Rishav
 *
 * @cite fsphil's SSDV
**/

#include <stdio.h>
#include <stdlib.h>

#include "ssdv.h"
#include "ssdv_packets.h"

int main()
{
  FILE *fout_bin = stdout;
  fout_bin = fopen("build/output_image.bin", "wb");
  fwrite(ssdv_packets, 1, sizeof(ssdv_packets), fout_bin);
  fclose(fout_bin);

  FILE *fin = stdin;
  FILE *fout = stdin;
  fin = fopen("build/output_image.bin", "rb");
  fout = fopen("output_image.jpeg", "wb");

  int droptest = 0;
  int verbose = 0;
  int errors;
  int i;

  ssdv_t ssdv;
  uint8_t pkt[SSDV_PKT_SIZE];

  uint8_t *jpeg;
  size_t jpeg_length;

  ssdv_dec_init(&ssdv);
  jpeg_length = 1024 * 1024 * 4;
  jpeg = (uint8_t *)malloc(jpeg_length);
  ssdv_dec_set_buffer(&ssdv, jpeg, jpeg_length);

  i = 0;
  while (fread(pkt, 1, SSDV_PKT_SIZE, fin) > 0)
  {
    if (droptest && (rand() / (RAND_MAX / 100) < droptest))
      continue;

    if (ssdv_dec_is_packet(pkt, &errors) != 0)
      continue;

    if (verbose)
    {
      ssdv_packet_info_t p;

      ssdv_dec_header(&p, pkt);
      fprintf(stderr, "Decoded image packet. Callsign: %s, Image ID: %d, Resolution: %dx%d, Packet ID: %d (%d errors corrected)\n"
                      ">> Type: %d, Quality: %d, EOI: %d, MCU Mode: %d, MCU Offset: %d, MCU ID: %d/%d\n",
              p.callsign_s,
              p.image_id,
              p.width,
              p.height,
              p.packet_id,
              errors,
              p.type,
              p.quality,
              p.eoi,
              p.mcu_mode,
              p.mcu_offset,
              p.mcu_id,
              p.mcu_count);
    }

    ssdv_dec_feed(&ssdv, pkt);
    i++;
  }
  ssdv_dec_get_jpeg(&ssdv, &jpeg, &jpeg_length);
  fwrite(jpeg, 1, jpeg_length, fout);
  free(jpeg);

  fprintf(stderr, "Read %i packets\n", i);
}