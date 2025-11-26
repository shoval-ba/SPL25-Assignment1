#include "MixingEngineService.h"
#include <iostream>
#include <memory>
#include <cmath>

/**
 * TODO: Implement MixingEngineService constructor
 */
MixingEngineService::MixingEngineService()
    : active_deck(0),
      auto_sync(false),
      bpm_tolerance(0)
{
    decks[0] = nullptr;
    decks[1] = nullptr;
    std::cout << "[MixingEngineService] Initialized with 2 empty decks." << std::endl;
}

/**
 * TODO: Implement MixingEngineService destructor
 */
MixingEngineService::~MixingEngineService()
{
    std::cout << "[MixingEngineService] Cleaning up decks.... " << std::endl;

    if (decks[0] != nullptr)
    {
        delete decks[0];
        decks[0] = nullptr;
    }
    if (decks[1] != nullptr)
    {
        delete decks[1];
        decks[1] = nullptr;
    }
}

/**
 * TODO: Implement loadTrackToDeck method
 * @param track: Reference to the track to be loaded
 * @return: Index of the deck where track was loaded, or -1 on failure
 */
int MixingEngineService::loadTrackToDeck(const AudioTrack &track)
{
    std::cout << "\n=== Loading Track to Deck ===" << std::endl;
    PointerWrapper<AudioTrack> cloned_track = track.clone();
    if (cloned_track.get() == nullptr)
    {
        std::cout << "[ERROR] Track: \"" << track.get_title() << "\" failed to clone" << std::endl;
        return -1;
    }
    int index;
    if (decks[0] == nullptr && decks[1] == nullptr)
    {
        index = 0;
    }
    else
    {
        index = 1 - active_deck;
    }
    std::cout << "[Deck Switch] Target deck: " << index << std::endl;

    if (decks[index] != nullptr)
    {
        delete decks[index];
        decks[index] = nullptr;
    }

    cloned_track->load();
    cloned_track->analyze_beatgrid();

    if (decks[active_deck] != nullptr && auto_sync)
    {
        if (!can_mix_tracks(cloned_track))
        {
            sync_bpm(cloned_track);
        }
    }

    decks[index] = cloned_track.release();
    std::cout << "[Load Complete] '" << decks[index]->get_title() << "' is now loaded on deck " << index << std::endl;

    if (decks[active_deck] != nullptr && active_deck != index)
    {
        std::cout << "[Unload] Unloading previous deck " << active_deck << " (" << decks[active_deck]->get_title() << ")" << std::endl;
        delete decks[active_deck];
        decks[active_deck] = nullptr;
    }
    active_deck = index;
    std::cout << "[Active Deck] Switched to deck " << index << std::endl;
    return index;
}

/**
 * @brief Display current deck status
 */
void MixingEngineService::displayDeckStatus() const
{
    std::cout << "\n=== Deck Status ===\n";
    for (size_t i = 0; i < 2; ++i)
    {
        if (decks[i])
            std::cout << "Deck " << i << ": " << decks[i]->get_title() << "\n";
        else
            std::cout << "Deck " << i << ": [EMPTY]\n";
    }
    std::cout << "Active Deck: " << active_deck << "\n";
    std::cout << "===================\n";
}

/**
 * TODO: Implement can_mix_tracks method
 *
 * Check if two tracks can be mixed based on BPM difference.
 *
 * @param track: Track to check for mixing compatibility
 * @return: true if BPM difference <= tolerance, false otherwise
 */
bool MixingEngineService::can_mix_tracks(const PointerWrapper<AudioTrack> &track) const
{
    if (decks[active_deck] == nullptr)
    {
        return false;
    }
    if (track.get() == nullptr)
    {
        return false;
    }
    float active_bpm = decks[active_deck]->get_bpm();
    float new_bpm = track->get_bpm();
    float diff = std::abs(active_bpm - new_bpm);
    if (diff <= bpm_tolerance)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * TODO: Implement sync_bpm method
 * @param track: Track to synchronize with active deck
 */
void MixingEngineService::sync_bpm(const PointerWrapper<AudioTrack> &track) const
{
    if (decks[active_deck] == nullptr || track.get() == nullptr)
    {
        return;
    }

    float active_bpm = decks[active_deck]->get_bpm();
    float new_bpm = track->get_bpm();

    float avg_bpm = (active_bpm + new_bpm) / 2.0;
    track->set_bpm(avg_bpm);

    std::cout << "[Sync BPM] Syncing BPM from " << new_bpm << " to " << avg_bpm << std::endl;
}
