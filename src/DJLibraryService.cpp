#include "DJLibraryService.h"
#include "SessionFileParser.h"
#include "MP3Track.h"
#include "WAVTrack.h"
#include <iostream>
#include <memory>
#include <filesystem>

DJLibraryService::DJLibraryService(const Playlist &playlist)
    : playlist(playlist) , library() {}

DJLibraryService::~DJLibraryService()
{
    for (size_t i = 0; i < library.size(); i++)
    {
        delete library[i];
    }
    library.clear();
}
/**
 * @brief Load a playlist from track indices referencing the library
 * @param library_tracks Vector of track info from config
 */
void DJLibraryService::buildLibrary(const std::vector<SessionConfig::TrackInfo> &library_tracks)
{

    for (size_t i = 0; i < library_tracks.size(); i++)
    {
        const auto &track_data = library_tracks[i];
        AudioTrack *new_track = nullptr;
        if (track_data.type == "MP3")
        {
            new_track = new MP3Track(
                track_data.title,
                track_data.artists,
                track_data.duration_seconds,
                track_data.bpm,
                track_data.extra_param1,
                track_data.extra_param2);
            std::cout << "MP3Track created: " << track_data.extra_param1 << " kbps" << std::endl;
        }
        else
        {
            new_track = new WAVTrack(
                track_data.title,
                track_data.artists,
                track_data.duration_seconds,
                track_data.bpm,
                track_data.extra_param1,
                track_data.extra_param2);
            std::cout << "WAVTrack created: " << track_data.extra_param1 << "Hz/"
                      << track_data.extra_param2 << "bit" << std::endl;
        }
        if (new_track != nullptr)
        {
            library.push_back(new_track);
        }
    }
    std::cout << "[INFO] Track library built: " << library.size() << " tracks loaded" << std::endl;
}
/**
 * @brief Display the current state of the DJ library playlist
 *
 */
void DJLibraryService::displayLibrary() const
{
    std::cout << "=== DJ Library Playlist: "
              << playlist.get_name() << " ===" << std::endl;

    if (playlist.is_empty())
    {
        std::cout << "[INFO] Playlist is empty.\n";
        return;
    }

    // Let Playlist handle printing all track info
    playlist.display();

    std::cout << "Total duration: " << playlist.get_total_duration() << " seconds" << std::endl;
}

/**
 * @brief Get a reference to the current playlist
 *
 * @return Playlist&
 */
Playlist &DJLibraryService::getPlaylist()
{
    // Your implementation here
    return playlist;
}

/**
 * TODO: Implement findTrack method
 *
 * HINT: Leverage Playlist's find_track method
 */
AudioTrack *DJLibraryService::findTrack(const std::string &track_title)
{
    AudioTrack *track = playlist.find_track(track_title);
    if (track != nullptr)
    {
        return track;
    }
    else
    {
        return nullptr;
    }
}

void DJLibraryService::loadPlaylistFromIndices(const std::string &playlist_name,
                                               const std::vector<int> &track_indices)
{
    std::cout << "[INFO] Loading playlist: " << playlist_name << std::endl;
    playlist = Playlist(playlist_name);

    for (int index : track_indices)
    {
        int real_index = index - 1;
        if (real_index < 0 || static_cast<size_t>(real_index) >= library.size())
        {
            std::cout << "[WARNING] Invalid track index: " << index << std::endl;
            continue;
        }

        AudioTrack *original_track = library[real_index];
        PointerWrapper<AudioTrack> cloned_wrapper = original_track->clone();
        if (cloned_wrapper.get() == nullptr)
        {
            std::cout << "[ERROR] Failed to clone track at index " << index << std::endl;
            continue;
        }

        cloned_wrapper->load();
        cloned_wrapper->analyze_beatgrid();

        AudioTrack *raw_track = cloned_wrapper.release();
        playlist.add_track(raw_track);
        std::cout << "Added '" << raw_track->get_title() << "' to playlist '" << playlist_name << "'" << std::endl;
    }
    std::cout << "[INFO] Playlist loaded: " << playlist_name << " (" << track_indices.size() << " tracks)" << std::endl;
}

/**
 * TODO: Implement getTrackTitles method
 * @return Vector of track titles in the playlist
 */
std::vector<std::string> DJLibraryService::getTrackTitles() const
{

    std::vector<std::string> tracks;
    const std::vector<AudioTrack *> &playlist_tracks = playlist.getTracks();
    for (AudioTrack *track : playlist_tracks)
    {
        tracks.push_back(track->get_title());
    }
    return tracks;
}
