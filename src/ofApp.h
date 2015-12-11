#pragma once

#include "ofMain.h"
#include "ofxStk.h"
#include <math.h>

#define numVoices     8
#define numSteps      16
#define defaultTempo  90
#define boxSize       .03
#define boxSizeExtra  .07
#define gutter         40

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void whichStep();
    void initDemo();
    void initSamples();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    ofTrueTypeFont roboto;
    float boxSizeFloat;
    
    stk::FileWvIn kick;
    stk::FileWvIn snare;
    stk::FileWvIn bass;
    stk::FileWvIn clap;
    stk::FileWvIn hhat;
    stk::FileWvIn unlock;
    stk::FileWvIn vox1;
    stk::FileWvIn vox2;

    
    stk::FileWvIn samples[numVoices];
    
    Boolean tiles[numSteps][numVoices];
    Boolean demoTiles[numSteps][numVoices];
    
    float time;
    Boolean stepChanged;
    float stepSwitches[numSteps];
    
    int selectedStep;
    int currentStep;
    int localTempo;
    int voiceIndex;
    
    void audioOut(float *output,int bufferSize,int nChannels);
    
    stk::FreeVerb globalReverb;
    stk::Chorus globalChorus;
    stk::OneZero filter;

    bool chorusToggleArray[numVoices+1];
    bool chorusOn;
    bool reverbOn;
    
    ofImage texture;
    ofMesh mesh;
    ofLight	light;
    ofEasyCam cam;
    
    ofBoxPrimitive box;
    ofBoxPrimitive boxes[numSteps][numVoices];
    
    ofMaterial material;
    ofVboMesh boxSides[ofBoxPrimitive::SIDES_TOTAL];
    
    void printUsage();
    
    bool editMode;
    bool isPaused;
    bool circleMode;
    bool demo;
    int perspective;
    int mode;
};
