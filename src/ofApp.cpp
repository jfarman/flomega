#include "ofApp.h"

GLfloat lightOnePosition[] = {40.0, 40, 100.0, 0.0};
GLfloat lightOneColor[] = {0.99, 0.99, 0.99, 1.0};

GLfloat lightTwoPosition[] = {-40.0, 40, 100.0, 0.0};
GLfloat lightTwoColor[] = {0.99, 0.99, 0.99, 1.0};

//--------------------------------------------------------------
void ofApp::setup(){
    
    localTempo = defaultTempo;
    
    isPaused = false;
    demo = false;
    editMode = false;
    circleMode = false;
    chorusOn = false;
    reverbOn = false;
    stepChanged = false;
    editMode = false;

    currentStep = 0;
    voiceIndex = 0;
    selectedStep = 0;
    
    ofSetVerticalSync(true);
    ofDisableArbTex();
    texture.loadImage("fur.jpeg");
    
    // Initialize global chorus settings
    globalChorus.setEffectMix(1.0);
    globalReverb.setEffectMix(.5);
    
    // Initialize settings for models and lighting */
    ofEnableDepthTest();
    glShadeModel (GL_SMOOTH);

    glLightfv (GL_LIGHT0, GL_POSITION, lightOnePosition);
    glLightfv (GL_LIGHT0, GL_DIFFUSE, lightOneColor);
    glEnable (GL_LIGHT0);
    glLightfv (GL_LIGHT1, GL_POSITION, lightTwoPosition);
    glLightfv (GL_LIGHT1, GL_DIFFUSE, lightTwoColor);
    glEnable (GL_LIGHT1);
    glEnable (GL_LIGHTING);
    glColorMaterial (GL_FRONT_AND_BACK, GL_DIFFUSE);
    glEnable (GL_COLOR_MATERIAL);
    
    ofEnableSmoothing();
    time = ofGetElapsedTimef();
    
    boxSizeFloat = ofGetWidth() * boxSize;
    float height = ofGetHeight() * .07;
    
    for(unsigned int i = 0; i < numSteps; i++) {
        stepSwitches[i] = i+1;
        for(unsigned int j = 0; j < numVoices; j++) {
            tiles[i][j] = false;
            boxes[i][j].set(boxSizeFloat);
        }
    }
    
    printUsage();
    initDemo();
    initSamples();
    stk::Stk::setSampleRate(44100.0);
    ofSoundStreamSetup(2, 0);
}

//--------------------------------------------------------------

// whichStep: Looks at a running timer and sets the current step
// author: NICK GANG (orangagang@gmail.com)

