/*
 * Copyright (c) 2003-2007 Andrea Luzzardi <scox@sig11.org>
 *
 * This file is part of the pam_usb project. pam_usb is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * pam_usb is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>
#include "mem.h"
#include "conf.h"
#include "hal.h"
#include "log.h"
#include "pad.h"
#include "device.h"

static int pusb_device_connected(t_pusb_options *opts, DBusConnection *dbus)
{
	int retval = 0;

	log_debug("Searching for \"%s\" in the hardware database...\n",
			opts->device.name);
	retval = pusb_hal_find_item_for_udisk2(dbus,
			"Serial", opts->device.serial,
			"Vendor", opts->device.vendor,
			"Model", opts->device.model,
			"IdUUID", opts->device.volume_uuid,
			NULL);

	log_debug("Device \"%s\" is connected (good).\n", opts->device.name);
	return retval;
}

int pusb_device_check(t_pusb_options *opts,
		const char *user)
{
	DBusConnection	*dbus = NULL;
	int				retval = 0;

	log_debug("Connecting to HAL...\n");
	if (!(dbus = pusb_hal_dbus_connect()))
		return (0);

	if (!pusb_device_connected(opts, dbus))
	{
		pusb_hal_dbus_disconnect(dbus);
		return (0);
	}

	if (opts->one_time_pad)
	{
		log_warning("DISABLED: Performing one time pad verification...\n");
		//retval = pusb_pad_check(opts, dbus, user);
		retval = 1;
	}
	else
	{
		log_debug("One time pad is disabled, no more verifications to do.\n");
		retval = 1;
	}

	pusb_hal_dbus_disconnect(dbus);
	return (retval);
}

int pusb_device_check_for_udisks2(t_pusb_options *opts,
		const char *user)
{
	DBusConnection	*dbus = NULL;

	if (!pusb_device_connected(opts, dbus))
	{
		return 0;
	}

	return 1;
}
