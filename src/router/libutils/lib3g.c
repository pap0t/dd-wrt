/*
 * lib3g.c
 *
 * Copyright (C) 2009 Sebastian Gottschall <gottschall@dd-wrt.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Id:
 * this module detects various special 3G/CDMA USB Devices which is required to handle them correct, since some devices
 * are using non standard tty interfaces. everything which is not handled here in this list, is supported by the default tty handling which means dialin on tty0
 */

#include <stdio.h>
#include <stdlib.h>
#include <shutils.h>
#include <bcmnvram.h>

static int scanFor(int Vendor, int Product)
{
#if defined(ARCH_broadcom) && !defined(HAVE_BCMMODERN)
	char grepstr[128];
	sprintf(grepstr, "grep Vendor=%x ProdID=%x /tmp/usb/devices|wc -l",
		Vendor, Product);
	FILE *check = popen(grepstr, "rb");
	if (check) {
		int count = 0;
		fscanf(check, "%d", &count);
		fclose(check);
		if (count > 0) {
			eval("umount /tmp/usb");
			return 1;
		}
	}
	return 0;
#else
	int count = 1;
	int hub = 1;
	while (hub < 5) {
		char sysfs[64];
		sprintf(sysfs, "/sys/bus/usb/devices/%d-0:1.0/bInterfaceNumber",
			count);
		FILE *probe = fopen(sysfs, "rb");
		if (!probe) {
			count = 1;
			hub++;
			continue;
		}
		fclose(probe);

		sprintf(sysfs, "/sys/bus/usb/devices/%d-%d/idProduct", count,
			hub);
		FILE *modem = fopen(sysfs, "rb");
		if (!modem) {
			count++;
			continue;
		}
		int idProduct;
		int idVendor;
		fscanf(modem, "%X", &idProduct);
		fclose(modem);
		sprintf(sysfs, "/sys/bus/usb/devices/%d-%d/idVendor", count,
			hub);
		modem = fopen(sysfs, "rb");
		if (!modem) {
			count++;
			continue;
		}
		fscanf(modem, "%X", &idVendor);
		fclose(modem);
		if (idVendor == Vendor && idProduct == Product)
			return 1;

		count++;
	}
	return 0;
#endif
}

void checkreset(char *tty)
{
#ifdef HAVE_CAMBRIA
	eval("comgt", "-d", tty, "-s", "/etc/comgt/reset.comgt");
	FILE *check = NULL;
	int count = 0;
	sleep(1);
	while (!(check = fopen(tty, "rb")) && count < 10) {
		sleep(1);
		count++;
	}
	if (check)
		fclose(check);
	else
		fprintf(stderr, "reset error\n");
	fprintf(stderr, "wakeup card\n");
	eval("comgt", "-d", tty, "-s", "/etc/comgt/wakeup.comgt");
	sleep(5);		//give extra delay for registering
#endif
}

static void reset_mc(int needreset, char *controldev)
{
	if (needreset)
		checkreset(controldev);
}

static void modeswitch_e1550(int needreset, char *controldev)
{
	system
	    ("usb_modeswitch -v 0x12d1 -p 0x1446 -m 1 55534243000000000000000000000011060000000000000000000000000000");
	sleep(2);
}

static void modeswitch_usb760(int needreset, char *controldev)
{
	system
	    ("usb_modeswitch -v 0x1410 -p 0x5030 -m 1 5553424312345678000000000000061b000000020000000000000000000000");
	sleep(2);
}

static void modeswitch_icon210(int needreset, char *controldev)
{
	FILE *out = fopen("/tmp/usb_modeswitch.conf", "wb");
	fprintf(out, "DefaultVendor=0x1e0e\n");
	fprintf(out, "DefaultProduct=0xf000\n");
	fprintf(out, "TargetVendor=0x1e0e\n");
	fprintf(out, "TargetProduct=0x9000\n");
	fprintf(out,
		"MessageContent=\"555342431234567800000000000006bd000000020000000000000000000000\"\n");
	fprintf(out, "ResponseEndpoint=0x01\n");
	fclose(out);
	system("usb_modeswitch -c /tmp/usb_modeswitch.conf");
	sleep(2);
}

