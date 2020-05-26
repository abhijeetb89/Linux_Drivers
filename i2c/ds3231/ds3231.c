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
#include <linux/slab.h>
#include <linux/i2c.h>
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

static int ds3231_read(struct i2c_client *client, u8 reg, u8 *value);
static int ds3231_write(struct i2c_client *client, u8 reg, u8 value);

static unsigned short addressList[] = { 0x68 };

static struct i2c_device_id i2c_idtable[] = {
      { "ds3231", 0 },
      { }
};

MODULE_DEVICE_TABLE(i2c, i2c_idtable);

static struct i2c_driver ds3231_driver = {
    .driver = {
        .name       = "ds3231",
        //.pm       = &foo_pm_ops,  /* optional */
    },

    .id_table       = i2c_idtable,
    .probe          = ds3231_probe,
    .remove         = ds3231_remove,
    /* if device autodetection is needed: */
    .class          = I2C_CLASS_HWMON,
    .detect         = ds3231_detect,
    .address_list   = addressList,

    //.shutdown       = ds3231_shutdown, /* optional */
    //.command        = ds3231_command,  /* optional, deprecated */
};

struct ds3231_data {
    struct i2c_client *client;
    u8 seconds;
    u8 minutes;
    u8 hours;
    u8 day;
    u8 date;
    u8 month;
    u8 year;
};

static ssize_t seconds_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    struct i2c_client *client = to_i2c_client(dev);
    struct ds3231_data *data = (struct ds3231_data*) i2c_get_clientdata(client);
    if (0 < ds3231_read(client, DS3231_SECONDS_REG_ADDR, &data->seconds)) {
        int ret = sprintf(buf, "%x\n", data->seconds);
	return ret;
    }

    printk(KERN_ERR "ds3231: seconds_show: error occurred");
    return sprintf(buf, "%d\n", 0);
}

static ssize_t seconds_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    struct i2c_client *client = to_i2c_client(dev);
    u8 value = simple_strtol(buf, NULL, 10);

    if (value > 59)
        value = 0;

    if (ds3231_write(client, DS3231_SECONDS_REG_ADDR, value) <= 0)
        return 0;

    return strlen(buf);
}

static ssize_t minutes_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    struct i2c_client *client = to_i2c_client(dev);
    struct ds3231_data *data = (struct ds3231_data*) i2c_get_clientdata(client);
    if (0 < ds3231_read(client, DS3231_MINUTES_REG_ADDR, &data->minutes)) {
        int ret = sprintf(buf, "%x\n", data->minutes);
	return ret;
    }

    printk(KERN_ERR "ds3231: minutes_show: error occurred");
    return sprintf(buf, "%d\n", 0);
}

static ssize_t minutes_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    struct i2c_client *client = to_i2c_client(dev);
    u8 value = simple_strtol(buf, NULL, 10);

    if (value > 59)
        value = 0;

    if (ds3231_write(client, DS3231_MINUTES_REG_ADDR, value) <= 0)
        return 0;

    return strlen(buf);
}

static ssize_t hours_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    struct i2c_client *client = to_i2c_client(dev);
    struct ds3231_data *data = (struct ds3231_data*) i2c_get_clientdata(client);
    if (0 < ds3231_read(client, DS3231_HOURS_REG_ADDR, &data->hours)) {
        int ret = sprintf(buf, "%x\n", data->hours);
	return ret;
    }

    printk(KERN_ERR "ds3231: hours_show: error occurred");
    return sprintf(buf, "%d\n", 0);
}

static ssize_t hours_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    struct i2c_client *client = to_i2c_client(dev);
    u8 value = simple_strtol(buf, NULL, 10);

    if (value > 12)
        value = 0;

    if (ds3231_write(client, DS3231_HOURS_REG_ADDR, value) <= 0)
        return 0;

    return strlen(buf);
}

static ssize_t day_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    struct i2c_client *client = to_i2c_client(dev);
    struct ds3231_data *data = (struct ds3231_data*) i2c_get_clientdata(client);
    if (0 < ds3231_read(client, DS3231_DAY_REG_ADDR, &data->day)) {
        int ret = sprintf(buf, "%x\n", data->day);
	return ret;
    }

    printk(KERN_ERR "ds3231: day_show: error occurred");
    return sprintf(buf, "%d\n", 0);
}

static ssize_t day_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    struct i2c_client *client = to_i2c_client(dev);
    u8 value = simple_strtol(buf, NULL, 10);

    if (value > 7)
        value = 0;

    if (ds3231_write(client, DS3231_DAY_REG_ADDR, value) <= 0)
        return 0;

    return strlen(buf);
}

static ssize_t date_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    struct i2c_client *client = to_i2c_client(dev);
    struct ds3231_data *data = (struct ds3231_data*) i2c_get_clientdata(client);
    if (0 < ds3231_read(client, DS3231_DATE_REG_ADDR, &data->date)) {
        int ret = sprintf(buf, "%x\n", data->date);
	return ret;
    }

    printk(KERN_ERR "ds3231: day_show: error occurred");
    return sprintf(buf, "%d\n", 0);
}

static ssize_t date_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    struct i2c_client *client = to_i2c_client(dev);
    u8 value = simple_strtol(buf, NULL, 10);

    struct ds3231_data *data = (struct ds3231_data*) i2c_get_clientdata(client);  

    if ((value > 30 && (data->month == 4 || data->month == 6 || data->month == 9 || data->month == 11)) ||
        (value > 28 && data->month == 2) ||
        (value > 31)
       )
        value = 1;

    if (ds3231_write(client, DS3231_DATE_REG_ADDR, value) <= 0)
        return 0;

    return strlen(buf);
}

