#include "CinderHPV.h"


namespace hpv {

    using namespace ci;
    using namespace std;

    HPVPlayerRef createPlayer()
    {
        HPVPlayerRef player = HPV::NewPlayer();
        if ( player != nullptr && RendererSingleton()->initPlayer( player->getID() ) == HPV_RET_ERROR_NONE ) {
            return player;
        }
        return nullptr;
    }

    gl::Texture2dRef createTexture( const HPVPlayerRef& player )
    {
        if ( player != nullptr ) {
            return gl::Texture2d::create(
                GL_TEXTURE_2D,
                RendererSingleton()->getTexturePtr( (GLuint)player->getID() ), 
                player->getWidth(), 
                player->getHeight(), 
                false 
            );
        }
        return nullptr;
    }

    gl::GlslProgRef createGlslProg( const HPVPlayerRef& player )
    {
        if ( player != nullptr ) {
            if ( player->getCompressionType() == HPVCompressionType::HPV_TYPE_SCALED_DXT5_CoCg_Y ) {
                static const GLchar* vert = R"(
                    #version 410

                    uniform mat4 ciModelViewProjection;

                    in vec4 ciPosition;
                    in vec4 ciColor;
                    in vec2 ciTexCoord;

                    out vec4 color;
                    out vec2 uv;

                    void main()
                    {
                        color   = ciColor;
                        uv      = ciTexCoord;

                        gl_Position = ciModelViewProjection * ciPosition;
                    }
                )";

                static const GLchar* frag = R"(
                    #version 410

                    const vec4 offsets = vec4( 0.50196078431373, 0.50196078431373, 0.0, 0.0 );
                    const float scale_factor = 255.0 / 8.0;

                    uniform sampler2D uSampler;

                    in vec4 color;                    
                    in vec2 uv;

                    out vec4 o_Color;

                    void main()
                    {
                        vec4 rgba = texture( uSampler, uv ) * color;
        
                        rgba -= offsets;
        
                        float Y = rgba.a;
                        float scale = rgba.b * scale_factor + 1;
                        float Co = rgba.r / scale;
                        float Cg = rgba.g / scale;
        
                        o_Color = vec4(Y + Co - Cg, Y + Cg, Y - Co - Cg, 1);
                    }
                )";

                return gl::GlslProg::create(
                    gl::GlslProg::Format()
                    .vertex( vert )
                    .fragment( frag )
                );
            }
            return gl::getStockShader( gl::ShaderDef().color().texture() );
        }
        return nullptr;
    }

    bool loadMovie( const HPVPlayerRef& player, const filesystem::path& path ) 
    {
        if ( player != nullptr && 
            player->open( path.string().c_str() ) == HPV_RET_ERROR_NONE && 
            RendererSingleton()->createGPUResources( player->getID()) == HPV_RET_ERROR_NONE ) {
            return true;
        }
        return false;
    }

    void play( const HPVPlayerRef& player )
    {
        buffer( player );
        player->play();
    }

    void pause( const HPVPlayerRef& player, bool paused)
    {
        if ( paused ) {
            player->pause();
            blit( player );
        } else {
            player->seek(static_cast<int64_t>( RendererSingleton()->getCPUFrameForNode( player->_id ) ) );
            buffer( player );
            player->resume();
        }
    }

    void stop( const HPVPlayerRef& player )
    {
        blit( player );
        player->stop();
    }

    double calcDuration( const HPVPlayerRef& player ) 
    {
        return ( player->getNumberOfFrames() - 1 ) / (double)player->getFrameRate();
    }

    uint32_t getFrame( const HPVPlayerRef& player )
    {
        return RendererSingleton()->getGPUFrameForNode( player->_id );
    }

    bool nextFrame( const HPVPlayerRef& player, bool sync )
    {
        const int64_t frame = getFrame( player );
        return seekFrame( player, ( frame + 1 ) > player->getLoopOut() ? player->getLoopIn() : frame, sync );
    }

    bool prevFrame( const HPVPlayerRef& player, bool sync )
    {
        const int64_t frame = getFrame( player );
        return seekFrame( player, ( frame - 1 ) > player->getLoopIn() ? player->getLoopOut() : frame, sync);
    }

    bool seekFrame( const HPVPlayerRef& player, int64_t frame, bool sync )
    {
        return player->seek( frame, sync ) == HPV_RET_ERROR_NONE;
    }

    bool seekPosition( const HPVPlayerRef& player, float position, bool sync )
    {
        float duration = (float)calcDuration( player );
        return seekSeconds( player, position * duration, sync );
    }

    bool seekSeconds( const HPVPlayerRef& player, double seconds, bool sync )
    {
        double duration = calcDuration( player );
        double position = seconds / duration;
        uint64_t frames = player->getNumberOfFrames();
        int64_t frame = (int64_t)( position * (double)frames );
        return seekFrame( player, frame, sync );
    }

    void buffer( const HPVPlayerRef& player, bool force ) 
    {
        if ( force || RendererSingleton()->needsBuffering( player->getID() ) == HPV_RET_ERROR_NONE ) {
            RendererSingleton()->setRenderState( player->getID(), HPVRenderState::STATE_BUFFER );
        }
    }

    void blit( const HPVPlayerRef& player, bool force ) 
    {
        if ( force || RendererSingleton()->needsBuffering( player->getID() ) == HPV_RET_ERROR_NONE ) {
            RendererSingleton()->setRenderState( player->getID(), HPVRenderState::STATE_BLIT );
        }
    }
}
 