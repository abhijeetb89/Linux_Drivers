/* ------------------------------------------------------------------------- */
/*									     */
/*  ds3231.c: loadable kernal module for rtc ds3231 of Maxim		     */
/*  Author: Abhijeet Badurkar									     */
/* ------------------------------------------------------------------------- */
/*  Copyright (C) 2020 Abhijeet Badurkar

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
    MA 02110-1301 USA.							     */
/* ------------------------------------------------------------------------- */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/of_device.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/rtc.h>
#include <linux/regmap.h>
#include <linux/bcd.h>
#include <linux/err.h>


#define    DS3231_SECONDS_REG_ADDR    0x00
#define    DS3231_MINUTES_REG_ADDR    0x01
#define    DS3231_HOURS_REG_ADDR      0x02
#define    DS3231_DAY_REG_ADDR        0x03
#define    DS3231_DATE_REG_ADDR       0x04
#define    DS3231_MONTH_REG_ADDR      0x05
#define    DS3231_YEAR_REG_ADDR       0x06
#define    DS3231_CONTROL_REG_ADDR    0x0E

const u8 control_reg_default_value =  0x1C;

static int ds3231_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int ds3231_remove(struct i2c_client *client);
static int ds3231_detect(struct i2c_client *client, struct i2c_board_info *info);

struct ds3231_data {
    struct i2c_client *client;
    struct device *dev;
    struct rtc_device *rtc;
    struct regmap *regmap;
};

static int ds3231_get_time(struct device *dev, struct rtc_time *time)
{
	int ret;
	u8 reg[7];
	struct i2c_client *client = to_i2c_client(dev);
	struct ds3231_data *data = (struct ds3231_data*) i2c_get_clientdata(client);

	ret = regmap_bulk_read(data->regmap, DS3231_SECONDS_REG_ADDR, reg, sizeof(reg));
	if (ret) {
		dev_info(data->dev, "%s: Read failed", __func__);
		return ret;
	}

	time->tm_sec = bcd2bin(reg[0]);
	time->tm_min = bcd2bin(reg[1]);
	time->tm_hour = bcd2bin(reg[2]);
	time->tm_wday = reg[3];
	time->tm_mday = bcd2bin(reg[4]);
	time->tm_mon = bcd2bin(reg[5] & 0x7F);
	time->tm_year = bcd2bin(reg[6]);

	dev_info(dev, "%s secs=%d, mins=%d, "
		"hours=%d, mday=%d, mon=%d, year=%d, wday=%d\n",
		"read", time->tm_sec, time->tm_min,
		time->tm_hour, time->tm_mday,
		time->tm_mon, time->tm_year, time->tm_wday);
	return 0;
}

static int ds3231_set_time(struct device *dev, struct rtc_time *time)
{
	int ret;
	u8 reg[7];
	struct i2c_client *client = to_i2c_client(dev);
	struct ds3231_data *data = (struct ds3231_data*) i2c_get_clientdata(client);

	reg[0] = bin2bcd(time->tm_sec);
	reg[1] = bin2bcd(time->tm_min);
	reg[2] = bin2bcd(time->tm_hour);
	reg[3] = time->tm_wday;
	reg[4] = bin2bcd(time->tm_mday);
	reg[5] = bin2bcd(time->tm_mon);
	reg[6] = bin2bcd(time->tm_year);

	ret = regmap_bulk_write(data->regmap, DS3231_CONTROL_REG_ADDR, reg, sizeof(reg));

	return ret;
}

static const struct rtc_class_ops ds3231_rtc_ops = {
	.read_time	= ds3231_get_time,
	.set_time	= ds3231_set_time,
};

static struct regmap_config ds3231_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
	.max_register = DS3231_CONTROL_REG_ADDR,
};

static int ds3231_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret = 0;
	u32 val;
	struct ds3231_data *data;

	printk(KERN_INFO "ds3231_probe: In probe");

	data = devm_kzalloc(&client->dev, sizeof(struct ds3231_data), GFP_KERNEL);
	if (NULL == data) {
		return -ENOMEM;
	}

	data->client = client;
	data->dev = &client->dev;
	data->regmap =  devm_regmap_init_i2c(client, &ds3231_regmap_config);
	if (IS_ERR(data->regmap)) {
		dev_err(data->dev, "failed to allocate register map\n");
		return PTR_ERR(data->regmap);
	}
	else
		dev_info(data->dev, "%s: Allocated regitser map", __func__);

	i2c_set_clientdata(client, data);

	data->rtc = devm_rtc_allocate_device(data->dev);
	if (IS_ERR(data->rtc))
		return PTR_ERR(data->rtc);

	data->rtc->ops = &ds3231_rtc_ops;

	dev_info(data->dev, "%s: Allocated regitser device", __func__);
	ret = rtc_register_device(data->rtc);
	if (ret)
		return ret;

	if (ret == 0) {
		ret = regmap_read(data->regmap, DS3231_CONTROL_REG_ADDR, &val);
		if (ret == 0) {
			dev_info(data->dev, "%s read:%d: value:%d\n",
				__func__, DS3231_CONTROL_REG_ADDR, val);
		}
		else
			dev_info(data->dev, "%s: Could not read control register: %d",
				__func__, ret);

	}

	return ret;
}

static int ds3231_remove(struct i2c_client *client)
{
	printk(KERN_INFO "ds3231_remove: In remove");

	void *ptr = NULL;
	if (NULL != (ptr = i2c_get_clientdata(client))) {
	kfree(ptr);
	}

	return 0;
}

static int ds3231_detect(struct i2c_client *client, struct i2c_board_info *info)
{
	printk(KERN_INFO "ds3231_detect: In detection");
	return 0;
}

static unsigned short addressList[] = { 0x68 };

static struct i2c_device_id i2c_idtable[] = {
      { "ds3231", 0 },
      { }
};

#ifdef CONFIG_OF
static const struct of_device_id ds3231_of_match = {
	.compatible = "dallas,ds3231"
};
#endif

MODULE_DEVICE_TABLE(i2c, i2c_idtable);

static struct i2c_driver ds3231_driver = {
	.driver = {
	.name       = "ds3231",
	.owner	    = THIS_MODULE,
	.of_match_table = of_match_ptr(&ds3231_of_match),
	},

	.id_table       = i2c_idtable,
	.probe          = ds3231_probe,
	.remove         = ds3231_remove,
};


/* __init and __exit */
module_i2c_driver(ds3231_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abhijeet Badurkar <abhijeetb89@gmail.com>");
MODULE_DESCRIPTION("ds3231 i2c Driver");

