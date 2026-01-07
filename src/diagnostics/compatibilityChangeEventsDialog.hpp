/*
* Copyright (C) 2017-2026, Emilien Vallot, Christophe Calmejane and other contributors

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

#include <QDialog>
#include <la/avdecc/controller/avdeccController.hpp>

class CompatibilityChangeEventsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit CompatibilityChangeEventsDialog(QString const& title, la::avdecc::controller::ControlledEntity::CompatibilityChangedEvents const& events, QWidget* parent = nullptr);
	virtual ~CompatibilityChangeEventsDialog() noexcept;

	// Deleted compiler auto-generated methods
	CompatibilityChangeEventsDialog(CompatibilityChangeEventsDialog&&) = delete;
	CompatibilityChangeEventsDialog(CompatibilityChangeEventsDialog const&) = delete;
	CompatibilityChangeEventsDialog& operator=(CompatibilityChangeEventsDialog const&) = delete;
	CompatibilityChangeEventsDialog& operator=(CompatibilityChangeEventsDialog&&) = delete;

private:
	class CompatibilityChangeEventsDialogImpl;
	CompatibilityChangeEventsDialogImpl* _pImpl{ nullptr };
};
