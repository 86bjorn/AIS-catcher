/*
Copyright(c) 2021 jvde.github@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "Device.h"

#ifdef HASSDRPLAY
#include <sdrplay_api.h>
#endif

namespace Device {

	class SettingsSDRPLAY : public DeviceSettings
	{
	private:
	public:

		friend class SDRPLAY;

		void Print();
		void Set(std::string option, std::string arg);
	};

	class SDRPLAY : public Control, public StreamOut<CFLOAT32>
	{
#ifdef HASSDRPLAY

		sdrplay_api_DeviceT device;

	public:

		// Control

		void Play();
		void Pause();

		void setSampleRate(uint32_t);
		void setFrequency(uint32_t);

		std::vector<uint32_t> SupportedSampleRates();

		bool isStreaming();

		virtual bool isCallback() { return true; }

		static void pushDeviceList(std::vector<Description>& DeviceList);
		static int getDeviceCount();

		// Device specific
		void openDevice(uint64_t h);
		void openDevice();

		void setSettings(SettingsSDRPLAY& s);

		// static constructor and data
		static class _API
		{
		public:

			float version = 0.0;
			bool running = false;
			std::string status = "unknown";

			_API(); ~_API();
		} _api;

#endif
	};
}
