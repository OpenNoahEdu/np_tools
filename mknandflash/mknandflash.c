/*
 * create nand flash image for pavo board
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 or
 * (at your option) version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>

#include "nand_flash.h"

char *flash_image = NULL;

char *uboot = NULL;
uint32_t uboot_start, uboot_end, uboot_add_ecc;
char *serial = NULL;
uint32_t serial_start, serial_end, serial_add_ecc;
char *uimage = NULL;
uint32_t uimage_start, uimage_end, uimage_add_ecc;
char *rootfs = NULL;
uint32_t rootfs_start, rootfs_end, rootfs_oob, rootfs_add_ecc;
char *settings = NULL;
uint32_t settings_start, settings_end, settings_oob, settings_add_ecc;
char *progfs = NULL;
uint32_t progfs_start, progfs_end, progfs_oob, progfs_add_ecc;
char *datafs = NULL;
uint32_t datafs_start, datafs_end, datafs_oob, datafs_add_ecc;
char *usrfs = NULL;
uint32_t usrfs_start, usrfs_end, usrfs_oob, usrfs_add_ecc;
char *usrdisk = NULL;
uint32_t usrdisk_start, usrdisk_end, usrdisk_oob, usrdisk_add_ecc;

static void set_args(int argc, char **argv)
{
    int i;
    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--uboot") == 0)
        {
            if (++i < argc)
                uboot = argv[i];
            if (++i < argc)
                uboot_start = strtol(argv[i], NULL, 0);
            if (++i < argc)
                uboot_end = strtol(argv[i], NULL, 0);
            if (++i < argc)
                uboot_add_ecc = strtol(argv[i], NULL, 0);
        }
        else if (strcmp(argv[i], "--serial") == 0)
        {
            if (++i < argc)
                serial = argv[i];
            if (++i < argc)
                serial_start = strtol(argv[i], NULL, 0);
            if (++i < argc)
                serial_end = strtol(argv[i], NULL, 0);
            if (++i < argc)
                serial_add_ecc = strtol(argv[i], NULL, 0);
        }
        else if (strcmp(argv[i], "--uimage") == 0)
        {
            if (++i < argc)
                uimage = argv[i];
            if (++i < argc)
                uimage_start = strtol(argv[i], NULL, 0);
            if (++i < argc)
                uimage_end = strtol(argv[i], NULL, 0);
            if (++i < argc)
                uimage_add_ecc = strtol(argv[i], NULL, 0);
        }
        else if (strcmp(argv[i], "--rootfs") == 0)
        {
            if (++i < argc)
                rootfs = argv[i];
            if (++i < argc)
                rootfs_start = strtol(argv[i], NULL, 0);
            if (++i < argc)
                rootfs_end = strtol(argv[i], NULL, 0);
            if (++i < argc)
                rootfs_oob = strtol(argv[i], NULL, 0);
            if (++i < argc)
                rootfs_add_ecc = strtol(argv[i], NULL, 0);
        }
        else if (strcmp(argv[i], "--settings") == 0)
        {
            if (++i < argc)
                settings = argv[i];
            if (++i < argc)
                settings_start = strtol(argv[i], NULL, 0);
            if (++i < argc)
                settings_end = strtol(argv[i], NULL, 0);
            if (++i < argc)
                settings_oob = strtol(argv[i], NULL, 0);
            if (++i < argc)
                settings_add_ecc = strtol(argv[i], NULL, 0);
        }
        else if (strcmp(argv[i], "--progfs") == 0)
        {
            if (++i < argc)
                progfs = argv[i];
            if (++i < argc)
                progfs_start = strtol(argv[i], NULL, 0);
            if (++i < argc)
                progfs_end = strtol(argv[i], NULL, 0);
            if (++i < argc)
                progfs_oob = strtol(argv[i], NULL, 0);
            if (++i < argc)
                progfs_add_ecc = strtol(argv[i], NULL, 0);
        }
        else if (strcmp(argv[i], "--datafs") == 0)
        {
            if (++i < argc)
                datafs = argv[i];
            if (++i < argc)
                datafs_start = strtol(argv[i], NULL, 0);
            if (++i < argc)
                datafs_end = strtol(argv[i], NULL, 0);
            if (++i < argc)
                datafs_oob = strtol(argv[i], NULL, 0);
            if (++i < argc)
                datafs_add_ecc = strtol(argv[i], NULL, 0);
        }
        else if (strcmp(argv[i], "--usrfs") == 0)
        {
            if (++i < argc)
                usrfs = argv[i];
            if (++i < argc)
                usrfs_start = strtol(argv[i], NULL, 0);
            if (++i < argc)
                usrfs_end = strtol(argv[i], NULL, 0);
            if (++i < argc)
                usrfs_oob = strtol(argv[i], NULL, 0);
            if (++i < argc)
                usrfs_add_ecc = strtol(argv[i], NULL, 0);
        }
        else if (strcmp(argv[i], "--usrdisk") == 0)
        {
            if (++i < argc)
                usrdisk = argv[i];
            if (++i < argc)
                usrdisk_start = strtol(argv[i], NULL, 0);
            if (++i < argc)
                usrdisk_end = strtol(argv[i], NULL, 0);
            if (++i < argc)
                usrdisk_oob = strtol(argv[i], NULL, 0);
            if (++i < argc)
                usrdisk_add_ecc = strtol(argv[i], NULL, 0);
        }
        else
        {
            flash_image = argv[i];
        }
    }
}

static int valid_conf_item(const char *filename, uint32_t start, uint32_t end)
{
    int fd = open(filename, O_RDWR);
    if (fd < 0)
    {
        fprintf(stderr, "Unable to open file: %s\n", filename);
        return (-1);
    }
    if (start % NAND_PAGE_SIZE)
    {
        fprintf(stderr, "Start address 0x%X, is not multiple of page size\n", start);
        return (-1);
    }
    if (end % NAND_PAGE_SIZE)
    {
        fprintf(stderr, "End address 0x%X, is not multiple of page size\n", end);
        return (-1);
    }
    close(fd);
    return 0;
}

static int valid_conf()
{
    if (!flash_image)
    {
        fprintf(stderr, "Missing output name\n");
        return (-1);
    }
    printf("output: \t%s\n", flash_image);

    if (uboot)
    {
        printf("uboot = \t%s\n", uboot);
        printf("uboot_start = \t0x%X\n", uboot_start);
        printf("uboot_end = \t0x%X\n", uboot_end);
        printf("uboot_add_ecc = \t%d\n", uboot_add_ecc);
        if (valid_conf_item(uboot, uboot_start, uboot_end))
            return -1;
    }

    if (serial)
    {
        printf("serial = \t%s\n", serial);
        printf("serial_start = \t0x%X\n", serial_start);
        printf("serial_end = \t0x%X\n", serial_end);
        printf("serial_add_ecc = \t%d\n", serial_add_ecc);
        if (valid_conf_item(serial, serial_start, serial_end))
            return -1;
    }
    if (uimage)
    {
        printf("uimage = \t%s\n", uimage);
        printf("uimage_start = \t0x%X\n", uimage_start);
        printf("uimage_end = \t0x%X\n", uimage_end);
        printf("uimage_add_ecc = \t%d\n", uimage_add_ecc);
        if (valid_conf_item(uimage, uimage_start, uimage_end))
            return -1;
    }

    if (rootfs)
    {
        printf("rootfs = \t%s\n", rootfs);
        printf("rootfs_start = \t0x%X\n", rootfs_start);
        printf("rootfs_end = \t0x%X\n", rootfs_end);
        printf("rootfs_obb = \t%d\n", rootfs_oob);
        printf("rootfs_add_ecc = \t%d\n", rootfs_add_ecc);
        if (valid_conf_item(rootfs, rootfs_start, rootfs_end))
            return -1;
    }

    if (settings)
    {
        printf("settings = \t%s\n", settings);
        printf("settings_start = \t0x%X\n", settings_start);
        printf("settings_end = \t0x%X\n", settings_end);
        printf("settings_obb = \t%d\n", settings_oob);
        printf("settings_add_ecc = \t%d\n", settings_add_ecc);
        if (valid_conf_item(settings, settings_start, settings_end))
            return -1;
    }

    if (progfs)
    {
        printf("progfs = \t%s\n", progfs);
        printf("progfs_start = \t0x%X\n", progfs_start);
        printf("progfs_end = \t0x%X\n", progfs_end);
        printf("progfs_obb = \t%d\n", progfs_oob);
        printf("progfs_add_ecc = \t%d\n", progfs_add_ecc);
        if (valid_conf_item(progfs, progfs_start, progfs_end))
            return -1;
    }

    if (datafs)
    {
        printf("datafs = \t%s\n", datafs);
        printf("datafs_start = \t0x%X\n", datafs_start);
        printf("datafs_end = \t0x%X\n", datafs_end);
        printf("datafs_obb = \t%d\n", datafs_oob);
        printf("datafs_add_ecc = \t%d\n", datafs_add_ecc);
        if (valid_conf_item(datafs, datafs_start, datafs_end))
            return -1;
    }

    if (usrfs)
    {
        printf("usrfs = \t%s\n", usrfs);
        printf("usrfs_start = \t0x%X\n", usrfs_start);
        printf("usrfs_end = \t0x%X\n", usrfs_end);
        printf("usrfs_obb = \t%d\n", usrfs_oob);
        printf("usrfs_add_ecc = \t%d\n", usrfs_add_ecc);
        if (valid_conf_item(usrfs, usrfs_start, usrfs_end))
            return -1;
    }

    if (usrdisk)
    {
        printf("usrdisk = \t%s\n", usrdisk);
        printf("usrdisk_start = \t0x%X\n", usrdisk_start);
        printf("usrdisk_end = \t0x%X\n", usrdisk_end);
        printf("usrdisk_obb = \t%d\n", usrdisk_oob);
        printf("usrdisk_add_ecc = \t%d\n", usrdisk_add_ecc);
        if (valid_conf_item(usrdisk, usrdisk_start, usrdisk_end))
            return -1;
    }

    printf("\n");

    return 0;
}

static int create_nandflash()
{
    int fd;
    uint32_t i;
    char buf[NAND_PAGE_SIZE + NAND_OOB_SIZE];
    fd = open(flash_image, O_RDWR);
    if (fd < 0)
    {
        printf("Nand flash image %s does not exit. create it!\n", flash_image);
        fd = open(flash_image, O_RDWR | O_CREAT);
        if (fd < 0)
        {
            printf("Can not create nand flash image %s!\n", flash_image);
            return (-1);
        }
        /*write the empty nand flash image(all 0xff) */
        memset(buf, 0xff, sizeof(buf));
        for (i = 0; i < NAND_PAGES; i++)
            write(fd, buf, sizeof(buf));
        printf("Create nand flash image %s successfully!\n", flash_image);
    }
    return fd;
}

