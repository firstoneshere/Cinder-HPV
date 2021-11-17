#pragma once

#include <filesystem>
#include "HPVRenderBridge.h"
#include "HPVPlayer.h"
#include "cinder/gl/gl.h"

namespace hpv {
    using namespace HPV;

    HPVPlayerRef            createPlayer();
    
    bool                    loadMovie( const HPVPlayerRef& player, const std::filesystem::path& path );
    
    ci::gl::Texture2dRef    createTexture( const HPVPlayerRef& player );
    ci::gl::GlslProgRef     createGlslProg( const HPVPlayerRef& player );

    void                    play( const HPVPlayerRef& player );
    void                    pause( const HPVPlayerRef& player, bool paused = true );
    void                    stop( const HPVPlayerRef& player );

    double                  calcDuration( const HPVPlayerRef& player );
    uint32_t                getFrame( const HPVPlayerRef& player );
    bool                    nextFrame( const HPVPlayerRef& player, bool sync = false );
    bool                    prevFrame( const HPVPlayerRef& player, bool sync = false );
    bool                    seekFrame( const HPVPlayerRef& player, int64_t frame, bool sync = false );
    bool                    seekPosition( const HPVPlayerRef& player, float position, bool sync = false );
    bool                    seekSeconds( const HPVPlayerRef& player, double seconds, bool sync = false );

    void                    buffer( const HPVPlayerRef& player, bool force = false );
    void                    blit( const HPVPlayerRef& player, bool force = false );
}