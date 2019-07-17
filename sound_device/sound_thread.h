#include <condition_variable>

std::mutex m;
std::condition_variable cv;



audioThread() 
{
    while(!isCloseRequested) 
    {
        m.lock();
        cv.wait(m);

        // wait() returns with the mutex still held.
        m.unlock();
        if(audio.dogSoundRequested) 
        {
            audio.playDogSound();
        }
    }
}


void dogSoundRequested()
{
    dogSoundRequested = true;
    cv.notify_one();
}