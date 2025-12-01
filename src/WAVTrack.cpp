#include <cmath>
#include "WAVTrack.h"
#include <iostream>

WAVTrack::WAVTrack(const std::string &title, const std::vector<std::string> &artists,
                   int duration, int bpm, int sample_rate, int bit_depth)
    : AudioTrack(title, artists, duration, bpm), sample_rate(sample_rate), bit_depth(bit_depth)
{

    // std::cout << "WAVTrack created: " << sample_rate << "Hz/" << bit_depth << "bit" << std::endl;
}

// ========== TODO: STUDENTS IMPLEMENT THESE VIRTUAL FUNCTIONS ==========

void WAVTrack::load()
{
    std::cout << "[WAVTrack::load] Loading WAV: \"" << get_title()
              << "\" at " << sample_rate << "Hz/" << bit_depth << "bit (uncompressed)..." << std::endl;
    long size_bytes = duration_seconds * sample_rate * (bit_depth / 8) * 2;
    std::cout << " → Estimated file size: " << size_bytes << " bytes" << std::endl;
    std::cout << " → Fast loading due to uncompressed format." << std::endl;
}

void WAVTrack::analyze_beatgrid()
{
    std::cout << "[WAVTrack::analyze_beatgrid] Analyzing beat grid for: \"" << title << "\"\n";
    // TODO: Implement WAV-specific beat detection analysis
    double beats_estimated = (duration_seconds / 60.0) * bpm;
    double precision_factor = 1.0;
    std::cout << " → Estimated beats: " << beats_estimated
              << " → Compression precision factor: " << precision_factor << " (uncompressed audio)" << std::endl;
}

double WAVTrack::get_quality_score() const
{
    double score = 70.0;
    if (sample_rate >= 44100)
    {
        score += 10.0;
    }
    if (sample_rate >= 96000)
    {
        score += 5.0;
    }
    if (bit_depth >= 16)
    {
        score += 10.0;
    }
    if (bit_depth >= 24)
    {
        score += 5.0;
    }
    if (score > 100.0)
    {
        score = 100.0;
    }
    return score;
}

PointerWrapper<AudioTrack> WAVTrack::clone() const
{
    WAVTrack *clone = new WAVTrack(title, artists, duration_seconds, bpm, sample_rate, bit_depth);
    return PointerWrapper<AudioTrack>(clone);
    // return PointerWrapper<AudioTrack>(new WAVTrack(*this));
}