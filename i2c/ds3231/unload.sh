# ------------------------------------------------------------------------------------
#
#   Author: Abhijeet Badurkar 
#   Description: Bash script to unload loadable kernal module for rtc ds3231 of Maxim.
#   (Note: Written and tested on Raspberry Pi 2)
# ------------------------------------------------------------------------------------
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
# ------------------------------------------------------------------------------------

#!/bin/sh
echo 0x68 | sudo tee /sys/class/i2c-adapter/i2c-1/delete_device
sudo rmmod ds3231