static ssize_t month_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    struct i2c_client *client = to_i2c_client(dev);
    struct ds3231_data *data = (struct ds3231_data*) i2c_get_clientdata(client);
    if (0 < ds3231_read(client, DS3231_MONTH_REG_ADDR, &data->month)) {
        int ret = sprintf(buf, "%x\n", data->month);
	return ret;
    }

    printk(KERN_ERR "ds3231: month_show: error occurred");
    return sprintf(buf, "%d\n", 0);
}

static ssize_t month_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    struct i2c_client *client = to_i2c_client(dev);
    u8 value = simple_strtol(buf, NULL, 10);

    if (value > 12)
        value = 1;

    if (ds3231_write(client, DS3231_MONTH_REG_ADDR, value) <= 0)
        return 0;

    return strlen(buf);
}

static ssize_t year_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    struct i2c_client *client = to_i2c_client(dev);
    struct ds3231_data *data = (struct ds3231_data*) i2c_get_clientdata(client);
    if (0 < ds3231_read(client, DS3231_YEAR_REG_ADDR, &data->year)) {
        int ret = sprintf(buf, "%x\n", data->year);
	return ret;
    }

    printk(KERN_ERR "ds3231: year_show: error occurred");
    return sprintf(buf, "%d\n", 0);
}

static ssize_t year_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    struct i2c_client *client = to_i2c_client(dev);
    u16 value = simple_strtol(buf, NULL, 10);

    if (value > 9999)
        value = 9999;

    u8 year = value % 100;

    if (ds3231_write(client, DS3231_YEAR_REG_ADDR, year) <= 0)
        return 0;

    return strlen(buf);
}

static DEVICE_ATTR_RW(seconds);
static DEVICE_ATTR_RW(minutes);
static DEVICE_ATTR_RW(hours);
static DEVICE_ATTR_RW(day);
static DEVICE_ATTR_RW(date);
static DEVICE_ATTR_RW(month);
static DEVICE_ATTR_RW(year);


static struct attribute *ds3231_attrs[] = {
    &dev_attr_seconds.attr,
    &dev_attr_minutes.attr,
    &dev_attr_hours.attr,
    &dev_attr_day.attr,
    &dev_attr_date.attr,
    &dev_attr_month.attr,
    &dev_attr_year.attr,
    NULL,
};

static struct attribute_group ds3231_attr_group = {
    .attrs = ds3231_attrs,
};

static int ds3231_read(struct i2c_client *client, u8 reg, u8 *value)
{
    struct i2c_msg msg_reg_ptr_wr = {
        .addr = client->addr,
        .len = 1,
        .buf = &reg,
    };

    struct i2c_msg msg_reg_read = {
        .addr = client->addr,
        .flags = I2C_M_RD,
        .len = 1,
        .buf = value,
    };

    struct i2c_msg msgArr[] = { msg_reg_ptr_wr, msg_reg_read };

    int ret = 0;
    if (0 > (ret = i2c_transfer(client->adapter, msgArr, 2))) {
        printk(KERN_ERR "ds3231_read: read failed!");
    }

    printk("ds3231_read: register %x = %x", reg, *value);

    return ret;
}

static int ds3231_write(struct i2c_client *client, u8 reg, u8 value)
{
   value = ((value / 10) << 4) | (value % 10);

   printk("ds3231_write: writing register %x with value %x", reg, value);

   u8 payload[] = {reg, value};

   struct i2c_msg msg_reg = {
        .addr = client->addr,
        .len = 2,
        .buf = payload,
    };

    int ret = 0;
    if (0 > (ret = i2c_transfer(client->adapter, &msg_reg, 1))) {
        printk(KERN_ERR "ds3231_write: write failed!");
    }

    return ret;
}

static int ds3231_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int ret = 0;
    printk(KERN_INFO "ds3231_probe: In probe");

    struct ds3231_data *data;

    data = (struct ds3231_data*) kzalloc(sizeof(struct ds3231_data), GFP_KERNEL);

    if (NULL == data) {
        return -ENOMEM;
    }

    data->client = client;
    i2c_set_clientdata(client, data);

    u8 value = control_reg_default_value;
    /* Configure registers */
    if ( 0 > (ret = ds3231_write(client, DS3231_CONTROL_REG_ADDR, value))) {
        printk(KERN_INFO "Could not read control register");
	value = 0;
    }
   
    u8 reg = 0; 
    if ((0 < ret) && (0 > ds3231_read(client, DS3231_CONTROL_REG_ADDR, &reg))) {
        printk(KERN_INFO "Could not read control register");
    }

    /* Create attributes for sysfs */
    if (sysfs_create_group(&client->dev.kobj, &ds3231_attr_group)) {
        printk(KERN_INFO "Could not create sysfs group for DS3231");
    }

    return 0;
}

static int ds3231_remove(struct i2c_client *client)
{
    printk(KERN_INFO "ds3231_remove: In remove");

    sysfs_remove_group(&client->dev.kobj, &ds3231_attr_group);

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

/* __init and __exit */
module_i2c_driver(ds3231_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Abhijeet Badurkar <abhijeetb89@gmail.com>");
MODULE_DESCRIPTION("ds3231 i2c Driver");

