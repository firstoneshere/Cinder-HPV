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

class PlayerApp : public App 
{
public:
	void setup() override;
	void cleanup() override;
	void update() override;
	void draw() override;
private:
	gl::GlslProgRef		mGlslProg	= nullptr;
	HPVPlayerRef		mPlayer		= nullptr;
	gl::Texture2dRef	mTexture	= nullptr;
};

void PlayerApp::setup()
{
	HPV::InitHPVEngine();

	mPlayer = hpv::createPlayer();
	if ( hpv::loadMovie( mPlayer, getAssetPath( "test_alpha.hpv" ) ) ) {
		mPlayer->setLoopMode( HPV_LOOPMODE_LOOP );
		mGlslProg = hpv::createGlslProg( mPlayer );
		mTexture = hpv::createTexture( mPlayer );
		hpv::play( mPlayer );
	}
}

void PlayerApp::cleanup() 
{
	HPV::DestroyHPVEngine();
}

void PlayerApp::update()
{
	HPV::Update();

	const double e = getElapsedSeconds();
	const double t = math<double>::sin( e * 2.0 );

	mPlayer->setSpeed( 1.0 + ( t > 0.0f ? t * 4.0 : t * 0.25 ) );
}

void PlayerApp::draw()
{
	gl::clear( Colorf( 1.0f, 0.5f, 0.0f ) ); 

	if ( mTexture && mGlslProg ) {
		const gl::ScopedGlslProg scope( mGlslProg );
		gl::draw( mTexture );
	}
}

CINDER_APP( PlayerApp, RendererGl( RendererGl::Options().msaa( 0 ).version( 4, 3 ) ), []( App::Settings* settings )
{
	settings->disableFrameRate();
	settings->setWindowSize( 1920, 1080 );
} )
 