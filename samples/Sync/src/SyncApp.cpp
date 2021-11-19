#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/Utilities.h"
#include "CinderHPV.h"

using namespace ci;
using namespace ci::app;
using namespace hpv;
using namespace std;

class SyncApp : public App 
{
public:
	void setup() override;
	void cleanup() override;
	void update() override;
	void draw() override;
private:
	struct Video
	{
		HPVPlayerRef		player;
		gl::Texture2dRef	texture;
	};
	std::list<Video>		mVideos;
};

void SyncApp::setup()
{
	HPV::InitHPVEngine();

	for ( int32_t i = 0; i < 4; ++i ) {
		Video video;
		video.player = hpv::createPlayer();
		if ( hpv::loadMovie( video.player, getAssetPath( "part" + toString( i ) + ".hpv" ) ) ) {
			video.player->setLoopMode( HPV_LOOPMODE_LOOP );
			video.texture = hpv::createTexture( video.player );
			hpv::play( video.player );
		}
		mVideos.push_back( video );
	}
}

void SyncApp::cleanup() 
{
	HPV::DestroyHPVEngine();
}

void SyncApp::update()
{
	HPV::Update();

	int64_t frame = -1;
	uint8_t i = 0;
	for ( Video& v : mVideos ) {
		if ( v.player != nullptr ) {
			int64_t f = hpv::getFrame( v.player );
			if ( frame < 0 || i == 0 ) {
				frame = f;
			} else {
				hpv::seekFrame( v.player, frame );
			}
		}
		++i;
	}
}

void SyncApp::draw()
{
	gl::clear( Color::black() ); 

	int32_t w = getWindowWidth() / 2;
	int32_t h = getWindowHeight() / 2;

	uint8_t i = 0;
	for ( const Video& v : mVideos ) {
		if ( v.texture != nullptr ) {
			int32_t x = w * ( i % 2 );
			int32_t y = ( i / 2 ) * h;
			gl::draw( v.texture, v.texture->getBounds(), Rectf( Area( x, y, x + w, y + h ) ) );
		}
		++i;
	}
}

CINDER_APP( SyncApp, RendererGl( RendererGl::Options().msaa( 0 ).version( 4, 3 ) ), []( App::Settings* settings )
{
	settings->disableFrameRate();
	settings->setWindowSize( 7680, 2160 );
	settings->setFullScreen( true );
} )
 