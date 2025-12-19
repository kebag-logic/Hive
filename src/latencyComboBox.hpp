/*
* Copyright (C) 2017-2025, Emilien Vallot, Christophe Calmejane and other contributors

* This file is part of Hive.

* Hive is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* Hive is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.

* You should have received a copy of the GNU Lesser General Public License
* along with Hive.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "aecpCommandComboBox.hpp"
#include "avdecc/helper.hpp"
#include <la/avdecc/controller/internals/avdeccControlledEntity.hpp>

#include <QInputDialog>

#include <chrono>
#include <optional>

using LatencyComboBox_t = std::tuple<std::chrono::nanoseconds, std::string, std::optional<bool>>; // Value to be sent to the device / Displayed value / Is custom value

Q_DECLARE_METATYPE(LatencyComboBox_t)

struct LatencyComboBoxCompare final
{
	bool operator()(LatencyComboBox_t const& lhs, LatencyComboBox_t const& rhs) const noexcept
	{
		// Check for custom values (we want them at the end of the combobox)
		auto const lhsCustom = std::get<2>(lhs);
		auto const rhsCustom = std::get<2>(rhs);
		auto const lhsIsCustom = lhsCustom && *lhsCustom;
		auto const rhsIsCustom = rhsCustom && *rhsCustom;

		// If lhs is custom and rhs is not, lhs is greater
		if (lhsIsCustom && !rhsIsCustom)
		{
			return false;
		}

		// If rhs is custom and lhs is not, rhs is greater
		if (!lhsIsCustom && rhsIsCustom)
		{
			return true;
		}

		// If both or none are custom, compare the values
		return std::get<0>(lhs) < std::get<0>(rhs);
	}
};
using LatencyComboBoxDataContainer = std::set<LatencyComboBox_t, LatencyComboBoxCompare>;
using BaseComboBoxType = AecpCommandComboBox<LatencyComboBox_t, LatencyComboBoxDataContainer>;

class LatencyComboBox final : public BaseComboBoxType
{
private:
public:
	LatencyComboBox(QWidget* parent = nullptr)
		: BaseComboBoxType{ parent }
	{
		// Handle index change
		setIndexChangedHandler(
			[this](LatencyComboBox_t const& latencyData)
			{
				auto latData = latencyData;
				auto const isCustomOpt = std::get<2>(latencyData);
				if (isCustomOpt && *isCustomOpt)
				{
					auto ok = false;
					auto const customValue = QInputDialog::getInt(this, "Latency (in nanoseconds)", "Count", 2000000, 1, 3000000, 1, &ok);

					if (ok)
					{
						auto const latency = std::chrono::nanoseconds{ customValue };
						latData = LatencyComboBox_t{ latency, labelFromLatency(latency), true };
					}
					else
					{
						latData = getCurrentData();
					}
				}
				return latData;
			});
	}

	void setCurrentLatencyData(LatencyComboBox_t const& latencyData)
	{
		setCurrentData(latencyData);
	}

	void setLatencyDatas(Data const& latencyDatas) noexcept
	{
		BaseComboBoxType::setAllData(latencyDatas,
			[](LatencyComboBox_t const& latencyData)
			{
				return QString::fromUtf8(std::get<1>(latencyData));
			});
	}

	LatencyComboBox_t const& getCurrentLatencyData() const noexcept
	{
		return getCurrentData();
	}

	/** Helper function to update the possible latency values based on the stream format. */
	void updatePossibleLatencyValues(la::avdecc::entity::model::StreamFormat const& streamFormat) noexcept
	{
		using namespace std::chrono_literals;
		auto const PresentationTimes = std::set<std::chrono::microseconds>{ 250us, 500us, 750us, 1000us, 1250us, 1500us, 1750us, 2000us, 2250us, 2500us, 2750us, 3000us };

		auto const streamFormatInfo = la::avdecc::entity::model::StreamFormatInfo::create(streamFormat);
		auto const freq = static_cast<std::uint32_t>(streamFormatInfo->getSamplingRate().getNominalSampleRate());
		auto latencyDatas = std::set<LatencyComboBox_t, LatencyComboBoxCompare>{};
		for (auto const& presentationTime : PresentationTimes)
		{
			// Compute the number of samples for the given desired presentation time, rounding to the nearest integer
			auto const numberOfSamplesInBuffer = std::lround(presentationTime.count() * freq / std::remove_reference_t<decltype(presentationTime)>::period::den);

			// Compute the required duration of the buffer to hold the desired number of samples
			auto const bufferDuration = std::chrono::nanoseconds{ std::lround(numberOfSamplesInBuffer * std::chrono::nanoseconds::period::den / freq) };

			latencyDatas.insert(LatencyComboBox_t{ bufferDuration, labelFromLatency(bufferDuration), false });
		}
		latencyDatas.insert(LatencyComboBox_t{ std::chrono::nanoseconds{}, "Custom", true });
		setLatencyDatas(latencyDatas);
	}

	static std::string labelFromLatency(std::chrono::nanoseconds const& latency) noexcept
	{
		// If the latency is below 100 microsecond, we display it in nanoseconds
		if (latency < std::chrono::microseconds{ 100 })
		{
			return std::to_string(latency.count()) + " nsec";
		}

		// Convert desired latency from nanoseconds to floating point milliseconds with 3 digits after the decimal point (only if not zero)
		auto const latencyInMs = static_cast<float>(latency.count()) / static_cast<float>(std::chrono::nanoseconds::period::den / std::chrono::milliseconds::period::den);
		auto const latencyInMsRounded = std::round(latencyInMs * 1000.0f) / 1000.0f;
		auto ss = std::stringstream{};
		ss << std::fixed << std::setprecision(3) << latencyInMsRounded;

		return ss.str() + " msec";
	}

private:
	using BaseComboBoxType::setIndexChangedHandler;
	using BaseComboBoxType::setAllData;
	using BaseComboBoxType::getCurrentData;

	bool isStaticValue(std::chrono::nanoseconds const& latency) const noexcept
	{
		for (auto const& latencyData : getAllData())
		{
			auto const lat = std::get<0>(latencyData);
			auto const isCustomOpt = std::get<2>(latencyData);
			if (lat == latency && (!isCustomOpt || !*isCustomOpt))

			{
				return true;
			}
		}
		return false;
	}

	virtual void setCurrentData(LatencyComboBox_t const& data) noexcept override
	{
		auto const lg = QSignalBlocker{ this }; // Block internal signals so setCurrentText do not trigger "currentIndexChanged"

		auto const previousLatency = std::get<0>(_previousData);
		auto const latency = std::get<0>(data);
		auto label = QString::fromUtf8(std::get<1>(data));
		auto newData = data;

		// If the previous value is not part of the static data, it was added as a temporary value and we have to remove it
		if (!isStaticValue(previousLatency))
		{
			auto const index = findData(QVariant::fromValue(_previousData));

			if (index != -1)
			{
				removeItem(index);
			}
		}

		// If the new value is not part of the static data, we have to add it as a temporary value
		if (!isStaticValue(latency))
		{
			auto const strLabel = labelFromLatency(latency);
			label = QString::fromUtf8(strLabel);
			newData = LatencyComboBox_t{ latency, strLabel, true };

			addItem(label, QVariant::fromValue(newData));
			auto const index = findData(QVariant::fromValue(newData));

			QFont font;
			font.setItalic(true);

			setItemData(index, font, Qt::FontRole);
		}

		_previousData = std::move(newData);
		setCurrentText(label);
	}
};
