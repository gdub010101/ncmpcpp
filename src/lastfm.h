/***************************************************************************
 *   Copyright (C) 2008-2016 by Andrzej Rybczak                            *
 *   electricityispower@gmail.com                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.              *
 ***************************************************************************/

#ifndef NCMPCPP_LASTFM_H
#define NCMPCPP_LASTFM_H

#include "config.h"

#include <future>
#include <memory>

#include "interfaces.h"
#include "lastfm_service.h"
#include "screen.h"
#include "utility/wide_string.h"

struct Lastfm: Screen<NC::Scrollpad>, Tabbable
{
	Lastfm();
	
	virtual void switchTo() override;
	virtual void resize() override;
	
	virtual std::wstring title() override;
	virtual ScreenType type() override { return ScreenType::Lastfm; }
	
	virtual void update() override;
	
	virtual bool isLockable() override { return false; }
	virtual bool isMergable() override { return true; }
	
	template <typename ServiceT>
	void queueJob(ServiceT *service)
	{
		auto old_service = dynamic_cast<ServiceT *>(m_service.get());
		// if the same service and arguments were used, leave old info
		if (old_service != nullptr && *old_service == *service)
			return;

		m_service = std::shared_ptr<ServiceT>(service);
		m_worker = std::async(std::launch::async,
		                      std::bind(&LastFm::Service::fetch, m_service));

		w.clear();
		w << "Fetching information...";
		w.flush();
		m_title = ToWString(m_service->name());
	}

private:
	void getResult();
	
	std::wstring m_title;
	
	std::shared_ptr<LastFm::Service> m_service;
	std::future<LastFm::Service::Result> m_worker;
};

extern Lastfm *myLastfm;

#endif // NCMPCPP_LASTFM_H