static void hsoinit_icon225(int needreset, char *controldev)
{
	system("ozerocdoff -wi 0x6971");
	sleep(10);
	system("insmod hso");
	FILE *out = fopen("/tmp/conninfo.ini", "wb");
	fprintf(out, "APN=%s\n", nvram_safe_get("wan_apn"));
	fprintf(out, "USER=%s\n", nvram_safe_get("ppp_username"));
	fprintf(out, "PASS=%s\n", nvram_safe_get("ppp_passwd"));
	fprintf(out, "PIN=%s\n", nvram_safe_get("wan_pin"));
	fclose(out);
	nvram_set("3gdata", "hso");
	system("/etc/hso/hso_connect.sh restart");
}

struct DEVICES {
	int vendor;
	int product;
	char *driver;
	char *controldevice;
	char *datadevice;
	int modeswitch;
	void (*customsetup) (int needreset, char *controldev);
	char *name;
};

static struct DEVICES devicelist[] = {
//sierra wireless cards
	{0x1199, 0x6880, "sierra", "/dev/usb/tts/3", "/dev/usb/tts/4", 1, NULL, "Sierra Wireless Compass 885"},	//
	{0x1199, 0x6890, "sierra", "/dev/usb/tts/3", "/dev/usb/tts/4", 1, NULL, "Sierra Wireless Compass 888"},	//
	{0x1199, 0x6893, "sierra", "/dev/usb/tts/3", "/dev/usb/tts/4", 1, NULL, "Sierra Wireless Compass 889"},	//
	{0x1199, 0x68a3, "sierra", "/dev/usb/tts/3", "/dev/usb/tts/4", 1, NULL, "Sierra Wireless Compass 889"},	//alternate variant
	{0x1199, 0x6832, "sierra", "/dev/usb/tts/2", "/dev/usb/tts/0", 1, &reset_mc, "Sierra Wireless MC8780"},	//
	{0x1199, 0x683c, "sierra", "/dev/usb/tts/3", "/dev/usb/tts/4", 1, &reset_mc, "Sierra Wireless MC8790"},	//
	{0x1199, 0x683d, "sierra", "/dev/usb/tts/3", "/dev/usb/tts/4", 1, &reset_mc, "Sierra Wireless MC8790"},	//
	{0x1199, 0x683e, "sierra", "/dev/usb/tts/3", "/dev/usb/tts/4", 1, &reset_mc, "Sierra Wireless MC8790"},	//
	{0x1199, 0x68a3, "sierra", "/dev/usb/tts/2", "/dev/usb/tts/0", 1, &reset_mc, "Sierra Wireless MC8700"},	//
	{0x1199, 0x6812, "sierra", "/dev/usb/tts/2", "/dev/usb/tts/0", 1, &reset_mc, "Sierra Wireless MC8775V"},	//
//option/huawai
	{0x12d1, 0x1001, "option", "/dev/usb/tts/0", "/dev/usb/tts/0", 2, NULL, "HUAWAI/Option E600 or generic"},	//
	{0x12d1, 0x1003, "option", "/dev/usb/tts/0", "/dev/usb/tts/0", 2, NULL, "HUAWAI/Option E172/EC270"},	//
	{0x12d1, 0x1412, "option", "/dev/usb/tts/0", "/dev/usb/tts/0", 2, NULL, "HUAWAI/Option EC168"},	//
	{0x12d1, 0x1446, "option", "/dev/usb/tts/0", "/dev/usb/tts/0", 2, &modeswitch_e1550, "HUAWAI/Option E1550"},	//
	{0x0af0, 0x7011, "option", "/dev/usb/tts/0", "/dev/usb/tts/0", 2, NULL, "HUAWAI/Option E301 HSUPA"},	//
	{0x1410, 0x5030, "option", "/dev/usb/tts/0", "/dev/usb/tts/0", 2, &modeswitch_usb760, "Novatel USB760 CDROM Mode"},	//
	{0x1410, 0x6000, "option", "/dev/usb/tts/0", "/dev/usb/tts/0", 2, NULL, "Novatel USB760 Modem Mode"},	//
//qualcomm
	{0x1e0e, 0x9000, "option", "/dev/usb/tts/2", "/dev/usb/tts/2", 3, NULL, "Qualcomm ICON 210 Modem Mode"},	//
	{0x1e0e, 0xf000, "option", "/dev/usb/tts/2", "/dev/usb/tts/2", 3, &modeswitch_icon210, "Qualcomm ICON 210 CDROM Mode"},	//
	{0x0af0, 0x6971, NULL, "hso", "hso", 0, &hsoinit_icon225, "Qualcomm ICON 225"},	//
//ericsson
	{0x0bdb, 0x1900, "option", "/dev/usb/tts/0", "/dev/usb/tts/1", 0, NULL, "Ericsson F3507g"},	//
	{0x0bdb, 0x1902, "option", "/dev/usb/tts/0", "/dev/usb/tts/1", 0, NULL, "Ericsson F3507g"},	//
	{0xffff, 0xffff, NULL, NULL, NULL, 0, NULL, NULL}	//
};

