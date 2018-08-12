#include <ctime>
#include <cassert>

#include "ppsspp_config.h"
#include "Common/Log.h"
#include "Core/Config.h"
#include "DiscordIntegration.h"

#include "ext/discord-rpc/include/discord_rpc.h"

// TODO: Enable on more platforms. Make optional.

#ifdef _WIN32

const bool g_DiscordEnabled = true;

#else

const bool g_DiscordEnabled = true;

#endif


Discord g_Discord;

static const char *ppsspp_app_id = "423397985041383434";

// No context argument? What?
static void handleDiscordError(int errCode, const char *message) {
	ERROR_LOG(SYSTEM, "Discord error code %d: '%s'", message);
}

Discord::~Discord() {
	assert(!initialized_);
}

bool Discord::IsEnabled() const {
	return g_DiscordEnabled && g_Config.bDiscordPresence;
}

void Discord::Init() {
	assert(IsEnabled());
	assert(!initialized_);

	DiscordEventHandlers eventHandlers{};
	eventHandlers.errored = &handleDiscordError;
	Discord_Initialize(ppsspp_app_id, &eventHandlers, 0, nullptr);

	initialized_ = true;
}

void Discord::Shutdown() {
	assert(initialized_);
	Discord_Shutdown();
	initialized_ = false;
}

void Discord::Update() {
	if (!IsEnabled()) {
		if (initialized_) {
			Shutdown();
		}
		return;
	} else {
		if (!initialized_) {
			Init();
		}
	}

#ifdef DISCORD_DISABLE_IO_THREAD
	Discord_UpdateConnection();
#endif
	Discord_RunCallbacks();
}

void Discord::SetPresenceGame(const char *gameTitle) {
	if (!IsEnabled())
		return;
	
	if (!initialized_) {
		Init();
	}

	DiscordRichPresence discordPresence{};
	discordPresence.state = gameTitle;
	discordPresence.details = "Playing";
	discordPresence.startTimestamp = time(0);
#ifdef GOLD
	discordPresence.largeImageKey = "icon_gold_png";
#else
	discordPresence.largeImageKey = "icon_regular_png";
#endif
	Discord_UpdatePresence(&discordPresence);
}

void Discord::SetPresenceMenu() {
	if (!IsEnabled())
		return;

	if (!initialized_) {
		Init();
	}

	DiscordRichPresence discordPresence{};
	discordPresence.state = "In menu";
	discordPresence.details = "";
	discordPresence.startTimestamp = time(0);
#ifdef GOLD
	discordPresence.largeImageKey = "icon_gold_png";
#else
	discordPresence.largeImageKey = "icon_regular_png";
#endif
	Discord_UpdatePresence(&discordPresence);
}