//--------------------------------------------------------------
void ofApp::whichStep(){
    for (int i=0;i<numSteps;i++){
        float numerator = time*((float)(localTempo*4.0f)/(60.0f));
        float stepMod=(fmod(numerator,numSteps));
        
        if (stepMod < stepSwitches[i] && stepMod > stepSwitches[i-1]){
            if (currentStep == i+1){
                stepChanged = false;
            }
            else {
                currentStep = i+1;
                stepChanged = true;
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    if (!isPaused) {
        whichStep();
        time += ofGetElapsedTimef() - time;
    }
}


//--------------------------------------------------------------
void ofApp::draw(){
    
    float spinX = (!isPaused) ? sin(time *.35f) : 0;
    float spinY = (!isPaused) ? cos(time *.075f) : 0;
    
    float centerX   = ofGetWidth() * .5;
    float centerY   = ofGetHeight() * .5;
    
    float width     = ofGetWidth() * .1;
    float height    = ofGetHeight() * .07;
    
    float totalGutterX   = gutter*(float)(numSteps-1);
    float totalGutterY   = gutter*(float)(numVoices-1);
    float totalBoxW  = boxSizeFloat*(float)numSteps;
    float totalBoxH = boxSizeFloat*(float)numVoices;
    
    float startx = centerX-((totalGutterX+totalBoxW)/2);
    float starty = centerY-((totalGutterY+totalBoxH)/2);
    
    double spiralSpacing = (2 * M_PI) / numVoices;
    double circleSpacing = (2 * M_PI) / numSteps;
    
    int highlightColor =  0x06C8AA;
    int defaultColor =  0x0681AA;
    int clickedColor = 0x004056;
    
    int clickedCol = -1;
    int clickedRow = -1;
    
    int sequencerLength = boxSizeFloat * numSteps;
    int sequencerHeight = boxSizeFloat * numVoices;
    
    ofColor ocean  = ofColor::fromHex(0x0681AA);
    ofBackgroundGradient(ocean * .8, ocean * .6);
    
    int colors[] = {0xfbeaf0,0xf8d5e1,0xf5c0d2,0xf1abc3,0xee96b4,
                    0xeb81a5,0xe76c96,0xe45687,0xe14278,0xde2d69};
    
    ofSetHexColor(0xffffff);
    ofFill();
    
    ofPushStyle();
    ofPushMatrix();
    
    for(unsigned int i = 0; i < numSteps; i++) {
        int color = (i == (currentStep-1)) ? highlightColor : defaultColor;
        for(unsigned int j = 0; j < numVoices; j++) {
            float x = startx + (boxSizeFloat * (float)i);
            float y = starty + (boxSizeFloat * (float)j);
            
            bool active = (demo) ? demoTiles[i][j] : tiles[i][j];
            int actual = (active) ? clickedColor : color;
            if (active && (i==(currentStep-1)) && stepChanged) {
                samples[j].reset();
            }
            
            bool curr = (editMode && (i==selectedStep) && (j==voiceIndex)) ? true : false;
            
            ofPushStyle();
            ofPushMatrix();
            material.begin();
            texture.getTextureReference().bind();
            ofEnableAlphaBlending();
            
            if (mode == 1 || mode == 2) cam.begin();
            
            if (curr) ofSetColor(242, 196, 7, 50);
            if (active) ofSetColor(222,45,105, 50);
            if (!(active || curr)) {
                ofSetColor(255,255,255,50);
                }
                if (mode == 1){                     // abstract spiral mode
                    double radius = (i+1) * 30;
                    double offset = 10*i;
                    double currentAngle = (j * spiralSpacing) + offset;
                    double circleX = (int) (0 + radius * cos(currentAngle));
                    double circleY = (int) (0 + radius * sin(currentAngle));
                    ofTranslate(circleX,circleY,0);
                } else if (mode == 2){              // circular sequence mode
                    double radius = (ofGetHeight() - 100.0)/2.0;
                    double currentAngle = (i * circleSpacing);
                    double circleX = (int) (0+ radius * cos(currentAngle));
                    double circleY = (int) (0 + radius * sin(currentAngle));
                    double z = -((double)j*500.0);
                    ofTranslate(circleX,circleY,z);
                } else {                            // sequence view/edit mode
                    ofTranslate(x+(gutter*i),y+(gutter*j),0);
                }
            
                if (i == (currentStep-1)) boxes[i][j].set(boxSizeFloat * 2);

                boxes[i][j].rotate(spinX, 1.0, 0.0, 0.0);
                boxes[i][j].rotate(spinY, 0, 1.0, 0.0);
                boxes[i][j].draw();
                boxes[i][j].set(boxSizeFloat);
                ofDisableAlphaBlending();
                texture.getTextureReference().unbind();
                material.end();
            
                if (mode == 1 || mode == 2) cam.end();
            
            ofPopMatrix();
            ofPopStyle();
        }
    }
    ofPopMatrix();
    ofPopStyle();
}


void ofApp::audioOut(float *output, int bufferSize, int nChannels){
    if (!isPaused) {
        stk::StkFrames overall(bufferSize,2);
    
        for(int i = 0; i < numVoices; i++) {
            stk::StkFrames frames(bufferSize,2);
            samples[i].tick(frames);
            overall += frames;
        }
        // Create two sets of effects: one for the left channel and one for the right channel
        stk::StkFrames leftChannel(bufferSize,1);
        
        // Copy the left Channel of 'overall' into `leftChannel`
        overall.getChannel(0, leftChannel, 0);
        
        if (chorusOn) {
            // chrous takes a mono frame and outputs a stereo frame
            // we will only use the left channel of the chorus output
            stk::StkFrames chorusOut(bufferSize,2);
            globalChorus.tick(leftChannel, chorusOut);
            chorusOut.getChannel(0, leftChannel, 0);
        }
        if (reverbOn) {
            // reverb also takes a mono frame and outputs a stereo frame
            // we will only use the left channel of the reverb output
            stk::StkFrames reverbOut(bufferSize,2);
            globalReverb.tick(leftChannel,reverbOut,0,0);
            reverbOut.getChannel(0, leftChannel, 0);
        }
        for (int i = 0; i < bufferSize ; i++) {
            output[2*i] = leftChannel(i,0);
            output[2*i+1] = leftChannel(i,0);
        }
    }
}

//--------------------------------------------------------------
void ofApp::initSamples(){
    bass.openFile(ofToDataPath("mustard/oneshots/808s/lewiv.wav", true));
    kick.openFile(ofToDataPath("mustard/oneshots/808s/808.wav", true));
    snare.openFile(ofToDataPath("trapstep/snare-005.wav", true));
    clap.openFile(ofToDataPath("clap.wav", true));
    hhat.openFile(ofToDataPath("trapstep/closed-hat-007-trap.wav", true));
    unlock.openFile(ofToDataPath("unlock.wav", true));
    vox1.openFile(ofToDataPath("mustard/oneshots/Vox/vox1.wav", true));
    vox2.openFile(ofToDataPath("mustard/oneshots/Vox/vox2.wav", true));
    
    float normalized = 1.0f / (float)numVoices;
    
    bass.normalize(normalized);
    kick.normalize(normalized);
    snare.normalize(normalized);
    clap.normalize(normalized);
    hhat.normalize(normalized);
    unlock.normalize(normalized);
    vox1.normalize(normalized);
    vox2.normalize(normalized);
    
    samples[0] = bass;
    samples[1] = kick;
    samples[2] = snare;
    samples[3] = clap;
    samples[4] = hhat;
    samples[5] = unlock;
    samples[6] = vox1;
    samples[7] = vox2;
    
    for(unsigned int i = 0; i < numVoices; i++) {
        unsigned long sampleLength = samples[i].getSize();
        stk::StkFrames frames(sampleLength,samples[i].channelsOut());
        samples[i].tick(frames);
    }
}

//--------------------------------------------------------------
void ofApp::initDemo(){
    bool demo[numSteps][numVoices] = {
        {0, 1, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 1, 1, 1, 0, 0, 0},
        {1, 0, 1, 0, 1, 0, 0, 0},
        {0, 1, 1, 0, 1, 0, 0, 0},
        {0, 0, 1, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 1, 0, 0, 0},
        {1, 0, 0, 0, 1, 0, 0, 0},
        {0, 1, 0, 0, 1, 0, 1, 0},
        {0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 1, 1, 1, 0, 0, 0},
        {1, 0, 1, 0, 1, 0, 0, 0},
        {1, 0, 0, 0, 1, 0, 1, 0},
        {0, 0, 0, 0, 1, 0, 0, 0},
    };
    
    for(unsigned int i = 0; i < numSteps; i++) {
        for(unsigned int j = 0; j < numVoices; j++) {
            demoTiles[i][j] = demo[i][j];
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'f') {
        ofToggleFullscreen();
    }
    if (key == 'p') {
        isPaused = !isPaused;
    }
    else if (key == 'e') {
        editMode = !editMode;
    }
    else if (key == OF_KEY_UP){
        if (voiceIndex == -1) return;
        else voiceIndex--;
    } else if ( key == OF_KEY_DOWN){
        if (voiceIndex == (numVoices-1)) return;
        else voiceIndex++;
    }
    else if (key == OF_KEY_RIGHT){
        if (selectedStep == (numSteps-1)) return;
        else selectedStep++;
    } else if ( key == OF_KEY_LEFT){
        if (selectedStep == -1) return;
        else selectedStep--;
    }
    else if (key == ' ') {
        if (editMode && (voiceIndex >= 0)) {
            tiles[selectedStep][voiceIndex] = !tiles[selectedStep][voiceIndex];
        }
    }
    else if ((key == '+') || (key == '=')) {
        localTempo = localTempo + 10;
    }
    else if ((key == '-')  || (key == '_')) {
        localTempo = localTempo - 10;
    }
    else if ((key == '<')  || (key == ',') || (key == '>')  || (key == '.')) {
        chorusToggleArray[voiceIndex+1] = !chorusToggleArray[voiceIndex+1];
    }
    else if (key == 'd') {
        demo = !demo;
    }
    else if (key == 'c') {
        chorusOn = !chorusOn;
    }
    else if (key == 'r') {
        reverbOn = !reverbOn;
    }
    else if (key == 'm') {
        int modeUpdate = (mode+1)%3;
        float size = (modeUpdate == 0) ? boxSize : boxSizeExtra;
        boxSizeFloat = size * ofGetWidth();
        mode = modeUpdate;
    }
}

//--------------------------------------------------------------
void ofApp::printUsage(){
    cout << "----------------------------" << endl;
    cout << "Welcome to the // F L O M E G A // Step Sequencer" << endl;
    cout << "Jennifer Farman 2015" << endl;
    cout << "----------------------------" << endl;
    cout << "'e' - turn on the sequence editors" << endl;
    cout << "'p' - pause / unpause" << endl;
    cout << "'f' - toggle fullscreen" << endl;
    cout << "'c' - toggle chorus effect" << endl;
    cout << "'r' - toggle chorus effect" << endl;
    cout << "'f' - toggle demo on/off (demo mode initializes a demo sequence)" << endl;
    cout << "'m' - toggle mode (view/edit mode, abstract mode, circular mode)" << endl;
    cout << "' ' - toggle selected cube on/off" << endl;
    cout << "'+/-' - increase or decrease local tempo" << endl;
    cout << "left/right arrow keys' - move along the sequence editor" << endl;
    cout << "up/down arrow keys' - change current instrument in sequence editor" << endl;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
