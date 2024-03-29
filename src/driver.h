/* $Id: driver.h 20283 2010-08-01 19:22:34Z frosch $ */

/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file driver.h Base for all drivers (video, sound, music, etc). */

#ifndef DRIVER_H
#define DRIVER_H

#include "core/enum_type.hpp"
#include "core/string_compare_type.hpp"
#include <map>

const char *GetDriverParam(const char * const *parm, const char *name);
bool GetDriverParamBool(const char * const *parm, const char *name);
int GetDriverParamInt(const char * const *parm, const char *name, int def);

class Driver {
public:
	virtual const char *Start(const char * const *parm) = 0;

	virtual void Stop() = 0;

	virtual ~Driver() { }

	/** The type of driver */
	enum Type {
		DT_BEGIN = 0, ///< Helper for iteration
		DT_MUSIC = 0, ///< A music driver, needs to be before sound to properly shut down extmidi forked music players
		DT_SOUND,     ///< A sound driver
		DT_VIDEO,     ///< A video driver
		DT_END,       ///< Helper for iteration
	};

	virtual const char *GetName() const = 0;
};

DECLARE_POSTFIX_INCREMENT(Driver::Type)


class DriverFactoryBase {
private:
	Driver::Type type;
	const char *name;
	int priority;

	typedef std::map<const char *, DriverFactoryBase *, StringCompare> Drivers;

	static Drivers &GetDrivers()
	{
		static Drivers &s_drivers = *new Drivers();
		return s_drivers;
	}

	static Driver **GetActiveDriver(Driver::Type type)
	{
		static Driver *s_driver[3] = { NULL, NULL, NULL };
		return &s_driver[type];
	}

	static const char *GetDriverTypeName(Driver::Type type)
	{
		static const char * const driver_type_name[] = { "music", "sound", "video" };
		return driver_type_name[type];
	}

protected:
	void RegisterDriver(const char *name, Driver::Type type, int priority);

public:
	DriverFactoryBase() :
		name(NULL)
	{}

	virtual ~DriverFactoryBase();

	/**
	 * Shuts down all active drivers
	 */
	static void ShutdownDrivers()
	{
		for (Driver::Type dt = Driver::DT_BEGIN; dt < Driver::DT_END; dt++) {
			Driver *driver = *GetActiveDriver(dt);
			if (driver != NULL) driver->Stop();
		}
	}

	static Driver *SelectDriver(const char *name, Driver::Type type);
	static char *GetDriversInfo(char *p, const char *last);

	/**
	 * Get a nice description of the driver-class.
	 */
	virtual const char *GetDescription() = 0;

	/**
	 * Create an instance of this driver-class.
	 */
	virtual Driver *CreateInstance() = 0;
};

#endif /* DRIVER_H */