static void put_to_flash(int nand_flash_fd, const char *image, int image_start, int image_end, int with_oob, int add_ecc)
{
    unsigned char data[NAND_PAGE_SIZE];
    unsigned char oob[NAND_OOB_SIZE];

    int start_page, end_page, i;
    uint32_t is_continue = 1;

    start_page = image_start / NAND_PAGE_SIZE;
    end_page = image_end / NAND_PAGE_SIZE;

    int image_fd = open(image, O_RDWR);

    lseek(nand_flash_fd, start_page * (NAND_PAGE_SIZE + NAND_OOB_SIZE),
          SEEK_SET);
    lseek(image_fd, 0, SEEK_SET);
    memset(data, 0xff, NAND_PAGE_SIZE);
    memset(oob, 0xff, NAND_OOB_SIZE);
    for (i = start_page; i < end_page; i++)
    {
        if (read(image_fd, data, NAND_PAGE_SIZE) < NAND_PAGE_SIZE)
            is_continue = 0;
        if (with_oob)
        {
            /*image has oob */
            if (read(image_fd, oob, NAND_OOB_SIZE) < NAND_OOB_SIZE)
                is_continue = 0;
        }
        else
        {
            /*if (add_ecc)
            	for (j = 0; j < NAND_PAGE_SIZE / 256; j++)
                nand_calculate_ecc(data + j * 256,
                                   oob + NAND_ECC_OFFSET + j * 3);*/
            /*TODO:ECC*/
        }
        write(nand_flash_fd, data, NAND_PAGE_SIZE);
        write(nand_flash_fd, oob, NAND_OOB_SIZE);
        if (!is_continue)
            break;
    }

    close(image_fd);
}

