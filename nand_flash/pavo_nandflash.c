/*
 * create nand flash image for pavo board
 *
 * Copyright (C) 2008 yajin <yajin@vm-kernel.org>
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


 
#include <confuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "types.h"
#include "nand_flash.h"

char *flash_image = NULL;

char *uboot_image = NULL;
uint32_t uboot_start, uboot_end,uboot_add_ecc;
char *kernel_image = NULL;
uint32_t kernel_start, kernel_end,kernel_add_ecc;
char *rootfs_image = NULL;
uint32_t rootfs_start, rootfs_end, rootfs_oob,rootfs_add_ecc;

static int parse_configure(char *file_name)
{
    cfg_opt_t opts[] = {
        CFG_SIMPLE_STR("flash_image", &flash_image),

        CFG_SIMPLE_STR("uboot_image", &uboot_image),
        CFG_SIMPLE_INT("uboot_start", &uboot_start),
        CFG_SIMPLE_INT("uboot_end", &uboot_end),
        CFG_SIMPLE_INT("uboot_add_ecc", &uboot_add_ecc),

        CFG_SIMPLE_STR("kernel_image", &kernel_image),
        CFG_SIMPLE_INT("kernel_start", &kernel_start),
        CFG_SIMPLE_INT("kernel_end", &kernel_end),
        CFG_SIMPLE_INT("kernel_add_ecc", &kernel_add_ecc),

        CFG_SIMPLE_STR("rootfs_image", &rootfs_image),
        CFG_SIMPLE_INT("rootfs_start", &rootfs_start),
        CFG_SIMPLE_INT("rootfs_end", &rootfs_end),
        CFG_SIMPLE_INT("rootfs_oob", &rootfs_oob),
        CFG_SIMPLE_INT("rootfs_add_ecc", &rootfs_add_ecc),

        CFG_END()
    };
    cfg_t *cfg;

    cfg = cfg_init(opts, 0);
    if (cfg_parse(cfg, file_name) == CFG_PARSE_ERROR)
        return (-1);
    cfg_free(cfg);
    return (0);
}




static int valid_conf()
{
    int fd;

    if (!flash_image)
    {
        printf("flash_image is null \n");
        return (-1);
    }


    if (uboot_image)
    {
        fd = open(uboot_image, O_RDWR);
        if (fd < 0)
        {
            printf("Can not open uboot image  %s \n", uboot_image);
            return (-1);
        }
        if (uboot_start % PAVO_NAND_PAGE_SIZE)
        {
            printf
                ("uboot image start address 0x%x is not multiple of page size \n",
                 uboot_start);
            return (-1);
        }
        if (uboot_end % PAVO_NAND_PAGE_SIZE)
        {
            printf
                ("uboot image end address 0x%x is not multiple of page size \n",
                 uboot_end);
            return (-1);
        }

        close(fd);
    }

    if (kernel_image)
    {
        fd = open(kernel_image, O_RDWR);
        if (fd < 0)
        {
            printf("Can not open kernel image  %s \n", kernel_image);
            return (-1);
        }
        if (kernel_start % PAVO_NAND_PAGE_SIZE)
        {
            printf
                ("(kernel image start address 0x%x is not multiple of page size \n",
                 kernel_start);
            return (-1);
        }
        if (kernel_end % PAVO_NAND_PAGE_SIZE)
        {
            printf
                ("kernel image end address 0x%x is not multiple of page size \n",
                 kernel_end);
            return (-1);
        }

        close(fd);
    }

    if (rootfs_image)
    {
        fd = open(rootfs_image, O_RDWR);
        if (fd < 0)
        {
            printf("Can not open rootfs image  %s \n", rootfs_image);
            return (-1);
        }
        if (rootfs_start % PAVO_NAND_PAGE_SIZE)
        {
            printf
                ("(rootfs image start address 0x%x is not multiple of page size \n",
                 rootfs_start);
            return (-1);
        }
        if (rootfs_end % PAVO_NAND_PAGE_SIZE)
        {
            printf
                ("(rootfs image end address 0x%x is not multiple of page size \n",
                 rootfs_end);
            return (-1);
        }

        close(fd);
    }

    return (0);
}


static int create_pavo_nandflash()
{
    int fd;
    uint32_t i;
    char buf[PAVO_NAND_PAGE_SIZE + PAVO_NAND_OOB_SIZE];
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
        for (i = 0; i < PAVO_NAND_PAGES; i++)
            write(fd, buf, sizeof(buf));
        printf("Create nand flash image %s successfully!\n", flash_image);
    }
    return fd;
}

static void put_to_flash(int nand_flash_fd, int image_fd, int image_start,
                         int image_end, int with_oob,int add_ecc)
{
    unsigned char data[PAVO_NAND_PAGE_SIZE];
    unsigned char oob[PAVO_NAND_OOB_SIZE];

    int start_page, end_page, i;
    uint32_t is_continue=1;

    start_page = image_start / PAVO_NAND_PAGE_SIZE;
    end_page = image_end / PAVO_NAND_PAGE_SIZE;

    lseek(nand_flash_fd, start_page * (PAVO_NAND_PAGE_SIZE + PAVO_NAND_OOB_SIZE),
          SEEK_SET);
    lseek(image_fd, 0, SEEK_SET);
    memset(data, 0xff, PAVO_NAND_PAGE_SIZE);
    memset(oob, 0xff, PAVO_NAND_OOB_SIZE);
    for (i = start_page; i < end_page; i++)
    {
        if (read(image_fd, data, PAVO_NAND_PAGE_SIZE)< PAVO_NAND_PAGE_SIZE)
            is_continue=0;
        if (with_oob)
        {
            /*image has oob */
            if (read(image_fd, oob, PAVO_NAND_OOB_SIZE)<PAVO_NAND_OOB_SIZE)
            	is_continue=0;
        }
        else
        {
        	 /*if (add_ecc)
            	for (j = 0; j < PAVO_NAND_PAGE_SIZE / 256; j++)
                nand_calculate_ecc(data + j * 256,
                                   oob + PAVO_NAND_ECC_OFFSET + j * 3);*/
           /*TODO:ECC*/
        }
        write(nand_flash_fd, data, PAVO_NAND_PAGE_SIZE);
        write(nand_flash_fd, oob, PAVO_NAND_OOB_SIZE);
        if (!is_continue)
        	break;
    }
}


