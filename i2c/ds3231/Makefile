# ----------------------------------------------------------------------------------
#
#   Author: Abhijeet Badurkar 
#   Description: Makefile to compile loadable kernal module for rtc ds3231 of Maxim
# 
# ----------------------------------------------------------------------------------
#   Copyright (C) 2020 Abhijeet Badurkar
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#   MA 02110-1301 USA.
# ----------------------------------------------------------------------------------

obj-m := ds3231.o
EXTRA_CFLAGS := -std=gnu99 -Wno-declaration-after-statement
KERNELDIR := /lib/modules/$(shell uname -r)/build

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