int main(int argc, char **argv)
{
    set_args(argc, argv);

    if (valid_conf() < 0)
        exit(-1);

    int nand_flash_fd = create_nandflash();
    if (nand_flash_fd < 0)
        exit(-1);
    if (uboot)
        put_to_flash(nand_flash_fd, uboot, uboot_start, uboot_end, 0, uboot_add_ecc);
    if (serial)
        put_to_flash(nand_flash_fd, serial, serial_start, serial_end, 0, serial_add_ecc);
    if (uimage)
        put_to_flash(nand_flash_fd, uimage, uimage_start, uimage_end, 0, uimage_add_ecc);
    if (rootfs)
        put_to_flash(nand_flash_fd, rootfs, rootfs_start, rootfs_end, rootfs_oob, rootfs_add_ecc);
    if (settings)
        put_to_flash(nand_flash_fd, settings, settings_start, settings_end, settings_oob, settings_add_ecc);
    if (progfs)
        put_to_flash(nand_flash_fd, progfs, progfs_start, progfs_end, progfs_oob, progfs_add_ecc);
    if (datafs)
        put_to_flash(nand_flash_fd, datafs, datafs_start, datafs_end, datafs_oob, datafs_add_ecc);
    if (usrfs)
        put_to_flash(nand_flash_fd, usrfs, usrfs_start, usrfs_end, usrfs_oob, usrfs_add_ecc);
    if (usrdisk)
        put_to_flash(nand_flash_fd, usrdisk, usrdisk_start, usrdisk_end, usrdisk_oob, usrdisk_add_ecc);
    close(nand_flash_fd);

    return (0);
}
