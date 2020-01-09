#ifndef TAIHANG_DOWNLOAD_H
#define TAIHANG_DOWNLOAD_H

int taihang_download(uint32_t addr, int len, uint8_t image_file[]);
int dev_info_get(uint8_t * version);
int reset_device();
#endif