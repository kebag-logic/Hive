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

#include "streamOutputCountersTreeWidgetItem.hpp"

#include <map>
#include <type_traits>

#include <QMenu>

template<typename CounterNamesMap>
void StreamOutputCountersTreeWidgetItem::createCounters(CounterNamesMap const& counterNamesMap)
{
	using CounterType = typename std::decay_t<decltype(counterNamesMap)>::key_type;
	for (auto bitPos = 0u; bitPos < (sizeof(std::underlying_type_t<CounterType>) * 8); ++bitPos)
	{
		auto const flag = static_cast<CounterType>(1u << bitPos);
		auto* widget = new QTreeWidgetItem(this);
		if (auto const nameIt = counterNamesMap.find(flag); nameIt != counterNamesMap.end())
		{
			widget->setText(0, nameIt->second);
		}
		else
		{
			widget->setText(0, QString{ "Unknown 0x%1" }.arg(1u << bitPos, 8, 16, QChar{ '0' }));
		}
		widget->setHidden(true); // Hide until we get a counter value (so we don't display counters not supported by the entity)
		_counters[static_cast<la::avdecc::entity::model::DescriptorCounterValidFlag>(flag)] = widget;
	}
}

template<typename CountersType>
void StreamOutputCountersTreeWidgetItem::updateCounters(CountersType const& counters)
{
	for (auto const& [flag, value] : counters)
	{
		if (auto const it = _counters.find(static_cast<la::avdecc::entity::model::DescriptorCounterValidFlag>(flag)); it != _counters.end())
		{
			auto* widget = it->second;
			AVDECC_ASSERT(widget != nullptr, "If widget is found in the map, it should not be nullptr");
			widget->setText(1, QString::number(value));
			widget->setHidden(false);
		}
	}
}

