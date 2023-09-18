/*
	Copyright(c) 2021-2023 jvde.github@gmail.com

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <string>
#include <time.h>
#include <vector>
#include <iomanip>
#include <sstream>

#include "Utilities.h"

namespace AIS {

#define MAX_AIS_LENGTH (128 * 8)

	class GPS {
		float lat = 0, lon = 0, heading = 0, speed = 0;
		const std::string& nmea;
		const std::string& json;

		const std::string formatLatLon(float, bool) const;

	public:
		GPS(float lt, float ln, const std::string& s, const std::string& j) : lat(lt), lon(ln), nmea(s), json(j) {}

		float getLat() const { return lat; }
		float getLon() const { return lon; }

		const std::string getNMEA() const;
		const std::string getJSON() const;
	};

	class Message {
	protected:
		const int MAX_NMEA_CHARS = 56;
		static int ID;
		std::string line = "!AIVDM,X,X,X,X," + std::string(MAX_NMEA_CHARS, '.') + ",X*XX\n\r"; // longest line

		uint8_t data[128];
		std::time_t rxtime;
		int length;
		char channel;
		int station;

	public:
		std::vector<std::string> NMEA;

		void Stamp(std::time_t t = (std::time_t)0L) {
			if ((long int)t == 0)
				std::time(&rxtime);
			else
				setRxTimeUnix(t);
		}

		std::string getNMEAJSON(unsigned mode, float level, float ppm) const;

		std::string getRxTime() const {
			return Util::Convert::toTimeStr(rxtime);
		}

		std::time_t getRxTimeUnix() const {
			return rxtime;
		}

		void setRxTimeUnix(std::time_t t) {
			rxtime = t;
		}

		void clear() {
			length = 0;
			NMEA.resize(0);
			std::memset(data, 0, 128);
		}

		bool validate() {
			const int ml[27] = { 149, 149, 149, 168, 418, 88, 72, 56, 168, 70, 168, 72, 40, 40, 88, 92, 80, 168, 312, 70, 271, 145, 154, 160, 72, 60, 96 };

			if (type() < 1 || type() > 27) return false;
			if (getLength() < ml[type() - 1]) return false;

			return true;
		}

		unsigned type() const {
			return data[0] >> 2;
		}

		unsigned repeat() const {
			return data[0] & 3;
		}

		unsigned mmsi() const {
			return (data[1] << 22) | (data[2] << 14) | (data[3] << 6) | (data[4] >> 2);
		}

		unsigned getUint(int start, int len) const;
		int getInt(int start, int len) const;
		void getText(int start, int len, std::string& str) const;

		void setBit(int i, bool b) {
			if (b)
				data[i >> 3] |= (1 << (i & 7));
			else
				data[i >> 3] &= ~(1 << (i & 7));
		}

		bool getBit(int i) const {
			return data[i >> 3] & (1 << (i & 7));
		}

		char getLetter(int pos) const;
		void setLetter(int pos, char c);
		void appendLetter(char c) { setLetter(length / 6, c); }
		void reduceLength(int l) { length -= l; }

		void setLength(int l) { length = l; }
		int getLength() const { return length; }

		void setChannel(char c) { channel = c; }
		char getChannel() const { return channel; }

		void setOrigin(char c, int s) {
			channel = c;
			station = s;
		}
		int getStation() const { return station; }
		void buildNMEA(TAG& tag, int id = -1);
	};

	class Filter : public Setting {
		const uint32_t all_msg = 0b1111111111111111111111111110;
		uint32_t allow = all_msg;
		bool on = false;
		bool GPS = true, AIS = true;

	public:
		bool SetOption(std::string option, std::string arg);
		bool isOn() { return on; }
		std::string getAllowed();
		bool includeGPS() { return on ? GPS : true; }
		bool include(const Message& msg) {
			if (!on) return true;
			if (!AIS) return false;
			unsigned type = msg.type() & 31;
			return ((1U << type) & allow) != 0;
		}
	};
}
