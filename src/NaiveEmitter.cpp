/**
 * EclipseLovesCinder example application
 *
 **********************************************************
 **********************************************************
 ********************* IMPORTANT **************************
 * On first run:
 * 	- Select Project -> Clean...
 *
 *  - If change the project name, you have to tell the debugger where the new one is:
 *  	Go into Run -> Debug Configurations
 *  	Set where the application lives to something like this
 *  	Debug/{APPNAME}/Contents/MacOS/{APPNAME}
 *
 **********************************************************
 **********************************************************
 *
 * This project is released under public domain, do whatever with it.
 *
 *
 * Mario Gonzalez
 * http://onedayitwillmake
 */

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/app/MouseEvent.h"
#include "cinder/Rand.h"
#include "Resources.h"
#include <sstream>
#include <vector>
#include <gl.h>

#include "ParticleSystem.h"

class NaiveEmitter : public ci::app::AppBasic {
public:
	void setup();
	void prepareSettings( ci::app::AppBasic::Settings *settings );
	void mouseDown( ci::app::MouseEvent event );
	void update();
	void draw();
	void shutdown();

	ci::gl::Texture texture;
	std::vector<particle::ParticleSystem*> emitterList;
	size_t totalParticleCount;
};

void NaiveEmitter::prepareSettings( ci::app::AppBasic::Settings *settings ) {
	settings->setWindowSize( 800, 600 );
}

void NaiveEmitter::setup() {
	totalParticleCount = 0;

	// Test loading a texture
	ci::gl::Texture::Format format;
	format.enableMipmapping( false );
	format.setMinFilter( GL_NEAREST );
	format.setMagFilter( GL_NEAREST );
	texture = ci::gl::Texture( ci::loadImage( loadResource( RES_WHEEL ) ), format );

	for( size_t i = 0; i < 300; ++i ) {
		emitterList.push_back( new particle::ParticleSystem() );
	}
}

void NaiveEmitter::mouseDown( ci::app::MouseEvent event ) {
	for(std::vector< particle::ParticleSystem*>::iterator itr = emitterList.begin(); itr != emitterList.end(); ++itr ) {
		(*itr)->clear();
	}
}

void NaiveEmitter::update() {

	totalParticleCount = 0;
	for(std::vector<particle::ParticleSystem*>::iterator itr = emitterList.begin(); itr != emitterList.end(); ++itr ) {
		(*itr)->update();
		totalParticleCount += (*itr)->particles.size();
	}
}

void NaiveEmitter::draw() {
	// clear out the window with black
	ci::Color aColor = ci::Color( 0, 0, 0 );
	ci::gl::clear( ci::Color( 0, 0, 0 ) );

	ci::gl::enableAdditiveBlending();

	if( emitterList.size() == 0 ) return;
	particle::ParticleSystem* emitter = emitterList.at( ci::Rand::randInt(0, emitterList.size() - 1 ) );

	if ( texture ) {
		ci::gl::color( ci::ColorA(1.0f, 1.0f, 1.0f, 1.0f) );

			const float scale = ci::Rand::randFloat(0.1, 1.5);
			const float halfWidth = texture.getCleanWidth() / 2.0f * scale;
			const float halfHeight = texture.getCleanHeight() / 2.0f * scale;


			ci::Vec2f pos = getMousePos();
			const ci::Area srcArea = ci::Area( texture.getCleanBounds() );
			ci::Rectf destRect = ci::Rectf( pos.x-halfWidth, pos.y-halfHeight, pos.x + halfWidth, pos.y + halfHeight);
			const ci::Rectf srcCoords = texture.getAreaTexCoords( srcArea );


			// Add a particle to any random emitter
			emitter->add( pos, ci::Rand::randVec2f() * 1.5, srcCoords, destRect );

		texture.enableAndBind();
			glEnableClientState( GL_VERTEX_ARRAY );
			glEnableClientState( GL_TEXTURE_COORD_ARRAY );
			glEnableClientState( GL_COLOR_ARRAY );
				for(std::vector<particle::ParticleSystem*>::const_iterator itr = emitterList.begin(); itr != emitterList.end(); ++itr ) {
					glVertexPointer( 2, GL_FLOAT, 0, &((*itr)->verts)[0] );
					glTexCoordPointer( 2, GL_FLOAT, 0, &((*itr)->texCoords)[0] );
					glColorPointer( 4, GL_FLOAT, 0, &((*itr)->colors)[0].r );
					glDrawArrays( GL_TRIANGLES, 0, (*itr)->verts.size() / 2 );
				}
			glDisableClientState( GL_VERTEX_ARRAY );
			glDisableClientState( GL_TEXTURE_COORD_ARRAY );
			glDisableClientState( GL_COLOR_ARRAY );
		texture.disable();



		ci::gl::color( ci::ColorA(1.0f, 1.0f, 1.0f, 1.0f) );
		ci::gl::enableAlphaBlending();

		static ci::Font font = ci::Font( "monaco", 10.0f );
		std::stringstream ss;
		ss << totalParticleCount << std::endl;
		ci::gl::drawString( ss.str(), ci::Vec2i(5, 5), ci::ColorA(1,1,1,1), font );
	}
}


void NaiveEmitter::shutdown() {
	std::cout << "Shutdown..." << std::endl;
	AppBasic::shutdown();
}
CINDER_APP_BASIC( NaiveEmitter, ci::app::RendererGl )