StreamOutputCountersTreeWidgetItem::StreamOutputCountersTreeWidgetItem(la::avdecc::UniqueIdentifier const entityID, la::avdecc::entity::model::StreamIndex const streamIndex, la::avdecc::entity::model::StreamOutputCounters const& counters, std::optional<la::avdecc::entity::model::SignalPresenceChannels> const signalPresence, QTreeWidget* parent)
	: QTreeWidgetItem(parent)
	, _entityID(entityID)
	, _streamIndex(streamIndex)
{
	static std::map<la::avdecc::entity::StreamOutputCounterValidFlagMilan12, QString> s_counterNames_Milan12{
		{ la::avdecc::entity::StreamOutputCounterValidFlagMilan12::StreamStart, "Stream Start" },
		{ la::avdecc::entity::StreamOutputCounterValidFlagMilan12::StreamStop, "Stream Stop" },
		{ la::avdecc::entity::StreamOutputCounterValidFlagMilan12::MediaReset, "Media Reset" },
		{ la::avdecc::entity::StreamOutputCounterValidFlagMilan12::TimestampUncertain, "Timestamp Uncertain" },
		{ la::avdecc::entity::StreamOutputCounterValidFlagMilan12::FramesTx, "Frames TX" },
	};
	static std::map<la::avdecc::entity::StreamOutputCounterValidFlag17221, QString> s_counterNames_17221{
		{ la::avdecc::entity::StreamOutputCounterValidFlag17221::StreamStart, "Stream Start" },
		{ la::avdecc::entity::StreamOutputCounterValidFlag17221::StreamStop, "Stream Stop" },
		{ la::avdecc::entity::StreamOutputCounterValidFlag17221::StreamInterrupted, "Stream Interrupted" },
		{ la::avdecc::entity::StreamOutputCounterValidFlag17221::MediaReset, "Media Reset" },
		{ la::avdecc::entity::StreamOutputCounterValidFlag17221::TimestampUncertain, "Timestamp Uncertain" },
		{ la::avdecc::entity::StreamOutputCounterValidFlag17221::TimestampValid, "Timestamp Valid" },
		{ la::avdecc::entity::StreamOutputCounterValidFlag17221::TimestampNotValid, "Timestamp Not Valid" },
		{ la::avdecc::entity::StreamOutputCounterValidFlag17221::FramesTx, "Frames TX" },
		{ la::avdecc::entity::StreamOutputCounterValidFlag17221::EntitySpecific8, "Entity Specific 8" },
		{ la::avdecc::entity::StreamOutputCounterValidFlag17221::EntitySpecific7, "Entity Specific 7" },
		{ la::avdecc::entity::StreamOutputCounterValidFlag17221::EntitySpecific6, "Entity Specific 6" },
		{ la::avdecc::entity::StreamOutputCounterValidFlag17221::EntitySpecific5, "Entity Specific 5" },
		{ la::avdecc::entity::StreamOutputCounterValidFlag17221::EntitySpecific4, "Entity Specific 4" },
		{ la::avdecc::entity::StreamOutputCounterValidFlag17221::EntitySpecific3, "Entity Specific 3" },
		{ la::avdecc::entity::StreamOutputCounterValidFlag17221::EntitySpecific2, "Entity Specific 2" },
		{ la::avdecc::entity::StreamOutputCounterValidFlag17221::EntitySpecific1, "Entity Specific 1" },
	};
	static std::map<la::avdecc::entity::StreamOutputCounterValidFlagMilanSignalPresence, QString> s_counterNames_Milan13SignalPresence{
		{ la::avdecc::entity::StreamOutputCounterValidFlagMilanSignalPresence::StreamStart, "Stream Start" },
		{ la::avdecc::entity::StreamOutputCounterValidFlagMilanSignalPresence::StreamStop, "Stream Stop" },
		{ la::avdecc::entity::StreamOutputCounterValidFlagMilanSignalPresence::StreamInterrupted, "Stream Interrupted" },
		{ la::avdecc::entity::StreamOutputCounterValidFlagMilanSignalPresence::MediaReset, "Media Reset" },
		{ la::avdecc::entity::StreamOutputCounterValidFlagMilanSignalPresence::TimestampUncertain, "Timestamp Uncertain" },
		{ la::avdecc::entity::StreamOutputCounterValidFlagMilanSignalPresence::TimestampValid, "Timestamp Valid" },
		{ la::avdecc::entity::StreamOutputCounterValidFlagMilanSignalPresence::TimestampNotValid, "Timestamp Not Valid" },
		{ la::avdecc::entity::StreamOutputCounterValidFlagMilanSignalPresence::FramesTx, "Frames TX" },
		{ la::avdecc::entity::StreamOutputCounterValidFlagMilanSignalPresence::SignalPresence2, "Signal Presence 2" },
		{ la::avdecc::entity::StreamOutputCounterValidFlagMilanSignalPresence::SignalPresence1, "Signal Presence 1" },
		{ la::avdecc::entity::StreamOutputCounterValidFlagMilanSignalPresence::EntitySpecific8, "Entity Specific 8" },
		{ la::avdecc::entity::StreamOutputCounterValidFlagMilanSignalPresence::EntitySpecific7, "Entity Specific 7" },
		{ la::avdecc::entity::StreamOutputCounterValidFlagMilanSignalPresence::EntitySpecific6, "Entity Specific 6" },
		{ la::avdecc::entity::StreamOutputCounterValidFlagMilanSignalPresence::EntitySpecific5, "Entity Specific 5" },
		{ la::avdecc::entity::StreamOutputCounterValidFlagMilanSignalPresence::EntitySpecific4, "Entity Specific 4" },
		{ la::avdecc::entity::StreamOutputCounterValidFlagMilanSignalPresence::EntitySpecific3, "Entity Specific 3" },
		{ la::avdecc::entity::StreamOutputCounterValidFlagMilanSignalPresence::EntitySpecific2, "Entity Specific 2" },
		{ la::avdecc::entity::StreamOutputCounterValidFlagMilanSignalPresence::EntitySpecific1, "Entity Specific 1" },
	};

	// Create fields
	auto supportSignalPresence = false;
	try
	{
		switch (counters.getCounterType())
		{
			case la::avdecc::entity::model::StreamOutputCounters::CounterType::Milan_12:
				createCounters(s_counterNames_Milan12);
				break;
			case la::avdecc::entity::model::StreamOutputCounters::CounterType::IEEE17221_2021:
				createCounters(s_counterNames_17221);
				break;
			case la::avdecc::entity::model::StreamOutputCounters::CounterType::Milan_SignalPresence:
			{
				// Raw counters
				createCounters(s_counterNames_Milan13SignalPresence);
				// Signal Presence
				_signalPresenceWidget = new QTreeWidgetItem(this);
				_signalPresenceWidget->setText(0, "Signal Presence");
				// We need to care about Signal Presence
				supportSignalPresence = true;
				break;
			}
			default:
				AVDECC_ASSERT(false, "Unhandled CounterType");
				throw std::invalid_argument("Unhandled CounterType");
				break;
		}
	}
	catch (std::invalid_argument const&)
	{
		// Exception, don't create anything
	}

	// Update counters right now
	updateCounters(counters);

	// Listen for StreamOutputCountersChanged
	connect(&hive::modelsLibrary::ControllerManager::getInstance(), &hive::modelsLibrary::ControllerManager::streamOutputCountersChanged, this,
		[this](la::avdecc::UniqueIdentifier const entityID, la::avdecc::entity::model::StreamIndex const streamIndex, la::avdecc::entity::model::StreamOutputCounters const& counters)
		{
			if (entityID == _entityID && streamIndex == _streamIndex)
			{
				updateCounters(counters);
			}
		});

	// Handle SignalPresence
	if (supportSignalPresence)
	{
		// Update right now
		updateSignalPresence(signalPresence ? *signalPresence : la::avdecc::entity::model::SignalPresenceChannels{});

		// Listen for StreamOutputSignalPresenceChanged
		connect(&hive::modelsLibrary::ControllerManager::getInstance(), &hive::modelsLibrary::ControllerManager::streamOutputSignalPresenceChanged, this,
			[this](la::avdecc::UniqueIdentifier const entityID, la::avdecc::entity::model::StreamIndex const streamIndex, la::avdecc::entity::model::SignalPresenceChannels const& signalPresence)
			{
				if (entityID == _entityID && streamIndex == _streamIndex)
				{
					updateSignalPresence(signalPresence);
				}
			});
	}
}