char *get3GControlDevice(void)
{
#if defined(ARCH_broadcom) && !defined(HAVE_BCMMODERN)
	mkdir("/tmp/usb");
	eval("mount", "-t", "usbfs", "usb", "/tmp/usb");
//insmod("sierra");  //further investigation required (compass problem)
#endif
	int needreset = 1;
	char *ttsdevice = "/dev/usb/tts/0";
#ifdef HAVE_CAMBRIA
	int gpio1 = atoi(nvram_safe_get("gpio26"));
	int gpio2 = atoi(nvram_safe_get("gpio27"));
	int select = atoi(nvram_safe_get("wan_select"));
	switch (select) {
	case 1:
		if (gpio1 == 1 || gpio2 == 0)
			needreset = 0;
		else {
			gpio1 = 1;
			gpio2 = 0;
		}
		break;
	case 2:
		if (gpio1 == 0 || gpio2 == 1)
			needreset = 0;
		else {
			gpio1 = 0;
			gpio2 = 1;
		}
		break;
	case 3:
		if (gpio1 == 1 || gpio2 == 1)
			needreset = 0;
		else {
			gpio1 = 1;
			gpio2 = 1;
		}
		break;
	default:
		if (gpio1 == 0 && gpio2 == 0) {
			gpio1 = 1;
			gpio2 = 0;
		} else
			needreset = 0;
		break;
	}
	if (gpio1) {
		nvram_set("gpio26", "1");
		set_gpio(26, 1);
	} else {
		nvram_set("gpio26", "0");
		set_gpio(26, 0);
	}
	if (gpio2) {
		nvram_set("gpio27", "1");
		set_gpio(27, 1);
	} else {
		nvram_set("gpio27", "0");
		set_gpio(27, 0);
	}
#endif
	nvram_unset("3gnmvariant");
	nvram_set("3gdata", "/dev/usb/tts/0");

	int devicecount = 0;
	while (devicelist[devicecount].vendor != 0xffff) {
		if (scanFor
		    (devicelist[devicecount].vendor,
		     devicelist[devicecount].product)) {
			fprintf(stderr, "%s detected\n",
				devicelist[devicecount].name);
			if (devicelist[devicecount].driver) {
				insmod("usbserial");
				insmod(devicelist[devicecount].driver);
			}
			if (devicelist[devicecount].datadevice)
				nvram_set("3gdata",
					  devicelist[devicecount].datadevice);
			if (devicelist[devicecount].modeswitch) {
				char variant[32];
				sprintf(variant, "%d",
					devicelist[devicecount].modeswitch);
				nvram_set("3gnmvariant", variant);
			}
			//start custom setup, if defined
			if (devicelist[devicecount].customsetup)
				devicelist[devicecount].customsetup(needreset,
								    devicelist
								    [devicecount].controldevice);
			return devicelist[devicecount].controldevice;
		}
		devicecount++;
	}
	//not found, use generic implementation (tts0, all drivers)

	insmod("usbserial");
	insmod("sierra");
	insmod("option");
	return ttsdevice;
}

/*
//future
typedef struct {
char *devicename;
int vendorid;
int productid;
char *drivers;
char *3gdata;
char *controldevice;
int iconswitch;
}3GDEVICE;
*/