static void put_uboot(int nand_flash_fd)
{
    int fd;
    fd = open(uboot_image, O_RDWR);
    put_to_flash(nand_flash_fd, fd, uboot_start, uboot_end, 0,uboot_add_ecc);
    printf("put uboot image %s to nand flash image %s successfully!\n",
           uboot_image, flash_image);
    close(fd);
}

static void put_kernel(int nand_flash_fd)
{
    int fd;
    fd = open(kernel_image, O_RDWR);
    put_to_flash(nand_flash_fd, fd, kernel_start, kernel_end, 0,kernel_add_ecc);
    printf("put kernel image %s to nand flash image %s successfully!\n",
           kernel_image, flash_image);
    close(fd);
}

static void put_rootfs(int nand_flash_fd)
{
    int fd;
    fd = open(rootfs_image, O_RDWR);
    put_to_flash(nand_flash_fd, fd, rootfs_start, rootfs_end, rootfs_oob,rootfs_add_ecc);
    printf("put rootfs image %s to nand flash image %s successfully!\n",
           rootfs_image, flash_image);
    close(fd);
}

int main(int argc, char **argv)
{
    int nand_flash_fd;
    if (parse_configure("pavo_nandflash.conf") < 0)
    {
        printf("error when parsing configuration file.\n");
        exit(-1);
    }

    if (valid_conf() < 0)
        exit(-1);
    nand_flash_fd = create_pavo_nandflash();
    if (nand_flash_fd < 0)
        exit(-1);
    if (uboot_image)
        put_uboot(nand_flash_fd);
    if (kernel_image)
        put_kernel(nand_flash_fd);
    if (rootfs_image)
        put_rootfs(nand_flash_fd);

    close(nand_flash_fd);

    return (0);
}
