/*
 * Copyright 2011-2012 Arx Libertatis Team (see the AUTHORS file)
 *
 * This file is part of Arx Libertatis.
 *
 * Arx Libertatis is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Arx Libertatis is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Arx Libertatis.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "core/SaveGame.h"

#include <sstream>
#include <iomanip>
#include <algorithm>

#include "core/Config.h"
#include "io/fs/Filesystem.h"
#include "io/log/Logger.h"
#include "io/resource/PakReader.h"
#include "scene/ChangeLevel.h"

using std::string;

namespace {

static const fs::path SAVEGAME_NAME = "gsave.sav";
static const fs::path SAVEGAME_DIR = "save";
static const fs::path SAVEGAME_THUMBNAIL = "gsave.bmp";
static const std::string QUICKSAVE_ID = "ARX_QUICK_ARX";

enum SaveGameChange {
	SaveGameRemoved,
	SaveGameUnchanged,
	SaveGameChanged
};

static int saveTimeCompare(const SaveGame & a, const SaveGame & b) {
	return (a.stime > b.stime);
}

} // anonnymous namespace

SaveGameList savegames;

void SaveGameList::update() {
	
	LogDebug("SaveGameList::update()");
	
	size_t old_count = savelist.size();
	std::vector<SaveGameChange> found(old_count, SaveGameRemoved);
	
	bool new_saves = false;
	
	size_t max_name_length = 0;
	
	fs::path savedir = config.paths.user / SAVEGAME_DIR;
	
	for(fs::directory_iterator it(savedir); !it.end(); ++it) {
		
		fs::path dirname = it.name();
		fs::path path = savedir / dirname / SAVEGAME_NAME;
		
		std::time_t stime = fs::last_write_time(path);
		if(stime == 0) {
			LogDebug("ignoring directory without " << SAVEGAME_NAME << ": " << path);
			continue;
		}
		
		size_t index = (size_t)-1;
		for(size_t i = 0; i < old_count; i++) {
			if(savelist[i].savefile == path) {
				index = i;
			}
		}
		if(index != (size_t)-1 && savelist[index].stime == stime) {
			found[index] = SaveGameUnchanged;
			continue;
		}
		
		string name;
		float version;
		long level;
		unsigned long ignored;
		if(ARX_CHANGELEVEL_GetInfo(path, name, version, level, ignored) == -1) {
			LogWarning << "unable to get save file info for " << path;
			continue;
		}
		
		new_saves = true;
		
		SaveGame * save;
		if(index == (size_t)-1) {
			// Make another save game slot at the end
			savelist.resize(savelist.size() + 1);
			save = &savelist.back();
		} else {
			found[index] = SaveGameChanged;
			save = &savelist[index];
		}
		
		save->name = name;
		save->level = level;
		save->stime = stime;
		save->savefile = path;
		
		save->quicksave = (name == QUICKSAVE_ID || name == "ARX_QUICK_ARX1");
		
		fs::path thumbnail = path.parent() / SAVEGAME_THUMBNAIL;
		if(fs::exists(thumbnail)) {
			res::path thumbnail_res = res::path("save") / dirname.string() / SAVEGAME_THUMBNAIL.string();
			resources->removeFile(thumbnail_res);
			resources->addFiles(thumbnail, thumbnail_res);
			save->thumbnail = thumbnail_res.remove_ext();
		} else {
			save->thumbnail.clear();
		}
		
		max_name_length = std::max(save->quicksave ? 9 : name.length(), max_name_length);
		
		const struct tm & t = *localtime(&stime);
		std::ostringstream oss;
		oss << std::setfill('0') << (t.tm_year + 1900) << "-" << std::setw(2) << (t.tm_mon + 1)
		    << "-" << std::setw(2) << t.tm_mday << "   " << std::setfill(' ') << std::setw(2)
		    << t.tm_hour << ":" << std::setfill('0') << std::setw(2) << t.tm_min << ":"
		    << std::setw(2) << t.tm_sec;
		save->time = oss.str();
	}
	
	size_t o = 0;
	for(size_t i = 0; i < savelist.size(); i++) {
		
		// print new savegames
		if(i >= old_count || found[i] == SaveGameChanged) {
			
			std::ostringstream oss;
			if(savelist[i].quicksave) {
				oss << "(quicksave)" << std::setw(max_name_length - 8) << ' ';
			} else {
				oss << "\"" << savelist[i].name << "\""
						<< std::setw(max_name_length - savelist[i].name.length() + 1) << ' ';
			}
			
			LogInfo << "found save " << oss.str() << "  " << savelist[i].time;
		}
		
		if(i >= old_count || found[i] != SaveGameRemoved) {
			if(o != i) {
				savelist[o] = savelist[i];
			}
			o++;
		}
	}
	savelist.resize(o);
	
	if(new_saves) {
		std::sort(savelist.begin(), savelist.end(), saveTimeCompare);
	}
	
	LogDebug("found " << savelist.size() << " savegames");
}

void SaveGameList::remove(iterator save) {
	
	arx_assert(save >= begin() && save < end());
	
	fs::remove(save->savefile);
	fs::path savedir = save->savefile.parent();
	fs::remove(savedir / SAVEGAME_THUMBNAIL);
	if(fs::directory_iterator(savedir).end()) {
		fs::remove(savedir);
	}
	
	update();
}

bool SaveGameList::save(const string & name, iterator overwrite, const Image & thumbnail) {
	
	arx_assert(overwrite >= begin() && overwrite <= end());
	
	fs::path savefile;
	if(overwrite != end()) {
		savefile = overwrite->savefile;
	} else {
		size_t index = 0;
		do {
			std::ostringstream oss;
			oss << "save" << std::setfill('0') << std::setw(4) << index++;
			savefile = config.paths.user / SAVEGAME_DIR / oss.str();
		} while(fs::exists(savefile));
		fs::create_directories(savefile);
		savefile /= SAVEGAME_NAME;
	}
	
	if(!ARX_CHANGELEVEL_Save(name, savefile)) {
		return false;
	}
	
	if(thumbnail.IsValid() && !thumbnail.save(savefile.parent() / SAVEGAME_THUMBNAIL)) {
		LogWarning << "failed to save screenshot to " << (savefile.parent() / SAVEGAME_THUMBNAIL);
	}
	
	update();
	
	return true;
}

bool SaveGameList::quicksave(const Image & thumbnail) {
	
	iterator overwrite = end();
	std::time_t time = std::numeric_limits<std::time_t>::max();
	
	size_t nfound = 0;
	
	// Find the oldest quicksave.
	for(iterator i = begin(); i != end(); i++) {
		if(i->quicksave) {
			nfound++;
			if(i->stime < time) {
				overwrite = i, time = i->stime;
			}
		}
	}
	
	// Create a new quicksave slot if there aren't enough already.
	if(nfound < (size_t)config.misc.quicksaveSlots) {
		overwrite = end();
	}
	
	return save(QUICKSAVE_ID, overwrite, thumbnail);
}

SaveGameList::iterator SaveGameList::quickload() {
	
	if(savelist.empty()) {
		return end();
	}
	
	// update() already sorts the savegame list so we can just return the first one.
	
	return begin();
}