void StreamOutputCountersTreeWidgetItem::updateCounters(la::avdecc::entity::model::StreamOutputCounters const& counters)
{
	try
	{
		switch (counters.getCounterType())
		{
			case la::avdecc::entity::model::StreamOutputCounters::CounterType::Milan_12:
				updateCounters(counters.getCounters<la::avdecc::entity::StreamOutputCounterValidFlagsMilan12>());
				this->setText(0, "Counters (Milan v1.2)");
				break;
			case la::avdecc::entity::model::StreamOutputCounters::CounterType::IEEE17221_2021:
				updateCounters(counters.getCounters<la::avdecc::entity::StreamOutputCounterValidFlags17221>());
				this->setText(0, "Counters (1722.1 v2021)");
				break;
			case la::avdecc::entity::model::StreamOutputCounters::CounterType::Milan_SignalPresence:
				updateCounters(counters.getCounters<la::avdecc::entity::StreamOutputCounterValidFlagsMilanSignalPresence>());
				this->setText(0, "Counters (Milan v1.3 - Signal Presence)");
				break;
			default:
				AVDECC_ASSERT(false, "Unhandled CounterType");
				throw std::invalid_argument("Unhandled CounterType");
				break;
		}
	}
	catch (std::invalid_argument const&)
	{
		// Exception, update all counters to reflect that
		for (auto const counterKV : _counters)
		{
			auto* widget = counterKV.second;
			AVDECC_ASSERT(widget != nullptr, "If widget is found in the map, it should not be nullptr");
			widget->setText(1, QString{ "Unhandled Output Counter Type" });
			widget->setHidden(false);
		}
	}
}

void StreamOutputCountersTreeWidgetItem::updateSignalPresence(la::avdecc::entity::model::SignalPresenceChannels const& signalPresence)
{
	// Process each bit of the signal presence value and convert to either "1" or "0"
	if (_signalPresenceWidget)
	{
		auto str = QString{};
		str.reserve(signalPresence.size() * 2); // Reserve enough space for the string
		for (auto bitPos = 0u; bitPos < signalPresence.size(); ++bitPos)
		{
			if (signalPresence.test(bitPos))
			{
				str.append("1");
			}
			else
			{
				str.append("0");
			}
		}
		_signalPresenceWidget->setText(1, str);
	}
}
