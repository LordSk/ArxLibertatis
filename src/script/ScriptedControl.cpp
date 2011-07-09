
#include "script/ScriptedControl.h"

#include "physics/Attractors.h"
#include "physics/Collisions.h"
#include "io/Logger.h"
#include "platform/String.h"
#include "scene/Interactive.h"
#include "scene/GameSound.h"
#include "script/ScriptEvent.h"
#include "script/ScriptUtils.h"

using std::string;

namespace script {

namespace {

class AttractorCommand : public Command {
	
public:
	
	ScriptResult execute(Context & context) {
		
		string target = context.getLowercase();
		
		long t = GetTargetByNameTarget(target);
		if(t == -2) {
			t = GetInterNum(context.getIO());
		}
		
		string power = context.getLowercase();
		
		float val = 0.f;
		float radius = 0.f;
		
		if(power != "off") {
			val = context.getFloatVar(power);
			radius = context.getFloat();
		}
		
		ARX_SPECIAL_ATTRACTORS_Add(t, val, radius);
		
		LogDebug << "attractor \"" << target << "\" " << val << ' ' << radius;
		
		return ACCEPT;
	}
	
	~AttractorCommand() { }
	
};

class AmbianceCommand : public Command {
	
public:
	
	ScriptResult execute(Context & context) {
		
		string options = context.getFlags();
		
		if(!options.empty()) {
			u64 flg = flags(options);
			if(flg & flag('v')) {
				float volume = context.getFloat();
				string ambiance = context.getLowercase();
				ARX_SOUND_PlayScriptAmbiance(ambiance, ARX_SOUND_PLAY_LOOPED, volume * 0.01f);
				LogDebug << "ambiance " << options << ' ' << volume << " \"" << ambiance << "\"";
			} else if(!flg || (flg & ~flag('v'))) {
				LogWarning << "unexpected flags: ambiance " << options;
			}
		} else {
			string ambiance = context.getLowercase();
			if(ambiance == "kill") {
				ARX_SOUND_KillAmbiances();
			} else {
				ARX_SOUND_PlayScriptAmbiance(ambiance);
			}
			LogDebug << "ambiance \"" << ambiance << "\"";
		}
		
		return ACCEPT;
	}
	
	~AmbianceCommand() { }
	
};

class AnchorBlockCommand : public Command {
	
public:
	
	ScriptResult execute(Context & context) {
		
		string choice = context.getLowercase();
		
		if(choice == "on" || choice == "yes") {
			ANCHOR_BLOCK_By_IO(context.getIO(), 1);
		} else {
			ANCHOR_BLOCK_By_IO(context.getIO(), 0);
		}
		
		LogDebug << "anchorblock \"" << choice << "\"";
		
		return ACCEPT;
	}
	
	~AnchorBlockCommand() { }
	
};

class AttachCommand : public Command {
	
public:
	
	ScriptResult execute(Context & context) {
		
		string sourceio = context.getLowercase();
		long t = GetTargetByNameTarget(sourceio);
		if(t == -2) {
			t = GetInterNum(context.getIO()); //self
		}
		
		string source = context.getLowercase(); // source action_point
		
		string targetio = context.getLowercase();
		long t2 = GetTargetByNameTarget(targetio);
		if(t2 == -2) {
			t2 = GetInterNum(context.getIO()); //self
		}
		
		string target = context.getLowercase();
		
		ARX_INTERACTIVE_Attach(t, t2, source, target);
		
		LogDebug << "attach " << sourceio << ' ' << source << ' ' << targetio << ' ' << target;
		
		return ACCEPT;
	}
	
	~AttachCommand() {}
	
};

}

void setupScriptedControl() {
	
	ScriptEvent::registerCommand("attractor", new AttractorCommand);
	ScriptEvent::registerCommand("ambiance", new AmbianceCommand);
	ScriptEvent::registerCommand("anchorblock", new AnchorBlockCommand);
	ScriptEvent::registerCommand("attach", new AttachCommand);
	
}

} // namespace script
